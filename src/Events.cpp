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

		void NotifyBookMenuState(bool a_opening)
		{
			const auto taskInterface = SKSE::GetTaskInterface();
			if (!taskInterface) {
				logger::error("Unable to notify moreHUD of Book Menu state: SKSE task interface is unavailable"sv);
				return;
			}

			taskInterface->AddUITask([a_opening]() {
				if (!IsAHZMovieLoaded()) {
					logger::debug("Skipping Book Menu {} notification because AHZHudInfo.swf is not loaded"sv,
						a_opening ? "open"sv : "close"sv);
					return;
				}

				const auto ui = RE::UI::GetSingleton();
				const auto view = ui ? ui->GetMovieView(RE::HUDMenu::MENU_NAME) : nullptr;
				if (!view) {
					logger::debug("Unable to notify moreHUD of Book Menu {}: HUD movie is unavailable"sv,
						a_opening ? "open"sv : "close"sv);
					return;
				}

				RE::GFxValue result;
				RE::GFxValue argument;
				argument.SetBoolean(a_opening);
				if (!view->Invoke("_root.AHZWidgetContainer.SetBookMenuOpen", &result, &argument, 1)) {
					logger::warn("Unable to invoke moreHUD Book Menu state handler"sv);
					return;
				}

				if (!result.IsNumber()) {
					logger::debug("moreHUD received Book Menu {} notification"sv, a_opening ? "open"sv : "close"sv);
					return;
				}

				switch (static_cast<std::int32_t>(result.GetNumber())) {
				case 1:
					logger::debug("Book Menu opened without BookMode; engaged moreHUD visibility fallback"sv);
					break;
				case 2:
					logger::debug("Book Menu closed; cleared moreHUD visibility fallback"sv);
					break;
				default:
					logger::debug("Book Menu {} with vanilla BookMode handling active"sv,
						a_opening ? "opened"sv : "closed"sv);
					break;
				}
			});
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

		if (!REL::Module::IsVR() && a_event->menuName == RE::BookMenu::MENU_NAME) {
			NotifyBookMenuState(a_event->opening);
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
