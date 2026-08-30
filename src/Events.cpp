#include "pch.h"
#include "Events.h"
#include "AHZTarget.h"
#include "Offsets-VR.h"

namespace Events
{
    namespace
    {
        std::atomic_bool s_ahzMovieLoaded{ false };
        constexpr auto   AHZ_MOVIE_LOADED_EVENT = "AHZmoreHUD_MovieLoaded"sv;
        constexpr auto   BTPS_MENU_NAME = "BTPS Menu"sv;

        using AdvanceMovie_t = void (*)(RE::IMenu*, float, std::uint32_t);
        AdvanceMovie_t s_originalBTPSAdvanceMovie = nullptr;
        double s_btpsAuthoredScale = 100.0;

        struct BTPSVisibilityState
        {
            bool active{ false };
            bool activateText{ true };
            bool grayBar{ true };
            bool infoText{ true };
            bool activateButton{ true };
        } s_btpsVisibility;

        bool GetAHZWidget(RE::GFxValue& a_widget)
        {
            const auto ui = RE::UI::GetSingleton();
            if (!ui) {
                return false;
            }

            const auto hud = ui->GetMenu(RE::HUDMenu::MENU_NAME);
            if (!hud || !hud->uiMovie) {
                return false;
            }

            hud->uiMovie->GetVariable(&a_widget, "AHZWidgetContainer.AHZWidget");
            return a_widget.IsObject();
        }

        bool GetObjectMember(const RE::GFxValue& a_parent, const char* a_name, RE::GFxValue& a_value)
        {
            return a_parent.GetMember(a_name, &a_value) && a_value.IsObject();
        }

        bool GetNumberMember(const RE::GFxValue& a_parent, const char* a_name, double& a_value)
        {
            RE::GFxValue member;
            if (!a_parent.GetMember(a_name, &member) || !member.IsNumber()) {
                return false;
            }

            a_value = member.GetNumber();
            return true;
        }

        bool GetVisibleMember(const RE::GFxValue& a_object, bool& a_visible)
        {
            RE::GFxValue member;
            if (!a_object.GetMember("_visible", &member) || !member.IsBool()) {
                return false;
            }

            a_visible = member.GetBool();
            return true;
        }

        void SetVisibleMember(RE::GFxValue& a_object, bool a_visible)
        {
            RE::GFxValue value{ a_visible };
            a_object.SetMember("_visible", value);
        }

        bool GetBTPSRolloverObjects(RE::IMenu* a_menu, RE::GFxValue& a_widget, RE::GFxValue (&a_elements)[4])
        {
            if (!a_menu || !a_menu->uiMovie) {
                return false;
            }

            a_menu->uiMovie->GetVariable(&a_widget, "mc_SelectionWidget");
            return a_widget.IsObject() &&
                   GetObjectMember(a_widget, "ActivateText", a_elements[0]) &&
                   GetObjectMember(a_widget, "GrayBar", a_elements[1]) &&
                   GetObjectMember(a_widget, "InfoText", a_elements[2]) &&
                   GetObjectMember(a_widget, "ActivateButtonInstance", a_elements[3]);
        }

        void RestoreBTPSRolloverVisibility(RE::IMenu* a_menu, bool a_release)
        {
            if (!s_btpsVisibility.active) {
                return;
            }

            RE::GFxValue widget;
            RE::GFxValue elements[4];
            if (GetBTPSRolloverObjects(a_menu, widget, elements)) {
                SetVisibleMember(elements[0], s_btpsVisibility.activateText);
                SetVisibleMember(elements[1], s_btpsVisibility.grayBar);
                SetVisibleMember(elements[2], s_btpsVisibility.infoText);
                SetVisibleMember(elements[3], s_btpsVisibility.activateButton);
            }

            if (a_release) {
                s_btpsVisibility = {};
            }
        }

