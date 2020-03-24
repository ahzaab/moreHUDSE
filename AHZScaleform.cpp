#include <wchar.h>
#include <stdlib.h>
#include <sstream>
#include <iostream>
#include <list>
#include <algorithm>
#include "skse64/GameReferences.h"
#include "skse64/GameObjects.h"
#include "skse64/GameRTTI.h"
#include "skse64/GameSettings.h"
#include "skse64/GameExtraData.h"
#include "skse64/ScaleformCallbacks.h"
#include "skse64/ScaleformMovie.h"
#include "skse64/GameMenus.h"
#include "skse64/skse64_common/Utilities.h"
#include "AHZArmorInfo.h"
#include "AHZWeaponInfo.h"
#include "AHZScaleform.h"
#include "AHZScaleformHook.h"
#include "AHZFormLookup.h"
#include "AHZUtilities.h"

static std::map<UInt8, string> m_soulMap;

RelocAddr<GET_ACTOR_WARMTH_RATING> GetActorWarmthRating_Native(0x003BD5F0);
RelocAddr<GET_ARMOR_WARMTH_RATING>GetArmorWarmthRating_Native(0x003BD510);
RelocAddr<GET_MAGIC_ITEM_DESCRIPTION> GetMagicItemDescription2(0x00892CD0);
RelocAddr<PROCESS_SURVIVAL_MODE> ProcessSurvivalMode(0x008930F0);
RelocAddr<_IsSurvivalMode> IsSurvivalMode(0x008DAAB0);

float CAHZScaleform::GetBaseDamage(TESAmmo* pthisAmmo)
{
   return pthisAmmo->settings.damage;
}

string CAHZScaleform::GetSoulLevelName(UInt8 soulLevel)
{
	if (m_soulMap.empty()) //Cache it,  No need to hit the game setting every time
	{
		SettingCollectionMap	* settings = *g_gameSettingCollection;
		m_soulMap[1] = string(settings->Get("sSoulLevelNamePetty")->data.s);
		m_soulMap[2] = string(settings->Get("sSoulLevelNameLesser")->data.s);
		m_soulMap[3] = string(settings->Get("sSoulLevelNameCommon")->data.s);
		m_soulMap[4] = string(settings->Get("sSoulLevelNameGreater")->data.s);
		m_soulMap[5] = string(settings->Get("sSoulLevelNameGrand")->data.s);
	}
	if (m_soulMap.find(soulLevel) == m_soulMap.end())
	{
		return string("");
	}
	return m_soulMap[soulLevel];
}

bool MagicDisallowEnchanting(BGSKeywordForm *pKeywords)
{
	if (pKeywords)
	{
		for (UInt32 k = 0; k < pKeywords->numKeywords; k++) {
			if (pKeywords->keywords[k]) {
				string keyWordName = string(pKeywords->keywords[k]->keyword.Get());
				if (keyWordName == "MagicDisallowEnchanting")
				{
					return true;  // Is enchanted, but cannot be enchanted by player
				}
			}
		}
	}
	return false;
}

