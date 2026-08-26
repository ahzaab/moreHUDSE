#pragma once

struct AHZItemComparisonResult
{
    float        statChange{ 0.0F };
    std::int32_t valueChange{ 0 };
};

class CAHZItemComparisonContext
{
public:
    CAHZItemComparisonContext();
    ~CAHZItemComparisonContext();

    CAHZItemComparisonContext(const CAHZItemComparisonContext&) = delete;
    CAHZItemComparisonContext(CAHZItemComparisonContext&&) = delete;
    CAHZItemComparisonContext& operator=(const CAHZItemComparisonContext&) = delete;
    CAHZItemComparisonContext& operator=(CAHZItemComparisonContext&&) = delete;

    [[nodiscard]] bool IsAvailable() const noexcept { return m_initialized; }
    [[nodiscard]] std::optional<AHZItemComparisonResult> Compare(
        RE::TESBoundObject* a_item,
        RE::ExtraDataList* a_extraData,
        bool a_compareWithEquipped = true,
        bool a_ignoreWornState = false);

private:
    struct EquippedArmorSlotValue
    {
        float              rating{ 0.0F };
        std::uint32_t      pad04{ 0 };
        RE::TESObjectARMO* armor{ nullptr };
    };
    static_assert(sizeof(EquippedArmorSlotValue) == 0x10);

    struct EngineContext
    {
        static constexpr std::size_t kBipedSlotCount = 32;

        RE::BSTArray<EquippedArmorSlotValue> armorRatingsBySlot;
        RE::BSTArray<EquippedArmorSlotValue> warmthRatingsBySlot;
        std::array<std::int32_t, kBipedSlotCount> goldValuesBySlot{};
    };
    static_assert(offsetof(EngineContext, armorRatingsBySlot) == 0x00);
    static_assert(offsetof(EngineContext, warmthRatingsBySlot) == 0x18);
    static_assert(offsetof(EngineContext, goldValuesBySlot) == 0x30);
    static_assert(sizeof(EngineContext) == 0xB0);

    static EngineContext* BuildEquippedArmorContext_Native(EngineContext* a_context);
    static void CalculateItemComparison_Native(EngineContext* a_context, RE::InventoryEntryData* a_item,
        float* a_statChange, std::int32_t* a_valueChange, bool a_compareWithEquipped, bool a_ignoreWornState);
    static EngineContext* DestroyEquippedArmorContext_Native(EngineContext* a_context);

    EngineContext m_context{};
    bool          m_initialized{ false };
};
