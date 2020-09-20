#pragma once

// Alignemnt comes from https://github.com/Ryan-rsm-McKenzie/CommonLibSSE/blob/master/include/RE/Projectile.h
// Credit goes to Ryan.  I only needed the one item 
class AHZProjectile : public TESObjectREFR
{
public:
    UInt8	unk98[0x147 - 0x98];	// 98
    InventoryEntryData *extraData;	// 148
    UInt8	unk150[0x1B7 - 0x150];	// 150
    TESAmmo * sourceAmmo;			// 1B8
};

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
    //static bool IsProjectileWithSource(TESObjectREFR * objectRef);
};

