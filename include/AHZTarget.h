#pragma once

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
    bool                               bookRead{};
    RE::FormType                       formType{ RE::FormType::None };
    RE::FormID   formId;
    std::string  effectsDescription{};
    std::string                        description{};
    float        damage{};
    RE::BIPED_MODEL::ArmorType         armorType{ RE::BIPED_MODEL::ArmorType::kLightArmor }; //0
    float        armorRating{};
    float        armorWarmthRating{};
    RE::BIPED_MODEL::BipedObjectSlot   armorslotMask{ RE::BIPED_MODEL::BipedObjectSlot::kNone };
    std::vector<std::string>           knownIngredientEffects{};
    std::string                      name{};
    std::string                        displayName{};
    float                              weight{};
    RE::WEAPON_TYPE                    weaponType{};
    EnchantmentType                    enchantmentType{};
    bool                               isSpellTome{};
    RE::ActorValue                     bookSkill{};
    bool                               isBoltAmmo{};

    void Dump() const
    {
        logger::trace("\r\n");
        logger::trace("\r\n");
        logger::trace("******************************************");
        logger::trace("isValid: {}", isValid);
        logger::trace("canCarry: {}", canCarry);
        logger::trace("bookRead: {}", bookRead);
        logger::trace("formType: {}", formType);
        logger::trace("formId: {}", formId);
        logger::trace("effectsDescription: {}", effectsDescription);
        logger::trace("description: {}", description);
        logger::trace("bookSkill: {}", bookSkill);
        logger::trace("isSpellTome: {}", isSpellTome);
        logger::trace("damage: {}", damage);
        logger::trace("armorType: {}", armorType);
        logger::trace("armorRating: {}", armorRating);
        logger::trace("armorWarmthRating: {}", armorWarmthRating);
        logger::trace("armorslotMask: {}", armorslotMask);
        logger::trace("knownIngredientEffects:");
        for (auto& i: knownIngredientEffects) {
            logger::trace("   {}", i);
        }
        logger::trace("name: {}", name);
        logger::trace("displayName: {}", displayName);
        logger::trace("weight: {}", weight);
        logger::trace("weaponType: {}", weaponType);
        logger::trace("enchantmentType: {}", enchantmentType);
        logger::trace("isAmmoBolt: {}", isBoltAmmo);
        
    }

 private:
    void Reset()
    {
        isValid = false;
        canCarry = false;
        bookRead = false;
        formType = RE::FormType::None;
        formId = static_cast<RE::FormID>(0);
        effectsDescription.clear(); 
        description.clear();
        bookSkill = RE::ActorValue::kNone;
        isSpellTome = false;
        damage = 0;
        armorType = RE::BIPED_MODEL::ArmorType::kLightArmor; //0
        armorRating = 0;
        armorWarmthRating = 0;
        armorslotMask = RE::BIPED_MODEL::BipedObjectSlot::kNone;
        knownIngredientEffects.clear();
        name.clear();
        displayName.clear();
        weight = 0;
        weaponType = RE::WEAPON_TYPE::kHandToHandMelee; //0
        enchantmentType = EnchantmentType::None;
        isBoltAmmo = false;
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

    void SetTarget(RE::TESObjectREFR* pTargetRef);
    const TargetData& GetTarget() const noexcept { return m_target; };

    CAHZTarget(CAHZTarget& other) = delete;
    void operator=(const CAHZTarget&) = delete;

    //------------------Native Wrappers -----------------------------
    static void GetMagicItemDescription_Native(void*, RE::TESForm* a1, RE::BSString* a2);
    static char* ProcessSurvivalMode_Native(RE::BSString* a2);
    static float GetArmorWarmthRating_Native(RE::TESForm* a1);
    static float GetActorWarmthRating_Native(RE::Actor* a1, float s2);
    //------------------End Native Wrappers -------------------------

private:
    CAHZTarget() = default;
    
    void UpdateTarget();

    RE::TESObjectREFR* m_pObjectRef = nullptr;
    RE::TESForm*       m_pForm = nullptr;
    TargetData         m_target{};

    RE::IngredientItem* m_IngredientItem = nullptr;
    RE::AlchemyItem*    m_AlchemyItem = nullptr;
    RE::SpellItem*    m_SpellItem = nullptr;

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
    [[nodiscard]] float                      GetArmorRating();
    [[nodiscard]] float                      GetWeaponDamage();
    [[nodiscard]] RE::ActorValue             GetBookSkill();
    [[nodiscard]] bool                       GetIsSpellTome();
    [[nodiscard]] std::string                GetEffectsDescription();
    [[nodiscard]] std::string         GetDescription();
    [[nodiscard]] RE::BIPED_MODEL::ArmorType GetArmorType();
    [[nodiscard]] bool                GetIsBookAndWasRead();
    [[nodiscard]] RE::AlchemyItem*    GetAlchemyItem();
    [[nodiscard]] RE::IngredientItem* GetIngredient();
    [[nodiscard]] RE::SpellItem*      GetSpellItem();
    [[nodiscard]] std::string         GetTargetName();
    [[nodiscard]] std::string         GetTargetSoulLevelName();
    [[nodiscard]] void         GetMagicItemDescription(RE::MagicItem* item, std::string& description);

    [[nodiscard]] static RE::IngredientItem* GetIngredientFromLeveledList(RE::TESForm* thisObject);
    [[nodiscard]] static RE::AlchemyItem*    GetAlchemyItemFromLeveledList(RE::TESForm* thisObject);
    [[nodiscard]] static bool                CanPickUp(RE::TESForm* form);
    [[nodiscard]] static bool                GetIsNthEffectKnown(RE::IngredientItem* thisMagic, uint32_t index);
    [[nodiscard]] static bool                MagicDisallowEnchanting(const RE::BGSKeywordForm* pKeywords);
};