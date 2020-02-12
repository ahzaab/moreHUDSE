#include "skse64/PluginAPI.h"
#include "skse64/skse64_common/skse_version.h"
#include "skse64/skse64_common/SafeWrite.h"
#include "skse64/skse64_common/BranchTrampoline.h"
#include "skse64/ScaleformCallbacks.h"
#include "skse64/ScaleformMovie.h"
#include "skse64/GameAPI.h"
#include "skse64/GameData.h"
#include "skse64/GameObjects.h"
#include "skse64/PapyrusNativeFunctions.h"
#include "skse64/Hooks_Papyrus.h"
#include "skse64/GameTypes.h"
#include "skse64/GameSettings.h"
#include "skse64/GameReferences.h"
#include "skse64/GameFormComponents.h"
#include "skse64/GameForms.h"
#include "skse64/GameRTTI.h"
#include "skse64/GameMenus.h"
#include "skse64/PapyrusUI.h"
#include "skse64/PapyrusVM.h"
#include "skse64/PapyrusEvents.h"
#include "skse64/GameExtraData.h"
#include "skse64/PapyrusUtility.h"
#include "AHZPlayerInfo.h"
#include "AHZScaleform.h"
#include "AHZFormLookup.h"
#include "AHZScaleformHook.h"
#include "../common/IErrors.h"
#include "skse64/PapyrusIngredient.h"
#include "AHZScaleformHook.h"
#include "skse64/GameEvents.h"
#include "skse64/GameMenus.h"
#include <shlobj.h>
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include "AHZConsole.h"
#include "AHZUtilities.h"
#include "AHZVanillaFormTable.h"
#include "AHZExternalFormTable.h"
#include "xbyak/xbyak.h"

using namespace std;

// Is Actor Sentient = 606850
// Get Soul Level = 3C1740

IDebugLog	gLog;
PluginHandle	g_pluginHandle = kPluginHandle_Invalid;
static UInt32 g_skseVersion = 0;
SKSEScaleformInterface		* g_scaleform = NULL;
SKSEMessagingInterface *g_skseMessaging = NULL;
AHZEventHandler menuEvent;
AHZCrosshairRefEventHandler crossHairEvent;
#define PLUGIN_VERSION  (30605)

// Just initialize to start routing to the console window
CAHZDebugConsole theDebugConsole;

/**** scaleform functions ****/

class SKSEScaleform_InstallHooks : public GFxFunctionHandler
{
public:
   virtual void	Invoke(Args * args)
   {
   }
};

class SKSEScaleform_GetTargetObjectData : public GFxFunctionHandler
{
public:
   virtual void	Invoke(Args * args)
   {
      CAHZScaleform::ProcessTargetObject(CAHZPlayerInfo::GetTargetRef(), args);
   }
};

class SKSEScaleform_GetPlayerData : public GFxFunctionHandler
{
public:
   virtual void	Invoke(Args * args)
   {
      CAHZScaleform::ProcessPlayerData(args);
   }
};

class SKSEScaleform_GetIsPlayerInCombat : public GFxFunctionHandler
{
public:
   virtual void	Invoke(Args * args)
   {
      args->result->SetBool(CAHZPlayerInfo::GetIsInCombat());
   }
};

class SKSEScaleform_GetTargetEffects : public GFxFunctionHandler
{
public:
   virtual void	Invoke(Args * args)
   {
      CAHZScaleform::ProcessTargetEffects(CAHZPlayerInfo::GetTargetRef(), args);
   }
};

class SKSEScaleform_GetIsBookAndWasRead : public GFxFunctionHandler
{
public:
   virtual void	Invoke(Args * args)
   {
      TESObjectREFR * pTargetReference = CAHZPlayerInfo::GetTargetRef();

      // If the target is not valid or it can't be picked up by the player
      if (!pTargetReference)
      {
         args->result->SetBool(false);
         return;
      }
      args->result->SetBool(CAHZScaleform::GetIsBookAndWasRead(pTargetReference));
   }
};

class SKSEScaleform_GetArmorWeightClassString : public GFxFunctionHandler
{
public:
   virtual void	Invoke(Args * args)
   {
      CAHZScaleform::ProcessArmorClass(CAHZPlayerInfo::GetTargetRef(), args);
   }
};

class SKSEScaleform_GetValueToWeightString : public GFxFunctionHandler
{
public:
   virtual void	Invoke(Args * args)
   {
      CAHZScaleform::ProcessValueToWeight(CAHZPlayerInfo::GetTargetRef(), args);
   }
};

