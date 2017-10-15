#include "skse64/PluginAPI.h"
#include "skse64/skse64_common/skse_version.h"
#include "skse64/skse64_common/SafeWrite.h"
#include "skse64/skse64_common/BranchTrampoline.h"
#include "skse64/ScaleformCallbacks.h"
#include "skse64/ScaleformMovie.h"
#include "skse64/GameAPI.h"
#include "skse64/GameData.h"
#include "skse64/GameObjects.h"
#include "skse64/PapyrusNativeFunctions.h"
#include "skse64/Hooks_Papyrus.h"
#include "skse64/GameTypes.h"
#include "skse64/GameSettings.h"
#include "skse64/GameReferences.h"
#include "skse64/GameFormComponents.h"
#include "skse64/GameForms.h"
#include "skse64/GameRTTI.h"
#include "skse64/GameMenus.h"
#include "skse64/PapyrusUI.h"
#include "skse64/PapyrusVM.h"
#include "skse64/PapyrusEvents.h"
#include "skse64/GameExtraData.h"
#include "skse64/PapyrusUtility.h"
#include "PluginProcessInfo.h"
#include "AHZPlayerInfo.h"
#include "AHZUtility.h"
#include "AHZScaleformHook.h"
#include "../common/IErrors.h"
#include "skse64/PapyrusIngredient.h"
#include "AHZScaleformHook.h"
#include "skse64/GameEvents.h"
#include "skse64/GameMenus.h"
using namespace std;

#define AHZ_NON_ENGLISH		std::wstring
#define AHZ_ENGLISH			std::wstring

IDebugLog	gLog("AHZmoreHUDPlugin.log");

PluginHandle	g_pluginHandle = kPluginHandle_Invalid;
static UInt32 g_skseVersion = 0;
SKSEScaleformInterface		* g_scaleform = NULL;
SKSEMessagingInterface *g_skseMessaging = NULL;
AHZEventHandler menuEvent;
AHZCrosshairRefEventHandler crossHairEvent;
TESObjectREFR*	g_MycurCrosshairRef = NULL;
InventoryEntryData *g_objDec;

/**** scaleform functions ****/

class SKSEScaleform_InstallHooks : public GFxFunctionHandler
{
public:
	virtual void	Invoke(Args * args)
	{
	}
};

class SKSEScaleform_GetIngredientData : public GFxFunctionHandler
{
public:
	virtual void	Invoke(Args * args)
	{				
		if (IsEnglish())
		{
			CAHZUtility<AHZ_ENGLISH> utility;
			utility.ProcessIngredientData(CAHZPlayerInfo::GetTargetRef(), args);
		}
		else
		{
			CAHZUtility<AHZ_NON_ENGLISH> utility;
			utility.ProcessIngredientData(CAHZPlayerInfo::GetTargetRef(), args);
		}
	}
};

class SKSEScaleform_GetTargetObjectData : public GFxFunctionHandler
{
public:
	virtual void	Invoke(Args * args)
	{	
		if (IsEnglish())
		{
			CAHZUtility<AHZ_ENGLISH> utility;
			utility.ProcessTargetObject(CAHZPlayerInfo::GetTargetRef(), args);
		}
		else
		{
			CAHZUtility<AHZ_NON_ENGLISH> utility;
			utility.ProcessTargetObject(CAHZPlayerInfo::GetTargetRef(), args);
		}
	}
};

class SKSEScaleform_GetPlayerData : public GFxFunctionHandler
{
public:
	virtual void	Invoke(Args * args)
	{		
		if (IsEnglish())
		{
			CAHZUtility<AHZ_ENGLISH> utility;
			utility.ProcessPlayerData(args);
		}
		else
		{
			CAHZUtility<AHZ_NON_ENGLISH> utility;
			utility.ProcessPlayerData(args);
		}
	}
};

class SKSEScaleform_GetIsPlayerInCombat : public GFxFunctionHandler
{
public:
	virtual void	Invoke(Args * args)
	{		
		args->result->SetBool(CAHZPlayerInfo::GetIsInCombat());
	}
};

