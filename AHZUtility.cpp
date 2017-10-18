#include <wchar.h> 
#include <stdlib.h>
#include <list>
#include <algorithm>
#include "skse64/GameReferences.h"
#include "skse64/GameObjects.h"
#include "skse64/GameRTTI.h"
#include "skse64/GameSettings.h"
#include "skse64/GameExtraData.h"
#include "skse64/ScaleformCallbacks.h"
#include "skse64/ScaleformMovie.h"
#include "skse64/skse64_common/Utilities.h"
#include "AHZArmorInfo.h"
#include "AHZWeaponInfo.h"
#include "IngredientLUT.h"
#include "FoodLUT.h"
#include "AHZUtility.h"

RelocAddr<_IsSurvivalMode> IsSurvivalMode(0x008D9220);
RelocAddr<GET_MAGIC_ITEM_DESCRIPTION> GetMagicItemDescription2(0x891320);
RelocAddr<PROCESS_SURVIVAL_MODE> ProcessSurvivalMode(0x891740);



float GetBaseDamage(TESAmmo* pthisAmmo)
{
	return pthisAmmo->settings.damage;
}

double GetActualDamage(AHZWeaponData *weaponData)
{	
	if (!weaponData)
		return 0.0;

	PlayerCharacter* pPC = (*g_thePlayer);

	if(pPC) 
	{
		InventoryEntryData objDesc(weaponData->equipData.pForm, 0);

		// Allocate a list to send
		objDesc.extendDataList = new tList<BaseExtraList>();

		if (weaponData->equipData.pExtraData)
		{
			objDesc.extendDataList->Insert(weaponData->equipData.pExtraData);
		}

		float fDamage = CALL_MEMBER_FN(pPC, GetDamage)(&objDesc);
		
		// Delete the allocated dummy list
		delete objDesc.extendDataList;
		
		// This could be rounded, but the the script decide
		return mRound(fDamage);
	}

	return 0.0;
}

const char * GetTargetName(TESObjectREFR *thisObject)
{
	PlayerCharacter* pPC = (*g_thePlayer);

	if(pPC) 
	{
		const char * test2 = thisObject->extraData.GetDisplayName(thisObject->baseForm);
		InventoryEntryData objDesc(thisObject->baseForm, 0);
		objDesc.extendDataList = new tList<BaseExtraList>();
		objDesc.extendDataList->Insert(&thisObject->extraData);
		const char * test = CALL_MEMBER_FN(&objDesc, GenerateName)();
		delete objDesc.extendDataList;
		return test;
	}
	return NULL;
}

double GetActualArmorRating(AHZArmorData* armorData)
{	
	if (!armorData->armor)
		return 0.0;

	PlayerCharacter* pPC = (*g_thePlayer);

	if(pPC) 
	{
		InventoryEntryData objDesc(armorData->equipData.pForm, 0);
		// Allocate a dummy list so skyrim does not crash. For armor information
		// skyrim doesn't appear to need the list
		objDesc.extendDataList = new tList<BaseExtraList>();
		if (armorData->equipData.pExtraData)
		{
			objDesc.extendDataList->Insert(armorData->equipData.pExtraData);
		}

		double fRating = CALL_MEMBER_FN(pPC, GetArmorValue)(&objDesc);
		
		// Delete the allocated dummy list
		delete objDesc.extendDataList;
		
		// This could be rounded, but the the script decide
		return mRound(fRating);
	}

	return 0.0;
}


double GetTotalActualArmorRating(void)
{
	double totalRating = 0.0;


	// Keep a list of items to make sure they are not added more than once
	// Some items take up more than one slot
	std::list<TESForm*> clist;
	for (UInt64 slot = 1; slot <= (UInt64)0x2000; slot <<= 1)
	{
		AHZArmorData armorData = CAHZArmorInfo::GetArmorFromSlotMask(slot);
		if (armorData.equipData.pForm)
		{
			if (find(clist.begin(), clist.end(),armorData.equipData.pForm) == clist.end())
			{
				clist.push_front(armorData.equipData.pForm);
				if (armorData.armor)
				{
					totalRating += GetActualArmorRating(&armorData);
				}
			}
		}
	}
	return mRound(totalRating);
}

double mRound(double r)
{
	return (r >= 0.0) ? floor(r + 0.5) : ceil(r - 0.5);
}

double GetArmorRatingDiff(TESObjectREFR *thisArmor)
{
	UInt64 slot = 1;
	float oldArmorRating = 0.0;
	float newArmorRating = 0.0;
	float oldTotalArmorRating = 0.0;
	float newTotalArmorRating = 0.0;
	double deltaRating = 0.0;
	if (!thisArmor)
		return 0.0;
	
	// Get the new armor rating
	AHZArmorData armorData(thisArmor);
	if (!armorData.armor)
		return 0.0;

	newArmorRating = GetActualArmorRating(&armorData);
	
	// Get the armor rating from the armor that shares the same slot
	AHZArmorData sameSlotData = CAHZArmorInfo::GetArmorFromSlotMask(
		armorData.armor->bipedObject.GetSlotMask());
	if (sameSlotData.armor)
	{
		oldArmorRating = GetActualArmorRating(&sameSlotData);
	}
	
	// Get the total
	oldTotalArmorRating = GetTotalActualArmorRating();	

	newTotalArmorRating = (oldTotalArmorRating - oldArmorRating) + newArmorRating;

	deltaRating = newTotalArmorRating - oldTotalArmorRating;

	return deltaRating;
}

