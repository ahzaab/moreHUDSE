#include "PCH.h"
#include "Papyrus.h"
#include <SpellmakingMenu.h>
#include <HudMenu.h>

namespace Skywind
{
	bool Papyrus::Register()
	{
		auto papyrus = SKSE::GetPapyrusInterface();
		if (!papyrus->Register(Scaleform::SpellmakingMenu::RegisterFuncs)) {
			return false;
		} else {
			logger::info("registered papyrus SpellmakingMenu funcs");
		}

		if (!papyrus->Register(Scaleform::HudMenu::RegisterFuncs)) {
			return false;
		} else {
			logger::info("registered papyrus hudmenu funcs");
		}

		return true;
	}
}

OnBirthSignAcceptHandler* OnBirthSignAcceptHandler::GetSingleton()
{
	static OnBirthSignAcceptHandler singleton;
	return &singleton;
}


OnBirthSignAcceptHandler::OnBirthSignAcceptHandler() :
	Base(EVN_NAME)
{}
