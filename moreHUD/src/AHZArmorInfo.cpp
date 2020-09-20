#include "skse64/GameReferences.h"
#include "skse64/GameFormComponents.h"
#include "skse64/GameRTTI.h"
#include "skse64/GameForms.h"
#include "skse64/GameAPI.h"
#include "skse64/GameExtraData.h"
#include "AHZArmorInfo.h"

class MatchBySlot : public FormMatcher
{
	UInt32 m_mask;
public:
	MatchBySlot(UInt32 slot) : 
	  m_mask(slot) 
	{
		
	}

	bool Matches(TESForm* pForm) const {
		if (pForm) {
			BGSBipedObjectForm* pBip = DYNAMIC_CAST(pForm, TESForm, BGSBipedObjectForm);
			if (pBip) {
				return (pBip->data.parts & m_mask) != 0;
			}
		}
		return false;
	}
};

CAHZArmorInfo::CAHZArmorInfo(void)
{
}

CAHZArmorInfo::~CAHZArmorInfo(void)
{
}

AHZArmorData CAHZArmorInfo::GetArmorFromSlotMask(UInt32 slotMask)
{
	AHZArmorData data;
	PlayerCharacter* pPC = (*g_thePlayer);
	if (pPC)
	{
		MatchBySlot matcher((UInt32)slotMask);	
		ExtraContainerChanges* pContainerChanges = static_cast<ExtraContainerChanges*>(pPC->extraData.GetByType(kExtraData_ContainerChanges));
		if (pContainerChanges) 
		{
			data.equipData = pContainerChanges->FindEquipped(matcher);
			if (data.equipData.pForm)
			{
            if (data.equipData.pForm->GetFormType() == kFormType_Armor)
            {
               data.armor = DYNAMIC_CAST(data.equipData.pForm, TESForm, TESObjectARMO);
            }
            if (data.equipData.pForm->GetFormType() == kFormType_Light)
            {
               data.torch = DYNAMIC_CAST(data.equipData.pForm, TESForm, TESObjectLIGH);
            }
			}
		}
	}
	return data;
}

