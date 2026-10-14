#include "pch.h"
#include "AHZTarget.h"
#include "AHZFormLookup.h"
#include "AHZPlayerInfo.h"
#include "AHZPapyrusMoreHud.h"
#include "AHZArmorInfo.h"
#include "AHZWeaponInfo.h"
#include "CompletionistAPI.h"
#include "ActorValueList.h"
#include "Offsets-VR.h"
#include "Offsets-SE.h"
#include "Offsets-AE.h"

namespace
{
    std::string GetSoulLevelName(uint8_t soulLevel)
    {
        constexpr const char* names[] = { "", "sSoulLevelNamePetty", "sSoulLevelNameLesser",
            "sSoulLevelNameCommon", "sSoulLevelNameGreater", "sSoulLevelNameGrand" };
        const auto settings = RE::GameSettingCollection::GetSingleton();
        if (!settings || soulLevel == 0 || soulLevel >= std::size(names)) {
            return {};
        }
        const auto setting = settings->GetSetting(names[soulLevel]);
        const auto name = setting ? setting->GetString() : nullptr;
        return name ? std::string{name} : std::string{};
    }

    float mRound(float r)
    {
        return (r >= 0.0f) ? floor(r + 0.5f) : ceil(r - 0.5f);
    }
}

    bool CAHZTarget::IsSurvivalMode()
    {
        if (!REL::Module::IsVR()) {
            const auto dobj = RE::BGSDefaultObjectManager::GetSingleton();
            if (dobj) {
                const auto survival = dobj->GetObject<RE::TESGlobal>(
                    RE::DefaultObjectID::kSurvivalModeEnabled);

                return survival && *survival ? (*survival)->value == 1.0F : false;
            }
        }
        return false;
    }

//------------------Native Wrappers -----------------------------

// 1408C2D30 1.6.318
void CAHZTarget::GetMagicItemDescription_Native(void*, RE::TESForm* a1, RE::BSString* a2)
{
    using func_t = decltype(&CAHZTarget::GetMagicItemDescription_Native);
    REL::Relocation<func_t> func{ REL::VariantID{
        moreHUDSE::SE::Offsets::GetMagicItemDescription.id(),
        moreHUDSE::AE::Offsets::GetMagicItemDescription.id(),
        moreHUDSE::VR::Offsets::GetMagicItemDescription.offset() } };
    if (!a1 || !a2 || !func.address()) {
        logger::warn("Cannot derive magic description: form, output or relocation is unavailable"sv);
        return;
    }
    func(nullptr, a1, a2);
}

// 1408C33D0 1.6.318
char* CAHZTarget::ProcessSurvivalMode_Native([[maybe_unused]]RE::BSString* a2)
{
    if (REL::Module::IsSE() || REL::Module::IsAE()) {
        using func_t = decltype(&CAHZTarget::ProcessSurvivalMode_Native);
        REL::Relocation<func_t> func{ REL::RelocationID{ moreHUDSE::SE::Offsets::ProcessSurvivalMode.id(),
            moreHUDSE::AE::Offsets::ProcessSurvivalMode.id() } };
            return a2 && func.address() ? func(a2) : nullptr;
        }
        return nullptr;
    }

// 1403D4D60 1.6.318
float CAHZTarget::GetArmorWarmthRating_Native([[maybe_unused]]RE::TESForm* a1)
{
    if (REL::Module::IsSE() || REL::Module::IsAE()) {
        using func_t = decltype(&CAHZTarget::GetArmorWarmthRating_Native);
        REL::Relocation<func_t> func{ REL::RelocationID{ moreHUDSE::SE::Offsets::GetArmorWarmthRating.id(), moreHUDSE::AE::Offsets::GetArmorWarmthRating.id() } };
        return a1 && func.address() ? func(a1) : 0.0F;
    }
    return 0;
}

// 1403d4e37  1.6.318
float CAHZTarget::GetActorWarmthRating_Native([[maybe_unused]]RE::Actor* a1, [[maybe_unused]]float s2)
{
    if (REL::Module::IsSE() || REL::Module::IsAE()) {
        using func_t = decltype(&CAHZTarget::GetActorWarmthRating_Native);
        REL::Relocation<func_t> func{ REL::RelocationID{ moreHUDSE::SE::Offsets::GetActorWarmthRating.id(), moreHUDSE::AE::Offsets::GetActorWarmthRating.id() } };
        return a1 && func.address() ? func(a1, s2) : 0.0F;
    }
    return 0;
}

