#pragma once

#include "RE/Skyrim.h"


namespace Events
{
	class HitHandler : public RE::BSTEventSink<RE::TESHitEvent>
	{
	public:
		using EventResult = RE::BSEventNotifyControl;


		static HitHandler* GetSingleton();
		static void		   Sink();

		virtual EventResult ProcessEvent(const RE::TESHitEvent* a_event, RE::BSTEventSource<RE::TESHitEvent>* a_eventSource) override;

	private:
		HitHandler() = default;
		HitHandler(const HitHandler&) = delete;
		HitHandler(HitHandler&&) = delete;
		virtual ~HitHandler() = default;

		HitHandler& operator=(const HitHandler&) = delete;
		HitHandler& operator=(HitHandler&&) = delete;
	};


	class MenuAndInputHander : public RE::BSTEventSink<RE::MenuOpenCloseEvent>, public RE::BSTEventSink<RE::InputEvent*>
	{
	public:
		static MenuAndInputHander*		 GetSingleton();
		static void						 Sink();
		virtual RE::BSEventNotifyControl ProcessEvent(RE::MenuOpenCloseEvent const* a_event, [[maybe_unused]] RE::BSTEventSource<RE::MenuOpenCloseEvent>* a_eventSource) override;
		virtual RE::BSEventNotifyControl ProcessEvent(RE::InputEvent* const* a_event, [[maybe_unused]] RE::BSTEventSource<RE::InputEvent*>* a_eventSource) override;

	private:
	};

	void Install();
}