double GetTotalActualWeaponDamage(void)
{
	float totalWeaponDamage = 0.0;
	bool is2Handed = FALSE;
	AHZWeaponData leftWeapon = CAHZWeaponInfo::GetLeftHandWeapon();
	AHZWeaponData rightWeapon = CAHZWeaponInfo::GetRightHandWeapon();
	AHZWeaponData equippedAmmo = CAHZWeaponInfo::GetEquippedAmmo();
	
	if (leftWeapon.weapon)
	{
		if (IsBow(leftWeapon.weapon))
		{
			is2Handed = TRUE;
			totalWeaponDamage = GetActualDamage(&leftWeapon);

			// Add the arrow damage
			if (equippedAmmo.ammo && !isBolt(equippedAmmo.ammo))
			{			
				totalWeaponDamage += GetActualDamage(&equippedAmmo);
			}
		}
		else if (IsCrossBow(leftWeapon.weapon))
		{
			is2Handed = TRUE;
			totalWeaponDamage = GetActualDamage(&leftWeapon);

			// Add the arrow damage
			if (equippedAmmo.ammo && isBolt(equippedAmmo.ammo))
			{				
				totalWeaponDamage += GetActualDamage(&equippedAmmo);
			}
		}
		else if (IsTwoHanded(leftWeapon.weapon))
		{
			is2Handed = TRUE;
			totalWeaponDamage = GetActualDamage(&leftWeapon);
		}
		else if (IsOneHanded(leftWeapon.weapon))
		{
			totalWeaponDamage = GetActualDamage(&leftWeapon);
		}
	}

	if (rightWeapon.weapon)
	{
		if (IsBow(rightWeapon.weapon) && !is2Handed)
		{
			is2Handed = TRUE;
			totalWeaponDamage = GetActualDamage(&rightWeapon);

			// Add the arrow damage
			if (equippedAmmo.ammo && !isBolt(equippedAmmo.ammo))
			{			
				totalWeaponDamage += GetActualDamage(&equippedAmmo);
			}
		}
		else if (IsCrossBow(rightWeapon.weapon) && !is2Handed)
		{
			is2Handed = TRUE;
			totalWeaponDamage = GetActualDamage(&rightWeapon);

			// Add the arrow damage
			if (equippedAmmo.ammo && isBolt(equippedAmmo.ammo))
			{
				totalWeaponDamage += GetActualDamage(&equippedAmmo);
			}
		}
		else if (IsTwoHanded(rightWeapon.weapon) && !is2Handed)
		{
			is2Handed = TRUE;
			totalWeaponDamage = GetActualDamage(&rightWeapon);
		}
		else if (IsOneHanded(rightWeapon.weapon))
		{
			// Add the damage from the second weapon
			totalWeaponDamage += GetActualDamage(&rightWeapon);
		}
	}
	return totalWeaponDamage;
}

bool isBolt(TESAmmo *thisAmmo) 
{ 
	return (thisAmmo->settings.flags >= 0 && thisAmmo->settings.flags <= 3); 
}

double GetWeaponDamageDiff(TESObjectREFR *targetWeaponOrAmmo)
{
	double totalWeaponDamage = 0.0;
	double targetArrowDamage = 0.0;
	AHZWeaponData leftWeapon;
	AHZWeaponData rightWeapon;
	AHZWeaponData targetWeapon;
	AHZWeaponData equippedAmmo;
	
	if (!targetWeaponOrAmmo)
		return 0.0;
	
	targetWeapon = CAHZWeaponInfo::GetWeaponInfo(targetWeaponOrAmmo);
	equippedAmmo = CAHZWeaponInfo::GetEquippedAmmo();

	// Must be a weapon or armor targeted
	if (targetWeapon.weapon)
	{
		if (equippedAmmo.ammo)
		{
			// TODO Try to get the equipped arrow with extra data
			if (IsBow(targetWeapon.weapon) && !isBolt(equippedAmmo.ammo))
			{
				targetArrowDamage = GetActualDamage(&equippedAmmo);
			}
			else if (IsCrossBow(targetWeapon.weapon) && isBolt(equippedAmmo.ammo))
			{
				targetArrowDamage = GetActualDamage(&equippedAmmo);
			}
		}
	}
	else if (targetWeapon.ammo)
	{
		//targetAmmo = targetWeapon.ammo;
	}
	else 
	{
		return 0.0;
	}
	
	rightWeapon = CAHZWeaponInfo::GetRightHandWeapon();
	leftWeapon = CAHZWeaponInfo::GetLeftHandWeapon();

	if (leftWeapon.weapon)
	{
		if (IsBow(leftWeapon.weapon))
		{
			float tempDamage = GetActualDamage(&leftWeapon);
			float tempArrowDamage = 0.0;
			// Add the arrow damage
			if (equippedAmmo.ammo && !isBolt(equippedAmmo.ammo))
			{			
				tempArrowDamage = GetActualDamage(&equippedAmmo);
				tempDamage += tempArrowDamage;
			}

			if (targetWeapon.weapon)
			{
				return (GetActualDamage(&targetWeapon) + targetArrowDamage) - tempDamage;
			}
			else if (targetWeapon.ammo && !isBolt(targetWeapon.ammo))
			{
				return ((tempDamage - tempArrowDamage) + GetActualDamage(&targetWeapon)) - tempDamage;
			}
			else 
			{
				return 0.0;
			}
		}
		else if (IsCrossBow(leftWeapon.weapon))
		{
			float tempDamage = GetActualDamage(&leftWeapon);
			float tempArrowDamage = 0.0;
			// Add the arrow damage
			if (equippedAmmo.ammo && isBolt(equippedAmmo.ammo))
			{				
				tempArrowDamage = GetActualDamage(&equippedAmmo);
				tempDamage += tempArrowDamage;
			}

			if (targetWeapon.weapon)
			{
				return (GetActualDamage(&targetWeapon) + targetArrowDamage) - tempDamage;
			}
			else if (targetWeapon.ammo && isBolt(targetWeapon.ammo))
			{
				return ((tempDamage - tempArrowDamage) + GetActualDamage(&targetWeapon) - tempDamage);
			}
			else 
			{
				return 0.0;
			}
		}
		else if (IsTwoHanded(leftWeapon.weapon) && targetWeapon.weapon)
		{
			return (GetActualDamage(&targetWeapon) + targetArrowDamage) - GetActualDamage(&leftWeapon);
		}
		else if (IsOneHanded(leftWeapon.weapon))
		{
			totalWeaponDamage = GetActualDamage(&leftWeapon);
		}
	}

	if (rightWeapon.weapon)
	{
		if (IsOneHanded(rightWeapon.weapon))
		{
			// Add the damage from the second weapon
			totalWeaponDamage += GetActualDamage(&rightWeapon);
		}
	}


	// If we made it this far either no weapon is equipped or single handed weapons are equipped
	if (targetWeapon.weapon)
	{
		return (GetActualDamage(&targetWeapon) + targetArrowDamage) - totalWeaponDamage;
	}
	
	return 0.0;
}

