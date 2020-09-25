#include "PCH.h"
#include "Events.h"

namespace Events
{
    bool MenuHandler::s_ahzMenuLoaded = false;

    auto MenuHandler::GetSingleton() -> MenuHandler*
    {
        static MenuHandler singleton;
        return std::addressof(singleton);
    }

    void MenuHandler::Sink()
    {
        auto ui = RE::UI::GetSingleton();
        ui->AddEventSink(static_cast<RE::BSTEventSink<RE::MenuOpenCloseEvent>*>(MenuHandler::GetSingleton()));
    }

    auto MenuHandler::ProcessEvent(RE::MenuOpenCloseEvent const* a_event, [[maybe_unused]] RE::BSTEventSource<RE::MenuOpenCloseEvent>* a_eventSource) -> RE::BSEventNotifyControl
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
}