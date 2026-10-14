#pragma once
#include "AHZActorInfo.h"
#include <mutex>
#include <atomic>

class SafeEnemyDataHolder
{
public:
    SafeEnemyDataHolder() = default;
    ~SafeEnemyDataHolder() = default;

    CAHZActorData GetData()
    {
        std::lock_guard<std::recursive_mutex> lock(m_mutex);
        CAHZActorData                         data;
        data = m_data;
        return data;
    };

    void SetData(const CAHZActorData& data)
    {
        std::lock_guard<std::recursive_mutex> lock(m_mutex);
        m_data = data;
    };

private:
    CAHZActorData m_data;
    std::recursive_mutex m_mutex;
};

class AHZEnemyHealthUpdateHook
{
public:
    static void          Install();
    static CAHZActorData GetCurrentEnemyData();

private:
    static SafeEnemyDataHolder ahzEnemyData;
    static std::atomic<RE::RefHandle> lastRefHandle;
    static bool                Hook_EnemyHealthLookupReferenceByHandle_impl(const RE::RefHandle& refHandle, RE::NiPointer<RE::TESObjectREFR>& refrOut);
};
