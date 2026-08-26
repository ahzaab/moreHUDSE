#include "pch.h"

#include "AHZTargetDataCollector.h"

#include "AHZItemComparison.h"

TargetHUDData CAHZTargetDataCollector::Collect(const ResolvedTarget& a_resolvedTarget, const TargetData& a_target)
{
    TargetHUDData result;
    result.isValid = a_target.isValid;
    if (!result.isValid) {
        return result;
    }

    result.weight = a_target.weight;
    result.formType = a_target.formType;

    const bool supportsItemComparison = a_target.formType == RE::FormType::Armor ||
                                        a_target.formType == RE::FormType::Weapon ||
                                        a_target.formType == RE::FormType::Ammo;

    if (a_target.formType == RE::FormType::Weapon || a_target.formType == RE::FormType::Ammo) {
        result.ratingOrDamage = GetTotalActualWeaponDamage();
        result.difference = GetWeaponDamageDiff(a_target);
    } else if (a_target.formType == RE::FormType::Armor) {
        result.ratingOrDamage = GetTotalActualArmorRating();
        result.difference = GetArmorRatingDiff(a_target);

        if (CAHZTarget::IsSurvivalMode()) {
            result.isSurvivalMode = true;
            result.totalWarmthRating = GetTotalWarmthRating();
            result.warmthDifference = GetWarmthRatingDiff(a_target);
            result.warmthRating = a_target.armorWarmthRating;
        }
    }

    if (supportsItemComparison) {
        CAHZItemComparisonContext comparison;
        if (const auto engineResult = comparison.Compare(
                a_resolvedTarget.GetBoundObject(), a_resolvedTarget.extraData)) {
            if (std::abs(engineResult->statChange - result.difference) > 0.01F) {
                logger::debug("[engine experiment] comparison differs for {:08X}: engine={}, legacy={}",
                    a_target.formId,
                    engineResult->statChange,
                    result.difference);
            }
            result.difference = engineResult->statChange;
        }
    }

    // sourceReference is deliberately retained in ResolvedTarget for future
    // calculations that need world/runtime state. No current comparison reads it.
    return result;
}

auto CAHZTargetDataCollector::GetActualDamage(AHZWeaponData* weaponData) -> float
{
    if (!weaponData)
        return 0.0f;

    auto pPC = RE::PlayerCharacter::GetSingleton();

    if (pPC) {
        RE::InventoryEntryData objDesc(weaponData->equipData.boundObject, 0);
        if (weaponData->equipData.pExtraData) {
            objDesc.AddExtraList(weaponData->equipData.pExtraData);
        }

        float fDamage = pPC->GetDamage(&objDesc);

        return Round(fDamage);
    }

    return 0.0;
}

auto CAHZTargetDataCollector::GetArmorWarmthRating(AHZArmorData* armorData) -> float
{
    if (!armorData->armor)
        return 0.0f;

    auto fRating = CAHZTarget::GetArmorWarmthRating_Native(armorData->equipData.boundObject);

    return Round(fRating);
}

auto CAHZTargetDataCollector::GetActualArmorRating(AHZArmorData* armorData) -> float
{
    if (!armorData->armor)
        return 0.0f;

    auto pPC = RE::PlayerCharacter::GetSingleton();

    if (pPC) {
        RE::InventoryEntryData objDesc(armorData->equipData.boundObject, 0);
        if (armorData->equipData.pExtraData) {
            objDesc.AddExtraList(armorData->equipData.pExtraData);
        }

        auto fRating = pPC->GetArmorValue(&objDesc);

        return Round(fRating);
    }

    return 0.0;
}

// TODO: Port Note, I should probably decode the function that already does this
auto CAHZTargetDataCollector::GetTotalActualArmorRating() -> float
{
    float totalRating = 0.0f;
    // Keep a list of items to make sure they are not added more than once
    // Some items take up more than one slot
    std::list<const RE::TESForm*> clist;
    for (uint64_t slot = 1; slot <= static_cast<uint64_t>(0x2000); slot <<= 1) {
        AHZArmorData armorData = CAHZArmorInfo::GetArmorFromSlotMask(static_cast<RE::BIPED_MODEL::BipedObjectSlot>(slot));
        if (armorData.equipData.boundObject) {
            if (find(clist.begin(), clist.end(), armorData.equipData.boundObject) == clist.end()) {
                clist.push_front(armorData.equipData.boundObject);
                if (armorData.armor) {
                    totalRating += GetActualArmorRating(&armorData);
                }
            }
        }
    }
    const auto legacyRating = Round(totalRating);
    if (auto player = RE::PlayerCharacter::GetSingleton()) {
        player->WornArmorChanged();
        const auto engineRating = Round(player->AsActorValueOwner()->GetActorValue(RE::ActorValue::kDamageResist));
        if (std::abs(engineRating - legacyRating) > 0.01F) {
            logger::debug("[engine experiment] armor total differs: engine={}, legacy={}", engineRating, legacyRating);
        }
        return engineRating;
    }

    return legacyRating;
}

