#include "pch.h"
#include "Events.h"
#include "AHZTarget.h"
#include "Offsets-VR.h"

namespace Events
{
    namespace
    {
        std::atomic_bool s_ahzMovieLoaded{ false };
        std::atomic_bool s_bookMenuOpen{ false };
        std::atomic_bool s_bookModeActive{ false };
        std::atomic_bool s_hudReadinessProbeArmed{ false };
        std::atomic_uint64_t s_hudGeneration{ 0 };
        std::atomic<RE::GFxMovieView*> s_hudMovie{ nullptr };
        std::atomic<RE::GFxMovieView*> s_bookHiddenMovie{ nullptr };
        std::atomic_bool s_containerWasVisibleBeforeBook{ true };
        constexpr auto   AHZ_MOVIE_LOADED_EVENT = "AHZmoreHUD_MovieLoaded"sv;
        constexpr auto   AHZ_BOTTOM_BAR_PATH = "_root.AHZWidgetContainer.AHZWidget.AHZBottomBar_mc"sv;
        constexpr auto   AHZ_CONTAINER_PATH = "_root.AHZWidgetContainer"sv;
        constexpr auto   AHZ_GOLD_VALUE_PATH = "_root.AHZWidgetContainer.AHZWidget.AHZBottomBar_mc.PlayerInfoCard_mc.PlayerGoldValue"sv;
        constexpr auto   AHZ_ICON_CONTAINER_PATH = "_global.ahz.scripts.widgets.AHZHudInfoWidget.IconContainer"sv;
        constexpr auto   AHZ_WIDGET_PATH = "_root.AHZWidgetContainer.AHZWidget"sv;

        void ApplyBottomBarCompatibility(RE::GFxMovieView* a_view)
        {
            if (!a_view) {
                logger::debug("Cannot apply bottom-bar compatibility without a HUD movie"sv);
                return;
            }

            RE::GFxValue bottomBar;
            RE::GFxValue goldValue;
            if (!a_view->GetVariable(&bottomBar, AHZ_BOTTOM_BAR_PATH.data()) || !bottomBar.IsObject() ||
                !a_view->GetVariable(&goldValue, AHZ_GOLD_VALUE_PATH.data()) || !goldValue.IsObject()) {
                logger::debug("Bottom-bar compatibility objects are not available in this AHZHudInfo.swf"sv);
                return;
            }

            if (!bottomBar.HasMember("PLAYER_CARD_WIDTH")) {
                logger::debug("AHZBottomBar_mc has no legacy PLAYER_CARD_WIDTH member; skipped native layout adjustment"sv);
                return;
            }

            RE::GFxValue authoredX;
            RE::GFxValue authoredWidth;
            if (!goldValue.GetMember("_x", &authoredX) || !authoredX.IsNumber() ||
                !goldValue.GetMember("_width", &authoredWidth) || !authoredWidth.IsNumber()) {
                logger::warn("Could not read the authored PlayerGoldValue right edge; skipped native bottom-bar adjustment"sv);
                return;
            }

            // Replacement movies may author the player card under a translated
            // parent, making its valid local right edge negative (Edge UI uses
            // -383.6). Only non-finite coordinates are invalid here.
            const double rightEdge = authoredX.GetNumber() + authoredWidth.GetNumber();
            if (!std::isfinite(rightEdge)) {
                logger::warn("Ignoring non-finite authored PlayerGoldValue right edge: {}"sv, rightEdge);
                return;
            }

            RE::GFxValue playerCardWidth{ rightEdge };
            if (bottomBar.SetMember("PLAYER_CARD_WIDTH", playerCardWidth)) {
                logger::info("Applied native bottom-bar PLAYER_CARD_WIDTH: {}"sv, rightEdge);
            } else {
                logger::warn("Could not apply the native bottom-bar PLAYER_CARD_WIDTH adjustment"sv);
            }
        }

        bool SetAHZContainerVisibility(RE::GFxMovieView* a_view, bool a_visible)
        {
            if (!a_view) {
                return false;
            }

            RE::GFxValue container;
            if (!a_view->GetVariable(&container, AHZ_CONTAINER_PATH.data()) || !container.IsObject()) {
                return false;
            }

            RE::GFxValue visibility{ a_visible };
            return container.SetMember("_visible", visibility);
        }