bool IsTwoHanded(TESObjectWEAP * thisWeapon)
{
			//kType_HandToHandMelee = 0,
			//kType_OneHandSword,
			//kType_OneHandDagger,
			//kType_OneHandAxe,
			//kType_OneHandMace,
			//kType_TwoHandSword,
			//kType_TwoHandAxe,
			//kType_Bow,
			//kType_Staff,
			//kType_CrossBow,
			//kType_H2H,
			//kType_1HS,
			//kType_1HD,
			//kType_1HA,
			//kType_1HM,
			//kType_2HS,
			//kType_2HA,
			//kType_Bow2,
			//kType_Staff2,
			//kType_CBow	
	return (thisWeapon->type() == TESObjectWEAP::GameData::kType_TwoHandSword ||
			thisWeapon->type() == TESObjectWEAP::GameData::kType_TwoHandAxe ||
			thisWeapon->type() == TESObjectWEAP::GameData::kType_Bow ||
			thisWeapon->type() == TESObjectWEAP::GameData::kType_CrossBow ||
			thisWeapon->type() == TESObjectWEAP::GameData::kType_2HS ||
			thisWeapon->type() == TESObjectWEAP::GameData::kType_2HA ||
			thisWeapon->type() == TESObjectWEAP::GameData::kType_Bow2 ||
			thisWeapon->type() == TESObjectWEAP::GameData::kType_CBow);
}

bool IsOneHanded(TESObjectWEAP * thisWeapon)
{
			//kType_HandToHandMelee = 0,
			//kType_OneHandSword,
			//kType_OneHandDagger,
			//kType_OneHandAxe,
			//kType_OneHandMace,
			//kType_TwoHandSword,
			//kType_TwoHandAxe,
			//kType_Bow,
			//kType_Staff,
			//kType_CrossBow,
			//kType_H2H,
			//kType_1HS,
			//kType_1HD,
			//kType_1HA,
			//kType_1HM,
			//kType_2HS,
			//kType_2HA,
			//kType_Bow2,
			//kType_Staff2,
			//kType_CBow	
	return (thisWeapon->type() == TESObjectWEAP::GameData::kType_OneHandSword ||
			thisWeapon->type() == TESObjectWEAP::GameData::kType_OneHandDagger ||
			thisWeapon->type() == TESObjectWEAP::GameData::kType_OneHandAxe ||
			thisWeapon->type() == TESObjectWEAP::GameData::kType_OneHandMace ||
			thisWeapon->type() == TESObjectWEAP::GameData::kType_1HS ||
			thisWeapon->type() == TESObjectWEAP::GameData::kType_1HD ||
			thisWeapon->type() == TESObjectWEAP::GameData::kType_1HA ||
			thisWeapon->type() == TESObjectWEAP::GameData::kType_1HM);
}

bool IsBowType(TESObjectWEAP * thisWeapon)
{
			//kType_HandToHandMelee = 0,
			//kType_OneHandSword,
			//kType_OneHandDagger,
			//kType_OneHandAxe,
			//kType_OneHandMace,
			//kType_TwoHandSword,
			//kType_TwoHandAxe,
			//kType_Bow,
			//kType_Staff,
			//kType_CrossBow,
			//kType_H2H,
			//kType_1HS,
			//kType_1HD,
			//kType_1HA,
			//kType_1HM,
			//kType_2HS,
			//kType_2HA,
			//kType_Bow2,
			//kType_Staff2,
			//kType_CBow	
	return (thisWeapon->type() == TESObjectWEAP::GameData::kType_Bow ||
			thisWeapon->type() == TESObjectWEAP::GameData::kType_CrossBow ||
			thisWeapon->type() == TESObjectWEAP::GameData::kType_Bow2 ||
			thisWeapon->type() == TESObjectWEAP::GameData::kType_CBow);
}

bool IsBow(TESObjectWEAP * thisWeapon)
{
			//kType_HandToHandMelee = 0,
			//kType_OneHandSword,
			//kType_OneHandDagger,
			//kType_OneHandAxe,
			//kType_OneHandMace,
			//kType_TwoHandSword,
			//kType_TwoHandAxe,
			//kType_Bow,
			//kType_Staff,
			//kType_CrossBow,
			//kType_H2H,
			//kType_1HS,
			//kType_1HD,
			//kType_1HA,
			//kType_1HM,
			//kType_2HS,
			//kType_2HA,
			//kType_Bow2,
			//kType_Staff2,
			//kType_CBow	
	return (thisWeapon->type() == TESObjectWEAP::GameData::kType_Bow ||
			thisWeapon->type() == TESObjectWEAP::GameData::kType_Bow2);
}

bool IsCrossBow(TESObjectWEAP * thisWeapon)
{
			//kType_HandToHandMelee = 0,
			//kType_OneHandSword,
			//kType_OneHandDagger,
			//kType_OneHandAxe,
			//kType_OneHandMace,
			//kType_TwoHandSword,
			//kType_TwoHandAxe,
			//kType_Bow,
			//kType_Staff,
			//kType_CrossBow,
			//kType_H2H,
			//kType_1HS,
			//kType_1HD,
			//kType_1HA,
			//kType_1HM,
			//kType_2HS,
			//kType_2HA,
			//kType_Bow2,
			//kType_Staff2,
			//kType_CBow	
	return (thisWeapon->type() == TESObjectWEAP::GameData::kType_CrossBow ||
			thisWeapon->type() == TESObjectWEAP::GameData::kType_CBow);
}





//bool GetIsNthEffectKnown(IngredientItem* thisMagic, UInt32 index)
//{
//	bool isKnown = false;
//	
//	enum	// type - these are flags
//	{
//		kType_NoEffect	=		0,
//		kType_FirstEffect =		1 << 0,
//		kType_SecondEffect =	1 << 1,
//		kType_ThirdEffect =		1 << 2,
//		kType_FourthEffect =	1 << 3
//	};
//	
//	if (!thisMagic)
//		return false;
//	switch (index)
//	{
//	case 0:
//		isKnown = (((thisMagic->unkA8.unk00 & 0xFF) & kType_FirstEffect)== kType_FirstEffect);
//		break;
//	case 1:
//		isKnown = (((thisMagic->unkA8.unk00 & 0xFF) & kType_SecondEffect) == kType_SecondEffect);
//		break;
//	case 2:
//		isKnown = (((thisMagic->unkA8.unk00 & 0xFF) & kType_ThirdEffect) == kType_ThirdEffect);
//		break;
//	case 3:
//		isKnown = (((thisMagic->unkA8.unk00 & 0xFF) & kType_FourthEffect) == kType_FourthEffect);
//		break;
//	default:
//		break;
//	}
//	return isKnown;
//}

