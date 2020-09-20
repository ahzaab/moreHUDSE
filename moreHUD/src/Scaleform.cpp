#include "PCH.h"

#include "Scaleform.h"

#include "AHZScaleform.h"


#include "SKSE/API.h"


namespace Scaleform
{

    namespace SKSE
    {
        class SKSEScaleform_InstallHooks : public RE::GFxFunctionHandler
        {
        public:
            virtual void Call(Params& a_params)
            {
            }
        };

        class SKSEScaleform_GetTargetObjectData : public RE::GFxFunctionHandler
        {
        public:
            virtual void Call(Params& a_params)
            {
                CAHZScaleform::ProcessTargetObject(CAHZPlayerInfo::GetTargetRef(), args);
            }
        };

        class SKSEScaleform_GetPlayerData : public RE::GFxFunctionHandler
        {
        public:
            virtual void Call(Params& a_params)
            {
                CAHZScaleform::ProcessPlayerData(a_params);
            }
        };

        class SKSEScaleform_GetIsPlayerInCombat : public RE::GFxFunctionHandler
        {
        public:
            virtual void Call(Params& a_params)
            {
                a_params->result->SetBool(CAHZPlayerInfo::GetIsInCombat());
            }
        };

        class SKSEScaleform_GetTargetEffects : public RE::GFxFunctionHandler
        {
        public:
            virtual void Call(Params& a_params)
            {
                CAHZScaleform::ProcessTargetEffects(CAHZPlayerInfo::GetTargetRef(), args);
            }
        };

        class SKSEScaleform_GetIsBookAndWasRead : public RE::GFxFunctionHandler
        {
        public:
            virtual void Call(Params& a_params)
            {
                TESObjectREFR* pTargetReference = CAHZPlayerInfo::GetTargetRef();

                // If the target is not valid or it can't be picked up by the player
                if (!pTargetReference) {
                    args->result->SetBool(false);
                    return;
                }
                args->result->SetBool(CAHZScaleform::GetIsBookAndWasRead(pTargetReference));
            }
        };

        class SKSEScaleform_GetArmorWeightClassString : public RE::GFxFunctionHandler
        {
        public:
            virtual void Call(Params& a_params)
            {
                CAHZScaleform::ProcessArmorClass(CAHZPlayerInfo::GetTargetRef(), args);
            }
        };

        class SKSEScaleform_GetValueToWeightString : public RE::GFxFunctionHandler
        {
        public:
            virtual void Call(Params& a_params)
            {
                CAHZScaleform::ProcessValueToWeight(CAHZPlayerInfo::GetTargetRef(), args);
            }
        };

        class SKSEScaleform_GetTargetWarmthRating : public RE::GFxFunctionHandler
        {
        public:
            virtual void Call(Params& a_params)
            {
                args->result->SetNumber(CAHZScaleform::GetArmorWarmthRating(CAHZPlayerInfo::GetTargetRef()));
            }
        };

        class SKSEScaleform_AHZLog : public RE::GFxFunctionHandler
        {
        public:
            virtual void Call(Params& a_params)
            {
#if _DEBUG
                _MESSAGE("%s", args->args[0].GetString());
#endif
            }
        };

        class SKSEScaleform_GetBookSkillString : public RE::GFxFunctionHandler
        {
        public:
            virtual void Call(Params& a_params)
            {
                CAHZScaleform::ProcessBookSkill(CAHZPlayerInfo::GetTargetRef(), args);
            }
        };

        class SKSEScaleform_GetIsValidTarget : public RE::GFxFunctionHandler
        {
        public:
            virtual void Call(Params& a_params)
            {
                CAHZScaleform::ProcessValidTarget(CAHZPlayerInfo::GetTargetRef(), args);
            }
        };

        class SKSEScaleform_GetEnemyInformation : public RE::GFxFunctionHandler
        {
        public:
            virtual void Call(Params& a_params)
            {
                CAHZScaleform::ProcessEnemyInformation(args);
            }
        };

        class SKSEScaleform_IsAKnownEnchantedItem : public RE::GFxFunctionHandler
        {
        public:
            virtual void Call(Params& a_params)
            {
                TESObjectREFR* pTargetReference = CAHZPlayerInfo::GetTargetRef();

                // If the target is not valid or it can't be picked up by the player
                if (!pTargetReference) {
                    args->result->SetNumber(0);
                    return;
                }
                args->result->SetNumber(CAHZScaleform::GetIsKnownEnchantment(pTargetReference));
            }
        };

        class SKSEScaleform_IsTargetInFormList : public RE::GFxFunctionHandler
        {
        public:
            virtual void Call(Params& a_params)
            {
                if (args &&
                    args->args &&
                    args->numArgs > 0 &&
                    args->args[0].GetType() == GFxValue::kType_String) {
                    TESObjectREFR* pTargetReference = CAHZPlayerInfo::GetTargetRef();
                    // If the target is not valid then say false
                    if (!pTargetReference) {
                        args->result->SetBool(false);
                        return;
                    }

                    auto keyName = string(args->args[0].GetString());

                    args->result->SetBool(papyrusMoreHud::HasForm(keyName, pTargetReference->baseForm->formID));
                    return;
                }

                args->result->SetBool(false);
                return;
            }
        };

        class SKSEScaleform_IsTargetInIconList : public RE::GFxFunctionHandler
        {
        public:
            virtual void Call(Params& a_params)
            {
                if (args &&
                    args->args &&
                    args->numArgs > 0 &&
                    args->args[0].GetType() == GFxValue::kType_String) {
                    auto iconName = string(args->args[0].GetString());

                    TESObjectREFR* pTargetReference = CAHZPlayerInfo::GetTargetRef();
                    // If the target is not valid then say false
                    if (!pTargetReference) {
                        args->result->SetBool(false);
                        return;
                    }

                    const char*  name = NULL;
                    TESFullName* pFullName = DYNAMIC_CAST(pTargetReference->baseForm, TESForm, TESFullName);
                    if (pFullName)
                        name = pFullName->name.data;

                    // Can't get the same for the crc
                    if (!name) {
                        args->result->SetBool(false);
                        return;
                    }

                    auto hash = (SInt32)HashUtil::CRC32(name, pTargetReference->baseForm->formID & 0x00FFFFFF);

                    auto resultIconName = string(papyrusMoreHud::GetIconName(hash));

                    if (!resultIconName.length()) {
                        args->result->SetBool(false);
                        return;
                    }

                    args->result->SetBool(resultIconName == iconName);
                    return;
                }
                args->result->SetBool(false);
            }
        };

    }

	void RegisterCallbacks()
	{



		logger::info("Registered all scaleform callbacks");
	}


	void RegisterCreators()
	{
		//BirthSignMenu::Register();
		//LevelUpMenu::Register();
		MeterMenu::Register();
		SpellmakingMenu::Register();
		StatsMenuEx::Register();

		logger::info("Registered all scaleform creators");
	}
}
