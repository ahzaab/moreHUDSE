#pragma once


class AHZArmorData
{
public:
	EquipData equipData;
	TESObjectARMO *armor;

	AHZArmorData(void)
	{
		equipData.pForm = NULL;
		equipData.pExtraData = NULL;
		armor = NULL;
	}

	AHZArmorData(TESObjectREFR *thisObject)
	{
		if (thisObject && thisObject->baseForm)
		{
			equipData.pForm = thisObject->baseForm;
			equipData.pExtraData = &thisObject->extraData;
			armor = DYNAMIC_CAST(thisObject->baseForm, TESForm, TESObjectARMO);
		}
		else
		{
			equipData.pForm = NULL;
			equipData.pExtraData = NULL;
			armor = NULL;
		}
	}

	~AHZArmorData(void){}
};

class CAHZArmorInfo
{
public:
	CAHZArmorInfo(void);
	~CAHZArmorInfo(void);
	static AHZArmorData GetArmorFromSlotMask(UInt32 slotMask);
};

