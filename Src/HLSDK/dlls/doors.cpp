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

extern void SetMovedir( entvars_t *ev );

#define noiseMoving		noise1
#define noiseArrived	noise2

#define DOOR_SENTENCEWAIT 6
#define DOOR_SOUNDWAIT 3
#define BUTTON_SOUNDWAIT 0.5

void PlayLockSounds( entvars_t *pev, locksound_t *pls, int flocked, int fbutton )
{
	float flsoundwait;

	if ( fbutton )
		flsoundwait = BUTTON_SOUNDWAIT;
	else
		flsoundwait = DOOR_SOUNDWAIT;

	if ( flocked )
	{
		int fplaysound = ( pls->sLockedSound && gpGlobals->time > pls->flwaitSound );
		float fvol;

		if ( fplaysound )
			fvol = 0.25;
		else
			fvol = 1.0;

		if ( fplaysound )
		{
			EMIT_SOUND( ENT( pev ), CHAN_ITEM, ( char * )STRING( pls->sLockedSound ), fvol, ATTN_NORM );
			pls->flwaitSound = gpGlobals->time + flsoundwait;
		}
	}
	else
	{
		int fplaysound = ( pls->sUnlockedSound && gpGlobals->time > pls->flwaitSound );
		float fvol;

		if ( fplaysound )
			fvol = 0.25;
		else
			fvol = 1.0;

		if ( fplaysound )
		{
			EMIT_SOUND( ENT( pev ), CHAN_ITEM, ( char * )STRING( pls->sUnlockedSound ), fvol, ATTN_NORM );
			pls->flwaitSound = gpGlobals->time + flsoundwait;
		}
	}
}