IngredientItem* GetIngredient(TESObjectREFR *thisObject)
{
	if (!thisObject)
		return NULL;
	
	if (thisObject->baseForm->GetFormType() == kFormType_Ingredient)
		return DYNAMIC_CAST(thisObject->baseForm, TESForm, IngredientItem);

	if (thisObject->baseForm->GetFormType() == kFormType_Flora)
	{
		TESFlora *flora = DYNAMIC_CAST(thisObject->baseForm, TESForm, TESFlora);
		if (flora)
		{
			TESForm *form = (TESForm *)flora->produce.produce;

			// If the ingredient is actually an ingredient
			if (form->formType == kFormType_Ingredient)
			{
				return DYNAMIC_CAST(flora->produce.produce, TESForm, IngredientItem);
			}
			// If the ingredient is actually a levelitem (Harvest overhaul mod or a coin purse)
			else if (form->formType == kFormType_LeveledItem)
			{
				TESLevItem *lvli = DYNAMIC_CAST(form, TESForm, TESLevItem);
				// Get the first form and see if it is an ingredient
				if (lvli->leveledList.length > 0)
				{
					TESForm *itemform = (TESForm *)lvli->leveledList.entries[0].form;
					if (itemform)
					{
						IngredientItem *ingredient = DYNAMIC_CAST(itemform, TESForm, IngredientItem);
						return ingredient;
					}
				}
			}
		}
	}
	else if (thisObject->baseForm->GetFormType() == kFormType_Tree)
	{
		TESObjectTREE *tree = DYNAMIC_CAST(thisObject->baseForm, TESForm, TESObjectTREE);
		if (tree)
		{
			TESForm *form = (TESForm *)tree->produce.produce;//DYNAMIC_CAST(tree->produce.produce, IngredientItem, TESForm);

			// If the ingredient is actually an ingredient
			if (form->formType == kFormType_Ingredient)
			{
				return DYNAMIC_CAST(tree->produce.produce, TESForm, IngredientItem);
			}
			// If the ingredient is actually a levelitem (Harvest overhaul mod or a coin purse)
			else if (form->formType == kFormType_LeveledItem)
			{
				TESLevItem *lvli = DYNAMIC_CAST(form, TESForm, TESLevItem);
				// Get the first form and see if it is an ingredient
				if (lvli->leveledList.length > 0)
				{
					TESForm *itemform = (TESForm *)lvli->leveledList.entries[0].form;
					if (itemform)
					{
						IngredientItem *ingredient = DYNAMIC_CAST(itemform, TESForm, IngredientItem);
						return ingredient;
					}
				}
			}
		}
	}
	else if (thisObject->baseForm->GetFormType() == kFormType_Activator)
	{
		CIngredientLUT lut;
		return lut.GetIngredient(thisObject->baseForm->formID);
	}

	return NULL;
}

AlchemyItem* GetFood(TESObjectREFR *thisObject)
{
	if (!thisObject)
		return NULL;
	
	if (thisObject->baseForm->GetFormType() == kFormType_Potion)
		return DYNAMIC_CAST(thisObject->baseForm, TESForm, AlchemyItem);

	if (thisObject->baseForm->GetFormType() == kFormType_Flora)
	{
		TESFlora *flora = DYNAMIC_CAST(thisObject->baseForm, TESForm, TESFlora);
		if (flora)
		{
			TESForm *form = (TESForm *)flora->produce.produce;

			// If the food is actually food
			if (form->formType == kFormType_Potion)
			{
				return DYNAMIC_CAST(form, TESForm, AlchemyItem);
			}
			// If the food is actually a levelitem (Harvest overhaul mod or a coin purse)
			else if (form->formType == kFormType_LeveledItem)
			{
				TESLevItem *lvli = DYNAMIC_CAST(form, TESForm, TESLevItem);
				// Get the first form and see if it is food
				if (lvli->leveledList.length > 0)
				{
					TESForm *itemform = (TESForm *)lvli->leveledList.entries[0].form;
					if (itemform)
					{
						AlchemyItem *alchmyItem = DYNAMIC_CAST(itemform, TESForm, AlchemyItem);
						return alchmyItem;
					}
				}
			}
		}
	}
	else if (thisObject->baseForm->GetFormType() == kFormType_Tree)
	{
		TESObjectTREE *tree = DYNAMIC_CAST(thisObject->baseForm, TESForm, TESObjectTREE);
		if (tree)
		{
			TESForm *form = (TESForm *)tree->produce.produce;

			// If the produce is actually food
			if (form->formType == kFormType_Potion)
			{
				return DYNAMIC_CAST(form, TESForm, AlchemyItem);
			}
			// If the ingredient is actually a levelitem (Harvest overhaul mod or a coin purse)
			else if (form->formType == kFormType_LeveledItem)
			{
				TESLevItem *lvli = DYNAMIC_CAST(form, TESForm, TESLevItem);
				// Get the first form and see if it is food
				if (lvli->leveledList.length > 0)
				{
					TESForm *itemform = (TESForm *)lvli->leveledList.entries[0].form;
					if (itemform)
					{
						AlchemyItem *alchmyItem = DYNAMIC_CAST(itemform, TESForm, AlchemyItem);
						return alchmyItem;
					}
				}
			}
		}
	}
	else if (thisObject->baseForm->GetFormType() == kFormType_Activator)
	{
		CFoodLUT lut;
		return lut.GetFood(thisObject->baseForm->formID);
	}

	return NULL;
}

bool CanPickUp(UInt32 formType)
{
	return (formType == kFormType_Weapon || 
			formType == kFormType_Armor || 
			formType == kFormType_SoulGem ||
			formType == kFormType_Potion ||
			formType == kFormType_Misc ||
			formType == kFormType_Ingredient ||
			formType == kFormType_Book ||
			formType == kFormType_Ammo ||
			formType == kFormType_ScrollItem ||
			formType == kFormType_LeveledItem ||
			formType == kFormType_Outfit ||
			formType == kFormType_Key); 
}


