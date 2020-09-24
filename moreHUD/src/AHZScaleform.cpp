#include "PCH.h"

#include <wchar.h>
#include <stdlib.h>
#include <sstream>
#include <iostream>
#include <list>
#include <algorithm>
#include "AHZArmorInfo.h"
#include "AHZWeaponInfo.h"
#include "AHZScaleform.h"
#include "AHZScaleformHook.h"
#include "AHZFormLookup.h"
#include "AHZUtilities.h"
#include "ActorValueList.h"

static std::map<uint8_t, string> m_soulMap;

//RelocAddr<GET_ACTOR_WARMTH_RATING> GetActorWarmthRating_Native(0x003BD5F0);
//RelocAddr<GET_ARMOR_WARMTH_RATING>GetArmorWarmthRating_Native(0x003BD510);
//RelocAddr<GET_MAGIC_ITEM_DESCRIPTION> GetMagicItemDescription2(0x00892CD0);
//RelocAddr<PROCESS_SURVIVAL_MODE> ProcessSurvivalMode(0x008930F0);
//RelocAddr<_IsSurvivalMode> IsSurvivalMode(0x008DAAB0);


//src\main.cpp(125) :
//    [trace] GetActorWarmthRating_Native : 25834 src\main.cpp(126) :
//    [trace] GetArmorWarmthRating_Native : 25833 src\main.cpp(127) :
//    [trace] GetMagicItemDescription2 : 51022 src\main.cpp(128) :
//    [trace] ProcessSurvivalMode : 51023 src\main.cpp(129) :
//    [trace] IsSurvivalMode : 52058

void  CAHZScaleform::GetMagicItemDescription2(void*, RE::TESForm* a1, RE::BSString* a2)
{
    using func_t = decltype(&CAHZScaleform::GetMagicItemDescription2);
    REL::Relocation<func_t> func{ REL::ID(51022) };
    func(nullptr, a1, a2);
}

 char* CAHZScaleform::ProcessSurvivalMode(RE::BSString* a2)
 {
     using func_t = decltype(&CAHZScaleform::ProcessSurvivalMode);
     REL::Relocation<func_t> func{ REL::ID(51023) };
     return func(a2);
 }

 bool  CAHZScaleform::IsSurvivalMode()
 {
     using func_t = decltype(&CAHZScaleform::IsSurvivalMode);
     REL::Relocation<func_t> func{ REL::ID(52058) };
     return func();
 }
 float CAHZScaleform::GetArmorWarmthRating_Native(RE::TESForm* a1) 
 {
     using func_t = decltype(&CAHZScaleform::GetArmorWarmthRating_Native);
     REL::Relocation<func_t> func{ REL::ID(25833) };
     return func(a1);
 }
 float CAHZScaleform::GetActorWarmthRating_Native(RE::Actor* a1, float s2) 
 {
     using func_t = decltype(&CAHZScaleform::GetActorWarmthRating_Native);
     REL::Relocation<func_t> func{ REL::ID(25834) };
     return func(a1, s2);
 }


float CAHZScaleform::GetBaseDamage(RE::TESAmmo* pthisAmmo)
{
    return pthisAmmo->data.damage;
}

string CAHZScaleform::GetSoulLevelName(uint8_t soulLevel)
{
	if (m_soulMap.empty()) //Cache it,  No need to hit the game setting every time
	{
        auto                  settings = RE::GameSettingCollection::GetSingleton();
		m_soulMap[1] = string(settings->GetSetting("sSoulLevelNamePetty")->GetString());
        m_soulMap[2] = string(settings->GetSetting("sSoulLevelNameLesser")->GetString());
        m_soulMap[3] = string(settings->GetSetting("sSoulLevelNameCommon")->GetString());
        m_soulMap[4] = string(settings->GetSetting("sSoulLevelNameGreater")->GetString());
        m_soulMap[5] = string(settings->GetSetting("sSoulLevelNameGrand")->GetString());
	}
	if (m_soulMap.find(soulLevel) == m_soulMap.end())
	{
		return string("");
	}
	return m_soulMap[soulLevel];
}

bool MagicDisallowEnchanting(RE::BGSKeywordForm *pKeywords)
{
	if (pKeywords)
	{
		for (uint32_t k = 0; k < pKeywords->numKeywords; k++) {
			if (pKeywords->keywords[k]) {
				auto keyword = pKeywords->GetKeywordAt(k).value_or(nullptr);
                if (keyword) {
                    string keyWordName = keyword->GetFormEditorID();
                    if (keyWordName == "MagicDisallowEnchanting")
                    {
                        return true;  // Is enchanted, but cannot be enchanted by player
                    }
                }
			}
		}
	}
	return false;
}

