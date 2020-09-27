#include "PCH.h"
#include "IForEachScriptObjectFunctor.h" 
#include "AHZForEachScriptObjectFunctor.h" 

using namespace std;

CAHZForEachScriptObjectFunctor::CAHZForEachScriptObjectFunctor(
    string a_varName)
{
    m_result.SetNone();

    string prefix = "::";
    string suffix = "_var";

    // Properties omit the prefix and sufix, but we are looking at variables
    m_variable = prefix + a_varName + suffix;
}


auto CAHZForEachScriptObjectFunctor::Visit(RE::BSScript::IForEachScriptObjectFunctor::SCRIPT_OBJECT_MESSSAGE* script, [[maybe_unused]] void* arg2) -> bool
{
    auto classInfo = script->typeInfo;
    if (classInfo == nullptr) {
        return true;
    }

    if (classInfo->variableCount == 0u) {
        return true;
    }

    auto iter = classInfo->GetVariableIter();
    if (iter != nullptr) {
        for (std::uint32_t i = 0; i < classInfo->GetTotalNumVariables(); ++i) {
            auto& prop = iter[i];
            if (std::string(prop.name.c_str()) == m_variable) {
                auto                                      vm = RE::SkyrimVM::GetSingleton()->impl;
                RE::BSTSmartPointer<RE::BSScript::Object> boundObject;
                vm->FindBoundObject(script->handle, classInfo->name.data(), boundObject);

                auto found = vm->GetVariableValue(boundObject, i, m_result);

                if (found) {
                    logger::trace("found variable: {}, on script: {}", prop.name.c_str(), classInfo->name);
                }

                // Found returns false to stop the visitor
                return !found;
            }
        }
    }
    return true;
}

auto CAHZForEachScriptObjectFunctor::GetScriptVariable() -> RE::BSScript::Variable*
{
    return (m_result.IsNoneObject() || m_result.IsNoneArray()) ? nullptr : &m_result;
}