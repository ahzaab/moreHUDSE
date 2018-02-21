#include "AHZFormLookup.h"

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
		theInstance.m_scriptVMVariables.push_back("::lootable_var");
		theInstance.m_scriptVMVariables.push_back("::Nirnroot_var");
		theInstance.m_scriptVMVariables.push_back("::TempleBlessing_var");
		theInstance.m_scriptVMVariables.push_back("::nonIngredientLootable_var");
		theInstance.m_scriptVMVariables.push_back("::myIngredient_var");
		theInstance.m_scriptVMVariables.push_back("::myFood_var");
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
	else if (targetReference->baseForm->formType == kFormType_Activator)
	{
		return GetAttachedForm(targetReference);
	}
	else
	{
		return targetReference;
	}
}

TESForm * CAHZFormLookup::GetFormFromLookup(TESObjectREFR * targetRef)
{
	if (m_LUT.find(targetRef->baseForm->formID) != m_LUT.end())
	{
		UInt32 formID = m_LUT.find(targetRef->baseForm->formID)->second;
		TESForm * form = LookupFormByID(formID);
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

void CAHZFormLookup::AddFormID(string baseFormModName, UInt32 baseFormID, string targetFormModName, UInt32 targetFormID)
{
	// If not exists
	if (m_modIndexLUT.find(baseFormModName) == m_modIndexLUT.end())
	{
		DataHandler * dataHandler = DataHandler::GetSingleton();
		BSFixedString b(baseFormModName.c_str());
		UInt32 modIndex = (UInt32)dataHandler->GetModIndex(b.data) << 24;
		m_modIndexLUT[baseFormModName] = modIndex;
	}

   // If not exists
   if (m_modIndexLUT.find(targetFormModName) == m_modIndexLUT.end())
   {
      DataHandler * dataHandler = DataHandler::GetSingleton();
      BSFixedString b(targetFormModName.c_str());
      UInt32 modIndex = (UInt32)dataHandler->GetModIndex(b.data) << 24;
      m_modIndexLUT[targetFormModName] = modIndex;
   }

	// If exists
	if (m_modIndexLUT.find(baseFormModName) != m_modIndexLUT.end() && m_modIndexLUT.find(targetFormModName) != m_modIndexLUT.end())
	{
		UInt8 baseModIndex = m_modIndexLUT[baseFormModName];
      UInt8 targetModIndex = m_modIndexLUT[targetFormModName];
		if (baseModIndex != 0xFF000000 && baseModIndex != 0xFF000000)
		{
			m_LUT[(baseFormID & 0x00FFFFFF) | baseModIndex] = (targetFormID & 0x00FFFFFF | targetModIndex);
		}
	}
}

TESForm * CAHZFormLookup::GetAttachedForm(TESObjectREFR *form)
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

	for (p = m_scriptVMVariables.begin(); p != m_scriptVMVariables.end(); p++) {

		//_MESSAGE("GetAttachedForm");
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

TESForm* CAHZFormLookup::GetAttachedForm(TESObjectREFR *form, string variableName)
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

CAHZFormLookup::CAHZFormLookup()
{
}

CAHZFormLookup::~CAHZFormLookup()
{
}