uint32_t CAHZScaleform::GetIsKnownEnchantment(RE::TESObjectREFR *targetRef)
{
    auto     pPC = RE::PlayerCharacter::GetSingleton();
	RE::TESForm *baseForm;
	if (pPC && targetRef && (baseForm = targetRef->GetBaseObject()) &&
        (baseForm->GetFormType() == RE::FormType::Weapon || baseForm->GetFormType() == RE::FormType::Armor || baseForm->GetFormType() == RE::FormType::Ammo || baseForm->GetFormType() == RE::FormType::Projectile))
	{
		RE::EnchantmentItem * enchantment = nullptr;
        auto enchantable = baseForm->As<RE::TESEnchantableForm>();
        if (baseForm->GetFormType() == RE::FormType::Projectile)
            enchantable = AHZGetForm(targetRef)->As<RE::TESEnchantableForm>();
		
		bool wasExtra = false;
		if (enchantable) { // Check the item for a base enchantment
			enchantment = enchantable->formEnchanting;
		}
		if (auto extraEnchant = static_cast<RE::ExtraEnchantment*>(targetRef->extraList.GetByType(RE::ExtraDataType::kEnchantment)))
		{
			wasExtra = true;
            enchantment = extraEnchant->enchantment;
		}

		if (enchantment)
		{
            if ((enchantment->formFlags & RE::TESForm::RecordFlags::kKnown) == RE::TESForm::RecordFlags::kKnown) {
                return MagicDisallowEnchanting(DYNAMIC_CAST(enchantment, RE::EnchantmentItem, RE::BGSKeywordForm)) ? 2 : 1;
            } else if (MagicDisallowEnchanting(DYNAMIC_CAST(enchantment, RE::EnchantmentItem, RE::BGSKeywordForm)))
			{
				return 2;
			}

			auto baseEnchantment = static_cast<RE::EnchantmentItem *>(enchantment->data.baseEnchantment);
			if (baseEnchantment)
			{
                if ((baseEnchantment->formFlags & RE::TESForm::RecordFlags::kKnown) == RE::TESForm::RecordFlags::kKnown) {
					return MagicDisallowEnchanting(DYNAMIC_CAST(baseEnchantment, RE::EnchantmentItem, RE::BGSKeywordForm)) ? 2 : 1;
				}
				else if (MagicDisallowEnchanting(DYNAMIC_CAST(baseEnchantment, RE::EnchantmentItem, RE::BGSKeywordForm)))
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
            // TODO: Opps Bet this doen't work.  
            // It probably always returned nullptr prior to the port.  It use to use the old DYNAMIC_CAST but the TESEnchantanbleForm does derive fromRE::TESForm
            // Just sticking this in for now.  The nullptr is checked
            return MagicDisallowEnchanting(dynamic_cast<RE::BGSKeywordForm*>(enchantable)) ? 2 : 0;
		}

	}
	return 0;
}
double CAHZScaleform::GetActualDamage(AHZWeaponData *weaponData)
{
   if (!weaponData)
      return 0.0;

   auto pPC = RE::PlayerCharacter::GetSingleton();

   if (pPC)
   {
      RE::InventoryEntryData objDesc(weaponData->equipData.boundObject, 0);

      // Allocate a list to send
      // TODO: Port Note:  This may need to be evaluated again we may not need to do this anymore
      objDesc.extraLists = new RE::BSSimpleList<RE::ExtraDataList*>();

      if (weaponData->equipData.pExtraData)
      {
          objDesc.extraLists->push_front(weaponData->equipData.pExtraData);
      }

      float fDamage = pPC->GetDamage(&objDesc);

      // Delete the allocated dummy list
      delete objDesc.extraLists;

      // This could be rounded, but the the script decide
      return mRound(fDamage);
   }

   return 0.0;
}

double CAHZScaleform::GetArmorWarmthRating(AHZArmorData* armorData)
{
   if (!armorData->armor)
      return 0.0;

   RE::InventoryEntryData objDesc(armorData->equipData.boundObject, 0);

   // Allocate a dummy list so skyrim does not crash. For armor information
   // skyrim doesn't appear to need the list
   // TODO: Port Note:  This may need to be evaluated again we may not need to do this anymore
   objDesc.extraLists = new RE::BSSimpleList<RE::ExtraDataList*>();
   if (armorData->equipData.pExtraData)
   {
       objDesc.extraLists->push_front(armorData->equipData.pExtraData);
   }

   double fRating = GetArmorWarmthRating_Native(objDesc.object);

   // Delete the allocated dummy list
   delete objDesc.extraLists;

   // This could be rounded, but the the script decide
   return mRound(fRating);

   return 0.0;
}

double CAHZScaleform::GetArmorWarmthRating(RE::TESObjectREFR* targetRef)
{
    // TODO: Port Note: Checking if the reference base object is null may not be needed
    if (!targetRef || !targetRef->GetBaseObject())
      return 0.0;

   if (targetRef->GetBaseObject()->GetFormType() != RE::FormType::Armor || !IsSurvivalMode())
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

   auto pPC = RE::PlayerCharacter::GetSingleton();

   if (pPC)
   {
      RE::InventoryEntryData objDesc(armorData->equipData.boundObject, 0);

      // Allocate a dummy list so skyrim does not crash. For armor information
      // skyrim doesn't appear to need the list
      // TODO: Port Note:  This may need to be evaluated again we may not need to do this anymore
      objDesc.extraLists = new RE::BSSimpleList<RE::ExtraDataList*>();
      if (armorData->equipData.pExtraData)
      {
          objDesc.extraLists->push_front(armorData->equipData.pExtraData);
      }

      double fRating = pPC->GetArmorValue(&objDesc);

      // Delete the allocated dummy list
      delete objDesc.extraLists;

      // This could be rounded, but the the script decide
      return mRound(fRating);
   }

   return 0.0;
}

// TODO: Port Note, I should probably decode the function that already does this
double CAHZScaleform::GetTotalActualArmorRating(void)
{
   double totalRating = 0.0;
   double rating = 0.0;
   // Keep a list of items to make sure they are not added more than once
   // Some items take up more than one slot
   std::list<RE::TESForm*> clist;
   for (uint64_t slot = 1; slot <= static_cast<uint64_t>(0x2000); slot <<= 1)
   {
      AHZArmorData armorData = CAHZArmorInfo::GetArmorFromSlotMask(static_cast<RE::BIPED_MODEL::BipedObjectSlot>(slot));
      if (armorData.equipData.boundObject)
      {
         if (find(clist.begin(), clist.end(), armorData.equipData.boundObject) == clist.end())
         {
            clist.push_front(armorData.equipData.boundObject);
            if (armorData.armor)
            {
               totalRating += GetActualArmorRating(&armorData);
            }
         }
      }
   }
   return mRound(totalRating);
}

// TODO: Port Note, I should probably decode the function that already does this
double CAHZScaleform::GetTotalWarmthRating(void)
{
    auto pPC = RE::PlayerCharacter::GetSingleton();

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

double CAHZScaleform::GetArmorRatingDiff(RE::TESObjectREFR *thisArmor)
{
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
       armorData.armor->GetSlotMask());
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

double CAHZScaleform::GetWarmthRatingDiff(RE::TESObjectREFR *thisArmor)
{
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
      armorData.armor->GetSlotMask());
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

bool CAHZScaleform::isBolt(RE::TESAmmo *thisAmmo)
{
   return (thisAmmo->IsBolt());
}

double CAHZScaleform::GetWeaponDamageDiff(RE::TESObjectREFR *targetWeaponOrAmmo)
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
         auto tempDamage = GetActualDamage(&leftWeapon);
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

bool CAHZScaleform::IsTwoHanded(RE::TESObjectWEAP * thisWeapon)
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
   //return (thisWeapon->type() == RE::TESObjectWEAP::GameData::kType_TwoHandSword ||
   //   thisWeapon->type() == TESObjectWEAP::GameData::kType_TwoHandAxe ||
   //   thisWeapon->type() == TESObjectWEAP::GameData::kType_Bow ||
   //   thisWeapon->type() == TESObjectWEAP::GameData::kType_CrossBow ||
   //   thisWeapon->type() == TESObjectWEAP::GameData::kType_2HS ||
   //   thisWeapon->type() == TESObjectWEAP::GameData::kType_2HA ||
   //   thisWeapon->type() == TESObjectWEAP::GameData::kType_Bow2 ||
   //   thisWeapon->type() == TESObjectWEAP::GameData::kType_CBow);


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

bool CAHZScaleform::IsOneHanded(RE::TESObjectWEAP * thisWeapon)
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

bool CAHZScaleform::IsBow(RE::TESObjectWEAP * thisWeapon)
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

bool CAHZScaleform::IsCrossBow(RE::TESObjectWEAP * thisWeapon)
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

RE::AlchemyItem * CAHZScaleform::GetAlchemyItemFromLeveledList(RE::TESForm *thisObject)
{
    if (thisObject->formType == RE::FormType::LeveledItem)
    {
        RE::TESLevItem *lvli = DYNAMIC_CAST(thisObject, RE::TESForm, RE::TESLevItem);

        // Get the first form and see if it is an ingredient
        if (lvli && lvli->numEntries > 0)
        {
            for (int i = 0; i < lvli->numEntries; i++)
            {
                auto itemform = lvli->entries[i].form;
                if (itemform)
                {
                    if (itemform->formType == RE::FormType::AlchemyItem)
                    {
                        auto alchemyItem = DYNAMIC_CAST(itemform, RE::TESForm, RE::AlchemyItem);
                        return alchemyItem;
                    }
                }
            }
        }
    }

    return NULL;
}

RE::IngredientItem* CAHZScaleform::GetIngredientFromLeveledList(RE::TESForm *thisObject)
{
    if (thisObject->formType == RE::FormType::LeveledItem)
    {
        auto lvli = DYNAMIC_CAST(thisObject, RE::TESForm, RE::TESLevItem);

        // Get the first form and see if it is an ingredient
        if (lvli && lvli->numEntries > 0)
        {
            for (int i = 0; i < lvli->numEntries; i++)
            {
                auto itemform = lvli->entries[i].form;
                if (itemform)
                {
                    if (itemform->formType == RE::FormType::Ingredient)
                    {
                        auto *ingredient = DYNAMIC_CAST(itemform, RE::TESForm, RE::IngredientItem);
                        return ingredient;
                    }
                }
            }
        }
    }

    return NULL;
}

RE::IngredientItem* CAHZScaleform::GetIngredient(RE::TESForm *thisObject)
{
    if (!thisObject)
        return NULL;

    auto reference = AHZGetReference(thisObject);
    if (reference)
    {
        thisObject = reference->GetBaseObject();
    }

    if (!thisObject) {
        return nullptr;
    }

    if (thisObject->GetFormType() == RE::FormType::Ingredient)
        return DYNAMIC_CAST(thisObject, RE::TESForm, RE::IngredientItem);

    if (thisObject->GetFormType() == RE::FormType::Flora)
    {
        // Skip if harvested
        if (reference && (reference->formFlags & RE::TESObjectREFR::RecordFlags::kHarvested))
        {
            return nullptr;
        }

        RE::TESFlora *flora = DYNAMIC_CAST(thisObject, RE::TESForm, RE::TESFlora);
        if (flora)
        {
            auto form = static_cast<RE::TESForm*>(flora->produceItem);

            if (!form) {
                return nullptr;
            }

            // If the ingredient is actually an ingredient
            if (form->formType == RE::FormType::Ingredient)
            {
                return DYNAMIC_CAST(form, RE::TESForm, RE::IngredientItem);
            }

            // If the ingredient is actually a levelitem (Harvest overhaul mod or a coin purse)
            else if (form->formType == RE::FormType::LeveledItem)
            {
                auto itemform = GetIngredientFromLeveledList(form);

                if (!itemform)
                {
                    return nullptr;
                } else if (itemform->formType == RE::FormType::Ingredient)
                {
                    auto ingredient = DYNAMIC_CAST(itemform, RE::TESForm, RE::IngredientItem);
                    return ingredient;
                }
            } else if (form->formType == RE::FormType::FormList)
            {
                auto lvli = DYNAMIC_CAST(form, RE::TESForm, RE::BGSListForm);

                // Get the first form and see if it is an ingredient
                if (lvli->forms.size() > 0)
                {
                    auto itemform = lvli->forms[0];
                    if (itemform)
                    {
                        auto ingredient = DYNAMIC_CAST(itemform, RE::TESForm, RE::IngredientItem);
                        return ingredient;
                    }
                }
            }
        }
    } else if (thisObject->GetFormType() == RE::FormType::Tree)
    {
        // Skip if harvested
        if (reference && (reference->formFlags & RE::TESObjectREFR::RecordFlags::kHarvested))
        {
            return nullptr;
        }

        auto tree = DYNAMIC_CAST(thisObject, RE::TESForm, RE::TESObjectTREE);
        if (tree)
        {
           auto form = static_cast<RE::TESForm*>(tree->produceItem);//DYNAMIC_CAST(tree->produce.produce, IngredientItem,RE::TESForm);

            if (!form) {
                return nullptr;
            }

            // If the ingredient is actually an ingredient
            if (form->formType == RE::FormType::Ingredient)
            {
                return DYNAMIC_CAST(form, RE::TESForm, RE::IngredientItem);
            }

            // If the ingredient is actually a levelitem (Harvest overhaul mod or a coin purse)
            else if (form->formType == RE::FormType::LeveledItem)
            {
                auto itemform = GetIngredientFromLeveledList(form);

                if (!itemform)
                {
                    return nullptr;
                } else if (itemform->formType == RE::FormType::Ingredient)
                {
                    auto ingredient = DYNAMIC_CAST(itemform, RE::TESForm, RE::IngredientItem);
                    return ingredient;
                }
            } else if (form->formType == RE::FormType::FormList)
            {
                auto lvli = DYNAMIC_CAST(form, RE::TESForm, RE::BGSListForm);

                if (!lvli) {
                    return nullptr;
                }

                // Get the first form and see if it is an ingredient
                if (lvli->forms.size() > 0)
                {
                    auto itemform = lvli->forms[0];
                    if (itemform)
                    {
                        auto ingredient = DYNAMIC_CAST(itemform, RE::TESForm, RE::IngredientItem);
                        return ingredient;
                    }
                }
            }
        }
    }

    return nullptr;
}

RE::SpellItem* CAHZScaleform::GetSpellItem(RE::TESForm *thisObject)
{
   if (!thisObject)
        return nullptr;

   auto reference = AHZGetReference(thisObject);
   if (reference)
   {
      thisObject = reference->GetBaseObject();
   }

   if (!thisObject)
	   return nullptr;

   return DYNAMIC_CAST(thisObject, RE::TESForm, RE::SpellItem);
}

RE::AlchemyItem* CAHZScaleform::GetAlchemyItem(RE::TESForm *thisObject)
{
    if (!thisObject)
        return nullptr;

    auto reference = AHZGetReference(thisObject);
    if (reference) {
        thisObject = reference->GetBaseObject();
    }

    if (!thisObject) {
        return nullptr;
    }

    if (thisObject->GetFormType() == RE::FormType::AlchemyItem)
        return DYNAMIC_CAST(thisObject, RE::TESForm, RE::AlchemyItem);

    if (thisObject->GetFormType() == RE::FormType::Flora)
    {
        // Skip if harvested
        if (reference && (reference->formFlags & RE::TESObjectREFR::RecordFlags::kHarvested))
        {
            return nullptr;
        }

        auto flora = DYNAMIC_CAST(thisObject, RE::TESForm, RE::TESFlora);
        if (flora)
        {
            auto form = static_cast<RE::TESForm*>(flora->produceItem);

            if (!form) {
                return nullptr;
            }

            // If the food is actually food
            if (form->formType == RE::FormType::AlchemyItem)
            {
                return DYNAMIC_CAST(form, RE::TESForm, RE::AlchemyItem);
            }

            // If the food is actually a levelitem (Harvest overhaul mod or a coin purse)
            else if (form->formType == RE::FormType::LeveledItem)
            {
                auto itemform = GetAlchemyItemFromLeveledList(form);

                if (!itemform)
                {
                    return nullptr;
                } else if (itemform->formType == RE::FormType::AlchemyItem)
                {
                    auto alchmyItem = DYNAMIC_CAST(itemform, RE::TESForm, RE::AlchemyItem);
                    return alchmyItem;
                }
            } else if (form->formType == RE::FormType::FormList)
            {
                auto lvli = DYNAMIC_CAST(form, RE::TESForm, RE::BGSListForm);

                if (!lvli) {
                    return nullptr;
                }

                // Get the first form and see if it is an ingredient
                if (lvli->forms.size() > 0)
                {
                    auto itemform = lvli->forms[0];
                    if (itemform)
                    {
                        auto alchmyItem = DYNAMIC_CAST(itemform, RE::TESForm, RE::AlchemyItem);
                        return alchmyItem;
                    }
                }
            }
        }
    } else if (thisObject->GetFormType() == RE::FormType::Tree)
    {
        // Skip if harvested
        if (reference && (reference->formFlags & RE::TESObjectREFR::RecordFlags::kHarvested))
        {
            return nullptr;
        }

        auto tree = DYNAMIC_CAST(thisObject, RE::TESForm, RE::TESObjectTREE);
        if (tree)
        {
            auto form = static_cast<RE::TESForm*>(tree->produceItem);

            if (!form) {
                return nullptr;
            }

            // If the produce is actually food
            if (form->formType == RE::FormType::AlchemyItem)
            {
                return DYNAMIC_CAST(form, RE::TESForm, RE::AlchemyItem);
            }

            // If the ingredient is actually a levelitem (Harvest overhaul mod or a coin purse)
            else if (form->formType == RE::FormType::LeveledItem)
            {
                auto itemform = GetAlchemyItemFromLeveledList(form);

                if (!itemform)
                {
                    return nullptr;
                } else if (itemform->formType == RE::FormType::AlchemyItem)
                {
                    auto alchmyItem = DYNAMIC_CAST(itemform, RE::TESForm, RE::AlchemyItem);
                    return alchmyItem;
                }
            } else if (form->formType == RE::FormType::FormList)
            {
                auto lvli = DYNAMIC_CAST(form, RE::TESForm, RE::BGSListForm);

                if (!lvli) {
                    return nullptr;
                }

                // Get the first form and see if it is an ingredient
                if (lvli->forms.size() > 0)
                {
                    auto itemform = lvli->forms[0];
                    if (itemform)
                    {
                        auto alchmyItem = DYNAMIC_CAST(itemform, RE::TESForm, RE::AlchemyItem);
                        return alchmyItem;
                    }
                }
            }
        }
    }

    return NULL;
}

bool CAHZScaleform::CanPickUp(RE::TESForm* form)
{
	if (!form) {
		return false;
	}

	auto formType = form->GetFormType();

	bool canCarry = (formType == RE::FormType::Weapon ||
                     formType == RE::FormType::Armor ||
                     formType == RE::FormType::SoulGem ||
                     formType == RE::FormType::AlchemyItem ||
                     formType == RE::FormType::Misc ||
                     formType == RE::FormType::Ingredient ||
                     formType == RE::FormType::Book ||
                     formType == RE::FormType::Ammo ||
                     formType == RE::FormType::Scroll ||
                     formType == RE::FormType::Outfit ||
                     formType == RE::FormType::KeyMaster ||
                     formType == RE::FormType::Projectile);  // Projectiles with the "Can Pick Up" flag set to false will not even register in the crossshairs
	return canCarry;
}

string CAHZScaleform::GetTargetName(RE::TESForm *thisObject)
{
   string name;
   
   auto reference = AHZGetReference(thisObject);

   if (!reference)
   {
      return name;
   }

   auto pFullName = DYNAMIC_CAST(reference->GetBaseObject(), RE::TESForm, RE::TESFullName);

   const char* displayName = reference->extraList.GetDisplayName(reference->GetBaseObject());

   // If the name can be created
   if (displayName)
   {
      name.append(displayName);
   }
   // Use the base name
   else if (pFullName)
   {
      name.append(pFullName->GetFullName());
   }
   else // second attempt to get the name
   {
       // TODO Port Note:  There is no GetFullName at theRE::TESForm level
       //auto asForm = static_cast<RE::TESForm*>(reference->GetBaseObject());
       //name.append(asForm->GetFullName());
   }

   // If this is a soul gem, also get the gem size name
   if (reference->GetBaseObject()->formType == RE::FormType::SoulGem)
   {
      auto asForm = static_cast<RE::TESForm*>(reference->GetBaseObject());
      auto gem = DYNAMIC_CAST(asForm, RE::TESForm, RE::TESSoulGem);
      if (gem)
      {
         string soulName("");
         auto  soulLevel = gem->soulCapacity;
         soulName = GetSoulLevelName(static_cast<uint8_t>(soulLevel.get()));

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

bool CAHZScaleform::GetIsBookAndWasRead(RE::TESObjectREFR *theObject)
{
   if (!theObject)
      return false;

   if (!theObject->GetBaseObject())
	   return false;

   if (theObject->GetBaseObject()->GetFormType() != RE::FormType::Book)
      return false;

   auto item = DYNAMIC_CAST(theObject->GetBaseObject(), RE::TESForm, RE::TESObjectBOOK);
   if (item && (item->IsRead()))
   {
      return true;
   }
   else
   {
      return false;
   }
}

static uint32_t lasttargetRef;

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

void CAHZScaleform::ProcessEnemyInformation(RE::GFxFunctionHandler::Params & args)
{
    auto          pPC = RE::PlayerCharacter::GetSingleton();
	CAHZActorData actorData;
	actorData.Level = 0;
	actorData.IsSentient = 0;
	uint16_t playerLevel = 0;
	uint32_t soulType = 0;

	if (pPC)
	{
		actorData = AHZEnemyHealthUpdateHook::GetCurrentEnemyData();
		if (actorData.Level)
		{
            playerLevel = pPC->GetLevel();
			if (!actorData.IsSentient) {  // If sentient, then don't bother all sentients have grand soul gem levels
				soulType = CAHZActorInfo::GetSoulType(actorData.Level, static_cast<uint8_t>(actorData.IsSentient));
			}
		}
	}

	RE::GFxValue enemyObj;
    RE::GFxValue playerObj;
	args.movie->CreateObject(&enemyObj);
	args.movie->CreateObject(&playerObj);

	RegisterNumber(&enemyObj, "level", actorData.Level);
	RegisterNumber(&playerObj, "level", playerLevel);
	string soulName = GetSoulLevelName(static_cast<uint8_t>(soulType));
	if (soulType && soulName.length())
	{
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

	if (args.args[0].HasMember("player"))
	{
		args.args[0].SetMember("player", &playerObj);
	}
	if (args.args[0].HasMember("enemy"))
	{
		args.args[0].SetMember("enemy", &enemyObj);
	}
}

string CAHZScaleform::GetArmorWeightClass(RE::TESObjectREFR *theObject)
{
   string desc;

   if (!theObject)
      return desc;

   if (!theObject->GetBaseObject())
	   return desc;

   if (theObject->GetBaseObject()->GetFormType() != RE::FormType::Armor)
      return desc;

   auto item = DYNAMIC_CAST(theObject->GetBaseObject(), RE::TESForm, RE::TESObjectARMO);
   if (!item)
      return desc;

   auto avList = SKSE::ActorValueList::GetSingleton();
   if (item->IsLightArmor() || item->IsHeavyArmor())
   {
      // Utilize the AV value to get the localized name for "Light Armor"
      if (item->IsLightArmor())
      {
         auto info = avList->GetActorValue(RE::ActorValue::kLightArmor);
         if (info)
         {
            desc.append("<FONT FACE=\"$EverywhereMediumFont\"SIZE=\"15\"COLOR=\"#999999\"KERNING=\"0\">     ");
            desc.append(info->GetFullName());
            desc.append("<\\FONT>");
         }
      }

      // Utilize the AV value to get the localized name for "Heavy Armor"
      else if (item->IsHeavyArmor())
      {
          auto info = avList->GetActorValue(RE::ActorValue::kHeavyArmor);
          if (info) {
              desc.append("<FONT FACE=\"$EverywhereMediumFont\"SIZE=\"15\"COLOR=\"#999999\"KERNING=\"0\">     ");
              desc.append(info->GetFullName());
              desc.append("<\\FONT>");
          }
      }
   }
   return desc;
};

string CAHZScaleform::GetValueToWeight(RE::TESObjectREFR *theObject, const char * stringFromHUD, const char * vmTranslated)
{
	string desc;

	if (!theObject)
		return desc;

	if (!theObject->GetBaseObject())
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
};

string CAHZScaleform::GetBookSkill(RE::TESObjectREFR *theObject)
{
   string desc;
   if (theObject->GetBaseObject()->GetFormType() == RE::FormType::Book)
   {
       auto item = DYNAMIC_CAST(theObject->GetBaseObject(), RE::TESForm, RE::TESObjectBOOK);

      if (!item)
         return desc;

      // If this is a spell book, then it is not a skill book
      if ((item->data.flags & RE::OBJ_BOOK::Flag::kTeachesSpell) == RE::OBJ_BOOK::Flag::kTeachesSpell)
         return desc;

      if (((item->data.flags & RE::OBJ_BOOK::Flag::kAdvancesActorValue) == RE::OBJ_BOOK::Flag::kAdvancesActorValue) &&
          item->data.teaches.actorValueToAdvance != RE::ActorValue::kNone)
      {
         auto avList = SKSE::ActorValueList::GetSingleton();
         if (avList)
         {
            auto info = avList->GetActorValue(item->data.teaches.actorValueToAdvance);
             if (info) {
                 desc.append("<FONT FACE=\"$EverywhereMediumFont\"SIZE=\"15\"COLOR=\"#999999\"KERNING=\"0\">       ");
                 desc.append(info->GetFullName());
                 desc.append("<\\FONT>");
             }
         }
      }
   }
   return desc;
}

void CAHZScaleform::AppendDescription(RE::TESDescription *desObj, RE::TESForm *parent, std::string& description)
{
   RE::BSString bsDescription;
   string tempString = "";

   if (parent->GetFormType() == RE::FormType::Book)
   {
     // CALL_MEMBER_FN(desObj, Get)(&bsDescription, parent, 0x4D414E43);
      desObj->GetDescription(bsDescription, parent, 0x4D414E43);
      if (!bsDescription.empty())
      {
         tempString.clear();
         tempString.append(bsDescription.c_str());
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
      RE::BSString bsDescription2;
      auto desc = DYNAMIC_CAST(parent, RE::TESForm, RE::TESDescription);
      if (desc)
      {
         //CALL_MEMBER_FN(desc, Get)(&bsDescription2, parent, 0x43534544);
          desc->GetDescription(bsDescription2, parent, 0x43534544);
         tempString.clear();
         if (!bsDescription2.empty())
             tempString.append(bsDescription2.c_str());
         if (tempString != "LOOKUP FAILED!" && tempString.length() > 1)
         {
            string formatted = "";
            FormatDescription(tempString, formatted);
            description.append(formatted);
         }
         else
         {
            //CALL_MEMBER_FN(desc, Get)(&bsDescription2, parent, 0);
            desc->GetDescription(bsDescription2, parent, 0);
            tempString.clear();
            if (&bsDescription2)
                tempString.append(bsDescription2.c_str());
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

string CAHZScaleform::GetEffectsDescription(RE::TESObjectREFR *theObject)
{
   RE::BSString description;
   string effectDescription;
   string  desc;
   string effectsString;
   RE::MagicItem * magicItem = nullptr;

   if (!theObject)
      return desc;

   //tArray<MagicItem::EffectItem*> *effectList = NULL;
   //SettingCollectionMap *settings = *g_gameSettingCollection;

   auto settings = RE::GameSettingCollection::GetSingleton();

   if (theObject->GetBaseObject()->GetFormType() == RE::FormType::AlchemyItem)
   {
      auto item = DYNAMIC_CAST(theObject->GetBaseObject(), RE::TESForm, RE::AlchemyItem);

      if (auto extraEnchant = static_cast<RE::ExtraEnchantment*>(theObject->extraList.GetByType(RE::ExtraDataType::kEnchantment))) // Enchanted
      {
         if (extraEnchant->enchantment)
         {
            GetMagicItemDescription(extraEnchant->enchantment, effectDescription);
            desc.append(effectDescription);
         }
      }

      if (item)
      {
         GetMagicItemDescription(item, effectDescription);
         desc.append(effectDescription);
      }
   } else if (theObject->GetBaseObject()->GetFormType() == RE::FormType::Weapon)
   {
       auto item = DYNAMIC_CAST(theObject->GetBaseObject(), RE::TESForm, RE::TESObjectWEAP);

      // If there was no effects, then display athe description if available
      if (item)
      {
         // Get the description if any (Mostly Dawnguard and Dragonborn stuff uses the descriptions)
         AppendDescription(item, item, desc);
      }

      if (item && !desc.length())
      {
         //Get enchantment description
         if (item && item->formEnchanting)
         {
            GetMagicItemDescription(item->formEnchanting, effectDescription);
            desc.append(effectDescription);
         }

         // Items modified by the player
         else if (auto extraEnchant = static_cast<RE::ExtraEnchantment*>(theObject->extraList.GetByType(RE::ExtraDataType::kEnchantment))) // Enchanted
         {
            if (extraEnchant->enchantment)
            {
                 GetMagicItemDescription(extraEnchant->enchantment, effectDescription);
               desc.append(effectDescription);
            }
         }
      }
   } else if (theObject->GetBaseObject()->GetFormType() == RE::FormType::Armor)
   {
      auto item = DYNAMIC_CAST(theObject->GetBaseObject(), RE::TESForm, RE::TESObjectARMO);

      // If there was no effects, then display athe description if available
      if (item)
      {
         // Get the description if any (Mostly Dawnguard and Dragonborn stuff uses the descriptions)
         AppendDescription(item, item, desc);
      }

      if (item && !desc.length())
      {
         //Get enchantment description
         if (item && item->formEnchanting)
         {
             GetMagicItemDescription(item->formEnchanting, effectDescription);
            desc.append(effectDescription);
         }

         // Items modified by the player
         else if (auto extraEnchant = static_cast<RE::ExtraEnchantment*>(theObject->extraList.GetByType(RE::ExtraDataType::kEnchantment))) // Enchanted
         {
            if (extraEnchant->enchantment)
            {
                 GetMagicItemDescription(extraEnchant->enchantment, effectDescription);
               desc.append(effectDescription);
            }
         }
      }
   } else if (theObject->GetBaseObject()->GetFormType() == RE::FormType::Ammo ||
              theObject->GetBaseObject()->GetFormType() == RE::FormType::Projectile)
   {
	   RE::TESAmmo *item = nullptr;
	   
	   if (theObject->GetBaseObject()->GetFormType() == RE::FormType::Projectile)
		   item = DYNAMIC_CAST(AHZGetForm(theObject), RE::TESForm, RE::TESAmmo);
	   else
           item = DYNAMIC_CAST(theObject->GetBaseObject(), RE::TESForm, RE::TESAmmo);
	   
      if (item)
      {
         // Get the description if any (Mostly Dawnguard and Dragonborn stuff uses the descriptions)
         AppendDescription(item, item, desc);
      }

      if (auto extraEnchant = static_cast<RE::ExtraEnchantment*>(theObject->extraList.GetByType(RE::ExtraDataType::kEnchantment)))  // Enchanted
      {
         if (extraEnchant->enchantment)
         {
            GetMagicItemDescription(extraEnchant->enchantment, effectDescription);
            desc.append(effectDescription);
         }
      }
   } else if (theObject->GetBaseObject()->GetFormType() == RE::FormType::Book)
   {
      auto item = DYNAMIC_CAST(theObject->GetBaseObject(), RE::TESForm, RE::TESObjectBOOK);

      if (item)
      {
         // Get the description if any (Mostly Dawnguard and Dragonborn stuff uses the descriptions)
          AppendDescription(&item->itemCardDescription, item, desc);
      }

      if (item &&
          ((item->data.flags & RE::OBJ_BOOK::Flag::kTeachesSpell) == RE::OBJ_BOOK::Flag::kTeachesSpell) && !desc.length())
      {
         if (item->data.teaches.spell)
         {
            GetMagicItemDescription(item->data.teaches.spell, effectDescription);
            desc.append(effectDescription);
         }
      }
   } else if (theObject->GetBaseObject()->GetFormType() == RE::FormType::Scroll)
   {
      auto item = DYNAMIC_CAST(theObject->GetBaseObject(), RE::TESForm, RE::ScrollItem);
      if (item)
      {
         // Get the description if any (Mostly Dawnguard and Dragonborn stuff uses the descriptions)
         AppendDescription(item, item, desc);

         if (!desc.length())
         {
            GetMagicItemDescription(item, effectDescription);
            desc.append(effectDescription);
         }
      }
   }
   return desc;
};

void CAHZScaleform::ProcessTargetEffects(RE::TESObjectREFR* targetObject, RE::GFxFunctionHandler::Params &args)
{
   RE::TESObjectREFR * pTargetReference = targetObject;
   RE::AlchemyItem *alchemyItem = nullptr;
   RE::SpellItem *spellItem = nullptr;
   RE::IngredientItem *ingredientItem = nullptr;
   string name;


   bool calculateInvenotry = args.args[1].GetBool();

   if (!calculateInvenotry)
   {
      args.args[0].DeleteMember("inventoryObj");
   }

   // No valid reference
   if (!pTargetReference)
   {
      args.args[0].DeleteMember("effectsObj");
      args.args[0].DeleteMember("ingredientObj");
      args.args[0].DeleteMember("inventoryObj");
      return;
   }

   auto targetForm = AHZGetForm(pTargetReference);

   // See if its an ingredient.  Note they are formated differently with known effects;
   if ((ingredientItem = GetIngredient(targetForm)) != nullptr)
   {
      args.args[0].DeleteMember("effectsObj");
      BuildIngredientObject(ingredientItem, args);

      if (calculateInvenotry)
      {
         BuildInventoryObject(ingredientItem, args);
      }

      return;
   }

   // See if its harvestable food
   else if ((alchemyItem = GetAlchemyItem(targetForm)) != nullptr)
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
      AppendDescription(spellItem, spellItem, name);

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
      RE::GFxValue obj;
      args.movie->CreateObject(&obj);

      name = CAHZUtilities::trim(name);

      RegisterString(&obj, "effectsDescription",  name.c_str());

      // Add the object to the scaleform function
      args.args[0].SetMember("effectsObj", &obj);
   }
   else
   {
      args.args[0].DeleteMember("effectsObj");
   }
};

void CAHZScaleform::ProcessArmorClass(RE::TESObjectREFR* targetObject, RE::GFxFunctionHandler::Params &args)
{
   RE::TESObjectREFR * pTargetReference = targetObject;
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

void CAHZScaleform::ProcessValueToWeight(RE::TESObjectREFR* targetObject, RE::GFxFunctionHandler::Params &args)
{
   RE::TESObjectREFR * pTargetReference = targetObject;
   static string valueToWeight;

   // If the target is not valid or it can't be picked up by the player
   if (!pTargetReference)
   {
      SetResultString(args, "");
      return;
   }

   valueToWeight.clear();
   valueToWeight.append(GetValueToWeight(pTargetReference, args.args[0].GetString(), args.args[1].GetString()).c_str());

   SetResultString(args, valueToWeight.c_str());
};

void CAHZScaleform::ProcessBookSkill(RE::TESObjectREFR* targetObject, RE::GFxFunctionHandler::Params &args)
{
   RE::TESObjectREFR * pTargetReference = targetObject;
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

void CAHZScaleform::SetResultString(RE::GFxFunctionHandler::Params &args, const char * str)
{
   args.retVal->SetString(str);
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

void CAHZScaleform::ProcessTargetObject(RE::TESObjectREFR* targetObject, RE::GFxFunctionHandler::Params &args)
{
   RE::TESObjectREFR * pTargetReference = targetObject;
   float totalArmorOrWeapon = 0.0;
   float difference = 0.0;
   float totalWarmthRating = 0.0;
   float warmthDifference = 0.0;
   bool isSurvivalMode = false;
   float warmthRating = 0.0;

   if (!targetObject)
   {
      args.args[0].DeleteMember("targetObj");
      return;
   }

   RE::GFxValue obj;
   args.movie->CreateObject(&obj);

   auto baseForm = pTargetReference->GetBaseObject();

   if (baseForm->GetFormType() == RE::FormType::Weapon ||
       baseForm->GetFormType() == RE::FormType::Ammo ||
       baseForm->GetFormType() == RE::FormType::Projectile)
   {
      RE::TESForm *form = nullptr;
      RE::TESAmmo *ammo = nullptr;

      // If ammo is NULL, it is OK
      totalArmorOrWeapon = GetTotalActualWeaponDamage();
      difference = GetWeaponDamageDiff(pTargetReference);
   } else if (baseForm->GetFormType() == RE::FormType::Armor)
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

   //float weight = CALL_MEMBER_FN(pTargetReference, GetWeight)();
   float weight = pTargetReference->GetWeight();
   if (pTargetReference->extraList.HasType(RE::ExtraDataType::kCount))
   {
      auto xCount = static_cast<RE::ExtraCount*>(pTargetReference->extraList.GetByType(RE::ExtraDataType::kCount));
      if (xCount)
      {
         weight = weight * static_cast<float_t>((static_cast<int16_t>(xCount->count & 0x7FFF)));
      }
   }

   RegisterNumber(&obj, "objWeight", weight);

   // Used by the scaleform script to know if this is a weapon, armor, or something else
   RegisterNumber(&obj, "formType", static_cast<uint32_t>(baseForm->GetFormType()));
   args.args[0].SetMember("targetObj", &obj);
};

bool CAHZScaleform::GetIsNthEffectKnown(RE::IngredientItem* thisMagic, uint32_t index)
{
    bool isKnown = false;

    enum  // type - these are flags
    {
        kType_NoEffect = 0,
        kType_FirstEffect = 1 << 0,
        kType_SecondEffect = 1 << 1,
        kType_ThirdEffect = 1 << 2,
        kType_FourthEffect = 1 << 3
    };

    switch (index) {
    case 0:
        isKnown = ((thisMagic->gamedata.knownEffectFlags & kType_FirstEffect) == kType_FirstEffect);
        break;
    case 1:
        isKnown = ((thisMagic->gamedata.knownEffectFlags & kType_SecondEffect) == kType_SecondEffect);
        break;
    case 2:
        isKnown = ((thisMagic->gamedata.knownEffectFlags & kType_ThirdEffect) == kType_ThirdEffect);
        break;
    case 3:
        isKnown = ((thisMagic->gamedata.knownEffectFlags & kType_FourthEffect) == kType_FourthEffect);
        break;
    default:
        break;
    }
    return isKnown;
}

void CAHZScaleform::BuildIngredientObject(RE::IngredientItem* ingredient, RE::GFxFunctionHandler::Params &args)
{


   // If no ingredient, then we are done here
   if (!ingredient)
   {
      args.args[0].DeleteMember("ingredientObj");
      return;
   }

   string strings[4];

   // Not all ingredients have 4 effects
   uint32_t effectsCount = ingredient->effects.size();

   for (int i = 0; i < 4; i++)
   {
      strings[i].clear();
      if (GetIsNthEffectKnown(ingredient, i))
      {
         //MagicItem::EffectItem* pEI = NULL;
         auto pEI = ingredient->effects[i];
         if (pEI)
         {
            auto pFullName = DYNAMIC_CAST(pEI->baseEffect, RE::TESForm, RE::TESFullName);
            if (pFullName)
            {
               strings[i].append(pFullName->GetFullName()); 
            }
         }
      }
   }
   RE::GFxValue obj2;
   args.movie->CreateObject(&obj2);

   if (effectsCount >= 1)
      RegisterString(&obj2, "effect1", strings[0].c_str());

   if (effectsCount >= 2)
      RegisterString(&obj2, "effect2", strings[1].c_str());

   if (effectsCount >= 3)
      RegisterString(&obj2, "effect3", strings[2].c_str());

   if (effectsCount >= 4)
      RegisterString(&obj2, "effect4", strings[3].c_str());
   args.args[0].SetMember("ingredientObj", &obj2);
};

void CAHZScaleform::BuildInventoryObject(RE::TESForm* form, RE::GFxFunctionHandler::Params &args)
{
    // Used to store the name
    string name;

    // Used to store the count of the item
    uint32_t itemCount = 0;

    auto reference = AHZGetReference(form);

    if (reference)
    {
        // Get the number of this in the inventory
        itemCount = CAHZPlayerInfo::GetItemAmount(reference->GetBaseObject()->formID);
        name = CAHZScaleform::GetTargetName(reference);
    }
    else if (form)
    {
        // Get the number of this in the inventory
        itemCount = CAHZPlayerInfo::GetItemAmount(form->formID);
        auto pFullName = DYNAMIC_CAST(form, RE::TESForm, RE::TESFullName);
        if (pFullName)
        {
            name.append(pFullName->GetFullName());
        }
    }

    // If the name contains a string
    if (name.length())
    {
        RE::GFxValue obj;
        args.movie->CreateObject(&obj);

        RegisterString(&obj, "inventoryName", name.c_str());
        RegisterNumber(&obj, "inventoryCount", itemCount);

        // Add the object to the scaleform function
        args.args[0].SetMember("inventoryObj", &obj);
    }
    else
    {
        args.args[0].DeleteMember("inventoryObj");
    }
}

void CAHZScaleform::RegisterString(RE::GFxValue * dst, const char * name, const char * str)
{
    RE::GFxValue fxValue;
    fxValue.SetString(str);
    dst->SetMember(name, &fxValue);
}

void CAHZScaleform::RegisterNumber(RE::GFxValue * dst, const char * name, double value)
{
    RE::GFxValue fxValue;
    fxValue.SetNumber(value);
    dst->SetMember(name, &fxValue);
}

void CAHZScaleform::RegisterBoolean(RE::GFxValue * dst, const char * name, bool value)
{
    RE::GFxValue fxValue;
    fxValue.SetBoolean(value);
    dst->SetMember(name, &fxValue);
}

void CAHZScaleform::ProcessValidTarget(RE::TESObjectREFR* targetObject, RE::GFxFunctionHandler::Params &args)
{
    RE::TESObjectREFR * pTargetReference = targetObject;

    if (!pTargetReference)
    {
        // return false, indicating that the target object is not valid for acquiring data
        args.retVal->SetBoolean(false);
        args.args[0].DeleteMember("dataObj");
        return;
    }

    bool canCarry = false;
    bool isActivator = false;

    auto targetForm = AHZGetForm(pTargetReference);
    RE::SpellItem* spellItem = nullptr;

    if (pTargetReference->GetBaseObject() && pTargetReference->GetBaseObject()->GetFormType() == RE::FormType::Activator && targetForm)
    {
        isActivator = true;
    }

    // If the target is not valid or it can't be picked up by the player
    if ((canCarry = (GetIngredient(targetForm) != nullptr)) ||
        (canCarry = (GetAlchemyItem(targetForm) != nullptr)) ||
        (canCarry = CanPickUp(pTargetReference->GetBaseObject()) ||
        (isActivator)) ||
        ((spellItem = GetSpellItem(targetForm)) != nullptr))
    {
        if (isActivator && !CanPickUp(targetForm))
        {
            canCarry = false;
        }

        if (spellItem)
        {
            canCarry = false;
        }

        RE::GFxValue obj;
        args.movie->CreateObject(&obj);

        RegisterBoolean(&obj, "canCarry", canCarry);

        // Add the object to the scaleform function
        args.args[0].SetMember("outObj", &obj);

        // return false, indicating that the target object is not valid for acquiring data
        args.retVal->SetBoolean(true);
    }
    else
    {
        // Add the object to the scaleform function
        args.args[0].DeleteMember("outObj");

        // The object is valid
        args.retVal->SetBoolean(false);
    }
}

void CAHZScaleform::ProcessPlayerData(RE::GFxFunctionHandler::Params& args)
{

    RE::GFxValue obj;
    args.movie->CreateObject(&obj);

    auto  player = RE::PlayerCharacter::GetSingleton();
    auto  encumbranceNumber = player->GetActorValue(RE::ActorValue::kInventoryWeight);
    auto  maxEncumbranceNumber = player->GetActorValue(RE::ActorValue::kCarryWeight);

    // Enter the data into the Scaleform function
    RegisterNumber(&obj, "encumbranceNumber", encumbranceNumber);
    RegisterNumber(&obj, "maxEncumbranceNumber", maxEncumbranceNumber);
    RegisterNumber(&obj, "goldNumber", CAHZPlayerInfo::GetGoldAmount());
    args.args[0].SetMember("playerObj", &obj);
}

void CAHZScaleform::GetMagicItemDescription(RE::MagicItem * item, std::string& description)
{
    string outerString = "";
    description.clear();

    RE::BSString temp;

    GetMagicItemDescription2(nullptr, item, &temp);
    char *temp2 = ProcessSurvivalMode(&temp);

    description.append(temp.c_str());
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