#pragma once

class CAHZTarget
{
public:
    void SetTarget(const RE::NiPointer<RE::TESObjectREFR> &target);
    void ClearTarget();
    mutable RE::BSReadWriteLock _lock;

    static CAHZTarget * Singleton(){
        static CAHZTarget instance;
        return std::addressof(instance);
    }

    [[nodiscard]] bool IsReference()
    {
        return m_pObjectRef != nullptr;
    };

    [[nodiscard]] bool IsValid() {
        return m_pForm != nullptr;
    };

    [[nodiscard]] RE::TESObjectREFR* GetReference();
    [[nodiscard]] RE::TESForm*       GetForm();

private:
    CAHZTarget() :
        m_pObjectRef(nullptr),
        m_pForm(nullptr) {}

    CAHZTarget(const CAHZTarget &other) = delete;
    RE::TESObjectREFR* m_pObjectRef;
    RE::TESForm*       m_pForm;
    
    RE::NiPointer<RE::TESObjectREFR> s_target;
};