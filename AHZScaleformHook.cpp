#include "skse64/GameTypes.h"
#include "skse64/GameMenus.h"
#include "skse64/ScaleFormTypes.h"
#include "skse64/ScaleFormCallbacks.h"
#include "skse64/ScaleFormMovie.h"
#include "skse64/skse64_common/SafeWrite.h"
#include "skse64/xbyak/xbyak.h"
#include "AHZScaleformHook.h"
#include "skse64/GameMenus.h"
#include "skse64/PapyrusEvents.h"
#include <string>

using namespace std;
static bool ahzMenuLoaded = false;
TESObjectREFR *ahzTargetReference = NULL;

EventResult AHZEventHandler::ReceiveEvent(MenuOpenCloseEvent * evn, EventDispatcher<MenuOpenCloseEvent> * dispatcher)
{
	string menuName(evn->menuName.data);

	if ((ahzMenuLoaded == false) && (menuName == "HUD Menu") && (evn->opening))
	{
		GFxMovieView *view = MenuManager::GetSingleton()->GetMovieView(&evn->menuName);

		if (view)
		{
			InstallAHZHudComponents(view);
			ahzMenuLoaded = true;
			return EventResult::kEvent_Abort;
		}
	}

	return EventResult::kEvent_Continue;
}

EventResult AHZCrosshairRefEventHandler::ReceiveEvent(SKSECrosshairRefEvent * evn, EventDispatcher<SKSECrosshairRefEvent> * dispatcher)
{
	ahzTargetReference = evn->crosshairRef;
	return EventResult::kEvent_Continue;
}


void InstallAHZHudComponents(GFxMovieView * view)
{
	GFxValue hudComponent;
	GFxValue result;
	GFxValue args[2];

	if (!view)
	{
		_ERROR("The IMenu returned NULL. The moreHUD widgets will not be loaded.");
	}

	args[0].SetString("AHZWidgetContainer");
	view->Invoke("_root.getNextHighestDepth", &args[1], NULL, 0);
	view->Invoke("_root.createEmptyMovieClip", &hudComponent, args, 2);
	
	if (!hudComponent.objectInterface)
	{
		_ERROR("moreHUD could not create an empty movie clip for the HUDMenu. The moreHUD widgets will not be loaded.");
		return;
	}

	//BSFixedString bs1("Interface/HUDMenu.swf");
	//BSFixedString bs2("Interface/exported/HUDMenu.gfx");
	//BSFixedString bs1("HUDMenu.swf");
	// Determine which file is loaded for the hud menu
	//if (MenuManager::GetSingleton()->GetMovieView(&bs1))
	//{

	//GFxValue result;

	//while (!IsDebuggerPresent())
	//{
	//	Sleep(10);
	//}

	//Sleep(1000 * 2);

		args[0].SetString("AHZHudInfo.swf");
		hudComponent.Invoke("loadMovie", &result, &args[0], 1);
		
		//args[0].SetString("exported/AHZHudInfo.swf");
		//hudComponent.Invoke("loadMovie", &result, &args[0], 1);

		//args[0].SetString("Interface/exported/AHZHudInfo.swf");
		//hudComponent.Invoke("loadMovie", &result, &args[0], 1);
	
		
	//}
	//else if (MenuManager::GetSingleton()->GetMovieView(&bs2))
	//{
	//	args[0].SetString("AHZHudInfo.swf");
	//	hudComponent.Invoke("loadMovie", NULL, &args[0], 1);
	//}
	//else
	//{
	//	_ERROR("No valid HUDMenu movie clip found.  The moreHUD widgets will not be loaded.");
	//}
}

