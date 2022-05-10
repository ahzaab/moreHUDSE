#include "pch.h"
#include "Events.h"
#include "AHZTarget.h"

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

    void Install()
    {
        CrosshairHandler::Sink();
        logger::info("registered crosshair event"sv);
        MenuHandler::Sink();
        logger::info("registered menu event"sv);
        logger::info("Installed all event sinks"sv);
    }

}
