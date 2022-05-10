#include "PCH.h"
#include "AHZScaleformHook.h"
#include <string>
#include <mutex>
#include "SKSE/Trampoline.h"
#include <Xbyak/xbyak.h>

// 1408B2880 1.6.318
constexpr REL::ID   EnemyUpdateHookBase(static_cast<std::uint64_t>(51671));
uintptr_t           EnemyUpdateHook = (EnemyUpdateHookBase.address() + 0x44);
SafeEnemyDataHolder AHZEnemyHealthUpdateHook::ahzEnemyData;
RE::RefHandle       AHZEnemyHealthUpdateHook::lastRefHandle = 0;
RE::BGSKeyword* AHZEnemyHealthUpdateHook::NoSoulTrapRace = nullptr;

bool AHZEnemyHealthUpdateHook::Hook_EnemyHealthLookupReferenceByHandle_impl(const RE::RefHandle& refHandle, RE::NiPointer<RE::TESObjectREFR>& refrOut)
{
    auto result = RE::LookupReferenceByHandle(refHandle, refrOut);
    RE::TESObjectREFR* reference = refrOut.get();
    if (!reference) {
        return result;
    }

    bool     targetChanged = (lastRefHandle != refHandle);
    uint16_t npcLevel = 0;
    uint32_t isSentient = 0;
    float    maxHealth = 0;
    float    health = 0;
    float    maxMagicka = 0;
    float    magicka = 0;
    float    maxStamina = 0;
    float    stamina = 0;
    lastRefHandle = refHandle;

    if (reference) {
        if (reference->GetBaseObject()->formType == RE::FormType::NPC ||
            reference->GetBaseObject()->formType == RE::FormType::ActorCharacter) {
            auto pNPC = reference->As<RE::Actor>();
            if (pNPC) {
                npcLevel = pNPC->GetLevel();
                if (NoSoulTrapRace == nullptr)
                {
                    NoSoulTrapRace = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("NoSoulTrap");
                }
                auto noSoulTrapRace = NoSoulTrapRace && pNPC->GetRace()->HasKeyword(NoSoulTrapRace);
                isSentient = CAHZActorInfo::IsSentient(pNPC) || noSoulTrapRace;
                maxHealth = pNPC->GetPermanentActorValue(RE::ActorValue::kHealth);
                health = pNPC->GetActorValue(RE::ActorValue::kHealth);
                maxMagicka = pNPC->GetPermanentActorValue(RE::ActorValue::kMagicka);
                magicka = pNPC->GetActorValue(RE::ActorValue::kMagicka);
                maxStamina = pNPC->GetPermanentActorValue(RE::ActorValue::kStamina);
                stamina = pNPC->GetActorValue(RE::ActorValue::kStamina);
            }
        }
    }


    CAHZActorData data;
    data.targetChanged = targetChanged;
    data.Level = npcLevel;
    data.IsSentient = isSentient;
    data.maxHealth = maxHealth;
    data.health = health;
    data.maxMagicka = maxMagicka;
    data.magicka = magicka;
    data.maxStamina = maxStamina;
    data.stamina = stamina;
    ahzEnemyData.SetData(data);
    return result;
}

void AHZEnemyHealthUpdateHook::Install()
{
    auto& trampoline = SKSE::GetTrampoline();
    trampoline.write_call<5>(
        SKSE::stl::unrestricted_cast<std::uintptr_t>(EnemyUpdateHook),
        SKSE::stl::unrestricted_cast<std::uintptr_t>(AHZEnemyHealthUpdateHook::Hook_EnemyHealthLookupReferenceByHandle_impl));
}

auto AHZEnemyHealthUpdateHook::GetCurrentEnemyData() -> CAHZActorData
{
    return ahzEnemyData.GetData();
}