auto CAHZTargetDataCollector::GetTotalWarmthRating() -> float
{
    auto pPC = RE::PlayerCharacter::GetSingleton();

    if (!pPC) {
        return 0.0;
    }

#if !defined(ENABLE_SKYRIM_VR)
    return pPC->GetWarmthRating();
#else
    return CAHZTarget::GetActorWarmthRating_Native(pPC, 0.0);
#endif
}

auto CAHZTargetDataCollector::Round(float r) -> float
{
    return (r >= 0.0f) ? floor(r + 0.5f) : ceil(r - 0.5f);
}

float CAHZTargetDataCollector::GetArmorRatingDiff(const TargetData& target)
{
    float oldArmorRating = 0.0f;
    float newArmorRating = 0.0f;
    float oldTotalArmorRating = 0.0f;
    float newTotalArmorRating = 0.0f;
    float deltaRating = 0.0f;
    if (!target.isValid)
        return 0.0f;

    if (target.formType != RE::FormType::Armor)
        return 0.0f;

    newArmorRating = target.armorRating;

    // Get the armor rating from the armor that shares the same slot
    AHZArmorData sameSlotData = CAHZArmorInfo::GetArmorFromSlotMask(
        target.armorslotMask);
    if (sameSlotData.armor) {
        oldArmorRating = GetActualArmorRating(&sameSlotData);
    }

    // Get the total
    oldTotalArmorRating = GetTotalActualArmorRating();

    newTotalArmorRating = (oldTotalArmorRating - oldArmorRating) + newArmorRating;

    deltaRating = newTotalArmorRating - oldTotalArmorRating;

    return deltaRating;
}

auto CAHZTargetDataCollector::GetWarmthRatingDiff(const TargetData& target) -> float
{
    float oldArmorRating = 0.0f;
    float newArmorRating = 0.0f;
    float oldTotalArmorRating = 0.0f;
    float newTotalArmorRating = 0.0f;
    float deltaRating = 0.0f;
    if (!target.isValid)
        return 0.0f;

    if (target.formType != RE::FormType::Armor)
        return 0.0;

    newArmorRating = target.armorWarmthRating;

    // Get the armor rating from the armor that shares the same slot
    AHZArmorData sameSlotData = CAHZArmorInfo::GetArmorFromSlotMask(target.armorslotMask);
    if (sameSlotData.armor) {
        oldArmorRating = GetArmorWarmthRating(&sameSlotData);
    }

    // Get the total
    oldTotalArmorRating = GetTotalWarmthRating();

    newTotalArmorRating = (oldTotalArmorRating - oldArmorRating) + newArmorRating;

    deltaRating = newTotalArmorRating - oldTotalArmorRating;

    return deltaRating;
}

