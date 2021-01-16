#include "PCH.h"
#include "AHZActorInfo.h"

auto CAHZActorInfo::IsSentient(RE::Actor* actor) -> uint32_t
{
    using func_t = decltype(&CAHZActorInfo::IsSentient);
    REL::Relocation<func_t> func{ REL::ID(36889) };
    return func(actor);
}

auto CAHZActorInfo::GetSoulType(uint16_t actorLevel, uint8_t isActorSentient) -> uint32_t
{
    using func_t = decltype(&CAHZActorInfo::GetSoulType);
    REL::Relocation<func_t> func{ REL::ID(25933) };
    return func(actorLevel, isActorSentient);
}