        bool HideAHZIconContainer(RE::GFxMovieView* a_view)
        {
            if (!a_view) {
                return false;
            }

            // moreHUD's icon clips are siblings of the vanilla rollover text,
            // not children of AHZWidgetContainer. Use the original public static
            // IconContainer contract so older and third-party movies remain valid.
            RE::GFxValue iconContainer;
            if (!a_view->GetVariable(&iconContainer, AHZ_ICON_CONTAINER_PATH.data()) || !iconContainer.IsObject()) {
                // Some derived movies expose the compatibility object through the
                // widget instance instead of leaving it only on the AS2 class.
                RE::GFxValue widget;
                if (!a_view->GetVariable(&widget, AHZ_WIDGET_PATH.data()) || !widget.IsObject() ||
                    !widget.GetMember("IconContainer", &iconContainer) || !iconContainer.IsObject()) {
                    return false;
                }
            }

            if (!iconContainer.HasMember("Hide")) {
                return false;
            }

            return iconContainer.Invoke("Hide");
        }

        bool IsBookSuppressionActive() noexcept
        {
            return s_bookMenuOpen.load(std::memory_order_acquire) || s_bookModeActive.load(std::memory_order_acquire);
        }

        bool RequestCrosshairTargetRefreshAfterBook()
        {
            const auto player = RE::PlayerCharacter::GetSingleton();
            if (!player) {
                logger::warn("Cannot refresh the rollover after BookMenu: PlayerCharacter is unavailable"sv);
                return false;
            }

            // Do not replay a captured reference through CAHZTarget here. The known lifecycle
            // hazard is that a crosshair target can become invalid outside SKSE's lookup hook.
            // IDA confirms UpdateCrosshairs (Address Library ID 40621) is Skyrim's full native
            // publisher: it resolves the current handle into an owning NiPointer, re-enters the
            // SKSE crosshair hook, and queues fresh rollover HUDData. The adjacent ID 40622 is
            // the flag-only helper; using it here would not rebuild the rollover.
            player->UpdateCrosshairs();
            return true;
        }

        void HideAHZContainerForBook(RE::GFxMovieView* a_view)
        {
            if (!a_view) {
                return;
            }

            const auto hiddenMovie = s_bookHiddenMovie.load(std::memory_order_acquire);
            if (hiddenMovie != a_view) {
                RE::GFxValue container;
                if (!a_view->GetVariable(&container, AHZ_CONTAINER_PATH.data()) || !container.IsObject()) {
                    logger::debug("Book suppression began before _root.AHZWidgetContainer was available"sv);
                    return;
                }

                RE::GFxValue visibility;
                const bool wasVisible = !container.GetMember("_visible", &visibility) || !visibility.IsBool() || visibility.GetBool();
                s_containerWasVisibleBeforeBook.store(wasVisible, std::memory_order_release);
                s_bookHiddenMovie.store(a_view, std::memory_order_release);

                logger::debug(
                    "Book suppression began in GFx movie {} (container previously visible: {})"sv,
                    static_cast<const void*>(a_view),
                    wasVisible);
            }

            // Reassert both states after every relevant HUD message/movie advance.
            // SetCrosshairTarget can recreate ahzEye after the initial BookMode push.
            SetAHZContainerVisibility(a_view, false);
            HideAHZIconContainer(a_view);
        }

        void RestoreAHZContainerAfterBook(RE::GFxMovieView* a_view)
        {
            if (!a_view || s_bookHiddenMovie.load(std::memory_order_acquire) != a_view) {
                logger::debug("Book suppression ended without a moreHUD container hidden by the DLL"sv);
                s_bookHiddenMovie.store(nullptr, std::memory_order_release);
                return;
            }

            const bool restoreVisibility = s_containerWasVisibleBeforeBook.load(std::memory_order_acquire);
            if (!SetAHZContainerVisibility(a_view, restoreVisibility)) {
                logger::debug("BookMenu closed before _root.AHZWidgetContainer was available"sv);
                s_bookHiddenMovie.store(nullptr, std::memory_order_release);
                return;
            }

            logger::debug(
                "Book suppression ended; restored moreHUD container visibility to {} in GFx movie {}"sv,
                restoreVisibility,
                static_cast<const void*>(a_view));

            s_bookHiddenMovie.store(nullptr, std::memory_order_release);

            if (restoreVisibility) {
                RE::GFxValue widget;
                if (a_view->GetVariable(&widget, AHZ_WIDGET_PATH.data()) && widget.IsObject() && widget.HasMember("RefreshWidgets")) {
                    widget.Invoke("RefreshWidgets");
                    logger::debug("Refreshed moreHUD widgets after BookMenu close"sv);
                }

                if (RequestCrosshairTargetRefreshAfterBook()) {
                    logger::debug("Requested a vanilla crosshair rollover refresh after BookMenu suppression ended"sv);
                }
            }
        }