        void CaptureAndHideBTPSRollover(RE::IMenu* a_menu)
        {
            RE::GFxValue widget;
            RE::GFxValue elements[4];
            if (!GetBTPSRolloverObjects(a_menu, widget, elements)) {
                return;
            }

            GetVisibleMember(elements[0], s_btpsVisibility.activateText);
            GetVisibleMember(elements[1], s_btpsVisibility.grayBar);
            GetVisibleMember(elements[2], s_btpsVisibility.infoText);
            GetVisibleMember(elements[3], s_btpsVisibility.activateButton);
            s_btpsVisibility.active = true;

            for (auto& element : elements) {
                SetVisibleMember(element, false);
            }
        }

        bool SyncBTPSRolloverLayout(RE::IMenu* a_menu)
        {
            RE::GFxValue widget;
            RE::GFxValue elements[4];
            if (!GetBTPSRolloverObjects(a_menu, widget, elements)) {
                return false;
            }

            const auto rect = a_menu->uiMovie->GetVisibleFrameRect();
            const double width = rect.right - rect.left;
            const double height = rect.bottom - rect.top;
            if (width <= 0.0 || height <= 0.0) {
                return false;
            }

            double x;
            double y;
            double scale;
            double alpha;
            bool widgetVisible;
            if (!GetNumberMember(widget, "_x", x) ||
                !GetNumberMember(widget, "_y", y) ||
                !GetNumberMember(widget, "_xscale", scale) ||
                !GetNumberMember(elements[0], "_alpha", alpha) ||
                !GetVisibleMember(widget, widgetVisible)) {
                return false;
            }

            RE::GFxValue ahzWidget;
            if (!GetAHZWidget(ahzWidget)) {
                return false;
            }

            RE::GFxValue version;
            if (!ahzWidget.Invoke("GetExternalRolloverLayoutVersion", &version) || !version.IsNumber() || version.GetNumber() < 1.0) {
                return false;
            }

            const bool visible = a_menu->uiMovie->GetVisible() && widgetVisible && alpha > 0.0;
            RE::GFxValue args[5]{
                (x - rect.left) / width,
                (y - rect.top) / height,
                scale / s_btpsAuthoredScale,
                alpha / 100.0,
                visible
            };
            RE::GFxValue result;
            return ahzWidget.Invoke("SetExternalRolloverLayout", &result, args, 5) && result.IsBool() && result.GetBool();
        }

        void ClearBTPSRolloverLayout(RE::IMenu* a_menu = nullptr)
        {
            RE::GPtr<RE::IMenu> menu;
            if (!a_menu) {
                const auto ui = RE::UI::GetSingleton();
                menu = ui ? ui->GetMenu(BTPS_MENU_NAME) : nullptr;
                a_menu = menu.get();
            }

            RestoreBTPSRolloverVisibility(a_menu, true);

            RE::GFxValue widget;
            if (GetAHZWidget(widget)) {
                widget.Invoke("ClearExternalRolloverLayout");
            }
        }

        void BTPSAdvanceMovieHook(RE::IMenu* a_menu, float a_interval, std::uint32_t a_currentTime)
        {
            RestoreBTPSRolloverVisibility(a_menu, false);

            // Preserve a clean moreHUD baseline before BTPS's legacy compatibility
            // code changes content and icon transforms. The post-pass restores
            // that baseline and applies the final BTPS transform for this frame.
            SyncBTPSRolloverLayout(a_menu);
            s_originalBTPSAdvanceMovie(a_menu, a_interval, a_currentTime);

            if (SyncBTPSRolloverLayout(a_menu)) {
                CaptureAndHideBTPSRollover(a_menu);
            } else {
                RestoreBTPSRolloverVisibility(a_menu, true);
            }
        }

