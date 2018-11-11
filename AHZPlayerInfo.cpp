#include "skse64/GameReferences.h"
#include "skse64/GameFormComponents.h"
#include "skse64/GameRTTI.h"
#include "skse64/GameForms.h"
#include "skse64/GameAPI.h"
#include "skse64/GameExtraData.h"
#include "AHZPlayerInfo.h"
#include "AHZScaleFormHook.h"
#include "skse64/Hooks_Gameplay.h"

typedef  UInt32(*GET_GOLD_AMOUNT)(Actor* a1);
RelocAddr<GET_GOLD_AMOUNT>GetGoldAmount_Native(0x005E74E0);
//.text:00007FF6D6A774E0
//.text : 00007FF6D6A774E0; ============== = S U B R O U T I N E ====================================== =
//.text:00007FF6D6A774E0
//.text : 00007FF6D6A774E0
//.text : 00007FF6D6A774E0; int __fastcall GetGoldAmount_Native(Actor *a1)
//.text:00007FF6D6A774E0                         GetGoldAmount_Native proc near; CODE XREF : sub_7FF6D67706E0 + 42↑p
//.text : 00007FF6D6A774E0; sub_7FF6D6AEDBB0 + 2AF↓p ...
//.text:00007FF6D6A774E0 48 83 EC 28                             sub     rsp, 28h
//.text : 00007FF6D6A774E4 48 83 C1 70                             add     rcx, 70h; a1
//.text:00007FF6D6A774E8 E8 23 CB B2 FF                          call    sub_7FF6D65A4010
//.text : 00007FF6D6A774ED 48 85 C0                                test    rax, rax
//.text : 00007FF6D6A774F0 74 0C                                   jz      short loc_7FF6D6A774FE
//.text : 00007FF6D6A774F2 48 8B C8                                mov     rcx, rax
//.text : 00007FF6D6A774F5 48 83 C4 28                             add     rsp, 28h
//.text : 00007FF6D6A774F9 E9 A2 05 C0 FF                          jmp     sub_7FF6D6677AA0
//.text : 00007FF6D6A774FE; -------------------------------------------------------------------------- -
//.text:00007FF6D6A774FE
//.text : 00007FF6D6A774FE                         loc_7FF6D6A774FE : ; CODE XREF : GetGoldAmount_Native + 10↑j
//.text : 00007FF6D6A774FE 48 83 C4 28                             add     rsp, 28h
//.text : 00007FF6D6A77502 C3                                      retn
//.text : 00007FF6D6A77502                         GetGoldAmount_Native endp
//.text : 00007FF6D6A77502
//.text : 00007FF6D6A77502; -------------------------------------------------------------------------- -

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
   PlayerCharacter* pPC = (*g_thePlayer);
   if (pPC)
   {
      return GetGoldAmount_Native(pPC);//CAHZPlayerInfo::GetItemAmount(0xF);
   }
   return 0;
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