UInt32 CAHZScaleform::GetIsKnownEnchantment(TESObjectREFR *targetRef)
{
	PlayerCharacter* pPC = (*g_thePlayer);
	TESForm *baseForm;
	if (pPC && targetRef && (baseForm = targetRef->baseForm) &&
		(baseForm->GetFormType() == kFormType_Weapon || baseForm->GetFormType() == kFormType_Armor || baseForm->GetFormType() == kFormType_Ammo || baseForm->GetFormType() == kFormType_Projectile))
	{
		EnchantmentItem * enchantment = NULL;
		TESEnchantableForm * enchantable = DYNAMIC_CAST(baseForm, TESForm, TESEnchantableForm);
		if (baseForm->GetFormType() == kFormType_Projectile)
			enchantable = DYNAMIC_CAST(AHZGetForm(targetRef), TESForm, TESEnchantableForm);
		
		bool wasExtra = false;
		if (enchantable) { // Check the item for a base enchantment
			enchantment = enchantable->enchantment;
		}
		if (ExtraEnchantment* extraEnchant = static_cast<ExtraEnchantment*>(targetRef->extraData.GetByType(kExtraData_Enchantment)))
		{
			wasExtra = true;
			enchantment = extraEnchant->enchant;
		}

		if (enchantment)
		{
			if ((enchantment->flags & TESForm::kFlagPlayerKnows) == TESForm::kFlagPlayerKnows) {
				return MagicDisallowEnchanting(DYNAMIC_CAST(enchantment, EnchantmentItem, BGSKeywordForm)) ? 2 : 1;
			}
			else if (MagicDisallowEnchanting(DYNAMIC_CAST(enchantment, EnchantmentItem, BGSKeywordForm)))
			{
				return 2;
			}

			EnchantmentItem * baseEnchantment = (EnchantmentItem *)(enchantment->data.baseEnchantment);
			if (baseEnchantment)
			{
				if ((baseEnchantment->flags & TESForm::kFlagPlayerKnows) == TESForm::kFlagPlayerKnows) {
					return MagicDisallowEnchanting(DYNAMIC_CAST(baseEnchantment, EnchantmentItem, BGSKeywordForm)) ? 2 : 1;
				}
				else if (MagicDisallowEnchanting(DYNAMIC_CAST(baseEnchantment, EnchantmentItem, BGSKeywordForm)))
				{
					return 2;
				}
			}
		}

		// Its safe to assume that if it not a base enchanted item, that it was enchanted by the player and therefore, they
		// know the enchantment
		if (wasExtra)
		{
			return 1;
		}
		else if (enchantable) {
			return MagicDisallowEnchanting(DYNAMIC_CAST(enchantable, TESEnchantableForm, BGSKeywordForm)) ? 2 : 0;
		}

	}
	return 0;
}
double CAHZScaleform::GetActualDamage(AHZWeaponData *weaponData)
{
   if (!weaponData)
      return 0.0;

   PlayerCharacter* pPC = (*g_thePlayer);

   if (pPC)
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

double CAHZScaleform::GetArmorWarmthRating(AHZArmorData* armorData)
{
   if (!armorData->armor)
      return 0.0;

   InventoryEntryData objDesc(armorData->equipData.pForm, 0);

   // Allocate a dummy list so skyrim does not crash. For armor information
   // skyrim doesn't appear to need the list
   objDesc.extendDataList = new tList<BaseExtraList>();
   if (armorData->equipData.pExtraData)
   {
      objDesc.extendDataList->Insert(armorData->equipData.pExtraData);
   }

   double fRating = GetArmorWarmthRating_Native(objDesc.type);

   // Delete the allocated dummy list
   delete objDesc.extendDataList;

   // This could be rounded, but the the script decide
   return mRound(fRating);

   return 0.0;
}

double CAHZScaleform::GetArmorWarmthRating(TESObjectREFR* targetRef)
{
   if (!targetRef || !targetRef->baseForm)
      return 0.0;

   if (targetRef->baseForm->GetFormType() != kFormType_Armor || !IsSurvivalMode())
   {
      return 0.0;
   }

   AHZArmorData armorData(targetRef);
   return GetArmorWarmthRating(&armorData);
}

double CAHZScaleform::GetPlayerWarmthRating(void)
{

   return 0.0;
}

double CAHZScaleform::GetActualArmorRating(AHZArmorData* armorData)
{
   if (!armorData->armor)
      return 0.0;

   PlayerCharacter* pPC = (*g_thePlayer);

   if (pPC)
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

double CAHZScaleform::GetTotalActualArmorRating(void)
{
   double totalRating = 0.0;
   double rating = 0.0;
   // Keep a list of items to make sure they are not added more than once
   // Some items take up more than one slot
   std::list<TESForm*> clist;
   for (UInt64 slot = 1; slot <= (UInt64)0x2000; slot <<= 1)
   {
      AHZArmorData armorData = CAHZArmorInfo::GetArmorFromSlotMask(slot);
      if (armorData.equipData.pForm)
      {
         if (find(clist.begin(), clist.end(), armorData.equipData.pForm) == clist.end())
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

double CAHZScaleform::GetTotalWarmthRating(void)
{
   PlayerCharacter* pPC = (*g_thePlayer);

   if (!pPC)
   {
      return 0.0;
   }

   return GetActorWarmthRating_Native(pPC, 0.0);
}

double CAHZScaleform::mRound(double r)
{
   return (r >= 0.0) ? floor(r + 0.5) : ceil(r - 0.5);
}

double CAHZScaleform::GetArmorRatingDiff(TESObjectREFR *thisArmor)
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

double CAHZScaleform::GetWarmthRatingDiff(TESObjectREFR *thisArmor)
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

   newArmorRating = GetArmorWarmthRating(&armorData);

   // Get the armor rating from the armor that shares the same slot
   AHZArmorData sameSlotData = CAHZArmorInfo::GetArmorFromSlotMask(
      armorData.armor->bipedObject.GetSlotMask());
   if (sameSlotData.armor)
   {
      oldArmorRating = GetArmorWarmthRating(&sameSlotData);
   }

   // Get the total
   oldTotalArmorRating = GetTotalWarmthRating();

   newTotalArmorRating = (oldTotalArmorRating - oldArmorRating) + newArmorRating;

   deltaRating = newTotalArmorRating - oldTotalArmorRating;

   return deltaRating;
}

double CAHZScaleform::GetTotalActualWeaponDamage(void)
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

bool CAHZScaleform::isBolt(TESAmmo *thisAmmo)
{
   return (thisAmmo->isBolt());
}

double CAHZScaleform::GetWeaponDamageDiff(TESObjectREFR *targetWeaponOrAmmo)
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

   //DumpClass(targetWeapon.ammo, 256);

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

bool CAHZScaleform::IsTwoHanded(TESObjectWEAP * thisWeapon)
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

bool CAHZScaleform::IsOneHanded(TESObjectWEAP * thisWeapon)
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

//bool CAHZUtility::IsBowType(TESObjectWEAP * thisWeapon)
//{
//			//kType_HandToHandMelee = 0,
//			//kType_OneHandSword,
//			//kType_OneHandDagger,
//			//kType_OneHandAxe,
//			//kType_OneHandMace,
//			//kType_TwoHandSword,
//			//kType_TwoHandAxe,
//			//kType_Bow,
//			//kType_Staff,
//			//kType_CrossBow,
//			//kType_H2H,
//			//kType_1HS,
//			//kType_1HD,
//			//kType_1HA,
//			//kType_1HM,
//			//kType_2HS,
//			//kType_2HA,
//			//kType_Bow2,
//			//kType_Staff2,
//			//kType_CBow
//	return (thisWeapon->type() == TESObjectWEAP::GameData::kType_Bow ||
//			thisWeapon->type() == TESObjectWEAP::GameData::kType_CrossBow ||
//			thisWeapon->type() == TESObjectWEAP::GameData::kType_Bow2 ||
//			thisWeapon->type() == TESObjectWEAP::GameData::kType_CBow);
//}

bool CAHZScaleform::IsBow(TESObjectWEAP * thisWeapon)
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

bool CAHZScaleform::IsCrossBow(TESObjectWEAP * thisWeapon)
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

AlchemyItem * CAHZScaleform::GetAlchemyItemFromLeveledList(TESForm *thisObject)
{
    if (thisObject->formType == kFormType_LeveledItem)
    {
        TESLevItem *lvli = DYNAMIC_CAST(thisObject, TESForm, TESLevItem);

        // Get the first form and see if it is an ingredient
        if (lvli && lvli->leveledList.length > 0)
        {
            for (int i = 0; i < lvli->leveledList.length; i++)
            {
                TESForm *itemform = (TESForm *)lvli->leveledList.entries[i].form;
                if (itemform)
                {
                    if (itemform->formType == kFormType_Potion)
                    {
                        AlchemyItem *alchemyItem = DYNAMIC_CAST(itemform, TESForm, AlchemyItem);
                        return alchemyItem;
                    }
                }
            }
        }
    }

    return NULL;
}

IngredientItem* CAHZScaleform::GetIngredientFromLeveledList(TESForm *thisObject)
{
    if (thisObject->formType == kFormType_LeveledItem)
    {
        TESLevItem *lvli = DYNAMIC_CAST(thisObject, TESForm, TESLevItem);

        // Get the first form and see if it is an ingredient
        if (lvli && lvli->leveledList.length > 0)
        {
            for (int i = 0; i < lvli->leveledList.length; i++)
            {
                TESForm *itemform = (TESForm *)lvli->leveledList.entries[i].form;
                if (itemform)
                {
                    if (itemform->formType == kFormType_Ingredient)
                    {
                        IngredientItem *ingredient = DYNAMIC_CAST(itemform, TESForm, IngredientItem);
                        return ingredient;
                    }
                }
            }
        }
    }

    return NULL;
}

IngredientItem* CAHZScaleform::GetIngredient(TESForm *thisObject)
{
    if (!thisObject)
        return NULL;

    TESObjectREFR*reference = AHZGetReference(thisObject);
    if (reference)
    {
        thisObject = reference->baseForm;
    }

    if (!thisObject) {
        return NULL;
    }

    if (thisObject->GetFormType() == kFormType_Ingredient)
        return DYNAMIC_CAST(thisObject, TESForm, IngredientItem);

    if (thisObject->GetFormType() == kFormType_Flora)
    {
        // Skip if harvested
        if (reference && (reference->flags & TESObjectREFR::kFlag_Harvested))
        {
            return NULL;
        }

        TESFlora *flora = DYNAMIC_CAST(thisObject, TESForm, TESFlora);
        if (flora)
        {
            TESForm *form = (TESForm *)flora->produce.produce;

            if (!form) {
                return NULL;
            }

            // If the ingredient is actually an ingredient
            if (form->formType == kFormType_Ingredient)
            {
                return DYNAMIC_CAST(flora->produce.produce, TESForm, IngredientItem);
            }

            // If the ingredient is actually a levelitem (Harvest overhaul mod or a coin purse)
            else if (form->formType == kFormType_LeveledItem)
            {
                TESForm *itemform = GetIngredientFromLeveledList(form);

                if (!itemform)
                {
                    return NULL;
                }
                else if (itemform->formType == kFormType_Ingredient)
                {
                    IngredientItem *ingredient = DYNAMIC_CAST(itemform, TESForm, IngredientItem);
                    return ingredient;
                }
            }
            else if (form->formType == kFormType_List)
            {
                BGSListForm *lvli = DYNAMIC_CAST(form, TESForm, BGSListForm);

                // Get the first form and see if it is an ingredient
                if (lvli->forms.count > 0)
                {
                    TESForm *itemform = (TESForm *)lvli->forms.entries[0];
                    if (itemform)
                    {
                        IngredientItem *ingredient = DYNAMIC_CAST(itemform, TESForm, IngredientItem);
                        return ingredient;
                    }
                }
            }
        }
    }
    else if (thisObject->GetFormType() == kFormType_Tree)
    {
        // Skip if harvested
        if (reference && (reference->flags & TESObjectREFR::kFlag_Harvested))
        {
            return NULL;
        }

        TESObjectTREE *tree = DYNAMIC_CAST(thisObject, TESForm, TESObjectTREE);
        if (tree)
        {
            TESForm *form = (TESForm *)tree->produce.produce;//DYNAMIC_CAST(tree->produce.produce, IngredientItem, TESForm);

            if (!form) {
                return NULL;
            }

            // If the ingredient is actually an ingredient
            if (form->formType == kFormType_Ingredient)
            {
                return DYNAMIC_CAST(tree->produce.produce, TESForm, IngredientItem);
            }

            // If the ingredient is actually a levelitem (Harvest overhaul mod or a coin purse)
            else if (form->formType == kFormType_LeveledItem)
            {
                TESForm *itemform = GetIngredientFromLeveledList(form);

                if (!itemform)
                {
                    return NULL;
                }
                else if (itemform->formType == kFormType_Ingredient)
                {
                    IngredientItem *ingredient = DYNAMIC_CAST(itemform, TESForm, IngredientItem);
                    return ingredient;
                }
            }
            else if (form->formType == kFormType_List)
            {
                BGSListForm *lvli = DYNAMIC_CAST(form, TESForm, BGSListForm);

                if (!lvli) {
                    return NULL;
                }

                // Get the first form and see if it is an ingredient
                if (lvli->forms.count > 0)
                {
                    TESForm *itemform = (TESForm *)lvli->forms.entries[0];
                    if (itemform)
                    {
                        IngredientItem *ingredient = DYNAMIC_CAST(itemform, TESForm, IngredientItem);
                        return ingredient;
                    }
                }
            }
        }
    }

    return NULL;
}

SpellItem* CAHZScaleform::GetSpellItem(TESForm *thisObject)
{
   if (!thisObject)
      return NULL;

   TESObjectREFR*reference = AHZGetReference(thisObject);
   if (reference)
   {
      thisObject = reference->baseForm;
   }

   if (!thisObject)
	   return NULL;

   return DYNAMIC_CAST(thisObject, TESForm, SpellItem);
}

AlchemyItem* CAHZScaleform::GetAlchemyItem(TESForm *thisObject)
{
    if (!thisObject)
        return NULL;

    TESObjectREFR*reference = AHZGetReference(thisObject);
    if (reference) {
        thisObject = reference->baseForm;
    }

    if (!thisObject) {
        return NULL;
    }

    if (thisObject->GetFormType() == kFormType_Potion)
        return DYNAMIC_CAST(thisObject, TESForm, AlchemyItem);

    if (thisObject->GetFormType() == kFormType_Flora)
    {
        // Skip if harvested
        if (reference && (reference->flags & TESObjectREFR::kFlag_Harvested))
        {
            return NULL;
        }

        TESFlora *flora = DYNAMIC_CAST(thisObject, TESForm, TESFlora);
        if (flora)
        {
            TESForm *form = (TESForm *)flora->produce.produce;

            if (!form) {
                return NULL;
            }

            // If the food is actually food
            if (form->formType == kFormType_Potion)
            {
                return DYNAMIC_CAST(form, TESForm, AlchemyItem);
            }

            // If the food is actually a levelitem (Harvest overhaul mod or a coin purse)
            else if (form->formType == kFormType_LeveledItem)
            {
                TESForm *itemform = GetAlchemyItemFromLeveledList(form);

                if (!itemform)
                {
                    return NULL;
                }
                else if (itemform->formType == kFormType_Potion)
                {
                    AlchemyItem *alchmyItem = DYNAMIC_CAST(itemform, TESForm, AlchemyItem);
                    return alchmyItem;
                }
            }
            else if (form->formType == kFormType_List)
            {
                BGSListForm *lvli = DYNAMIC_CAST(form, TESForm, BGSListForm);

                if (!lvli) {
                    return NULL;
                }

                // Get the first form and see if it is an ingredient
                if (lvli->forms.count > 0)
                {
                    TESForm *itemform = (TESForm *)lvli->forms.entries[0];
                    if (itemform)
                    {
                        AlchemyItem *alchmyItem = DYNAMIC_CAST(itemform, TESForm, AlchemyItem);
                        return alchmyItem;
                    }
                }
            }
        }
    }
    else if (thisObject->GetFormType() == kFormType_Tree)
    {
        // Skip if harvested
        if (reference && (reference->flags & TESObjectREFR::kFlag_Harvested))
        {
            return NULL;
        }

        TESObjectTREE *tree = DYNAMIC_CAST(thisObject, TESForm, TESObjectTREE);
        if (tree)
        {
            TESForm *form = (TESForm *)tree->produce.produce;

            if (!form) {
                return NULL;
            }

            // If the produce is actually food
            if (form->formType == kFormType_Potion)
            {
                return DYNAMIC_CAST(form, TESForm, AlchemyItem);
            }

            // If the ingredient is actually a levelitem (Harvest overhaul mod or a coin purse)
            else if (form->formType == kFormType_LeveledItem)
            {
                TESForm *itemform = GetAlchemyItemFromLeveledList(form);

                if (!itemform)
                {
                    return NULL;
                }
                else if (itemform->formType == kFormType_Potion)
                {
                    AlchemyItem *alchmyItem = DYNAMIC_CAST(itemform, TESForm, AlchemyItem);
                    return alchmyItem;
                }
            }
            else if (form->formType == kFormType_List)
            {
                BGSListForm *lvli = DYNAMIC_CAST(form, TESForm, BGSListForm);

                if (!lvli) {
                    return NULL;
                }

                // Get the first form and see if it is an ingredient
                if (lvli->forms.count > 0)
                {
                    TESForm *itemform = (TESForm *)lvli->forms.entries[0];
                    if (itemform)
                    {
                        AlchemyItem *alchmyItem = DYNAMIC_CAST(itemform, TESForm, AlchemyItem);
                        return alchmyItem;
                    }
                }
            }
        }
    }

    return NULL;
}

bool CAHZScaleform::CanPickUp(TESForm* form)
{
	if (!form) {
		return false;
	}

	UINT32 formType = form->GetFormType();

	bool canCarry =  (formType == kFormType_Weapon ||
		formType == kFormType_Armor ||
		formType == kFormType_SoulGem ||
		formType == kFormType_Potion ||
		formType == kFormType_Misc ||
		formType == kFormType_Ingredient ||
		formType == kFormType_Book ||
		formType == kFormType_Ammo ||
		formType == kFormType_ScrollItem ||
		formType == kFormType_Outfit ||
		formType == kFormType_Key ||
		formType == kFormType_Projectile);  // Projectiles with the "Can Pick Up" flag set to false will not even register in the crossshairs
	return canCarry;
}

string CAHZScaleform::GetTargetName(TESForm *thisObject)
{
   string name;
   
   TESObjectREFR * reference = AHZGetReference(thisObject);

   if (!reference)
   {
      return name;
   }

   TESFullName* pFullName = DYNAMIC_CAST(reference->baseForm, TESForm, TESFullName);

   const char* displayName = reference->extraData.GetDisplayName(reference->baseForm);

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
   else // second attempt to get the name
   {
      name.append(reference->baseForm->GetFullName());
   }

   // If this is a soul gem, also get the gem size name
   if (reference->baseForm->formType == kFormType_SoulGem)
   {
      TESSoulGem *gem = DYNAMIC_CAST(reference->baseForm, TESForm, TESSoulGem);
      if (gem)
      {
         string soulName("");
         SettingCollectionMap	* settings = *g_gameSettingCollection;

		 soulName = GetSoulLevelName(gem->soulSize);


         if (soulName.length())
         {
            name.append(" (");
            name.append(soulName);
            name.append(")");
         }
      }
   }

   return name;
};

bool CAHZScaleform::GetIsBookAndWasRead(TESObjectREFR *theObject)
{
   if (!theObject)
      return false;

   if (!theObject->baseForm)
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
}

static UInt32 lasttargetRef;

static bool DoubleCompare(double a, double b)
{
	double delta = fabs(a - b);
	if (delta <  std::numeric_limits<double>::epsilon() &&
		delta > -std::numeric_limits<double>::epsilon()) {
		return true;
	}
	return false;
}

static double GetPct(double current, double max)
{
	double percent = -1; 

	if (max < 0)
	{
		return percent;
	}

	if (!DoubleCompare(max, 0.0))
	{
		percent = ceil((current / max) * 100.0);
		return fmin(100, fmax(percent, -1));  // negative indicates that the actor value is not used
	}

	return percent;
}

void CAHZScaleform::ProcessEnemyInformation(GFxFunctionHandler::Args * args)
{
	PlayerCharacter* pPC = (*g_thePlayer);
	CAHZActorData actorData;
	actorData.Level = 0;
	actorData.IsSentient = 0;
	UInt16 playerLevel = 0;
	UInt32 soulType = 0;

	if (pPC)
	{
		actorData = GetCurrentEnemyData();
		if (actorData.Level)
		{
			playerLevel = CALL_MEMBER_FN(pPC, GetLevel)();
			if (!actorData.IsSentient) {  // If sentient, then don't bother all sentients have grand soul gem levels
				soulType = CAHZActorInfo::GetSoulType(actorData.Level, actorData.IsSentient);
			}
		}
	}

	GFxValue enemyObj;
	GFxValue playerObj;
	args->movie->CreateObject(&enemyObj);
	args->movie->CreateObject(&playerObj);

	RegisterNumber(&enemyObj, "level", actorData.Level);
	RegisterNumber(&playerObj, "level", playerLevel);
	string soulName = GetSoulLevelName((UInt8)soulType);
	if (soulType && soulName.length())
	{
		RegisterString(&enemyObj, args->movie, "soul", soulName.c_str());
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

	if (args->args[0].HasMember("player"))
	{
		args->args[0].SetMember("player", &playerObj);
	}
	if (args->args[0].HasMember("enemy"))
	{
		args->args[0].SetMember("enemy", &enemyObj);
	}
}

string CAHZScaleform::GetArmorWeightClass(TESObjectREFR *theObject)
{
   string desc;

   if (!theObject)
      return desc;

   if (!theObject->baseForm)
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

string CAHZScaleform::GetValueToWeight(TESObjectREFR *theObject, const char * stringFromHUD, const char * vmTranslated)
{
	string desc;

	if (!theObject)
		return desc;

	if (!theObject->baseForm)
		return desc;

	if (!stringFromHUD)
		return desc;

	//<TEXTFORMAT INDENT="0" LEFTMARGIN="0" RIGHTMARGIN="0" LEADING="2"><P ALIGN="CENTER"><FONT FACE="$EverywhereMediumFont" SIZE="15" COLOR="#999999" KERNING="0">WEIGHT </FONT><FONT FACE="$EverywhereBoldFont" SIZE="24" COLOR="#FFFFFF" KERNING="0">0.5</FONT><FONT FACE="$EverywhereMediumFont" SIZE="15" COLOR="#999999" KERNING="0">      VALUE </FONT><FONT FACE="$EverywhereBoldFont" SIZE="24" COLOR="#FFFFFF" KERNING="0">21</FONT></P></TEXTFORMAT>

	// Using regex from the HUD string to extract the value and weight values.  The SKSE version are either broken or unreliable
	std::regex rgx(R"(\s+([0-9]*\.?[0-9]+))");
	std::smatch match;
	string s = stringFromHUD;
	const string cs = const_cast<string &>(s);
	vector<string> parts;

	while (regex_search(s, match, rgx))
	{
		if (!match.size())
		{
			return desc;
		}

		parts.push_back(match[0]);

		s = match.suffix();
	}

	if (parts.size() < 2)
	{
		return desc;
	}

	// The fixed positions of the matches (containing groups)
	string weight = parts[parts.size() - 2];
	string value = parts[parts.size() - 1];
	char *end;

	float weightValue = strtof(weight.c_str(), &end);
	float valueValue = strtof(value.c_str(), &end);

	// Don't show a neg or 0 ratio, its pointless
	if (weightValue <= 0.0 || valueValue <= 0.0)
	{
		return desc;
	}

	float vW = valueValue / weightValue;

	// Add the VW label
	desc.append("<FONT FACE=\"$EverywhereMediumFont\"SIZE=\"15\"COLOR=\"#999999\"KERNING=\"0\">     ");
	desc.append(vmTranslated);
	desc.append(" <\\FONT>");

	char floatHold[64];
	size_t size = 64;

	//Rounding trick
	sprintf_s(floatHold, size, "%.2f", vW);
	vW = strtof(floatHold, &end);

	if (vW < 1.0)
	{
		sprintf_s(floatHold, size, "%.1g", vW);
	}
	else
	{
		sprintf_s(floatHold, size, "%.0f", vW);
	}

	desc.append("<FONT FACE=\"$EverywhereBoldFont\"SIZE=\"24\"COLOR=\"#FFFFFF\"KERNING=\"0\">");
	desc.append(floatHold);
	desc.append("<\\FONT>");

	return desc;


	//string desc;

	//if (!theObject)
	//	return desc;

	//if (!theObject->baseForm)
	//	return desc;

	//if (!stringFromHUD)
	//	return desc;

	//char *end;
	//float weightValue = 0.0;
	//float valueValue = 0.0;
	//std::istringstream iss(stringFromHUD);
	//std::string throwaway;

	//vector<string> parts;
	//int i = 0;
	//// Get the weight and value from the HUD text since there is no decoded way (reliably) to get the value for all forms and references
	//do 
	//{
	//	parts.push_back("");
	//} while (iss >> parts[i++]); // stream the different fields.  THe weight and height are the last
	//
	//if (parts.size() < 4)
	//{
	//	return desc;
	//}

	//// Remove the last empty string if one exists
	//if (parts[parts.size() - 1] == "")
	//{
	//	parts.pop_back();
	//}

	//// check the size again
	//if (parts.size() < 4)
	//{
	//	return desc;
	//}

	//// The last value should be the "Value"
	//// The third from last should be the weight
	//weightValue = atof(parts[parts.size() - 3].c_str());
	//valueValue = atof(parts[parts.size() - 1].c_str());

	//// Don't show a neg or 0 ratio, its pointless
	//if ((mRound(weightValue * 100) / 100) <= 0.0 || (mRound(valueValue * 100) / 100) <= 0.0)
	//{
	//	return desc;
	//}

	//float vW = valueValue / weightValue;

	//// Add the VW label
	//desc.append("<FONT FACE=\"$EverywhereMediumFont\"SIZE=\"15\"COLOR=\"#999999\"KERNING=\"0\">     ");
	//desc.append(vmTranslated);
	//desc.append(" <\\FONT>");

	//char floatHold[64];
	//size_t size = 64;

	////Rounding trick
	//sprintf_s(floatHold, size, "%.2f", vW);
	//vW = strtof(floatHold, &end);

	//if (vW < 1.0)
	//{
	//	sprintf_s(floatHold, size, "%.1g", vW);
	//}
	//else
	//{
	//	sprintf_s(floatHold, size, "%.0f", vW);
	//}

	//desc.append("<FONT FACE=\"$EverywhereBoldFont\"SIZE=\"24\"COLOR=\"#FFFFFF\"KERNING=\"0\">");
	//desc.append(floatHold);
	//desc.append("<\\FONT>");
 //  
	//return desc;
};

string CAHZScaleform::GetBookSkill(TESObjectREFR *theObject)
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

void CAHZScaleform::AppendDescription(TESDescription *desObj, TESForm *parent, std::string& description)
{
   BSString bsDescription;
   string tempString = "";

   if (parent->GetFormType() == kFormType_Book)
   {
      CALL_MEMBER_FN(desObj, Get)(&bsDescription, parent, 0x4D414E43);
      if (&bsDescription)
      {
         tempString.clear();
         tempString.append(bsDescription.Get());
         if (tempString != "LOOKUP FAILED!" && tempString.length() > 1)
         {
            string formatted = "";
            FormatDescription(tempString, formatted);
            description.append(formatted);
         }
      }
   }
   else
   {
      BSString bsDescription2;
      TESDescription * desc = DYNAMIC_CAST(parent, TESForm, TESDescription);
      if (desc)
      {
         CALL_MEMBER_FN(desc, Get)(&bsDescription2, parent, 0x43534544);
         tempString.clear();
         if (&bsDescription2)
            tempString.append(bsDescription2.Get());
         if (tempString != "LOOKUP FAILED!" && tempString.length() > 1)
         {
            string formatted = "";
            FormatDescription(tempString, formatted);
            description.append(formatted);
         }
         else
         {
            CALL_MEMBER_FN(desc, Get)(&bsDescription2, parent, 0);
            tempString.clear();
            if (&bsDescription2)
               tempString.append(bsDescription2.Get());
            if (tempString != "LOOKUP FAILED!" && tempString.length() > 1)
            {
               string formatted = "";
               FormatDescription(tempString, formatted);
               description.append(formatted);
            }
         }
      }
   }
}

string CAHZScaleform::GetEffectsDescription(TESObjectREFR *theObject)
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

      // If there was no effects, then display athe description if available
      if (item)
      {
         // Get the description if any (Mostly Dawnguard and Dragonborn stuff uses the descriptions)
         AppendDescription(&item->description, item, desc);
      }

      if (item && !desc.length())
      {
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
      }
   }
   else if (theObject->baseForm->GetFormType() == kFormType_Armor)
   {
      TESObjectARMO *item = DYNAMIC_CAST(theObject->baseForm, TESForm, TESObjectARMO);

      // If there was no effects, then display athe description if available
      if (item)
      {
         // Get the description if any (Mostly Dawnguard and Dragonborn stuff uses the descriptions)
         AppendDescription(&item->description, item, desc);
      }

      if (item && !desc.length())
      {
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
      }
   }
   else if (theObject->baseForm->GetFormType() == kFormType_Ammo ||
			theObject->baseForm->GetFormType() == kFormType_Projectile)
   {
	   TESAmmo *item = NULL;
	   
	   if (theObject->baseForm->GetFormType() == kFormType_Projectile)
		   item = DYNAMIC_CAST(AHZGetForm(theObject), TESForm, TESAmmo);
	   else
		   item = DYNAMIC_CAST(theObject->baseForm, TESForm, TESAmmo);
	   
      if (item)
      {
         // Get the description if any (Mostly Dawnguard and Dragonborn stuff uses the descriptions)
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
         AppendDescription(&item->description2, item, desc);
      }

      if (item &&
         ((item->data.flags & TESObjectBOOK::Data::kType_Spell) == TESObjectBOOK::Data::kType_Spell) && !desc.length())
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
         AppendDescription(&item->description, item, desc);

         if (!desc.length())
         {
            GetMagicItemDescription(item, effectDescription);
            desc.append(effectDescription);
         }
      }
   }
   return desc;
};

void CAHZScaleform::ProcessTargetEffects(TESObjectREFR* targetObject, GFxFunctionHandler::Args *args)
{
   TESObjectREFR * pTargetReference = targetObject;
   AlchemyItem *alchemyItem = NULL;
   SpellItem *spellItem = NULL;
   IngredientItem *ingredientItem = NULL;
   string name;

   if (!args)
   {
      return;
   }

   bool calculateInvenotry = args->args[1].GetBool();

   if (!calculateInvenotry)
   {
      args->args[0].DeleteMember("inventoryObj");
   }

   // No valid reference
   if (!pTargetReference)
   {
      args->args[0].DeleteMember("effectsObj");
      args->args[0].DeleteMember("ingredientObj");
      args->args[0].DeleteMember("inventoryObj");
      return;
   }

   TESForm *targetForm = AHZGetForm(pTargetReference);

   // See if its an ingredient.  Note they are formated differently with known effects;
   if ((ingredientItem = GetIngredient(targetForm)) != NULL)
   {
      args->args[0].DeleteMember("effectsObj");
      BuildIngredientObject(ingredientItem, args);

      if (calculateInvenotry)
      {
         BuildInventoryObject(ingredientItem, args);
      }

      return;
   }

   // See if its harvestable food
   else if ((alchemyItem = GetAlchemyItem(targetForm)) != NULL)
   {
      string effectDescription;
      GetMagicItemDescription(alchemyItem, effectDescription);
      name.append(effectDescription);

      if (calculateInvenotry)
      {
         BuildInventoryObject(alchemyItem, args);
      }
   }

   // Spell items like blessings
   else if ((spellItem = GetSpellItem(targetForm)) != NULL)
   {
      AppendDescription(&(spellItem->description), spellItem, name);

      if (!name.length())
      {
         string effectDescription;
         GetMagicItemDescription(spellItem, effectDescription);
         name.append(effectDescription);
      }

      if (calculateInvenotry)
      {
         BuildInventoryObject(spellItem, args);
      }
   }
   else //For all effects from books, potions, weapon enchantments, etc.
   {
      // Get the effects description if it exists for this object
      name = GetEffectsDescription(pTargetReference);

      if (calculateInvenotry)
      {
         BuildInventoryObject(targetForm, args);
      }
   }

   // If the name contains a string
   if (name.length())
   {
      GFxValue obj;
      args->movie->CreateObject(&obj);

      name = CAHZUtilities::trim(name);

      RegisterString(&obj, args->movie, "effectsDescription",  name.c_str());

      // Add the object to the scaleform function
      args->args[0].SetMember("effectsObj", &obj);
   }
   else
   {
      args->args[0].DeleteMember("effectsObj");
   }
};

void CAHZScaleform::ProcessArmorClass(TESObjectREFR* targetObject, GFxFunctionHandler::Args *args)
{
   if (!args)
   {
      return;
   }

   TESObjectREFR * pTargetReference = targetObject;
   static string weightClass;

   // If the target is not valid or it can't be picked up by the player
   if (!pTargetReference)
   {
      SetResultString(args, "");
      return;
   }

   weightClass.clear();
   weightClass.append(GetArmorWeightClass(pTargetReference).c_str());

   SetResultString(args, weightClass.c_str());
};

void CAHZScaleform::ProcessValueToWeight(TESObjectREFR* targetObject, GFxFunctionHandler::Args *args)
{
   if (!args)
   {
      return;
   }

   TESObjectREFR * pTargetReference = targetObject;
   static string valueToWeight;

   // If the target is not valid or it can't be picked up by the player
   if (!pTargetReference)
   {
      SetResultString(args, "");
      return;
   }

   valueToWeight.clear();
   valueToWeight.append(GetValueToWeight(pTargetReference, args->args[0].GetString(), args->args[1].GetString()).c_str());

   SetResultString(args, valueToWeight.c_str());
};

void CAHZScaleform::ProcessBookSkill(TESObjectREFR* targetObject, GFxFunctionHandler::Args *args)
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
   bookSkill.append(GetBookSkill(pTargetReference).c_str());

   SetResultString(args,
      bookSkill.c_str());
};

void CAHZScaleform::SetResultString(GFxFunctionHandler::Args *args, const char * str)
{
   args->result->SetString(str);
};

void CAHZScaleform::ReplaceStringInPlace(std::string& subject, const std::string& search,
   const std::string& replace)
{
   size_t pos = 0;
   while ((pos = subject.find(search, pos)) != std::string::npos)
   {
      subject.replace(pos, search.length(), replace);
      pos += replace.length();
   }
};

void CAHZScaleform::ProcessTargetObject(TESObjectREFR* targetObject, GFxFunctionHandler::Args *args)
{
   TESObjectREFR * pTargetReference = targetObject;
   float totalArmorOrWeapon = 0.0;
   float difference = 0.0;
   float totalWarmthRating = 0.0;
   float warmthDifference = 0.0;
   bool isSurvivalMode = false;
   float warmthRating = 0.0;

   if (!args)
   {
      return;
   }

   if (!targetObject)
   {
      args->args[0].DeleteMember("targetObj");
      return;
   }

   GFxValue obj;
   args->movie->CreateObject(&obj);

   if (pTargetReference->baseForm->GetFormType() == kFormType_Weapon ||
      pTargetReference->baseForm->GetFormType() == kFormType_Ammo ||
	   pTargetReference->baseForm->GetFormType() == kFormType_Projectile)
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

      if (IsSurvivalMode())
      {
         isSurvivalMode = true;
         totalWarmthRating = GetTotalWarmthRating();
         warmthDifference = GetWarmthRatingDiff(pTargetReference);
         warmthRating = GetArmorWarmthRating(pTargetReference);
      }

   }

   // Enter the data into the Scaleform function
   RegisterNumber(&obj, "ratingOrDamage", totalArmorOrWeapon);
   RegisterNumber(&obj, "difference", difference);
   RegisterNumber(&obj, "totalWarmthRating", totalWarmthRating);
   RegisterNumber(&obj, "warmthRating", warmthRating);
   RegisterNumber(&obj, "warmthDifference", warmthDifference);
   RegisterBoolean(&obj, "isSurvivalMode", isSurvivalMode);

   float weight = CALL_MEMBER_FN(pTargetReference, GetWeight)();
   if (pTargetReference->extraData.HasType(kExtraData_Count))
   {
      ExtraCount* xCount = static_cast<ExtraCount*>(pTargetReference->extraData.GetByType(kExtraData_Count));
      if (xCount)
      {
         weight = weight * (float)(SInt16)(xCount->count & 0x7FFF);
      }
   }

   RegisterNumber(&obj, "objWeight", weight);

   // Used by the scaleform script to know if this is a weapon, armor, or something else
   RegisterNumber(&obj, "formType", pTargetReference->baseForm->GetFormType());
   args->args[0].SetMember("targetObj", &obj);
};

void CAHZScaleform::BuildIngredientObject(IngredientItem* ingredient, GFxFunctionHandler::Args *args)
{
   if (!args)
   {
      return;
   }

   // If no ingredient, then we are done here
   if (!ingredient)
   {
      args->args[0].DeleteMember("ingredientObj");
      return;
   }

   string strings[4];

   // Not all ingredients have 4 effects
   UInt32 effectsCount = ingredient->effectItemList.count;

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
               strings[i].append(pFullName->name.data); 
            }
         }
      }
   }
   GFxValue obj2;
   args->movie->CreateObject(&obj2);

   if (effectsCount >= 1)
      RegisterString(&obj2, args->movie, "effect1", strings[0].c_str());

   if (effectsCount >= 2)
      RegisterString(&obj2, args->movie, "effect2", strings[1].c_str());

   if (effectsCount >= 3)
      RegisterString(&obj2, args->movie, "effect3", strings[2].c_str());

   if (effectsCount >= 4)
      RegisterString(&obj2, args->movie, "effect4", strings[3].c_str());
   args->args[0].SetMember("ingredientObj", &obj2);
};

