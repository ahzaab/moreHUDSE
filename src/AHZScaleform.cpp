#include "pch.h"

#include <cwchar>
#include <cstdlib>
#include <sstream>
#include <iostream>
#include <list>
#include <algorithm>
#include "AHZScaleform.h"
#include "AHZPlayerInfo.h"
#include "AHZScaleformHook.h"
#include "AHZUtilities.h"

static auto DoubleCompare(double a, double b) -> bool
{
    double delta = fabs(a - b);
    if (delta < std::numeric_limits<double>::epsilon() &&
        delta > -std::numeric_limits<double>::epsilon()) {
        return true;
    }
    return false;
}

static auto GetPct(double current, double max) -> double
{
    double percent = -1;

    if (max < 0) {
        return percent;
    }

    if (!DoubleCompare(max, 0.0)) {
        percent = ceil((current / max) * 100.0);
        return fmin(100, fmax(percent, -1));  // negative indicates that the actor value is not used
    }

    return percent;
}

void CAHZScaleform::ProcessEnemyInformation(RE::GFxFunctionHandler::Params& args)
{
    const auto actorData = AHZEnemyHealthUpdateHook::GetCurrentEnemyData();
    const auto playerLevel = actorData.playerLevel;
    const auto& soulName = actorData.soulName;

    if (!REL::Module::IsVR()) {
        RE::GFxValue enemyObj;
        RE::GFxValue playerObj;
        args.movie->CreateObject(&enemyObj);
        args.movie->CreateObject(&playerObj);

        RegisterNumber(&enemyObj, "level", actorData.Level);
        RegisterNumber(&playerObj, "level", playerLevel);
        if (!soulName.empty()) {
            RegisterString(&enemyObj, "soul", soulName.c_str());
        }
        RegisterNumber(&enemyObj, "maxHealth", ceil(actorData.maxHealth));
        RegisterNumber(&enemyObj, "health", ceil(actorData.health));
        RegisterNumber(&enemyObj, "healthPct", GetPct(actorData.health, actorData.maxHealth));
        RegisterNumber(&enemyObj, "maxMagicka", ceil(actorData.maxMagicka));
        RegisterNumber(&enemyObj, "magicka", ceil(actorData.magicka));
        RegisterNumber(&enemyObj, "magickaPct", GetPct(actorData.magicka, actorData.maxMagicka));
        RegisterNumber(&enemyObj, "maxStamina", ceil(actorData.maxStamina));
        RegisterNumber(&enemyObj, "stamina", ceil(actorData.stamina));
        RegisterNumber(&enemyObj, "staminaPct", GetPct(actorData.stamina, actorData.maxStamina));
        RegisterBoolean(&enemyObj, "targetChanged", actorData.targetChanged);

        if (args.args[0].HasMember("player")) {
            args.args[0].SetMember("player", playerObj);
        }
        if (args.args[0].HasMember("enemy")) {
            args.args[0].SetMember("enemy", enemyObj);
        }
    } else {
        RE::GFxValue legacyObj;
        args.movie->CreateObject(&legacyObj);
        if (actorData.Level) {
            RegisterNumber(&legacyObj, "EnemyLevel", actorData.Level);
            RegisterNumber(&legacyObj, "PlayerLevel", playerLevel);
            if (!soulName.empty()) {
                RegisterString(&legacyObj, "Soul", soulName.c_str());
            }
        }
        if (args.args[0].HasMember("outObj")) {
            args.args[0].SetMember("outObj", legacyObj);
        }
    }
}