class SKSEScaleform_GetTargetWarmthRating : public GFxFunctionHandler
{
public:
   virtual void	Invoke(Args * args)
   {
      args->result->SetNumber(CAHZScaleform::GetArmorWarmthRating(CAHZPlayerInfo::GetTargetRef()));
   }
};

class SKSEScaleform_AHZLog : public GFxFunctionHandler
{
public:
   virtual void	Invoke(Args * args)
   {
#if _DEBUG
      _MESSAGE("%s", args->args[0].GetString());
#endif
   }
};

class SKSEScaleform_GetBookSkillString : public GFxFunctionHandler
{
public:
   virtual void	Invoke(Args * args)
   {
      CAHZScaleform::ProcessBookSkill(CAHZPlayerInfo::GetTargetRef(), args);
   }
};

class SKSEScaleform_GetIsValidTarget : public GFxFunctionHandler
{
public:
   virtual void	Invoke(Args * args)
   {
      CAHZScaleform::ProcessValidTarget(CAHZPlayerInfo::GetTargetRef(), args);

   }
};

class SKSEScaleform_GetEnemyInformation : public GFxFunctionHandler
{
public:
   virtual void	Invoke(Args * args)
   {
      CAHZScaleform::ProcessEnemyInformation(args);
   }
};

class SKSEScaleform_IsAKnownEnchantedItem : public GFxFunctionHandler
{
public:
   virtual void	Invoke(Args * args)
   {
      TESObjectREFR * pTargetReference = CAHZPlayerInfo::GetTargetRef();

      // If the target is not valid or it can't be picked up by the player
      if (!pTargetReference)
      {
         args->result->SetNumber(0);
         return;
      }
      args->result->SetNumber(CAHZScaleform::GetIsKnownEnchantment(pTargetReference));
   }
};

bool RegisterScaleform(GFxMovieView * view, GFxValue * root)
{
   RegisterFunction <SKSEScaleform_InstallHooks>(root, view, "InstallHooks");
   RegisterFunction <SKSEScaleform_GetTargetObjectData>(root, view, "GetTargetObjectData");
   RegisterFunction <SKSEScaleform_GetPlayerData>(root, view, "GetPlayerData");
   RegisterFunction <SKSEScaleform_GetIsValidTarget>(root, view, "GetIsValidTarget");
   RegisterFunction <SKSEScaleform_GetIsPlayerInCombat>(root, view, "GetIsPlayerInCombat");
   RegisterFunction <SKSEScaleform_GetTargetEffects>(root, view, "GetTargetEffects");
   RegisterFunction <SKSEScaleform_GetIsBookAndWasRead>(root, view, "GetIsBookAndWasRead");
   RegisterFunction <SKSEScaleform_GetArmorWeightClassString>(root, view, "GetArmorWeightClassString");
   RegisterFunction <SKSEScaleform_GetBookSkillString>(root, view, "GetBookSkillString");
   RegisterFunction <SKSEScaleform_GetValueToWeightString>(root, view, "GetValueToWeightString");
   RegisterFunction <SKSEScaleform_GetTargetWarmthRating>(root, view, "GetTargetWarmthRating");
   RegisterFunction <SKSEScaleform_GetEnemyInformation>(root, view, "GetEnemyInformation");
   RegisterFunction <SKSEScaleform_IsAKnownEnchantedItem>(root, view, "IsAKnownEnchantedItem");
   

   RegisterFunction <SKSEScaleform_AHZLog>(root, view, "AHZLog");
   MenuManager::GetSingleton()->MenuOpenCloseEventDispatcher()->AddEventSink(&menuEvent);
   return true;
}

