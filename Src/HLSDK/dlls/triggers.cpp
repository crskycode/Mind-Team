#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "func_break.h"
#include "player.h"

#define SF_TRIGGER_PUSH_START_OFF			2
#define SF_TRIGGER_HURT_TARGETONCE			1
#define SF_TRIGGER_HURT_START_OFF			2
#define SF_TRIGGER_HURT_NO_CLIENTS			8
#define SF_TRIGGER_HURT_CLIENTONLYFIRE		16
#define SF_TRIGGER_HURT_CLIENTONLYTOUCH		32

extern void SetMovedir( entvars_t *pev );
extern Vector VecBModelOrigin( entvars_t *pevBModel );

class CFrictionModifier : public CBaseEntity
{
public:
	void Spawn( void );
	void KeyValue( KeyValueData *pkvd );
	void ChangeFriction( CBaseEntity *pOther );

	virtual int ObjectCaps( void ) { return CBaseEntity::ObjectCaps() & ~FCAP_ACROSS_TRANSITION; }

	float m_frictionFraction;
};

LINK_ENTITY_TO_CLASS( func_friction, CFrictionModifier );

void CFrictionModifier::Spawn( void )
{
	pev->solid = SOLID_TRIGGER;
	SET_MODEL( ENT( pev ), STRING( pev->model ) );
	pev->movetype = MOVETYPE_NONE;
	SetTouch( &CFrictionModifier::ChangeFriction );
}

void CFrictionModifier::ChangeFriction( CBaseEntity *pOther )
{
	if ( pOther->pev->movetype != MOVETYPE_BOUNCEMISSILE && pOther->pev->movetype != MOVETYPE_BOUNCE )
		pOther->pev->friction = m_frictionFraction;
}

void CFrictionModifier::KeyValue( KeyValueData *pkvd )
{
	if ( FStrEq( pkvd->szKeyName, "modifier" ) )
	{
		m_frictionFraction = atof( pkvd->szValue ) / 100.0;
		pkvd->fHandled = TRUE;
	}
	else
		CBaseEntity::KeyValue( pkvd );
}

#define SF_AUTO_FIREONCE 0x0001

class CAutoTrigger : public CBaseDelay
{
public:
	void KeyValue( KeyValueData *pkvd );
	void Spawn( void );
	void Precache( void );
	void Think( void );

	int ObjectCaps( void ) { return CBaseDelay::ObjectCaps() & ~FCAP_ACROSS_TRANSITION; }

private:
	int m_globalstate;
	USE_TYPE triggerType;
};

LINK_ENTITY_TO_CLASS( trigger_auto, CAutoTrigger );

void CAutoTrigger::KeyValue( KeyValueData *pkvd )
{
	if ( FStrEq( pkvd->szKeyName, "globalstate" ) )
	{
		m_globalstate = ALLOC_STRING( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else if ( FStrEq( pkvd->szKeyName, "triggerstate" ) )
	{
		int type = atoi( pkvd->szValue );

		switch ( type )
		{
			case 0:
				triggerType = USE_OFF;
				break;

			case 2:
				triggerType = USE_TOGGLE;
				break;

			default:
				triggerType = USE_ON;
				break;
		}

		pkvd->fHandled = TRUE;
	}
	else
		CBaseDelay::KeyValue( pkvd );
}

void CAutoTrigger::Spawn( void )
{
	Precache();
}

void CAutoTrigger::Precache( void )
{
	pev->nextthink = gpGlobals->time + 0.1;
}

void CAutoTrigger::Think( void )
{
	SUB_UseTargets( this, triggerType, 0 );

	if ( pev->spawnflags & SF_AUTO_FIREONCE )
		UTIL_Remove( this );
}

#define SF_RELAY_FIREONCE 0x0001

class CTriggerRelay : public CBaseDelay
{
public:
	void KeyValue( KeyValueData *pkvd );
	void Spawn( void );
	void Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );

	int ObjectCaps( void ) { return CBaseDelay::ObjectCaps() & ~FCAP_ACROSS_TRANSITION; }

private:
	USE_TYPE triggerType;
};

LINK_ENTITY_TO_CLASS( trigger_relay, CTriggerRelay );

void CTriggerRelay::KeyValue( KeyValueData *pkvd )
{
	if ( FStrEq( pkvd->szKeyName, "triggerstate" ) )
	{
		int type = atoi( pkvd->szValue );

		switch ( type )
		{
			case 0:
				triggerType = USE_OFF;
				break;

			case 2:
				triggerType = USE_TOGGLE;
				break;

			default:
				triggerType = USE_ON;
				break;
		}

		pkvd->fHandled = TRUE;
	}
	else
		CBaseDelay::KeyValue( pkvd );
}

void CTriggerRelay::Spawn( void )
{
}

void CTriggerRelay::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	SUB_UseTargets( this, triggerType, 0 );

	if ( pev->spawnflags & SF_RELAY_FIREONCE )
		UTIL_Remove( this );
}