string CAHZUtility::GetTargetName(TESObjectREFR *thisObject)
{
	string name;
	TESFullName* pFullName = DYNAMIC_CAST(thisObject->baseForm, TESForm, TESFullName);
	const char* displayName = thisObject->extraData.GetDisplayName(thisObject->baseForm);

	// If the name can be created
	if (displayName)
	{
		name.append(displayName);
	}
	// Use the base name
	else if (pFullName)
	{
		name.append(pFullName->name.data);
	}

	// If this is a soul gem, also get the gem size name
	if (thisObject->baseForm->formType == kFormType_SoulGem)
	{
		TESSoulGem *gem = DYNAMIC_CAST(thisObject->baseForm, TESForm, TESSoulGem);
		if (gem)
		{
			char * soulName = NULL;
			SettingCollectionMap	* settings = *g_gameSettingCollection;
			switch (gem->soulSize)
			{
			case 1: soulName = settings->Get("sSoulLevelNamePetty")->data.s; break;
			case 2: soulName = settings->Get("sSoulLevelNameLesser")->data.s; break;
			case 3: soulName = settings->Get("sSoulLevelNameCommon")->data.s; break;
			case 4: soulName = settings->Get("sSoulLevelNameGreater")->data.s; break;
			case 5: soulName = settings->Get("sSoulLevelNameGrand")->data.s; break;
			default: break;
			}

			if (soulName)
			{
				name.append(" (");
				name.append(soulName);
				name.append(")");
			}
		}
	}

	return name;
};

bool CAHZUtility::GetIsBookAndWasRead(TESObjectREFR *theObject)
{
	if (!theObject)
		return false;

	if (theObject->baseForm->GetFormType() != kFormType_Book)
		return false;

	TESObjectBOOK *item = DYNAMIC_CAST(theObject->baseForm, TESForm, TESObjectBOOK);
	if (item && ((item->data.flags & TESObjectBOOK::Data::kType_Read) == TESObjectBOOK::Data::kType_Read))
	{
		return true;
	}
	else
	{
		return false;
	}
};


string CAHZUtility::GetArmorWeightClass(TESObjectREFR *theObject)
{
	string desc;

	if (!theObject)
		return desc;

	if (theObject->baseForm->GetFormType() != kFormType_Armor)
		return desc;

	TESObjectARMO *item = DYNAMIC_CAST(theObject->baseForm, TESForm, TESObjectARMO);
	if (!item)
		return desc;

	ActorValueList * avList = ActorValueList::GetSingleton();
	if (avList && item->bipedObject.data.weightClass <= 1)
	{
		// Utilize the AV value to get the localized name for "Light Armor"
		if (item->bipedObject.data.weightClass == 0)
		{
			ActorValueInfo * info = avList->GetActorValue(12);
			if (info)
			{
				TESFullName *fname = DYNAMIC_CAST(info, ActorValueInfo, TESFullName);
				if (fname && fname->name.data)
				{
					desc.append("<FONT FACE=\"$EverywhereMediumFont\"SIZE=\"15\"COLOR=\"#999999\"KERNING=\"0\">     ");
					desc.append(fname->name.data);
					desc.append("<\\FONT>");
				}
			}
		}
		// Utilize the AV value to get the localized name for "Heavy Armor"
		else if (item->bipedObject.data.weightClass == 1)
		{
			ActorValueInfo * info = avList->GetActorValue(11);
			if (info)
			{
				TESFullName *fname = DYNAMIC_CAST(info, ActorValueInfo, TESFullName);
				if (fname && fname->name.data)
				{
					desc.append("<FONT FACE=\"$EverywhereMediumFont\"SIZE=\"15\"COLOR=\"#999999\"KERNING=\"0\">     ");
					desc.append(fname->name.data);
					desc.append("<\\FONT>");
				}
			}
		}
	}
	return desc;
};

string CAHZUtility::GetValueToWeight(TESObjectREFR *theObject)
{
	string desc;

	if (!theObject)
		return desc;

	if (!theObject->baseForm)
		return desc;

	//<TEXTFORMAT INDENT="0" LEFTMARGIN="0" RIGHTMARGIN="0" LEADING="2"><P ALIGN="CENTER"><FONT FACE="$EverywhereMediumFont" SIZE="15" COLOR="#999999" KERNING="0">WEIGHT </FONT><FONT FACE="$EverywhereBoldFont" SIZE="24" COLOR="#FFFFFF" KERNING="0">0.5</FONT><FONT FACE="$EverywhereMediumFont" SIZE="15" COLOR="#999999" KERNING="0">      VALUE </FONT><FONT FACE="$EverywhereBoldFont" SIZE="24" COLOR="#FFFFFF" KERNING="0">21</FONT></P></TEXTFORMAT>


	//double value = 0.0;
	//bool gotValue = false;

	//TESValueForm* pValue = DYNAMIC_CAST(theObject->baseForm, TESForm, TESValueForm);
	//if (pValue)
	//{
	//	value = pValue->value;
	//	gotValue = true;
	//}
	//else 
	//{
	//	AlchemyItem* alchemyItem = DYNAMIC_CAST(theObject->baseForm, TESForm, AlchemyItem);
	//	if (alchemyItem)// && (alchemyItem->itemData.flags & AlchemyItem::kFlag_ManualCalc) == AlchemyItem::kFlag_ManualCalc)
	//	{
	//		value = alchemyItem->itemData.value;
	//		gotValue = true;
	//	}
	//}

	//if (!gotValue)
	//	return desc;


	//double weight = GetFormWeight(theObject->baseForm);

	//if (weight <= 0.0)
	//	return desc;

	// Add the VW label
	desc.append("<FONT FACE=\"$EverywhereMediumFont\"SIZE=\"15\"COLOR=\"#999999\"KERNING=\"0\">     ");
	desc.append("VW");
	desc.append("<\\FONT>");
	desc.append(" ");

	//double vW = round(value / weight);

	//char vWString[256] = "";
	//if (vW < 1.0)
	//{
	//	sprintf_s(vWString, "%.1f", vW);
	//}
	//else
	//{
	//	sprintf_s(vWString, "%.0f", vW);
	//}
	

	desc.append("<FONT FACE=\"$EverywhereBoldFont\"SIZE=\"24\"COLOR=\"#FFFFFF\"KERNING=\"0\">");
	//desc.append(vWString);
	desc.append("0");
	desc.append("<\\FONT>");

	return desc;
};

string CAHZUtility::GetBookSkill(TESObjectREFR *theObject)
{
	string desc;
	if (theObject->baseForm->GetFormType() == kFormType_Book)
	{
		TESObjectBOOK *item = DYNAMIC_CAST(theObject->baseForm, TESForm, TESObjectBOOK);

		if (!item)
			return desc;

		// If this is a spell book, then it is not a skill book
		if ((item->data.flags & TESObjectBOOK::Data::kType_Spell) == TESObjectBOOK::Data::kType_Spell)
			return desc;

		if (((item->data.flags & TESObjectBOOK::Data::kType_Skill) == TESObjectBOOK::Data::kType_Skill) &&
			item->data.teaches.skill)
		{
			ActorValueList * avList = ActorValueList::GetSingleton();
			if (avList)
			{
				ActorValueInfo * info = avList->GetActorValue(item->data.teaches.skill);
				if (info)
				{
					TESFullName *fname = DYNAMIC_CAST(info, ActorValueInfo, TESFullName);
					if (fname && fname->name.data)
					{
						desc.append("<FONT FACE=\"$EverywhereMediumFont\"SIZE=\"15\"COLOR=\"#999999\"KERNING=\"0\">       ");
						desc.append(fname->name.data);
						desc.append("<\\FONT>");
					}
				}
			}
		}
	}
	return desc;
}

