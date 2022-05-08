#include "pch.h"
#include "AHZTarget.h"
#include "AHZFormLookup.h"

namespace
{
    bool IsSurvivalMode()
    {
        using TESGlobal = RE::TESGlobal;
        const auto dobj = RE::BGSDefaultObjectManager::GetSingleton();
        const auto survival = dobj ? dobj->GetObject<TESGlobal>(RE::DEFAULT_OBJECT::kSurvivalModeEnabled) : nullptr;
        return survival ? survival->value == 1.0F : false;
    }

    std::string GetSoulLevelName(uint8_t soulLevel)
    {
        static std::map<uint8_t, std::string> s_soulMap;
        if (s_soulMap.empty())  //Cache it,  No need to hit the game setting every time
        {
            auto settings = RE::GameSettingCollection::GetSingleton();
            s_soulMap[1] = std::string(settings->GetSetting("sSoulLevelNamePetty")->GetString());
            s_soulMap[2] = std::string(settings->GetSetting("sSoulLevelNameLesser")->GetString());
            s_soulMap[3] = std::string(settings->GetSetting("sSoulLevelNameCommon")->GetString());
            s_soulMap[4] = std::string(settings->GetSetting("sSoulLevelNameGreater")->GetString());
            s_soulMap[5] = std::string(settings->GetSetting("sSoulLevelNameGrand")->GetString());
        }
        if (s_soulMap.find(soulLevel) == s_soulMap.end()) {
            return std::string{};
        }
        return s_soulMap[soulLevel];
    }

    float mRound(float r)
    {
        return (r >= 0.0f) ? floor(r + 0.5f) : ceil(r - 0.5f);
    }
}

//------------------Native Wrappers -----------------------------

// 1408C2D30 1.6.318
void CAHZTarget::GetMagicItemDescription_Native(void*, RE::TESForm* a1, RE::BSString* a2)
{
   using func_t = decltype(&CAHZTarget::GetMagicItemDescription_Native);
   REL::Relocation<func_t> func{ REL::ID(51900) };
   func(nullptr, a1, a2);
}

// 1408C33D0 1.6.318
char* CAHZTarget::ProcessSurvivalMode_Native(RE::BSString* a2)
{
   using func_t = decltype(&CAHZTarget::ProcessSurvivalMode_Native);
   REL::Relocation<func_t> func{ REL::ID(51901) };
   return func(a2);
}

// 1403D4D60 1.6.318
float CAHZTarget::GetArmorWarmthRating_Native(RE::TESForm* a1)
{
   using func_t = decltype(&CAHZTarget::GetArmorWarmthRating_Native);
   REL::Relocation<func_t> func{ REL::ID(26393) };
   return func(a1);
}

// 1403d4e37  1.6.318
float CAHZTarget::GetActorWarmthRating_Native(RE::Actor* a1, float s2)
{
   using func_t = decltype(&CAHZTarget::GetActorWarmthRating_Native);
   REL::Relocation<func_t> func{ REL::ID(26394) };
   return func(a1, s2);
}

//------------------End Native Wrappers -------------------------



void CAHZTarget::SetTarget(RE::TESObjectREFR* pTargetRef)
{    
    m_pForm = CAHZFormLookup::Instance().GetTESForm(pTargetRef);
    if (m_pForm) {
        if (m_pForm->GetFormType() == RE::FormType::Reference)
        {
            m_pObjectRef = m_pForm->As<RE::TESObjectREFR>();
            if (m_pObjectRef)
                m_pForm = m_pObjectRef->GetBaseObject();
            else
                m_pForm = nullptr;
        } else {
            m_pObjectRef = nullptr;
        }
    } else {
        m_pObjectRef = nullptr;
    }

    
    UpdateTarget();

}

