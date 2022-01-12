#pragma once
#include <wchar.h>
#include <stdlib.h>
#include <list>
#include <algorithm>
#include "AHZArmorInfo.h"
#include "AHZPlayerInfo.h"
#include "AHZWeaponInfo.h"
#include "AHZTarget.h"
#include <iostream>
#include <fstream>
#include <regex>

using namespace std;

class CAHZScaleform
{
public:
    static void  GetMagicItemDescription2(void*, RE::TESForm * a1, RE::BSString* a2);
    static char* ProcessSurvivalMode(RE::BSString* a2);
    static bool  IsSurvivalMode();
    static float GetArmorWarmthRating_Native(RE::TESForm* a1);
    static float GetActorWarmthRating_Native(RE::Actor* a1, float s2);


    static void     ProcessTargetObject(RE::GFxFunctionHandler::Params& args);
    static void     ProcessTargetEffects(RE::GFxFunctionHandler::Params& args);
    static void     ProcessArmorClass(RE::GFxFunctionHandler::Params& args);
    static void     ProcessBookSkill(RE::GFxFunctionHandler::Params& args);
    static void     ProcessValidTarget(RE::GFxFunctionHandler::Params& args);
    static void     ProcessPlayerData(RE::GFxFunctionHandler::Params& args);
    static void     ProcessValueToWeight(RE::GFxFunctionHandler::Params& args);
    static bool     GetIsBookAndWasRead();
    static void     ProcessEnemyInformation(RE::GFxFunctionHandler::Params& args);
    static uint32_t GetIsKnownEnchantment();
    static float    GetTotalWarmthRating();
    static float    GetArmorWarmthRating();


private:
    static void ReplaceStringInPlace(std::string& subject, const std::string& search,
        const std::string& replace);

    static void RegisterString(RE::GFxValue* dst, const char* name, const char* str);
    static void RegisterNumber(RE::GFxValue* dst, const char* name, double value);
    static void RegisterBoolean(RE::GFxValue* dst, const char* name, bool value);
    static void SetResultString(RE::GFxFunctionHandler::Params& args, const char* str);
    static void GetMagicItemDescription(RE::MagicItem * item, std::string& description);
    static void AppendDescription(RE::TESDescription * desObj, RE::TESForm * parent, std::string& description);
    static void FormatDescription(std::string& unFormated, std::string& formatted);

    static string GetTargetName();
    static string GetArmorWeightClass();
    static string GetEffectsDescription();
    static string GetBookSkill();
    static string GetValueToWeight(const char* stringFromHUD, const char* vmTranslated);
    static float  GetBaseDamage(RE::TESAmmo* pthisAmmo);
    static float  GetActualDamage(AHZWeaponData* weaponData);
    static float  GetActualArmorRating(AHZArmorData* armorData);
    static float  GetArmorWarmthRating(AHZArmorData* armorData);
    static float  GetPlayerWarmthRating();
    static float  GetTotalActualArmorRating();
    static float  GetArmorRatingDiff();
    static float  GetWarmthRatingDiff();
    static float  GetWeaponDamageDiff();
    static float  GetTotalActualWeaponDamage();
    static void   BuildIngredientObject(RE::IngredientItem * ingredient, RE::GFxFunctionHandler::Params& args);
    static void   BuildInventoryObject(RE::GFxFunctionHandler::Params& arg);
    static bool   GetIsNthEffectKnown(RE::IngredientItem * thisMagic, uint32_t index);

    static bool                IsTwoHanded(RE::TESObjectWEAP * thisWeapon);
    static bool                IsOneHanded(RE::TESObjectWEAP * thisWeapon);
    static bool                IsBow(RE::TESObjectWEAP * thisWeapon);
    static bool                IsCrossBow(RE::TESObjectWEAP * thisWeapon);
    static bool                isBolt(RE::TESAmmo * thisAmmo);
    static float               mRound(float d);
    static RE::IngredientItem* GetIngredient();
    static RE::IngredientItem* GetIngredientFromLeveledList(RE::TESForm* thisObject);
    static RE::AlchemyItem*    GetAlchemyItem();
    static RE::AlchemyItem*    GetAlchemyItemFromLeveledList(RE::TESForm* thisObject);
    static RE::SpellItem*      GetSpellItem();
    static string              GetSoulLevelName(uint8_t soulLevel);

    // const char * GetTargetName(TESObjectREFR *thisObject);
    static bool CanPickUp(RE::TESForm * form);
};
