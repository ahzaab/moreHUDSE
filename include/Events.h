#pragma once

#include "SKSE\API.h"

namespace Events
{
    using EventResult = RE::BSEventNotifyControl;

    class MenuHandler : public RE::BSTEventSink<RE::MenuOpenCloseEvent>
    {
    public:
        static MenuHandler* GetSingleton();
        static void         Sink();
        virtual EventResult ProcessEvent(RE::MenuOpenCloseEvent const* a_event, [[maybe_unused]] RE::BSTEventSource<RE::MenuOpenCloseEvent>* a_eventSource) override;

    private:
        static bool s_ahzMenuLoaded;

    private:
        MenuHandler(){};
        MenuHandler(const MenuHandler&) = delete;
        MenuHandler(MenuHandler&&) = delete;
        ~MenuHandler() = default;

        MenuHandler& operator=(const MenuHandler&) = delete;
        MenuHandler& operator=(MenuHandler&&) = delete;
    };

#ifndef VR_BUILD
    class CrosshairHandler :
        public RE::BSTEventSink<SKSE::CrosshairRefEvent>
    {
    public:
        static CrosshairHandler* GetSingleton();
        static void              Sink();
        EventResult              ProcessEvent(const SKSE::CrosshairRefEvent* a_event, RE::BSTEventSource<SKSE::CrosshairRefEvent>*);

    private:
        CrosshairHandler(){};
        CrosshairHandler(const CrosshairHandler&) = delete;
        CrosshairHandler(CrosshairHandler&&) = delete;
        ~CrosshairHandler() = default;

        CrosshairHandler& operator=(const CrosshairHandler&) = delete;
        CrosshairHandler& operator=(CrosshairHandler&&) = delete;
    };
#endif

    void Install();
}
