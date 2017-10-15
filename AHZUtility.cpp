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

//RelocAddr<_GetMyEffectsName> GetMyEffectsName(0x009530B0);

RelocAddr<_IsSurvivalMode> IsSurvivalMode(0x008D9220);

//RelocAddr<GET_MAGIC_ITEM_DESCRIPTION> GetMagicItemDescription2(0x8912A0);

//RelocAddr<GET_MAGIC_ITEM_DESCRIPTION> GetMagicItemDescription2(0x890960);


RelocAddr<GET_MAGIC_ITEM_DESCRIPTION> GetMagicItemDescription2(0x891320);

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


bool IsEnglish()
{
	static bool settingsRead = false;
	static bool isEnglish = false;

	// Only read from file once, others read from ram
	if (!settingsRead)
	{
		settingsRead = true;
		Setting	* setting = GetINISetting("sLanguage:General");
		if (setting && setting->GetType() == Setting::kType_String)
		{
			if (strstr(setting->data.s, "ENGLISH"))
			{
				isEnglish = true;
			}
			else
			{
				isEnglish = false;
			}
		}
		else
		{
			// default to english if the setting does not exist
			isEnglish = true;
		}
	}

	return isEnglish;
}

UInt32 SafeRead32(UInt32 addr)
{
	UInt32	oldProtect;
	UInt32 data;

	VirtualProtect((void *)addr, 4, PAGE_EXECUTE_READ, &oldProtect);
	data = *((UInt32 *)addr);
	VirtualProtect((void *)addr, 4, oldProtect, &oldProtect);
	return data;
}

