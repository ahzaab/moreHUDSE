#include "AHZVM.h"

class CAHZGetScriptVariableFunctor : public IForEachScriptObjectFunctor
{
public:
   CAHZGetScriptVariableFunctor(VMClassRegistry * registry, UInt64 handle, string var)
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
         return true;
      }

      BSFixedString varName(m_variable.c_str());
      UInt32 variableId = CALL_MEMBER_FN(classInfo, GetVariable)(&varName);
      if (variableId == -1) {
         return true;
      }

      if (m_registry->ExtractValue(m_handle, &classInfo->name, variableId, &m_result))
      {
         // Found
         _MESSAGE("Found at Class: %s, Property, %s", classInfo->name, m_variable.c_str());
         return false;
      }

      return true;
   }

   VMValue * getResult() { return m_result.type == VMValue::kType_None ? NULL : &m_result; }

private:
   VMClassRegistry	* m_registry;
   string	m_variable;
   UInt64			m_handle;
   VMValue			m_result;
};

CAHZTargetFinder& CAHZTargetFinder::Instance() {
   static CAHZTargetFinder theInstance;

   if (theInstance.m_vanillaItems.empty())
   {
      theInstance.m_vanillaItems.push_back("::lootable_var");
      theInstance.m_vanillaItems.push_back("::Nirnroot_var");
      theInstance.m_vanillaItems.push_back("::TempleBlessing_var");
      theInstance.m_vanillaItems.push_back("::nonIngredientLootable_var");
   }
   return theInstance;
}


TESForm * CAHZTargetFinder::GetAttachedForm(TESObjectREFR *form)
{
   vector<string>::iterator p;

   if (!form)
   {
      return NULL;
   }

   if (form->baseForm->formType != kFormType_Activator)
   {
      return NULL;
   }

   for (p = m_vanillaItems.begin(); p != m_vanillaItems.end(); p++) {
      _MESSAGE("GetAttachedForm");
      TESForm* attachedForm = NULL;
      if ((attachedForm = GetAttachedForm(form, *p)) != NULL)
      {
         if (attachedForm->formType == kFormType_LeveledItem)
         {
            TESLevItem *lvli = DYNAMIC_CAST(attachedForm, TESForm, TESLevItem);
            // Get the first form and see if it is an ingredient
            if (lvli->leveledList.length > 0)
            {
               TESForm *itemform = (TESForm *)lvli->leveledList.entries[0].form;
               return itemform;
            }
         }
         else if (attachedForm->formType == kFormType_List)
         {
            BGSListForm *lvli = DYNAMIC_CAST(attachedForm, TESForm, BGSListForm);
            // Get the first form and see if it is an ingredient
            if (lvli->forms.count > 0)
            {
               TESForm *itemform = (TESForm *)lvli->forms.entries[0];
               return itemform;
            }
         }
         else
         {
            return attachedForm;
         }
      }
   }

   return NULL;
}

TESForm* CAHZTargetFinder::GetAttachedForm(TESObjectREFR *form, string variableName)
{
   if (form) {
      VMClassRegistry		* registry = (*g_skyrimVM)->GetClassRegistry();
      IObjectHandlePolicy	* policy = registry->GetHandlePolicy();
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
               TESForm * retForm;
               UnpackValue<TESForm>(&retForm, retValue);
               return retForm;
            }
         }
      }
   }

   return NULL;
}

CAHZTargetFinder::CAHZTargetFinder()
{
}


CAHZTargetFinder::~CAHZTargetFinder()
{
}