void CAHZTarget::UpdateTarget()
{
    m_target.Reset();
    if (!IsValid()) {
        return;
    }

    auto IsIngredient = GetIngredient() != nullptr;
    auto IsAlchemyItem = GetAlchemyItem() != nullptr;
    auto IsSpellItem = GetSpellItem() != nullptr;
    auto canPickUp = CanPickUp(GetForm());

    // Can carry or what-can-be-harvested can be carried
    m_target.canCarry = IsIngredient || IsAlchemyItem || canPickUp;

    // Spell items are valid but cannot be picked up. e.g. Shrines
    m_target.isValid = m_target.canCarry || IsSpellItem;

    if (!m_target.isValid) {
        return;
    }

    m_target.formType = GetForm()->GetFormType();
    m_target.formId = GetForm()->GetFormID();
    m_target.name = GetTargetName();
    m_target.displayName = m_target.name;

    auto soulName = GetTargetSoulLevelName();

    if (!soulName.empty()) {
        m_target.displayName.append(" (");
        m_target.displayName.append(soulName);
        m_target.displayName.append(")");
    }

    m_target.bookRead = GetIsBookAndWasRead();
    m_target.armorType = GetArmorType();
    m_target.description = GetDescription();
    m_target.effectsDescription = GetEffectsDescription();
    m_target.isSpellTome = GetIsSpellTome();
    m_target.bookSkill = GetBookSkill();
    m_target.damage = GetWeaponDamage();
    m_target.armorRating = GetArmorRating();
    m_target.armorslotMask = GetArmorSlotMask();
    m_target.armorWarmthRating = GetArmorWarmthRating();
    m_target.knownIngredientEffects = GetKnownIngredientEffects();
    m_target.weight = GetWeight();
    m_target.weaponType = GetWeaponType();
    m_target.enchantmentType = GetIsKnownEnchantment();



}

EnchantmentType CAHZTarget::GetIsKnownEnchantment()
{
   EnchantmentType result{ EnchantmentType::None };
   if (!IsValid())
       return result;
   
   auto baseForm = GetForm();

    if (baseForm &&
        (baseForm->GetFormType() == RE::FormType::Weapon ||
            baseForm->GetFormType() == RE::FormType::Armor ||
            baseForm->GetFormType() == RE::FormType::Ammo ||
            baseForm->GetFormType() == RE::FormType::Projectile)) {
        RE::EnchantmentItem* enchantment = nullptr;
        auto                 keyWordForm = baseForm->As<RE::BGSKeywordForm>();
        auto                 enchantable = baseForm->As<RE::TESEnchantableForm>();

        bool wasExtra = false;
        if (enchantable) {  // Check the item for a base enchantment
            enchantment = enchantable->formEnchanting;
        }

        if (IsReference()) {
            if (auto extraEnchant = static_cast<RE::ExtraEnchantment*>(GetReference()->extraList.GetByType(RE::ExtraDataType::kEnchantment))) {
                wasExtra = true;
                enchantment = extraEnchant->enchantment;
            }
        }

        if (enchantment) {
           // It has an enchantment be it may not be known.
            result = EnchantmentType::Unknown;

            if ((enchantment->formFlags & RE::TESForm::RecordFlags::kKnown) == RE::TESForm::RecordFlags::kKnown) {
                return MagicDisallowEnchanting(enchantment) ? EnchantmentType::CannotDisenchant : EnchantmentType::Known;
            } else if (MagicDisallowEnchanting(enchantment)) {
                return EnchantmentType::CannotDisenchant;
            }

            auto baseEnchantment = static_cast<RE::EnchantmentItem*>(enchantment->data.baseEnchantment);
            if (baseEnchantment) {
                if ((baseEnchantment->formFlags & RE::TESForm::RecordFlags::kKnown) == RE::TESForm::RecordFlags::kKnown) {
                    return MagicDisallowEnchanting(baseEnchantment) ? EnchantmentType::CannotDisenchant : EnchantmentType::Known;
                } else if (MagicDisallowEnchanting(baseEnchantment)) {
                    return EnchantmentType::CannotDisenchant;
                }
            }
        }

        // Its safe to assume that if it not a base enchanted item, that it was enchanted by the player and therefore, they
        // know the enchantment
        if (wasExtra) {
            return EnchantmentType::Known;
        } else if (enchantable) {
            return MagicDisallowEnchanting(keyWordForm) ? EnchantmentType::CannotDisenchant : result;
        }
    }
    return result;
}


RE::WEAPON_TYPE CAHZTarget::GetWeaponType()
{
    if (!IsValid()) {
        return RE::WEAPON_TYPE::kHandToHandMelee;
    }

    if (GetForm()->GetFormType() != RE::FormType::Weapon)
        return RE::WEAPON_TYPE::kHandToHandMelee;

    auto item = GetForm()->As<RE::TESObjectWEAP>();

    if (!item)
        return RE::WEAPON_TYPE::kHandToHandMelee;

    return item->weaponData.animationType.get();
}

