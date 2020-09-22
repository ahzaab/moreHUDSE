#include "PCH.h"
#include "AHZFormLookup.h"

// Base Address = 7FF62ACA0000
//.text:00007FF62BEEF240; == == == == == == == = S U B R O U T I N E == == == == == == == == == == == == == == == == == == == =
//.text:00007FF62BEEF240
//.text : 00007FF62BEEF240
//.text : 00007FF62BEEF240                         GetVMVariable   proc near; CODE XREF : sub_7FF62AF83E00 + 63↑p
//.text : 00007FF62BEEF240; sub_7FF62AFC2890 + 2E1↑p ...
//.text:00007FF62BEEF240
//.text : 00007FF62BEEF240                         arg_0 = qword ptr  8
//.text : 00007FF62BEEF240                         arg_8 = qword ptr  10h
//.text : 00007FF62BEEF240                         arg_10 = qword ptr  18h
//.text : 00007FF62BEEF240                         arg_18 = qword ptr  20h
//.text : 00007FF62BEEF240
//.text : 00007FF62BEEF240 48 89 5C 24 08                          mov[rsp + arg_0], rbx
//.text:00007FF62BEEF245 48 89 74 24 10                          mov[rsp + arg_8], rsi
//.text : 00007FF62BEEF24A 48 89 7C 24 18                          mov[rsp + arg_10], rdi
//.text : 00007FF62BEEF24F 4C 89 74 24 20                          mov[rsp + arg_18], r14
//.text : 00007FF62BEEF254 41 57                                   push    r15
//.text : 00007FF62BEEF256 48 83 EC 20                             sub     rsp, 20h
//.text : 00007FF62BEEF25A 44 8B 05 17 C8 26 02                    mov     r8d, cs : TlsIndex
//.text : 00007FF62BEEF261 48 8B F9                                mov     rdi, rcx
//.text : 00007FF62BEEF264 65 48 8B 04 25 58 00 00 + mov     rax, gs : 58h
//.text : 00007FF62BEEF26D 83 CE FF or esi, 0FFFFFFFFh
//.text : 00007FF62BEEF270 B9 68 07 00 00                          mov     ecx, 768h
//.text : 00007FF62BEEF275 4C 8B FA                                mov     r15, rdx
//.text : 00007FF62BEEF278 4E 8B 34 C0                             mov     r14, [rax + r8 * 8]
//.text : 00007FF62BEEF27C 4C 03 F1                                add     r14, rcx
//.text : 00007FF62BEEF27F 41 8B 1E                                mov     ebx, [r14]
//.text : 00007FF62BEEF282 41 C7 06 14 00 00 00                    mov     dword ptr[r14], 14h
//.text : 00007FF62BEEF289 48 85 FF                                test    rdi, rdi
//.text : 00007FF62BEEF28C 0F 84 A0 00 00 00                       jz      loc_7FF62BEEF332
//.text : 00007FF62BEEF292
//.text : 00007FF62BEEF292                         loc_7FF62BEEF292 : ; CODE XREF : GetVMVariable + EC↓j
//.text : 00007FF62BEEF292 44 8B 47 20                             mov     r8d, [rdi + 20h]
//.text : 00007FF62BEEF296 41 8B C0                                mov     eax, r8d
//.text : 00007FF62BEEF299 83 E0 03 and eax, 3
//.text : 00007FF62BEEF29C 3C 03                                   cmp     al, 3
//.text : 00007FF62BEEF29E 0F 85 8E 00 00 00                       jnz     loc_7FF62BEEF332
//.text : 00007FF62BEEF2A4 48 8B 47 30                             mov     rax, [rdi + 30h]
//.text : 00007FF62BEEF2A8 41 8B C8                                mov     ecx, r8d
//.text : 00007FF62BEEF2AB 4D 8B 0F                                mov     r9, [r15]
//.text : 00007FF62BEEF2AE 48 C1 E9 02                             shr     rcx, 2
//.text : 00007FF62BEEF2B2 83 E1 3F                                and     ecx, 3Fh
//.text : 00007FF62BEEF2B5 41 C1 E8 08                             shr     r8d, 8
//.text:00007FF62BEEF2B9 41 81 E0 FF 03 00 00 and r8d, 3FFh
//.text : 00007FF62BEEF2C0 48 8D 14 C8                             lea     rdx, [rax + rcx * 8]
//.text : 00007FF62BEEF2C4 48 8B CF                                mov     rcx, rdi
//.text : 00007FF62BEEF2C7 E8 74 29 00 00                          call    sub_7FF62BEF1C40
//.text : 00007FF62BEEF2CC 4C 8B C8                                mov     r9, rax
//.text : 00007FF62BEEF2CF 48 85 C0                                test    rax, rax
//.text : 00007FF62BEEF2D2 74 51                                   jz      short loc_7FF62BEEF325
//.text : 00007FF62BEEF2D4 8B 47 20                                mov     eax, [rdi + 20h]
//.text : 00007FF62BEEF2D7 45 33 C0                                xor     r8d, r8d
//.text : 00007FF62BEEF2DA 48 8B 57 10                             mov     rdx, [rdi + 10h]
//.text : 00007FF62BEEF2DE 48 C1 E8 02                             shr     rax, 2
//.text : 00007FF62BEEF2E2 83 E0 3F                                and     eax, 3Fh
//.text : 00007FF62BEEF2E5 48 C1 E0 03                             shl     rax, 3
//.text : 00007FF62BEEF2E9 4C 2B C8                                sub     r9, rax
//.text : 00007FF62BEEF2EC 4C 2B 4F 30                             sub     r9, [rdi + 30h]
//.text : 00007FF62BEEF2F0 49 C1 F9 04                             sar     r9, 4
//.text : 00007FF62BEEF2F4 48 85 D2                                test    rdx, rdx
//.text : 00007FF62BEEF2F7 74 28                                   jz      short loc_7FF62BEEF321
//.text : 00007FF62BEEF2F9 0F 1F 80 00 00 00 00                    nop     dword ptr[rax + 00000000h]
//.text : 00007FF62BEEF300
//.text : 00007FF62BEEF300                         loc_7FF62BEEF300 : ; CODE XREF : GetVMVariable + DF↓j
//.text : 00007FF62BEEF300 8B 42 20                                mov     eax, [rdx + 20h]
//.text : 00007FF62BEEF303 8B C8                                   mov     ecx, eax
//.text : 00007FF62BEEF305 83 E1 03 and ecx, 3
//.text : 00007FF62BEEF308 80 F9 03                                cmp     cl, 3
//.text : 00007FF62BEEF30B 75 14                                   jnz     short loc_7FF62BEEF321
//.text : 00007FF62BEEF30D 48 8B 52 10                             mov     rdx, [rdx + 10h]
//.text : 00007FF62BEEF311 C1 E8 08                                shr     eax, 8
//.text:00007FF62BEEF314 25 FF 03 00 00 and eax, 3FFh
//.text : 00007FF62BEEF319 44 03 C0                                add     r8d, eax
//.text : 00007FF62BEEF31C 48 85 D2                                test    rdx, rdx
//.text : 00007FF62BEEF31F 75 DF                                   jnz     short loc_7FF62BEEF300
//.text : 00007FF62BEEF321
//.text : 00007FF62BEEF321                         loc_7FF62BEEF321 : ; CODE XREF : GetVMVariable + B7↑j
//.text : 00007FF62BEEF321; GetVMVariable + CB↑j
//.text:00007FF62BEEF321 43 8D 34 08                             lea     esi, [r8 + r9]
//.text:00007FF62BEEF325
//.text : 00007FF62BEEF325                         loc_7FF62BEEF325 : ; CODE XREF : GetVMVariable + 92↑j
//.text : 00007FF62BEEF325 48 8B 7F 10                             mov     rdi, [rdi + 10h]
//.text : 00007FF62BEEF329 48 85 FF                                test    rdi, rdi
//.text : 00007FF62BEEF32C 0F 85 60 FF FF FF                       jnz     loc_7FF62BEEF292
//.text : 00007FF62BEEF332
//.text : 00007FF62BEEF332                         loc_7FF62BEEF332 : ; CODE XREF : GetVMVariable + 4C↑j
//.text : 00007FF62BEEF332; GetVMVariable + 5E↑j
//.text:00007FF62BEEF332 48 8B 7C 24 40                          mov     rdi, [rsp + 28h + arg_10]
//.text : 00007FF62BEEF337 8B C6                                   mov     eax, esi
//.text : 00007FF62BEEF339 48 8B 74 24 38                          mov     rsi, [rsp + 28h + arg_8]
//.text : 00007FF62BEEF33E 41 89 1E                                mov[r14], ebx
//.text : 00007FF62BEEF341 48 8B 5C 24 30                          mov     rbx, [rsp + 28h + arg_0]
//.text : 00007FF62BEEF346 4C 8B 74 24 48                          mov     r14, [rsp + 28h + arg_18]
//.text : 00007FF62BEEF34B 48 83 C4 20                             add     rsp, 20h
//.text : 00007FF62BEEF34F 41 5F                                   pop     r15
//.text : 00007FF62BEEF351 C3                                      retn
//.text : 00007FF62BEEF351                         GetVMVariable   endp
//.text : 00007FF62BEEF351
//.text : 00007FF62BEEF351; -------------------------------------------------------------------------- -
//


