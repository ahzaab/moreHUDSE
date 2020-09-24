#include "PCH.h"
#include "AHZScaleformHook.h"
#include <string>
#include <mutex>

using namespace std;
static SafeEnemyDataHolder ahzEnemyData;
static std::recursive_mutex enemyLock;


//Unpacked
//HxD Raw 49 8B CA E8 48 6F 64 00 48 8D 4D F0 E8 0F 6D 65 00 48 8D 4D 30 E8 06 6D 65 00 66 44 89 65 70 4C    8814A0
//HxD Raw 49 8B CA E8 E8 6F 64 00 48 8D 4D F0 E8 AF 6D 65 00 48 8D 4D 30 E8 A6 6D 65 00 66 44 89 65 70 4C -->1.5.39
//CFF Explorer .text  53 20 48 8B 4B 10 E8 B5 67 64 00 84 C0 0F 84 F4 00 00 00 8B 43 18 24 8F 3C 08 0F 94 C0 88 44 24
//ida 48 8B C4 55 56 57 41 54 41 55 41 56 41 57 48 8D A8 18 FF FF FF 48 81 EC B0 01 00 00 48 C7 45 A8
uintptr_t Enemy_Update_Hook_Base = 0x00882520;

static uint64_t lastRefHandle = 0;
REL::RelocAddr<uintptr_t> Enemy_Update_Hook_Target(Enemy_Update_Hook_Base + 0x44);
void EnemyHealth_Update_Hook(uint32_t * refHandle, RE::NiPointer<RE::TESObjectREFR> *refrOut)
{

    RE::TESObjectREFR * reference = refrOut->get();
    if (!reference)
    {
        return;
    }

    bool targetChanged = (lastRefHandle != reinterpret_cast<uint64_t>(refHandle));
    uint16_t npcLevel = 0;
    uint32_t isSentient = 0;
    float maxHealth = 0;
    float health = 0;
    float maxMagicka = 0;
    float magicka = 0;
    float maxStamina = 0;
    float stamina = 0;
    lastRefHandle = reinterpret_cast<uint64_t>(refHandle);

    if (reference)
    {
        if (reference->GetBaseObject()->formType == RE::FormType::NPC ||
            reference->GetBaseObject()->formType == RE::FormType::ActorCharacter)
        {
            auto pNPC = DYNAMIC_CAST(reference, RE::TESObjectREFR, RE::Actor);
            if (pNPC)
            {
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


CAHZActorData GetCurrentEnemyData()
{
    return ahzEnemyData.GetData();
}

void AHZInstallEnemyHealthUpdateHook()
{
    struct HUDMenu_RegisterExtension_Code : Xbyak::CodeGenerator {
        HUDMenu_RegisterExtension_Code(void * buf, uint64_t funcAddr) : Xbyak::CodeGenerator(4096, buf)
        {
            Xbyak::Label retnLabel;
            Xbyak::Label funcLabel;

            static_cast<bool RE::LookupReferenceByHandle

            // Call original code and use the same signature as LookupREFRByHandle to pass the object ref to the hooked
            // function
            call(RE::LookupReferenceByHandle);

            // Call our function (Same signature as LookupREFRByHandle)
            call(ptr[rip + funcLabel]);

            // Jump back from the Trampoline
            jmp(ptr[rip + retnLabel]);

            L(funcLabel);
            dq(funcAddr);

            L(retnLabel);
            dq(Enemy_Update_Hook_Target.GetUIntPtr() + 5);
        }
    };

    void * codeBuf = g_localTrampoline.StartAlloc();
    HUDMenu_RegisterExtension_Code code(codeBuf, (uintptr_t)EnemyHealth_Update_Hook);
    g_localTrampoline.EndAlloc(code.getCurr());

    g_branchTrampoline.Write5Branch(Enemy_Update_Hook_Target.GetUIntPtr(), uintptr_t(code.getCode()));
}