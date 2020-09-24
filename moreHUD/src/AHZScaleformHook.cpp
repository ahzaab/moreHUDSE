#include "PCH.h"
#include "AHZScaleformHook.h"
#include <string>
#include <mutex>

using namespace std;


constexpr REL::ID   EnemyUpdateHookBase(static_cast<std::uint64_t>(50776));
uintptr_t           EnemyUpdateHook = (EnemyUpdateHookBase.address() + 0x44);
SafeEnemyDataHolder AHZEnemyHealthUpdateHook::ahzEnemyData;
uint64_t            AHZEnemyHealthUpdateHook::lastRefHandle = 0;

void AHZEnemyHealthUpdateHook::EnemyHealthUpdateHook_impl(uint32_t* refHandle, RE::NiPointer<RE::TESObjectREFR>* refrOut)
{
    RE::TESObjectREFR* reference = refrOut->get();
    if (!reference) {
        return;
    }

    bool     targetChanged = (lastRefHandle != reinterpret_cast<uint64_t>(refHandle));
    uint16_t npcLevel = 0;
    uint32_t isSentient = 0;
    float    maxHealth = 0;
    float    health = 0;
    float    maxMagicka = 0;
    float    magicka = 0;
    float    maxStamina = 0;
    float    stamina = 0;
    lastRefHandle = reinterpret_cast<uint64_t>(refHandle);

    if (reference) {
        if (reference->GetBaseObject()->formType == RE::FormType::NPC ||
            reference->GetBaseObject()->formType == RE::FormType::ActorCharacter) {
            auto pNPC = DYNAMIC_CAST(reference, RE::TESObjectREFR, RE::Actor);
            if (pNPC) {
                npcLevel = pNPC->GetLevel();
                isSentient = CAHZActorInfo::IsSentient(pNPC);
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
}

    void AHZEnemyHealthUpdateHook::Install()
{
    struct Code : Xbyak::CodeGenerator
    {
        Code(std::uintptr_t funcAddr)
        {
            Xbyak::Label retnLabel;
            Xbyak::Label funcLabel;
            Xbyak::Label LookupREFRByHandleLbl;

            // Call original code and use the same signature as LookupREFRByHandle to pass the object ref to the hooked
            // function
            call(ptr[rip + LookupREFRByHandleLbl]);

            // Call our function (Same signature as LookupREFRByHandle)
            call(ptr[rip + funcLabel]);

            // Jump back from the Trampoline
            jmp(ptr[rip + retnLabel]);

            L(funcLabel);
            dq(funcAddr);

            L(LookupREFRByHandleLbl);
            dq(RE::Offset::LookupReferenceByHandle.address());

            L(retnLabel);
            dq(EnemyUpdateHook + 5);
        }
    };

    Code code(EnemyUpdateHook);
    code.ready();

    auto& trampoline = SKSE::GetTrampoline();
    trampoline.write_branch<5>(
        reinterpret_cast<std::uintptr_t>(EnemyHealthUpdateHook_impl),
        trampoline.allocate(code));
}

CAHZActorData AHZEnemyHealthUpdateHook::GetCurrentEnemyData()
{
    return ahzEnemyData.GetData();
}