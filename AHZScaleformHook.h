#pragma once
#include <string>
#include "skse64/PapyrusEvents.h"
#include "skse64/GameMenus.h"

void InstallAHZHudComponents(GFxMovieView * view);
using namespace std;

extern TESObjectREFR *ahzTargetReference;
extern EnemyHealth *ahzrealEnemyHud;

class AHZEventHandler : public BSTEventSink <MenuOpenCloseEvent> {

	EventResult ReceiveEvent(MenuOpenCloseEvent * evn, EventDispatcher<MenuOpenCloseEvent> * dispatcher);
};

class AHZCrosshairRefEventHandler : public BSTEventSink <SKSECrosshairRefEvent>
{
	EventResult ReceiveEvent(SKSECrosshairRefEvent * evn, EventDispatcher<SKSECrosshairRefEvent> * dispatcher);
};