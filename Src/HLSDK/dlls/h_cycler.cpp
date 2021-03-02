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

extern short g_sModelIndexSmoke;

class CCycler : public CBaseAnimating
{
public:
	void GenericCyclerSpawn( char *szModel, Vector vecMin, Vector vecMax );
	int ObjectCaps( void ) { return ( CBaseEntity::ObjectCaps() | FCAP_IMPULSE_USE ); }
	void TakeDamage( entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType );

	void Spawn( void );
	void Think( void );
	void Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );

	virtual BOOL IsAlive( void ) { return FALSE; }

	int m_animate;
};

class CGenericCycler : public CCycler
{
public:
	void Spawn( void ) { GenericCyclerSpawn( ( char * )STRING( pev->model ), Vector( -16, -16, 0 ), Vector( 16, 16, 72 ) ); }
};

LINK_ENTITY_TO_CLASS( cycler, CGenericCycler );

void CCycler::GenericCyclerSpawn( char *szModel, Vector vecMin, Vector vecMax )
{
	if ( !szModel || !*szModel )
	{
		ALERT( at_error, "cycler at %.0f %.0f %0.f missing modelname", pev->origin.x, pev->origin.y, pev->origin.z );
		REMOVE_ENTITY( ENT( pev ) );
		return;
	}

	pev->classname = MAKE_STRING( "cycler" );
	PRECACHE_MODEL( szModel );
	SET_MODEL( ENT( pev ), szModel );

	CCycler::Spawn();

	UTIL_SetSize( pev, vecMin, vecMax );
}

void CCycler::Spawn( void )
{
	InitBoneControllers();

	pev->solid = SOLID_SLIDEBOX;
	pev->movetype = MOVETYPE_NONE;
	pev->takedamage = DAMAGE_YES;
	pev->effects = 0;
	pev->health = 80000;
	pev->yaw_speed = 5;
	pev->ideal_yaw = pev->angles.y;

	m_flFrameRate = 75;
	m_flGroundSpeed	= 0;

	pev->nextthink += 1.0;

	ResetSequenceInfo();

	if ( pev->sequence != 0 || pev->frame != 0 )
	{
		m_animate = 0;
		pev->framerate = 0;
	}
	else
		m_animate = 1;
}

void CCycler::Think( void )
{
	pev->nextthink = gpGlobals->time + 0.1;

	if ( m_animate )
		StudioFrameAdvance();

	if ( m_fSequenceFinished && !m_fSequenceLoops )
	{
		pev->animtime = gpGlobals->time;
		pev->framerate = 1.0;
		m_fSequenceFinished = FALSE;
		m_flLastEventCheck = gpGlobals->time;
		pev->frame = 0;

		if ( !m_animate )
			pev->framerate = 0.0;
	}
}

void CCycler::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	m_animate = !m_animate;

	if ( m_animate )
		pev->framerate = 1.0;
	else
		pev->framerate = 0.0;
}

void CCycler::TakeDamage( entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType )
{
	if ( m_animate )
	{
		pev->sequence++;

		ResetSequenceInfo();

		if ( m_flFrameRate == 0.0 )
		{
			pev->sequence = 0;
			ResetSequenceInfo();
		}

		pev->frame = 0;
	}
	else
	{
		pev->framerate = 1.0;
		StudioFrameAdvance( 0.1 );
		pev->framerate = 0;
		ALERT( at_console, "sequence: %d, frame %.0f\n", pev->sequence, pev->frame );
	}
}

class CCyclerSprite : public CBaseEntity
{
public:
	void Spawn( void );
	void Think( void );
	void Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );

	virtual int ObjectCaps( void ) { return ( CBaseEntity::ObjectCaps() | FCAP_DONT_SAVE | FCAP_IMPULSE_USE ); }

	virtual void TakeDamage( entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType );

	void Animate( float frames );

	inline int ShouldAnimate( void ) { return m_animate && m_maxFrame > 1.0; }

	int m_animate;
	float m_lastTime;
	float m_maxFrame;
};

LINK_ENTITY_TO_CLASS( cycler_sprite, CCyclerSprite );

void CCyclerSprite::Spawn( void )
{
	pev->solid = SOLID_SLIDEBOX;
	pev->movetype = MOVETYPE_NONE;
	pev->takedamage = DAMAGE_YES;
	pev->effects = 0;

	pev->frame = 0;
	pev->nextthink = gpGlobals->time + 0.1;
	m_animate = 1;
	m_lastTime = gpGlobals->time;

	PRECACHE_MODEL( ( char * )STRING( pev->model ) );
	SET_MODEL( ENT( pev ), STRING( pev->model ) );

	m_maxFrame = ( float )MODEL_FRAMES( pev->modelindex ) - 1;
}

void CCyclerSprite::Think( void )
{
	if ( ShouldAnimate() )
		Animate( pev->framerate * ( gpGlobals->time - m_lastTime ) );

	pev->nextthink = gpGlobals->time + 0.1;
	m_lastTime = gpGlobals->time;
}

void CCyclerSprite::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	m_animate = !m_animate;
	ALERT( at_console, "Sprite: %s\n", STRING( pev->model ) );
}

void CCyclerSprite::TakeDamage( entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType )
{
	if ( m_maxFrame > 1.0 )
		Animate( 1.0 );
}

void CCyclerSprite::Animate( float frames )
{
	pev->frame += frames;

	if ( m_maxFrame > 0 )
		pev->frame = fmod( pev->frame, m_maxFrame );
}

class CWreckage : public CBaseAnimating
{
public:
	void Spawn( void );
	void Precache( void );
	void Think( void );

	int m_flStartTime;
};

LINK_ENTITY_TO_CLASS( cycler_wreckage, CWreckage );

void CWreckage::Spawn( void )
{
	pev->solid = SOLID_NOT;
	pev->movetype = MOVETYPE_NONE;
	pev->takedamage = 0;
	pev->effects = 0;

	pev->frame = 0;
	pev->nextthink = gpGlobals->time + 0.1;

	if ( pev->model )
	{
		PRECACHE_MODEL( ( char * )STRING( pev->model ) );
		SET_MODEL( ENT( pev ), STRING( pev->model ) );
	}

	m_flStartTime = gpGlobals->time;
}

void CWreckage::Precache( void )
{
	if ( pev->model )
		PRECACHE_MODEL( ( char * )STRING( pev->model ) );
}

void CWreckage::Think( void )
{
	StudioFrameAdvance();

	pev->nextthink = gpGlobals->time + 0.2;

	if ( pev->dmgtime )
	{
		if ( pev->dmgtime < gpGlobals->time )
		{
			UTIL_Remove( this );
			return;
		}
		else if ( RANDOM_FLOAT( 0, pev->dmgtime - m_flStartTime ) > pev->dmgtime - gpGlobals->time )
			return;
	}

	Vector VecSrc;

	VecSrc.x = RANDOM_FLOAT( pev->absmin.x, pev->absmax.x );
	VecSrc.y = RANDOM_FLOAT( pev->absmin.y, pev->absmax.y );
	VecSrc.z = RANDOM_FLOAT( pev->absmin.z, pev->absmax.z );

	MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, VecSrc );
	WRITE_BYTE( TE_SMOKE );
	WRITE_COORD( VecSrc.x );
	WRITE_COORD( VecSrc.y );
	WRITE_COORD( VecSrc.z );
	WRITE_SHORT( g_sModelIndexSmoke );
	WRITE_BYTE( RANDOM_LONG( 0, 49 ) + 50 );
	WRITE_BYTE( RANDOM_LONG( 0, 3 ) + 8 );
	MESSAGE_END();
}