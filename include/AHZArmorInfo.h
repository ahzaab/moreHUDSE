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

    AHZArmorData(CAHZTarget& target)
    {
        if (target.IsValid()) {
            equipData.boundObject = target.GetForm()->As<RE::TESBoundObject>();
            equipData.pExtraData = target.IsReference() ? &target.GetReference()->extraList : nullptr;
            armor = target.GetForm()->As<RE::TESObjectARMO>();
            torch = target.GetForm()->As<RE::TESObjectLIGH>();
        } else {
            equipData.boundObject = nullptr;
            equipData.pExtraData = nullptr;
            armor = nullptr;
            torch = nullptr;
        }
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
