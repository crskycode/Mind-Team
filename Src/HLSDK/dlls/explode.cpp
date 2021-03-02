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
#include "explode.h"

class CEnvExplosion : public CBaseEntity
{
public:
	void Spawn();
	void Smoke( void );
	void KeyValue( KeyValueData *pkvd );
	void Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );

	int m_iMagnitude;
	int m_spriteScale;
};

LINK_ENTITY_TO_CLASS( env_explosion, CEnvExplosion );

void CEnvExplosion::KeyValue( KeyValueData *pkvd )
{
	if ( FStrEq( pkvd->szKeyName, "iMagnitude" ) )
	{
		m_iMagnitude = atoi( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else
		CBaseEntity::KeyValue( pkvd );
}

void CEnvExplosion::Spawn( void )
{
	pev->solid = SOLID_NOT;
	pev->effects = EF_NODRAW;

	pev->movetype = MOVETYPE_NONE;

	float flSpriteScale;
	flSpriteScale = ( m_iMagnitude - 50 ) * 0.6;

	if ( flSpriteScale < 10 )
		flSpriteScale = 10;

	m_spriteScale = ( int )flSpriteScale;
}

void CEnvExplosion::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	TraceResult tr;

	pev->model = iStringNull;
	pev->solid = SOLID_NOT;

	Vector vecSpot;

	vecSpot = pev->origin + Vector( 0, 0, 8 );

	UTIL_TraceLine( vecSpot, vecSpot + Vector( 0, 0, -40 ), ignore_monsters, ENT( pev ), &tr );

	if ( tr.flFraction != 1.0 )
		pev->origin = tr.vecEndPos + ( tr.vecPlaneNormal * ( m_iMagnitude - 24 ) * 0.6 );
	else
		pev->origin = pev->origin;

	if ( !( pev->spawnflags & SF_ENVEXPLOSION_NODECAL ) )
	{
	}

	if ( !( pev->spawnflags & SF_ENVEXPLOSION_NOFIREBALL ) )
	{
		MESSAGE_BEGIN( MSG_PAS, SVC_TEMPENTITY, pev->origin );
		WRITE_BYTE( TE_EXPLOSION );
		WRITE_COORD( pev->origin.x );
		WRITE_COORD( pev->origin.y );
		WRITE_COORD( pev->origin.z );
		WRITE_SHORT( g_sModelIndexFireball );
		WRITE_BYTE( ( BYTE )m_spriteScale );
		WRITE_BYTE( 15 );
		WRITE_BYTE( TE_EXPLFLAG_NONE );
		MESSAGE_END();
	}
	else
	{
		MESSAGE_BEGIN( MSG_PAS, SVC_TEMPENTITY, pev->origin );
		WRITE_BYTE( TE_EXPLOSION );
		WRITE_COORD( pev->origin.x );
		WRITE_COORD( pev->origin.y );
		WRITE_COORD( pev->origin.z );
		WRITE_SHORT( g_sModelIndexFireball );
		WRITE_BYTE( 0 );
		WRITE_BYTE( 15 );
		WRITE_BYTE( TE_EXPLFLAG_NONE );
		MESSAGE_END();
	}

	if ( !( pev->spawnflags & SF_ENVEXPLOSION_NODAMAGE ) )
	{
	}

	SetThink( &CEnvExplosion::Smoke );
	pev->nextthink = gpGlobals->time + 0.3;

	if ( !( pev->spawnflags & SF_ENVEXPLOSION_NOSPARKS ) )
	{
		int sparkCount = RANDOM_LONG( 0, 3 );

		for ( int i = 0; i < sparkCount; i++ )
			Create( "spark_shower", pev->origin, tr.vecPlaneNormal, NULL );
	}
}

void CEnvExplosion::Smoke( void )
{
	if ( !( pev->spawnflags & SF_ENVEXPLOSION_NOSMOKE ) )
	{
		MESSAGE_BEGIN( MSG_PAS, SVC_TEMPENTITY, pev->origin );
		WRITE_BYTE( TE_SMOKE );
		WRITE_COORD( pev->origin.x );
		WRITE_COORD( pev->origin.y );
		WRITE_COORD( pev->origin.z );
		WRITE_SHORT( g_sModelIndexSmoke );
		WRITE_BYTE( ( BYTE )m_spriteScale );
		WRITE_BYTE( 12 );
		MESSAGE_END();
	}

	if ( !( pev->spawnflags & SF_ENVEXPLOSION_REPEATABLE ) )
		UTIL_Remove( this );
}

void ExplosionCreate( Vector &center, Vector &angles, edict_t *pOwner, int magnitude, BOOL doDamage )
{
	KeyValueData kvd;
	char buf[128];

	CBaseEntity *pExplosion = CBaseEntity::Create( "env_explosion", center, angles, pOwner );
	sprintf( buf, "%3d", magnitude );
	kvd.szKeyName = "iMagnitude";
	kvd.szValue = buf;
	pExplosion->KeyValue( &kvd );

	if ( !doDamage )
		pExplosion->pev->spawnflags |= SF_ENVEXPLOSION_NODAMAGE;

	pExplosion->Spawn();
	pExplosion->Use( NULL, NULL, USE_TOGGLE, 0 );
}