auto CAHZTargetDataCollector::GetTotalActualWeaponDamage() -> float
{
    std::optional<float> engineDamage;
    if (REL::Module::IsAE()) {
        if (auto player = RE::PlayerCharacter::GetSingleton()) {
            engineDamage = player->GetEquippedWeaponsDamage();
        }
    }

    float         totalWeaponDamage = 0.0;
    bool          is2Handed = false;
    AHZWeaponData leftWeapon = CAHZWeaponInfo::GetLeftHandWeapon();
    AHZWeaponData rightWeapon = CAHZWeaponInfo::GetRightHandWeapon();
    AHZWeaponData equippedAmmo = CAHZWeaponInfo::GetEquippedAmmo();

    if (leftWeapon.weapon) {
        if (IsBow(leftWeapon.weapon)) {
            is2Handed = true;
            totalWeaponDamage = GetActualDamage(&leftWeapon);

            // Add the arrow damage
            if (equippedAmmo.ammo && !IsBolt(equippedAmmo.ammo)) {
                totalWeaponDamage += GetActualDamage(&equippedAmmo);
            }
        } else if (IsCrossBow(leftWeapon.weapon)) {
            is2Handed = true;
            totalWeaponDamage = GetActualDamage(&leftWeapon);

            // Add the arrow damage
            if (equippedAmmo.ammo && IsBolt(equippedAmmo.ammo)) {
                totalWeaponDamage += GetActualDamage(&equippedAmmo);
            }
        } else if (IsTwoHanded(leftWeapon.weapon)) {
            is2Handed = true;
            totalWeaponDamage = GetActualDamage(&leftWeapon);
        } else if (IsOneHanded(leftWeapon.weapon)) {
            totalWeaponDamage = GetActualDamage(&leftWeapon);
        }
    }

    if (rightWeapon.weapon) {
        if (IsBow(rightWeapon.weapon) && !is2Handed) {
            is2Handed = true;
            totalWeaponDamage = GetActualDamage(&rightWeapon);

            // Add the arrow damage
            if (equippedAmmo.ammo && !IsBolt(equippedAmmo.ammo)) {
                totalWeaponDamage += GetActualDamage(&equippedAmmo);
            }
        } else if (IsCrossBow(rightWeapon.weapon) && !is2Handed) {
            is2Handed = true;
            totalWeaponDamage = GetActualDamage(&rightWeapon);

            // Add the arrow damage
            if (equippedAmmo.ammo && IsBolt(equippedAmmo.ammo)) {
                totalWeaponDamage += GetActualDamage(&equippedAmmo);
            }
        } else if (IsTwoHanded(rightWeapon.weapon) && !is2Handed) {
            is2Handed = true;
            totalWeaponDamage = GetActualDamage(&rightWeapon);
        } else if (IsOneHanded(rightWeapon.weapon)) {
            // Add the damage from the second weapon
            totalWeaponDamage += GetActualDamage(&rightWeapon);
        }
    }
    if (engineDamage) {
        if (std::abs(*engineDamage - totalWeaponDamage) > 0.01F) {
            logger::debug("[engine experiment] weapon total differs: engine={}, legacy={}", *engineDamage, totalWeaponDamage);
        }
        return *engineDamage;
    }

    return totalWeaponDamage;
}

auto CAHZTargetDataCollector::IsBolt(RE::TESAmmo* thisAmmo) -> bool
{
    return (thisAmmo->IsBolt());
}

auto CAHZTargetDataCollector::GetWeaponDamageDiff(const TargetData& target) -> float
{
    float         totalWeaponDamage = 0.0f;
    float         targetArrowDamage = 0.0f;
    AHZWeaponData leftWeapon;
    AHZWeaponData rightWeapon;
    AHZWeaponData equippedAmmo;

    if (!target.isValid)
        return 0.0f;

    equippedAmmo = CAHZWeaponInfo::GetEquippedAmmo();

    //DumpClass(targetWeapon.ammo, 256);

    // Must be a weapon or armor targeted
    if (target.formType == RE::FormType::Weapon) {
        if (equippedAmmo.ammo) {
            // TODO Try to get the equipped arrow with extra data
            if (IsBow(target) && !IsBolt(equippedAmmo.ammo)) {
                targetArrowDamage = GetActualDamage(&equippedAmmo);
            } else if (IsCrossBow(target) && IsBolt(equippedAmmo.ammo)) {
                targetArrowDamage = GetActualDamage(&equippedAmmo);
            }
        }
    } else if (target.formType == RE::FormType::Ammo) {
        //targetAmmo = targetWeapon.ammo;
    } else {
        return 0.0f;
    }

    rightWeapon = CAHZWeaponInfo::GetRightHandWeapon();
    leftWeapon = CAHZWeaponInfo::GetLeftHandWeapon();

    if (leftWeapon.weapon) {
        if (IsBow(leftWeapon.weapon)) {
            float tempDamage = GetActualDamage(&leftWeapon);
            float tempArrowDamage = 0.0f;

            // Add the arrow damage
            if (equippedAmmo.ammo && !IsBolt(equippedAmmo.ammo)) {
                tempArrowDamage = GetActualDamage(&equippedAmmo);
                tempDamage += tempArrowDamage;
            }

            if (target.formType == RE::FormType::Weapon) {
                return (target.damage + targetArrowDamage) - tempDamage;
            } else if (target.formType == RE::FormType::Ammo && !target.isBoltAmmo) {
                return ((tempDamage - tempArrowDamage) + target.damage) - tempDamage;
            } else {
                return 0.0f;
            }
        } else if (IsCrossBow(leftWeapon.weapon)) {
            auto  tempDamage = GetActualDamage(&leftWeapon);
            float tempArrowDamage = 0.0f;

            // Add the arrow damage
            if (equippedAmmo.ammo && IsBolt(equippedAmmo.ammo)) {
                tempArrowDamage = GetActualDamage(&equippedAmmo);
                tempDamage += tempArrowDamage;
            }

            if (target.formType == RE::FormType::Weapon) {
                return (target.damage + targetArrowDamage) - tempDamage;
            } else if (target.formType == RE::FormType::Ammo && target.isBoltAmmo) {
                return ((tempDamage - tempArrowDamage) + target.damage - tempDamage);
            } else {
                return 0.0f;
            }
        } else if (IsTwoHanded(leftWeapon.weapon) && target.formType == RE::FormType::Weapon) {
            return (target.damage + targetArrowDamage) - GetActualDamage(&leftWeapon);
        } else if (IsOneHanded(leftWeapon.weapon)) {
            totalWeaponDamage = GetActualDamage(&leftWeapon);
        }
    }

    if (rightWeapon.weapon) {
        if (IsOneHanded(rightWeapon.weapon)) {
            // Add the damage from the second weapon
            totalWeaponDamage += GetActualDamage(&rightWeapon);
        }
    }

    // If we made it this far either no weapon is equipped or single handed weapons are equipped
    if (target.formType == RE::FormType::Weapon) {
        return (target.damage + targetArrowDamage) - totalWeaponDamage;
    }

    return 0.0f;
}

