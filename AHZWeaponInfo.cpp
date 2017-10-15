#include "skse64/PluginAPI.h"
#include "skse64/skse64_common/skse_version.h"
#include "skse64/skse64_common/SafeWrite.h"
#include "skse64/ScaleformCallbacks.h"
#include "skse64/ScaleformMovie.h"
#include "skse64/GameAPI.h"
#include "skse64/GameData.h"
#include "skse64/GameObjects.h"
#include "skse64/PapyrusNativeFunctions.h"
#include "skse64/Hooks_Papyrus.h"
#include "skse64/GameTypes.h"
#include "skse64/GameReferences.h"
#include "skse64/GameFormComponents.h"
#include "skse64/GameForms.h"
#include "skse64/GameRTTI.h"
#include "skse64/GameMenus.h"
#include "skse64/PapyrusUI.h"
#include "skse64/PapyrusVM.h"
#include "skse64/PapyrusEvents.h"
#include "skse64/GameExtraData.h"
#include <list>
#include <algorithm>
#include "PluginProcessInfo.h"
#include "IngredientLUT.h"
#include "skse64/PapyrusObjectReference.h"
#include "AHZWeaponInfo.h"


class ContainerAmmoVistor
{	
public:
	AHZWeaponData ammoData;
	ContainerAmmoVistor()
	{ }
	
	bool Accept(InventoryEntryData* pEntryData)
	{
		if (pEntryData && pEntryData->type && pEntryData->type->GetFormType() == kFormType_Ammo)
		{
			if (pEntryData->extendDataList)
			{
				UInt32 count = pEntryData->extendDataList->Count();
				for (int i = 0; i < count; i++)
				{
					BaseExtraList * extraList = pEntryData->extendDataList->GetNthItem(i);
					if (extraList->HasType(kExtraData_Worn))
					{
						ammoData.equipData.pForm = pEntryData->type;
						ammoData.equipData.pExtraData = extraList;
						ammoData.ammo = DYNAMIC_CAST(ammoData.equipData.pForm, TESForm, TESAmmo);
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

CAHZWeaponInfo::CAHZWeaponInfo(void)
{
}

CAHZWeaponInfo::~CAHZWeaponInfo(void)
{
}

AHZWeaponData CAHZWeaponInfo::GetWeaponInfo(TESObjectREFR * thisObject)
{
	AHZWeaponData weaponData;

	// Must be a weapon
	if (!thisObject)
		return weaponData;

	if (thisObject->baseForm->GetFormType() != kFormType_Weapon &&
		thisObject->baseForm->GetFormType() != kFormType_Ammo)
	{
		return weaponData;
	}

	weaponData.equipData.pForm = thisObject->baseForm;
	weaponData.equipData.pExtraData = &thisObject->extraData;

	if (thisObject->baseForm->GetFormType() == kFormType_Weapon)
		weaponData.weapon = DYNAMIC_CAST(weaponData.equipData.pForm, TESForm, TESObjectWEAP);
	else if (thisObject->baseForm->GetFormType() == kFormType_Ammo)
		weaponData.ammo = DYNAMIC_CAST(weaponData.equipData.pForm, TESForm, TESAmmo);
	return weaponData;
}


AHZWeaponData CAHZWeaponInfo::GetLeftHandWeapon(void)
{
	AHZWeaponData weaponData;
	PlayerCharacter* pPC = (*g_thePlayer);
	if (pPC)
	{
		TESForm * tempItem = pPC->GetEquippedObject(true);
		if (tempItem && tempItem->GetFormType() == kFormType_Weapon)
		{	
			MatchByForm matcher(tempItem);
			ExtraContainerChanges* containerChanges = static_cast<ExtraContainerChanges*>(pPC->extraData.GetByType(kExtraData_ContainerChanges));
			if (!containerChanges)
				return weaponData;
			weaponData.equipData = containerChanges->FindEquipped(matcher, true, true);
			if (weaponData.equipData.pForm)
				weaponData.weapon = DYNAMIC_CAST(weaponData.equipData.pForm, TESForm, TESObjectWEAP);
			return weaponData;
		}
	}
	return weaponData;
}

AHZWeaponData CAHZWeaponInfo::GetRightHandWeapon(void)
{
	AHZWeaponData weaponData;
	PlayerCharacter* pPC = (*g_thePlayer);
	if (pPC)
	{
		TESForm * tempItem = pPC->GetEquippedObject(false);
		if (tempItem && tempItem->GetFormType() == kFormType_Weapon)
		{	
			MatchByForm matcher(tempItem);
			ExtraContainerChanges* containerChanges = static_cast<ExtraContainerChanges*>(pPC->extraData.GetByType(kExtraData_ContainerChanges));
			if (!containerChanges)
				return weaponData;
			weaponData.equipData = containerChanges->FindEquipped(matcher, true, false);
			if (weaponData.equipData.pForm)
				weaponData.weapon = DYNAMIC_CAST(weaponData.equipData.pForm, TESForm, TESObjectWEAP);
			return weaponData;
		}
	}
	weaponData.equipData.pExtraData = NULL;
	weaponData.equipData.pForm = NULL;
	weaponData.weapon = NULL;
	weaponData.ammo = NULL;
	return weaponData;
}

AHZWeaponData CAHZWeaponInfo::GetEquippedAmmo(void)
{
	AHZWeaponData weaponData;

	PlayerCharacter* pPC = (*g_thePlayer);
	if (pPC)
	{			
		ExtraContainerChanges* containerChanges = static_cast<ExtraContainerChanges*>(pPC->extraData.GetByType(kExtraData_ContainerChanges));
		if (!containerChanges)
			return weaponData;

		ContainerAmmoVistor visitor;
		containerChanges->data->objList->Visit(visitor);
		if (visitor.ammoData.ammo)
		{
			return visitor.ammoData;
		}
	}
	return weaponData;
}



