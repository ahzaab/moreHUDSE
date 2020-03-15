#pragma once

#include "skse64/GameReferences.h"

struct CAHZActorData{
	UInt32 IsSentient;
	UInt16 Level;
};

class CAHZActorInfo
{
public:
	CAHZActorInfo(void);
	~CAHZActorInfo(void);

	static UInt32 IsSentient(Actor *actor);
	static UInt32 GetSoulType(UInt16 actorLevel, UInt8 isActorSentient);
};