float CAHZTarget::GetWeight()
{
    if (!IsValid())
        return {};

    float weight = GetForm()->GetWeight();
    if (IsReference() && GetReference()->extraList.HasType(RE::ExtraDataType::kCount)) {
        auto xCount = static_cast<RE::ExtraCount*>(GetReference()->extraList.GetByType(RE::ExtraDataType::kCount));
        if (xCount) {
            weight = weight * static_cast<float_t>((static_cast<int16_t>(xCount->count & 0x7FFF)));
        }
    }

    return weight;
}

std::vector<std::string> CAHZTarget::GetKnownIngredientEffects()
{
   if (!IsValid())
        return {};
   std::vector<std::string> knownEffects;
   auto item = GetIngredient();

   if (!item)
       return {};

   auto effectsCount = static_cast<std::uint32_t>(item->effects.size());
   
   for (std::uint32_t i = 0; i < effectsCount; i++) {
       if (GetIsNthEffectKnown(item, i)) {
           auto pEffect = item->effects[i];
           if (pEffect) {
               auto pFullName = pEffect->baseEffect;
               if (pFullName) {
                   knownEffects.push_back(pFullName->GetFullName());
               }
           }
       } else {
           knownEffects.push_back({});
       }     
   }

   return knownEffects;
}

float CAHZTarget::GetArmorWarmthRating()
{
    if (!IsValid())
        return {};

    if (GetForm()->GetFormType() != RE::FormType::Armor)
        return {};

    auto item = GetForm()->As<RE::TESObjectARMO>();

    if (!item)
        return {};

    auto fRating = GetArmorWarmthRating_Native(item);

    return mRound(fRating);
}

RE::BIPED_MODEL::BipedObjectSlot CAHZTarget::GetArmorSlotMask()
{
    if (!IsValid()) {
        return RE::BIPED_MODEL::BipedObjectSlot::kNone;
    }

    if (GetForm()->GetFormType() != RE::FormType::Armor)
        return RE::BIPED_MODEL::BipedObjectSlot::kNone;

    auto item = GetForm()->As<RE::TESObjectARMO>();

    if (!item)
        return RE::BIPED_MODEL::BipedObjectSlot::kNone;

    return item->GetSlotMask();
}

float CAHZTarget::GetArmorRating()
{
    // Must be armor
    if (!IsValid())
        return {};

    auto boundObject = GetForm()->As<RE::TESBoundObject>();
    auto pExtraData = IsReference() ? &GetReference()->extraList : nullptr;

    if (!boundObject)
        return {};

    auto pPC = RE::PlayerCharacter::GetSingleton();

    if (pPC) {
        RE::InventoryEntryData objDesc(boundObject, 0);
        if (pExtraData) {
            objDesc.AddExtraList(pExtraData);
        }

        auto fRating = pPC->GetArmorValue(&objDesc);

        // This could be rounded, but the the script decide
        return mRound(fRating);
    }

    return 0.0;
}

float CAHZTarget::GetWeaponDamage()
{
    // Must be a weapon or ammo
    if (!IsValid())
        return {};

    auto baseForm = GetForm();


    if (baseForm->GetFormType() != RE::FormType::Weapon &&
        baseForm->GetFormType() != RE::FormType::Ammo) {
        return {};
    }

    auto boundObject = baseForm->As<RE::TESBoundObject>();
    auto pExtraData = IsReference() ? &GetReference()->extraList: nullptr;

   if (IsReference() && baseForm->GetFormType() == RE::FormType::Projectile) {
        auto asArrowProjectile = GetReference()->As<RE::ArrowProjectile>();
        auto ammo = GetReference()->As<RE::TESAmmo>();
        if (asArrowProjectile) {
            boundObject = ammo;
            pExtraData = &asArrowProjectile->extraList;
        }
    }

   if (!boundObject)
        return {};

    auto pPC = RE::PlayerCharacter::GetSingleton();

    if (pPC) {
        RE::InventoryEntryData objDesc(boundObject, 0);
        if (pExtraData) {
            objDesc.AddExtraList(pExtraData);
        }

        auto fDamage = pPC->GetDamage(&objDesc);

        return mRound(fDamage);
    }

    return {};
}

bool CAHZTarget::GetIsSpellTome()
{
    if (!IsValid()) {
        return false;
    }

    if (GetForm()->GetFormType() != RE::FormType::Book)
        return false;

    auto item = GetForm()->As<RE::TESObjectBOOK>();

    if (!item)
        return false;

   return item->data.flags.any(RE::OBJ_BOOK::Flag::kTeachesSpell);
}

