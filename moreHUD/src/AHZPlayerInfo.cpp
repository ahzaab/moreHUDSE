#include "PCH.h"
#include "AHZPlayerInfo.h"
#include "Events.h"


uint32_t CAHZPlayerInfo::GetItemAmount(uint32_t formID)
{
    auto pPC = RE::PlayerCharacter::GetSingleton();

    auto pContainer = DYNAMIC_CAST(pPC->GetBaseObject(), RE::TESForm, RE::TESContainer);
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

uint32_t CAHZPlayerInfo::GetGoldAmount(void)
{
    auto goldAmount = RE::PlayerCharacter::GetSingleton()->GetGoldAmount();
    return goldAmount < 0 ? 0 : goldAmount;
}


RE::TESObjectREFR* CAHZPlayerInfo::GetTargetRef()
{
    return Events::CrosshairRefManager::GetSingleton()->GetCrosshairReference();
}

bool CAHZPlayerInfo::GetIsInCombat()
{
    auto pPC = RE::PlayerCharacter::GetSingleton();
    if (pPC) {
        return pPC->IsInCombat();
    }
    return false;
}
