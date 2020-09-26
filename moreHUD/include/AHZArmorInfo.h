#pragma once


class AHZArmorData
{
public:

	struct FoundEquipData
    {
        RE::TESBoundObject* boundObject;
        RE::ExtraDataList*  pExtraData;
    };

    FoundEquipData equipData;
	RE::TESObjectARMO *armor;
	RE::TESObjectLIGH *torch;

	AHZArmorData(void)
	{
		equipData.boundObject = nullptr;
        equipData.pExtraData = nullptr;
        armor = nullptr;
        torch = nullptr;
	}

	AHZArmorData(RE::TESObjectREFR *thisObject)
	{
		if (thisObject && thisObject->GetBaseObject())
		{
			equipData.boundObject = thisObject->GetBaseObject();
			equipData.pExtraData = &thisObject->extraList;
            armor = DYNAMIC_CAST(thisObject->GetBaseObject(), RE::TESForm, RE::TESObjectARMO);
            torch = DYNAMIC_CAST(thisObject->GetBaseObject(), RE::TESForm, RE::TESObjectLIGH);
		}
		else
		{
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