        void ArmHUDReadinessProbe(RE::GFxMovieView* a_view, bool a_forceNewGeneration)
        {
            if (!a_view) {
                logger::warn("Cannot arm the moreHUD readiness probe without a HUD movie"sv);
                return;
            }

            const auto previousView = s_hudMovie.exchange(a_view, std::memory_order_acq_rel);
            const bool newMovie = previousView != a_view;
            if (newMovie || a_forceNewGeneration) {
                s_hudGeneration.fetch_add(1, std::memory_order_acq_rel);
                s_ahzMovieLoaded.store(false, std::memory_order_release);
            }

            if (newMovie || a_forceNewGeneration || !IsAHZMovieLoaded()) {
                s_hudReadinessProbeArmed.store(true, std::memory_order_release);
                logger::debug(
                    "Armed moreHUD readiness probe for GFx movie {} (new movie: {}, new widget: {})"sv,
                    static_cast<const void*>(a_view),
                    newMovie,
                    a_forceNewGeneration);
            }
        }

        void ProbeHUDReadiness(RE::GFxMovieView* a_view)
        {
            if (!s_hudReadinessProbeArmed.load(std::memory_order_acquire) || !a_view) {
                return;
            }

            const auto expectedView = s_hudMovie.load(std::memory_order_acquire);
            if (expectedView != a_view) {
                ArmHUDReadinessProbe(a_view, true);
            }

            RE::GFxValue widget;
            if (!a_view->GetVariable(&widget, AHZ_WIDGET_PATH.data()) || !widget.IsObject() || !widget.HasMember("updateSettings")) {
                return;
            }

            s_hudReadinessProbeArmed.store(false, std::memory_order_release);
            logger::info(
                "_root.AHZWidgetContainer.AHZWidget is ready in GFx movie {}; stopped readiness probing"sv,
                static_cast<const void*>(a_view));
            ApplyBottomBarCompatibility(a_view);
            if (s_bookMenuOpen.load(std::memory_order_acquire)) {
                HideAHZContainerForBook(a_view);
            }
            NotifyAHZMovieLoaded();
        }

        class HUDMenuHook
        {
        public:
            static void Install()
            {
                REL::Relocation<std::uintptr_t> vtable{ RE::HUDMenu::VTABLE[0] };
                if (vtable.address() == 0) {
                    logger::critical("Cannot install HUDMenu hooks: HUDMenu vtable relocation is unavailable"sv);
                    return;
                }

                const auto* entries = reinterpret_cast<const std::uintptr_t*>(vtable.address());
                if (entries[0x04] == 0 || entries[0x05] == 0) {
                    logger::critical("Cannot install HUDMenu hooks: one or more original vtable functions are unavailable"sv);
                    return;
                }

                _processMessage = vtable.write_vfunc(0x04, ProcessMessage);
                _advanceMovie = vtable.write_vfunc(0x05, AdvanceMovie);
                if (_processMessage.address() == 0 || _advanceMovie.address() == 0) {
                    logger::critical("HUDMenu hooks installed without valid original function addresses"sv);
                    return;
                }

                logger::info("Installed HUDMenu message and movie-advance hooks"sv);
            }

