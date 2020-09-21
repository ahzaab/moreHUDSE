#pragma once

#include "SKSE\API.h"

namespace Events
{
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
            _cachedRef.get();
        }

    protected:
        using EventResult = RE::BSEventNotifyControl;

        inline EventResult ProcessEvent(const SKSE::CrosshairRefEvent* a_event, RE::BSTEventSource<SKSE::CrosshairRefEvent>*) override
        {
            auto crosshairRef =
                a_event && a_event->crosshairRef ?
                    a_event->crosshairRef->CreateRefHandle() :
                    RE::ObjectRefHandle();
            if (_cachedRef == crosshairRef) {
                return EventResult::kContinue;
            }

            _cachedRef = crosshairRef;

            return EventResult::kContinue;
        }


    private:
        CrosshairRefManager() = default;
        CrosshairRefManager(const CrosshairRefManager&) = delete;
        CrosshairRefManager(CrosshairRefManager&&) = delete;

        ~CrosshairRefManager() = default;

        CrosshairRefManager& operator=(const CrosshairRefManager&) = delete;
        CrosshairRefManager& operator=(CrosshairRefManager&&) = delete;

        RE::ObjectRefHandle _cachedRef;
    };

    inline void Install()
    {
        CrosshairRefManager::Register();
        logger::info("registered crosshair event"sv);
    }

}
