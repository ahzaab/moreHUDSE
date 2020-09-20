#pragma once

class CAHZPlayerInfo
{
public:
	CAHZPlayerInfo(void);
	~CAHZPlayerInfo(void);

	static TESObjectREFR * GetTargetRef();
	static UInt32 GetItemAmount(UInt32 formID);
	static UInt32 GetGoldAmount(void);
	static bool GetIsInCombat(void);
};

