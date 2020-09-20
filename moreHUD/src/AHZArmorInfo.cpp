#include "PCH.h"
#include "AHZArmorInfo.h"


CAHZArmorInfo::CAHZArmorInfo(void)
{
}

CAHZArmorInfo::~CAHZArmorInfo(void)
{
}

AHZArmorData CAHZArmorInfo::GetArmorFromSlotMask(uint32_t slotMask)
{
	AHZArmorData data;
    auto         pPC = RE::PlayerCharacter::GetSingleton();
    auto         inventoryChanges = pPC->GetInventoryChanges();
    auto         armor = inventoryChanges->GetArmorInSlot(slotMask);

	if (armor) {
        auto list = inventoryChanges->entryList;
        //auto it = list->begin();

		for (auto it = list->begin(); it != list->end(); ++it) {
			auto entry = *it;
			if (entry->object->GetFormID() == armor->formID) {
				for (auto entryListIT = entry->extraLists->begin(); entryListIT != entry->extraLists->end(); ++entryListIT) {
                    auto extraData = *entryListIT;
					if (extraData &&
						(extraData->HasType(RE::ExtraDataType::kWorn) || extraData->HasType(RE::ExtraDataType::kWornLeft)))
					{
                        data.equipData.pForm = entry->object;
                        data.equipData.pExtraData = extraData;

						if (data.equipData.pForm) {
                            if (data.equipData.pForm->GetFormType() == RE::FormType::Armor) {
                                data.armor = DYNAMIC_CAST(data.equipData.pForm, RE::TESForm, RE::TESObjectARMO);
                            }
                            if (data.equipData.pForm->GetFormType() == RE::FormType::Light) {
                                data.torch = DYNAMIC_CAST(data.equipData.pForm, RE::TESForm, RE::TESObjectLIGH);
                            }
                        }

						return data;
					}
				}
			}

		}
		
		return data;
	}


}