#define SF_MULTIMAN_CLONE 0x80000000
#define SF_MULTIMAN_THREAD	0x00000001

class CMultiManager : public CBaseToggle
{
public:
	void KeyValue( KeyValueData *pkvd );
	void Spawn( void );
	void ManagerThink( void );
	void ManagerUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );

	BOOL HasTarget( string_t targetname );

	int ObjectCaps( void ) { return CBaseToggle::ObjectCaps() & ~FCAP_ACROSS_TRANSITION; }

	int m_cTargets;
	int m_index;
	float m_startTime;
	int m_iTargetName[MAX_MULTI_TARGETS];
	float m_flTargetDelay[MAX_MULTI_TARGETS];

private:
	inline BOOL IsClone( void ) { return ( pev->spawnflags & SF_MULTIMAN_CLONE ) ? TRUE : FALSE; }
	inline BOOL ShouldClone( void )
	{
		if ( IsClone() )
			return FALSE;

		return ( pev->spawnflags & SF_MULTIMAN_THREAD ) ? TRUE : FALSE;
	}

	CMultiManager *Clone( void );
};

LINK_ENTITY_TO_CLASS( multi_manager, CMultiManager );

void CMultiManager::KeyValue( KeyValueData *pkvd )
{
	if ( FStrEq( pkvd->szKeyName, "wait" ) )
	{
		m_flWait = atof( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else
	{
		if ( m_cTargets < MAX_MULTI_TARGETS )
		{
			char tmp[128];

			UTIL_StripToken( pkvd->szKeyName, tmp );
			m_iTargetName[m_cTargets] = ALLOC_STRING( tmp );
			m_flTargetDelay[m_cTargets] = atof( pkvd->szValue );
			m_cTargets++;
			pkvd->fHandled = TRUE;
		}
	}
}

void CMultiManager::Spawn( void )
{
	pev->solid = SOLID_NOT;
	SetUse( &CMultiManager::ManagerUse );
	SetThink( &CMultiManager::ManagerThink );

	int swapped = 1;

	while ( swapped )
	{
		swapped = 0;

		for ( int i = 1; i < m_cTargets; i++ )
		{
			if ( m_flTargetDelay[i] < m_flTargetDelay[i - 1] )
			{
				int name = m_iTargetName[i];
				float delay = m_flTargetDelay[i];
				m_iTargetName[i] = m_iTargetName[i - 1];
				m_flTargetDelay[i] = m_flTargetDelay[i - 1];
				m_iTargetName[i - 1] = name;
				m_flTargetDelay[i - 1] = delay;
				swapped = 1;
			}
		}
	}
}

BOOL CMultiManager::HasTarget( string_t targetname )
{
	for ( int i = 0; i < m_cTargets; i++ )
		if ( FStrEq( STRING( targetname ), STRING( m_iTargetName[i] ) ) )
			return TRUE;

	return FALSE;
}

void CMultiManager::ManagerThink( void )
{
	float time;

	time = gpGlobals->time - m_startTime;

	while ( m_index < m_cTargets && m_flTargetDelay[m_index] <= time )
	{
		FireTargets( STRING( m_iTargetName[m_index] ), m_hActivator, this, USE_TOGGLE, 0 );
		m_index++;
	}

	if ( m_index >= m_cTargets )
	{
		SetThink( NULL );

		if ( IsClone() )
		{
			UTIL_Remove( this );
			return;
		}

		SetUse( &CMultiManager::ManagerUse );
	}
	else
		pev->nextthink = m_startTime + m_flTargetDelay[m_index];
}

CMultiManager *CMultiManager::Clone( void )
{
	CMultiManager *pMulti = GetClassPtr( ( CMultiManager * )NULL );

	edict_t *pEdict = pMulti->pev->pContainingEntity;
	memcpy( pMulti->pev, pev, sizeof( *pev ) );
	pMulti->pev->pContainingEntity = pEdict;

	pMulti->pev->spawnflags |= SF_MULTIMAN_CLONE;
	pMulti->m_cTargets = m_cTargets;
	memcpy( pMulti->m_iTargetName, m_iTargetName, sizeof( m_iTargetName ) );
	memcpy( pMulti->m_flTargetDelay, m_flTargetDelay, sizeof( m_flTargetDelay ) );

	return pMulti;
}

void CMultiManager::ManagerUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	if ( ShouldClone() )
	{
		CMultiManager *pClone = Clone();
		pClone->ManagerUse( pActivator, pCaller, useType, value );
		return;
	}

	m_hActivator = pActivator;
	m_index = 0;
	m_startTime = gpGlobals->time;

	SetUse( NULL );

	SetThink( &CMultiManager::ManagerThink );
	pev->nextthink = gpGlobals->time;
}

#define SF_RENDER_MASKFX (1<<0)
#define SF_RENDER_MASKAMT (1<<1)
#define SF_RENDER_MASKMODE (1<<2)
#define SF_RENDER_MASKCOLOR (1<<3)

class CRenderFxManager : public CBaseEntity
{
public:
	void Spawn( void );
	void Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
};

LINK_ENTITY_TO_CLASS( env_render, CRenderFxManager );

void CRenderFxManager::Spawn( void )
{
	pev->solid = SOLID_NOT;
}

void CRenderFxManager::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	if ( !FStringNull( pev->target ) )
	{
		edict_t *pentTarget = NULL;

		while ( 1 )
		{
			pentTarget = FIND_ENTITY_BY_TARGETNAME( pentTarget, STRING( pev->target ) );

			if ( FNullEnt( pentTarget ) )
				break;

			entvars_t *pevTarget = VARS( pentTarget );

			if ( !FBitSet( pev->spawnflags, SF_RENDER_MASKFX ) )
				pevTarget->renderfx = pev->renderfx;

			if ( !FBitSet( pev->spawnflags, SF_RENDER_MASKAMT ) )
				pevTarget->renderamt = pev->renderamt;

			if ( !FBitSet( pev->spawnflags, SF_RENDER_MASKMODE ) )
				pevTarget->rendermode = pev->rendermode;

			if ( !FBitSet( pev->spawnflags, SF_RENDER_MASKCOLOR ) )
				pevTarget->rendercolor = pev->rendercolor;
		}
	}
}

