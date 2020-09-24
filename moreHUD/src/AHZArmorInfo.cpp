#include "PCH.h"
#include "AHZArmorInfo.h"


CAHZArmorInfo::CAHZArmorInfo(void)
{
}

CAHZArmorInfo::~CAHZArmorInfo(void)
{
}

AHZArmorData CAHZArmorInfo::GetArmorFromSlotMask(RE::BIPED_MODEL::BipedObjectSlot slotMask)
{
    AHZArmorData data;
    auto         pPC = RE::PlayerCharacter::GetSingleton();
    auto         inventoryChanges = pPC->GetInventoryChanges();
    auto         armor = inventoryChanges->GetArmorInSlot(static_cast<uint32_t>(slotMask));

    if (armor) {
        auto list = inventoryChanges->entryList;
        //auto it = list->begin();

        for (auto it = list->begin(); it != list->end(); ++it) {
            auto entry = *it;
            if (entry->object->GetFormID() == armor->formID) {
                for (auto entryListIT = entry->extraLists->begin(); entryListIT != entry->extraLists->end(); ++entryListIT) {
                    auto extraData = *entryListIT;
                    if (extraData &&
                        (extraData->HasType(RE::ExtraDataType::kWorn) || extraData->HasType(RE::ExtraDataType::kWornLeft))) {
                        data.equipData.boundObject = entry->object;
                        data.equipData.pExtraData = extraData;

                        if (data.equipData.boundObject) {
                            if (data.equipData.boundObject->GetFormType() == RE::FormType::Armor) {
                                data.armor = DYNAMIC_CAST(data.equipData.boundObject, RE::TESForm, RE::TESObjectARMO);
                            }
                            if (data.equipData.boundObject->GetFormType() == RE::FormType::Light) {
                                data.torch = DYNAMIC_CAST(data.equipData.boundObject, RE::TESForm, RE::TESObjectLIGH);
                            }
                        }

                        return data;
                    }
                }
            }
        }

        return data;
    }

    return data;
}

