#pragma once

class CAHZTarget
{
public:
    void SetTarget(RE::TESObjectREFR* pTargetRef);

    [[nodiscard]] bool IsReference()
    {
        return m_pObjectRef != nullptr;
    };

    [[nodiscard]] bool IsValid() {
        return m_pForm != nullptr;
    };

    [[nodiscard]] RE::TESObjectREFR* GetReference() { return m_pObjectRef; };
    [[nodiscard]] RE::TESForm*       GetForm() { return m_pForm; };

private:
    RE::TESObjectREFR* m_pObjectRef;
    RE::TESForm*       m_pForm;
    mutable RE::BSReadWriteLock _lock;
};