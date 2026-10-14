#pragma once

class CAHZTarget;

class CAHZPlayerInfo
{
public:
    CAHZPlayerInfo() = delete;
    ~CAHZPlayerInfo() = delete;
    static uint32_t GetGoldAmount();
    static bool GetIsInCombat();

private:
    friend class CAHZTarget;
    static uint32_t GetItemAmount(uint32_t formID);
};