RE::ActorValue CAHZTarget::GetBookSkill()
{
    if (!IsValid()) {
        return RE::ActorValue::kNone;
    }

    if (GetForm()->GetFormType() != RE::FormType::Book)
        return RE::ActorValue::kNone;

    auto item = GetForm()->As<RE::TESObjectBOOK>();

    if (!item)
        return RE::ActorValue::kNone;

    if (item->data.flags.any(RE::OBJ_BOOK::Flag::kAdvancesActorValue)) {
        return item->data.teaches.actorValueToAdvance;
    }

    return RE::ActorValue::kNone;
}

std::string CAHZTarget::GetEffectsDescription()
{
    RE::BSString description;
    std::string effectDescription;
    std::string desc;
    std::string effectsString;

    if (!IsValid())
        return desc;

    if (GetForm()->GetFormType() == RE::FormType::AlchemyItem) {
        auto item = GetForm()->As<RE::AlchemyItem>();

        if (IsReference()) {
            if (auto extraEnchant = static_cast<RE::ExtraEnchantment*>(GetReference()->extraList.GetByType(RE::ExtraDataType::kEnchantment)))  // Enchanted
            {
                if (extraEnchant->enchantment) {
                    GetMagicItemDescription(extraEnchant->enchantment, effectDescription);
                    desc.append(effectDescription);
                }
            }
        }

        if (item) {
            GetMagicItemDescription(item, effectDescription);
            desc.append(effectDescription);
        }
    } else if (GetForm()->GetFormType() == RE::FormType::Weapon) {
        auto item = GetForm()->As<RE::TESObjectWEAP>();

        if (item) {
            //Get enchantment description
            if (item && item->formEnchanting) {
                GetMagicItemDescription(item->formEnchanting, effectDescription);
                desc.append(effectDescription);
            }

            // Items modified by the player
            else if (IsReference())  // Enchanted
            {
                if (auto extraEnchant = static_cast<RE::ExtraEnchantment*>(GetReference()->extraList.GetByType(RE::ExtraDataType::kEnchantment))) {
                    if (extraEnchant->enchantment) {
                        GetMagicItemDescription(extraEnchant->enchantment, effectDescription);
                        desc.append(effectDescription);
                    }
                }
            }
        }
    } else if (GetForm()->GetFormType() == RE::FormType::Armor) {
        auto item = GetForm()->As<RE::TESObjectARMO>();

        if (item) {
            //Get enchantment description
            if (item && item->formEnchanting) {
                GetMagicItemDescription(item->formEnchanting, effectDescription);
                desc.append(effectDescription);
            }

            // Items modified by the player
            else if (IsReference()) {
                if (auto extraEnchant = static_cast<RE::ExtraEnchantment*>(GetReference()->extraList.GetByType(RE::ExtraDataType::kEnchantment)))  // Enchanted
                {
                    if (extraEnchant->enchantment) {
                        GetMagicItemDescription(extraEnchant->enchantment, effectDescription);
                        desc.append(effectDescription);
                    }
                }
            }
        }
    } else if (GetForm()->GetFormType() == RE::FormType::Ammo) {
        if (IsReference()) {
            if (auto extraEnchant = static_cast<RE::ExtraEnchantment*>(GetReference()->extraList.GetByType(RE::ExtraDataType::kEnchantment)))  // Enchanted
            {
                if (extraEnchant->enchantment) {
                    GetMagicItemDescription(extraEnchant->enchantment, effectDescription);
                    desc.append(effectDescription);
                }
            }
        }
    } else if (GetForm()->GetFormType() == RE::FormType::Book) {
        auto item = GetForm()->As<RE::TESObjectBOOK>();

        if (item &&
            ((item->data.flags & RE::OBJ_BOOK::Flag::kTeachesSpell) == RE::OBJ_BOOK::Flag::kTeachesSpell) && !desc.length()) {
            if (item->data.teaches.spell) {
                GetMagicItemDescription(item->data.teaches.spell, effectDescription);
                desc.append(effectDescription);
            }
        }
    } else if (GetForm()->GetFormType() == RE::FormType::Scroll) {
        auto item = GetForm()->As<RE::ScrollItem>();
        if (item) {
            GetMagicItemDescription(item, effectDescription);
            desc.append(effectDescription);
        }
    }
    return desc;
};


