#include "pch.h"
#include "AHZTarget.h"
#include "AHZFormLookup.h"

void CAHZTarget::SetTarget(const RE::NiPointer<RE::TESObjectREFR> &target)
{
    RE::BSWriteLockGuard locker(_lock);
    // Copy to attach a new ref count
    s_target = target;
    m_pForm = CAHZFormLookup::Instance().GetTESForm(s_target.get());
    if (m_pForm) {
        if (m_pForm->GetFormType() == RE::FormType::Reference)
        {
            m_pObjectRef = m_pForm->As<RE::TESObjectREFR>();
            if (m_pObjectRef)
                m_pForm = m_pObjectRef->GetBaseObject();
            else
                m_pForm = nullptr;
        } else {
            m_pObjectRef = nullptr;
        }
    } else {
        m_pObjectRef = nullptr;
    }
}

void CAHZTarget::ClearTarget(){
    RE::BSWriteLockGuard locker(_lock);
    m_pForm = nullptr;
    m_pObjectRef = nullptr;
}

RE::TESObjectREFR* CAHZTarget::GetReference()
{
    return m_pObjectRef;
}

RE::TESForm* CAHZTarget::GetForm()
{
    return m_pForm;
}