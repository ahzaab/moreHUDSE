#pragma once

#include <wchar.h>
#include <stdlib.h>
#include <list>
#include <algorithm>
#include "skse64/GameReferences.h"
#include "skse64/GameObjects.h"
#include "skse64/GameData.h"
#include "skse64/GameRTTI.h"
#include "skse64/GameSettings.h"
#include "skse64/GameExtraData.h"
#include "skse64/ScaleformCallbacks.h"
#include "skse64/ScaleformMovie.h"
#include "skse64/PapyrusIngredient.h"
#include "skse64/PapyrusSpell.h"
#include "skse64/PapyrusArgs.h"
#include "skse64/PapyrusVM.h"
#include "AHZArmorInfo.h"
#include "AHZPlayerInfo.h"
#include "AHZWeaponInfo.h"
#include <iostream>
#include <fstream>
#include <regex>
#include "AHZConsole.h"
#include "string.h"
using namespace std;

class CAHZFormLookup
{
public:
   static CAHZFormLookup& Instance();
   TESForm * GetTESForm(TESObjectREFR * targetReference);
   void AddScriptVarable(string vmVariableName);
   void AddFormID(string baseFormModName, UInt32 baseFormID, string targetFormModName, UInt32 targetFormID);

   inline static TESObjectREFR * GetReference(TESForm * theForm)
   {
      if (theForm->GetFormType() == kFormType_Reference)
      {
         TESObjectREFR *reference = DYNAMIC_CAST(theForm, TESForm, TESObjectREFR);
         return reference;
      }
      else
      {
         return NULL;
      }
   }

private:
   CAHZFormLookup();
   ~CAHZFormLookup();
   TESForm * GetAttachedForm(TESObjectREFR *form);
   TESForm * CAHZFormLookup::GetFormFromLookup(TESObjectREFR * targetRef);
   static TESForm * GetAttachedForm(TESObjectREFR *form, string variableName);
   CAHZFormLookup(CAHZFormLookup const&);      // copy ctor is hidden
   CAHZFormLookup& operator=(CAHZFormLookup const&) {}; // assign op is hidden
   std::vector<string> m_scriptVMVariables;
   std::map<UInt32, UInt32> m_LUT;
   std::map<string, UInt32> m_modIndexLUT;
};

#define AHZGetForm(x) (CAHZFormLookup::Instance().GetTESForm((x)))
#define AHZGetReference(x) (CAHZFormLookup::Instance().GetReference((x)))