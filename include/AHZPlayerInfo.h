#pragma once

class CAHZPlayerInfo
{
public:
    CAHZPlayerInfo() = delete;
    ~CAHZPlayerInfo() = delete;

    static CAHZTarget GetTarget();
    static uint32_t GetItemAmount(uint32_t formID);
    static uint32_t GetGoldAmount();
    static bool     GetIsInCombat();

private:
    static RE::TESObjectREFRPtr GetTargetRef();
};
