#include "pch.h"
#include "AHZTarget.h"
#include "AHZFormLookup.h"

void CAHZTarget::SetTarget(RE::NiPointer<RE::TESObjectREFR> pTargetRef)
{
    RE::BSReadLockGuard locker(_lock);
    // RAII - stays in scope for the life of the CAHZTarget object
    m_crossHairRef = pTargetRef;
    
    m_pForm = CAHZFormLookup::Instance().GetTESForm(m_crossHairRef.get());
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
