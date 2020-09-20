#pragma once

class CAHZActorData{
public:

	uint32_t IsSentient;
	uint16_t Level;
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

	static uint32_t IsSentient(RE::Actor* actor);
    static uint32_t GetSoulType(uint16_t actorLevel, uint8_t isActorSentient);
};