class SKSEScaleform_GetTargetInventoryCount : public GFxFunctionHandler
{
public:
	virtual void	Invoke(Args * args)
	{
		if (IsEnglish())
		{
			CAHZUtility<AHZ_ENGLISH> utility;
			utility.ProcessInventoryCount(CAHZPlayerInfo::GetTargetRef(), args);
		}
		else
		{
			CAHZUtility<AHZ_NON_ENGLISH> utility;
			utility.ProcessInventoryCount(CAHZPlayerInfo::GetTargetRef(), args);
		}
	}
};

class SKSEScaleform_GetTargetEffects : public GFxFunctionHandler
{
public:
	virtual void	Invoke(Args * args)
	{
		if (IsEnglish())
		{
			CAHZUtility<AHZ_ENGLISH> utility;
			utility.ProcessTargetEffects(CAHZPlayerInfo::GetTargetRef(), args);
		}
		else
		{
			CAHZUtility<AHZ_NON_ENGLISH> utility;
			utility.ProcessTargetEffects(CAHZPlayerInfo::GetTargetRef(), args);
		}
	}
};

class SKSEScaleform_GetIsBookAndWasRead : public GFxFunctionHandler
{
public:
	virtual void	Invoke(Args * args)
	{
		TESObjectREFR * pTargetReference = CAHZPlayerInfo::GetTargetRef();

		// If the target is not valid or it can't be picked up by the player
		if (!pTargetReference)
		{
			args->result->SetBool(false);
			return;
		}

		if (IsEnglish())
		{
			CAHZUtility<AHZ_ENGLISH> utility;
			args->result->SetBool(utility.GetIsBookAndWasRead(pTargetReference));
		}
		else
		{
			CAHZUtility<AHZ_NON_ENGLISH> utility;
			args->result->SetBool(utility.GetIsBookAndWasRead(pTargetReference));
		}
	}
};

class SKSEScaleform_GetArmorWeightClassString : public GFxFunctionHandler
{
public:
	virtual void	Invoke(Args * args)
	{
		if (IsEnglish())
		{
			CAHZUtility<AHZ_ENGLISH> utility;
			utility.ProcessArmorClass(CAHZPlayerInfo::GetTargetRef(), args);
		}
		else
		{
			CAHZUtility<AHZ_NON_ENGLISH> utility;
			utility.ProcessArmorClass(CAHZPlayerInfo::GetTargetRef(), args);
		}
	}
};

class SKSEScaleform_GetBookSkillString : public GFxFunctionHandler
{
public:
	virtual void	Invoke(Args * args)
	{
		if (IsEnglish())
		{
			CAHZUtility<AHZ_ENGLISH> utility;
			utility.ProcessBookSkill(CAHZPlayerInfo::GetTargetRef(), args);
		}
		else
		{
			CAHZUtility<AHZ_NON_ENGLISH> utility;
			utility.ProcessBookSkill(CAHZPlayerInfo::GetTargetRef(), args);
		}
	}
};

class SKSEScaleform_GetIsValidTarget : public GFxFunctionHandler
{
public:
	virtual void	Invoke(Args * args)
	{
		if (IsEnglish())
		{
			CAHZUtility<AHZ_ENGLISH> utility;
			utility.ProcessValidTarget(CAHZPlayerInfo::GetTargetRef(), args);
		}
		else
		{
			CAHZUtility<AHZ_NON_ENGLISH> utility;
			utility.ProcessValidTarget(CAHZPlayerInfo::GetTargetRef(), args);
		}
	}
};



bool __cdecl Hook_Crosshair_LookupREFRByHandle2(UInt32 * refHandle, TESObjectREFR ** refrOut)
{
	bool result = LookupREFRByHandle(refHandle, refrOut);

	if (refrOut)
	{
		g_MycurCrosshairRef = *refrOut;
	}
	else
	{
		g_MycurCrosshairRef = NULL;
	}

	return result;
}

