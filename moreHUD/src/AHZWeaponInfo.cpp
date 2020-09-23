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
	{ }

	bool Accept(RE::InventoryEntryData* pEntryData)
	{
        if (pEntryData && pEntryData->object && pEntryData->object->GetFormType() == RE::FormType::Ammo)
		{
			if (pEntryData->extraLists)
			{
                for (auto it = pEntryData->extraLists->begin(); it != pEntryData->extraLists->end(); ++it)
				{
                    auto extraList = *it;
					if (extraList->HasType(RE::ExtraDataType::kWorn))
					{
						ammoData.equipData.pForm = pEntryData->object;
						ammoData.equipData.pExtraData = extraList;
						ammoData.ammo = DYNAMIC_CAST(ammoData.equipData.pForm, RE::TESForm, RE::TESAmmo);
						if (ammoData.ammo)
						{
							return false;
						}
					}
				}
			}
		}
		return true;
	}
};

AHZWeaponData CAHZWeaponInfo::GetWeaponInfo(RE::TESObjectREFR * thisObject)
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
        baseForm->GetFormType() != RE::FormType::Projectile)
	{
		return weaponData;
	}

	weaponData.equipData.pForm = baseForm;
	weaponData.equipData.pExtraData = &thisObject->extraList;

	if (baseForm->GetFormType() == RE::FormType::Weapon)
		weaponData.weapon = DYNAMIC_CAST(weaponData.equipData.pForm,RE::TESForm, RE::TESObjectWEAP);
    else if (baseForm->GetFormType() == RE::FormType::Ammo)
		weaponData.ammo = DYNAMIC_CAST(weaponData.equipData.pForm,RE::TESForm,RE::TESAmmo);
    else if (baseForm->GetFormType() == RE::FormType::Projectile)
	{
		auto asArrowProjectile = DYNAMIC_CAST(thisObject, RE::TESObjectREFR, RE::ArrowProjectile);
		weaponData.ammo = DYNAMIC_CAST(AHZGetForm(thisObject),RE::TESForm, RE::TESAmmo);
		if (asArrowProjectile) {
			weaponData.equipData.pForm = weaponData.ammo;
			weaponData.equipData.pExtraData = &asArrowProjectile->extraList;
		}
	}

	return weaponData;
}

AHZWeaponData CAHZWeaponInfo::GetLeftHandWeapon(void)
{
	AHZWeaponData weaponData;
    auto          pPC = RE::PlayerCharacter::GetSingleton();
	if (pPC)
	{
        auto list = pPC->GetInventoryChanges()->entryList;
        auto equippedItem = pPC->GetEquippedObject(true);

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
                            weaponData.weapon == DYNAMIC_CAST(weaponData.equipData.boundObject, RE::TESForm, RE::TESObjectWEAP);
                        }

                        return weaponData;
                    }
                }
            }
        }
	}
	return weaponData;
}

AHZWeaponData CAHZWeaponInfo::GetRightHandWeapon(void)
{
    AHZWeaponData weaponData;
    auto          pPC = RE::PlayerCharacter::GetSingleton();
    if (pPC) {
        auto list = pPC->GetInventoryChanges()->entryList;
        auto equippedItem = pPC->GetEquippedObject(false);

        for (auto it = list->begin(); it != list->end(); ++it) {
            auto entry = *it;
            if (entry->object->GetFormID() == equippedItem->formID) {
                for (auto entryListIT = entry->extraLists->begin(); entryListIT != entry->extraLists->end(); ++entryListIT) {
                    auto extraData = *entryListIT;
                    if (extraData &&
                        extraData->HasType(RE::ExtraDataType::kWorn)) {
                        weaponData.equipData.boundObject = entry->object;
                        weaponData.equipData.pExtraData = extraData;

                        if (weaponData.equipData.boundObject) {
                            weaponData.weapon == DYNAMIC_CAST(weaponData.equipData.boundObject, RE::TESForm, RE::TESObjectWEAP);
                        }

                        return weaponData;
                    }
                }
            }
        }
    }
    return weaponData;
}

AHZWeaponData CAHZWeaponInfo::GetEquippedAmmo(void)
{
    AHZWeaponData ammoData;
    auto          pPC = RE::PlayerCharacter::GetSingleton();
    if (pPC) {
        auto list = pPC->GetInventoryChanges()->entryList;

        for (auto it = list->begin(); it != list->end(); ++it) {
            auto entry = *it;
            if (entry->object->GetFormType() == RE::FormType::Ammo) {
                for (auto entryListIT = entry->extraLists->begin(); entryListIT != entry->extraLists->end(); ++entryListIT) {
                    auto extraData = *entryListIT;
                    if (extraData &&
                        extraData->HasType(RE::ExtraDataType::kWorn)) {
                        ammoData.equipData.boundObject = entry->object;
                        ammoData.equipData.pExtraData = extraData;

                        if (ammoData.equipData.boundObject) {
                            ammoData.ammo == DYNAMIC_CAST(ammoData.equipData.boundObject, RE::TESForm, RE::TESAmmo);
                            return ammoData;
                        }                      
                    }
                }
            }
        }
    }

	return ammoData;
}

//bool CAHZWeaponInfo::IsProjectileWithSource(TESObjectREFR * objectRef)
//{
//    if (!objectRef || !objectRef->baseForm || objectRef->baseForm->GetFormType() != kFormType_Projectile)
//        return false;
//
//    Projectile *pProjectile = (DYNAMIC_CAST(objectRef, TESObjectREFR, Projectile));
//
//    if (pProjectile) {
//        AHZProjectile *a = (AHZProjectile*)(pProjectile);
//        if (a && a->sourceAmmo)
//            return true;
//    }
//
//    return false;
//}