auto CAHZTargetDataCollector::IsTwoHanded(RE::TESObjectWEAP* thisWeapon) -> bool
{
    // TODO: Port Note, this is quite different, SKSE had more types defined.  So
    // for now we better stick with the values that were used prior to port

    enum  // type
    {
        kType_HandToHandMelee = 0,
        kType_OneHandSword,
        kType_OneHandDagger,
        kType_OneHandAxe,
        kType_OneHandMace,
        kType_TwoHandSword,
        kType_TwoHandAxe,
        kType_Bow,
        kType_Staff,
        kType_CrossBow,
        kType_H2H,
        kType_1HS,
        kType_1HD,
        kType_1HA,
        kType_1HM,
        kType_2HS,
        kType_2HA,
        kType_Bow2,
        kType_Staff2,
        kType_CBow
    };

    return (thisWeapon->IsTwoHandedSword() ||
            thisWeapon->IsTwoHandedAxe() ||
            thisWeapon->IsBow() ||
            thisWeapon->IsCrossbow() ||
            thisWeapon->weaponData.animationType == static_cast<RE::WEAPON_TYPE>(kType_2HS) ||
            thisWeapon->weaponData.animationType == static_cast<RE::WEAPON_TYPE>(kType_2HA) ||
            thisWeapon->weaponData.animationType == static_cast<RE::WEAPON_TYPE>(kType_Bow2) ||
            thisWeapon->weaponData.animationType == static_cast<RE::WEAPON_TYPE>(kType_CBow));
}

auto CAHZTargetDataCollector::IsOneHanded(RE::TESObjectWEAP* thisWeapon) -> bool
{
    // TODO: Port Note, this is quite different, SKSE had more types defined.  So
    // for now we better stick with the values that were used prior to port
    enum  // type
    {
        kType_HandToHandMelee = 0,
        kType_OneHandSword,
        kType_OneHandDagger,
        kType_OneHandAxe,
        kType_OneHandMace,
        kType_TwoHandSword,
        kType_TwoHandAxe,
        kType_Bow,
        kType_Staff,
        kType_CrossBow,
        kType_H2H,
        kType_1HS,
        kType_1HD,
        kType_1HA,
        kType_1HM,
        kType_2HS,
        kType_2HA,
        kType_Bow2,
        kType_Staff2,
        kType_CBow
    };

    return (thisWeapon->IsOneHandedSword() ||
            thisWeapon->IsOneHandedDagger() ||
            thisWeapon->IsOneHandedAxe() ||
            thisWeapon->IsOneHandedMace() ||
            thisWeapon->weaponData.animationType == static_cast<RE::WEAPON_TYPE>(kType_1HS) ||
            thisWeapon->weaponData.animationType == static_cast<RE::WEAPON_TYPE>(kType_1HD) ||
            thisWeapon->weaponData.animationType == static_cast<RE::WEAPON_TYPE>(kType_1HA) ||
            thisWeapon->weaponData.animationType == static_cast<RE::WEAPON_TYPE>(kType_1HM));
}

