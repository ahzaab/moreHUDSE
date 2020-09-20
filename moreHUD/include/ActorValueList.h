#pragma once

namespace SKSE
{
    class ActorValueList
    {
    public:
        inline static constexpr uint32_t kNumActorValues = static_cast<uint32_t>(RE::ActorValue::kTotal);

        static ActorValueList* GetSingleton(void);
        RE::ActorValueInfo*        GetActorValue(RE::ActorValue id);

    private:
        uint32_t        unk00;                         // 00
        uint32_t        pad04;                         // 04
        RE::ActorValueInfo* actorValues[kNumActorValues];  // 08
    };

}