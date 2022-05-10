#include "PCH.h"
#include "AHZTarget.h"
#include "AHZPlayerInfo.h"
#include "Events.h"

auto CAHZPlayerInfo::GetItemAmount(uint32_t formID) -> uint32_t
{
    auto pPC = RE::PlayerCharacter::GetSingleton();

    if (!pPC)
        return 0;

    auto pContainer = pPC->GetBaseObject()->As<RE::TESContainer>();
    if (!pContainer)
        return 0;

    auto inventoryCounts = pPC->GetInventoryCounts([formID](RE::TESBoundObject& obj) {
        return (obj.GetFormID() == formID);
    });

    if (inventoryCounts.empty()) {
        return 0;
    }

    auto counts = inventoryCounts.begin()->second;

    return counts < 0 ? 0 : counts;
}

auto CAHZPlayerInfo::GetGoldAmount() -> uint32_t
{
    auto pPC = RE::PlayerCharacter::GetSingleton();

    if (!pPC)
        return 0;

    auto goldAmount = pPC->GetGoldAmount();
    return goldAmount < 0 ? 0 : goldAmount;
}

auto CAHZPlayerInfo::GetIsInCombat() -> bool
{
    auto pPC = RE::PlayerCharacter::GetSingleton();
    if (!pPC)
        return false;

    return pPC->IsInCombat();
}