std::string CAHZTarget::GetDescription()
{
    if (!IsValid())
        return {};

    RE::BSString bsDescription;
    std::string  tempString{};

    if (GetForm()->GetFormType() == RE::FormType::Book) {
        auto asBook = GetForm()->As<RE::TESObjectBOOK>();
        if (asBook) {
            asBook->itemCardDescription.GetDescription(bsDescription, GetForm(), 0x4D414E43);
            if (!bsDescription.empty()) {
                tempString.assign(bsDescription.c_str());
                if (tempString != "LOOKUP FAILED!" && tempString.length() > 1) {
                    return tempString;
                }
            }
        }
    } else {
        RE::BSString bsDescription2;
        auto         desc = GetForm()->As<RE::TESDescription>();
        if (desc) {
            desc->GetDescription(bsDescription2, GetForm(), 0x43534544);
            if (!bsDescription2.empty())
                tempString.append(bsDescription2.c_str());
            if (tempString != "LOOKUP FAILED!" && tempString.length() > 1) {
                return tempString;
            } else {
                desc->GetDescription(bsDescription2, GetForm(), 0);
                if (!bsDescription2.empty())
                    tempString.assign(bsDescription2.c_str());
                if (tempString != "LOOKUP FAILED!" && tempString.length() > 1) {
                    return tempString;
                }
            }
        }
    }

    return {};
}

RE::BIPED_MODEL::ArmorType CAHZTarget::GetArmorType()
{
    if (!IsValid())
        return RE::BIPED_MODEL::ArmorType::kClothing;

    if (GetForm()->GetFormType() != RE::FormType::Armor)
        return RE::BIPED_MODEL::ArmorType::kClothing;

    auto item = GetForm()->As<RE::TESObjectARMO>();
    if (!item)
        return RE::BIPED_MODEL::ArmorType::kClothing;

    return item->GetArmorType();
};

bool CAHZTarget::GetIsBookAndWasRead()
{
    if (!IsValid())
        return false;

    if (GetForm()->GetFormType() != RE::FormType::Book)
        return false;

    auto item = GetForm()->As<RE::TESObjectBOOK>();
    return item && item->IsRead();
}

RE::AlchemyItem* CAHZTarget::GetAlchemyItem()
{
    if (!IsValid())
        return nullptr;


    if (GetForm()->GetFormType() == RE::FormType::AlchemyItem)
        return GetForm()->As<RE::AlchemyItem>();

    if (GetForm()->GetFormType() == RE::FormType::Flora) {
        // Skip if harvested
        if (IsReference() && (GetReference()->formFlags & RE::TESObjectREFR::RecordFlags::kHarvested)) {
            return nullptr;
        }

        auto flora = GetForm()->As<RE::TESFlora>();
        if (flora) {
            auto form = static_cast<RE::TESForm*>(flora->produceItem);

            if (!form) {
                return nullptr;
            }

            // If the food is actually food
            if (form->formType == RE::FormType::AlchemyItem) {
                return DYNAMIC_CAST(form, RE::TESForm, RE::AlchemyItem);
            }

            // If the food is actually a levelitem (Harvest overhaul mod or a coin purse)
            else if (form->formType == RE::FormType::LeveledItem) {
                auto itemform = GetAlchemyItemFromLeveledList(form);
                return itemform ? itemform : nullptr;
            } else if (form->formType == RE::FormType::FormList) {
                auto lvli = DYNAMIC_CAST(form, RE::TESForm, RE::BGSListForm);

                if (!lvli) {
                    return nullptr;
                }

                // Get the first form and see if it is an ingredient
                if (lvli->forms.size() > 0) {
                    auto itemform = lvli->forms[0];
                    if (itemform) {
                        auto alchmyItem = DYNAMIC_CAST(itemform, RE::TESForm, RE::AlchemyItem);
                        return alchmyItem;
                    }
                }
            }
        }
    } else if (GetForm()->GetFormType() == RE::FormType::Tree) {
        // Skip if harvested
        if (IsReference() && (GetReference()->formFlags & RE::TESObjectREFR::RecordFlags::kHarvested)) {
            return nullptr;
        }

        auto tree = GetForm()->As<RE::TESObjectTREE>();
        if (tree) {
            auto form = static_cast<RE::TESForm*>(tree->produceItem);

            if (!form) {
                return nullptr;
            }

            // If the produce is actually food
            if (form->formType == RE::FormType::AlchemyItem) {
                return DYNAMIC_CAST(form, RE::TESForm, RE::AlchemyItem);
            }

            // If the ingredient is actually a levelitem (Harvest overhaul mod or a coin purse)
            else if (form->formType == RE::FormType::LeveledItem) {
                auto itemform = GetAlchemyItemFromLeveledList(form);
                return itemform ? itemform : nullptr;
            } else if (form->formType == RE::FormType::FormList) {
                auto lvli = DYNAMIC_CAST(form, RE::TESForm, RE::BGSListForm);

                if (!lvli) {
                    return nullptr;
                }

                // Get the first form and see if it is an ingredient
                if (lvli->forms.size() > 0) {
                    auto itemform = lvli->forms[0];
                    if (itemform) {
                        auto alchmyItem = DYNAMIC_CAST(itemform, RE::TESForm, RE::AlchemyItem);
                        return alchmyItem;
                    }
                }
            }
        }
    }

    return nullptr;
}

