#include "pch.h"
#include "AHZFormLookup.h"
#include "AHZForEachScriptObjectFunctor.h"

bool CAHZFormLookup::s_lotdCheck = false;
bool CAHZFormLookup::s_lotdInstalled = false;

auto CAHZFormLookup::Instance() -> CAHZFormLookup&
{
    static CAHZFormLookup theInstance;
    return theInstance;
}

auto CAHZFormLookup::GetTESForm(RE::TESObjectREFR* targetReference) -> RE::TESForm*
{
    if (!targetReference) {
        return nullptr;
    }
    RE::TESForm* lutForm = nullptr;
    if ((lutForm = GetFormFromLookup(targetReference)) != nullptr) {
        return lutForm;
    } else if (targetReference->GetBaseObject() && targetReference->GetBaseObject()->formType == RE::FormType::Activator) {
        return GetAttachedForm(targetReference);
    } else if (targetReference->GetBaseObject() && targetReference->GetBaseObject()->formType == RE::FormType::Projectile) {
        auto pProjectile = targetReference->As<RE::Projectile>();

        if (pProjectile) {
            auto a = reinterpret_cast<const AHZProjectile*>(pProjectile);
            if (a && a->sourceAmmo)
                return a->sourceAmmo;
            else
                return targetReference;
        } else
            return targetReference;
    } else {
        return targetReference;
    }
}

auto CAHZFormLookup::GetFormFromLookup(RE::TESObjectREFR* targetRef) -> RE::TESForm*
{
    if (!targetRef || !targetRef->GetBaseObject())
        return nullptr;

    if (m_LUT.find(targetRef->GetBaseObject()->formID) != m_LUT.end()) {
        uint32_t formID = m_LUT.find(targetRef->GetBaseObject()->formID)->second;
        auto     form = RE::TESForm::LookupByID(formID);
        return form;
    }
    return nullptr;
}

void CAHZFormLookup::AddScriptVarable(std::string vmVariableName)
{
    if (find(m_scriptVMVariables.begin(), m_scriptVMVariables.end(), vmVariableName) == m_scriptVMVariables.end()) {
        m_scriptVMVariables.push_back(vmVariableName);
    }
}

void CAHZFormLookup::AddFormID(std::string baseFormModName, uint32_t baseFormID, std::string targetFormModName, uint32_t targetFormID)
{
    auto dataHandler = RE::TESDataHandler::GetSingleton();

    auto isBaseLight = false;

    // Normalize to the raw formIDs
    if (!REL::Module::IsVR()) {
         isBaseLight = dataHandler->GetLoadedLightModIndex(baseFormModName).has_value();
    }

    baseFormID = isBaseLight ? baseFormID & 0x00000FFF : baseFormID & 0x00FFFFFF;

    auto isTargetLight = false;

    if (!REL::Module::IsVR()) {
        auto isTargetLight = dataHandler->GetLoadedLightModIndex(targetFormModName).has_value();
    }
    
    targetFormID = isTargetLight ? targetFormID & 0x00000FFF : targetFormID & 0x00FFFFFF;

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

auto CAHZFormLookup::GetAttachedForm(RE::TESObjectREFR* form) -> RE::TESForm*
{
    std::vector<std::string>::iterator p;

    if (!form) {
        return nullptr;
    }

    if (!form->GetBaseObject()) {
        return nullptr;
    }

    if (form->GetBaseObject()->formType != RE::FormType::Activator) {
        return nullptr;
    }

    for (p = m_scriptVMVariables.begin(); p != m_scriptVMVariables.end(); p++) {
        //_MESSAGE("GetAttachedForm");
        RE::TESForm* attachedForm = nullptr;
        if ((attachedForm = GetAttachedForm(form, *p)) != nullptr) {
            if (attachedForm->formType == RE::FormType::LeveledItem) {
                auto lvli = attachedForm->As<RE::TESLevItem>();

                // Get the first form and see if it is an ingredient
                if (lvli && lvli->entries.size() > 0) {
                    auto itemform = lvli->entries[0].form;
                    return itemform;
                }
            } else if (attachedForm->formType == RE::FormType::FormList) {
                auto* lvli = attachedForm->As<RE::BGSListForm>();

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

    //// Hardcoded for LOTD, I have to lookup by index
    //

    //if (!s_lotdCheck) {
    //    auto dataHandler = RE::TESDataHandler::GetSingleton();
    //    if (dataHandler && dataHandler->LookupModByName("LegacyoftheDragonborn.esm"sv)) {
    //        s_lotdInstalled = true;
    //    }
    //    s_lotdCheck = true;
    //}

    //if (s_lotdInstalled) {
    //    RE::TESForm* dbm_displayListBase = GetAttachedForm(form, "afDisplayList");
    //    int32_t dbm_displayListIndex = GetAttachedInteger(form, "aiDisplayListIndex");
    //    if (dbm_displayListBase && dbm_displayListBase->formType == RE::FormType::FormList){
    //        auto* lvli = dbm_displayListBase->As<RE::BGSListForm>();
    //        if (lvli && dbm_displayListIndex > -1 && dbm_displayListIndex < static_cast<int32_t>(lvli->forms.size())){
    //            return lvli->forms[dbm_displayListIndex];
    //        }
    //    }
    //}

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
        for (std::uint32_t i = 0; i < info->GetNumVariables(); ++i) {
            auto& prop = iter[i];
            if (std::string(prop.name.c_str()) == variableName) {
                vmImpl->GetVariableValue(result, i, var);
                return var;
            }
        }
    }

    return var;
}

auto CAHZFormLookup::GetAttachedForm(RE::TESObjectREFR* form, std::string variableName) -> RE::TESForm*
{
    if (form) {
        if (!form->GetBaseObject())
            return nullptr;

        auto vm = RE::SkyrimVM::GetSingleton()->impl;
        auto handlePolicy = vm.get()->GetObjectHandlePolicy();
        auto handle = handlePolicy->GetHandleForObject(form->GetFormType(), form);

        if (handle != handlePolicy->EmptyHandle()) {
            CAHZForEachScriptObjectFunctor functor(variableName);
            vm->ForEachBoundObject(handle, &functor);
            auto variable = functor.GetScriptVariable();

            if (variable && variable->IsObject()) {
                auto retForm = variable->Unpack<RE::TESForm*>();
                return retForm;
            }
        }
    }

    return nullptr;
}

auto CAHZFormLookup::GetAttachedInteger(RE::TESObjectREFR* form, std::string variableName) -> int32_t
{
    if (form) {
        if (!form->GetBaseObject())
            return -1;

        auto vm = RE::SkyrimVM::GetSingleton()->impl;
        auto handlePolicy = vm.get()->GetObjectHandlePolicy();
        auto handle = handlePolicy->GetHandleForObject(form->GetFormType(), form);

        if (handle != handlePolicy->EmptyHandle()) {
            CAHZForEachScriptObjectFunctor functor(variableName);
            vm->ForEachBoundObject(handle, &functor);
            auto variable = functor.GetScriptVariable();

            if (variable && variable->IsInt()) {
                return variable->GetSInt();
            }
        }
    }

    return -1;
}
