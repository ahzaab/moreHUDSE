#pragma once

#include "SKSE\API.h"

namespace Events
{
    class MenuHandler : public RE::BSTEventSink<RE::MenuOpenCloseEvent>
    {
    public:
        static MenuHandler*              GetSingleton();
        static void                      Sink();
        virtual RE::BSEventNotifyControl ProcessEvent(RE::MenuOpenCloseEvent const* a_event, [[maybe_unused]] RE::BSTEventSource<RE::MenuOpenCloseEvent>* a_eventSource) override;

    private:
        static bool s_ahzMenuLoaded;
    };

    class CrosshairRefManager :
        public RE::BSTEventSink<SKSE::CrosshairRefEvent>
    {
    public:
        static inline CrosshairRefManager* GetSingleton()
        {
            static CrosshairRefManager singleton;
            return std::addressof(singleton);
        }
        static inline void Register()
        {
            auto crosshair = SKSE::GetCrosshairRefEventSource();
            if (crosshair) {
                crosshair->AddEventSink(GetSingleton());
                logger::info("Registered {}"sv, typeid(SKSE::CrosshairRefEvent).name());
            }
        }

        inline RE::TESObjectREFR* GetCrosshairReference()
        {
            return _cachedRef;
        }

    protected:
        using EventResult = RE::BSEventNotifyControl;

        inline EventResult ProcessEvent(const SKSE::CrosshairRefEvent* a_event, RE::BSTEventSource<SKSE::CrosshairRefEvent>*) override
        {
            //auto crosshairRef =
            //    a_event && a_event->crosshairRef ?
            //        a_event->crosshairRef->CreateRefHandle() :
            //        RE::ObjectRefHandle();
            //if (_cachedRef == crosshairRef) {
            //    return EventResult::kContinue;
            //}

            if (a_event && a_event->crosshairRef) {
                _cachedRef = a_event->crosshairRef.get();
            } else {
                _cachedRef = nullptr;
            }

            //_cachedRef = crosshairRef;

            return EventResult::kContinue;
        }


    private:
        CrosshairRefManager() = default;
        CrosshairRefManager(const CrosshairRefManager&) = delete;
        CrosshairRefManager(CrosshairRefManager&&) = delete;

        ~CrosshairRefManager() = default;

        CrosshairRefManager& operator=(const CrosshairRefManager&) = delete;
        CrosshairRefManager& operator=(CrosshairRefManager&&) = delete;

        RE::TESObjectREFR* _cachedRef;
    };

    inline void Install()
    {
        CrosshairRefManager::Register();
        logger::info("registered crosshair event"sv);
        MenuHandler::Sink();
        logger::info("registered menu event"sv);

        logger::info("Installed all event sinks"sv);
    }

}