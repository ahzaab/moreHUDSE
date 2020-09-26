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

        if (!inventoryChanges) {
            return data;
        }

        auto list = inventoryChanges->entryList;
        //auto it = list->begin();

        if (!list) {
            return data;
        }

        for (auto it = list->begin(); it != list->end(); ++it) {
            auto entry = *it;
            if (entry && entry->object->GetFormID() == armor->formID) {
                if (entry->extraLists) {
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
        }

        return data;
    }

    return data;
}

