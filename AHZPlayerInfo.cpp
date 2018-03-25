#include "skse64/GameReferences.h"
#include "skse64/GameFormComponents.h"
#include "skse64/GameRTTI.h"
#include "skse64/GameForms.h"
#include "skse64/GameAPI.h"
#include "skse64/GameExtraData.h"
#include "AHZPlayerInfo.h"
#include "AHZScaleFormHook.h"
#include "skse64/Hooks_Gameplay.h"

//typedef  bool (__fastcall *GET_IS_IN_COMBAT)(PlayerCharacter *theActor);
//static GET_IS_IN_COMBAT pGetIsInCombat = (GET_IS_IN_COMBAT)0x0074E5F0;

//typedef  UInt32 (__fastcall *GET_GOLD_AMOUNT)(PlayerCharacter *theActor, int a);
//static GET_GOLD_AMOUNT pGetGoldAmount = (GET_GOLD_AMOUNT)0x1D6A90; // 0x1F49C0; //(GET_GOLD_AMOUNT)0x006A8190;


class ContainerFindItemByID
{
	UInt32 _formID;
public:
	ContainerFindItemByID(UInt32 formID) :_formID(formID)
	{ }

	bool Accept(TESContainer::Entry* pEntry)
	{
		if (pEntry && pEntry->form && pEntry->form->formID == _formID)
		{
			return true;
		}

		return false;
	}
};

class ContainerItemVistor
{	
public:
	UInt32 itemAmount;
	UInt32 _formID;
	ContainerItemVistor(UInt32 formID) :itemAmount(0), _formID(formID)
	{ }
	
	bool Accept(TESContainer::Entry* pEntry)
	{
		if (pEntry && pEntry->form && pEntry->form->formID == _formID)
		{
			//TESFullName* pFullName = DYNAMIC_CAST(pEntry, TESObjectREFR, TESFullName);
			itemAmount += pEntry->count;
		}
		return true;
	}
};

CAHZPlayerInfo::CAHZPlayerInfo(void)
{
}

CAHZPlayerInfo::~CAHZPlayerInfo(void)
{
}

UInt32 CAHZPlayerInfo::GetItemAmount(UInt32 formID)
{
	PlayerCharacter* pPC = (*g_thePlayer);
	if (pPC)
	{			
		TESContainer* pContainer = NULL;
		pContainer = DYNAMIC_CAST(pPC->baseForm, TESForm, TESContainer);
		if (!pContainer)
			return 0;
		
		// Get the data from the base container
		ContainerItemVistor itemVistor(formID);
		pContainer->Visit(itemVistor);

		// Now get the data from the container changes
		ExtraContainerChanges* pXContainerChanges = static_cast<ExtraContainerChanges*>(pPC->extraData.GetByType(kExtraData_ContainerChanges));
		if (!pXContainerChanges)
		{
			return itemVistor.itemAmount;
		}
		UInt32 length = pXContainerChanges->data->objList->Count();
		for (int i = 0; i < length; i++)
		{
			InventoryEntryData * data = pXContainerChanges->data->objList->GetNthItem(i);
			if (data && data->type && data->type->formID == formID)
			{
				itemVistor.itemAmount += data->countDelta;
				return itemVistor.itemAmount;
			}
		}
		return itemVistor.itemAmount;
	}
	return 0;
}

UInt32 CAHZPlayerInfo::GetGoldAmount(void)
{
	return CAHZPlayerInfo::GetItemAmount(0xF);
}


TESObjectREFR * CAHZPlayerInfo::GetTargetRef()
{
	return g_ahzTargetReference;
}

bool CAHZPlayerInfo::GetIsInCombat(void)
{
	PlayerCharacter* pPC = (*g_thePlayer);
	if (pPC)
	{	
		return pPC->IsInCombat();
	}
	return false;
}