void CAHZScaleform::BuildInventoryObject(TESForm* form, GFxFunctionHandler::Args *args)
{
    if (!args)
    {
        return;
    }

    // Used to store the name
    string name;

    // Used to store the count of the item
    UInt32 itemCount;

    TESObjectREFR * reference = AHZGetReference(form);

    if (reference)
    {
        // Get the number of this in the inventory
        itemCount = CAHZPlayerInfo::GetItemAmount(reference->baseForm->formID);
        name = CAHZScaleform::GetTargetName(reference);
    }
    else if (form)
    {
        // Get the number of this in the inventory
        itemCount = CAHZPlayerInfo::GetItemAmount(form->formID);
        TESFullName* pFullName = DYNAMIC_CAST(form, TESForm, TESFullName);
        if (pFullName)
        {
            name.append(pFullName->name.data);
        }
    }

    // If the name contains a string
    if (name.length())
    {
        GFxValue obj;
        args->movie->CreateObject(&obj);

        RegisterString(&obj, args->movie, "inventoryName", name.c_str());
        RegisterNumber(&obj, "inventoryCount", itemCount);

        // Add the object to the scaleform function
        args->args[0].SetMember("inventoryObj", &obj);
    }
    else
    {
        args->args[0].DeleteMember("inventoryObj");
    }
}

