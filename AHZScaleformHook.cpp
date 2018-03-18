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
EnemyHealth *ahzrealEnemyHud = NULL;

EventResult AHZEventHandler::ReceiveEvent(MenuOpenCloseEvent * evn, EventDispatcher<MenuOpenCloseEvent> * dispatcher)
{
	string menuName(evn->menuName.data);

	if ((ahzMenuLoaded == false) && (menuName == "HUD Menu") && (evn->opening))
	{
		GFxMovieView *view = MenuManager::GetSingleton()->GetMovieView(&evn->menuName);

		if (view)
		{
			InstallAHZHudComponents(view);

         UIStringHolder	* stringHolder = UIStringHolder::GetSingleton();
         HUDMenu *hudMenu = static_cast<HUDMenu*>(MenuManager::GetSingleton()->GetMenu(&stringHolder->hudMenu));
         if (hudMenu)
         {
            for (int i = 0; i < hudMenu->hudComponents.count; i++)
            {
               HUDObject *componant;
               hudMenu->hudComponents.GetNthItem(i, componant);
               ahzrealEnemyHud = dynamic_cast<EnemyHealth*>(componant);
               if (ahzrealEnemyHud)
               {
                  break;
               }
            }
         }

         if (!ahzrealEnemyHud)
         {
            _ERROR("Could not find the enemy hud component!.  The enemy level feature will not work");
         }

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

	args[0].SetString("AHZHudInfo.swf");
	hudComponent.Invoke("loadMovie", &result, &args[0], 1);
}

