#pragma once
#include <string>
#include "skse64/PapyrusEvents.h"

void InstallAHZHudComponents(GFxMovieView * view);
using namespace std;

extern TESObjectREFR *ahzTargetReference;

class AHZEventHandler : public BSTEventSink <MenuOpenCloseEvent> {

	EventResult ReceiveEvent(MenuOpenCloseEvent * evn, EventDispatcher<MenuOpenCloseEvent> * dispatcher);
};

class AHZCrosshairRefEventHandler : public BSTEventSink <SKSECrosshairRefEvent>
{
	EventResult ReceiveEvent(SKSECrosshairRefEvent * evn, EventDispatcher<SKSECrosshairRefEvent> * dispatcher);
};