void CAHZScaleform::RegisterString(GFxValue * dst, GFxMovieView * view, const char * name, const char * str)
{
    GFxValue fxValue;
    fxValue.SetString(str);
    dst->SetMember(name, &fxValue);
}

void CAHZScaleform::RegisterNumber(GFxValue * dst, const char * name, double value)
{
    GFxValue fxValue;
    fxValue.SetNumber(value);
    dst->SetMember(name, &fxValue);
}

void CAHZScaleform::RegisterBoolean(GFxValue * dst, const char * name, bool value)
{
    GFxValue fxValue;
    fxValue.SetBool(value);
    dst->SetMember(name, &fxValue);
}

void CAHZScaleform::ProcessValidTarget(TESObjectREFR* targetObject, GFxFunctionHandler::Args *args)
{
    TESObjectREFR * pTargetReference = targetObject;

    if (!args)
    {
        return;
    }

    if (!pTargetReference)
    {
        // return false, indicating that the target object is not valid for acquiring data
        args->result->SetBool(false);
        args->args[0].DeleteMember("dataObj");
        return;
    }

    bool canCarry = false;
    bool isActivator = false;

    TESForm *targetForm = AHZGetForm(pTargetReference);
    TESForm *spellItem = NULL;

    if (pTargetReference->baseForm && pTargetReference->baseForm->GetFormType() == kFormType_Activator && targetForm)
    {
        isActivator = true;
    }

    // If the target is not valid or it can't be picked up by the player
    if ((canCarry = (GetIngredient(targetForm) != NULL)) ||
        (canCarry = (GetAlchemyItem(targetForm) != NULL)) ||
        (canCarry = CanPickUp(pTargetReference->baseForm) ||
        (isActivator)) ||
            ((spellItem = GetSpellItem(targetForm)) != NULL))
    {
        if (isActivator && !CanPickUp(targetForm))
        {
            canCarry = false;
        }

        if (spellItem)
        {
            canCarry = false;
        }

        GFxValue obj;
        args->movie->CreateObject(&obj);

        RegisterBoolean(&obj, "canCarry", canCarry);

        // Add the object to the scaleform function
        args->args[0].SetMember("outObj", &obj);

        // return false, indicating that the target object is not valid for acquiring data
        args->result->SetBool(true);
    }
    else
    {
        // Add the object to the scaleform function
        args->args[0].DeleteMember("outObj");

        // The object is valid
        args->result->SetBool(false);
    }
}