        private:
            static RE::UI_MESSAGE_RESULTS ProcessMessage(RE::HUDMenu* a_menu, RE::UIMessage& a_message)
            {
                if (!a_menu || _processMessage.address() == 0) {
                    logger::error("Skipping HUDMenu::ProcessMessage hook because the menu or original function is unavailable"sv);
                    return RE::UI_MESSAGE_RESULTS::kPassOn;
                }

                bool bookModeChanged = false;
                bool bookModePushed = false;

                if (a_message.type == RE::UI_MESSAGE_TYPE::kUpdate && a_message.data) {
                    const auto data = skyrim_cast<RE::HUDData*>(a_message.data);
                    if (data) {
                        if (data->type == RE::HUD_MESSAGE_TYPE::kSetMode && data->text == "BookMode") {
                            bookModeChanged = true;
                            bookModePushed = data->show;
                            s_bookModeActive.store(bookModePushed, std::memory_order_release);
                            logger::debug("Observed native BookMode {}"sv, bookModePushed ? "push"sv : "pop"sv);
                        }
                    }
                }

                const auto result = _processMessage(a_menu, a_message);
                auto* view = a_menu ? a_menu->uiMovie.get() : nullptr;
                if (IsBookSuppressionActive()) {
                    HideAHZContainerForBook(view);
                } else if (bookModeChanged && !bookModePushed) {
                    RestoreAHZContainerAfterBook(view);
                }

                return result;
            }

            static void AdvanceMovie(RE::HUDMenu* a_menu, float a_interval, std::uint32_t a_currentTime)
            {
                if (!a_menu || _advanceMovie.address() == 0) {
                    logger::error("Skipping HUDMenu::AdvanceMovie hook because the menu or original function is unavailable"sv);
                    return;
                }

                _advanceMovie(a_menu, a_interval, a_currentTime);
                auto* view = a_menu->uiMovie.get();
                ProbeHUDReadiness(view);
                if (IsBookSuppressionActive()) {
                    HideAHZContainerForBook(view);
                }
            }

            static inline REL::Relocation<decltype(ProcessMessage)> _processMessage;
            static inline REL::Relocation<decltype(AdvanceMovie)> _advanceMovie;
        };
    }

    bool MenuHandler::s_ahzMenuLoadRequested = false;

    bool IsAHZMovieLoaded() noexcept
    {
        return s_ahzMovieLoaded.load(std::memory_order_acquire);
    }

    void NotifyAHZMovieLoaded()
    {
        if (s_ahzMovieLoaded.exchange(true, std::memory_order_acq_rel)) {
            logger::debug("AHZHudInfo.swf is already marked ready"sv);
            return;
        }

        const auto generation = s_hudGeneration.load(std::memory_order_acquire);
        logger::info("AHZHudInfo.swf generation {} loaded and initialized successfully"sv, generation);

        // HUDMenu::AdvanceMovie executes in the UI movie's context. Queue the event
        // so Papyrus is notified from SKSE's task execution context instead.
        const auto taskInterface = SKSE::GetTaskInterface();
        if (!taskInterface) {
            logger::error("Unable to notify Papyrus that AHZHudInfo.swf loaded: SKSE task interface is unavailable"sv);
            return;
        }

        taskInterface->AddTask([generation]() {
            if (!IsAHZMovieLoaded() || s_hudGeneration.load(std::memory_order_acquire) != generation) {
                return;
            }

            const auto eventSource = SKSE::GetModCallbackEventSource();
            if (!eventSource) {
                logger::error("Unable to notify Papyrus that AHZHudInfo.swf loaded: mod callback event source is unavailable"sv);
                return;
            }

            SKSE::ModCallbackEvent event{
                RE::BSFixedString(AHZ_MOVIE_LOADED_EVENT),
                RE::BSFixedString(),
                0.0F,
                nullptr
            };
            eventSource->SendEvent(&event);
            logger::info("Sent {} event to Papyrus for HUD generation {}"sv, AHZ_MOVIE_LOADED_EVENT, generation);
        });
    }

    MenuHandler* MenuHandler::GetSingleton()
    {
        static MenuHandler singleton;
        return std::addressof(singleton);
    }

    void MenuHandler::Sink()
    {
        auto* ui = RE::UI::GetSingleton();
        if (!ui) {
            logger::critical("Unable to register the menu event sink: UI singleton is unavailable"sv);
            return;
        }

        ui->AddEventSink(static_cast<RE::BSTEventSink<RE::MenuOpenCloseEvent>*>(MenuHandler::GetSingleton()));
    }