// Listens to events dispatched by SKSE
void EventListener(SKSEMessagingInterface::Message* msg)
{
   if (!msg)
   {
      return;
   }

   if (string(msg->sender) == "SKSE" && msg->type == SKSEMessagingInterface::kMessage_DataLoaded)
   {
	   // First load the built-in (Known Vanilla) ACTI Forms and VM Script Variables
	   CAHZVanillaFormTable::LoadACTIForms(CAHZFormLookup::Instance());
	   CAHZVanillaFormTable::LoadVMVariables(CAHZFormLookup::Instance());

	   // Second load any addional forms added externally
	   _MESSAGE("Processing any third party .mhud files that may exist...");

	   // Read all .mhuf files and load in the lookup tables
	   string skyrimDataPath = CAHZUtilities::GetSkyrimDataPath();

	   // Get all .mhud files from the skyrim data folder
	   vector<string> mHudFiles = CAHZUtilities::GetMHudFileList(skyrimDataPath);

	   if (!mHudFiles.size())
	   {
		   _MESSAGE("No third party .mHud files where detected.");
	   }
	   else
	   {
		   // Load the external ACTI Forms and VM Script Variables 
		   CAHZExternalFormTable::LoadACTIForms(CAHZFormLookup::Instance(), mHudFiles);
		   CAHZExternalFormTable::LoadVMVariables(CAHZFormLookup::Instance(), mHudFiles);

		   _MESSAGE("%d third party .mHud file(s) processed", mHudFiles.size());
	   }

	   _MESSAGE("Third party .mHud file processing completed.");
   }
}

extern "C"
{
   bool SKSEPlugin_Query(const SKSEInterface * skse, PluginInfo * info)
   {
      gLog.OpenRelative(CSIDL_MYDOCUMENTS, "\\My Games\\Skyrim Special Edition\\SKSE\\moreHUDSE.log");
      gLog.SetPrintLevel(IDebugLog::kLevel_VerboseMessage);
      gLog.SetLogLevel(IDebugLog::kLevel_Message);

      // populate info structure
      info->infoVersion = PluginInfo::kInfoVersion;
      info->name = "Ahzaab's moreHUD Plugin";
      info->version = PLUGIN_VERSION;

      // store plugin handle so we can identify ourselves later
      g_pluginHandle = skse->GetPluginHandle();

      if (skse->isEditor)
      {
         _ERROR("loaded in editor, marking as incompatible");

         return false;
      }
      else if (skse->runtimeVersion != (MAKE_EXE_VERSION(1, 5, 97)))
      {
         _ERROR("unsupported runtime version %08X", skse->runtimeVersion);

         return false;
      }
      else if (SKSE_VERSION_RELEASEIDX < 53)
      {
         _ERROR("unsupported skse release index %08X", SKSE_VERSION_RELEASEIDX);

         return false;
      }

      // get the scaleform interface and query its version
      g_scaleform = (SKSEScaleformInterface *)skse->QueryInterface(kInterface_Scaleform);
      if (!g_scaleform)
      {
         _ERROR("couldn't get scaleform interface");
         return false;
      }

      if (g_scaleform->interfaceVersion < SKSEScaleformInterface::kInterfaceVersion)
      {
         _ERROR("scaleform interface too old (%d expected %d)", g_scaleform->interfaceVersion, SKSEScaleformInterface::kInterfaceVersion);
         return false;
      }

      // ### do not do anything else in this callback
      // ### only fill out PluginInfo and return true/false

      g_skseMessaging = (SKSEMessagingInterface *)skse->QueryInterface(kInterface_Messaging);
      if (!g_skseMessaging)
      {
         _ERROR("couldn't get messaging interface");
         return false;
      }

      // supported runtime version
      return true;
   }

   bool SKSEPlugin_Load(const SKSEInterface * skse)
   {
	   //while (!IsDebuggerPresent())
	   //{
	   //   Sleep(10);
	   //}

	   //Sleep(1000 * 2);

      // register scaleform callbacks
      g_scaleform->Register("AHZmoreHUDPlugin", RegisterScaleform);

      EventDispatcher<SKSECrosshairRefEvent> * dispatcher = (EventDispatcher<SKSECrosshairRefEvent> *)g_skseMessaging->GetEventDispatcher(SKSEMessagingInterface::kDispatcher_CrosshairEvent);
      dispatcher->AddEventSink(&crossHairEvent);

      // Register listener for the gme loaded event
      g_skseMessaging->RegisterListener(skse->GetPluginHandle(), "SKSE", EventListener);

      if (!g_branchTrampoline.Create(1024 * 64))
      {
         _ERROR("couldn't create branch trampoline. this is fatal. skipping remainder of init process.");
         return false;
      }

      if (!g_localTrampoline.Create(1024 * 64, nullptr))
      {
         _ERROR("couldn't create codegen buffer. this is fatal. skipping remainder of init process.");
         return false;
      }

      AHZInstallEnemyHealthUpdateHook();

      _MESSAGE("%s -v%d Loaded", "AHZmoreHUDPlugin", PLUGIN_VERSION);
      return true;
   }
};