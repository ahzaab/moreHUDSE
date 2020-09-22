#pragma once

#include <wchar.h>
#include <stdlib.h>
#include <list>
#include <algorithm>
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
   auto GetScriptVariable(RE::TESForm* a_form, const char* a_scriptName, const char* a_scriptVariable) -> RE::BSScript::Variable const;
};

#define AHZGetForm(x) (CAHZFormLookup::Instance().GetTESForm((x)))
#define AHZGetReference(x) (CAHZFormLookup::Instance().GetReference((x)))

namespace RE
{
    namespace BSScript
    {
        // This type is not fully decoded or correctly sized, just enough to use the functor
        class ScriptObjectMessage
        {
        public: 
            uint32_t                      unk00;
            ObjectTypeInfo* typeInfo;
            void*                         unk08;
            uint32_t                      unk0C;
            uint32_t                      unk10;
            uint32_t                      unk14;
            uint32_t                      unk18;
        };

        class IForEachScriptObjectFunctor
        {
        public:
            IForEachScriptObjectFunctor() = default;
            virtual ~IForEachScriptObjectFunctor() = default;

            // return true to continue
            virtual bool Visit(ScriptObjectMessage* arg, void* arg2) = 0;
        };
    }
}