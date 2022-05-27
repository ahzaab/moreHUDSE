#pragma once

// Alignemnt comes from https://github.com/Ryan-rsm-McKenzie/CommonLibSSE/blob/master/include/RE/Projectile.h
// Credit goes to Ryan.  I only needed the one item
class AHZProjectile : public RE::TESObjectREFR
{
public:
    uint8_t                 unk98[0x147 - 0x98];    // 98
    RE::InventoryEntryData* extraData;              // 148
    uint8_t                 unk150[0x1B7 - 0x150];  // 150
    RE::TESAmmo*            sourceAmmo;             // 1B8
};

class AHZWeaponData
{
public:
    struct FoundEquipData
    {
        RE::TESBoundObject* boundObject;
        RE::ExtraDataList*  pExtraData;
    };

    FoundEquipData     equipData;
    RE::TESObjectWEAP* weapon;
    RE::TESAmmo*       ammo;

    AHZWeaponData()
    {
        equipData.boundObject = nullptr;
        equipData.pExtraData = nullptr;
        weapon = nullptr;
        ammo = nullptr;
    }
    ~AHZWeaponData() = default;
};

class CAHZWeaponInfo
{
public:
    CAHZWeaponInfo() = default;
    ~CAHZWeaponInfo() = default;
    static AHZWeaponData GetLeftHandWeapon(void);
    static AHZWeaponData GetRightHandWeapon(void);
    static AHZWeaponData GetEquippedAmmo(void);
};
