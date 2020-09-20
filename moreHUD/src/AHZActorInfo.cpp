#include "AHZActorInfo.h"

// Is Actor Sentient = 606850
// Get Soul Level = 3C1740


typedef  UInt32 (__fastcall *GET_ACTOR_IS_SENTIENT)(Actor *theActor);
RelocAddr<GET_ACTOR_IS_SENTIENT>pGetActorIsSentient(0x00606850);

typedef  UInt32 (__fastcall *GET_ACTOR_SOUL_TYPE)(UInt32 level, UInt8 isSetient);
RelocAddr<GET_ACTOR_SOUL_TYPE>pGetActorSoulType(0x003C1740);

UInt32 CAHZActorInfo::IsSentient(Actor *actor)
{
	return pGetActorIsSentient(actor);
}

UInt32 CAHZActorInfo::GetSoulType(UInt16 actorLevel, UInt8 isActorSentient)
{
	UInt32 type = pGetActorSoulType(actorLevel, isActorSentient);
	return type;
}