    EventResult MenuHandler::ProcessEvent(RE::MenuOpenCloseEvent const* a_event, [[maybe_unused]] RE::BSTEventSource<RE::MenuOpenCloseEvent>* a_eventSource)
    {
        if (a_event == nullptr) {
            return RE::BSEventNotifyControl::kContinue;
        }
        if (REL::Module::IsVR()) {
            logger::trace("Menu: {}"sv, a_event->menuName.c_str());
            if (!a_event->opening && a_event->menuName == "WSEnemyMeters"sv) {
                s_ahzMenuLoadRequested = false;
                s_ahzMovieLoaded.store(false, std::memory_order_release);
            } else if (s_ahzMenuLoadRequested == false && a_event->menuName == "WSEnemyMeters"sv && a_event->opening) {
                auto* ui = RE::UI::GetSingleton();
                if (!ui) {
                    logger::error("The UI singleton is unavailable while opening WSEnemyMeters"sv);
                    return RE::BSEventNotifyControl::kContinue;
                }

                auto view = ui->GetMovieView(a_event->menuName);
                if (view) {
                    RE::GFxValue hudComponent;
                    RE::GFxValue result;
                    RE::GFxValue args[2];

                    RE::GFxValue _lockroot;
                    _lockroot.SetBoolean(true);
                    view->SetVariable("_lockroot", &_lockroot, RE::GFxMovie::SetVarType::kSticky);

                    args[0].SetString("AHZEnemyLevelInstance");
                    if (!view->Invoke("getNextHighestDepth", &args[1], nullptr, 0) || !args[1].IsNumber()) {
                        logger::error("moreHUD could not obtain the next WSEnemyMeters movie depth"sv);
                        return RE::BSEventNotifyControl::kContinue;
                    }

                    if (!view->Invoke("createEmptyMovieClip", &hudComponent, args, 2) || !hudComponent.IsObject()) {
                        logger::error("moreHUD could not create an empty movie clip for the WSEnemyMeters. The moreHUD enemy data will not be loaded."sv);
                        return RE::BSEventNotifyControl::kContinue;
                    }

                    args[0].SetString("AHZEnemyLevel.swf");
                    if (!hudComponent.Invoke("loadMovie", &result, &args[0], 1)) {
                        logger::error("moreHUD could not request AHZEnemyLevel.swf for WSEnemyMeters"sv);
                        return RE::BSEventNotifyControl::kContinue;
                    }

                    s_ahzMenuLoadRequested = true;
                    return RE::BSEventNotifyControl::kContinue;
                }
            }
        } else if (a_event->menuName == RE::BookMenu::MENU_NAME) {
            s_bookMenuOpen.store(a_event->opening, std::memory_order_release);
            auto* ui = RE::UI::GetSingleton();
            if (!ui) {
                logger::error("The UI singleton is unavailable while processing BookMenu state"sv);
                return RE::BSEventNotifyControl::kContinue;
            }

            const auto view = ui->GetMovieView(RE::HUDMenu::MENU_NAME);
            if (IsBookSuppressionActive()) {
                HideAHZContainerForBook(view.get());
            } else {
                RestoreAHZContainerAfterBook(view.get());
            }
            return RE::BSEventNotifyControl::kContinue;
        } else if (a_event->menuName == RE::HUDMenu::MENU_NAME) {
            if (!a_event->opening) {
                // HUDMenu's GFx movie can remain alive across an in-session save
                // load. Its moreHUD child belongs to that movie and must remain
                // attached for as long as the movie does.
                logger::debug("HUD Menu closed; preserving moreHUD container state"sv);
                return RE::BSEventNotifyControl::kContinue;
            }

            auto* ui = RE::UI::GetSingleton();
            if (!ui) {
                logger::error("The UI singleton is unavailable while opening HUDMenu"sv);
                return RE::BSEventNotifyControl::kContinue;
            }

            auto view = ui->GetMovieView(a_event->menuName);
            if (!view) {
                logger::error("The HUDMenu returned NULL. The moreHUD widgets will not be loaded."sv);
                return RE::BSEventNotifyControl::kContinue;
            }

            logger::debug("HUD Menu opened with GFx movie {}"sv, static_cast<const void*>(view.get()));

            RE::GFxValue existingContainer;
            if (view->GetVariable(&existingContainer, AHZ_CONTAINER_PATH.data()) && existingContainer.IsObject()) {
                s_ahzMenuLoadRequested = true;
                logger::info("Reusing existing _root.AHZWidgetContainer in GFx movie {}"sv, static_cast<const void*>(view.get()));
                ArmHUDReadinessProbe(view.get(), false);
                return RE::BSEventNotifyControl::kContinue;
            }

            logger::info("_root.AHZWidgetContainer is absent; injecting AHZHudInfo.swf into GFx movie {}"sv, static_cast<const void*>(view.get()));

            RE::GFxValue hudComponent;
            RE::GFxValue result;
            RE::GFxValue args[2];

            args[0].SetString("AHZWidgetContainer");
            if (!view->Invoke("_root.getNextHighestDepth", &args[1], nullptr, 0) || !args[1].IsNumber()) {
                s_ahzMenuLoadRequested = false;
                logger::error("moreHUD could not obtain the next HUDMenu movie depth"sv);
                return RE::BSEventNotifyControl::kContinue;
            }

            if (!view->Invoke("_root.createEmptyMovieClip", &hudComponent, args, 2) || !hudComponent.IsObject()) {
                s_ahzMenuLoadRequested = false;
                logger::error("moreHUD could not create an empty movie clip for the HUDMenu. The moreHUD widgets will not be loaded."sv);
                return RE::BSEventNotifyControl::kContinue;
            }

            ArmHUDReadinessProbe(view.get(), true);
            args[0].SetString("AHZHudInfo.swf");
            if (!hudComponent.Invoke("loadMovie", &result, &args[0], 1)) {
                s_ahzMenuLoadRequested = false;
                s_hudReadinessProbeArmed.store(false, std::memory_order_release);
                logger::error("moreHUD could not request AHZHudInfo.swf for the HUDMenu."sv);
                return RE::BSEventNotifyControl::kContinue;
            }

            s_ahzMenuLoadRequested = true;
            return RE::BSEventNotifyControl::kContinue;
        }
        return RE::BSEventNotifyControl::kContinue;
    }


