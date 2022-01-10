#include "pch.h"
#include "AHZTarget.h"
#include "AHZFormLookup.h"

void CAHZTarget::SetTarget(const RE::TESObjectREFR* pTargetRef)
{
    m_pForm = CAHZFormLookup::Instance().GetTESForm(pTargetRef);

    if (m_pForm) {
        m_pObjectRef = CAHZFormLookup::Instance().GetReference(m_pForm);
    } else {
        m_pObjectRef = nullptr;
    }
}