auto CAHZScaleform::GetValueToWeight(const char* stringFromHUD, const char* vmTranslated) -> string
{
    string desc;

    if (!stringFromHUD)
        return desc;

    //<TEXTFORMAT INDENT="0" LEFTMARGIN="0" RIGHTMARGIN="0" LEADING="2"><P ALIGN="CENTER"><FONT FACE="$EverywhereMediumFont" SIZE="15" COLOR="#999999" KERNING="0">WEIGHT </FONT><FONT FACE="$EverywhereBoldFont" SIZE="24" COLOR="#FFFFFF" KERNING="0">0.5</FONT><FONT FACE="$EverywhereMediumFont" SIZE="15" COLOR="#999999" KERNING="0">      VALUE </FONT><FONT FACE="$EverywhereBoldFont" SIZE="24" COLOR="#FFFFFF" KERNING="0">21</FONT></P></TEXTFORMAT>

    // Using regex from the HUD string to extract the value and weight values.  The SKSE version are either broken or unreliable
    std::regex     rgx(R"(\s+([0-9]*\.?[0-9]+))");
    std::smatch    match;
    string         s = stringFromHUD;
    const string   cs = const_cast<string&>(s);
    vector<string> parts;

    while (regex_search(s, match, rgx)) {
        if (!match.size()) {
            return desc;
        }

        parts.push_back(match[0]);

        s = match.suffix();
    }

    if (parts.size() < 2) {
        return desc;
    }

    // The fixed positions of the matches (containing groups)
    string weight = parts[parts.size() - 2];
    string value = parts[parts.size() - 1];
    char*  end;

    float weightValue = strtof(weight.c_str(), &end);
    float valueValue = strtof(value.c_str(), &end);

    // Don't show a neg or 0 ratio, its pointless
    if (weightValue <= 0.0 || valueValue <= 0.0) {
        return desc;
    }

    float vW = valueValue / weightValue;

    // Add the VW label
    desc.append(R"(<FONT FACE="$EverywhereMediumFont"SIZE="15"COLOR="#999999"KERNING="0">     )");
    desc.append(vmTranslated);
    desc.append(" <\\FONT>");

    char   floatHold[64];
    size_t size = 64;

    //Rounding trick
    sprintf_s(floatHold, size, "%.2f", vW);
    vW = strtof(floatHold, &end);

    if (vW < 1.0) {
        sprintf_s(floatHold, size, "%.1g", vW);
    } else {
        sprintf_s(floatHold, size, "%.0f", vW);
    }

    desc.append(R"(<FONT FACE="$EverywhereBoldFont"SIZE="24"COLOR="#FFFFFF"KERNING="0">)");
    desc.append(floatHold);
    desc.append("<\\FONT>");

    return desc;
};

void CAHZScaleform::ProcessTargetEffects(const TargetData& target, RE::GFxFunctionHandler::Params& args)
{
    string name;

    bool calculateInventory = args.args[1].GetBool();

    if (!calculateInventory) {
        args.args[0].DeleteMember("inventoryObj");
    }

    // No valid reference
    if (!target.isValid) {
        args.args[0].DeleteMember("effectsObj");
        args.args[0].DeleteMember("ingredientObj");
        args.args[0].DeleteMember("inventoryObj");
        return;
    }

    // See if its an ingredient.  Note they are formated differently with known effects;
    if (target.formType == RE::FormType::Ingredient) {
        args.args[0].DeleteMember("effectsObj");
        BuildIngredientObject(target, args);

        if (calculateInventory) {
            BuildInventoryObject(target, args);
        }

        return;
    }

    if (!target.description.empty()) {
        FormatDescription(target.description, name);
    } else if (!target.effectsDescription.empty()) {
        name = target.effectsDescription;
    }

    if (calculateInventory) {
        BuildInventoryObject(target, args);
    }

    // If the name contains a string
    if (name.length()) {
        RE::GFxValue obj;
        args.movie->CreateObject(&obj);

        name = CAHZUtilities::trim(name);

        RegisterString(&obj, "effectsDescription", name.c_str());

        // Add the object to the scaleform function
        args.args[0].SetMember("effectsObj", obj);
    } else {
        args.args[0].DeleteMember("effectsObj");
    }
}

void CAHZScaleform::ProcessArmorClass(const TargetData& target, RE::GFxFunctionHandler::Params& args)
{
    // If the target is not valid or it can't be picked up by the player
    if (!target.isValid) {
        SetResultString(args, "");
        return;
    }

    SetResultString(args, target.armorWeightClass.c_str());
};

void CAHZScaleform::ProcessValueToWeight(const TargetData& target, RE::GFxFunctionHandler::Params& args)
{
    // If the target is not valid or it can't be picked up by the player
    if (!target.isValid) {
        SetResultString(args, "");
        return;
    }

    const auto valueToWeight = GetValueToWeight(args.args[0].GetString(), args.args[1].GetString());

    SetResultString(args, valueToWeight.c_str());
};

void CAHZScaleform::ProcessBookSkill(const TargetData& target, RE::GFxFunctionHandler::Params& args)
{
    // If the target is not valid or it can't be picked up by the player
    if (!target.isValid) {
        SetResultString(args, "");
        return;
    }

    SetResultString(args, target.bookSkillText.c_str());
};

void CAHZScaleform::SetResultString(RE::GFxFunctionHandler::Params& args, const char* str)
{
    if (!args.movie || !args.retVal) {
        return;
    }
    // Copy into the movie's string storage; never return a pointer into a temporary snapshot.
    args.movie->CreateString(args.retVal, str ? str : "");
};

void CAHZScaleform::ReplaceStringInPlace(std::string& subject, const std::string& search,
    const std::string& replace)
{
    size_t pos = 0;
    while ((pos = subject.find(search, pos)) != std::string::npos) {
        subject.replace(pos, search.length(), replace);
        pos += replace.length();
    }
};

void CAHZScaleform::ProcessTargetObject(const TargetData& target, RE::GFxFunctionHandler::Params& args)
{
    if (!target.isValid) {
        args.args[0].DeleteMember("targetObj");
        return;
    }

    RE::GFxValue obj;
    args.movie->CreateObject(&obj);
    const auto totalArmorOrWeapon = target.equippedRatingOrDamage;
    const auto difference = target.ratingOrDamageDifference;
    const auto totalWarmthRating = target.totalWarmthRating;
    const auto warmthDifference = target.warmthDifference;
    const auto isSurvivalMode = target.isSurvivalMode;
    const auto warmthRating = isSurvivalMode ? target.armorWarmthRating : 0.0F;

    // Enter the data into the Scaleform function
    RegisterNumber(&obj, "ratingOrDamage", totalArmorOrWeapon);
    RegisterNumber(&obj, "difference", difference);
    RegisterNumber(&obj, "totalWarmthRating", totalWarmthRating);
    RegisterNumber(&obj, "warmthRating", warmthRating);
    RegisterNumber(&obj, "warmthDifference", warmthDifference);
    RegisterBoolean(&obj, "isSurvivalMode", isSurvivalMode);

    float weight = target.weight;

    RegisterNumber(&obj, "objWeight", weight);

    // Used by the scaleform script to know if this is a weapon, armor, or something else
    RegisterNumber(&obj, "formType", static_cast<uint32_t>(target.formType));
    args.args[0].SetMember("targetObj", obj);
};

void CAHZScaleform::BuildIngredientObject(const TargetData& target, RE::GFxFunctionHandler::Params& args)
{
    // If no ingredient, then we are done here
    if (!target.isValid) {
        args.args[0].DeleteMember("ingredientObj");
        return;
    }

    // Not all ingredients have 4 effects
    auto effectsCount = static_cast<uint32_t>(target.knownIngredientEffects.size());

    RE::GFxValue obj2;
    args.movie->CreateObject(&obj2);

    if (effectsCount >= 1)
        RegisterString(&obj2, "effect1", target.knownIngredientEffects[0].c_str());

    if (effectsCount >= 2)
        RegisterString(&obj2, "effect2", target.knownIngredientEffects[1].c_str());

    if (effectsCount >= 3)
        RegisterString(&obj2, "effect3", target.knownIngredientEffects[2].c_str());

    if (effectsCount >= 4)
        RegisterString(&obj2, "effect4", target.knownIngredientEffects[3].c_str());
    args.args[0].SetMember("ingredientObj", obj2);
};

void CAHZScaleform::BuildInventoryObject(const TargetData& target, RE::GFxFunctionHandler::Params& args)
{
    // Used to store the name
    string name;

    // Used to store the count of the item
    uint32_t itemCount = 0;

    if (!target.isValid) {
        args.args[0].DeleteMember("inventoryObj");
        return;
    }

    // Get the number of this in the inventory
    itemCount = target.inventoryCount;

    name = target.displayName;

    // If the name contains a string
    if (name.length()) {
        RE::GFxValue obj;
        args.movie->CreateObject(&obj);

        RegisterString(&obj, "inventoryName", name.c_str());
        RegisterNumber(&obj, "inventoryCount", itemCount);

        // Add the object to the scaleform function
        args.args[0].SetMember("inventoryObj", obj);
    } else {
        args.args[0].DeleteMember("inventoryObj");
    }
}

void CAHZScaleform::RegisterString(RE::GFxValue* dst, const char* name, const char* str)
{
    RE::GFxValue fxValue;
    fxValue.SetString(str);
    dst->SetMember(name, fxValue);
}

void CAHZScaleform::RegisterNumber(RE::GFxValue* dst, const char* name, double value)
{
    RE::GFxValue fxValue;
    fxValue.SetNumber(value);
    dst->SetMember(name, fxValue);
}

void CAHZScaleform::RegisterBoolean(RE::GFxValue* dst, const char* name, bool value)
{
    RE::GFxValue fxValue;
    fxValue.SetBoolean(value);
    dst->SetMember(name, fxValue);
}

void CAHZScaleform::ProcessValidTarget(const TargetData& target, RE::GFxFunctionHandler::Params& args)
{
    if (!target.isValid) {
        // return false, indicating that the target object is not valid for acquiring data
        args.retVal->SetBoolean(false);
        args.args[0].DeleteMember("dataObj");
        return;
    }

    RE::GFxValue obj;
    args.movie->CreateObject(&obj);

    RegisterBoolean(&obj, "canCarry", target.canCarry);

    // Add the object to the scaleform function
    args.args[0].SetMember("outObj", obj);

    // return false, indicating that the target object is not valid for acquiring data
    args.retVal->SetBoolean(true);
}

void CAHZScaleform::ProcessPlayerData(RE::GFxFunctionHandler::Params& args)
{
    RE::GFxValue obj;
    args.movie->CreateObject(&obj);
    auto pPC = RE::PlayerCharacter::GetSingleton();
    if (pPC) {
        const auto values = pPC->AsActorValueOwner();
        if (!values) {
            logger::debug("Cannot read player data: actor value owner is unavailable"sv);
            return;
        }
        auto encumbranceNumber = values->GetActorValue(RE::ActorValue::kInventoryWeight);
        auto maxEncumbranceNumber = values->GetActorValue(RE::ActorValue::kCarryWeight);

        // Enter the data into the Scaleform function
        RegisterNumber(&obj, "encumbranceNumber", encumbranceNumber);
        RegisterNumber(&obj, "maxEncumbranceNumber", maxEncumbranceNumber);
        RegisterNumber(&obj, "goldNumber", CAHZPlayerInfo::GetGoldAmount());
        args.args[0].SetMember("playerObj", obj);
    }
}

void CAHZScaleform::FormatDescription(const std::string unFormated, std::string& formatted)
{
    std::string workingUnformatted{ unFormated.c_str() };
    formatted.clear();

    // Fixes a bug from CK when adding new lines.  It adds "\r\n\n" for each "\r\n"
    ReplaceStringInPlace(workingUnformatted, "\r\n\n", "\n");

    // Also convert from Windows to Unix, because that is what works best with flash
    ReplaceStringInPlace(workingUnformatted, "\r\n", "\n");

    constexpr std::string_view stringFormatter = "<font face = '$EverywhereMediumFont' size = '20' color = '#FFFFFF'>{}</font>"sv;

    std::regex rgx("\\<.+?\\>");

    std::smatch match;

    // Regex search uses const iterator
    const auto cs = const_cast<std::string&>(workingUnformatted);
    auto       searchStart(cs.cbegin());
    auto       workingString = workingUnformatted;

    while (regex_search(searchStart, cs.end(), match, rgx)) {
        std::string temps = match[0];
        ReplaceStringInPlace(temps, "<", "");
        ReplaceStringInPlace(temps, ">", "");
        // The game also removes "." because they are part of the substitution logic.
        ReplaceStringInPlace(temps, ".", "");
        std::string origMatch = match[0];

        // Replace each "<xxx>" with stringFormatter(xxx)
        const auto tempformatter = fmt::format(stringFormatter, temps.c_str());
        ReplaceStringInPlace(workingString, origMatch, tempformatter);

        searchStart += match.position() + match.length();
    }

    formatted.append(workingString);
}
