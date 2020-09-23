#pragma once
#include "AHZActorInfo.h"

void AHZInstallEnemyHealthUpdateHook();
CAHZActorData GetCurrentEnemyData();
#include <mutex>

static std::recursive_mutex mtx;

class SafeEnemyDataHolder
{
public:
   SafeEnemyDataHolder() = default;
    ~SafeEnemyDataHolder() = default;

   CAHZActorData GetData()
   {
       std::lock_guard<recursive_mutex> lock(mtx);
	   CAHZActorData data;
	   data = m_data;
	   return data;
   };

   void SetData(const CAHZActorData &data)
   {
       std::lock_guard<recursive_mutex> lock(mtx);
	   m_data = data;
   };

   private:
		CAHZActorData m_data;
};