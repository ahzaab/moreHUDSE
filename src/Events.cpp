#include "pch.h"
#include "Events.h"
#include "AHZTarget.h"
#ifdef VR_BUILD
#include "Offsets-VR.h"
#endif

namespace Events
{
    bool MenuHandler::s_ahzMenuLoaded = false;

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
#ifdef VR_BUILD
        if (s_ahzMenuLoaded == false && a_event->menuName == "WSEnemyMeters"sv && a_event->opening) {
            auto view = RE::UI::GetSingleton()->GetMovieView(a_event->menuName);
            if (view) {
                RE::GFxValue hudComponent;
                RE::GFxValue result;
                RE::GFxValue args[2];

                if (!view) {
                    logger::error("The IMenu returned NULL. The moreHUD widgets will not be loaded."sv);
                }

                RE::GFxValue _lockroot = true;
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
                s_ahzMenuLoaded = true;
                return RE::BSEventNotifyControl::kStop;
            }
        }
#else
        if (s_ahzMenuLoaded == false && a_event->menuName == RE::HUDMenu::MENU_NAME && a_event->opening) {
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
                s_ahzMenuLoaded = true;
                return RE::BSEventNotifyControl::kStop;
            }
        }
#endif
        return RE::BSEventNotifyControl::kContinue;
    }

#ifndef VR_BUILD
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
#else
    uintptr_t           WandUpdateHook = (moreHUDSE::Offsets::WandUpdateHookBase.address() + 0x7F);
    bool Hook_WandLookupREFRByHandle_Impl(RE::RefHandle& refHandle, RE::NiPointer<RE::TESObjectREFR>& refrOut)
    {
        auto               result = RE::LookupReferenceByHandle(refHandle, refrOut);
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
#endif

    void Install()
    {
#ifdef VR_BUILD
    auto& trampoline = SKSE::GetTrampoline();
    trampoline.write_call<5>(
        SKSE::stl::unrestricted_cast<std::uintptr_t>(WandUpdateHook),
        SKSE::stl::unrestricted_cast<std::uintptr_t>(Hook_WandLookupREFRByHandle_Impl));
#else
        CrosshairHandler::Sink();
#endif
        logger::info("registered crosshair event"sv);
        MenuHandler::Sink();
        logger::info("registered menu event"sv);
        logger::info("Installed all event sinks"sv);
    }

}
