#include "pch.h"
#include "AHZActorInfo.h"
#include "Offsets-VR.h"
#include "Offsets-SE.h"
#include "Offsets-AE.h"


// 14062DE60 1.6.318
auto CAHZActorInfo::IsSentient(RE::Actor* actor) -> uint32_t
{
    using func_t = decltype(&CAHZActorInfo::IsSentient);
    REL::Relocation<func_t> func{ REL::VariantID{ 
        moreHUDSE::SE::Offsets::IsSentient.id(), 
        moreHUDSE::AE::Offsets::IsSentient.id(),
        moreHUDSE::VR::Offsets::IsSentient.offset() } };
    return func(actor);
}

// 1403D91A0 1.6.318
auto CAHZActorInfo::GetSoulType(uint16_t actorLevel, uint8_t isActorSentient) -> uint32_t
{
    using func_t = decltype(&CAHZActorInfo::GetSoulType);
    REL::Relocation<func_t> func{ REL::VariantID{ 
        moreHUDSE::SE::Offsets::GetSoulType.id(), 
        moreHUDSE::AE::Offsets::GetSoulType.id(),
        moreHUDSE::VR::Offsets::GetSoulType.offset() } };
    return func(actorLevel, isActorSentient);
}