class CBaseTrigger : public CBaseToggle
{
public:
	void TeleportTouch( CBaseEntity *pOther );
	void KeyValue( KeyValueData *pkvd );
	void MultiTouch( CBaseEntity *pOther );
	void HurtTouch( CBaseEntity *pOther );
	void CDAudioTouch( CBaseEntity *pOther );
	void ActivateMultiTrigger( CBaseEntity *pActivator );
	void MultiWaitOver( void );
	void CounterUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	void ToggleUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	void InitTrigger( void );

	virtual int ObjectCaps( void ) { return CBaseToggle::ObjectCaps() & ~FCAP_ACROSS_TRANSITION; }
};

LINK_ENTITY_TO_CLASS( trigger, CBaseTrigger );

void CBaseTrigger::InitTrigger()
{
	if ( pev->angles != g_vecZero )
		SetMovedir( pev );

	pev->solid = SOLID_TRIGGER;
	pev->movetype = MOVETYPE_NONE;
	SET_MODEL( ENT( pev ), STRING( pev->model ) );

	if ( CVAR_GET_FLOAT( "showtriggers" ) == 0 )
		SetBits( pev->effects, EF_NODRAW );
}

void CBaseTrigger::KeyValue( KeyValueData *pkvd )
{
	if ( FStrEq( pkvd->szKeyName, "damage" ) )
	{
		pev->dmg = atof( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else if ( FStrEq( pkvd->szKeyName, "count" ) )
	{
		m_cTriggersLeft = ( int )atof( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else if ( FStrEq( pkvd->szKeyName, "damagetype" ) )
	{
		m_bitsDamageInflict = atoi( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else
		CBaseToggle::KeyValue( pkvd );
}

class CTriggerHurt : public CBaseTrigger
{
public:
	void Spawn( void );
	void RadiationThink( void );
};

LINK_ENTITY_TO_CLASS( trigger_hurt, CTriggerHurt );

void CTriggerHurt::Spawn( void )
{
	InitTrigger();
	SetTouch( &CBaseTrigger::HurtTouch );

	if ( !FStringNull( pev->targetname ) )
		SetUse( &CBaseTrigger::ToggleUse );
	else
		SetUse( NULL );

	if ( m_bitsDamageInflict & DMG_RADIATION )
	{
		SetThink( &CTriggerHurt::RadiationThink );
		pev->nextthink = gpGlobals->time + RANDOM_FLOAT( 0.0, 0.5 );
	}

	if ( FBitSet( pev->spawnflags, SF_TRIGGER_HURT_START_OFF ) )
		pev->solid = SOLID_NOT;

	UTIL_SetOrigin( pev, pev->origin );
}

void CTriggerHurt::RadiationThink( void )
{
	edict_t *pentPlayer;
	CBasePlayer *pPlayer = NULL;
	float flRange;
	entvars_t *pevTarget;
	Vector vecSpot1;
	Vector vecSpot2;
	Vector vecRange;
	Vector origin;
	Vector view_ofs;

	origin = pev->origin;
	view_ofs = pev->view_ofs;

	pev->origin = ( pev->absmin + pev->absmax ) * 0.5;
	pev->view_ofs = pev->view_ofs * 0.0;

	pentPlayer = FIND_CLIENT_IN_PVS( edict() );

	pev->origin = origin;
	pev->view_ofs = view_ofs;

	if ( !FNullEnt( pentPlayer ) )
	{
		pPlayer = GetClassPtr( ( CBasePlayer * )VARS( pentPlayer ) );

		pevTarget = VARS( pentPlayer );

		vecSpot1 = ( pev->absmin + pev->absmax ) * 0.5;
		vecSpot2 = ( pevTarget->absmin + pevTarget->absmax ) * 0.5;

		vecRange = vecSpot1 - vecSpot2;
		flRange = vecRange.Length();
	}

	pev->nextthink = gpGlobals->time + 0.25;
}

void CBaseTrigger::ToggleUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	if ( pev->solid == SOLID_NOT )
	{
		pev->solid = SOLID_TRIGGER;
		gpGlobals->force_retouch++;
	}
	else
		pev->solid = SOLID_NOT;

	UTIL_SetOrigin( pev, pev->origin );
}

void CBaseTrigger::HurtTouch( CBaseEntity *pOther )
{
	float fldmg;

	if ( !pOther->pev->takedamage )
		return;

	if ( ( pev->spawnflags & SF_TRIGGER_HURT_CLIENTONLYTOUCH ) && !pOther->IsPlayer() )
		return;

	if ( ( pev->spawnflags & SF_TRIGGER_HURT_NO_CLIENTS ) && pOther->IsPlayer() )
		return;

	if ( pev->dmgtime > gpGlobals->time )
	{
		if ( gpGlobals->time != pev->pain_finished )
		{
			if ( pOther->IsPlayer() )
			{
				int playerMask = 1 << ( pOther->entindex() - 1 );

				if ( pev->impulse & playerMask )
					return;

				pev->impulse |= playerMask;
			}
			else
				return;
		}
	}
	else
	{
		pev->impulse = 0;

		if ( pOther->IsPlayer() )
		{
			int playerMask = 1 << ( pOther->entindex() - 1 );

			pev->impulse |= playerMask;
		}
	}


	fldmg = pev->dmg * 0.5;

	if ( fldmg < 0 )
		pOther->TakeHealth( -fldmg );
	else
		pOther->TakeDamage( pev, pev, fldmg, m_bitsDamageInflict );

	pev->pain_finished = gpGlobals->time;

	pev->dmgtime = gpGlobals->time + 0.5;

	if ( pev->target )
	{
		if ( pev->spawnflags & SF_TRIGGER_HURT_CLIENTONLYFIRE )
		{
			if ( !pOther->IsPlayer() )
				return;
		}

		SUB_UseTargets( pOther, USE_TOGGLE, 0 );

		if ( pev->spawnflags & SF_TRIGGER_HURT_TARGETONCE )
			pev->target = 0;
	}
}

class CTriggerMultiple : public CBaseTrigger
{
public:
	void Spawn( void );
};

LINK_ENTITY_TO_CLASS( trigger_multiple, CTriggerMultiple );

void CTriggerMultiple::Spawn( void )
{
	if ( m_flWait == 0 )
		m_flWait = 0.2;

	InitTrigger();

	SetTouch( &CBaseTrigger::MultiTouch );
}

class CTriggerOnce : public CTriggerMultiple
{
public:
	void Spawn( void );
};

LINK_ENTITY_TO_CLASS( trigger_once, CTriggerOnce );

void CTriggerOnce::Spawn( void )
{
	m_flWait = -1;
	CTriggerMultiple::Spawn();
}

void CBaseTrigger::MultiTouch( CBaseEntity *pOther )
{
	entvars_t *pevToucher;

	pevToucher = pOther->pev;

	if ( ( ( pevToucher->flags & FL_CLIENT ) && !( pev->spawnflags & SF_TRIGGER_NOCLIENTS ) ) ||
	     ( ( pevToucher->flags & FL_MONSTER ) && ( pev->spawnflags & SF_TRIGGER_ALLOWMONSTERS ) ) ||
	     ( pev->spawnflags & SF_TRIGGER_PUSHABLES ) && FClassnameIs( pevToucher, "func_pushable" ) )
		ActivateMultiTrigger( pOther );
}

void CBaseTrigger::ActivateMultiTrigger( CBaseEntity *pActivator )
{
	if ( pev->nextthink > gpGlobals->time )
		return;

	if ( !UTIL_IsMasterTriggered( m_sMaster, pActivator ) )
		return;

	if ( FClassnameIs( pev, "trigger_secret" ) )
	{
		if ( pev->enemy == NULL || !FClassnameIs( pev->enemy, "player" ) )
			return;

		gpGlobals->found_secrets++;
	}

	if ( !FStringNull( pev->noise ) )
		EMIT_SOUND( ENT( pev ), CHAN_VOICE, ( char * )STRING( pev->noise ), 1, ATTN_NORM );

	m_hActivator = pActivator;
	SUB_UseTargets( m_hActivator, USE_TOGGLE, 0 );

	if ( pev->message && pActivator->IsPlayer() )
	{
	}

	if ( m_flWait > 0 )
	{
		SetThink( &CBaseTrigger::MultiWaitOver );
		pev->nextthink = gpGlobals->time + m_flWait;
	}
	else
	{
		SetTouch( NULL );
		pev->nextthink = gpGlobals->time + 0.1;
		SetThink( &CBaseEntity::SUB_Remove );
	}
}

void CBaseTrigger::MultiWaitOver( void )
{
	SetThink( NULL );
}

void CBaseTrigger::CounterUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	m_cTriggersLeft--;
	m_hActivator = pActivator;

	if ( m_cTriggersLeft < 0 )
		return;

	BOOL fTellActivator =
	    ( m_hActivator != 0 ) &&
	    FClassnameIs( m_hActivator->pev, "player" ) &&
	    !FBitSet( pev->spawnflags, SPAWNFLAG_NOMESSAGE );

	if ( m_cTriggersLeft != 0 )
	{
		if ( fTellActivator )
		{
			switch ( m_cTriggersLeft )
			{
				case 1: ALERT( at_console, "Only 1 more to go..." ); break;

				case 2: ALERT( at_console, "Only 2 more to go..." ); break;

				case 3: ALERT( at_console, "Only 3 more to go..." ); break;

				default: ALERT( at_console, "There are more to go..." ); break;
			}
		}

		return;
	}

	if ( fTellActivator )
		ALERT( at_console, "Sequence completed!" );

	ActivateMultiTrigger( m_hActivator );
}

class CTriggerCounter : public CBaseTrigger
{
public:
	void Spawn( void );
};

LINK_ENTITY_TO_CLASS( trigger_counter, CTriggerCounter );

void CTriggerCounter::Spawn( void )
{
	m_flWait = -1;

	if ( m_cTriggersLeft == 0 )
		m_cTriggersLeft = 2;

	SetUse( &CBaseTrigger::CounterUse );
}

class CLadder : public CBaseTrigger
{
public:
	void KeyValue( KeyValueData *pkvd );
	void Spawn( void );
	void Precache( void );
};

LINK_ENTITY_TO_CLASS( func_ladder, CLadder );

void CLadder::KeyValue( KeyValueData *pkvd )
{
	CBaseTrigger::KeyValue( pkvd );
}

void CLadder::Precache( void )
{
	pev->solid = SOLID_NOT;
	pev->skin = CONTENTS_LADDER;

	if ( CVAR_GET_FLOAT( "showtriggers" ) == 0 )
	{
		pev->rendermode = kRenderTransTexture;
		pev->renderamt = 0;
	}

	pev->effects &= ~EF_NODRAW;
}

void CLadder::Spawn( void )
{
	Precache();

	SET_MODEL( ENT( pev ), STRING( pev->model ) );
	pev->movetype = MOVETYPE_PUSH;
}

class CTriggerPush : public CBaseTrigger
{
public:
	void Spawn( void );
	void KeyValue( KeyValueData *pkvd );
	void Touch( CBaseEntity *pOther );
};

LINK_ENTITY_TO_CLASS( trigger_push, CTriggerPush );

void CTriggerPush::KeyValue( KeyValueData *pkvd )
{
	CBaseTrigger::KeyValue( pkvd );
}

void CTriggerPush::Spawn( void )
{
	if ( pev->angles == g_vecZero )
		pev->angles.y = 360;

	InitTrigger();

	if ( pev->speed == 0 )
		pev->speed = 100;

	if ( FBitSet( pev->spawnflags, SF_TRIGGER_PUSH_START_OFF ) )
		pev->solid = SOLID_NOT;

	SetUse( &CBaseTrigger::ToggleUse );

	UTIL_SetOrigin( pev, pev->origin );
}

void CTriggerPush::Touch( CBaseEntity *pOther )
{
	entvars_t *pevToucher = pOther->pev;

	switch ( pevToucher->movetype )
	{
		case MOVETYPE_NONE:
		case MOVETYPE_PUSH:
		case MOVETYPE_NOCLIP:
		case MOVETYPE_FOLLOW:
			return;
	}

	if ( pevToucher->solid != SOLID_NOT && pevToucher->solid != SOLID_BSP )
	{
		if ( FBitSet( pev->spawnflags, SF_TRIG_PUSH_ONCE ) )
		{
			pevToucher->velocity = pevToucher->velocity + ( pev->movedir * pev->speed );

			if ( pevToucher->velocity.z > 0 )
				pevToucher->flags &= ~FL_ONGROUND;

			UTIL_Remove( this );
		}
		else
		{
			Vector vecPush = ( pev->movedir * pev->speed );

			if ( pevToucher->flags & FL_BASEVELOCITY )
				vecPush = vecPush + pevToucher->basevelocity;

			pevToucher->basevelocity = vecPush;

			pevToucher->flags |= FL_BASEVELOCITY;
		}
	}
}

void CBaseTrigger::TeleportTouch( CBaseEntity *pOther )
{
	entvars_t *pevToucher = pOther->pev;
	edict_t *pentTarget = NULL;

	if ( !FBitSet( pevToucher->flags, FL_CLIENT | FL_MONSTER ) )
		return;

	if ( !UTIL_IsMasterTriggered( m_sMaster, pOther ) )
		return;

	if ( !( pev->spawnflags & SF_TRIGGER_ALLOWMONSTERS ) )
	{
		if ( FBitSet( pevToucher->flags, FL_MONSTER ) )
			return;
	}

	if ( ( pev->spawnflags & SF_TRIGGER_NOCLIENTS ) )
	{
		if ( pOther->IsPlayer() )
			return;
	}

	pentTarget = FIND_ENTITY_BY_TARGETNAME( pentTarget, STRING( pev->target ) );

	if ( FNullEnt( pentTarget ) )
		return;

	Vector tmp = VARS( pentTarget )->origin;

	if ( pOther->IsPlayer() )
		tmp.z -= pOther->pev->mins.z;

	tmp.z++;
	pevToucher->flags &= ~FL_ONGROUND;

	UTIL_SetOrigin( pevToucher, tmp );
	pevToucher->angles = pentTarget->v.angles;

	if ( pOther->IsPlayer() )
		pevToucher->v_angle = pentTarget->v.angles;

	pevToucher->fixangle = TRUE;
	pevToucher->velocity = pevToucher->basevelocity = g_vecZero;
}

class CTriggerTeleport : public CBaseTrigger
{
public:
	void Spawn( void );
};

LINK_ENTITY_TO_CLASS( trigger_teleport, CTriggerTeleport );

void CTriggerTeleport::Spawn( void )
{
	InitTrigger();
	SetTouch( &CBaseTrigger::TeleportTouch );
}

LINK_ENTITY_TO_CLASS( info_teleport_destination, CPointEntity );

class CTriggerGravity : public CBaseTrigger
{
public:
	void Spawn( void );
	void GravityTouch( CBaseEntity *pOther );
};

LINK_ENTITY_TO_CLASS( trigger_gravity, CTriggerGravity );

void CTriggerGravity::Spawn( void )
{
	InitTrigger();
	SetTouch( &CTriggerGravity::GravityTouch );
}

void CTriggerGravity::GravityTouch( CBaseEntity *pOther )
{
	if ( !pOther->IsPlayer() )
		return;

	pOther->pev->gravity = pev->gravity;
}

class CTriggerChangeTarget : public CBaseDelay
{
public:
	void KeyValue( KeyValueData *pkvd );
	void Spawn( void );
	void Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );

	int ObjectCaps( void ) { return CBaseDelay::ObjectCaps() & ~FCAP_ACROSS_TRANSITION; }

private:
	int m_iszNewTarget;
};

LINK_ENTITY_TO_CLASS( trigger_changetarget, CTriggerChangeTarget );

void CTriggerChangeTarget::KeyValue( KeyValueData *pkvd )
{
	if ( FStrEq( pkvd->szKeyName, "m_iszNewTarget" ) )
	{
		m_iszNewTarget = ALLOC_STRING( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else
		CBaseDelay::KeyValue( pkvd );
}

void CTriggerChangeTarget::Spawn( void )
{
}

void CTriggerChangeTarget::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	CBaseEntity *pTarget = UTIL_FindEntityByString( NULL, "targetname", STRING( pev->target ) );

	if ( pTarget )
		pTarget->pev->target = m_iszNewTarget;
}

#define SF_CAMERA_PLAYER_POSITION 1
#define SF_CAMERA_PLAYER_TARGET 2
#define SF_CAMERA_PLAYER_TAKECONTROL 4

class CTriggerCamera : public CBaseDelay
{
public:
	void Spawn( void );
	void KeyValue( KeyValueData *pkvd );
	void Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	void FollowTarget( void );
	void Move( void );

	virtual int ObjectCaps( void ) { return CBaseEntity::ObjectCaps() & ~FCAP_ACROSS_TRANSITION; }

	EHANDLE m_hPlayer;
	EHANDLE m_hTarget;
	CBaseEntity *m_pentPath;
	int m_sPath;
	float m_flWait;
	float m_flReturnTime;
	float m_flStopTime;
	float m_moveDistance;
	float m_targetSpeed;
	float m_initialSpeed;
	float m_acceleration;
	float m_deceleration;
	int m_state;
};

LINK_ENTITY_TO_CLASS( trigger_camera, CTriggerCamera );

void CTriggerCamera::Spawn( void )
{
	pev->movetype = MOVETYPE_NOCLIP;
	pev->solid = SOLID_NOT;
	pev->renderamt = 0;
	pev->rendermode = kRenderTransTexture;

	m_initialSpeed = pev->speed;

	if ( m_acceleration == 0 )
		m_acceleration = 500;

	if ( m_deceleration == 0 )
		m_deceleration = 500;
}

void CTriggerCamera::KeyValue( KeyValueData *pkvd )
{
	if ( FStrEq( pkvd->szKeyName, "wait" ) )
	{
		m_flWait = atof( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else if ( FStrEq( pkvd->szKeyName, "moveto" ) )
	{
		m_sPath = ALLOC_STRING( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else if ( FStrEq( pkvd->szKeyName, "acceleration" ) )
	{
		m_acceleration = atof( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else if ( FStrEq( pkvd->szKeyName, "deceleration" ) )
	{
		m_deceleration = atof( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else
		CBaseDelay::KeyValue( pkvd );
}

void CTriggerCamera::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	if ( !ShouldToggle( useType, m_state ) )
		return;

	m_state = !m_state;

	if ( m_state == 0 )
	{
		m_flReturnTime = gpGlobals->time;
		return;
	}

	if ( !pActivator || !pActivator->IsPlayer() )
		pActivator = CBaseEntity::Instance( g_engfuncs.pfnPEntityOfEntIndex( 1 ) );

	m_hPlayer = pActivator;

	m_flReturnTime = gpGlobals->time + m_flWait;
	pev->speed = m_initialSpeed;
	m_targetSpeed = m_initialSpeed;

	if ( FBitSet( pev->spawnflags, SF_CAMERA_PLAYER_TARGET ) )
		m_hTarget = m_hPlayer;
	else
		m_hTarget = GetNextTarget();

	if ( m_hTarget == NULL )
		return;

	if ( FBitSet( pev->spawnflags, SF_CAMERA_PLAYER_TAKECONTROL ) )
		( ( CBasePlayer * )pActivator )->EnableControl( FALSE );

	if ( m_sPath )
		m_pentPath = Instance( FIND_ENTITY_BY_TARGETNAME( NULL, STRING( m_sPath ) ) );
	else
		m_pentPath = NULL;

	m_flStopTime = gpGlobals->time;

	if ( m_pentPath )
	{
		if ( m_pentPath->pev->speed != 0 )
			m_targetSpeed = m_pentPath->pev->speed;

		m_flStopTime += m_pentPath->GetDelay();
	}

	if ( FBitSet( pev->spawnflags, SF_CAMERA_PLAYER_POSITION ) )
	{
		UTIL_SetOrigin( pev, pActivator->pev->origin + pActivator->pev->view_ofs );
		pev->angles.x = -pActivator->pev->angles.x;
		pev->angles.y = pActivator->pev->angles.y;
		pev->angles.z = 0;
		pev->velocity = pActivator->pev->velocity;
	}
	else
		pev->velocity = Vector( 0, 0, 0 );

	SET_VIEW( pActivator->edict(), edict() );

	SET_MODEL( ENT( pev ), STRING( pActivator->pev->model ) );

	SetThink( &CTriggerCamera::FollowTarget );
	pev->nextthink = gpGlobals->time;

	m_moveDistance = 0;
	Move();
}

void CTriggerCamera::FollowTarget( void )
{
	if ( m_hPlayer == NULL )
		return;

	if ( m_hTarget == NULL || m_flReturnTime < gpGlobals->time )
	{
		if ( m_hPlayer->IsAlive() )
		{
			SET_VIEW( m_hPlayer->edict(), m_hPlayer->edict() );
			( ( CBasePlayer * )( ( CBaseEntity * )m_hPlayer ) )->EnableControl( TRUE );
		}

		SUB_UseTargets( this, USE_TOGGLE, 0 );
		pev->avelocity = Vector( 0, 0, 0 );
		m_state = 0;
		return;
	}

	Vector vecGoal = UTIL_VecToAngles( m_hTarget->pev->origin - pev->origin );
	vecGoal.x = -vecGoal.x;

	if ( pev->angles.y > 360 )
		pev->angles.y -= 360;

	if ( pev->angles.y < 0 )
		pev->angles.y += 360;

	float dx = vecGoal.x - pev->angles.x;
	float dy = vecGoal.y - pev->angles.y;

	if ( dx < -180 )
		dx += 360;

	if ( dx > 180 )
		dx = dx - 360;

	if ( dy < -180 )
		dy += 360;

	if ( dy > 180 )
		dy = dy - 360;

	pev->avelocity.x = dx * 40 * gpGlobals->frametime;
	pev->avelocity.y = dy * 40 * gpGlobals->frametime;

	if ( !( FBitSet( pev->spawnflags, SF_CAMERA_PLAYER_TAKECONTROL ) ) )
	{
		pev->velocity = pev->velocity * 0.8;

		if ( pev->velocity.Length() < 10.0 )
			pev->velocity = g_vecZero;
	}

	pev->nextthink = gpGlobals->time;

	Move();
}

void CTriggerCamera::Move( void )
{
	if ( !m_pentPath )
		return;

	m_moveDistance -= pev->speed * gpGlobals->frametime;

	if ( m_moveDistance <= 0 )
	{
		if ( m_pentPath->pev->message )
		{
			FireTargets( STRING( m_pentPath->pev->message ), this, this, USE_TOGGLE, 0 );
		}

		m_pentPath = m_pentPath->GetNextTarget();

		if ( !m_pentPath )
			pev->velocity = g_vecZero;
		else
		{
			if ( m_pentPath->pev->speed != 0 )
				m_targetSpeed = m_pentPath->pev->speed;

			Vector delta = m_pentPath->pev->origin - pev->origin;
			m_moveDistance = delta.Length();
			pev->movedir = delta.Normalize();
			m_flStopTime = gpGlobals->time + m_pentPath->GetDelay();
		}
	}

	if ( m_flStopTime > gpGlobals->time )
		pev->speed = UTIL_Approach( 0, pev->speed, m_deceleration * gpGlobals->frametime );
	else
		pev->speed = UTIL_Approach( m_targetSpeed, pev->speed, m_acceleration * gpGlobals->frametime );

	float fraction = 2 * gpGlobals->frametime;
	pev->velocity = ( ( pev->movedir * pev->speed ) * fraction ) + ( pev->velocity * ( 1 - fraction ) );
}