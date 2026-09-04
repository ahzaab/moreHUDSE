#include "pch.h"
#include "Events.h"
#include "AHZTarget.h"
#include "Offsets-VR.h"

namespace Events
{
    namespace
    {
        std::atomic_bool s_ahzMovieLoaded{ false };
        std::atomic_bool s_hudReadinessProbeArmed{ false };
        std::atomic<RE::GFxMovieView*> s_hudMovie{ nullptr };
        constexpr auto   AHZ_MOVIE_LOADED_EVENT = "AHZmoreHUD_MovieLoaded"sv;
        constexpr auto   AHZ_CONTAINER_PATH = "_root.AHZWidgetContainer"sv;
        constexpr auto   AHZ_WIDGET_PATH = "_root.AHZWidgetContainer.AHZWidget"sv;

        void ArmHUDReadinessProbe(RE::GFxMovieView* a_view, bool a_forceNewGeneration)
        {
            const auto previousView = s_hudMovie.exchange(a_view, std::memory_order_acq_rel);
            const bool newMovie = previousView != a_view;
            if (newMovie || a_forceNewGeneration) {
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
            NotifyAHZMovieLoaded();
        }

        class HUDMenuAdvanceHook
        {
        public:
            static void Install()
            {
                REL::Relocation<std::uintptr_t> vtable{ RE::HUDMenu::VTABLE[0] };
                _advanceMovie = vtable.write_vfunc(0x05, AdvanceMovie);
                logger::info("Installed HUDMenu::AdvanceMovie readiness hook"sv);
            }

        private:
            static void AdvanceMovie(RE::HUDMenu* a_menu, float a_interval, std::uint32_t a_currentTime)
            {
                _advanceMovie(a_menu, a_interval, a_currentTime);
                ProbeHUDReadiness(a_menu ? a_menu->uiMovie.get() : nullptr);
            }

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
            logger::debug("AHZHudInfo.swf reported that it is already loaded"sv);
            return;
        }

        logger::info("AHZHudInfo.swf loaded and initialized successfully"sv);

        // Scaleform callbacks execute in the UI movie's context. Queue the event so
        // Papyrus is notified from SKSE's task execution context instead.
        const auto taskInterface = SKSE::GetTaskInterface();
        if (!taskInterface) {
            logger::error("Unable to notify Papyrus that AHZHudInfo.swf loaded: SKSE task interface is unavailable"sv);
            return;
        }

        taskInterface->AddTask([]() {
            if (!IsAHZMovieLoaded()) {
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
            logger::info("Sent {} event to Papyrus"sv, AHZ_MOVIE_LOADED_EVENT);
        });
    }

    MenuHandler* MenuHandler::GetSingleton()
    {
        static MenuHandler singleton;
        return std::addressof(singleton);
    }

    void MenuHandler::Sink()
    {
        auto ui = RE::UI::GetSingleton();
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
                auto view = RE::UI::GetSingleton()->GetMovieView(a_event->menuName);
                if (view) {
                    RE::GFxValue hudComponent;
                    RE::GFxValue result;
                    RE::GFxValue args[2];

                    if (!view) {
                        logger::error("The IMenu returned NULL. The moreHUD widgets will not be loaded."sv);
                    }

                    RE::GFxValue _lockroot;
                    _lockroot.SetBoolean(true);
                    view->SetVariable("_lockroot", &_lockroot, RE::GFxMovie::SetVarType::kSticky);

                    args[0].SetString("AHZEnemyLevelInstance");
                    view->Invoke("getNextHighestDepth", &args[1], nullptr, 0);
                    view->Invoke("createEmptyMovieClip", &hudComponent, args, 2);

                    if (!hudComponent.IsObject()) {
                        logger::error("moreHUD could not create an empty movie clip for the WSEnemyMeters. The moreHUD enemy data will not be loaded."sv);
                        return RE::BSEventNotifyControl::kContinue;
                    }

                    args[0].SetString("AHZEnemyLevel.swf");
                    hudComponent.Invoke("loadMovie", &result, &args[0], 1);
                    s_ahzMenuLoadRequested = true;
                    return RE::BSEventNotifyControl::kContinue;
                }
            }
        } else if (a_event->menuName == RE::HUDMenu::MENU_NAME) {
            if (!a_event->opening) {
                // HUDMenu's GFx movie can remain alive across an in-session save
                // load. Its moreHUD child belongs to that movie and must remain
                // attached for as long as the movie does.
                logger::debug("HUD Menu closed; preserving moreHUD container state"sv);
                return RE::BSEventNotifyControl::kContinue;
            }

            auto view = RE::UI::GetSingleton()->GetMovieView(a_event->menuName);
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
            view->Invoke("_root.getNextHighestDepth", &args[1], nullptr, 0);
            view->Invoke("_root.createEmptyMovieClip", &hudComponent, args, 2);

            if (!hudComponent.IsObject()) {
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
            HUDMenuAdvanceHook::Install();
        }

        logger::info("registered crosshair event"sv);
        MenuHandler::Sink();
        logger::info("registered menu event"sv);
        logger::info("Installed all event sinks"sv);
    }

}
