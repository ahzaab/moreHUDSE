#pragma once
#include <mutex>
#include <atomic>

class AHZWeaponData;
class AHZArmorData;

namespace Events
{
    class CrosshairHandler;
    bool Hook_WandLookupREFRByHandle_Impl(RE::RefHandle& refHandle, RE::NiPointer<RE::TESObjectREFR>& refrOut);
}

enum class EnchantmentType
{
    None = 0,
    Known = 1,
    CannotDisenchant = 2,
    Unknown = 3
};

class CAHZTarget;

struct TargetData
{
    friend class CAHZTarget;

    bool                             isValid{};
    bool                             canCarry{};
    bool                             bookRead{};
    bool                             hasInteriorCellDestination{};
    bool                             interiorCellVisited{};
    bool                             interiorCellCleared{};
    bool                             interiorCellIsDungeon{};
    bool                             interiorCellIsClearable{};
    bool                             interiorCellIsExteriorEntrance{};
    RE::FormType                     formType{ RE::FormType::None };
    RE::FormID                       formId{};
    std::string                      effectsDescription{};
    std::string                      description{};
    float                            damage{};
    RE::BIPED_MODEL::ArmorType       armorType{ RE::BIPED_MODEL::ArmorType::kLightArmor };  //0
    float                            armorRating{};
    float                            armorWarmthRating{};
    RE::BIPED_MODEL::BipedObjectSlot armorslotMask{ RE::BIPED_MODEL::BipedObjectSlot::kNone };
    std::vector<std::string>         knownIngredientEffects{};
    std::string                      name{};
    std::string                      displayName{};
    float                            weight{};
    RE::WEAPON_TYPE                  weaponType{};
    EnchantmentType                  enchantmentType{};
    bool                             isSpellTome{};
    RE::ActorValue                   bookSkill{ RE::ActorValue::kNone };
    bool                             isBoltAmmo{};

    // Owning, pointer-free values captured synchronously in the crosshair hook.
    uint32_t inventoryCount{};
    float equippedRatingOrDamage{};
    float ratingOrDamageDifference{};
    bool isSurvivalMode{};
    float totalWarmthRating{};
    float warmthDifference{};
    std::string armorWeightClass{};
    std::string bookSkillText{};
    std::string completionistIcon{};
    std::vector<std::string> formListIcons{};
    std::string locationStatusText{};

    void Dump() const
    {
        //logger::trace("\r\n");
        //logger::trace("\r\n");
        //logger::trace("******************************************");
        //logger::trace("isValid: {}", isValid);
        //logger::trace("canCarry: {}", canCarry);
        //logger::trace("bookRead: {}", bookRead);
        //logger::trace("formType: {}", formType);
        //logger::trace("formId: {}", formId);
        //logger::trace("effectsDescription: {}", effectsDescription);
        //logger::trace("description: {}", description);
        //logger::trace("bookSkill: {}", bookSkill);
        //logger::trace("isSpellTome: {}", isSpellTome);
        //logger::trace("damage: {}", damage);
        //logger::trace("armorType: {}", armorType);
        //logger::trace("armorRating: {}", armorRating);
        //logger::trace("armorWarmthRating: {}", armorWarmthRating);
        //logger::trace("armorslotMask: {}", armorslotMask);
        //logger::trace("knownIngredientEffects:");
        //for (auto& i : knownIngredientEffects) {
        //    logger::trace("   {}", i);
        //}
        //logger::trace("name: {}", name);
        //logger::trace("displayName: {}", displayName);
        //logger::trace("weight: {}", weight);
        //logger::trace("weaponType: {}", weaponType);
        //logger::trace("enchantmentType: {}", enchantmentType);
        //logger::trace("isAmmoBolt: {}", isBoltAmmo);
    }

private:
    void Reset()
    {
        *this = TargetData{};
    }

};

class CAHZTarget
{
public:
    static CAHZTarget& Singleton()
    {
        static CAHZTarget theInstance;
        return theInstance;
    };

    [[nodiscard]] TargetData GetTarget() const
    {
        std::lock_guard lock(m_snapshotMutex);
        return m_publishedTarget;
    }

    CAHZTarget(CAHZTarget& other) = delete;
    void operator=(const CAHZTarget&) = delete;

private:
    static bool IsSurvivalMode();

    //------------------Native Wrappers -----------------------------
    static void  GetMagicItemDescription_Native(void*, RE::TESForm* a1, RE::BSString* a2);
    static char* ProcessSurvivalMode_Native(RE::BSString* a2);
    static float GetArmorWarmthRating_Native(RE::TESForm* a1);
    static float GetActorWarmthRating_Native(RE::Actor* a1, float s2);
    //------------------End Native Wrappers -------------------------

private:
    friend class Events::CrosshairHandler;
    friend bool Events::Hook_WandLookupREFRByHandle_Impl(RE::RefHandle&, RE::NiPointer<RE::TESObjectREFR>&);

