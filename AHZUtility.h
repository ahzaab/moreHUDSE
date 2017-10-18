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
#include "skse64/PapyrusIngredient.h"
#include "skse64/PapyrusSpell.h"
#include "AHZArmorInfo.h"
#include "AHZPlayerInfo.h"
#include "AHZWeaponInfo.h"
#include "IngredientLUT.h"
#include <iostream>
#include <fstream>
#include <regex>

using namespace std;
using namespace papyrusIngredient;

float GetBaseDamage(TESAmmo* pthisAmmo);
double GetActualDamage(AHZWeaponData *weaponData);
double GetActualArmorRating(AHZArmorData* armorData);
double GetTotalActualArmorRating(void);
double GetArmorRatingDiff(TESObjectREFR *targetArmor);
double GetWeaponDamageDiff(TESObjectREFR *targetWeaponOrAmmo);
double GetTotalActualWeaponDamage(void);

bool IsTwoHanded(TESObjectWEAP * thisWeapon);
bool IsOneHanded(TESObjectWEAP * thisWeapon);
bool IsBow(TESObjectWEAP * thisWeapon);
bool IsCrossBow(TESObjectWEAP * thisWeapon);
bool isBolt(TESAmmo *thisAmmo);
double mRound(double d);
IngredientItem* GetIngredient(TESObjectREFR *thisObject);
AlchemyItem* GetFood(TESObjectREFR *thisObject);
const char * GetTargetName(TESObjectREFR *thisObject);
bool CanPickUp(UInt32 formType);

typedef  void(*GET_MAGIC_ITEM_DESCRIPTION)(void *, TESForm* a1, BSString *a2);// , SInt64 a3, const char *a4, const char *a5, unsigned int a6);
extern RelocAddr<GET_MAGIC_ITEM_DESCRIPTION> GetMagicItemDescription2;

typedef  char * (*PROCESS_SURVIVAL_MODE)(BSString *a2);// , SInt64 a3, const char *a4, const char *a5, unsigned int a6);
extern RelocAddr<PROCESS_SURVIVAL_MODE> ProcessSurvivalMode;

typedef bool(*_IsSurvivalMode)();
extern RelocAddr<_IsSurvivalMode> IsSurvivalMode;

class CAHZUtility
{
public:
	
	 string GetTargetName(TESObjectREFR *thisObject);
	 bool GetIsBookAndWasRead(TESObjectREFR *theObject);
	 string GetArmorWeightClass(TESObjectREFR *theObject);
	 string GetEffectsDescription(TESObjectREFR *theObject);
	 string GetBookSkill(TESObjectREFR *theObject);
	
	 void ProcessTargetObject(TESObjectREFR* targetObject, GFxFunctionHandler::Args *args);
	 void ProcessInventoryCount(TESObjectREFR* targetObject, GFxFunctionHandler::Args *args);
	 void ProcessTargetEffects(TESObjectREFR* targetObject, GFxFunctionHandler::Args *args);
	 void ProcessArmorClass(TESObjectREFR* targetObject, GFxFunctionHandler::Args *args);
	 void ProcessBookSkill(TESObjectREFR* targetObject, GFxFunctionHandler::Args *args);
	 bool ProcessValidTarget(TESObjectREFR* targetObject, GFxFunctionHandler::Args *args);
	 void ProcessPlayerData(GFxFunctionHandler::Args *args);
	 void ProcessIngredientData(TESObjectREFR* targetObject, GFxFunctionHandler::Args *args);
	 void ProcessValueToWeight(TESObjectREFR* targetObject, GFxFunctionHandler::Args *args);
	 string GetValueToWeight(TESObjectREFR *theObject);

private:
	 void ReplaceStringInPlace(std::string& subject, const std::string& search,
						  const std::string& replace);

	 void RegisterString(GFxValue * dst,  GFxMovieView * view, const char * name, const char * str);
	 void RegisterNumber(GFxValue * dst, const char * name, double value);

	 void SetResultString(GFxFunctionHandler::Args *args, const char * str);

	 void GetMagicItemDescription(MagicItem * item, std::string& description);

	 void AppendDescription(TESDescription *desObj, TESForm *parent, std::string& description);
};


