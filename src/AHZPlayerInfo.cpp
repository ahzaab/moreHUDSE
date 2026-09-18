#include "pch.h"
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
    const auto pPC = RE::PlayerCharacter::GetSingleton();

    if (!pPC) {
        return 0;
    }

    // Crash log 2JzsBCjH showed CommonLib's Actor::GetGoldAmount dereferencing a null
    // kGold default-object pointer while servicing GetPlayerData. The inventory is already
    // filterable by TESForm::IsGold(), so avoid depending on that fallible DOBJ lookup.
    const auto inventory = pPC->GetInventory([](RE::TESBoundObject& a_object) {
        return a_object.IsGold();
    });
    if (inventory.empty()) {
        return 0;
    }

    const auto goldAmount = inventory.begin()->second.first;
    return goldAmount < 0 ? 0 : static_cast<uint32_t>(goldAmount);
}

auto CAHZPlayerInfo::GetIsInCombat() -> bool
{
    auto pPC = RE::PlayerCharacter::GetSingleton();
    if (!pPC)
        return false;

    return pPC->IsInCombat();
}
