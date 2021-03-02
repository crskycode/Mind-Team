/***
 * 
 * Copyright (c) 1996-2002, Valve LLC. All rights reserved.
 * 
 * This product contains software technology licensed from Id
 * Software, Inc. ("Id Technology"). Id Technology (c) 1996 Id Software, Inc.
 * All Rights Reserved.
 * 
 * Use, distribution, and modification of this source code and/or resulting
 * object code is restricted to non-commercial enhancements to products from
 * Valve LLC. All other use, distribution, or modification is prohibited
 * without written permission from Valve LLC.
 * 
***/

#ifndef CBASE_H
#define CBASE_H

#define FCAP_CUSTOMSAVE			0x00000001
#define FCAP_ACROSS_TRANSITION	0x00000002
#define FCAP_MUST_SPAWN			0x00000004
#define FCAP_DONT_SAVE			0x80000000
#define FCAP_IMPULSE_USE		0x00000008
#define FCAP_CONTINUOUS_USE		0x00000010
#define FCAP_ONOFF_USE			0x00000020
#define FCAP_DIRECTIONAL_USE	0x00000040
#define FCAP_MASTER				0x00000080
#define FCAP_FORCE_TRANSITION	0x00000080

int DispatchSpawn(edict_t *pent);
void DispatchThink(edict_t *pent);
void DispatchUse(edict_t *pentUsed, edict_t *pentOther);
void DispatchTouch(edict_t *pentTouched, edict_t *pentOther);
void DispatchBlocked(edict_t *pfnBlocked, edict_t *pentOther);
void DispatchKeyValue(edict_t *pentKeyvalue, KeyValueData *pkvd);
void DispatchSave(edict_t *pent, SAVERESTOREDATA *pSaveData);
int DispatchRestore(edict_t *pent, SAVERESTOREDATA *pSaveData, int globalEntity);
void DispatchSetAbsBox(edict_t *pent);

void SaveWriteFields(SAVERESTOREDATA *pSaveData, const char *pname, void *pBaseData, TYPEDESCRIPTION *pFields, int fieldCount);
void SaveReadFields(SAVERESTOREDATA *pSaveData, const char *pname, void *pBaseData, TYPEDESCRIPTION *pFields, int fieldCount);

void SaveGlobalState(SAVERESTOREDATA *pSaveData);
void RestoreGlobalState(SAVERESTOREDATA *pSaveData);
void ResetGlobalState(void);

void OnFreeEntPrivateData(edict_t *pEnt);
int ShouldCollide(edict_t *pentTouched, edict_t *pentOther);

enum USE_TYPE
{
	USE_OFF,
	USE_ON,
	USE_SET,
	USE_TOGGLE,
};

class CBaseEntity;
class CBasePlayerWeapon;

/*******************************

CBaseEntity
	|-CAmbientGeneric
	|-CBaseDelay
	| 	|-CAutoTrigger
	| 	|-CBaseAnimating
	| 	| 	|-CBasePlayer
	| 	| 	|-CBaseToggle
	| 	| 	| 	|-CBaseButton
	| 	| 	| 	| 	|-CRotButton
	| 	| 	| 	|-CBaseDoor
	| 	| 	| 	| 	|-CRotDoor
	| 	| 	| 	|-CBaseTrigger
	| 	| 	| 	| 	|-CLadder
	| 	| 	| 	| 	|-CTriggerCounter
	| 	| 	| 	| 	|-CTriggerGravity
	| 	| 	| 	| 	|-CTriggerHurt
	| 	| 	| 	| 	|-CTriggerMultiple
	| 	| 	| 	| 	| 	|-CTriggerOnce
	| 	| 	| 	| 	|-CTriggerPush
	| 	| 	| 	| 	|-CTriggerTeleport
	| 	| 	| 	|-CFuncIllusionary
	| 	| 	| 	|-CMomentaryDoor
	| 	| 	| 	|-CMomentaryRotButton
	| 	| 	| 	|-CMultiManager
	| 	| 	|-CCycler
	| 	| 	| 	|-CGenericCycler
	| 	| 	|-CWreckage
	| 	|-CBreakable
	| 	| 	|-CPushable
	| 	|-CTriggerCamera
	| 	|-CTriggerChangeTarget
	| 	|-CTriggerRelay
	|-CBasePlayerWeapon
	| 	|-CHEGrenade
	| 	|-CKnife
	| 	|-CPistol
	| 	|-CRifle
	| 	|-CShotgun
	| 	|-CSniper
	|-CBaseSpectator
	|-CBeam
	| 	|-CLaser
	| 	|-CLightning
	|-CBubbling
	|-CButtonTarget
	|-CCorpse
	|-CCyclerSprite
	|-CDecal
	|-CEnvExplosion
	|-CEnvSpark
	|-CFrictionModifier
	|-CFuncRotating
	|-CFuncWall
	| 	|-CFuncConveyor
	| 	|-CFuncMonsterClip
	| 	|-CFuncWallToggle
	|-CPendulum
	|-CPointEntity
	| 	|-CEnvSound
	| 	|-CFade
	| 	|-CGlow
	| 	|-CLight
	| 	| 	|-CEnvLight
	| 	|-CMultiSource
	| 	|-CShake
	| 	|-CSprite
	|-CRenderFxManager
	|-CWeaponBox
	|-CWorld

*******************************/

