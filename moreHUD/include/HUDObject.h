// 18
class HUDObject
{
public:
	virtual ~HUDObject();

	virtual void Unk_01(void) = 0;
	virtual void Unk_02(void);
	virtual void Unk_03(void);
	virtual void Unk_04(void);

	UInt32			unk04;		// 04
	GFxMovieView	* view;		// 08
	UInt32			unk0C;		// 0C
	GFxValue		unk10;		// 10
};
STATIC_ASSERT(sizeof(HUDObject) == 0x20);

// 30
class Compass : public HUDObject
{
public:
	virtual void Unk_01(void);

	UInt32	unk20;	// 20
	UInt32	unk24;	// 24
	UInt32	unk28;	// 28
	UInt32	unk2C;	// 2C
};
STATIC_ASSERT(sizeof(Compass) == 0x30);

// A0
class FloatingQuestMarker : public HUDObject
{
public:

};

// 58
class HUDNotifications : public HUDObject
{
public:

};

// 68
class EnemyHealth : public HUDObject
{
public:
	UInt32			handle;			// 20
	UInt32			unk24;			// 24
	UInt32			unk28;			// 28
	UInt32			unk2C;			// 2C
	GFxValue		unk30;			// 30
	GFxValue		unk40;			// 40
	GFxValue		text;			// 50
	UInt32			unk5C;			// 5C
	UInt32			unk60;			// 60
	UInt32			unk64;			// 64

	TESObjectREFR	* GetTarget() const;
};
STATIC_ASSERT(offsetof(EnemyHealth, handle) == 0x20);

// 70
class StealthMeter : public HUDObject
{
public:

};

// 28
class HUDChargeMeter : public HUDObject
{
public:

};

// 38?
class HUDMeter : public HUDObject
{
public:
	virtual void Unk_01(void);
	virtual void Unk_02(void);
	virtual void Unk_03(void);
	virtual void Unk_05(void);
	
	char	* setMeterPercent;	// 20
	char	* startBlinking;	// 24
	char	* fadeOut;			// 28
	float	unk28;				// 2C
	UInt32	unk2C;				// 30
	UInt32	unk34;				// 34
};
STATIC_ASSERT(sizeof(HUDMeter) == 0x38);

// 38
class ActorValueMeter : public HUDMeter
{
public:
	
};

// 38
class ShoutMeter : public HUDMeter
{
public:
	
};

// 58
class HUDMenu : public IMenu
{
public:
	BSTEventSink<void>	unk1C;	// UserEventEnabledEvent
	tArray<HUDObject*>	hudComponents;	// 20
	UInt32	unk2C;
	UInt32	unk30;
	UInt32	unk34;
	UInt32	unk38;
	UInt32	unk3C;
	UInt32	unk40;
	UInt32	unk44;
	UInt32	unk48;
	UInt32	unk4C;
	UInt32	unk50;
	UInt32	unk54;
};
STATIC_ASSERT(sizeof(HUDMenu) == 0x58);












TESObjectREFR * EnemyHealth::GetTarget() const
{
	TESObjectREFR * refr = NULL;
	UInt32 refHandle = (*g_thePlayer)->targetHandle;
	LookupREFRByHandle(&refHandle, &refr);
	if(!refr) {
		refHandle = handle;
		LookupREFRByHandle(&refHandle, &refr);
	}

	return refr;
}















// 720
class PlayerCharacter : public Character
{
public:
	virtual ~PlayerCharacter();

	// parents
	BSTEventSink <void *>	menuOpenCloseEvent;			// 19C .?AV?$BSTEventSink@VMenuOpenCloseEvent@@@@
	BSTEventSink <void *>	menuModeChangeEvent;		// 1A0 .?AV?$BSTEventSink@VMenuModeChangeEvent@@@@
	BSTEventSink <void *>	userEventEnabledEvent;		// 1A4 .?AV?$BSTEventSink@VUserEventEnabledEvent@@@@
	BSTEventSource <void *>	actorCellEventSource;		// 1A8 .?AV?$BSTEventSource@UBGSActorCellEvent@@@@
	BSTEventSource <void *>	actorDeathEventSource;		// 1D8 .?AV?$BSTEventSource@UBGSActorDeathEvent@@@@
	BSTEventSource <void *>	positionPlayerEventSource;	// 208 .?AV?$BSTEventSource@UPositionPlayerEvent@@@@

	UInt32	pad238[(0x490 - 0x238) >> 2];	// 238
	UInt32	unk490;							// 490 - Handle
	UInt32	pad494[(0x568 - 0x494) >> 2];	// 494
	UInt32	unk568;							// 568 - Handle
	UInt32	pad56C[(0x58C - 0x56C) >> 2];	// 56C
	NiNode	* firstPersonSkeleton;			// 58C
	UInt32	pad590[(0x5AC - 0x590) >> 2];
	UInt32	lastRiddenHorseHandle;			// 5AC - Handle
	UInt32	pad5B0[(0x614 - 0x5B0) >> 2];
	PlayerSkills *	skills;					// 618
	UInt32	targetHandle;					// 61C
	UInt32	pad618[(0x644 - 0x61C) >> 2];
	TESForm	* tempPoison;					// 644
	UInt32	numTeammates;					// 648
	UInt32	pad64C[(0x6E0 - 0x64C) >> 2];
	UInt8	unk6E0;							// 6E0
	UInt8	numPerkPoints;					// 6E1
	UInt16  unk6E2;							// 6E2
	UInt32	unk6E4;							// 6E4

	tArray <TintMask *>	tintMasks;			// 6E8		// These are the actual tints
	tArray <TintMask *>	* overlayTintMasks;	// 6F4		// These apply when overlay head parts is enabled

	// Overlayed tints should be the same as original tints
	// occasionally they can have no type so index matching
	// is required to set anything on the tint
	TintMask * GetOverlayTintMask(TintMask * original);

	// Confirmed - Same as ExtraContainerChanges::EntryData
	// This type is used by scaleform to extend data
	// It can be used to extend more of the "ExtraData"
	struct ObjDesc
	{
		TESForm					* form;
		tList<BaseExtraList>	* extraData;
		SInt32					countDelta;

		MEMBER_FN_PREFIX(ObjDesc);
		DEFINE_MEMBER_FN(GenerateName, const char *, 0x00475AA0);
	};

	MEMBER_FN_PREFIX(PlayerCharacter);
	DEFINE_MEMBER_FN(GetTintList, tArray <TintMask *> *, 0x0055FF90);
	DEFINE_MEMBER_FN(GetNumTints, UInt32, 0x00735F40, UInt32 tintType);
	DEFINE_MEMBER_FN(GetTintMask, TintMask *, 0x00735F00, UInt32 tintType, UInt32 index);
	DEFINE_MEMBER_FN(GetDamage, double, 0x00730810, ObjDesc * pForm);
	DEFINE_MEMBER_FN(GetArmorValue, double, 0x007307E0, ObjDesc * pForm);
};