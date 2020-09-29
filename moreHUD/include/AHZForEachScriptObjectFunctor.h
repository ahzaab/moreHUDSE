#pragma once

class CAHZForEachScriptObjectFunctor : public RE::BSScript::IForEachScriptObjectFunctor
{
public:
    CAHZForEachScriptObjectFunctor(std::string a_varName);
    ~CAHZForEachScriptObjectFunctor() = default;
    virtual auto Visit(RE::BSScript::IForEachScriptObjectFunctor::SCRIPT_OBJECT_MESSSAGE* script, [[maybe_unused]] void* arg2) -> bool;
    auto         GetScriptVariable() -> RE::BSScript::Variable*;

private:
    std::string            m_variable;
    std::string            m_property;
    RE::BSScript::Variable m_result;

    bool VisitProperties(RE::BSScript::ObjectTypeInfo* typeInfo, RE::BSTSmartPointer<RE::BSScript::Object>& boundObject);
    bool VisitVariables(RE::BSScript::ObjectTypeInfo* typeInfo, RE::BSTSmartPointer<RE::BSScript::Object>& boundObject);
};
