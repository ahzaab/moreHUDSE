#pragma once

class CAHZPlayerInfo
{
public:
	CAHZPlayerInfo(void);
	~CAHZPlayerInfo(void);

	static RE::TESObjectREFR * GetTargetRef();
	static uint32_t GetItemAmount(uint32_t formID);
	static uint32_t GetGoldAmount(void);
	static bool GetIsInCombat(void);
};