    CrosshairHandler* CrosshairHandler::GetSingleton()
    {
        static CrosshairHandler singleton;
        return std::addressof(singleton);
    }

    void CrosshairHandler::Sink()
    {
        auto crosshair = SKSE::GetCrosshairRefEventSource();
        if (crosshair) {
            crosshair->AddEventSink(GetSingleton());
            logger::info("Registered {}"sv, typeid(SKSE::CrosshairRefEvent).name());
        }
    }

    EventResult CrosshairHandler::ProcessEvent(const SKSE::CrosshairRefEvent* a_event, RE::BSTEventSource<SKSE::CrosshairRefEvent>*)
    {
        if (!a_event) {
            logger::warn("Received a null crosshair event"sv);
            return EventResult::kContinue;
        }

        CAHZTarget::Singleton().SetTarget(a_event->crosshairRef.get());
        return EventResult::kContinue;
    }

    uintptr_t WandUpdateHook = (moreHUDSE::VR::Offsets::WandUpdateHookBase.address() + 0x7F);
    bool Hook_WandLookupREFRByHandle_Impl(RE::RefHandle& refHandle, RE::NiPointer<RE::TESObjectREFR>& refrOut)
    {
        auto result = RE::LookupReferenceByHandle(refHandle, refrOut);
        if (!result){
            CAHZTarget::Singleton().SetTarget(nullptr);
            return result;
        }

        if (!refrOut) {
            CAHZTarget::Singleton().SetTarget(nullptr);
            return result;
        }

        CAHZTarget::Singleton().SetTarget(refrOut.get());
        return result;
    }

    void Install()
    {
        if (REL::Module::IsVR()) {
            auto& trampoline = SKSE::GetTrampoline();
            trampoline.write_call<5>(
                SKSE::stl::unrestricted_cast<std::uintptr_t>(WandUpdateHook),
                SKSE::stl::unrestricted_cast<std::uintptr_t>(Hook_WandLookupREFRByHandle_Impl));
        } else {
            CrosshairHandler::Sink();
            HUDMenuHook::Install();
        }

        logger::info("registered crosshair event"sv);
        MenuHandler::Sink();
        logger::info("registered menu event"sv);
        logger::info("Installed all event sinks"sv);
    }

}
