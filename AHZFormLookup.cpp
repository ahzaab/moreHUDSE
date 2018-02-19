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
	if (targetReference->baseForm->formType == kFormType_Activator)
	{
		return GetAttachedForm(targetReference);
	}

	//else if ()
	//else
	//{
	//	return targetReference;
	//}
}

TESForm * CAHZFormLookup::GetFormFromLookup(TESObjectREFR * targetRef)
{
	map<UInt32, UInt32>::iterator p;

	if (m_shrineLUT.find(formID) != m_shrineLUT.end())
	{
		UInt32 spellFormID = m_shrineLUT.find(formID)->second;
		TESForm * form = LookupFormByID(spellFormID);
		return DYNAMIC_CAST(form, TESForm, SpellItem);
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

void CAHZFormLookup::AddFormID(string modName, UInt32 baseFormID, UInt32 targetFormID)
{
	// If not exists
	if (m_modLUT.find(modName) == m_modLUT.end())
	{
		DataHandler * dataHandler = DataHandler::GetSingleton();
		BSFixedString b(modName.c_str());
		UInt32 modIndex = (UInt32)dataHandler->GetModIndex(b.data) << 24;
		m_modLUT[modName] = modIndex;
	}

	// If exists
	if (m_modLUT.find(modName) != m_modLUT.end())
	{
		UInt8 modIndex = m_modLUT[modName];
		if (modIndex != 0xFF000000)
		{
			m_LUT[(baseFormID & 0x00FFFFFF) | modIndex] = (targetFormID & 0x00FFFFFF | modIndex);
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