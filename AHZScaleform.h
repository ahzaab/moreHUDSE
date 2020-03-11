#pragma once
#include <wchar.h>
#include <stdlib.h>
#include <list>
#include <algorithm>
#include "skse64/GameReferences.h"
#include "skse64/GameObjects.h"
#include "skse64/GameData.h"
#include "skse64/GameRTTI.h"
#include "skse64/GameSettings.h"
#include "skse64/GameExtraData.h"
#include "skse64/ScaleformCallbacks.h"
#include "skse64/ScaleformMovie.h"
#include "skse64/ScaleformLoader.h"
#include "skse64/PapyrusIngredient.h"
#include "skse64/PapyrusSpell.h"
#include "AHZArmorInfo.h"
#include "AHZPlayerInfo.h"
#include "AHZWeaponInfo.h"
#include <iostream>
#include <fstream>
#include <regex>

using namespace std;
using namespace papyrusIngredient;

typedef  void(*GET_MAGIC_ITEM_DESCRIPTION)(void *, TESForm* a1, BSString *a2);// , SInt64 a3, const char *a4, const char *a5, unsigned int a6);
extern RelocAddr<GET_MAGIC_ITEM_DESCRIPTION> GetMagicItemDescription2;

typedef  char * (*PROCESS_SURVIVAL_MODE)(BSString *a2);// , SInt64 a3, const char *a4, const char *a5, unsigned int a6);
extern RelocAddr<PROCESS_SURVIVAL_MODE> ProcessSurvivalMode;

typedef bool(*_IsSurvivalMode)();
extern RelocAddr<_IsSurvivalMode> IsSurvivalMode;

typedef float(*GET_ARMOR_WARMTH_RATING)(TESForm* a1);
extern RelocAddr<GET_ARMOR_WARMTH_RATING> GetArmorWarmthRating_Native;

typedef float(*GET_ACTOR_WARMTH_RATING)(Actor* a1, float s2);
extern RelocAddr<GET_ACTOR_WARMTH_RATING> GetActorWarmthRating_Native;

//3BD850

class CAHZScaleform
{
public:
   static void ProcessTargetObject(TESObjectREFR* targetObject, GFxFunctionHandler::Args *args);
   static void ProcessTargetEffects(TESObjectREFR* targetObject, GFxFunctionHandler::Args *args);
   static void ProcessArmorClass(TESObjectREFR* targetObject, GFxFunctionHandler::Args *args);
   static void ProcessBookSkill(TESObjectREFR* targetObject, GFxFunctionHandler::Args *args);
   static void ProcessValidTarget(TESObjectREFR* targetObject, GFxFunctionHandler::Args *args);
   static void ProcessPlayerData(GFxFunctionHandler::Args *args);
   static void ProcessValueToWeight(TESObjectREFR* targetObject, GFxFunctionHandler::Args *args);
   static bool GetIsBookAndWasRead(TESObjectREFR *theObject);
   static void ProcessEnemyInformation(GFxFunctionHandler::Args *args);
   static UInt32 GetIsKnownEnchantment(TESObjectREFR *targetRef);
   static double GetTotalWarmthRating(void);
   static double GetArmorWarmthRating(TESObjectREFR* targetRef);


private:
   static void ReplaceStringInPlace(std::string& subject, const std::string& search,
      const std::string& replace);

   static void RegisterString(GFxValue * dst, GFxMovieView * view, const char * name, const char * str);
   static void RegisterNumber(GFxValue * dst, const char * name, double value);
   static void RegisterBoolean(GFxValue * dst, const char * name, bool value);
   static void SetResultString(GFxFunctionHandler::Args *args, const char * str);
   static void GetMagicItemDescription(MagicItem * item, std::string& description);
   static void AppendDescription(TESDescription *desObj, TESForm *parent, std::string& description);
   static void FormatDescription(std::string& unFormated, std::string& formatted);

   static string GetTargetName(TESForm *thisObject);
   static string GetArmorWeightClass(TESObjectREFR *theObject);
   static string GetEffectsDescription(TESObjectREFR *theObject);
   static string GetBookSkill(TESObjectREFR *theObject);
   static string GetValueToWeight(TESObjectREFR *theObject, const char * stringFromHUD, const char * vmTranslated);
   static float GetBaseDamage(TESAmmo* pthisAmmo);
   static double GetActualDamage(AHZWeaponData *weaponData);
   static double GetActualArmorRating(AHZArmorData* armorData);
   static double GetArmorWarmthRating(AHZArmorData* armorData);
   static double GetPlayerWarmthRating(void);
   static double GetTotalActualArmorRating(void);
   static double GetArmorRatingDiff(TESObjectREFR *targetArmor);
   static double GetWarmthRatingDiff(TESObjectREFR *thisArmor);
   static double GetWeaponDamageDiff(TESObjectREFR *targetWeaponOrAmmo);
   static double GetTotalActualWeaponDamage(void);
   static void BuildIngredientObject(IngredientItem* ingredient, GFxFunctionHandler::Args *args);
   static void BuildInventoryObject(TESForm* form, GFxFunctionHandler::Args *args);

   static bool IsTwoHanded(TESObjectWEAP * thisWeapon);
   static bool IsOneHanded(TESObjectWEAP * thisWeapon);
   static  bool IsBow(TESObjectWEAP * thisWeapon);
   static bool IsCrossBow(TESObjectWEAP * thisWeapon);
   static bool isBolt(TESAmmo *thisAmmo);
   static double mRound(double d);
   static IngredientItem* GetIngredient(TESForm *initialTarget);
   static IngredientItem* GetIngredientFromLeveledList(TESForm *thisObject);
   static AlchemyItem* GetAlchemyItem(TESForm *initialTarget);
   static AlchemyItem * GetAlchemyItemFromLeveledList(TESForm *thisObject);
   static SpellItem* GetSpellItem(TESForm *initialTarget);
   static string GetSoulLevelName(UInt8 soulLevel);

   // const char * GetTargetName(TESObjectREFR *thisObject);
   static bool CanPickUp(TESForm * form);
};
