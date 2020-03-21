#pragma once

#include "skse64/GameReferences.h"

class CAHZActorData{
public:

	UInt32 IsSentient;
	UInt16 Level;
	float maxHealth;
	float health;
	float maxMagicka;
	float magicka;
	float maxStamina;
	float stamina;
	bool targetChanged;
	
	CAHZActorData()
	{
		maxHealth =0;
		health = 0;
		maxMagicka = 0;
		magicka = 0;
		maxStamina = 0;
		stamina = 0;
		IsSentient = 0;
		Level = 0;
		targetChanged = false;
	};

	CAHZActorData(const CAHZActorData & data)
	{
		maxHealth = data.maxHealth;
		health = data.health;
		maxMagicka = data.maxMagicka;
		magicka = data.magicka;
		maxStamina = data.maxStamina;
		stamina = data.stamina;
		IsSentient = data.IsSentient;
		Level = data.Level;
		targetChanged = data.targetChanged;
	};
};

class CAHZActorInfo
{
public:
	CAHZActorInfo(void);
	~CAHZActorInfo(void);

	static UInt32 IsSentient(Actor *actor);
	static UInt32 GetSoulType(UInt16 actorLevel, UInt8 isActorSentient);
};