void CAHZUtility::AppendDescription(TESDescription *desObj, TESForm *parent, std::string& description)
{
	BSString bsDescription;
	string tempString = "";
	CALL_MEMBER_FN(desObj, Get)(&bsDescription, parent, 'DESC');

	if (&bsDescription)
	{
		tempString.append(bsDescription.Get());

		if (tempString != "LOOKUP FAILED!")
		{
			description.append(tempString);
		}
	}
}

string CAHZUtility::GetEffectsDescription(TESObjectREFR *theObject)
{
	BSString description;
	string effectDescription;
	string  desc;
	string effectsString;
	MagicItem * magicItem = NULL;
	if (!theObject)
		return desc;

	tArray<MagicItem::EffectItem*> *effectList = NULL;
	SettingCollectionMap *settings = *g_gameSettingCollection;

	if (theObject->baseForm->GetFormType() == kFormType_Potion)
	{
		AlchemyItem *item = DYNAMIC_CAST(theObject->baseForm, TESForm, AlchemyItem);

		if (ExtraEnchantment* extraEnchant = static_cast<ExtraEnchantment*>(theObject->extraData.GetByType(kExtraData_Enchantment))) // Enchanted
		{
			if (extraEnchant->enchant)
			{
				GetMagicItemDescription(extraEnchant->enchant, effectDescription);
				desc.append(effectDescription);
			}
		}


		if (item)
		{
			GetMagicItemDescription(item, effectDescription);
			desc.append(effectDescription);
		}
	}
	else if (theObject->baseForm->GetFormType() == kFormType_Weapon)
	{
		TESObjectWEAP *item = DYNAMIC_CAST(theObject->baseForm, TESForm, TESObjectWEAP);

		//Get enchantment description
		if (item && item->enchantable.enchantment)
		{
			GetMagicItemDescription(item->enchantable.enchantment, effectDescription);
			desc.append(effectDescription);
		}
		// Items modified by the player
		else if (ExtraEnchantment* extraEnchant = static_cast<ExtraEnchantment*>(theObject->extraData.GetByType(kExtraData_Enchantment))) // Enchanted
		{
			if (extraEnchant->enchant)
			{
				GetMagicItemDescription(extraEnchant->enchant, effectDescription);
				desc.append(effectDescription);
			}
		}

		// If there was no effects, then display athe description if available
		if (item && !desc.length())
		{
			// Get the description if any (Mostly Dawnguard and Dragonborn stuff uses the descriptions)
			//CALL_MEMBER_FN(&item->description, Get)(&description, NULL, 1129530692);
			//CALL_MEMBER_FN(&item->description, Get)(&description, item, 'DESC');
			//desc.append(AHZT(description.Get()));
			AppendDescription(&item->description, item, desc);
		}
	}
	else if (theObject->baseForm->GetFormType() == kFormType_Armor)
	{
		TESObjectARMO *item = DYNAMIC_CAST(theObject->baseForm, TESForm, TESObjectARMO);

		//Get enchantment description
		if (item && item->enchantable.enchantment)
		{
			GetMagicItemDescription(item->enchantable.enchantment, effectDescription);
			desc.append(effectDescription);
		}
		// Items modified by the player
		else if (ExtraEnchantment* extraEnchant = static_cast<ExtraEnchantment*>(theObject->extraData.GetByType(kExtraData_Enchantment))) // Enchanted
		{
			if (extraEnchant->enchant)
			{
				GetMagicItemDescription(extraEnchant->enchant, effectDescription);
				desc.append(effectDescription);
			}
		}

		// If there was no effects, then display athe description if available
		if (item && !desc.length())
		{
			// Get the description if any (Mostly Dawnguard and Dragonborn stuff uses the descriptions)
			//CALL_MEMBER_FN(&item->description, Get)(&description, NULL, 1129530692);
			//CALL_MEMBER_FN(&item->description, Get)(&description, item, 'DESC');
			//desc.append(AHZT(description.Get()));
			AppendDescription(&item->description, item, desc);
		}
	}
	else if (theObject->baseForm->GetFormType() == kFormType_Ammo)
	{
		TESAmmo *item = DYNAMIC_CAST(theObject->baseForm, TESForm, TESAmmo);

		if (item)
		{
			// Get the description if any (Mostly Dawnguard and Dragonborn stuff uses the descriptions)
			//CALL_MEMBER_FN(&item->description, Get)(&description, item, 'DESC');//)1129530692);
			//desc.append(AHZT(description.Get()));
			AppendDescription(&item->description, item, desc);
		}

		if (ExtraEnchantment* extraEnchant = static_cast<ExtraEnchantment*>(theObject->extraData.GetByType(kExtraData_Enchantment))) // Enchanted
		{
			if (extraEnchant->enchant)
			{
				GetMagicItemDescription(extraEnchant->enchant, effectDescription);
				desc.append(effectDescription);
			}
		}
	}
	else if (theObject->baseForm->GetFormType() == kFormType_Book)
	{
		TESObjectBOOK *item = DYNAMIC_CAST(theObject->baseForm, TESForm, TESObjectBOOK);

		if (item)
		{
			// Get the description if any (Mostly Dawnguard and Dragonborn stuff uses the descriptions)
			//CALL_MEMBER_FN(&item->description2, Get)(&description, NULL, 1296125507);
			//CALL_MEMBER_FN(&item->description2, Get)(&description, item, 'DESC');
			//desc.append(AHZT(description.Get()));
			AppendDescription(&item->description2, item, desc);
		}

		if (item &&
			((item->data.flags & TESObjectBOOK::Data::kType_Spell) == TESObjectBOOK::Data::kType_Spell))
		{
			if (item->data.teaches.spell)
			{
				GetMagicItemDescription(item->data.teaches.spell, effectDescription);
				desc.append(effectDescription);
			}
		}
	}
	else if (theObject->baseForm->GetFormType() == kFormType_ScrollItem)
	{
		ScrollItem *item = DYNAMIC_CAST(theObject->baseForm, TESForm, ScrollItem);
		if (item)
		{
			// Get the description if any (Mostly Dawnguard and Dragonborn stuff uses the descriptions)
			//CALL_MEMBER_FN(&item->description, Get)(&description, NULL, 1129530692);
			//CALL_MEMBER_FN(&item->description, Get)(&description, item, 'DESC');
			//desc.append(AHZT(description.Get()));
			AppendDescription(&item->description, item, desc);

			GetMagicItemDescription(item, effectDescription);
			desc.append(effectDescription);



		}
	}
	return desc;
};

