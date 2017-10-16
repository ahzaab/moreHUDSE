#pragma once

class MatchByForm : public FormMatcher
{
	TESForm * m_form;
public:
	MatchByForm(TESForm * form) : m_form(form) {}

	bool Matches(TESForm* pForm) const { return m_form == pForm; }
};

class AHZWeaponData
{
public:
	EquipData equipData;
	TESObjectWEAP *weapon;
	TESAmmo *ammo;

	AHZWeaponData(void)
	{
		equipData.pForm = NULL;
		equipData.pExtraData = NULL;
		weapon = NULL;
		ammo = NULL;
		
	}
	~AHZWeaponData(void){}
};

class CAHZWeaponInfo
{
public:

	CAHZWeaponInfo(void);
	~CAHZWeaponInfo(void);
	
	static AHZWeaponData GetWeaponInfo(TESObjectREFR * thisObject);
	static AHZWeaponData GetLeftHandWeapon(void);
	static AHZWeaponData GetRightHandWeapon(void);
	static AHZWeaponData GetEquippedAmmo(void);
	static UInt32 GetGoldAmount(void);
};

