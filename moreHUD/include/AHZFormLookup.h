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
   RE::TESForm * GetTESForm(RE::TESObjectREFR * targetReference);
   void AddScriptVarable(string vmVariableName);
   void                   AddFormID(string baseFormModName, uint32_t baseFormID, string targetFormModName, uint32_t targetFormID);

   inline static RE::TESObjectREFR* GetReference(RE::TESForm* theForm)
   {
      if (theForm->GetFormType() == RE::FormType::Reference)
      {
         auto reference = theForm->As<RE::TESObjectREFR>();
         return reference;
      }
      else
      {
         return nullptr;
      }
   }

private:
   CAHZFormLookup();
   ~CAHZFormLookup();
   RE::TESForm * GetAttachedForm(RE::TESObjectREFR *form);
   RE::TESForm*    GetFormFromLookup(RE::TESObjectREFR* targetRef);
   static RE::TESForm* GetAttachedForm(RE::TESObjectREFR* form, string variableName);
   CAHZFormLookup(CAHZFormLookup const&);      // copy ctor is hidden
   CAHZFormLookup& operator=(CAHZFormLookup const&) {}; // assign op is hidden
   std::vector<string> m_scriptVMVariables;
   std::map<uint32_t, uint32_t> m_LUT;
   std::map<string, uint32_t>   m_modIndexLUT;
};

#define AHZGetForm(x) (CAHZFormLookup::Instance().GetTESForm((x)))
#define AHZGetReference(x) (CAHZFormLookup::Instance().GetReference((x)))