class CAHZGetScriptVariableFunctor : public RE::IForEachScriptObjectFunctor
{
public:
   CAHZGetScriptVariableFunctor(RE::VMClassRegistry * registry, UInt64 handle, string var)
   {
      m_registry = registry;
      m_handle = handle;
      m_variable = var;
   }
   virtual ~CAHZGetScriptVariableFunctor() {}

   bool iequals(const string& a, const string& b)
   {
      unsigned int sz = a.size();
      if (b.size() != sz)
         return false;
      for (unsigned int i = 0; i < sz; ++i)
         if (tolower(a[i]) != tolower(b[i]))
            return false;
      return true;
   }

   virtual bool Visit(VMScriptInstance * script, void * arg2)
   {
      VMClassInfo * classInfo = script->classInfo;
      if (!classInfo)
      {
         m_result.type = VMValue::kType_None;
         return false;
      }

      BSFixedString varName(m_variable.c_str());
      UInt32 variableId = CALL_MEMBER_FN(classInfo, GetVariable)(&varName);
      if (variableId == -1) {
         m_result.type = VMValue::kType_None;
         return false;
      }

      if (m_registry->ExtractValue(m_handle, &classInfo->name, variableId, &m_result))
      {
         // Found
         //_MESSAGE("Found at Class: %s, Property, %s", classInfo->name, m_variable.c_str());
         return false;
      }

      return false;
   }

