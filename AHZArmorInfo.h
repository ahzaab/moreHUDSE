#pragma once


class AHZArmorData
{
public:
	EquipData equipData;
	TESObjectARMO *armor;
	TESObjectLIGH *torch;

	AHZArmorData(void)
	{
		equipData.pForm = NULL;
		equipData.pExtraData = NULL;
		armor = NULL;
		torch = NULL;
	}

	AHZArmorData(TESObjectREFR *thisObject)
	{
		if (thisObject && thisObject->baseForm)
		{
			equipData.pForm = thisObject->baseForm;
			equipData.pExtraData = &thisObject->extraData;
			armor = DYNAMIC_CAST(thisObject->baseForm, TESForm, TESObjectARMO);
			torch = DYNAMIC_CAST(thisObject->baseForm, TESForm, TESObjectLIGH);
		}
		else
		{
			equipData.pForm = NULL;
			equipData.pExtraData = NULL;
			armor = NULL;
			torch = NULL;
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