void CAHZUtility::ProcessTargetEffects(TESObjectREFR* targetObject, GFxFunctionHandler::Args *args)
{
	TESObjectREFR * pTargetReference = targetObject;

	// See if its harvestable food
	AlchemyItem *food = GetFood(pTargetReference);

	// If the target is not valid or it can't be picked up by the player
	if ((!pTargetReference) ||
		((!CanPickUp(pTargetReference->baseForm->GetFormType())) &&
		(!food)))
	{
		args->args[0].DeleteMember("effectsObj");
		return;
	}

	// Used to store the name
	string name;


	PlayerCharacter* pPC = (*g_thePlayer);

	if (pPC)
	{
		InventoryEntryData objDesc(targetObject->baseForm, 0);

		// Allocate a list to send
		objDesc.extendDataList = new tList<BaseExtraList>();

		objDesc.extendDataList->Insert(&targetObject->extraData);


		const char *s;
		s = CALL_MEMBER_FN(&objDesc, GenerateName)();

		float fDamage = CALL_MEMBER_FN(pPC, GetDamage)(&objDesc);

		// Delete the allocated dummy list
		delete objDesc.extendDataList;

		// This could be rounded, but the the script decide
		float ff = mRound(fDamage);
	}


	// If this is harvestable food or normal food get the magic item description
	if (food)
	{
		string effectDescription;
		GetMagicItemDescription(food, effectDescription);
		name.append(effectDescription);
	}
	else
	{
		// Get the effects description if it exists for this object
		name = this->GetEffectsDescription(pTargetReference);
	}

	// If the name contains a string
	if (name.length())
	{
		GFxValue obj;
		args->movie->CreateObject(&obj);

		RegisterString(&obj, args->movie, "effectsDescription", name.c_str());

		// Add the object to the scaleform function
		args->args[0].SetMember("effectsObj", &obj);
	}
	else
	{
		args->args[0].DeleteMember("effectsObj");
	}
};

void CAHZUtility::ProcessArmorClass(TESObjectREFR* targetObject, GFxFunctionHandler::Args *args)
{
	TESObjectREFR * pTargetReference = targetObject;
	static string weightClass;

	// If the target is not valid or it can't be picked up by the player
	if (!pTargetReference)
	{
		SetResultString(args, "");
		return;
	}

	weightClass.clear();
	weightClass.append(this->GetArmorWeightClass(pTargetReference).c_str());

	SetResultString(args, weightClass.c_str());
};

void CAHZUtility::ProcessValueToWeight(TESObjectREFR* targetObject, GFxFunctionHandler::Args *args)
{
	TESObjectREFR * pTargetReference = targetObject;
	static string valueToWeight;

	// If the target is not valid or it can't be picked up by the player
	if (!pTargetReference)
	{
		SetResultString(args, "");
		return;
	}

	valueToWeight.clear();
	valueToWeight.append(this->GetValueToWeight(pTargetReference).c_str());

	SetResultString(args, valueToWeight.c_str());
};

void CAHZUtility::ProcessBookSkill(TESObjectREFR* targetObject, GFxFunctionHandler::Args *args)
{
	TESObjectREFR * pTargetReference = targetObject;
	static string bookSkill;

	// If the target is not valid or it can't be picked up by the player
	if (!pTargetReference)
	{
		SetResultString(args, "");
		return;
	}

	bookSkill.clear();
	bookSkill.append(this->GetBookSkill(pTargetReference).c_str());

	SetResultString(args,
		bookSkill.c_str());
};

void CAHZUtility::SetResultString(GFxFunctionHandler::Args *args, const char * str)
{
	args->result->SetString(str);
};

void CAHZUtility::ReplaceStringInPlace(std::string& subject, const std::string& search,
	const std::string& replace)
{
	size_t pos = 0;
	while ((pos = subject.find(search, pos)) != std::string::npos)
	{
		subject.replace(pos, search.length(), replace);
		pos += replace.length();
	}
};

void CAHZUtility::ProcessTargetObject(TESObjectREFR* targetObject, GFxFunctionHandler::Args *args)
{
	TESObjectREFR * pTargetReference = targetObject;
	float totalArmorOrWeapon = 0.0;
	float difference = 0.0;

	// If the target is not valid or it can't be picked up by the player
	if (!CAHZUtility::ProcessValidTarget(targetObject, NULL))
	{
		args->args[0].DeleteMember("targetObj");
		return;
	}

	GFxValue obj;
	args->movie->CreateObject(&obj);

	if (pTargetReference->baseForm->GetFormType() == kFormType_Weapon ||
		pTargetReference->baseForm->GetFormType() == kFormType_Ammo)
	{
		TESForm *form = NULL;
		TESAmmo *ammo = NULL;

		// If ammo is NULL, it is OK
		totalArmorOrWeapon = GetTotalActualWeaponDamage();
		difference = GetWeaponDamageDiff(pTargetReference);
	}
	else if (pTargetReference->baseForm->GetFormType() == kFormType_Armor)
	{
		totalArmorOrWeapon = GetTotalActualArmorRating();
		difference = GetArmorRatingDiff(pTargetReference);
	}

	// Enter the data into the Scaleform function
	this->RegisterNumber(&obj, "ratingOrDamage", totalArmorOrWeapon);
	this->RegisterNumber(&obj, "difference", difference);

	float weight = CALL_MEMBER_FN(pTargetReference, GetWeight)();
	if (pTargetReference->extraData.HasType(kExtraData_Count))
	{
		ExtraCount* xCount = static_cast<ExtraCount*>(pTargetReference->extraData.GetByType(kExtraData_Count));
		if (xCount)
		{
			weight = weight * (float)(SInt16)(xCount->count & 0x7FFF);
		}
	}

	this->RegisterNumber(&obj, "objWeight", weight);

	// Used by the scaleform script to know if this is a weapon, armor, or something else
	this->RegisterNumber(&obj, "formType", pTargetReference->baseForm->GetFormType());
	args->args[0].SetMember("targetObj", &obj);
};