bool RegisterScaleform(GFxMovieView * view, GFxValue * root)
{
	RegisterFunction <SKSEScaleform_InstallHooks>(root, view, "InstallHooks");
	RegisterFunction <SKSEScaleform_GetIngredientData>(root, view, "GetIngredientData");
	RegisterFunction <SKSEScaleform_GetTargetObjectData>(root, view, "GetTargetObjectData");
	RegisterFunction <SKSEScaleform_GetPlayerData>(root, view, "GetPlayerData");
	RegisterFunction <SKSEScaleform_GetIsValidTarget>(root, view, "GetIsValidTarget");
	RegisterFunction <SKSEScaleform_GetIsPlayerInCombat>(root, view, "GetIsPlayerInCombat");
	RegisterFunction <SKSEScaleform_GetTargetInventoryCount>(root, view, "GetTargetInventoryCount");
	RegisterFunction <SKSEScaleform_GetTargetEffects>(root, view, "GetTargetEffects");
	RegisterFunction <SKSEScaleform_GetIsBookAndWasRead>(root, view, "GetIsBookAndWasRead");
	RegisterFunction <SKSEScaleform_GetArmorWeightClassString>(root, view, "GetArmorWeightClassString");
	RegisterFunction <SKSEScaleform_GetBookSkillString>(root, view, "GetBookSkillString");

	//InstallAHZHudComponents(view);

	MenuManager::GetSingleton()->MenuOpenCloseEventDispatcher()->AddEventSink(&menuEvent);


	return true;
}

void AhzInventoryData(GFxMovieView * view, GFxValue * object, InventoryEntryData * item)
{
	g_objDec = item;
}

extern "C"
{

bool SKSEPlugin_Query(const SKSEInterface * skse, PluginInfo * info)
{
	//_MESSAGE("AHZmoreHUDPlugin");

	// populate info structure
	info->infoVersion =	PluginInfo::kInfoVersion;
	info->name =		"Ahzaab's moreHUD Plugin";
	info->version =		300;

	// store plugin handle so we can identify ourselves later
	g_pluginHandle = skse->GetPluginHandle();

	if(skse->isEditor)
	{
		_ERROR("loaded in editor, marking as incompatible");

		return false;
	}
	else if(skse->runtimeVersion != RUNTIME_VERSION_1_5_3)
	{
		_ERROR("unsupported runtime version %08X", skse->runtimeVersion);

		return false;
	}
	else if (skse->skseVersion < MAKE_EXE_VERSION(1,6,16))
	{
		_ERROR("unsupported skse version %08X", skse->skseVersion);

		return false;		
	}
	else if (SKSE_VERSION_RELEASEIDX < 53)
	{
		_ERROR("unsupported skse release index %08X", SKSE_VERSION_RELEASEIDX);

		return false;		
	}

	// get the scaleform interface and query its version
	g_scaleform = (SKSEScaleformInterface *)skse->QueryInterface(kInterface_Scaleform);
	if(!g_scaleform)
	{
		_ERROR("couldn't get scaleform interface");
		return false;
	}

	if(g_scaleform->interfaceVersion < SKSEScaleformInterface::kInterfaceVersion)
	{
		_ERROR("scaleform interface too old (%d expected %d)", g_scaleform->interfaceVersion, SKSEScaleformInterface::kInterfaceVersion);
		return false;
	}

	// ### do not do anything else in this callback
	// ### only fill out PluginInfo and return true/false



	g_skseMessaging = (SKSEMessagingInterface *)skse->QueryInterface(kInterface_Messaging);
	if (!g_skseMessaging)
	{
		_ERROR("couldn't get messaging interface");
		return false;
	}

	// supported runtime version
	return true;
}

bool SKSEPlugin_Load(const SKSEInterface * skse)
{
	// register scaleform callbacks
	g_scaleform->Register("AHZmoreHUDPlugin", RegisterScaleform);
	g_scaleform->RegisterForInventory(AhzInventoryData);

	EventDispatcher<SKSECrosshairRefEvent> * dispatcher = (EventDispatcher<SKSECrosshairRefEvent> *)g_skseMessaging->GetEventDispatcher(SKSEMessagingInterface::kDispatcher_CrosshairEvent);
	dispatcher->AddEventSink(&crossHairEvent);

	return true;
}

};

