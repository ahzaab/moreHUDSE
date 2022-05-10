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
    static bool IsSurvivalMode();

    static void  ProcessTargetObject(const TargetData& target, RE::GFxFunctionHandler::Params& args);
    static void  ProcessTargetEffects(const TargetData& target, RE::GFxFunctionHandler::Params& args);
    static void  ProcessArmorClass(const TargetData& target, RE::GFxFunctionHandler::Params& args);
    static void  ProcessBookSkill(const TargetData& target, RE::GFxFunctionHandler::Params& args);
    static void  ProcessValidTarget(const TargetData& target, RE::GFxFunctionHandler::Params& args);
    static void  ProcessPlayerData(RE::GFxFunctionHandler::Params& args);
    static void  ProcessValueToWeight(const TargetData& target, RE::GFxFunctionHandler::Params& args);
    static void  ProcessEnemyInformation(RE::GFxFunctionHandler::Params& args);
    static float GetTotalWarmthRating();


private:
    static void ReplaceStringInPlace(std::string& subject, const std::string& search,
        const std::string& replace);

    static void RegisterString(RE::GFxValue* dst, const char* name, const char* str);
    static void RegisterNumber(RE::GFxValue* dst, const char* name, double value);
    static void RegisterBoolean(RE::GFxValue* dst, const char* name, bool value);
    static void SetResultString(RE::GFxFunctionHandler::Params& args, const char* str);
    static void FormatDescription(const std::string unFormated, std::string& formatted);

    static string GetArmorWeightClass(const TargetData& target);
    static string GetBookSkill(const TargetData& target);
    static string GetValueToWeight(const char* stringFromHUD, const char* vmTranslated);
    static float  GetActualDamage(AHZWeaponData* weaponData);
    static float  GetActualArmorRating(AHZArmorData* armorData);
    static float  GetArmorWarmthRating(AHZArmorData* armorData);
    static float  GetPlayerWarmthRating();
    static float  GetTotalActualArmorRating();
    static float  GetArmorRatingDiff(const TargetData& target);
    static float  GetWarmthRatingDiff(const TargetData& target);
    static float  GetWeaponDamageDiff(const TargetData& target);
    static float  GetTotalActualWeaponDamage();
    static void   BuildIngredientObject(const TargetData& target, RE::GFxFunctionHandler::Params& args);
    static void   BuildInventoryObject(const TargetData& target, RE::GFxFunctionHandler::Params& args);
    static bool   GetIsNthEffectKnown(RE::IngredientItem* thisMagic, uint32_t index);

    static bool   IsTwoHanded(RE::TESObjectWEAP* thisWeapon);
    static bool   IsOneHanded(RE::TESObjectWEAP* thisWeapon);
    static bool   IsBow(RE::TESObjectWEAP* thisWeapon);
    static bool   IsBow(const TargetData& thisWeapon);
    static bool   IsCrossBow(RE::TESObjectWEAP* thisWeapon);
    static bool   IsCrossBow(const TargetData& thisWeapon);
    static bool   isBolt(RE::TESAmmo* thisAmmo);
    static float  mRound(float d);
    static string GetSoulLevelName(uint8_t soulLevel);
};
