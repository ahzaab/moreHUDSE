#include "pch.h"
#include "IForEachScriptObjectFunctor.h"
#include "AHZForEachScriptObjectFunctor.h"

//using namespace std;

CAHZForEachScriptObjectFunctor::CAHZForEachScriptObjectFunctor(
    std::string a_varName)
{
    m_result.SetNone();

    std::string prefix = "::";
    std::string suffix = "_var";

    // Properties omit the prefix and sufix, but we are looking at variables
    m_variable = prefix + a_varName + suffix;
    m_property = a_varName;
}

auto CAHZForEachScriptObjectFunctor::Visit(RE::BSScript::IForEachScriptObjectFunctor::SCRIPT_OBJECT_MESSAGE* script, [[maybe_unused]] void* unk1) -> bool
{
    auto classInfo = script->typeInfo;
    if (classInfo == nullptr) {
        return true;
    }

    auto                                      vm = RE::SkyrimVM::GetSingleton()->impl;
    RE::BSTSmartPointer<RE::BSScript::Object> boundObject;

    m_variableOffset = classInfo->GetTotalNumVariables();

    // Start with child and look through all parents
    for (auto iter = classInfo; iter; iter = iter->GetParent()) {
        m_variableOffset -= iter->GetNumVariables();
        vm->FindBoundObject(script->handle, iter->name.data(), boundObject);
        auto stillLooking = (VisitProperties(iter, boundObject) && VisitVariables(iter, boundObject));

        if (!stillLooking) {
            return false;
        }
    }

    // After all this searching, the variable is still not found
    return true;
}

bool CAHZForEachScriptObjectFunctor::VisitVariables(RE::BSScript::ObjectTypeInfo* typeInfo, RE::BSTSmartPointer<RE::BSScript::Object>& boundObject)
{
    auto vm = RE::SkyrimVM::GetSingleton()->impl;
    auto it = typeInfo->GetVariableIter();

    if (!it) {
        return true;
    }

    for (std::uint32_t i = 0; i < typeInfo->GetNumVariables(); i++) {
        auto& element = it[i];
        if (element.name.empty()) {
            continue;
        }
        if (element.name.data() == m_variable || element.name.data() == m_property) {
            auto found = vm->GetVariableValue(boundObject, m_variableOffset + i, m_result);
            found = found && !m_result.IsNoneObject() && !m_result.IsNoneArray();
            if (found) {
                return false;
            }
        }
    }

    return true;
}
bool CAHZForEachScriptObjectFunctor::VisitProperties(RE::BSScript::ObjectTypeInfo* typeInfo, RE::BSTSmartPointer<RE::BSScript::Object>& boundObject)
{
    auto vm = RE::SkyrimVM::GetSingleton()->impl;
    auto it = typeInfo->GetPropertyIter();

    if (!it) {
        return true;
    }

    for (std::uint32_t i = 0; i < typeInfo->GetNumProperties(); i++) {
        auto& element = it[i];
        if (element.name.empty()) {
            continue;
        }
        if (element.name.data() == m_property) {
            auto found = vm->GetPropertyValue(boundObject, element.name.data(), m_result);
            found = found && !m_result.IsNoneObject() && !m_result.IsNoneArray();
            if (found) {
                return false;
            }
        }
    }

    return true;
}

auto CAHZForEachScriptObjectFunctor::GetScriptVariable() -> RE::BSScript::Variable*
{
    return (m_result.IsNoneObject() || m_result.IsNoneArray()) ? nullptr : &m_result;
}