RE::IngredientItem* CAHZTarget::GetIngredient()
{
    if (!IsValid())
        return nullptr;


    if (GetForm()->GetFormType() == RE::FormType::Ingredient)
        return GetForm()->As<RE::IngredientItem>();

    if (GetForm()->GetFormType() == RE::FormType::Flora) {
        // Skip if harvested
        if (IsReference() && (GetReference()->formFlags & RE::TESObjectREFR::RecordFlags::kHarvested)) {
            return nullptr;
        }

        auto flora = GetForm()->As<RE::TESFlora>();
        if (flora) {
            auto form = static_cast<RE::TESForm*>(flora->produceItem);

            if (!form) {
                return nullptr;
            }

            // If the ingredient is actually an ingredient
            if (form->formType == RE::FormType::Ingredient) {
                return DYNAMIC_CAST(form, RE::TESForm, RE::IngredientItem);
            }

            // If the ingredient is actually a levelitem (Harvest overhaul mod or a coin purse)
            else if (form->formType == RE::FormType::LeveledItem) {
                auto itemform = GetIngredientFromLeveledList(form);
                return itemform ? itemform : nullptr;
            } else if (form->formType == RE::FormType::FormList) {
                auto lvli = DYNAMIC_CAST(form, RE::TESForm, RE::BGSListForm);

                // Get the first form and see if it is an ingredient
                if (lvli->forms.size() > 0) {
                    auto itemform = lvli->forms[0];
                    if (itemform) {
                        auto ingredient = DYNAMIC_CAST(itemform, RE::TESForm, RE::IngredientItem);
                        return ingredient;
                    }
                }
            }
        }
    } else if (GetForm()->GetFormType() == RE::FormType::Tree) {
        // Skip if harvested
        if (IsReference() && (GetReference()->formFlags & RE::TESObjectREFR::RecordFlags::kHarvested)) {
            return nullptr;
        }

        auto tree = GetForm()->As<RE::TESObjectTREE>();
        if (tree) {
            auto form = static_cast<RE::TESForm*>(tree->produceItem);  //DYNAMIC_CAST(tree->produce.produce, IngredientItem,RE::TESForm);

            if (!form) {
                return nullptr;
            }

            // If the ingredient is actually an ingredient
            if (form->formType == RE::FormType::Ingredient) {
                return DYNAMIC_CAST(form, RE::TESForm, RE::IngredientItem);
            }

            // If the ingredient is actually a levelitem (Harvest overhaul mod or a coin purse)
            else if (form->formType == RE::FormType::LeveledItem) {
                auto itemform = GetIngredientFromLeveledList(form);
                return itemform ? itemform : nullptr;
            } else if (form->formType == RE::FormType::FormList) {
                auto lvli = DYNAMIC_CAST(form, RE::TESForm, RE::BGSListForm);

                if (!lvli) {
                    return nullptr;
                }

                // Get the first form and see if it is an ingredient
                if (lvli->forms.size() > 0) {
                    auto itemform = lvli->forms[0];
                    if (itemform) {
                        auto ingredient = DYNAMIC_CAST(itemform, RE::TESForm, RE::IngredientItem);
                        return ingredient;
                    }
                }
            }
        }
    }

    return nullptr;
}

RE::SpellItem* CAHZTarget::GetSpellItem()
{
    if (!IsValid())
        return nullptr;

    return GetForm()->As<RE::SpellItem>();
}


