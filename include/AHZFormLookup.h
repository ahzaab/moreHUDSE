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
//using namespace std;

class CAHZFormLookup
{
public:
    static CAHZFormLookup& Instance();
    RE::TESForm*           GetTESForm(RE::TESObjectREFR* targetReference);
    void                   AddScriptVarable(std::string vmVariableName);
    void                   AddFormID(std::string baseFormModName, uint32_t baseFormID, std::string targetFormModName, uint32_t targetFormID);

    inline static RE::TESObjectREFR* GetReference(RE::TESForm* theForm)
    {
        if (!theForm) {
            return nullptr;
        }

        if (theForm->GetFormType() == RE::FormType::Reference) {
            auto reference = theForm->As<RE::TESObjectREFR>();
            return reference;
        } else {
            return nullptr;
        }
    }

private:
    // Force singleton
    CAHZFormLookup() = default;
    ~CAHZFormLookup() = default;
    RE::TESForm*        GetAttachedForm(RE::TESObjectREFR* form);
    RE::TESForm*        GetFormFromLookup(RE::TESObjectREFR* targetRef);
    static RE::TESForm* GetAttachedForm(RE::TESObjectREFR* form, std::string variableName);
    static int32_t      GetAttachedInteger(RE::TESObjectREFR* form, std::string variableName);
    CAHZFormLookup(CAHZFormLookup const&);                            // copy ctor is hidden
    CAHZFormLookup&              operator=(CAHZFormLookup const&){};  // assign op is hidden
    std::vector<std::string>     m_scriptVMVariables;
    std::map<uint32_t, uint32_t> m_LUT;
    auto                         GetScriptVariable(RE::TESForm* a_form, const char* a_scriptName, const char* a_scriptVariable) -> RE::BSScript::Variable const;
    static bool                  s_lotdCheck;
    static bool                  s_lotdInstalled;
};

//#define AHZGetForm(x)      (CAHZFormLookup::Instance().GetTESForm((x)))
//#define AHZGetReference(x) (CAHZFormLookup::Instance().GetReference((x)))
