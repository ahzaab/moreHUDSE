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


class SafeEnemyDataHolder: public SafeDataHolder<CAHZActorData>
{
public:
   SafeEnemyDataHolder() {};
   ~SafeEnemyDataHolder() {};

   CAHZActorData GetData()
   {
	   CAHZActorData data;
	   Lock();
	   data = m_data;
	   Release();
	   return data;
   };

   void SetData(const CAHZActorData &data)
   {
	   Lock();
	   m_data = data;
	   Release();
   };
};