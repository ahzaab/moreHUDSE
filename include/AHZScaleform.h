#pragma once
#include <wchar.h>
#include <stdlib.h>
#include <list>
#include <algorithm>
#include "AHZPlayerInfo.h"
#include "AHZTarget.h"
#include "AHZTargetDataCollector.h"
#include <iostream>
#include <fstream>
#include <regex>

using namespace std;

class CAHZScaleform
{
public:

    static void  ProcessTargetObject(const TargetHUDData& target, RE::GFxFunctionHandler::Params& args);
    static void  ProcessTargetEffects(const TargetData& target, RE::GFxFunctionHandler::Params& args);
    static void  ProcessArmorClass(const TargetData& target, RE::GFxFunctionHandler::Params& args);
    static void  ProcessBookSkill(const TargetData& target, RE::GFxFunctionHandler::Params& args);
    static void  ProcessValidTarget(const TargetData& target, RE::GFxFunctionHandler::Params& args);
    static void  ProcessPlayerData(RE::GFxFunctionHandler::Params& args);
    static void  ProcessValueToWeight(const TargetData& target, RE::GFxFunctionHandler::Params& args);
    static void  ProcessEnemyInformation(RE::GFxFunctionHandler::Params& args);

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
    static void   BuildIngredientObject(const TargetData& target, RE::GFxFunctionHandler::Params& args);
    static void   BuildInventoryObject(const TargetData& target, RE::GFxFunctionHandler::Params& args);
    static bool   GetIsNthEffectKnown(RE::IngredientItem* thisMagic, uint32_t index);

    static string GetSoulLevelName(uint8_t soulLevel);
};