void CAHZScaleform::ProcessPlayerData(GFxFunctionHandler::Args *args)
{
    if (!args)
    {
        return;
    }

    GFxValue obj;
    args->movie->CreateObject(&obj);

    UInt32 actorValue = LookupActorValueByName("InventoryWeight");
    float encumbranceNumber = ((*g_thePlayer)->actorValueOwner.GetCurrent(actorValue));
    actorValue = LookupActorValueByName("CarryWeight");
    float maxEncumbranceNumber = ((*g_thePlayer)->actorValueOwner.GetCurrent(actorValue));

    // Enter the data into the Scaleform function
    RegisterNumber(&obj, "encumbranceNumber", encumbranceNumber);
    RegisterNumber(&obj, "maxEncumbranceNumber", maxEncumbranceNumber);
    RegisterNumber(&obj, "goldNumber", CAHZPlayerInfo::GetGoldAmount());
    args->args[0].SetMember("playerObj", &obj);
}

void CAHZScaleform::GetMagicItemDescription(MagicItem * item, std::string& description)
{
    string outerString = "";
    description.clear();

    BSString temp;

    GetMagicItemDescription2(NULL, item, &temp);
    char *temp2 = ProcessSurvivalMode(&temp);

    description.append(temp.Get());
}

void CAHZScaleform::FormatDescription(std::string& unFormated, std::string& formatted)
{
    string outerString = "";
    formatted.clear();

    const char numberFormatter[] = "<font face = '$EverywhereMediumFont' size = '20' color = '#FFFFFF'>%.0f</font>";
    const char stringFormatter[] = "<font face = '$EverywhereMediumFont' size = '20' color = '#FFFFFF'>%s</font>";
    char tempformatter[1000];
    bool canBeAdded = true;

    std::regex survivalRegex("\\[SURV=.+\\]");
    std::smatch survivalMatch;
    const string survivalConst = const_cast<string &>(unFormated);
    if ((regex_search(survivalConst.begin(), survivalConst.end(), survivalMatch, survivalRegex)))
    {
        ReplaceStringInPlace(unFormated, "[SURV=", "");
        size_t offset = (size_t)(unFormated.length() - 1);
        size_t count = 1;
        unFormated.erase(offset, count);
        canBeAdded = IsSurvivalMode();
    }
    else
    {
        canBeAdded = true;
    }

    if (canBeAdded)
    {
        std::regex rgx("\\<\\d+?\\.?\\d*\\>|\\<\\w*\\>");
        std::smatch match;

        const string cs = const_cast<string &>(unFormated);
        string::const_iterator searchStart(cs.cbegin());
        string workingString = unFormated;

        while (regex_search(searchStart, cs.end(), match, rgx))
        {
            string temps = match[0];
            ReplaceStringInPlace(temps, "<", "");
            ReplaceStringInPlace(temps, ">", "");
            string origMatch = match[0];

            sprintf_s(tempformatter, 1000, stringFormatter, temps.c_str());
            ReplaceStringInPlace(workingString, origMatch, tempformatter);

            searchStart += match.position() + match.length();
        }
        outerString.append(workingString);
    }

    //TrimHelper(outerString);
    formatted.append(outerString);
}