//------------------End Native Wrappers -------------------------

void CAHZTarget::SetTarget(RE::TESObjectREFR* pTargetRef)
{
    // Hook calls may be concurrent or reentrant. Keep all borrowed engine state local
    // to this invocation, while Skyrim/SKSE owns the reference passed by the hook.
    const auto sequence = m_nextSnapshotSequence.fetch_add(1, std::memory_order_relaxed) + 1;
    CAHZTarget derivation;
    derivation.DeriveTarget(pTargetRef);
    std::lock_guard lock(m_snapshotMutex);
    if (sequence > m_publishedSnapshotSequence) {
        m_publishedTarget = std::move(derivation.m_target);
        m_publishedSnapshotSequence = sequence;
    }
}

void CAHZTarget::DeriveTarget(RE::TESObjectREFR* pTargetRef)
{
    // These pointers are a transient calculation context for this crosshair callback only.
    // Never leave them available for a later Scaleform, menu, task, or timer callback: the
    // underlying reference can be destroyed as soon as SKSE's crosshair hook returns.
    const SKSE::stl::scope_exit clearTransientTargetState{ [this]() noexcept {
        m_pObjectRef = nullptr;
        m_pForm = nullptr;
        m_IngredientItem = nullptr;
        m_AlchemyItem = nullptr;
        m_SpellItem = nullptr;
    } };

    m_pForm = CAHZFormLookup::Instance().GetTESForm(pTargetRef);
    if (m_pForm) {
        if (m_pForm->GetFormType() == RE::FormType::Reference) {
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
    CaptureRolloverData();
}

void CAHZTarget::CaptureRolloverData()
{
    if (m_target.hasInteriorCellDestination) {
        const char* key = "$mHUD_LocationStatusUnvisited";
        std::string label = "Unvisited";
        if (m_target.interiorCellCleared) {
            key = "$mHUD_LocationStatusCleared";
            label = "Cleared";
        } else if (m_target.interiorCellVisited) {
            if (m_target.interiorCellIsDungeon && m_target.interiorCellIsClearable) {
                key = "$mHUD_LocationStatusVisitedNotCleared";
                label = "Visited (Not Cleared)";
            } else {
                key = "$mHUD_LocationStatusVisited";
                label = "Visited";
            }
        }
        SKSE::Translation::Translate(key, label);
        if (const auto strings = RE::InterfaceStrings::GetSingleton()) {
            const auto& marker = REL::Module::IsVR() ? strings->GetVRRuntimeData().diamondMarker :
                REL::Module::IsAE() ? strings->GetAERuntimeData().diamondMarker :
                                     strings->GetRuntimeData().diamondMarker;
            if (!marker.empty()) {
                m_target.locationStatusText = std::string{"\n"} + marker.c_str() + label;
            }
        }
    }

    if (!m_target.isValid) {
        return;
    }

    m_target.inventoryCount = CAHZPlayerInfo::GetItemAmount(m_target.formId);
    m_target.formListIcons = PapyrusMoreHud::GetFormIcons(m_target.formId);
    m_target.armorWeightClass = GetArmorWeightClass(m_target);
    m_target.bookSkillText = GetBookSkillText(m_target);
    if (CompletionistAPI::IsReady()) {
        if (const auto object = GetForm() ? GetForm()->As<RE::TESBoundObject>() : nullptr) {
            m_target.completionistIcon = CompletionistAPI::GetIconInfo(object).GetRequiredIconName();
        }
    }

    if (m_target.formType == RE::FormType::Weapon || m_target.formType == RE::FormType::Ammo) {
        m_target.equippedRatingOrDamage = GetTotalActualWeaponDamage();
        m_target.ratingOrDamageDifference = GetWeaponDamageDiff(m_target);
    } else if (m_target.formType == RE::FormType::Armor) {
        m_target.equippedRatingOrDamage = GetTotalActualArmorRating();
        m_target.ratingOrDamageDifference = GetArmorRatingDiff(m_target);
        m_target.isSurvivalMode = IsSurvivalMode();
        if (m_target.isSurvivalMode) {
            m_target.totalWarmthRating = GetTotalWarmthRating();
            m_target.warmthDifference = GetWarmthRatingDiff(m_target);
        }
    }
}

void CAHZTarget::UpdateTarget()
{
    m_target.Reset();
    if (!IsValid()) {
        return;
    }

    UpdateInteriorCellDestination();

    m_IngredientItem = GetIngredient();
    m_AlchemyItem = GetAlchemyItem();
    m_SpellItem = GetSpellItem();

    auto canPickUp = CanPickUp(GetForm());

    // Can carry or what-can-be-harvested can be carried
    m_target.canCarry = m_IngredientItem || m_AlchemyItem || canPickUp;

    // Spell items are valid but cannot be picked up. e.g. Shrines
    m_target.isValid = m_target.canCarry || m_SpellItem;

    if (!m_target.isValid) {
        return;
    }
    auto isHarvested = false;
    if (m_IngredientItem && GetForm()->GetFormType() != RE::FormType::Ingredient) {
        m_pForm = m_IngredientItem;
        m_pObjectRef = nullptr;
        isHarvested = true;
    } else if (m_AlchemyItem && GetForm()->GetFormType() != RE::FormType::AlchemyItem) {
        m_pForm = m_AlchemyItem;
        m_pObjectRef = nullptr;
        isHarvested = true;
    } else if (m_SpellItem && GetForm()->GetFormType() != RE::FormType::Spell) {
        m_pForm = m_SpellItem;
        m_pObjectRef = nullptr;
        isHarvested = true;
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
    m_target.armorWarmthRating = IsSurvivalMode() ? GetArmorWarmthRating() : 0;
    m_target.knownIngredientEffects = GetKnownIngredientEffects();
    m_target.weight = isHarvested ? 0 : GetWeight();  // if harvested, we can't know the weight for sure.  Because we don't always know the amount harvested
    m_target.weaponType = GetWeaponType();
    m_target.enchantmentType = GetIsKnownEnchantment();
    m_target.isBoltAmmo = GetIsBoltAmmo();
}

void CAHZTarget::UpdateInteriorCellDestination()
{
    if (!IsReference() || GetForm()->GetFormType() != RE::FormType::Door) {
        return;
    }

    const auto linkedDoorOwner = GetReference()->extraList.GetTeleportLinkedDoor();
    const auto linkedDoor = linkedDoorOwner.get();
    if (!linkedDoor) {
        return;
    }

    const auto destinationCell = linkedDoor->GetParentCell();
    if (!destinationCell || !destinationCell->IsInteriorCell()) {
        return;
    }

    m_target.hasInteriorCellDestination = true;
    const auto sourceCell = GetReference()->GetParentCell();
    m_target.interiorCellIsExteriorEntrance = sourceCell && !sourceCell->IsInteriorCell();

    const auto location = destinationCell->GetLocation();
    m_target.interiorCellCleared = location && (location->IsCleared() || location->everCleared);

    const auto dungeonKeyword = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("LocTypeDungeon");
    const auto defaultObjectManager = RE::BGSDefaultObjectManager::GetSingleton();
    const auto clearableKeywordObject = defaultObjectManager ?
        defaultObjectManager->GetObject<RE::BGSKeyword>(RE::DefaultObjectID::kKeywordClearableLocation) :
        nullptr;
    const auto clearableKeyword = clearableKeywordObject ? *clearableKeywordObject : nullptr;
    constexpr std::size_t maxLocationParentDepth = 64;
    std::size_t           locationParentDepth = 0;
    for (auto currentLocation = location;
         currentLocation && locationParentDepth < maxLocationParentDepth;
         currentLocation = currentLocation->parentLoc, ++locationParentDepth) {
        if (dungeonKeyword && currentLocation->HasKeyword(dungeonKeyword)) {
            m_target.interiorCellIsDungeon = true;
        }

        if (!m_target.interiorCellIsClearable && clearableKeyword && currentLocation->HasKeyword(clearableKeyword)) {
            m_target.interiorCellIsClearable = true;
            m_target.interiorCellCleared = currentLocation->IsCleared() || currentLocation->everCleared;
        }
    }

    const auto player = RE::PlayerCharacter::GetSingleton();
    m_target.interiorCellVisited =
        m_target.interiorCellCleared ||
        (player && player->GetParentCell() == destinationCell) ||
        destinationCell->extraList.HasType<RE::ExtraDetachTime>();
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

bool CAHZTarget::GetIsBoltAmmo()
{
    if (!IsValid()) {
        return false;
    }

    if (GetForm()->GetFormType() != RE::FormType::Ammo)
        return false;

    auto item = GetForm()->As<RE::TESAmmo>();

    if (!item)
        return false;

    return item->IsBolt();
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
    auto                     item = m_IngredientItem;

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

    return static_cast<RE::BIPED_MODEL::BipedObjectSlot>(item->GetSlotMask().underlying());
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
    auto pExtraData = IsReference() ? &GetReference()->extraList : nullptr;

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
    std::string  effectDescription;
    std::string  desc;
    std::string  effectsString;

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
    }
    // Spell items like blessings
    else if (GetForm()->GetFormType() == RE::FormType::Spell) {
        auto item = GetForm()->As<RE::SpellItem>();
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
                ProcessSurvivalMode_Native(&bsDescription);
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
            if (!bsDescription2.empty()) {
                ProcessSurvivalMode_Native(&bsDescription);
                tempString.append(bsDescription2.c_str());
            }
            if (tempString != "LOOKUP FAILED!" && tempString.length() > 1) {
                return tempString;
            } else {
                desc->GetDescription(bsDescription2, GetForm(), 0);
                if (!bsDescription2.empty()) {
                    ProcessSurvivalMode_Native(&bsDescription);
                    tempString.assign(bsDescription2.c_str());
                }
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
                return form->As<RE::AlchemyItem>();
            }

            // If the food is actually a levelitem (Harvest overhaul mod or a coin purse)
            else if (form->formType == RE::FormType::LeveledItem) {
                auto itemform = GetAlchemyItemFromLeveledList(form);
                return itemform ? itemform : nullptr;
            } else if (form->formType == RE::FormType::FormList) {
                auto lvli = form->As<RE::BGSListForm>();

                if (!lvli) {
                    return nullptr;
                }

                // Get the first form and see if it is an ingredient
                if (lvli->forms.size() > 0) {
                    auto itemform = lvli->forms[0];
                    if (itemform) {
                        auto alchmyItem = itemform->As<RE::AlchemyItem>();
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
                return form->As<RE::AlchemyItem>();
            }

            // If the ingredient is actually a levelitem (Harvest overhaul mod or a coin purse)
            else if (form->formType == RE::FormType::LeveledItem) {
                auto itemform = GetAlchemyItemFromLeveledList(form);
                return itemform ? itemform : nullptr;
            } else if (form->formType == RE::FormType::FormList) {
                auto lvli = form->As<RE::BGSListForm>();

                if (!lvli) {
                    return nullptr;
                }

                // Get the first form and see if it is an ingredient
                if (lvli->forms.size() > 0) {
                    auto itemform = lvli->forms[0];
                    if (itemform) {
                        auto alchmyItem = itemform->As<RE::AlchemyItem>();
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
                return form->As<RE::IngredientItem>();
            }

            // If the ingredient is actually a levelitem (Harvest overhaul mod or a coin purse)
            else if (form->formType == RE::FormType::LeveledItem) {
                auto itemform = GetIngredientFromLeveledList(form);
                return itemform ? itemform : nullptr;
            } else if (form->formType == RE::FormType::FormList) {
                auto lvli = form->As<RE::BGSListForm>();

                // Get the first form and see if it is an ingredient
                if (lvli->forms.size() > 0) {
                    auto itemform = lvli->forms[0];
                    if (itemform) {
                        auto ingredient = itemform->As<RE::IngredientItem>();
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
                return form->As<RE::IngredientItem>();
            }

            // If the ingredient is actually a levelitem (Harvest overhaul mod or a coin purse)
            else if (form->formType == RE::FormType::LeveledItem) {
                auto itemform = GetIngredientFromLeveledList(form);
                return itemform ? itemform : nullptr;
            } else if (form->formType == RE::FormType::FormList) {
                auto lvli = form->As<RE::BGSListForm>();

                if (!lvli) {
                    return nullptr;
                }

                // Get the first form and see if it is an ingredient
                if (lvli->forms.size() > 0) {
                    auto itemform = lvli->forms[0];
                    if (itemform) {
                        auto ingredient = itemform->As<RE::IngredientItem>();
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
        RE::TESLevItem* lvli = thisObject->As<RE::TESLevItem>();

        // Get the first form and see if it is an ingredient
        if (lvli && lvli->numEntries > 0) {
            for (int i = 0; i < lvli->numEntries; i++) {
                auto itemform = lvli->entries[i].form;
                if (itemform) {
                    if (itemform->formType == RE::FormType::AlchemyItem) {
                        auto alchemyItem = itemform->As<RE::AlchemyItem>();
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
        auto lvli = thisObject->As<RE::TESLevItem>();

        // Get the first form and see if it is an ingredient
        if (lvli && lvli->numEntries > 0) {
            for (int i = 0; i < lvli->numEntries; i++) {
                auto itemform = lvli->entries[i].form;
                if (itemform) {
                    if (itemform->formType == RE::FormType::Ingredient) {
                        auto* ingredient = itemform->As<RE::IngredientItem>();
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
                    auto        asCstr = keyword->GetFormEditorID();
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

auto CAHZTarget::GetActualDamage(AHZWeaponData* weaponData) -> float
{
    if (!weaponData || !weaponData->equipData.boundObject)
        return 0.0f;

    auto pPC = RE::PlayerCharacter::GetSingleton();

    if (pPC) {
        RE::InventoryEntryData objDesc(weaponData->equipData.boundObject, 0);
        if (weaponData->equipData.pExtraData) {
            objDesc.AddExtraList(weaponData->equipData.pExtraData);
        }

        float fDamage = pPC->GetDamage(&objDesc);

        return mRound(fDamage);
    }

    return 0.0;
}

auto CAHZTarget::GetArmorWarmthRating(AHZArmorData* armorData) -> float
{
    if (!armorData || !armorData->armor || !armorData->equipData.boundObject)
        return 0.0f;

    auto fRating = CAHZTarget::GetArmorWarmthRating_Native(armorData->equipData.boundObject);

    return mRound(fRating);
}

auto CAHZTarget::GetActualArmorRating(AHZArmorData* armorData) -> float
{
    if (!armorData || !armorData->armor || !armorData->equipData.boundObject)
        return 0.0f;

    auto pPC = RE::PlayerCharacter::GetSingleton();

    if (pPC) {
        RE::InventoryEntryData objDesc(armorData->equipData.boundObject, 0);
        if (armorData->equipData.pExtraData) {
            objDesc.AddExtraList(armorData->equipData.pExtraData);
        }

        auto fRating = pPC->GetArmorValue(&objDesc);

        return mRound(fRating);
    }

    return 0.0;
}

// TODO: Port Note, I should probably decode the function that already does this
auto CAHZTarget::GetTotalActualArmorRating() -> float
{
    float totalRating = 0.0f;
    // Keep a list of items to make sure they are not added more than once
    // Some items take up more than one slot
    std::list<const RE::TESForm*> clist;
    for (uint64_t slot = 1; slot <= static_cast<uint64_t>(0x2000); slot <<= 1) {
        AHZArmorData armorData = CAHZArmorInfo::GetArmorFromSlotMask(static_cast<RE::BIPED_MODEL::BipedObjectSlot>(slot));
        if (armorData.equipData.boundObject) {
            if (std::find(clist.begin(), clist.end(), armorData.equipData.boundObject) == clist.end()) {
                clist.push_front(armorData.equipData.boundObject);
                if (armorData.armor) {
                    totalRating += GetActualArmorRating(&armorData);
                }
            }
        }
    }
    return mRound(totalRating);
}

auto CAHZTarget::GetTotalWarmthRating() -> float
{
    auto pPC = RE::PlayerCharacter::GetSingleton();

    if (!pPC) {
        return 0.0;
    }

    return CAHZTarget::GetActorWarmthRating_Native(pPC, 0.0);
}

auto CAHZTarget::mRound(float r) -> float
{
    return (r >= 0.0f) ? floor(r + 0.5f) : ceil(r - 0.5f);
}

float CAHZTarget::GetArmorRatingDiff(const TargetData& target)
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

auto CAHZTarget::GetWarmthRatingDiff(const TargetData& target) -> float
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

auto CAHZTarget::GetTotalActualWeaponDamage() -> float
{
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
            if (equippedAmmo.ammo && !isBolt(equippedAmmo.ammo)) {
                totalWeaponDamage += GetActualDamage(&equippedAmmo);
            }
        } else if (IsCrossBow(leftWeapon.weapon)) {
            is2Handed = true;
            totalWeaponDamage = GetActualDamage(&leftWeapon);

            // Add the arrow damage
            if (equippedAmmo.ammo && isBolt(equippedAmmo.ammo)) {
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
            if (equippedAmmo.ammo && !isBolt(equippedAmmo.ammo)) {
                totalWeaponDamage += GetActualDamage(&equippedAmmo);
            }
        } else if (IsCrossBow(rightWeapon.weapon) && !is2Handed) {
            is2Handed = true;
            totalWeaponDamage = GetActualDamage(&rightWeapon);

            // Add the arrow damage
            if (equippedAmmo.ammo && isBolt(equippedAmmo.ammo)) {
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
    return totalWeaponDamage;
}

auto CAHZTarget::isBolt(RE::TESAmmo* thisAmmo) -> bool
{
    return thisAmmo && thisAmmo->IsBolt();
}

auto CAHZTarget::GetWeaponDamageDiff(const TargetData& target) -> float
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
            if (IsBow(target) && !isBolt(equippedAmmo.ammo)) {
                targetArrowDamage = GetActualDamage(&equippedAmmo);
            } else if (IsCrossBow(target) && isBolt(equippedAmmo.ammo)) {
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
            if (equippedAmmo.ammo && !isBolt(equippedAmmo.ammo)) {
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
            if (equippedAmmo.ammo && isBolt(equippedAmmo.ammo)) {
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

auto CAHZTarget::IsTwoHanded(RE::TESObjectWEAP* thisWeapon) -> bool
{
    if (!thisWeapon) { return false; }
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

auto CAHZTarget::IsOneHanded(RE::TESObjectWEAP* thisWeapon) -> bool
{
    if (!thisWeapon) { return false; }
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

bool CAHZTarget::IsBow(RE::TESObjectWEAP* thisWeapon)
{
    if (!thisWeapon) { return false; }
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

bool CAHZTarget::IsBow(const TargetData& thisWeapon)
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

bool CAHZTarget::IsCrossBow(RE::TESObjectWEAP* thisWeapon)
{
    if (!thisWeapon) { return false; }
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

bool CAHZTarget::IsCrossBow(const TargetData& thisWeapon)
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

auto CAHZTarget::GetArmorWeightClass(const TargetData& target) -> std::string
{
    std::string desc;

    if (!target.isValid)
        return desc;

    if (target.formType != RE::FormType::Armor)
        return desc;

    auto avList = SKSE::ActorValueList::GetSingleton();
    if (!avList) { return desc; }
    if (target.armorType == RE::BIPED_MODEL::ArmorType::kLightArmor || target.armorType == RE::BIPED_MODEL::ArmorType::kHeavyArmor) {
        // Utilize the AV value to get the localized name for "Light Armor"
        if (target.armorType == RE::BIPED_MODEL::ArmorType::kLightArmor) {
            auto info = avList->GetActorValue(RE::ActorValue::kLightArmor);
            if (info) {
                desc.append(R"(<FONT FACE="$EverywhereMediumFont"SIZE="15"COLOR="#999999"KERNING="0">     )");
                desc.append(info->GetFullName());
                desc.append("<\\FONT>");
            }
        }

        // Utilize the AV value to get the localized name for "Heavy Armor"
        else if (target.armorType == RE::BIPED_MODEL::ArmorType::kHeavyArmor) {
            auto info = avList->GetActorValue(RE::ActorValue::kHeavyArmor);
            if (info) {
                desc.append(R"(<FONT FACE="$EverywhereMediumFont"SIZE="15"COLOR="#999999"KERNING="0">     )");
                desc.append(info->GetFullName());
                desc.append("<\\FONT>");
            }
        }
    }
    return desc;
};

auto CAHZTarget::GetBookSkillText(const TargetData& target) -> std::string
{
    std::string desc;

    if (!target.isValid)
        return desc;

    if (target.formType == RE::FormType::Book) {
        // If this is a spell book, then it is not a skill book
        if (target.isSpellTome)
            return desc;

        if (target.bookSkill != RE::ActorValue::kNone) {
            auto avList = SKSE::ActorValueList::GetSingleton();
    if (!avList) { return desc; }
            if (avList) {
                auto info = avList->GetActorValue(target.bookSkill);
                if (info) {
                    desc.append(R"(<FONT FACE="$EverywhereMediumFont"SIZE="15"COLOR="#999999"KERNING="0">       )");
                    desc.append(info->GetFullName());
                    desc.append("<\\FONT>");
                }
            }
        }
    }
    return desc;
}