void CAHZUtility::ProcessIngredientData(TESObjectREFR* targetObject, GFxFunctionHandler::Args *args)
{
	TESObjectREFR * pTargetReference = targetObject;
	IngredientItem * ingredient = GetIngredient(pTargetReference);
	ofstream myfile;

	// If no ingredient, then we are done here
	if (!ingredient)
	{
		args->args[0].DeleteMember("ingredientObj");
		return;
	}

	string strings[4];
	for (int i = 0; i < 4; i++)
	{
		strings[i].clear();
		if (GetIsNthEffectKnown(ingredient, i))
		{
			MagicItem::EffectItem* pEI = NULL;
			ingredient->effectItemList.GetNthItem(i, pEI);
			if (pEI)
			{
				TESFullName* pFullName = DYNAMIC_CAST(pEI->mgef, TESForm, TESFullName);
				if (pFullName)
				{
					myfile << pFullName->name.data << endl;
					strings[i].append(pFullName->name.data);
					myfile << strings[i].c_str() << endl;
				}
			}
		}
	}
	GFxValue obj2;
	args->movie->CreateObject(&obj2);
	this->RegisterString(&obj2, args->movie, "effect1", strings[0].c_str());
	this->RegisterString(&obj2, args->movie, "effect2", strings[1].c_str());
	this->RegisterString(&obj2, args->movie, "effect3", strings[2].c_str());
	this->RegisterString(&obj2, args->movie, "effect4", strings[3].c_str());
	args->args[0].SetMember("ingredientObj", &obj2);
};

void CAHZUtility::ProcessInventoryCount(TESObjectREFR* targetObject, GFxFunctionHandler::Args *args)
{
	TESObjectREFR * pTargetReference = targetObject;

	IngredientItem * ingredient = GetIngredient(pTargetReference);
	AlchemyItem *food = NULL;
	// If not an ingredient, then see if its food
	if (!ingredient)
		food = GetFood(pTargetReference);

	// If the target is not valid or it can't be picked up by the player
	if ((!pTargetReference) ||
		((!CanPickUp(pTargetReference->baseForm->GetFormType())) &&
		(!ingredient) &&
			(!food)))
	{
		args->args[0].DeleteMember("dataObj");
		return;
	}

	// Used to store the name
	string name;
	// Used to store the count of the item
	UInt32 itemCount;

	if (ingredient)
	{
		// Get the number of this in the inventory
		itemCount = CAHZPlayerInfo::GetItemAmount(ingredient->formID);
		TESFullName* pFullName = DYNAMIC_CAST(ingredient, TESForm, TESFullName);
		if (pFullName)
		{
			name.append(pFullName->name.data);
		}
	}
	else if (food)
	{
		// Get the number of this in the inventory
		itemCount = CAHZPlayerInfo::GetItemAmount(food->formID);
		TESFullName* pFullName = DYNAMIC_CAST(food, TESForm, TESFullName);
		if (pFullName)
		{
			name.append(pFullName->name.data);
		}
	}
	else
	{
		// Get the number of this in the inventory
		itemCount = CAHZPlayerInfo::GetItemAmount(pTargetReference->baseForm->formID);
		name = CAHZUtility::GetTargetName(pTargetReference);
	}

	// If the name contains a string
	if (name.length())
	{
		GFxValue obj;
		args->movie->CreateObject(&obj);

		this->RegisterString(&obj, args->movie, "inventoryName", name.c_str());
		this->RegisterNumber(&obj, "inventoryCount", itemCount);

		// Add the object to the scaleform function
		args->args[0].SetMember("dataObj", &obj);
	}
	else
	{
		args->args[0].DeleteMember("dataObj");
	}
};

void CAHZUtility::RegisterString(GFxValue * dst, GFxMovieView * view, const char * name, const char * str)
{
	//GFxValue	fxValue;
	//view->CreateString(&fxValue, str);
	//dst->SetMember(name, &fxValue);

	//RegisterUnmanagedString()

	GFxValue	fxValue;

	fxValue.SetString(str);

	dst->SetMember(name, &fxValue);
};


void CAHZUtility::RegisterNumber(GFxValue * dst, const char * name, double value)
{
	GFxValue	fxValue;
	fxValue.SetNumber(value);
	dst->SetMember(name, &fxValue);
};

bool CAHZUtility::ProcessValidTarget(TESObjectREFR* targetObject, GFxFunctionHandler::Args *args)
{
	TESObjectREFR * pTargetReference = targetObject;
	IngredientItem * ingredient = GetIngredient(pTargetReference);
	AlchemyItem * food = NULL;

	// If not an ingredient, then see if its food
	if (!ingredient)
	{
		food = GetFood(pTargetReference);
	}

	// If the target is not valid or it can't be picked up by the player
	if ((!pTargetReference) ||
		((!CanPickUp(pTargetReference->baseForm->GetFormType())) &&
		(!ingredient) &&
			(!food)))
	{
		if (args)
		{
			// return false, indicating that the target object is not valid for acquiring data
			args->result->SetBool(false);
		}
		return false;
	}
	else
	{
		if (args)
		{
			// The object is valid
			args->result->SetBool(true);
		}
		return true;
	}
}

void CAHZUtility::ProcessPlayerData(GFxFunctionHandler::Args *args)
{
	GFxValue obj;
	args->movie->CreateObject(&obj);

	UInt32 actorValue = LookupActorValueByName("InventoryWeight");
	float encumbranceNumber = ((*g_thePlayer)->actorValueOwner.GetCurrent(actorValue));
	actorValue = LookupActorValueByName("CarryWeight");
	float maxEncumbranceNumber = ((*g_thePlayer)->actorValueOwner.GetCurrent(actorValue));

	// Enter the data into the Scaleform function
	this->RegisterNumber(&obj, "encumbranceNumber", encumbranceNumber);
	this->RegisterNumber(&obj, "maxEncumbranceNumber", maxEncumbranceNumber);
	this->RegisterNumber(&obj, "goldNumber", CAHZPlayerInfo::GetGoldAmount());
	args->args[0].SetMember("playerObj", &obj);
}

void CAHZUtility::GetMagicItemDescription(MagicItem * item, std::string& description)
{
	string outerString = "";
	description.clear();

	BSString temp;

	GetMagicItemDescription2(NULL, item, &temp);
	char *temp2 = ProcessSurvivalMode(&temp);

	description.append(temp.Get());
}