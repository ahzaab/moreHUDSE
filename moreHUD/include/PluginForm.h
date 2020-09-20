#pragma once
#include "skse/PluginAPI.h"
#include "skse/skse_version.h"
#include "skse/SafeWrite.h"
#include "skse/ScaleformCallbacks.h"
#include "skse/ScaleformMovie.h"
#include "skse/GameAPI.h"
#include "skse/GameObjects.h"
#include "skse/PapyrusNativeFunctions.h"
#include "skse/Hooks_Papyrus.h"
#include "skse/GameTypes.h"
#include "skse/GameReferences.h"
#include "skse/GameFormComponents.h"
#include "skse/GameForms.h"
#include "skse/GameRTTI.h"
#include "skse/GameMenus.h"
#include "skse/PapyrusUI.h"
#include "skse/PapyrusEvents.h"

class PluginForm :
	public TESForm
{
public:


	PluginForm(void)
	{
	}

	~PluginForm(void)
	{
	}

	void OnCrosshairRefChange(TESObjectREFR* pObjRef)
	{
		DisplayHUDInformation(pObjRef);
	}



};

