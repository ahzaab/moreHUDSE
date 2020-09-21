#include "skse64/GameTypes.h"
#include "skse64/GameMenus.h"
#include "skse64/ScaleFormTypes.h"
#include "skse64/ScaleFormCallbacks.h"
#include "skse64/GameRTTI.h"
#include "skse64/ScaleFormMovie.h"
#include "skse64/skse64_common/SafeWrite.h"
#include "skse64/xbyak/xbyak.h"
#include "AHZScaleformHook.h"
#include "skse64/GameMenus.h"
#include "skse64/GameData.h"
#include "skse64/PapyrusEvents.h"
#include <string>
#include "xbyak/xbyak.h"
#include "skse64/skse64_common/BranchTrampoline.h"
#include <mutex>

using namespace std;
static bool ahzMenuLoaded = false;
static SafeEnemyDataHolder ahzEnemyData;
static std::recursive_mutex enemyLock;

EventResult AHZEventHandler::ReceiveEvent(MenuOpenCloseEvent * evn, EventDispatcher<MenuOpenCloseEvent> * dispatcher)
{
    string menuName(evn->menuName.data);

    if ((ahzMenuLoaded == false) && (menuName == "HUD Menu") && (evn->opening))
    {
        GFxMovieView *view = MenuManager::GetSingleton()->GetMovieView(&evn->menuName);

        if (view)
        {
            GFxValue hudComponent;
            GFxValue result;
            GFxValue args[2];

            if (!view)
            {
                _ERROR("The IMenu returned NULL. The moreHUD widgets will not be loaded.");
            }

            args[0].SetString("AHZWidgetContainer");
            view->Invoke("_root.getNextHighestDepth", &args[1], NULL, 0);
            view->Invoke("_root.createEmptyMovieClip", &hudComponent, args, 2);

            if (!hudComponent.objectInterface)
            {
                _ERROR("moreHUD could not create an empty movie clip for the HUDMenu. The moreHUD widgets will not be loaded.");
                return EventResult::kEvent_Abort;
            }

            args[0].SetString("AHZHudInfo.swf");
            hudComponent.Invoke("loadMovie", &result, &args[0], 1);
            ahzMenuLoaded = true;
            return EventResult::kEvent_Abort;
        }
    }

    return EventResult::kEvent_Continue;
}

RE::EventResult AHZCrosshairRefEventHandler::ReceiveEvent(SKSECrosshairRefEvent * evn, EventDispatcher<SKSECrosshairRefEvent> * dispatcher)
{
    g_ahzTargetReference = evn->crosshairRef;
    return EventResult::kEvent_Continue;
}

//Unpacked
//HxD Raw 49 8B CA E8 48 6F 64 00 48 8D 4D F0 E8 0F 6D 65 00 48 8D 4D 30 E8 06 6D 65 00 66 44 89 65 70 4C    8814A0
//HxD Raw 49 8B CA E8 E8 6F 64 00 48 8D 4D F0 E8 AF 6D 65 00 48 8D 4D 30 E8 A6 6D 65 00 66 44 89 65 70 4C -->1.5.39
//CFF Explorer .text  53 20 48 8B 4B 10 E8 B5 67 64 00 84 C0 0F 84 F4 00 00 00 8B 43 18 24 8F 3C 08 0F 94 C0 88 44 24
//ida 48 8B C4 55 56 57 41 54 41 55 41 56 41 57 48 8D A8 18 FF FF FF 48 81 EC B0 01 00 00 48 C7 45 A8
uintptr_t Enemy_Update_Hook_Base = 0x00882520;

static UInt64 lastRefHandle = 0;
RelocAddr<uintptr_t> Enemy_Update_Hook_Target(Enemy_Update_Hook_Base + 0x44);
void EnemyHealth_Update_Hook(UInt32 * refHandle, NiPointer<TESObjectREFR> *refrOut)
{

    TESObjectREFR * reference = *refrOut;
    if (!reference)
    {
        return;
    }

    bool targetChanged = (lastRefHandle != (UInt64)refHandle);
    UInt16 npcLevel = 0;
    UInt32 isSentient = 0;
    float maxHealth = 0;
    float health = 0;
    float maxMagicka = 0;
    float magicka = 0;
    float maxStamina = 0;
    float stamina = 0;
    lastRefHandle = (UInt64)refHandle;

    if (reference)
    {
        if (reference->baseForm->formType == kFormType_NPC ||
            reference->baseForm->formType == kFormType_Character)
        {
            Actor * pNPC = DYNAMIC_CAST(reference, TESObjectREFR, Actor);
            if (pNPC)
            {
                npcLevel = CALL_MEMBER_FN(pNPC, GetLevel)();
                isSentient = CAHZActorInfo::IsSentient(pNPC);
                UInt32 actorValue = LookupActorValueByName("health");
                maxHealth = pNPC->actorValueOwner.GetMaximum(actorValue);
                health = pNPC->actorValueOwner.GetCurrent(actorValue);
                actorValue = LookupActorValueByName("magicka");
                maxMagicka = pNPC->actorValueOwner.GetMaximum(actorValue);
                magicka = pNPC->actorValueOwner.GetCurrent(actorValue);
                actorValue = LookupActorValueByName("stamina");
                maxStamina = pNPC->actorValueOwner.GetMaximum(actorValue);
                stamina = pNPC->actorValueOwner.GetCurrent(actorValue);
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
        HUDMenu_RegisterExtension_Code(void * buf, UInt64 funcAddr) : Xbyak::CodeGenerator(4096, buf)
        {
            Xbyak::Label retnLabel;
            Xbyak::Label funcLabel;

            // Call original code and use the same signature as LookupREFRByHandle to pass the object ref to the hooked
            // function
            call(LookupREFRByHandle);

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