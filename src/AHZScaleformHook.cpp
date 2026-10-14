#include "pch.h"
#include "AHZScaleformHook.h"
#include <string>
#include <mutex>
#include "SKSE/Trampoline.h"
#include <Xbyak/xbyak.h>
#include "Offsets-VR.h"
#include "Offsets-SE.h"
#include "Offsets-AE.h"

#define HOOK_OFFSET (0x44)

// 1408B2880 1.6.318
// constexpr REL::ID   EnemyUpdateHookBase(static_cast<std::uint64_t>(51671));
uintptr_t           EnemyUpdateHook = 0;
SafeEnemyDataHolder AHZEnemyHealthUpdateHook::ahzEnemyData;
std::atomic<RE::RefHandle> AHZEnemyHealthUpdateHook::lastRefHandle{ 0 };

bool AHZEnemyHealthUpdateHook::Hook_EnemyHealthLookupReferenceByHandle_impl(const RE::RefHandle& refHandle, RE::NiPointer<RE::TESObjectREFR>& refrOut)
{
    const auto result = RE::LookupReferenceByHandle(refHandle, refrOut);
    CAHZActorData data;
    data.targetChanged = lastRefHandle.exchange(refHandle, std::memory_order_relaxed) != refHandle;
    // The enemy hook has its own owning lookup result. Never replay the crosshair target here.
    const auto actor = result && refrOut ? refrOut->As<RE::Actor>() : nullptr;
    const auto player = RE::PlayerCharacter::GetSingleton();
    if (actor && player) {
        const auto values = actor->AsActorValueOwner();
        const auto race = actor->GetRace();
        const auto noSoulTrapKeyword = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NoSoulTrap");
        data.Level = actor->GetLevel();
        data.playerLevel = player->GetLevel();
        data.IsSentient = CAHZActorInfo::IsSentient(actor) != 0 ||
            (race && noSoulTrapKeyword && race->HasKeyword(noSoulTrapKeyword));
        if (values) {
            data.maxHealth = values->GetPermanentActorValue(RE::ActorValue::kHealth);
            data.health = values->GetActorValue(RE::ActorValue::kHealth);
            data.maxMagicka = values->GetPermanentActorValue(RE::ActorValue::kMagicka);
            data.magicka = values->GetActorValue(RE::ActorValue::kMagicka);
            data.maxStamina = values->GetPermanentActorValue(RE::ActorValue::kStamina);
            data.stamina = values->GetActorValue(RE::ActorValue::kStamina);
        }
        if (data.Level && !data.IsSentient) {
            const auto soul = CAHZActorInfo::GetSoulType(data.Level, 0);
            constexpr const char* soulSettings[] = { "", "sSoulLevelNamePetty", "sSoulLevelNameLesser",
                "sSoulLevelNameCommon", "sSoulLevelNameGreater", "sSoulLevelNameGrand" };
            const auto settings = RE::GameSettingCollection::GetSingleton();
            if (settings && soul > 0 && soul < std::size(soulSettings)) {
                const auto setting = settings->GetSetting(soulSettings[soul]);
                const auto name = setting ? setting->GetString() : nullptr;
                if (name) { data.soulName = name; }
            }
        }
    }
    ahzEnemyData.SetData(data);
    return result;
}

void AHZEnemyHealthUpdateHook::Install()
{
    auto& trampoline = SKSE::GetTrampoline();

    const auto base = REL::Module::IsSE() ? moreHUDSE::SE::Offsets::EnemyUpdateHookBase.address() :
        REL::Module::IsVR() ? moreHUDSE::VR::Offsets::EnemyUpdateHookBase.address() :
                             moreHUDSE::AE::Offsets::EnemyUpdateHookBase.address();
    if (!base) {
        logger::error("Cannot install enemy lookup hook: relocation is unavailable"sv);
        return;
    }
    EnemyUpdateHook = base + HOOK_OFFSET;
    trampoline.write_call<5>(
        SKSE::stl::unrestricted_cast<std::uintptr_t>(EnemyUpdateHook),
        SKSE::stl::unrestricted_cast<std::uintptr_t>(AHZEnemyHealthUpdateHook::Hook_EnemyHealthLookupReferenceByHandle_impl));
}

auto AHZEnemyHealthUpdateHook::GetCurrentEnemyData() -> CAHZActorData
{
    return ahzEnemyData.GetData();
}