class CBaseEntity
{
public:
	CBaseEntity() { }
	virtual ~CBaseEntity() { }	// must call destructor in OnFreeEntPrivateData

	virtual void Spawn(void);
	virtual void Think(void);
	virtual void Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);
	virtual void Touch(CBaseEntity *pOther);
	virtual void Blocked(CBaseEntity *pOther);
	virtual void KeyValue(KeyValueData *pkvd);
	virtual void SetAbsBox(void);
	virtual bool Collide(CBaseEntity *pOther);

	virtual void Precache(void);
	virtual int ObjectCaps(void);

	virtual void TakeHealth(float flHealth);
	virtual void TakeDamage(entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType);
	virtual void Killed(entvars_t *pevInflictor, entvars_t *pevAttacker);
	virtual void TraceAttack(entvars_t *pevAttacker, float flDamage, Vector &vecDir, TraceResult *ptr, int bitsDamageType);

	virtual float GetDelay(void);

	virtual BOOL IsTriggered(CBaseEntity *pActivator);
	virtual BOOL HasTarget(string_t targetname);

	virtual bool FVisible(CBaseEntity *pEntity) const;
	virtual bool FVisible(const Vector &vecOrigin) const;

	virtual Vector EyePosition(void) const;

	virtual CBaseEntity *GetNextTarget(void);

	virtual int ShouldToggle(USE_TYPE useType, BOOL currentState);

	virtual bool IsAlive(void) const;
	virtual bool IsBSPModel(void) const;
	virtual bool IsInWorld(void) const;
	virtual bool IsPlayer(void) const;
	virtual bool IsNetClient(void) const;

	virtual void WorldSpaceAABB(Vector *pWorldMins, Vector *pWorldMaxs) const;
	virtual Vector &GetAbsOrigin(void) const { return pev->origin; }
	virtual Vector Center(void) const;
	virtual Vector WorldSpaceCenter(void) const { return Center(); }
	virtual Vector &GetAbsVelocity(void) const { return pev->velocity; }

	virtual Vector &WorldAlignMins(void) const { return pev->absmin; }
	virtual Vector &WorldAlignMaxs(void) const { return pev->absmax; }

	virtual Vector &GetPunchAngle(void) const { return pev->punchangle; }

	virtual const char *GetClassname(void) const { return STRING( pev->classname ); }

	virtual void ClearFlags(void) { pev->flags = 0; }
	virtual void AddFlag( unsigned int flags ) { pev->flags |= flags; }
	virtual unsigned int GetFlags(void) const { return pev->flags; }

	virtual float MaxSpeed(void) const { return pev->maxspeed; }

	edict_t *edict(void) const;
	int entindex(void) const;

	void SUB_Remove(void);
	void SUB_DoNothing(void);
	void SUB_StartFadeOut(void);
	void SUB_FadeOut(void);
	void SUB_CallUseToggle(void);

	void SUB_UseTargets(CBaseEntity *pActivator, USE_TYPE useType, float value);

	inline void *operator new(size_t size, entvars_t *pev)
	{
		return (void *)ALLOC_PRIVATE(ENT(pev), size);
	}
	inline void operator delete(void *memory, entvars_t *pev)
	{
		pev->flags |= FL_KILLME;
	}

	static CBaseEntity *Create(const char *szName, Vector &vecOrigin, Vector &vecAngles, edict_t *pentOwner = NULL);

	static CBaseEntity *Instance(const edict_t *pent);
	static CBaseEntity *Instance(const entvars_t *pev);

public:
	entvars_t *pev;

	void (CBaseEntity::*m_pfnThink)(void);
	void (CBaseEntity::*m_pfnUse)(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);
	void (CBaseEntity::*m_pfnTouch)(CBaseEntity *pOther);
	void (CBaseEntity::*m_pfnBlocked)(CBaseEntity *pOther);
	bool (CBaseEntity::*m_pfnCollide)(CBaseEntity *pOther);
};

