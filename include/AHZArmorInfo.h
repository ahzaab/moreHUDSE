#pragma once


class AHZArmorData
{
public:
    struct FoundEquipData
    {
        const RE::TESBoundObject* boundObject;
        const RE::ExtraDataList*  pExtraData;
    };

    FoundEquipData     equipData;
    const RE::TESObjectARMO* armor;
    const RE::TESObjectLIGH* torch;

    AHZArmorData(void)
    {
        equipData.boundObject = nullptr;
        equipData.pExtraData = nullptr;
        armor = nullptr;
        torch = nullptr;
    }

    AHZArmorData(CAHZTarget target)
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
