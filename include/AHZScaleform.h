#pragma once
#include <wchar.h>
#include <stdlib.h>
#include <list>
#include <algorithm>
#include "AHZArmorInfo.h"
#include "AHZPlayerInfo.h"
#include "AHZWeaponInfo.h"
#include <iostream>
#include <fstream>
#include <regex>

using namespace std;

class CAHZScaleform
{
public:
    static void     ProcessTargetObject(RE::TESObjectREFR* targetObject, RE::GFxFunctionHandler::Params& args);
    static void     ProcessTargetEffects(RE::TESObjectREFR* targetObject, RE::GFxFunctionHandler::Params& args);
    static void     ProcessArmorClass(RE::TESObjectREFR* targetObject, RE::GFxFunctionHandler::Params& args);
    static void     ProcessBookSkill(RE::TESObjectREFR* targetObject, RE::GFxFunctionHandler::Params& args);
    static void     ProcessValidTarget(RE::TESObjectREFR* targetObject, RE::GFxFunctionHandler::Params& args);
    static void     ProcessPlayerData(RE::GFxFunctionHandler::Params& args);
    static void     ProcessValueToWeight(RE::TESObjectREFR* targetObject, RE::GFxFunctionHandler::Params& args); 
    static void     ProcessEnemyInformation(RE::GFxFunctionHandler::Params& args);
    static void     ProcessIsBookAndWasRead(RE::TESObjectREFR* targetObject, RE::GFxFunctionHandler::Params& args);
    static void     ProcessIsKnownEnchantment(RE::TESObjectREFR* targetObject, RE::GFxFunctionHandler::Params& args);
    static void     ProcessArmorWarmthRating(RE::TESObjectREFR* targetObject, RE::GFxFunctionHandler::Params& args);
    static void     ProcessIsTargetInFormList(RE::TESObjectREFR* targetObject, RE::GFxFunctionHandler::Params& args);
    static void     ProcessIsTargetInIconList(RE::TESObjectREFR* targetObject, RE::GFxFunctionHandler::Params& args);

private:
    static void ReplaceStringInPlace(std::string& subject, const std::string& search,
        const std::string& replace);

    static void RegisterString(RE::GFxValue* dst, const char* name, const char* str);
    static void RegisterNumber(RE::GFxValue* dst, const char* name, double value);
    static void RegisterBoolean(RE::GFxValue* dst, const char* name, bool value);
    static void SetResultString(RE::GFxFunctionHandler::Params& args, const char* str);
    static void GetMagicItemDescription(RE::MagicItem* item, std::string& description);
    static void AppendDescription(RE::TESDescription* desObj, RE::TESForm* parent, std::string& description);
    static void FormatDescription(std::string& unFormated, std::string& formatted);

    static void     GetMagicItemDescription2(void*, RE::TESForm* a1, RE::BSString* a2);
    static char*    ProcessSurvivalMode(RE::BSString* a2);
    static bool     IsSurvivalMode();
    static float    GetArmorWarmthRating_Native(RE::TESForm* a1);
    static float    GetActorWarmthRating_Native(RE::Actor* a1, float s2);
    static bool     GetIsBookAndWasRead(RE::TESForm * theObject);
    static uint32_t GetIsKnownEnchantment(RE::TESObjectREFR* targetRef);
    static float    GetTotalWarmthRating();
    static float    GetArmorWarmthRating(RE::TESObjectREFR* targetRef);
    static string GetTargetName(RE::TESForm* thisObject);
    static string GetArmorWeightClass(RE::TESObjectREFR* theObject);
    static string GetEffectsDescription(RE::TESObjectREFR* theObject);
    static string GetBookSkill(RE::TESForm * theObject);
    static string GetValueToWeight(const char* stringFromHUD, const char* vmTranslated);
    static float  GetBaseDamage(RE::TESAmmo* pthisAmmo);
    static float  GetActualDamage(AHZWeaponData* weaponData);
    static float  GetActualArmorRating(AHZArmorData* armorData);
    static float  GetArmorWarmthRating(AHZArmorData* armorData);
    static float  GetPlayerWarmthRating();
    static float  GetTotalActualArmorRating();
    static float  GetArmorRatingDiff(RE::TESObjectREFR* targetArmor);
    static float  GetWarmthRatingDiff(RE::TESObjectREFR* thisArmor);
    static float  GetWeaponDamageDiff(RE::TESObjectREFR* targetWeaponOrAmmo);
    static float  GetTotalActualWeaponDamage();
    static void   BuildIngredientObject(RE::IngredientItem* ingredient, RE::GFxFunctionHandler::Params& args);
    static void   BuildInventoryObject(RE::TESForm* form, RE::GFxFunctionHandler::Params& arg);
    static bool   GetIsNthEffectKnown(RE::IngredientItem* thisMagic, uint32_t index);

    static bool                IsTwoHanded(RE::TESObjectWEAP* thisWeapon);
    static bool                IsOneHanded(RE::TESObjectWEAP* thisWeapon);
    static bool                IsBow(RE::TESObjectWEAP* thisWeapon);
    static bool                IsCrossBow(RE::TESObjectWEAP* thisWeapon);
    static bool                isBolt(RE::TESAmmo* thisAmmo);
    static float               mRound(float d);
    static RE::IngredientItem* GetIngredient(RE::TESForm* initialTarget);
    static RE::IngredientItem* GetIngredientFromLeveledList(RE::TESForm* thisObject);
    static RE::AlchemyItem*    GetAlchemyItem(RE::TESForm* initialTarget);
    static RE::AlchemyItem*    GetAlchemyItemFromLeveledList(RE::TESForm* thisObject);
    static RE::SpellItem*      GetSpellItem(RE::TESForm* initialTarget);
    static string              GetSoulLevelName(uint8_t soulLevel);

    // const char * GetTargetName(TESObjectREFR *thisObject);
    static bool CanPickUp(RE::TESForm* form);
};