    CAHZTarget() = default;

    void SetTarget(RE::TESObjectREFR* pTargetRef);
    void DeriveTarget(RE::TESObjectREFR* pTargetRef);
    void UpdateTarget();
    void CaptureRolloverData();
    static std::string GetArmorWeightClass(const TargetData& target);
    static std::string GetBookSkillText(const TargetData& target);
    static float GetActualDamage(AHZWeaponData* weaponData);
    static float GetActualArmorRating(AHZArmorData* armorData);
    static float GetArmorWarmthRating(AHZArmorData* armorData);
    static float GetTotalWarmthRating();
    static float GetTotalActualArmorRating();
    static float GetArmorRatingDiff(const TargetData& target);
    static float GetWarmthRatingDiff(const TargetData& target);
    static float GetWeaponDamageDiff(const TargetData& target);
    static float GetTotalActualWeaponDamage();
    static bool IsTwoHanded(RE::TESObjectWEAP* weapon);
    static bool IsOneHanded(RE::TESObjectWEAP* weapon);
    static bool IsBow(RE::TESObjectWEAP* weapon);
    static bool IsBow(const TargetData& target);
    static bool IsCrossBow(RE::TESObjectWEAP* weapon);
    static bool IsCrossBow(const TargetData& target);
    static bool isBolt(RE::TESAmmo* ammo);
    static float mRound(float value);

    RE::TESObjectREFR* m_pObjectRef = nullptr;
    RE::TESForm*       m_pForm = nullptr;
    TargetData         m_target{};
    TargetData         m_publishedTarget{};
    mutable std::mutex m_snapshotMutex;
    std::atomic_uint64_t m_nextSnapshotSequence{ 0 };
    uint64_t m_publishedSnapshotSequence{};

    RE::IngredientItem* m_IngredientItem = nullptr;
    RE::AlchemyItem*    m_AlchemyItem = nullptr;
    RE::SpellItem*      m_SpellItem = nullptr;

    [[nodiscard]] bool IsReference()
    {
        return m_pObjectRef != nullptr;
    };
    [[nodiscard]] bool IsValid()
    {
        return m_pForm != nullptr;
    };
    [[nodiscard]] RE::TESObjectREFR* GetReference() { return m_pObjectRef; };
    [[nodiscard]] RE::TESForm*       GetForm() { return m_pForm; };

    [[nodiscard]] bool                             GetIsBoltAmmo();
    [[nodiscard]] EnchantmentType                  GetIsKnownEnchantment();
    [[nodiscard]] RE::WEAPON_TYPE                  GetWeaponType();
    [[nodiscard]] float                            GetWeight();
    [[nodiscard]] std::vector<std::string>         GetKnownIngredientEffects();
    [[nodiscard]] float                            GetArmorWarmthRating();
    [[nodiscard]] RE::BIPED_MODEL::BipedObjectSlot GetArmorSlotMask();
    [[nodiscard]] float                            GetArmorRating();
    [[nodiscard]] float                            GetWeaponDamage();
    [[nodiscard]] RE::ActorValue                   GetBookSkill();
    [[nodiscard]] bool                             GetIsSpellTome();
    [[nodiscard]] std::string                      GetEffectsDescription();
    [[nodiscard]] std::string                      GetDescription();
    [[nodiscard]] RE::BIPED_MODEL::ArmorType       GetArmorType();
    [[nodiscard]] bool                             GetIsBookAndWasRead();
    [[nodiscard]] RE::AlchemyItem*                 GetAlchemyItem();
    [[nodiscard]] RE::IngredientItem*              GetIngredient();
    [[nodiscard]] RE::SpellItem*                   GetSpellItem();
    [[nodiscard]] std::string                      GetTargetName();
    [[nodiscard]] std::string                      GetTargetSoulLevelName();
    void                                           UpdateInteriorCellDestination();
    [[nodiscard]] void                             GetMagicItemDescription(RE::MagicItem* item, std::string& description);

    [[nodiscard]] static RE::IngredientItem* GetIngredientFromLeveledList(RE::TESForm* thisObject);
    [[nodiscard]] static RE::AlchemyItem*    GetAlchemyItemFromLeveledList(RE::TESForm* thisObject);
    [[nodiscard]] static bool                CanPickUp(RE::TESForm* form);
    [[nodiscard]] static bool                GetIsNthEffectKnown(RE::IngredientItem* thisMagic, uint32_t index);
    [[nodiscard]] static bool                MagicDisallowEnchanting(const RE::BGSKeywordForm* pKeywords);
};