bool CAHZTargetDataCollector::IsBow(RE::TESObjectWEAP* thisWeapon)
{
    // TODO: Port Note, this is quite different, SKSE had more types defined.  So
    // for now we better stick with the values that were used prior to port
    enum  // type
    {
        kType_HandToHandMelee = 0,
        kType_OneHandSword,
        kType_OneHandDagger,
        kType_OneHandAxe,
        kType_OneHandMace,
        kType_TwoHandSword,
        kType_TwoHandAxe,
        kType_Bow,
        kType_Staff,
        kType_CrossBow,
        kType_H2H,
        kType_1HS,
        kType_1HD,
        kType_1HA,
        kType_1HM,
        kType_2HS,
        kType_2HA,
        kType_Bow2,
        kType_Staff2,
        kType_CBow
    };

    return (thisWeapon->IsBow() ||
            thisWeapon->weaponData.animationType == static_cast<RE::WEAPON_TYPE>(kType_Bow2));
}

bool CAHZTargetDataCollector::IsBow(const TargetData& thisWeapon)
{
    // TODO: Port Note, this is quite different, SKSE had more types defined.  So
    // for now we better stick with the values that were used prior to port
    enum  // type
    {
        kType_HandToHandMelee = 0,
        kType_OneHandSword,
        kType_OneHandDagger,
        kType_OneHandAxe,
        kType_OneHandMace,
        kType_TwoHandSword,
        kType_TwoHandAxe,
        kType_Bow,
        kType_Staff,
        kType_CrossBow,
        kType_H2H,
        kType_1HS,
        kType_1HD,
        kType_1HA,
        kType_1HM,
        kType_2HS,
        kType_2HA,
        kType_Bow2,
        kType_Staff2,
        kType_CBow
    };

    return (thisWeapon.weaponType == RE::WEAPON_TYPE::kBow ||
            thisWeapon.weaponType == static_cast<RE::WEAPON_TYPE>(kType_Bow2));
}

bool CAHZTargetDataCollector::IsCrossBow(RE::TESObjectWEAP* thisWeapon)
{
    // TODO: Port Note, this is quite different, SKSE had more types defined.  So
    // for now we better stick with the values that were used prior to port
    enum  // type
    {
        kType_HandToHandMelee = 0,
        kType_OneHandSword,
        kType_OneHandDagger,
        kType_OneHandAxe,
        kType_OneHandMace,
        kType_TwoHandSword,
        kType_TwoHandAxe,
        kType_Bow,
        kType_Staff,
        kType_CrossBow,
        kType_H2H,
        kType_1HS,
        kType_1HD,
        kType_1HA,
        kType_1HM,
        kType_2HS,
        kType_2HA,
        kType_Bow2,
        kType_Staff2,
        kType_CBow
    };

    return (thisWeapon->IsCrossbow() ||
            thisWeapon->weaponData.animationType == static_cast<RE::WEAPON_TYPE>(kType_CBow));
}

bool CAHZTargetDataCollector::IsCrossBow(const TargetData& thisWeapon)
{
    // TODO: Port Note, this is quite different, SKSE had more types defined.  So
    // for now we better stick with the values that were used prior to port
    enum  // type
    {
        kType_HandToHandMelee = 0,
        kType_OneHandSword,
        kType_OneHandDagger,
        kType_OneHandAxe,
        kType_OneHandMace,
        kType_TwoHandSword,
        kType_TwoHandAxe,
        kType_Bow,
        kType_Staff,
        kType_CrossBow,
        kType_H2H,
        kType_1HS,
        kType_1HD,
        kType_1HA,
        kType_1HM,
        kType_2HS,
        kType_2HA,
        kType_Bow2,
        kType_Staff2,
        kType_CBow
    };

    return (thisWeapon.weaponType == RE::WEAPON_TYPE::kCrossbow ||
            thisWeapon.weaponType == static_cast<RE::WEAPON_TYPE>(kType_CBow));
}
