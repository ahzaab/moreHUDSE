#pragma once

#include "AHZArmorInfo.h"
#include "AHZTarget.h"
#include "AHZWeaponInfo.h"

struct TargetHUDData
{
    bool         isValid{ false };
    float        ratingOrDamage{ 0.0F };
    float        difference{ 0.0F };
    float        totalWarmthRating{ 0.0F };
    float        warmthRating{ 0.0F };
    float        warmthDifference{ 0.0F };
    bool         isSurvivalMode{ false };
    float        weight{ 0.0F };
    RE::FormType formType{ RE::FormType::None };
};

class CAHZTargetDataCollector
{
public:
    // The resolved base form identifies the displayed item. The source reference
    // remains available for calculations that need runtime state, while extraData
    // is used only when it belongs to the resolved item instance.
    [[nodiscard]] static TargetHUDData Collect(const ResolvedTarget& a_resolvedTarget, const TargetData& a_target);

private:
    static float GetActualDamage(AHZWeaponData* a_weaponData);
    static float GetActualArmorRating(AHZArmorData* a_armorData);
    static float GetArmorWarmthRating(AHZArmorData* a_armorData);
    static float GetTotalActualArmorRating();
    static float GetTotalWarmthRating();
    static float GetArmorRatingDiff(const TargetData& a_target);
    static float GetWarmthRatingDiff(const TargetData& a_target);
    static float GetWeaponDamageDiff(const TargetData& a_target);
    static float GetTotalActualWeaponDamage();
    static bool  IsTwoHanded(RE::TESObjectWEAP* a_weapon);
    static bool  IsOneHanded(RE::TESObjectWEAP* a_weapon);
    static bool  IsBow(RE::TESObjectWEAP* a_weapon);
    static bool  IsBow(const TargetData& a_weapon);
    static bool  IsCrossBow(RE::TESObjectWEAP* a_weapon);
    static bool  IsCrossBow(const TargetData& a_weapon);
    static bool  IsBolt(RE::TESAmmo* a_ammo);
    static float Round(float a_value);
};