        bool InstallBTPSAdvanceMovieHook(RE::IMenu* a_menu)
        {
            if (s_originalBTPSAdvanceMovie) {
                return true;
            }
            if (!a_menu) {
                return false;
            }

            constexpr std::size_t advanceMovieIndex = 0x05;
            const auto vtable = *reinterpret_cast<std::uintptr_t**>(a_menu);
            const auto originalAddress = vtable[advanceMovieIndex];

            HMODULE owner = nullptr;
            const auto flags = GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT;
            if (!GetModuleHandleExW(flags, reinterpret_cast<LPCWSTR>(originalAddress), &owner) ||
                owner != GetModuleHandleW(L"BetterThirdPersonSelection.dll")) {
                logger::warn("BTPS Menu AdvanceMovie is not owned by BetterThirdPersonSelection.dll; runtime integration was not installed"sv);
                return false;
            }

            RE::GFxValue widget;
            RE::GFxValue elements[4];
            double authoredScale;
            if (GetBTPSRolloverObjects(a_menu, widget, elements) &&
                GetNumberMember(widget, "_xscale", authoredScale) && authoredScale > 0.0) {
                s_btpsAuthoredScale = authoredScale;
            }

            REL::Relocation<std::uintptr_t> vtableRelocation{ reinterpret_cast<std::uintptr_t>(vtable) };
            const auto replacedAddress = vtableRelocation.write_vfunc(advanceMovieIndex, BTPSAdvanceMovieHook);
            s_originalBTPSAdvanceMovie = reinterpret_cast<AdvanceMovie_t>(replacedAddress);
            logger::info("Installed moreHUD's BTPS Menu post-update integration"sv);
            return true;
        }
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
                        return RE::BSEventNotifyControl::kStop;
                    }

                    args[0].SetString("AHZEnemyLevel.swf");
                    hudComponent.Invoke("loadMovie", &result, &args[0], 1);
                    s_ahzMenuLoadRequested = true;
                    return RE::BSEventNotifyControl::kStop;
                }
            }
        } else {
            if (!a_event->opening && a_event->menuName == RE::HUDMenu::MENU_NAME) {
                ClearBTPSRolloverLayout();
                s_ahzMenuLoadRequested = false;
                s_ahzMovieLoaded.store(false, std::memory_order_release);
                logger::debug("HUD Menu closed; cleared moreHUD movie state"sv);
            } else if (s_ahzMenuLoadRequested == false && a_event->menuName == RE::HUDMenu::MENU_NAME && a_event->opening) {
                auto view = RE::UI::GetSingleton()->GetMovieView(a_event->menuName);

                if (view) {
                    RE::GFxValue hudComponent;
                    RE::GFxValue result;
                    RE::GFxValue args[2];

                    if (!view) {
                        logger::error("The IMenu returned NULL. The moreHUD widgets will not be loaded."sv);
                    }

                    args[0].SetString("AHZWidgetContainer");
                    view->Invoke("_root.getNextHighestDepth", &args[1], nullptr, 0);
                    view->Invoke("_root.createEmptyMovieClip", &hudComponent, args, 2);

                    if (!hudComponent.IsObject()) {
                        logger::error("moreHUD could not create an empty movie clip for the HUDMenu. The moreHUD widgets will not be loaded."sv);
                        return RE::BSEventNotifyControl::kStop;
                    }

                    args[0].SetString("AHZHudInfo.swf");
                    hudComponent.Invoke("loadMovie", &result, &args[0], 1);
                    s_ahzMenuLoadRequested = true;
                    return RE::BSEventNotifyControl::kStop;
                }
            }

            if (!a_event->opening && a_event->menuName == BTPS_MENU_NAME) {
                ClearBTPSRolloverLayout();
                logger::debug("BTPS Menu closed; cleared external rollover layout"sv);
            } else if (a_event->opening && a_event->menuName == BTPS_MENU_NAME) {
                const auto menu = RE::UI::GetSingleton()->GetMenu(a_event->menuName);
                InstallBTPSAdvanceMovieHook(menu.get());
            }
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
        }

        logger::info("registered crosshair event"sv);
        MenuHandler::Sink();
        logger::info("registered menu event"sv);
        logger::info("Installed all event sinks"sv);
    }

}
