#include "pch.h"
#include "AHZTarget.h"
#include "AHZFormLookup.h"

void CAHZTarget::SetTarget(RE::TESObjectREFR* pTargetRef)
{
    RE::BSReadLockGuard locker(_lock);
    m_pForm = CAHZFormLookup::Instance().GetTESForm(pTargetRef);
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
