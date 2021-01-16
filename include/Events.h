#pragma once

#include "SKSE\API.h"

namespace Events
{
    using EventResult = RE::BSEventNotifyControl;

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
        static CrosshairRefManager* GetSingleton();
        static void                 Register();
        RE::TESObjectREFR*          GetCrosshairReference();

    protected:
        EventResult ProcessEvent(const SKSE::CrosshairRefEvent* a_event, RE::BSTEventSource<SKSE::CrosshairRefEvent>*);

    private:
        CrosshairRefManager();
        CrosshairRefManager(const CrosshairRefManager&) = delete;
        CrosshairRefManager(CrosshairRefManager&&) = delete;
        ~CrosshairRefManager() = default;

        CrosshairRefManager& operator=(const CrosshairRefManager&) = delete;
        CrosshairRefManager& operator=(CrosshairRefManager&&) = delete;

        RE::TESObjectREFR* _cachedRef;
    };

    void Install();
}
