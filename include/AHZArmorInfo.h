#pragma once
#include "AHZTarget.h"

class AHZArmorData
{
public:
    struct FoundEquipData
    {
        RE::TESBoundObject* boundObject;
        RE::ExtraDataList*  pExtraData;
    };

    FoundEquipData     equipData;
    RE::TESObjectARMO* armor;
    RE::TESObjectLIGH* torch;

    AHZArmorData(void)
    {
        equipData.boundObject = nullptr;
        equipData.pExtraData = nullptr;
        armor = nullptr;
        torch = nullptr;
    }

    ~AHZArmorData() = default;
};

class CAHZArmorInfo
{
public:
    CAHZArmorInfo() = default;
    ~CAHZArmorInfo() = default;
    static AHZArmorData GetArmorFromSlotMask(RE::BIPED_MODEL::BipedObjectSlot slotMask);

private:
    static RE::ExtraDataList* GetWornExtraList(RE::BSSimpleList<RE::ExtraDataList*>* extraList);
};
