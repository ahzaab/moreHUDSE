#include "PCH.h"
#include "AHZTarget.h"
#include "AHZArmorInfo.h"


auto CAHZArmorInfo::GetWornExtraList(RE::BSSimpleList<RE::ExtraDataList*>* extraList) -> RE::ExtraDataList*
{
    if (extraList) {
        for (auto entryListIT = extraList->begin(); entryListIT != extraList->end(); ++entryListIT) {
            auto extraData = *entryListIT;
            if (extraData &&
                (extraData->HasType(RE::ExtraDataType::kWorn) || extraData->HasType(RE::ExtraDataType::kWornLeft))) {
                return extraData;
            }
        }
    }

    return nullptr;
}

auto CAHZArmorInfo::GetArmorFromSlotMask(RE::BIPED_MODEL::BipedObjectSlot slotMask) -> AHZArmorData
{
    AHZArmorData        data;
    auto                pPC = RE::PlayerCharacter::GetSingleton();
    auto                inventoryChanges = pPC->GetInventoryChanges();
    RE::TESBoundObject* armor;
    //= inventoryChanges->GetArmorInSlot(static_cast<uint32_t>(slotMask));

    if (inventoryChanges) {
        auto list = inventoryChanges->entryList;
        if (list) {
            for (auto it = list->begin(); it != list->end(); ++it) {
                auto element = *it;
                if (element) {
                    if (element->object && (element->object->GetFormType() == RE::FormType::Armor || element->object->GetFormType() == RE::FormType::Light)) {
                        auto form = DYNAMIC_CAST(element->object, RE::TESBoundObject, RE::BGSBipedObjectForm);
                        if (form && (static_cast<uint32_t>(form->GetSlotMask()) & static_cast<uint32_t>(slotMask))) {
                            armor = element->object;
                            auto wornList = GetWornExtraList(element->extraLists);

                            if (wornList && armor) {
                                data.equipData.boundObject = armor;
                                data.equipData.pExtraData = wornList;

                                if (data.equipData.boundObject) {
                                    if (data.equipData.boundObject->GetFormType() == RE::FormType::Armor) {
                                        data.armor = data.equipData.boundObject->As<RE::TESObjectARMO>();
                                    }
                                    if (data.equipData.boundObject->GetFormType() == RE::FormType::Light) {
                                        data.torch = data.equipData.boundObject->As<RE::TESObjectLIGH>();
                                    }
                                }
                                return data;
                            }
                        }
                    }
                }
            }
        }
    }
    return data;
}
