#pragma once
#include "AHZActorInfo.h"

void AHZInstallEnemyHealthUpdateHook();
CAHZActorData GetCurrentEnemyData();



class SafeEnemyDataHolder: public RE::SafeDataHolder<CAHZActorData>
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