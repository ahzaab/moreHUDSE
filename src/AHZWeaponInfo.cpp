#include "PCH.h"
#include <list>
#include <algorithm>
#include "AHZWeaponInfo.h"
#include "AHZFormLookup.h"

class ContainerAmmoVistor
{
public:
    AHZWeaponData ammoData;
    ContainerAmmoVistor()
    = default;

    auto Accept(RE::InventoryEntryData* pEntryData) -> bool
    {
        if (pEntryData && pEntryData->object && pEntryData->object->GetFormType() == RE::FormType::Ammo) {
            if (pEntryData->extraLists) {
                for (auto it = pEntryData->extraLists->begin(); it != pEntryData->extraLists->end(); ++it) {
                    auto extraList = *it;
                    if (extraList->HasType(RE::ExtraDataType::kWorn)) {
                        ammoData.equipData.boundObject = pEntryData->object;
                        ammoData.equipData.pExtraData = extraList;
                        ammoData.ammo = DYNAMIC_CAST(ammoData.equipData.boundObject, RE::TESForm, RE::TESAmmo);
                        if (ammoData.ammo) {
                            return false;
                        }
                    }
                }
            }
        }
        return true;
    }
};

auto CAHZWeaponInfo::GetWeaponInfo(RE::TESObjectREFR* thisObject) -> AHZWeaponData
{
    AHZWeaponData weaponData;

    // Must be a weapon
    if (!thisObject)
        return weaponData;

    auto baseForm = thisObject->GetBaseObject();

    if (!baseForm) {
        return weaponData;
    }

    if (baseForm->GetFormType() != RE::FormType::Weapon &&
        baseForm->GetFormType() != RE::FormType::Ammo &&
        baseForm->GetFormType() != RE::FormType::Projectile) {
        return weaponData;
    }

    weaponData.equipData.boundObject = baseForm;
    weaponData.equipData.pExtraData = &thisObject->extraList;

    if (baseForm->GetFormType() == RE::FormType::Weapon)
        weaponData.weapon = DYNAMIC_CAST(weaponData.equipData.boundObject, RE::TESForm, RE::TESObjectWEAP);
    else if (baseForm->GetFormType() == RE::FormType::Ammo)
        weaponData.ammo = DYNAMIC_CAST(weaponData.equipData.boundObject, RE::TESForm, RE::TESAmmo);
    else if (baseForm->GetFormType() == RE::FormType::Projectile) {
        auto asArrowProjectile = DYNAMIC_CAST(thisObject, RE::TESObjectREFR, RE::ArrowProjectile);
        weaponData.ammo = DYNAMIC_CAST(AHZGetForm(thisObject), RE::TESForm, RE::TESAmmo);
        if (asArrowProjectile) {
            weaponData.equipData.boundObject = weaponData.ammo;
            weaponData.equipData.pExtraData = &asArrowProjectile->extraList;
        }
    }

    return weaponData;
}

auto CAHZWeaponInfo::GetLeftHandWeapon() -> AHZWeaponData
{
    AHZWeaponData weaponData;
    auto          pPC = RE::PlayerCharacter::GetSingleton();
    if (pPC) {
        auto list = pPC->GetInventoryChanges()->entryList;
        auto equippedItem = pPC->GetEquippedObject(true);

        if (!equippedItem) {
            return weaponData;
        }

        for (auto it = list->begin(); it != list->end(); ++it) {
            auto entry = *it;
            if (entry->object->GetFormID() == equippedItem->formID) {
                for (auto entryListIT = entry->extraLists->begin(); entryListIT != entry->extraLists->end(); ++entryListIT) {
                    auto extraData = *entryListIT;
                    if (extraData &&
                        (extraData->HasType(RE::ExtraDataType::kWorn) || extraData->HasType(RE::ExtraDataType::kWornLeft))) {
                        weaponData.equipData.boundObject = entry->object;
                        weaponData.equipData.pExtraData = extraData;

                        if (weaponData.equipData.boundObject) {
                            weaponData.weapon = DYNAMIC_CAST(weaponData.equipData.boundObject, RE::TESForm, RE::TESObjectWEAP);
                        }

                        return weaponData;
                    }
                }
            }
        }
    }
    return weaponData;
}

auto CAHZWeaponInfo::GetRightHandWeapon() -> AHZWeaponData
{
    AHZWeaponData weaponData;
    auto          pPC = RE::PlayerCharacter::GetSingleton();
    if (pPC) {
        auto list = pPC->GetInventoryChanges()->entryList;
        auto equippedItem = pPC->GetEquippedObject(false);

        if (!equippedItem) {
            return weaponData;
        }

        for (auto it = list->begin(); it != list->end(); ++it) {
            auto entry = *it;
            if (entry && entry->object->GetFormID() == equippedItem->formID) {
                if (entry->extraLists) {
                    for (auto entryListIT = entry->extraLists->begin(); entryListIT != entry->extraLists->end(); ++entryListIT) {
                        auto extraData = *entryListIT;
                        if (extraData &&
                            extraData->HasType(RE::ExtraDataType::kWorn)) {
                            weaponData.equipData.boundObject = entry->object;
                            weaponData.equipData.pExtraData = extraData;

                            if (weaponData.equipData.boundObject) {
                                weaponData.weapon = DYNAMIC_CAST(weaponData.equipData.boundObject, RE::TESForm, RE::TESObjectWEAP);
                            }

                            return weaponData;
                        }
                    }
                }
            }
        }
    }
    return weaponData;
}

auto CAHZWeaponInfo::GetEquippedAmmo() -> AHZWeaponData
{
    AHZWeaponData ammoData;
    auto          pPC = RE::PlayerCharacter::GetSingleton();
    if (pPC) {
        auto list = pPC->GetInventoryChanges()->entryList;

        for (auto it = list->begin(); it != list->end(); ++it) {
            auto entry = *it;
            if (entry && entry->object->GetFormType() == RE::FormType::Ammo) {
                if (entry->extraLists) {
                    for (auto entryListIT = entry->extraLists->begin(); entryListIT != entry->extraLists->end(); ++entryListIT) {
                        auto extraData = *entryListIT;
                        if (extraData &&
                            extraData->HasType(RE::ExtraDataType::kWorn)) {
                            ammoData.equipData.boundObject = entry->object;
                            ammoData.equipData.pExtraData = extraData;

                            if (ammoData.equipData.boundObject) {
                                ammoData.ammo = DYNAMIC_CAST(ammoData.equipData.boundObject, RE::TESForm, RE::TESAmmo);
                                return ammoData;
                            }
                        }
                    }
                }
            }
        }
    }

    return ammoData;
}