#define SetThink(a) m_pfnThink = static_cast<void (CBaseEntity::*)(void)>(a)
#define SetUse(a) m_pfnUse = static_cast<void (CBaseEntity::*)(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)>(a)
#define SetTouch(a) m_pfnTouch = static_cast<void (CBaseEntity::*)(CBaseEntity *pOther)>(a)
#define SetBlocked(a) m_pfnBlocked = static_cast<void (CBaseEntity::*)(CBaseEntity *pOther)>(a)
#define SetCollide(a) m_pfnCollide = static_cast<bool (CBaseEntity::*)(CBaseEntity *pOther)>(a)

class EHANDLE
{
public:
	inline edict_t *Get(void)
	{
		if (m_pEntity)
		{
			if (m_pEntity->serialnumber == m_iSerialNumber)
			{
				return m_pEntity;
			}
		}

		return NULL;
	}

	inline void Set(edict_t *pEntity)
	{
		m_pEntity = pEntity;

		if (m_pEntity)
		{
			m_iSerialNumber = m_pEntity->serialnumber;
		}
	}

	inline operator int ( )
	{
		return Get() ? 1 : 0;
	}

	inline operator CBaseEntity * ( )
	{
		return CBaseEntity::Instance(Get());
	}

	inline operator CBasePlayer * ( )
	{
		CBaseEntity *pEntity = CBaseEntity::Instance( Get() );
		if ( pEntity && pEntity->IsPlayer() )
			return (CBasePlayer *)pEntity;
		return NULL;
	}

	//inline CBaseEntity *operator=(CBaseEntity *pEntity)
	//{
	//	if (pEntity)
	//	{
	//		m_pEntity = pEntity->edict();

	//		if (m_pEntity)
	//			m_iSerialNumber = m_pEntity->serialnumber;
	//	}
	//	else
	//	{
	//		m_pEntity = NULL;
	//		m_iSerialNumber = 0;
	//	}
	inline void operator = ( CBaseEntity *pEntity )
	{
		if (pEntity)
		{
			m_pEntity = pEntity->edict();

			if (pEntity)
			{
				m_iSerialNumber = m_pEntity->serialnumber;
				return;
			}
		}

		m_pEntity = 0;
		m_iSerialNumber = 0;
	}

	//	return pEntity;
	//}

	inline CBaseEntity * operator -> ( )
	{
		return CBaseEntity::Instance(Get());
	}

private:
	edict_t *m_pEntity;
	int m_iSerialNumber;
};

class CPointEntity: public CBaseEntity
{
public:
	virtual void Spawn(void);
};

void FireTargets(const char *targetName, CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);

typedef struct locksounds
{
	string_t sLockedSound;
	string_t sUnlockedSound;

	float flwaitSound;
}
locksound_t;

#define MAX_MULTI_TARGETS	16
#define MS_MAX_TARGETS	32

class CMultiSource : public CPointEntity
{
public:
	virtual void Spawn(void);
	virtual void KeyValue(KeyValueData *pkvd);
	virtual void Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);
	virtual BOOL IsTriggered(CBaseEntity *pActivator);

	virtual int ObjectCaps(void)
	{
		return FCAP_MASTER;
	}

	void Register(void);

	EHANDLE m_rgEntities[MS_MAX_TARGETS];
	int m_rgTriggered[MS_MAX_TARGETS];

	int m_iTotal;
	string_t m_globalstate;
};

class CBaseDelay : public CBaseEntity
{
public:
	virtual void KeyValue(KeyValueData *pkvd);
	virtual void SUB_UseTargets(CBaseEntity *pActivator, USE_TYPE useType, float value);

	void DelayThink(void);

	float m_flDelay;
	int m_iszKillTarget;
};

class CBaseAnimating : public CBaseDelay
{
public:
	virtual float StudioFrameAdvance(float flInterval = 0.0);
	virtual int GetSequenceFlags(void);
	virtual int LookupActivity(int activity);
	virtual int LookupSequence(const char *label);
	virtual void ResetSequenceInfo(void);
	virtual float SetBoneController(int iController, float flValue);
	virtual void InitBoneControllers(void);
	virtual float SetBlending(int iBlender, float flValue);
	virtual void GetBonePosition(int iBone, Vector &origin, Vector &angles);
	virtual void GetAttachment(int iAttachment, Vector &origin, Vector &angles);
	virtual void SetBodygroup(int iGroup, int iValue);
	virtual int GetBodygroup(int iGroup);

	float m_flFrameRate;
	float m_flGroundSpeed;
	float m_flLastEventCheck;
	BOOL m_fSequenceFinished;
	BOOL m_fSequenceLoops;
};

