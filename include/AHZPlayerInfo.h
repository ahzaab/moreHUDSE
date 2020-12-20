#pragma once

class CAHZPlayerInfo
{
public:
    CAHZPlayerInfo() = default;
    ~CAHZPlayerInfo() = default;

    static RE::TESObjectREFR* GetTargetRef();
    static uint32_t           GetItemAmount(uint32_t formID);
    static uint32_t           GetGoldAmount();
    static bool               GetIsInCombat();
};
