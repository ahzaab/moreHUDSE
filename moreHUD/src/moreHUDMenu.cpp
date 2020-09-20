#include "PCH.h"

#include "moreHUDMenu.h"

#include <cmath>
#include <cstdint>
#include "SKSE\API.h"

namespace Scaleform
{




	void HudMenu::SendNotification(RE::StaticFunctionTag*, RE::BSFixedString a_message, int32_t a_Level, float a_startPercent, float a_endPercent)
	{
		auto task = SKSE::GetTaskInterface();
		task->AddUITask([a_message, a_Level, a_startPercent, a_endPercent]() {
			enum
			{
				QUEST_UPDATE,
				SKILL_LEVEL_UPDATE,
				PLAYER_LEVEL_UPDATE,
				SHOUT_UPDATE
			};

			enum
			{
				kNotificationText,
				kStatus,
				kSoundID,
				kObjectiveCount,
				kNotificationType,
				kLevel,
				kStartPercent,
				kEndPercent,
				kDragonText
			};

			auto ui = RE::UI::GetSingleton();
			auto hud = ui->GetMenu<RE::HUDMenu>();

			if (!hud) {
				//a_vm->TraceStack("Hud menu is not open!", a_stackID, Serverity::kWarning);
				return;
			}

			auto view = hud ? hud->uiMovie : nullptr;
			auto delegate = hud ? hud->fxDelegate : nullptr;
			if (view && delegate) {
				RE::FxResponseArgsEx<9> args;
				args[kNotificationText] = a_message.c_str();
				args[kStatus] = "";
				args[kSoundID] = "UISkillIncreaseSD";
				args[kObjectiveCount] = 0;
				args[kNotificationType] = SKILL_LEVEL_UPDATE;
				args[kLevel] = a_Level;
				args[kStartPercent] = a_startPercent;
				args[kEndPercent] = a_endPercent;
				args[kDragonText] = "";

				delegate->Invoke(view.get(), "ShowNotification", args);
			}
		});
	}

	bool HudMenu::RegisterFuncs(VM* a_vm)
	{
		a_vm->RegisterFunction("SendNotification", "Skywind", SendNotification, true);

		return true;
	}
}