#define SF_ITEM_USE_ONLY	(1<<8)

class CBaseToggle : public CBaseAnimating
{
public:
	virtual void KeyValue(KeyValueData *pkvd);

	float GetDelay(void);

	void LinearMove(Vector vecDest, float flSpeed);
	void LinearMoveDone(void);
	void AngularMove(Vector vecDestAngle, float flSpeed);
	void AngularMoveDone(void);

	static float AxisValue(int flags, Vector &angles);
	static void AxisDir(entvars_t *pev);
	static float AxisDelta(int flags, Vector &angle1, Vector &angle2);

	TOGGLE_STATE m_toggle_state;
	float m_flActivateFinished;
	float m_flMoveDistance;
	float m_flWait;
	float m_flLip;
	float m_flTWidth;
	float m_flTLength;

	Vector m_vecPosition1;
	Vector m_vecPosition2;
	Vector m_vecAngle1;
	Vector m_vecAngle2;

	int m_cTriggersLeft;
	float m_flHeight;
	EHANDLE m_hActivator;
	Vector m_vecFinalDest;
	Vector m_vecFinalAngle;

	int m_bitsDamageInflict;

	void (CBaseToggle::*m_pfnCallWhenMoveDone)(void);

	string_t m_sMaster;
};

#define SetMoveDone(a) m_pfnCallWhenMoveDone = static_cast<void (CBaseToggle::*)(void)>(a)

#define DMG_GENERIC			0x00000000
#define DMG_CRUSH			0x00000001
#define DMG_BULLET			0x00000002
#define DMG_SLASH			0x00000008
#define DMG_BURN			0x00000010
#define DMG_FREEZE			0x00000020
#define DMG_FALL			0x00000040
#define DMG_BLAST			0x00000080
#define DMG_CLUB			0x00000100
#define DMG_SHOCK			0x00000200
#define DMG_SONIC			0x00000400
#define DMG_ENERGYBEAM		0x00000800
#define DMG_NEVERGIB		0x00001000
#define DMG_ALWAYSGIB		0x00002000
#define DMG_DROWN			0x00004000
#define DMG_PARALYZE		0x00008000
#define DMG_NERVEGAS		0x00010000
#define DMG_POISON			0x00020000
#define DMG_RADIATION		0x00040000
#define DMG_DROWNRECOVER	0x00080000
#define DMG_ACID			0x00100000
#define DMG_SLOWBURN		0x00200000
#define DMG_SLOWFREEZE		0x00400000
#define DMG_MORTAR			0x00800000
#define DMG_KNIFE			0x01000000

#define DMG_SHOWNHUD (DMG_POISON | DMG_ACID | DMG_FREEZE | DMG_SLOWFREEZE | DMG_DROWN | DMG_BURN | DMG_SLOWBURN | DMG_NERVEGAS | DMG_RADIATION | DMG_SHOCK)

char *ButtonSound(int sound);

class CBaseButton : public CBaseToggle
{
public:
	virtual void Spawn(void);
	virtual void Precache(void);
	virtual void KeyValue(KeyValueData *pkvd);

	virtual int ObjectCaps(void)
	{
		return (CBaseToggle:: ObjectCaps() & ~FCAP_ACROSS_TRANSITION) | (pev->takedamage ? 0 : FCAP_IMPULSE_USE);
	}

	void RotSpawn(void);

	void ButtonActivate(void);

	void ButtonShot(void);
	void ButtonTouch(CBaseEntity *pOther);
	void ButtonSpark(void);
	void TriggerAndWait(void);
	void ButtonReturn(void);
	void ButtonBackHome(void);
	void ButtonUse(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);

	virtual void TakeDamage(entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType);

	enum BUTTON_CODE
	{
		BUTTON_NOTHING,
		BUTTON_ACTIVATE,
		BUTTON_RETURN,
	};

	BUTTON_CODE ButtonResponseToTouch(void);

	BOOL m_fStayPushed;
	BOOL m_fRotating;

	string_t m_strChangeTarget;

	locksound_t m_ls;

	BYTE m_bLockedSound;
	BYTE m_bUnlockedSound;

	int m_sounds;
};

template <class T> T *GetClassPtr(T *a)
{
	entvars_t *pev = (entvars_t *)a;

	if (!pev)
		pev = VARS(CREATE_ENTITY());

	a = (T *)GET_PRIVATE(ENT(pev));

	if (!a)
	{
		a = new(pev) T;
		a->pev = pev;
	}

	return a;
}

class CWorld : public CBaseEntity
{
public:
	virtual void Spawn(void);
	virtual void Precache(void);
	virtual void KeyValue(KeyValueData *pkvd);
};

#endif