   VMValue * getResult() { return m_result.type == VMValue::kType_None ? NULL : &m_result; }

private:
   VMClassRegistry	* m_registry;
   string	m_variable;
   UInt64			m_handle;
   VMValue			m_result;
};

CAHZFormLookup& CAHZFormLookup::Instance() {
   static CAHZFormLookup theInstance;

   if (theInstance.m_scriptVMVariables.empty())
   {
      //theInstance.m_scriptVMVariables.push_back("::lootable_var");
      //theInstance.m_scriptVMVariables.push_back("::Nirnroot_var");
      //theInstance.m_scriptVMVariables.push_back("::TempleBlessing_var");
      //theInstance.m_scriptVMVariables.push_back("::nonIngredientLootable_var");
      //theInstance.m_scriptVMVariables.push_back("::myIngredient_var");
      //theInstance.m_scriptVMVariables.push_back("::myFood_var");
   }
   return theInstance;
}

TESForm * CAHZFormLookup::GetTESForm(TESObjectREFR * targetReference)
{
	TESForm * lutForm = NULL;
	if ((lutForm = GetFormFromLookup(targetReference)) != NULL)
	{
		return lutForm;
	}
	else if (targetReference->baseForm && targetReference->baseForm->formType == kFormType_Activator)
	{
		return GetAttachedForm(targetReference);
	}
	else if (targetReference->baseForm && targetReference->baseForm->formType == kFormType_Projectile)
	{
		Projectile *pProjectile = (DYNAMIC_CAST(targetReference, TESObjectREFR, Projectile));

		if (pProjectile) {
			AHZProjectile *a = (AHZProjectile*)(pProjectile);
			if (a && a->sourceAmmo)
				return a->sourceAmmo;
			else
				return targetReference;
		}
		else
			return targetReference;
	}
	else
	{
		return targetReference;
	}
}

RE::TESForm * CAHZFormLookup::GetFormFromLookup(RE::TESObjectREFR * targetRef)
{
	if (!targetRef || !targetRef->GetBaseObject())
		return nullptr;

	if (m_LUT.find(targetRef->GetBaseObject()->formID) != m_LUT.end())
	{
        uint32_t formID = m_LUT.find(targetRef->GetBaseObject()->formID)->second;
		auto form = RE::TESForm::LookupByID(formID);
		return form;
	}
	return NULL;
}

void CAHZFormLookup::AddScriptVarable(string vmVariableName)
{
   string prefix = "::";
   string suffix = "_var";

   if (vmVariableName.length() < prefix.length() || vmVariableName.substr(0, prefix.length()) != prefix)
   {
      vmVariableName.insert(0, prefix);
   }

   if (vmVariableName.length() < suffix.length() || vmVariableName.substr(vmVariableName.length() - suffix.length(), suffix.length()) != suffix)
   {
      vmVariableName.append(suffix);
   }

   if (find(m_scriptVMVariables.begin(), m_scriptVMVariables.end(), vmVariableName) == m_scriptVMVariables.end())
   {
      m_scriptVMVariables.push_back(vmVariableName);
   }
}

