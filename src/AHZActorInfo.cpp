#include "PCH.h"
#include "AHZActorInfo.h"

// 14062DE60 1.6.318
auto CAHZActorInfo::IsSentient(RE::Actor* actor) -> uint32_t
{
    using func_t = decltype(&CAHZActorInfo::IsSentient);
    REL::Relocation<func_t> func{ REL::ID(37913) };
    return func(actor);
}

// 1403D91A0 1.6.318
auto CAHZActorInfo::GetSoulType(uint16_t actorLevel, uint8_t isActorSentient) -> uint32_t
{
    using func_t = decltype(&CAHZActorInfo::GetSoulType);
    REL::Relocation<func_t> func{ REL::ID(26520) };
    return func(actorLevel, isActorSentient);
}
