

class CAHZTarget
{
public:
    CAHZTarget(const CAHZTarget& other) noexcept
    {
        m_pObjectRef = other.m_pObjectRef;
        m_pForm = other.m_pForm;
    }
    CAHZTarget() = default;

    void SetTarget(const RE::TESObjectREFR* pTargetRef);

    [nodiscard] bool IsReference() const noexcept
    {
        return m_pObjectRef != nullptr;
    };

    [nodiscard] bool IsValid() const noexcept {
        return m_pForm != nullptr;
    };

    [nodiscard] const RE::TESObjectREFR* GetReference() const noexcept { return m_pObjectRef; };
    [nodiscard] const RE::TESForm*       GetForm() const noexcept { return m_pForm; };

private:
    const RE::TESObjectREFR* m_pObjectRef{ nullptr };
    const RE::TESForm*       m_pForm{ nullptr };
};