#include "skse64/GameReferences.h"
#include "skse64/GameFormComponents.h"
#include "skse64/GameRTTI.h"
#include "skse64/GameForms.h"
#include "skse64/GameAPI.h"
#include "skse64/GameExtraData.h"
#include "AHZPlayerInfo.h"
#include "AHZScaleFormHook.h"
#include "skse64/Hooks_Gameplay.h"
//static UInt32 * const targetRefHandle = (UInt32 *)0x2F8E9B0;//0x01B39A84;

//RelocPtr<UInt32> targetRefHandle(0x2EDCD80);
//RelocPtr<UInt32> targetRefHandle2(0x2FA85F0);


//RelocPtr<UInt32> targetRefHandle(0x2EDCE64);

//static TESObjectREFR *targetedRef = NULL;

//typedef  bool (__cdecl *GET_IS_IN_COMBAT)(int a1, int a2, PlayerCharacter *theActor);
//static GET_IS_IN_COMBAT pGetIsInCombat = (GET_IS_IN_COMBAT)0x008DB020;

typedef  bool (__fastcall *GET_IS_IN_COMBAT)(PlayerCharacter *theActor);
static GET_IS_IN_COMBAT pGetIsInCombat = (GET_IS_IN_COMBAT)0x0074E5F0;

typedef  UInt32 (__fastcall *GET_GOLD_AMOUNT)(PlayerCharacter *theActor, int a);
static GET_GOLD_AMOUNT pGetGoldAmount = (GET_GOLD_AMOUNT)0x1D6A90; // 0x1F49C0; //(GET_GOLD_AMOUNT)0x006A8190;




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

/*class ContainerFindItemByReference
{
	TESObjectREFR *_theObject;
public:
	ContainerFindItemByReference(TESObjectREFR *theObject) :_theObject(theObject)
	{ }

	bool Accept(TESContainer::Entry* pEntry)
	{
		if (pEntry && pEntry->form && pEntry->form == _theObject->baseForm)
		{
			if (pEntry->data == _theObject->extraData.)
				return true;
			else
				return false;
		}

		return false;
	}
};*/

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
	//return CAHZPlayerInfo::GetItemAmount(0x0F);
	//PlayerCharacter* pPC = (*g_thePlayer);
	//if (pPC)
	//{	
	//	return pGetGoldAmount(pPC, 0);
	//}

	return CAHZPlayerInfo::GetItemAmount(0xF);
	return 0;
}

//TESObjectREFR * CAHZPlayerInfo::GetTargetRef()
//{
//	return Hooks_Gameplay_GetCrosshairRef();
//}

TESObjectREFR * CAHZPlayerInfo::GetTargetRef()
{
	//TESObjectREFR * targetedRef = NULL;
	//UInt32 tempHandle = *targetRefHandle;
	////UInt32 *tempHandle2 = (UInt32*)*targetRefHandle2;

	////if (!*tempHandle)
	////{
	////	LookupREFRByHandle(tempHandle2, &targetedRef);
	////}
	////else
	////{
	//	LookupREFRByHandle(&tempHandle, &targetedRef);
	////}

	////targetedRef = (TESObjectREFR*)*targetRefHandle;// //= DYNAMIC_CAST(tempHandle, UInt32, TESObjectREFR);
	//return targetedRef;

	return ahzTargetReference;
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