std::string CAHZTarget::GetTargetName()
{
    std::string name;

    if (!IsValid()) {
        return name;
    }

    auto pFullName = GetForm()->As<RE::TESFullName>();
    if (!pFullName)
        return name;

    const char* displayName = nullptr;

    if (IsReference()) {
        auto extraList = &GetReference()->extraList;
        if (extraList) {
            displayName = extraList->GetDisplayName(GetForm()->As<RE::TESBoundObject>());
        }
    }

    // If the name can be created
    if (displayName) {
        name.append(displayName);
    }
    // Use the base name
    else if (pFullName) {
        name.append(pFullName->GetFullName());
    }

    return name;
};

std::string CAHZTarget::GetTargetSoulLevelName()
{
    // If this is a soul gem, also get the gem size name
    if (GetForm()->formType == RE::FormType::SoulGem) {
        auto gem = GetForm()->As<RE::TESSoulGem>();
        if (gem) {
            std::string soulName{};
            auto        soulLevel = gem->soulCapacity;
            return GetSoulLevelName(static_cast<uint8_t>(soulLevel.get()));
        }
    }

    return {};
}

RE::AlchemyItem* CAHZTarget::GetAlchemyItemFromLeveledList(RE::TESForm* thisObject)
{
    if (thisObject->formType == RE::FormType::LeveledItem) {
        RE::TESLevItem* lvli = DYNAMIC_CAST(thisObject, RE::TESForm, RE::TESLevItem);

        // Get the first form and see if it is an ingredient
        if (lvli && lvli->numEntries > 0) {
            for (int i = 0; i < lvli->numEntries; i++) {
                auto itemform = lvli->entries[i].form;
                if (itemform) {
                    if (itemform->formType == RE::FormType::AlchemyItem) {
                        auto alchemyItem = DYNAMIC_CAST(itemform, RE::TESForm, RE::AlchemyItem);
                        return alchemyItem;
                    }
                }
            }
        }
    }

    return nullptr;
}

RE::IngredientItem* CAHZTarget::GetIngredientFromLeveledList(RE::TESForm* thisObject)
{
    if (thisObject->formType == RE::FormType::LeveledItem) {
        auto lvli = DYNAMIC_CAST(thisObject, RE::TESForm, RE::TESLevItem);

        // Get the first form and see if it is an ingredient
        if (lvli && lvli->numEntries > 0) {
            for (int i = 0; i < lvli->numEntries; i++) {
                auto itemform = lvli->entries[i].form;
                if (itemform) {
                    if (itemform->formType == RE::FormType::Ingredient) {
                        auto* ingredient = DYNAMIC_CAST(itemform, RE::TESForm, RE::IngredientItem);
                        return ingredient;
                    }
                }
            }
        }
    }

    return nullptr;
}

bool CAHZTarget::CanPickUp(RE::TESForm* form)
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

    if (!canCarry && formType == RE::FormType::Light) {
        auto possibleTorch = form->As<RE::TESObjectLIGH>();
        if (possibleTorch && possibleTorch->CanBeCarried()) {
            canCarry = true;
        }
    }

    return canCarry;
}

void CAHZTarget::GetMagicItemDescription(RE::MagicItem* item, std::string& description)
{
    description.clear();

    RE::BSString temp;

    GetMagicItemDescription_Native(nullptr, item, &temp);
    ProcessSurvivalMode_Native(&temp);

    description.append(temp.c_str());
}

bool CAHZTarget::GetIsNthEffectKnown(RE::IngredientItem* thisMagic, uint32_t index)
{
    if (!thisMagic)
        return false;

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

bool CAHZTarget::MagicDisallowEnchanting(const RE::BGSKeywordForm* pKeywords)
{
    if (pKeywords) {
        for (uint32_t k = 0; k < pKeywords->numKeywords; k++) {
            if (pKeywords->keywords[k]) {
                auto keyword = pKeywords->GetKeywordAt(k).value_or(nullptr);
                if (keyword) {
                    // Had to add this check because https://www.nexusmods.com/skyrimspecialedition/mods/34175?
                    // sets the editor ID for 'MagicDisallowEnchanting' to null (╯°□°）╯︵ ┻━┻
                    auto   asCstr = keyword->GetFormEditorID();
                    std::string keyWordName = asCstr ? asCstr : "";
                    if (keyWordName == "MagicDisallowEnchanting") {
                        return true;  // Is enchanted, but cannot be enchanted by player
                    }
                }
            }
        }
    }
    return false;
}