#pragma once
#include "skse64/PapyrusEvents.h"
#include "AHZActorInfo.h"

void AHZInstallEnemyHealthUpdateHook();
CAHZActorData GetCurrentEnemyData();
extern TESObjectREFR *g_ahzTargetReference;

class AHZEventHandler : public BSTEventSink <MenuOpenCloseEvent> {

   EventResult ReceiveEvent(MenuOpenCloseEvent * evn, EventDispatcher<MenuOpenCloseEvent> * dispatcher);
};

class AHZCrosshairRefEventHandler : public BSTEventSink <SKSECrosshairRefEvent>
{
   EventResult ReceiveEvent(SKSECrosshairRefEvent * evn, EventDispatcher<SKSECrosshairRefEvent> * dispatcher);
};


class SafeEnemyLevelDataHolder: public SafeDataHolder<CAHZActorData>
{
public:
   SafeEnemyLevelDataHolder() {};
   ~SafeEnemyLevelDataHolder() {};
};