void CBaseDoor::KeyValue( KeyValueData *pkvd )
{
	if ( FStrEq( pkvd->szKeyName, "skin" ) )
	{
		pev->skin = atof( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else if ( FStrEq( pkvd->szKeyName, "movesnd" ) )
	{
		m_bMoveSnd = atof( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else if ( FStrEq( pkvd->szKeyName, "stopsnd" ) )
	{
		m_bStopSnd = atof( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else if ( FStrEq( pkvd->szKeyName, "healthvalue" ) )
	{
		m_bHealthValue = atof( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else if ( FStrEq( pkvd->szKeyName, "locked_sound" ) )
	{
		m_bLockedSound = atof( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else if ( FStrEq( pkvd->szKeyName, "locked_sentence" ) )
	{
		m_bLockedSentence = atof( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else if ( FStrEq( pkvd->szKeyName, "unlocked_sound" ) )
	{
		m_bUnlockedSound = atof( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else if ( FStrEq( pkvd->szKeyName, "unlocked_sentence" ) )
	{
		m_bUnlockedSentence = atof( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else if ( FStrEq( pkvd->szKeyName, "WaveHeight" ) )
	{
		pev->scale = atof( pkvd->szValue ) * ( 1.0 / 8.0 );
		pkvd->fHandled = TRUE;
	}
	else
		CBaseToggle::KeyValue( pkvd );
}

LINK_ENTITY_TO_CLASS( func_door, CBaseDoor );
LINK_ENTITY_TO_CLASS( func_water, CBaseDoor );

void CBaseDoor::Spawn()
{
	Precache();
	SetMovedir( pev );

	if ( pev->skin == 0 )
	{
		if ( FBitSet( pev->spawnflags, SF_DOOR_PASSABLE ) )
			pev->solid = SOLID_NOT;
		else
			pev->solid = SOLID_BSP;
	}
	else
	{
		pev->solid = SOLID_NOT;
		SetBits( pev->spawnflags, SF_DOOR_SILENT );
	}

	pev->movetype = MOVETYPE_PUSH;

	UTIL_SetOrigin( pev, pev->origin );
	SET_MODEL( ENT( pev ), STRING( pev->model ) );

	if ( pev->speed == 0 )
		pev->speed = 100;

	m_vecPosition1 = pev->origin;
	m_vecPosition2 = m_vecPosition1 + ( pev->movedir * ( fabs( pev->movedir.x * ( pev->size.x - 2 ) ) + fabs( pev->movedir.y *
	                                    ( pev->size.y - 2 ) ) + fabs( pev->movedir.z * ( pev->size.z - 2 ) ) - m_flLip ) );

	if ( FBitSet( pev->spawnflags, SF_DOOR_START_OPEN ) )
	{
		UTIL_SetOrigin( pev, m_vecPosition2 );
		m_vecPosition2 = m_vecPosition1;
		m_vecPosition1 = pev->origin;
	}

	m_toggle_state = TS_AT_BOTTOM;

	if ( FBitSet( pev->spawnflags, SF_DOOR_USE_ONLY ) )
		SetTouch( NULL );
	else
		SetTouch( &CBaseDoor::DoorTouch );
}

void CBaseDoor::SetToggleState( int state )
{
	if ( state == TS_AT_TOP )
		UTIL_SetOrigin( pev, m_vecPosition2 );
	else
		UTIL_SetOrigin( pev, m_vecPosition1 );
}

void CBaseDoor::Precache( void )
{
	char *pszSound;

	switch ( m_bMoveSnd )
	{
		case 0:
			pev->noiseMoving = ALLOC_STRING( "common/null.wav" );
			break;

		case 1:
			PRECACHE_SOUND( "doors/doormove1.wav" );
			pev->noiseMoving = ALLOC_STRING( "doors/doormove1.wav" );
			break;

		case 2:
			PRECACHE_SOUND( "doors/doormove2.wav" );
			pev->noiseMoving = ALLOC_STRING( "doors/doormove2.wav" );
			break;

		case 3:
			PRECACHE_SOUND( "doors/doormove3.wav" );
			pev->noiseMoving = ALLOC_STRING( "doors/doormove3.wav" );
			break;

		case 4:
			PRECACHE_SOUND( "doors/doormove4.wav" );
			pev->noiseMoving = ALLOC_STRING( "doors/doormove4.wav" );
			break;

		case 5:
			PRECACHE_SOUND( "doors/doormove5.wav" );
			pev->noiseMoving = ALLOC_STRING( "doors/doormove5.wav" );
			break;

		case 6:
			PRECACHE_SOUND( "doors/doormove6.wav" );
			pev->noiseMoving = ALLOC_STRING( "doors/doormove6.wav" );
			break;

		case 7:
			PRECACHE_SOUND( "doors/doormove7.wav" );
			pev->noiseMoving = ALLOC_STRING( "doors/doormove7.wav" );
			break;

		case 8:
			PRECACHE_SOUND( "doors/doormove8.wav" );
			pev->noiseMoving = ALLOC_STRING( "doors/doormove8.wav" );
			break;

		case 9:
			PRECACHE_SOUND( "doors/doormove9.wav" );
			pev->noiseMoving = ALLOC_STRING( "doors/doormove9.wav" );
			break;

		case 10:
			PRECACHE_SOUND( "doors/doormove10.wav" );
			pev->noiseMoving = ALLOC_STRING( "doors/doormove10.wav" );
			break;

		default:
			pev->noiseMoving = ALLOC_STRING( "common/null.wav" );
			break;
	}

	switch ( m_bStopSnd )
	{
		case 0:
			pev->noiseArrived = ALLOC_STRING( "common/null.wav" );
			break;

		case 1:
			PRECACHE_SOUND( "doors/doorstop1.wav" );
			pev->noiseArrived = ALLOC_STRING( "doors/doorstop1.wav" );
			break;

		case 2:
			PRECACHE_SOUND( "doors/doorstop2.wav" );
			pev->noiseArrived = ALLOC_STRING( "doors/doorstop2.wav" );
			break;

		case 3:
			PRECACHE_SOUND( "doors/doorstop3.wav" );
			pev->noiseArrived = ALLOC_STRING( "doors/doorstop3.wav" );
			break;

		case 4:
			PRECACHE_SOUND( "doors/doorstop4.wav" );
			pev->noiseArrived = ALLOC_STRING( "doors/doorstop4.wav" );
			break;

		case 5:
			PRECACHE_SOUND( "doors/doorstop5.wav" );
			pev->noiseArrived = ALLOC_STRING( "doors/doorstop5.wav" );
			break;

		case 6:
			PRECACHE_SOUND( "doors/doorstop6.wav" );
			pev->noiseArrived = ALLOC_STRING( "doors/doorstop6.wav" );
			break;

		case 7:
			PRECACHE_SOUND( "doors/doorstop7.wav" );
			pev->noiseArrived = ALLOC_STRING( "doors/doorstop7.wav" );
			break;

		case 8:
			PRECACHE_SOUND( "doors/doorstop8.wav" );
			pev->noiseArrived = ALLOC_STRING( "doors/doorstop8.wav" );
			break;

		default:
			pev->noiseArrived = ALLOC_STRING( "common/null.wav" );
			break;
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

void CBaseDoor::DoorTouch( CBaseEntity *pOther )
{
	entvars_t *pevToucher = pOther->pev;

	if ( !FClassnameIs( pevToucher, "player" ) )
		return;

	if ( m_sMaster && !UTIL_IsMasterTriggered( m_sMaster, pOther ) )
		PlayLockSounds( pev, &m_ls, TRUE, FALSE );

	if ( !FStringNull( pev->targetname ) )
	{
		PlayLockSounds( pev, &m_ls, TRUE, FALSE );
		return;
	}

	m_hActivator.Set(pOther->edict());

	if ( DoorActivate() )
		SetTouch( NULL );
}

void CBaseDoor::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	m_hActivator.Set(pActivator->edict());

	if ( m_toggle_state == TS_AT_BOTTOM || FBitSet( pev->spawnflags, SF_DOOR_NO_AUTO_RETURN ) && m_toggle_state == TS_AT_TOP )
		DoorActivate();
}

int CBaseDoor::DoorActivate()
{
	if ( !UTIL_IsMasterTriggered( m_sMaster, m_hActivator ) )
		return 0;

	if ( FBitSet( pev->spawnflags, SF_DOOR_NO_AUTO_RETURN ) && m_toggle_state == TS_AT_TOP )
		DoorGoDown();
	else
	{
		if ( m_hActivator != NULL && m_hActivator->IsPlayer() )
			m_hActivator->TakeHealth( m_bHealthValue );

		PlayLockSounds( pev, &m_ls, FALSE, FALSE );

		DoorGoUp();
	}

	return 1;
}

extern Vector VecBModelOrigin( entvars_t *pevBModel );

void CBaseDoor::DoorGoUp( void )
{
	entvars_t *pevActivator;

	if ( !FBitSet( pev->spawnflags, SF_DOOR_SILENT ) )
		EMIT_SOUND( ENT( pev ), CHAN_STATIC, ( char * )STRING( pev->noiseMoving ), 1, ATTN_NORM );

	m_toggle_state = TS_GOING_UP;
	SetMoveDone( &CBaseDoor::DoorHitTop );

	if ( FClassnameIs( pev, "func_door_rotating" ) )
	{
		float sign = 1.0;

		if ( m_hActivator != NULL )
		{
			pevActivator = m_hActivator->pev;

			if ( !FBitSet( pev->spawnflags, SF_DOOR_ONEWAY ) && pev->movedir.y )
			{
				Vector vec = pevActivator->origin - pev->origin;

				Vector angles = pevActivator->angles;
				angles.x = 0;
				angles.z = 0;
				UTIL_MakeVectors( angles );
				UTIL_MakeVectors( pevActivator->angles );

				Vector vnext = ( pevActivator->origin + ( gpGlobals->v_forward * 10 ) ) - pev->origin;

				if ( ( vec.x * vnext.y - vec.y * vnext.x ) < 0 )
					sign = -1.0;
			}
		}

		AngularMove( m_vecAngle2 * sign, pev->speed );
	}
	else
		LinearMove( m_vecPosition2, pev->speed );
}

void CBaseDoor::DoorHitTop( void )
{
	if ( !FBitSet( pev->spawnflags, SF_DOOR_SILENT ) )
	{
		STOP_SOUND( ENT( pev ), CHAN_STATIC, ( char * )STRING( pev->noiseMoving ) );
		EMIT_SOUND( ENT( pev ), CHAN_STATIC, ( char * )STRING( pev->noiseArrived ), 1, ATTN_NORM );
	}

	m_toggle_state = TS_AT_TOP;

	if ( FBitSet( pev->spawnflags, SF_DOOR_NO_AUTO_RETURN ) )
	{
		if ( !FBitSet( pev->spawnflags, SF_DOOR_USE_ONLY ) )
			SetTouch( &CBaseDoor::DoorTouch );
	}
	else
	{
		pev->nextthink = pev->ltime + m_flWait;
		SetThink( &CBaseDoor::DoorGoDown );

		if ( m_flWait == -1 )
			pev->nextthink = -1;
	}

	if ( pev->netname && ( pev->spawnflags & SF_DOOR_START_OPEN ) )
		FireTargets( STRING( pev->netname ), m_hActivator, this, USE_TOGGLE, 0 );

	SUB_UseTargets( m_hActivator, USE_TOGGLE, 0 );
}

void CBaseDoor::DoorGoDown( void )
{
	if ( !FBitSet( pev->spawnflags, SF_DOOR_SILENT ) )
		EMIT_SOUND( ENT( pev ), CHAN_STATIC, ( char * )STRING( pev->noiseMoving ), 1, ATTN_NORM );

	m_toggle_state = TS_GOING_DOWN;

	SetMoveDone( &CBaseDoor::DoorHitBottom );

	if ( FClassnameIs( pev, "func_door_rotating" ) )
		AngularMove( m_vecAngle1, pev->speed );
	else
		LinearMove( m_vecPosition1, pev->speed );
}

void CBaseDoor::DoorHitBottom( void )
{
	if ( !FBitSet( pev->spawnflags, SF_DOOR_SILENT ) )
	{
		STOP_SOUND( ENT( pev ), CHAN_STATIC, ( char * )STRING( pev->noiseMoving ) );
		EMIT_SOUND( ENT( pev ), CHAN_STATIC, ( char * )STRING( pev->noiseArrived ), 1, ATTN_NORM );
	}

	m_toggle_state = TS_AT_BOTTOM;

	if ( FBitSet( pev->spawnflags, SF_DOOR_USE_ONLY ) )
		SetTouch( NULL );
	else
		SetTouch( &CBaseDoor::DoorTouch );

	SUB_UseTargets( m_hActivator, USE_TOGGLE, 0 );

	if ( pev->netname && !( pev->spawnflags & SF_DOOR_START_OPEN ) )
		FireTargets( STRING( pev->netname ), m_hActivator, this, USE_TOGGLE, 0 );
}

void CBaseDoor::Blocked( CBaseEntity *pOther )
{
	edict_t *pentTarget = NULL;
	CBaseDoor *pDoor = NULL;

	if ( pev->dmg )
		pOther->TakeDamage( pev, pev, pev->dmg, DMG_CRUSH );

	if ( m_flWait >= 0 )
	{
		if ( m_toggle_state == TS_GOING_DOWN )
			DoorGoUp();
		else
			DoorGoDown();
	}

	if ( !FStringNull( pev->targetname ) )
	{
		for ( ; ; )
		{
			pentTarget = FIND_ENTITY_BY_TARGETNAME( pentTarget, STRING( pev->targetname ) );

			if ( VARS( pentTarget ) != pev )
			{
				if ( FNullEnt( pentTarget ) )
					break;

				if ( FClassnameIs( pentTarget, "func_door" ) || FClassnameIs( pentTarget, "func_door_rotating" ) )
				{
					pDoor = GetClassPtr( ( CBaseDoor * )VARS( pentTarget ) );

					if ( pDoor->m_flWait >= 0 )
					{
						if ( pDoor->pev->velocity == pev->velocity && pDoor->pev->avelocity == pev->velocity )
						{
							if ( FClassnameIs( pentTarget, "func_door" ) )
							{
								pDoor->pev->origin = pev->origin;
								pDoor->pev->velocity = g_vecZero;
							}
							else
							{
								pDoor->pev->angles = pev->angles;
								pDoor->pev->avelocity = g_vecZero;
							}
						}

						if ( pDoor->m_toggle_state == TS_GOING_DOWN )
							pDoor->DoorGoUp();
						else
							pDoor->DoorGoDown();
					}
				}
			}
		}
	}
}

class CRotDoor : public CBaseDoor
{
public:
	virtual void Spawn( void );
	virtual void SetToggleState( int state );
};

LINK_ENTITY_TO_CLASS( func_door_rotating, CRotDoor );

void CRotDoor::Spawn( void )
{
	Precache();
	CBaseToggle::AxisDir( pev );

	if ( FBitSet( pev->spawnflags, SF_DOOR_ROTATE_BACKWARDS ) )
		pev->movedir = pev->movedir * -1;

	m_vecAngle1 = pev->angles;
	m_vecAngle2 = pev->angles + pev->movedir * m_flMoveDistance;

	if ( FBitSet( pev->spawnflags, SF_DOOR_PASSABLE ) )
		pev->solid = SOLID_NOT;
	else
		pev->solid = SOLID_BSP;

	pev->movetype = MOVETYPE_PUSH;

	UTIL_SetOrigin( pev, pev->origin );
	SET_MODEL( ENT( pev ), STRING( pev->model ) );

	if ( pev->speed == 0 )
		pev->speed = 100;

	if ( FBitSet( pev->spawnflags, SF_DOOR_START_OPEN ) )
	{
		pev->angles = m_vecAngle2;
		Vector vecSav = m_vecAngle1;
		m_vecAngle2 = m_vecAngle1;
		m_vecAngle1 = vecSav;
		pev->movedir = pev->movedir * -1;
	}

	m_toggle_state = TS_AT_BOTTOM;

	if ( FBitSet( pev->spawnflags, SF_DOOR_USE_ONLY ) )
		SetTouch( NULL );
	else
		SetTouch( &CBaseDoor::DoorTouch );
}

void CRotDoor::SetToggleState( int state )
{
	if ( state == TS_AT_TOP )
		pev->angles = m_vecAngle2;
	else
		pev->angles = m_vecAngle1;

	UTIL_SetOrigin( pev, pev->origin );
}

class CMomentaryDoor : public CBaseToggle
{
public:
	virtual void Spawn( void );
	virtual void Precache( void );
	virtual void KeyValue( KeyValueData *pkvd );
	virtual void Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	virtual int ObjectCaps( void ) { return CBaseToggle::ObjectCaps() & ~FCAP_ACROSS_TRANSITION; }

	BYTE m_bMoveSnd;
};

LINK_ENTITY_TO_CLASS( momentary_door, CMomentaryDoor );

void CMomentaryDoor::Spawn( void )
{
	SetMovedir( pev );

	pev->solid = SOLID_BSP;
	pev->movetype = MOVETYPE_PUSH;

	UTIL_SetOrigin( pev, pev->origin );
	SET_MODEL( ENT( pev ), STRING( pev->model ) );

	if ( pev->speed == 0 )
		pev->speed = 100;

	if ( pev->dmg == 0 )
		pev->dmg = 2;

	m_vecPosition1 = pev->origin;
	m_vecPosition2 = m_vecPosition1 + ( pev->movedir * ( fabs( pev->movedir.x * ( pev->size.x - 2 ) ) + fabs( pev->movedir.y *
	                                    ( pev->size.y - 2 ) ) + fabs( pev->movedir.z * ( pev->size.z - 2 ) ) - m_flLip ) );

	if ( FBitSet( pev->spawnflags, SF_DOOR_START_OPEN ) )
	{
		UTIL_SetOrigin( pev, m_vecPosition2 );
		m_vecPosition2 = m_vecPosition1;
		m_vecPosition1 = pev->origin;
	}

	SetTouch( NULL );

	Precache();
}

void CMomentaryDoor::Precache( void )
{
	switch ( m_bMoveSnd )
	{
		case 0:
			pev->noiseMoving = ALLOC_STRING( "common/null.wav" );
			break;

		case 1:
			PRECACHE_SOUND( "doors/doormove1.wav" );
			pev->noiseMoving = ALLOC_STRING( "doors/doormove1.wav" );
			break;

		case 2:
			PRECACHE_SOUND( "doors/doormove2.wav" );
			pev->noiseMoving = ALLOC_STRING( "doors/doormove2.wav" );
			break;

		case 3:
			PRECACHE_SOUND( "doors/doormove3.wav" );
			pev->noiseMoving = ALLOC_STRING( "doors/doormove3.wav" );
			break;

		case 4:
			PRECACHE_SOUND( "doors/doormove4.wav" );
			pev->noiseMoving = ALLOC_STRING( "doors/doormove4.wav" );
			break;

		case 5:
			PRECACHE_SOUND( "doors/doormove5.wav" );
			pev->noiseMoving = ALLOC_STRING( "doors/doormove5.wav" );
			break;

		case 6:
			PRECACHE_SOUND( "doors/doormove6.wav" );
			pev->noiseMoving = ALLOC_STRING( "doors/doormove6.wav" );
			break;

		case 7:
			PRECACHE_SOUND( "doors/doormove7.wav" );
			pev->noiseMoving = ALLOC_STRING( "doors/doormove7.wav" );
			break;

		case 8:
			PRECACHE_SOUND( "doors/doormove8.wav" );
			pev->noiseMoving = ALLOC_STRING( "doors/doormove8.wav" );
			break;

		default:
			pev->noiseMoving = ALLOC_STRING( "common/null.wav" );
			break;
	}
}

void CMomentaryDoor::KeyValue( KeyValueData *pkvd )
{
	if ( FStrEq( pkvd->szKeyName, "movesnd" ) )
	{
		m_bMoveSnd = atof( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else if ( FStrEq( pkvd->szKeyName, "stopsnd" ) )
		pkvd->fHandled = TRUE;
	else if ( FStrEq( pkvd->szKeyName, "healthvalue" ) )
		pkvd->fHandled = TRUE;
	else
		CBaseToggle::KeyValue( pkvd );
}

void CMomentaryDoor::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	if ( useType != USE_SET )
		return;

	if ( value > 1.0 )
		value = 1.0;

	Vector move = m_vecPosition1 + ( ( m_vecPosition2 - m_vecPosition1 ) * value );
	Vector delta = move - pev->origin;

	float speed = delta.Length() * 10;

	if ( speed != 0 )
	{
		if ( pev->nextthink < pev->ltime || pev->nextthink == 0 )
			EMIT_SOUND( ENT( pev ), CHAN_STATIC, ( char * )STRING( pev->noiseMoving ), 1, ATTN_NORM );

		LinearMove( move, speed );
	}
}