void CAHZFormLookup::AddFormID(string baseFormModName, uint32_t baseFormID, string targetFormModName, uint32_t targetFormID)
{
    auto dataHandler = RE::TESDataHandler::GetSingleton();

    // Normalize to the raw formIDs
    auto isBaseLight = dataHandler->GetLoadedLightModIndex(baseFormModName).has_value();
    baseFormID = isBaseLight ? baseFormID & 0x00000FFF : baseFormID & 0x00FFFFFF;
    auto isTargetLight = dataHandler->GetLoadedLightModIndex(targetFormModName).has_value();
    targetFormID = isBaseLight ? targetFormID & 0x00000FFF : targetFormID & 0x00FFFFFF;

    auto baseForm = dataHandler->LookupForm(baseFormID, baseFormModName);

    if (!baseForm) {
        return;
    }

    auto targetForm = dataHandler->LookupForm(targetFormID, targetFormModName);
    if (!targetForm) {
        return;
    }

    // Load into map if the entry does not already exist
    if (m_LUT.find(baseForm->formID) == m_LUT.end()) {
        m_LUT[baseForm->formID] = targetForm->formID;
        logger::trace("ACTI BASE ID:{:#08x}, ACTI Targ ID:{:#08x}", baseForm->formID, targetForm->formID);
    }
}

RE::TESForm* CAHZFormLookup::GetAttachedForm(RE::TESObjectREFR* form)
{
    vector<string>::iterator p;

    if (!form) {
        return nullptr;
    }

    if (!form->GetBaseObject()) {
        return NULL;
    }

    if (form->GetBaseObject()->formType != RE::FormType::Activator) {
        return nullptr;
    }

    for (p = m_scriptVMVariables.begin(); p != m_scriptVMVariables.end(); p++) {
        //_MESSAGE("GetAttachedForm");
        RE::TESForm* attachedForm = NULL;
        if ((attachedForm = GetAttachedForm(form, *p)) != NULL) {
            if (attachedForm->formType == RE::FormType::LeveledItem) {
                auto lvli = DYNAMIC_CAST(attachedForm, RE::TESForm, RE::TESLevItem);

                // Get the first form and see if it is an ingredient
                if (lvli && lvli->entries.size() > 0) {
                    auto itemform = lvli->entries[0].form;
                    return itemform;
                }
            } else if (attachedForm->formType == RE::FormType::FormList) {
                auto* lvli = DYNAMIC_CAST(attachedForm, RE::TESForm, RE::BGSListForm);

                // Get the first form and see if it is an ingredient
                if (lvli && lvli->forms.size() > 0) {
                    auto itemform = lvli->forms[0];
                    return itemform;
                }
            } else {
                return attachedForm;
            }
        }
    }

    return nullptr;
}


auto CAHZFormLookup::GetScriptVariable(RE::TESForm* a_form, const char* a_scriptName, const char* a_scriptVariable) -> RE::BSScript::Variable const
{
    RE::BSScript::Variable var;
    if (a_form == nullptr) {
        return var;
    }

    auto variableName = std::string(a_scriptVariable);
    variableName.insert(0, "::");
    variableName.append("_var");
    auto                                      vm = RE::SkyrimVM::GetSingleton();
    auto                                      vmImpl = vm->impl;
    auto                                      handlePolicy = vmImpl.get()->GetObjectHandlePolicy();
    auto                                      handle = handlePolicy->GetHandleForObject(a_form->GetFormType(), a_form);
    RE::BSTSmartPointer<RE::BSScript::Object> result;

    vmImpl->FindBoundObject(handle, a_scriptName, result);

    if (!result.get()) {
        return var;
    }

    RE::BSTSmartPointer<RE::BSScript::ObjectTypeInfo> info;
    vmImpl->GetScriptObjectType(a_scriptName, info);
    auto iter = info->GetVariableIter();
    if (iter) {
        for (std::uint32_t i = 0; i < info->GetTotalNumVariables(); ++i) {
            auto& prop = iter[i];
            if (std::string(prop.name.c_str()) == variableName) {
                vmImpl->GetVariableValue(result, i, var);
                return var;
            }
        }
    }

    return var;
}

RE::TESForm* CAHZFormLookup::GetAttachedForm(RE::TESObjectREFR *form, string variableName)
{
   if (form) {
      VMClassRegistry		* registry = (*g_skyrimVM)->GetClassRegistry();
      IObjectHandlePolicy	* policy = registry->GetHandlePolicy();

	  if (!form->GetBaseObject())
		  return nullptr;

      UInt64 handle = policy->Create(form->baseForm->formType, form);
      if (handle != policy->GetInvalidHandle())
      {
         CAHZGetScriptVariableFunctor scriptVariable(registry, handle, variableName);
         registry->VisitScripts(handle, &scriptVariable);

         VMValue * retValue;
         if ((retValue = scriptVariable.getResult()) != NULL)
         {
            if (retValue->IsIdentifier())
            {
              RE::TESForm * retForm;
               UnpackValue<TESForm>(&retForm, retValue);
               return retForm;
            }
         }
      }
   }

   return NULL;
}

CAHZFormLookup::CAHZFormLookup()
{
}

CAHZFormLookup::~CAHZFormLookup()
{
}