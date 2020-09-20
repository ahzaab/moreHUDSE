#include "PCH.h"
#include "statsmenu.h"
#include "statsmenuex.h"

namespace Scaleform
{
	namespace StatsMenu
	{
		constexpr REL::ID Vtbl(static_cast<std::uint64_t>(269955));
	}

	void StatsMenuHook::Hook_Accept(RE::FxDelegateHandler::CallbackProcessor* a_cbReg)
	{
		if (StatsMenuEx::GetActive()) {
			fxDelegate->callbacks.Clear();
			return;
		}

		_Accept(this, a_cbReg);
	}

	auto StatsMenuHook::Hook_ProcessMessage(RE::UIMessage& a_message)
		-> Result
	{
		if (StatsMenuEx::GetActive()) {
			return Result::kHandled;
		}

		return _ProcessMessage(this, a_message);
	}

	void StatsMenuHook::InstallHooks()
	{
		// Override to prevent the Native StatsMenu from doing anything.
		REL::Relocation<std::uintptr_t> vTable(Scaleform::StatsMenu::Vtbl);
		_Accept = vTable.write_vfunc(0x1, &StatsMenuHook::Hook_Accept);
		_ProcessMessage = vTable.write_vfunc(0x4, &StatsMenuHook::Hook_ProcessMessage);
		logger::info("Installed hooks");
	}
}
