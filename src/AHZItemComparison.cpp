#include "pch.h"

#include "AHZItemComparison.h"
#include "Offsets-AE.h"
#include "Offsets-SE.h"

CAHZItemComparisonContext::CAHZItemComparisonContext()
{
    m_initialized = BuildEquippedArmorContext_Native(&m_context) != nullptr;
}

CAHZItemComparisonContext::~CAHZItemComparisonContext()
{
    if (m_initialized) {
        DestroyEquippedArmorContext_Native(&m_context);
    }
}

std::optional<AHZItemComparisonResult> CAHZItemComparisonContext::Compare(
    RE::TESBoundObject* a_item,
    RE::ExtraDataList* a_extraData,
    bool a_compareWithEquipped,
    bool a_ignoreWornState)
{
    if (!m_initialized || !a_item) {
        return std::nullopt;
    }

    RE::InventoryEntryData entry(a_item, 0);
    if (a_extraData) {
        entry.AddExtraList(a_extraData);
    }

    AHZItemComparisonResult result;
    CalculateItemComparison_Native(
        &m_context,
        &entry,
        &result.statChange,
        &result.valueChange,
        a_compareWithEquipped,
        a_ignoreWornState);
    return result;
}

// Equipped-armor comparison snapshot builder: 14088B9B0 (1.5.97), 14092AE10 (1.6.1170), 140941160 (1.7.99)
CAHZItemComparisonContext::EngineContext* CAHZItemComparisonContext::BuildEquippedArmorContext_Native(
    [[maybe_unused]] EngineContext* a_context)
{
    if (REL::Module::IsSE() || REL::Module::IsAE()) {
        using func_t = decltype(&CAHZItemComparisonContext::BuildEquippedArmorContext_Native);
        REL::Relocation<func_t> func{ REL::RelocationID{
            moreHUDSE::SE::Offsets::BuildEquippedArmorComparisonContext.id(),
            moreHUDSE::AE::Offsets::BuildEquippedArmorComparisonContext.id() } };
        return func(a_context);
    }
    return nullptr;
}

// Item comparison calculator: 14088BAD0 (1.5.97), 14092B0D0 (1.6.1170), 140941420 (1.7.99)
void CAHZItemComparisonContext::CalculateItemComparison_Native(
    [[maybe_unused]] EngineContext* a_context,
    [[maybe_unused]] RE::InventoryEntryData* a_item,
    [[maybe_unused]] float* a_statChange,
    [[maybe_unused]] std::int32_t* a_valueChange,
    [[maybe_unused]] bool a_compareWithEquipped,
    [[maybe_unused]] bool a_ignoreWornState)
{
    if (REL::Module::IsSE() || REL::Module::IsAE()) {
        using func_t = decltype(&CAHZItemComparisonContext::CalculateItemComparison_Native);
        REL::Relocation<func_t> func{ REL::RelocationID{
            moreHUDSE::SE::Offsets::CalculateItemComparison.id(),
            moreHUDSE::AE::Offsets::CalculateItemComparison.id() } };
        func(a_context, a_item, a_statChange, a_valueChange, a_compareWithEquipped, a_ignoreWornState);
    }
}

// Equipped-armor comparison snapshot destructor: 140861560 (1.5.97), 1408FEA20 (1.6.1170), 140914780 (1.7.99)
CAHZItemComparisonContext::EngineContext* CAHZItemComparisonContext::DestroyEquippedArmorContext_Native(
    [[maybe_unused]] EngineContext* a_context)
{
    if (REL::Module::IsSE() || REL::Module::IsAE()) {
        using func_t = decltype(&CAHZItemComparisonContext::DestroyEquippedArmorContext_Native);
        REL::Relocation<func_t> func{ REL::RelocationID{
            moreHUDSE::SE::Offsets::DestroyEquippedArmorComparisonContext.id(),
            moreHUDSE::AE::Offsets::DestroyEquippedArmorComparisonContext.id() } };
        return func(a_context);
    }
    return nullptr;
}
