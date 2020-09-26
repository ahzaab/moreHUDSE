#include "PCH.h"
#include "AHZActorInfo.h"

uint32_t CAHZActorInfo::IsSentient(RE::Actor* actor)
{
    using func_t = decltype(&CAHZActorInfo::IsSentient);
    REL::Relocation<func_t> func{ REL::ID(36889) };
    return func(actor);
}

uint32_t CAHZActorInfo::GetSoulType(uint16_t actorLevel, uint8_t isActorSentient)
{
    using func_t = decltype(&CAHZActorInfo::GetSoulType);
    REL::Relocation<func_t> func{ REL::ID(25933) };
    return func(actorLevel, isActorSentient);
}
