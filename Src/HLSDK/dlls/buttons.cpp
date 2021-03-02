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

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "doors.h"


#define SF_BUTTON_DONTMOVE		1
#define SF_ROTBUTTON_NOTSOLID	1
#define SF_BUTTON_TOGGLE		32		// Button stays pushed until reactivated.
#define SF_BUTTON_SPARK_IF_OFF	64		// Button sparks in OFF state.
#define SF_BUTTON_TOUCH_ONLY	256		// Button only fires as a result of USE key.

#define SF_GLOBAL_SET 1		// Set global state to initial state on spawn.

LINK_ENTITY_TO_CLASS( multisource, CMultiSource );

void CMultiSource::KeyValue( KeyValueData *pkvd )
{
	if ( FStrEq( pkvd->szKeyName, "style" ) ||
	     FStrEq( pkvd->szKeyName, "height" ) ||
	     FStrEq( pkvd->szKeyName, "killtarget" ) ||
	     FStrEq( pkvd->szKeyName, "value1" ) ||
	     FStrEq( pkvd->szKeyName, "value2" ) ||
	     FStrEq( pkvd->szKeyName, "value3" ) )
		pkvd->fHandled = TRUE;

	else if ( FStrEq( pkvd->szKeyName, "globalstate" ) )
	{
		m_globalstate = ALLOC_STRING( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else
		CPointEntity::KeyValue( pkvd );
}


#define SF_MULTI_INIT 1


void CMultiSource::Spawn( void )
{
	pev->solid = SOLID_NOT;
	pev->movetype = MOVETYPE_NONE;
	pev->nextthink = gpGlobals->time + 0.1;
	pev->spawnflags |= SF_MULTI_INIT;

	SetThink( &CMultiSource::Register );
}

void CMultiSource::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	int i = 0;

	while ( i < m_iTotal )
		if ( m_rgEntities[i++] == pCaller )
			break;

	if ( i > m_iTotal )
	{
		ALERT( at_console, "MultiSrc:Used by non member %s.\n", STRING( pCaller->pev->classname ) );
		return;
	}

	m_rgTriggered[i - 1] ^= 1;

	if ( IsTriggered( pActivator ) )
	{
		ALERT( at_aiconsole, "Multisource %s enabled (%d inputs)\n", STRING( pev->targetname ), m_iTotal );

		USE_TYPE useType = USE_TOGGLE;

		if ( m_globalstate )
			useType = USE_ON;

		SUB_UseTargets( NULL, useType, 0 );
	}
}

BOOL CMultiSource::IsTriggered( CBaseEntity *pActivator )
{
	int i = 0;

	if ( pev->spawnflags & SF_MULTI_INIT )
		return 0;

	while ( i < m_iTotal )
	{
		if ( m_rgTriggered[i] == 0 )
			break;

		i++;
	}

	if ( i == m_iTotal )
	{
		return 1;
	}

	return 0;
}

void CMultiSource::Register( void )
{
	edict_t *pentTarget = NULL;

	m_iTotal = 0;
	memset( m_rgEntities, 0, MS_MAX_TARGETS * sizeof( EHANDLE ) );

	SetThink( &CBaseEntity::SUB_DoNothing );

	pentTarget = FIND_ENTITY_BY_STRING( NULL, "target", STRING( pev->targetname ) );

	while ( !FNullEnt( pentTarget ) && ( m_iTotal < MS_MAX_TARGETS ) )
	{
		CBaseEntity *pTarget = CBaseEntity::Instance( pentTarget );

		if ( pTarget )
			m_rgEntities[m_iTotal++] = pTarget;

		pentTarget = FIND_ENTITY_BY_STRING( pentTarget, "target", STRING( pev->targetname ) );
	}

	pentTarget = FIND_ENTITY_BY_STRING( NULL, "classname", "multi_manager" );

	while ( !FNullEnt( pentTarget ) && ( m_iTotal < MS_MAX_TARGETS ) )
	{
		CBaseEntity *pTarget = CBaseEntity::Instance( pentTarget );

		if ( pTarget && pTarget->HasTarget( pev->targetname ) )
			m_rgEntities[m_iTotal++] = pTarget;

		pentTarget = FIND_ENTITY_BY_STRING( pentTarget, "classname", "multi_manager" );
	}

	pev->spawnflags &= ~SF_MULTI_INIT;
}

void CBaseButton::Precache( void )
{
	char *pszSound;

	if ( FBitSet( pev->spawnflags, SF_BUTTON_SPARK_IF_OFF ) )
	{
		PRECACHE_SOUND( "buttons/spark1.wav" );
		PRECACHE_SOUND( "buttons/spark2.wav" );
		PRECACHE_SOUND( "buttons/spark3.wav" );
		PRECACHE_SOUND( "buttons/spark4.wav" );
		PRECACHE_SOUND( "buttons/spark5.wav" );
		PRECACHE_SOUND( "buttons/spark6.wav" );
	}

	if ( m_bLockedSound )
	{
		pszSound = ButtonSound( ( int )m_bLockedSound );
		PRECACHE_SOUND( pszSound );
		m_ls.sLockedSound = ALLOC_STRING( pszSound );
	}

	if ( m_bUnlockedSound )
	{
		pszSound = ButtonSound( ( int )m_bUnlockedSound );
		PRECACHE_SOUND( pszSound );
		m_ls.sUnlockedSound = ALLOC_STRING( pszSound );
	}
}

void CBaseButton::KeyValue( KeyValueData *pkvd )
{
	if ( FStrEq( pkvd->szKeyName, "changetarget" ) )
	{
		m_strChangeTarget = ALLOC_STRING( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else if ( FStrEq( pkvd->szKeyName, "locked_sound" ) )
	{
		m_bLockedSound = atof( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else if ( FStrEq( pkvd->szKeyName, "unlocked_sound" ) )
	{
		m_bUnlockedSound = atof( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else if ( FStrEq( pkvd->szKeyName, "sounds" ) )
	{
		m_sounds = atoi( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else
		CBaseToggle::KeyValue( pkvd );
}

void CBaseButton::TakeDamage( entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType )
{
	BUTTON_CODE code = ButtonResponseToTouch();

	if ( code == BUTTON_NOTHING )
		return;

	SetTouch( NULL );

	m_hActivator = CBaseEntity::Instance( pevAttacker );

	if ( m_hActivator == NULL )
		return;

	if ( code == BUTTON_RETURN )
	{
		EMIT_SOUND( ENT( pev ), CHAN_VOICE, ( char * )STRING( pev->noise ), 1, ATTN_NORM );

		if ( !( pev->spawnflags & SF_BUTTON_TOGGLE ) )
			SUB_UseTargets( m_hActivator, USE_TOGGLE, 0 );

		ButtonReturn();
	}
	else
		ButtonActivate();
}

LINK_ENTITY_TO_CLASS( func_button, CBaseButton );

void CBaseButton::Spawn( void )
{
	char *pszSound;

	pszSound = ButtonSound( m_sounds );
	PRECACHE_SOUND( pszSound );
	pev->noise = ALLOC_STRING( pszSound );

	Precache();

	if ( FBitSet( pev->spawnflags, SF_BUTTON_SPARK_IF_OFF ) )
	{
		SetThink( &CBaseButton::ButtonSpark );
		pev->nextthink = gpGlobals->time + 0.5;
	}

	SetMovedir( pev );

	pev->movetype = MOVETYPE_PUSH;
	pev->solid = SOLID_BSP;

	SET_MODEL( ENT( pev ), STRING( pev->model ) );

	if ( pev->speed == 0 )
		pev->speed = 40;

	if ( pev->health > 0 )
		pev->takedamage = DAMAGE_YES;

	if ( m_flWait == 0 )
		m_flWait = 1;

	if ( m_flLip == 0 )
		m_flLip = 4;

	m_toggle_state = TS_AT_BOTTOM;
	m_vecPosition1 = pev->origin;
	m_vecPosition2 = m_vecPosition1 + ( pev->movedir * ( fabs( pev->movedir.x * ( pev->size.x - 2 ) ) + fabs( pev->movedir.y *
	                                    ( pev->size.y - 2 ) ) + fabs( pev->movedir.z * ( pev->size.z - 2 ) ) - m_flLip ) );

	if ( ( ( m_vecPosition2 - m_vecPosition1 ).Length() < 1 ) || ( pev->spawnflags & SF_BUTTON_DONTMOVE ) )
		m_vecPosition2 = m_vecPosition1;

	m_fStayPushed = ( m_flWait == -1 ? TRUE : FALSE );
	m_fRotating = FALSE;

	if ( FBitSet( pev->spawnflags, SF_BUTTON_TOUCH_ONLY ) )
		SetTouch( &CBaseButton::ButtonTouch );
	else
	{
		SetTouch( NULL );
		SetUse( &CBaseButton::ButtonUse );
	}
}

char *ButtonSound( int sound )
{
	char *pszSound;

	switch ( sound )
	{
		case 0: pszSound = "common/null.wav"; break;

		case 1: pszSound = "buttons/button1.wav"; break;

		case 2: pszSound = "buttons/button2.wav"; break;

		case 3: pszSound = "buttons/button3.wav"; break;

		case 4: pszSound = "buttons/button4.wav"; break;

		case 5: pszSound = "buttons/button5.wav"; break;

		case 6: pszSound = "buttons/button6.wav"; break;

		case 7: pszSound = "buttons/button7.wav"; break;

		case 8: pszSound = "buttons/button8.wav"; break;

		case 9: pszSound = "buttons/button9.wav"; break;

		case 10: pszSound = "buttons/button10.wav"; break;

		case 11: pszSound = "buttons/button11.wav"; break;

		case 12: pszSound = "buttons/latchlocked1.wav"; break;

		case 13: pszSound = "buttons/latchunlocked1.wav"; break;

		case 14: pszSound = "buttons/lightswitch2.wav"; break;

		case 21: pszSound = "buttons/lever1.wav"; break;

		case 22: pszSound = "buttons/lever2.wav"; break;

		case 23: pszSound = "buttons/lever3.wav"; break;

		case 24: pszSound = "buttons/lever4.wav"; break;

		case 25: pszSound = "buttons/lever5.wav"; break;

		default: pszSound = "buttons/button9.wav"; break;
	}

	return pszSound;
}

void DoSpark( entvars_t *pev, Vector &location )
{
	Vector tmp = location + pev->size * 0.5;
	UTIL_Sparks( tmp );

	float flVolume = RANDOM_FLOAT( 0.25, 0.75 ) * 0.4;

	switch ( ( int )( RANDOM_FLOAT( 0, 1 ) * 6 ) )
	{
		case 0: EMIT_SOUND( ENT( pev ), CHAN_VOICE, "buttons/spark1.wav", flVolume, ATTN_NORM ); break;

		case 1: EMIT_SOUND( ENT( pev ), CHAN_VOICE, "buttons/spark2.wav", flVolume, ATTN_NORM ); break;

		case 2: EMIT_SOUND( ENT( pev ), CHAN_VOICE, "buttons/spark3.wav", flVolume, ATTN_NORM ); break;

		case 3: EMIT_SOUND( ENT( pev ), CHAN_VOICE, "buttons/spark4.wav", flVolume, ATTN_NORM ); break;

		case 4: EMIT_SOUND( ENT( pev ), CHAN_VOICE, "buttons/spark5.wav", flVolume, ATTN_NORM ); break;

		case 5: EMIT_SOUND( ENT( pev ), CHAN_VOICE, "buttons/spark6.wav", flVolume, ATTN_NORM ); break;
	}
}

void CBaseButton::ButtonSpark( void )
{
	SetThink( &CBaseButton::ButtonSpark );
	pev->nextthink = gpGlobals->time + ( 0.1 + RANDOM_FLOAT( 0, 1.5 ) );

	DoSpark( pev, pev->mins );
}

void CBaseButton::ButtonUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	if ( m_toggle_state == TS_GOING_UP || m_toggle_state == TS_GOING_DOWN )
		return;

	m_hActivator = pActivator;

	if ( m_toggle_state == TS_AT_TOP )
	{
		if ( !m_fStayPushed && FBitSet( pev->spawnflags, SF_BUTTON_TOGGLE ) )
		{
			EMIT_SOUND( ENT( pev ), CHAN_VOICE, ( char * )STRING( pev->noise ), 1, ATTN_NORM );
			ButtonReturn();
		}
	}
	else
		ButtonActivate();
}

CBaseButton::BUTTON_CODE CBaseButton::ButtonResponseToTouch( void )
{
	if ( m_toggle_state == TS_GOING_UP ||
	     m_toggle_state == TS_GOING_DOWN ||
	     ( m_toggle_state == TS_AT_TOP && !m_fStayPushed && !FBitSet( pev->spawnflags, SF_BUTTON_TOGGLE ) ) )
		return BUTTON_NOTHING;

	if ( m_toggle_state == TS_AT_TOP )
	{
		if ( ( FBitSet( pev->spawnflags, SF_BUTTON_TOGGLE ) ) && !m_fStayPushed )
			return BUTTON_RETURN;
	}
	else
		return BUTTON_ACTIVATE;

	return BUTTON_NOTHING;
}

void CBaseButton::ButtonTouch( CBaseEntity *pOther )
{
	if ( !FClassnameIs( pOther->pev, "player" ) )
		return;

	m_hActivator = pOther;

	BUTTON_CODE code = ButtonResponseToTouch();

	if ( code == BUTTON_NOTHING )
		return;

	if ( !UTIL_IsMasterTriggered( m_sMaster, pOther ) )
	{
		PlayLockSounds( pev, &m_ls, TRUE, TRUE );
		return;
	}

	SetTouch( NULL );

	if ( code == BUTTON_RETURN )
	{
		EMIT_SOUND( ENT( pev ), CHAN_VOICE, ( char * )STRING( pev->noise ), 1, ATTN_NORM );
		SUB_UseTargets( m_hActivator, USE_TOGGLE, 0 );
		ButtonReturn();
	}
	else
		ButtonActivate();
}

void CBaseButton::ButtonActivate( void )
{
	EMIT_SOUND( ENT( pev ), CHAN_VOICE, ( char * )STRING( pev->noise ), 1, ATTN_NORM );

	if ( !UTIL_IsMasterTriggered( m_sMaster, m_hActivator ) )
	{
		PlayLockSounds( pev, &m_ls, TRUE, TRUE );
		return;
	}
	else
		PlayLockSounds( pev, &m_ls, FALSE, TRUE );

	m_toggle_state = TS_GOING_UP;

	SetMoveDone( &CBaseButton::TriggerAndWait );

	if ( !m_fRotating )
		LinearMove( m_vecPosition2, pev->speed );
	else
		AngularMove( m_vecAngle2, pev->speed );
}

void CBaseButton::TriggerAndWait( void )
{
	if ( !UTIL_IsMasterTriggered( m_sMaster, m_hActivator ) )
		return;

	m_toggle_state = TS_AT_TOP;

	if ( m_fStayPushed || FBitSet( pev->spawnflags, SF_BUTTON_TOGGLE ) )
	{
		if ( !FBitSet( pev->spawnflags, SF_BUTTON_TOUCH_ONLY ) )
			SetTouch( NULL );
		else
			SetTouch( &CBaseButton::ButtonTouch );
	}
	else
	{
		pev->nextthink = pev->ltime + m_flWait;
		SetThink( &CBaseButton::ButtonReturn );
	}

	pev->frame = 1;

	SUB_UseTargets( m_hActivator, USE_TOGGLE, 0 );
}

void CBaseButton::ButtonReturn( void )
{
	m_toggle_state = TS_GOING_DOWN;

	SetMoveDone( &CBaseButton::ButtonBackHome );

	if ( !m_fRotating )
		LinearMove( m_vecPosition1, pev->speed );
	else
		AngularMove( m_vecAngle1, pev->speed );

	pev->frame = 0;
}

void CBaseButton::ButtonBackHome( void )
{
	m_toggle_state = TS_AT_BOTTOM;

	if ( FBitSet( pev->spawnflags, SF_BUTTON_TOGGLE ) )
		SUB_UseTargets( m_hActivator, USE_TOGGLE, 0 );

	if ( !FStringNull( pev->target ) )
	{
		edict_t *pentTarget = NULL;

		for ( ; ; )
		{
			pentTarget = FIND_ENTITY_BY_TARGETNAME( pentTarget, STRING( pev->target ) );

			if ( FNullEnt( pentTarget ) )
				break;

			if ( !FClassnameIs( pentTarget, "multisource" ) )
				continue;

			CBaseEntity *pTarget = CBaseEntity::Instance( pentTarget );

			if ( pTarget )
				pTarget->Use( m_hActivator, this, USE_TOGGLE, 0 );
		}
	}

	if ( !FBitSet( pev->spawnflags, SF_BUTTON_TOUCH_ONLY ) )
		SetTouch( NULL );
	else
		SetTouch( &CBaseButton::ButtonTouch );

	if ( FBitSet( pev->spawnflags, SF_BUTTON_SPARK_IF_OFF ) )
	{
		SetThink( &CBaseButton::ButtonSpark );
		pev->nextthink = gpGlobals->time + 0.5;
	}
}

class CRotButton : public CBaseButton
{
public:
	void Spawn( void );
};

LINK_ENTITY_TO_CLASS( func_rot_button, CRotButton );

void CRotButton::Spawn( void )
{
	char *pszSound;

	pszSound = ButtonSound( m_sounds );
	PRECACHE_SOUND( pszSound );
	pev->noise = ALLOC_STRING( pszSound );

	CBaseToggle::AxisDir( pev );

	if ( FBitSet( pev->spawnflags, SF_DOOR_ROTATE_BACKWARDS ) )
		pev->movedir = pev->movedir * -1;

	pev->movetype = MOVETYPE_PUSH;

	if ( pev->spawnflags & SF_ROTBUTTON_NOTSOLID )
		pev->solid = SOLID_NOT;
	else
		pev->solid = SOLID_BSP;

	SET_MODEL( ENT( pev ), STRING( pev->model ) );

	if ( pev->speed == 0 )
		pev->speed = 40;

	if ( m_flWait == 0 )
		m_flWait = 1;

	if ( pev->health > 0 )
		pev->takedamage = DAMAGE_YES;

	m_toggle_state = TS_AT_BOTTOM;

	m_vecAngle1 = pev->angles;
	m_vecAngle2 = pev->angles + pev->movedir * m_flMoveDistance;

	m_fStayPushed = ( m_flWait == -1 ? TRUE : FALSE );
	m_fRotating = TRUE;

	if ( !FBitSet( pev->spawnflags, SF_BUTTON_TOUCH_ONLY ) )
	{
		SetTouch( NULL );
		SetUse( &CBaseButton::ButtonUse );
	}
	else
		SetTouch( &CBaseButton::ButtonTouch );
}


#define SF_MOMENTARY_DOOR 0x0001


class CMomentaryRotButton : public CBaseToggle
{
public:
	void Spawn( void );
	void KeyValue( KeyValueData *pkvd );
	int ObjectCaps( void )
	{
		int flags = CBaseToggle::ObjectCaps() & ( ~FCAP_ACROSS_TRANSITION );

		if ( pev->spawnflags & SF_MOMENTARY_DOOR )
			return flags;

		return flags | FCAP_CONTINUOUS_USE;
	}
	void Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	void Off( void );
	void Return( void );
	void UpdateSelf( float value );
	void UpdateSelfReturn( float value );
	void UpdateAllButtons( float value, int start );
	void PlaySound( void );
	void UpdateTarget( float value );

	static CMomentaryRotButton *Instance( edict_t *pent ) { return ( CMomentaryRotButton * )GET_PRIVATE( pent ); };

	int m_lastUsed;
	int m_direction;
	float m_returnSpeed;
	vec3_t m_start;
	vec3_t m_end;
	int m_sounds;
};

LINK_ENTITY_TO_CLASS( momentary_rot_button, CMomentaryRotButton );

void CMomentaryRotButton::Spawn( void )
{
	CBaseToggle::AxisDir( pev );

	if ( pev->speed == 0 )
		pev->speed = 100;

	if ( m_flMoveDistance < 0 )
	{
		m_start = pev->angles + pev->movedir * m_flMoveDistance;
		m_end = pev->angles;
		m_direction = 1;
		m_flMoveDistance = -m_flMoveDistance;
	}
	else
	{
		m_start = pev->angles;
		m_end = pev->angles + pev->movedir * m_flMoveDistance;
		m_direction = -1;
	}

	if ( pev->spawnflags & SF_MOMENTARY_DOOR )
		pev->solid = SOLID_BSP;
	else
		pev->solid = SOLID_NOT;

	pev->movetype = MOVETYPE_PUSH;

	UTIL_SetOrigin( pev, pev->origin );
	SET_MODEL( ENT( pev ), STRING( pev->model ) );

	char *pszSound = ButtonSound( m_sounds );
	PRECACHE_SOUND( pszSound );
	pev->noise = ALLOC_STRING( pszSound );

	m_lastUsed = 0;
}

void CMomentaryRotButton::KeyValue( KeyValueData *pkvd )
{
	if ( FStrEq( pkvd->szKeyName, "returnspeed" ) )
	{
		m_returnSpeed = atof( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else if ( FStrEq( pkvd->szKeyName, "sounds" ) )
	{
		m_sounds = atoi( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else
		CBaseToggle::KeyValue( pkvd );
}

void CMomentaryRotButton::PlaySound( void )
{
	EMIT_SOUND( ENT( pev ), CHAN_VOICE, ( char * )STRING( pev->noise ), 1, ATTN_NORM );
}

void CMomentaryRotButton::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	pev->ideal_yaw = CBaseToggle::AxisDelta( pev->spawnflags, pev->angles, m_start ) / m_flMoveDistance;

	UpdateAllButtons( pev->ideal_yaw, 1 );
	UpdateTarget( pev->ideal_yaw );
}

void CMomentaryRotButton::UpdateAllButtons( float value, int start )
{
	edict_t *pentTarget = NULL;

	for ( ; ; )
	{
		pentTarget = FIND_ENTITY_BY_STRING( pentTarget, "target", STRING( pev->target ) );

		if ( FNullEnt( pentTarget ) )
			break;

		if ( FClassnameIs( VARS( pentTarget ), "momentary_rot_button" ) )
		{
			CMomentaryRotButton *pEntity = CMomentaryRotButton::Instance( pentTarget );

			if ( pEntity )
			{
				if ( start )
					pEntity->UpdateSelf( value );
				else
					pEntity->UpdateSelfReturn( value );
			}
		}
	}
}

void CMomentaryRotButton::UpdateSelf( float value )
{
	BOOL fplaysound = FALSE;

	if ( !m_lastUsed )
	{
		fplaysound = TRUE;
		m_direction = -m_direction;
	}

	m_lastUsed = 1;

	pev->nextthink = pev->ltime + 0.1;

	if ( m_direction > 0 && value >= 1.0 )
	{
		pev->avelocity = g_vecZero;
		pev->angles = m_end;
		return;
	}
	else if ( m_direction < 0 && value <= 0 )
	{
		pev->avelocity = g_vecZero;
		pev->angles = m_start;
		return;
	}

	if ( fplaysound )
		PlaySound();

	if ( pev->nextthink < pev->ltime )
		pev->nextthink = pev->ltime + 0.1;
	else
		pev->nextthink += 0.1;

	pev->avelocity = pev->movedir * ( m_direction * pev->speed );
	SetThink( &CMomentaryRotButton::Off );
}

void CMomentaryRotButton::UpdateTarget( float value )
{
	if ( !FStringNull( pev->target ) )
	{
		edict_t *pentTarget = NULL;

		for ( ; ; )
		{
			pentTarget = FIND_ENTITY_BY_TARGETNAME( pentTarget, STRING( pev->target ) );

			if ( FNullEnt( pentTarget ) )
				break;

			CBaseEntity *pEntity = CBaseEntity::Instance( pentTarget );

			if ( pEntity )
				pEntity->Use( this, this, USE_SET, value );
		}
	}
}

void CMomentaryRotButton::Off( void )
{
	pev->avelocity = g_vecZero;
	m_lastUsed = 0;

	if ( FBitSet( pev->spawnflags, SF_PENDULUM_AUTO_RETURN ) && m_returnSpeed > 0 )
	{
		SetThink( &CMomentaryRotButton::Return );
		pev->nextthink = pev->ltime + 0.1;
		m_direction = -1;
	}
	else
		SetThink( NULL );
}

void CMomentaryRotButton::Return( void )
{
	float value = CBaseToggle::AxisDelta( pev->spawnflags, pev->angles, m_start ) / m_flMoveDistance;

	UpdateAllButtons( value, 0 );

	if ( value > 0 )
		UpdateTarget( value );
}

void CMomentaryRotButton::UpdateSelfReturn( float value )
{
	if ( value <= 0 )
	{
		pev->avelocity = g_vecZero;
		pev->angles = m_start;
		pev->nextthink = -1;
		SetThink( NULL );
	}
	else
	{
		pev->avelocity = pev->movedir * -m_returnSpeed;
		pev->nextthink = pev->ltime + 0.1;
	}
}

class CEnvSpark : public CBaseEntity
{
public:
	void Spawn( void );
	void Precache( void );
	void SparkThink( void );
	void SparkStart( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	void SparkStop( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	void KeyValue( KeyValueData *pkvd );

	float m_flDelay;
};

LINK_ENTITY_TO_CLASS( env_spark, CEnvSpark );

void CEnvSpark::Spawn( void )
{
	SetThink( NULL );
	SetUse( NULL );

	if ( FBitSet( pev->spawnflags, 32 ) )
	{
		if ( FBitSet( pev->spawnflags, 64 ) )
		{
			SetThink( &CEnvSpark::SparkThink );
			SetUse( &CEnvSpark::SparkStop );
		}
		else
			SetUse( &CEnvSpark::SparkStart );
	}
	else
		SetThink( &CEnvSpark::SparkThink );

	pev->nextthink = gpGlobals->time + ( 0.1 + RANDOM_FLOAT( 0, 1.5 ) );

	if ( m_flDelay <= 0 )
		m_flDelay = 1.5;

	Precache();
}

void CEnvSpark::Precache( void )
{
	PRECACHE_SOUND( "buttons/spark1.wav" );
	PRECACHE_SOUND( "buttons/spark2.wav" );
	PRECACHE_SOUND( "buttons/spark3.wav" );
	PRECACHE_SOUND( "buttons/spark4.wav" );
	PRECACHE_SOUND( "buttons/spark5.wav" );
	PRECACHE_SOUND( "buttons/spark6.wav" );
}

void CEnvSpark::KeyValue( KeyValueData *pkvd )
{
	if ( FStrEq( pkvd->szKeyName, "MaxDelay" ) )
	{
		m_flDelay = atof( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else if ( FStrEq( pkvd->szKeyName, "style" ) ||
	          FStrEq( pkvd->szKeyName, "height" ) ||
	          FStrEq( pkvd->szKeyName, "killtarget" ) ||
	          FStrEq( pkvd->szKeyName, "value1" ) ||
	          FStrEq( pkvd->szKeyName, "value2" ) ||
	          FStrEq( pkvd->szKeyName, "value3" ) )
		pkvd->fHandled = TRUE;
	else
		CBaseEntity::KeyValue( pkvd );
}

void CEnvSpark::SparkThink( void )
{
	pev->nextthink = gpGlobals->time + 0.1 + RANDOM_FLOAT( 0, m_flDelay );
	DoSpark( pev, pev->origin );
}

void CEnvSpark::SparkStart( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	SetUse( &CEnvSpark::SparkStop );
	SetThink( &CEnvSpark::SparkThink );
	pev->nextthink = gpGlobals->time + ( 0.1 + RANDOM_FLOAT( 0, m_flDelay ) );
}

void CEnvSpark::SparkStop( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	SetUse( &CEnvSpark::SparkStart );
	SetThink( NULL );
}


#define SF_BTARGET_USE	0x0001
#define SF_BTARGET_ON	0x0002


class CButtonTarget : public CBaseEntity
{
public:
	void Spawn( void );
	void Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	void TakeDamage( entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType );
	int ObjectCaps( void );
};

LINK_ENTITY_TO_CLASS( button_target, CButtonTarget );

void CButtonTarget::Spawn( void )
{
	pev->movetype = MOVETYPE_PUSH;
	pev->solid = SOLID_BSP;

	SET_MODEL( ENT( pev ), STRING( pev->model ) );

	pev->takedamage = DAMAGE_YES;

	if ( FBitSet( pev->spawnflags, SF_BTARGET_ON ) )
		pev->frame = 1;
}

void CButtonTarget::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	if ( !ShouldToggle( useType, ( int )pev->frame ) )
		return;

	pev->frame = 1 - pev->frame;

	if ( pev->frame )
		SUB_UseTargets( pActivator, USE_ON, 0 );
	else
		SUB_UseTargets( pActivator, USE_OFF, 0 );
}

int CButtonTarget::ObjectCaps( void )
{
	int caps = CBaseEntity::ObjectCaps() & ~FCAP_ACROSS_TRANSITION;

	if ( FBitSet( pev->spawnflags, SF_BTARGET_USE ) )
		return caps | FCAP_IMPULSE_USE;
	else
		return caps;
}

void CButtonTarget::TakeDamage( entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType )
{
	Use( Instance( pevAttacker ), this, USE_TOGGLE, 0 );
}