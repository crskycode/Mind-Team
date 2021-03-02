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
#include "customentity.h"
#include "effects.h"
#include "weapons.h"
#include "func_break.h"
#include "shake.h"

#define SF_GIBSHOOTER_REPEATABLE	1

#define SF_FUNNEL_REVERSE	1

LINK_ENTITY_TO_CLASS( info_target, CPointEntity );

class CBubbling : public CBaseEntity
{
public:
	void Spawn( void );
	void Precache( void );
	void KeyValue( KeyValueData *pkvd );

	void FizzThink( void );
	void Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );

	int m_density;
	int m_frequency;
	int m_bubbleModel;
	int m_state;
};

LINK_ENTITY_TO_CLASS( env_bubbles, CBubbling );

#define SF_BUBBLES_STARTOFF	0x0001

void CBubbling::Spawn( void )
{
	Precache();
	SET_MODEL( ENT( pev ), STRING( pev->model ) );

	pev->solid = SOLID_NOT;
	pev->renderamt = 0;
	pev->rendermode = kRenderTransTexture;
	int speed = pev->speed > 0 ? pev->speed : -pev->speed;

	pev->rendercolor.x = speed >> 8;
	pev->rendercolor.y = speed & 255;
	pev->rendercolor.z = ( pev->speed < 0 ) ? 1 : 0;

	if ( !( pev->spawnflags & SF_BUBBLES_STARTOFF ) )
	{
		SetThink( &CBubbling::FizzThink );
		pev->nextthink = gpGlobals->time + 2.0;
		m_state = 1;
	}
	else
		m_state = 0;
}

void CBubbling::Precache( void )
{
	m_bubbleModel = PRECACHE_MODEL( "sprites/bubble.spr" );
}

void CBubbling::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	if ( ShouldToggle( useType, m_state ) )
		m_state = !m_state;

	if ( m_state )
	{
		SetThink( &CBubbling::FizzThink );
		pev->nextthink = gpGlobals->time + 0.1;
	}
	else
	{
		SetThink( NULL );
		pev->nextthink = 0;
	}
}

void CBubbling::KeyValue( KeyValueData *pkvd )
{
	if ( FStrEq( pkvd->szKeyName, "density" ) )
	{
		m_density = atoi( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else if ( FStrEq( pkvd->szKeyName, "frequency" ) )
	{
		m_frequency = atoi( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else if ( FStrEq( pkvd->szKeyName, "current" ) )
	{
		pev->speed = atoi( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else
		CBaseEntity::KeyValue( pkvd );
}

void CBubbling::FizzThink( void )
{
	MESSAGE_BEGIN( MSG_PAS, SVC_TEMPENTITY, VecBModelOrigin( pev ) );
	WRITE_BYTE( TE_FIZZ );
	WRITE_SHORT( ( short )ENTINDEX( edict() ) );
	WRITE_SHORT( ( short )m_bubbleModel );
	WRITE_BYTE( m_density );
	MESSAGE_END();

	if ( m_frequency > 19 )
		pev->nextthink = gpGlobals->time + 0.5;
	else
		pev->nextthink = gpGlobals->time + 2.5 - ( 0.1 * m_frequency );
}

LINK_ENTITY_TO_CLASS( beam, CBeam );

void CBeam::Spawn( void )
{
	pev->solid = SOLID_NOT;
	Precache();
}

void CBeam::Precache( void )
{
	if ( pev->owner )
		SetStartEntity( ENTINDEX( pev->owner ) );

	if ( pev->aiment )
		SetEndEntity( ENTINDEX( pev->aiment ) );
}

void CBeam::SetStartEntity( int entityIndex )
{
	pev->sequence = ( entityIndex & 0x0FFF ) | ( ( pev->sequence & 0xF000 ) << 12 );
	pev->owner = g_engfuncs.pfnPEntityOfEntIndex( entityIndex );
}

void CBeam::SetEndEntity( int entityIndex )
{
	pev->skin = ( entityIndex & 0x0FFF ) | ( ( pev->skin & 0xF000 ) << 12 );
	pev->aiment = g_engfuncs.pfnPEntityOfEntIndex( entityIndex );
}

Vector &CBeam::GetStartPos( void )
{
	if ( GetType() == BEAM_ENTS )
	{
		edict_t *pent = g_engfuncs.pfnPEntityOfEntIndex( GetStartEntity() );
		return pent->v.origin;
	}

	return pev->origin;
}

Vector &CBeam::GetEndPos( void )
{
	int type = GetType();

	if ( type == BEAM_POINTS || type == BEAM_HOSE )
		return pev->angles;

	edict_t *pent = g_engfuncs.pfnPEntityOfEntIndex( GetEndEntity() );

	if ( pent )
		return pent->v.origin;

	return pev->angles;
}

CBeam *CBeam::BeamCreate( const char *pSpriteName, int width )
{
	CBeam *pBeam = GetClassPtr( ( CBeam * )NULL );

	pBeam->pev->classname = MAKE_STRING( "beam" );

	pBeam->BeamInit( pSpriteName, width );

	return pBeam;
}

void CBeam::BeamInit( const char *pSpriteName, int width )
{
	pev->flags |= FL_CUSTOMENTITY;

	SetColor( 255, 255, 255 );
	SetBrightness( 255 );
	SetNoise( 0 );
	SetFrame( 0 );
	SetScrollRate( 0 );

	pev->model = MAKE_STRING( pSpriteName );

	SetTexture( PRECACHE_MODEL( ( char * )pSpriteName ) );
	SetWidth( width );

	pev->skin = 0;
	pev->sequence = 0;
	pev->rendermode = 0;
}

void CBeam::PointsInit( Vector &start, Vector &end )
{
	SetType( BEAM_POINTS );
	SetStartPos( start );
	SetEndPos( end );
	SetStartAttachment( 0 );
	SetEndAttachment( 0 );
	RelinkBeam();
}

void CBeam::HoseInit( Vector &start, Vector &direction )
{
	SetType( BEAM_HOSE );
	SetStartPos( start );
	SetEndPos( direction );
	SetStartAttachment( 0 );
	SetEndAttachment( 0 );
	RelinkBeam();
}

void CBeam::PointEntInit( Vector &start, int endIndex )
{
	SetType( BEAM_ENTPOINT );
	SetStartPos( start );
	SetEndEntity( endIndex );
	SetStartAttachment( 0 );
	SetEndAttachment( 0 );
	RelinkBeam();
}

void CBeam::EntsInit( int startIndex, int endIndex )
{
	SetType( BEAM_ENTS );
	SetStartEntity( startIndex );
	SetEndEntity( endIndex );
	SetStartAttachment( 0 );
	SetEndAttachment( 0 );
	RelinkBeam();
}

void CBeam::RelinkBeam( void )
{
	Vector &startPos = GetStartPos(), &endPos = GetEndPos();

	pev->mins.x = min( startPos.x, endPos.x );
	pev->mins.y = min( startPos.y, endPos.y );
	pev->mins.z = min( startPos.z, endPos.z );
	pev->maxs.x = max( startPos.x, endPos.x );
	pev->maxs.y = max( startPos.y, endPos.y );
	pev->maxs.z = max( startPos.z, endPos.z );
	pev->mins = pev->mins - pev->origin;
	pev->maxs = pev->maxs - pev->origin;

	UTIL_SetSize( pev, pev->mins, pev->maxs );
	UTIL_SetOrigin( pev, pev->origin );
}

void CBeam::TriggerTouch( CBaseEntity *pOther )
{
	if ( pOther->pev->flags & ( FL_CLIENT | FL_MONSTER ) )
	{
		if ( pev->owner )
		{
			CBaseEntity *pOwner = CBaseEntity::Instance( pev->owner );
			pOwner->Use( pOther, this, USE_TOGGLE, 0 );
		}

		ALERT( at_console, "Firing targets!!!\n" );
	}
}

CBaseEntity *CBeam::RandomTargetname( const char *szName )
{
	int total = 0;

	CBaseEntity *pEntity = NULL;
	CBaseEntity *pNewEntity = NULL;

	while ( ( pNewEntity = UTIL_FindEntityByTargetname( pNewEntity, szName ) ) != NULL )
	{
		total++;

		if ( RANDOM_LONG( 0, total - 1 ) < 1 )
			pEntity = pNewEntity;
	}

	return pEntity;
}

void CBeam::DoSparks( Vector &start, Vector &end )
{
	if ( pev->spawnflags & ( SF_BEAM_SPARKSTART | SF_BEAM_SPARKEND ) )
	{
		if ( pev->spawnflags & SF_BEAM_SPARKSTART )
			UTIL_Sparks( start );

		if ( pev->spawnflags & SF_BEAM_SPARKEND )
			UTIL_Sparks( end );
	}
}

class CLightning : public CBeam
{
public:
	void Spawn( void );
	void Precache( void );
	void KeyValue( KeyValueData *pkvd );
	void Activate( void );

	void StrikeThink( void );
	void DamageThink( void );
	void RandomArea( void );
	void RandomPoint( Vector &vecSrc );
	void Zap( Vector &vecSrc, Vector &vecDest );
	void StrikeUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	void ToggleUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );

	inline BOOL ServerSide( void )
	{
		if ( m_life == 0 && !( pev->spawnflags & SF_BEAM_RING ) )
			return TRUE;

		return FALSE;
	}

	void BeamUpdateVars( void );

	int m_active;
	int m_iszStartEntity;
	int m_iszEndEntity;
	float m_life;
	int m_boltWidth;
	int m_noiseAmplitude;
	int m_brightness;
	int m_speed;
	float m_restrike;
	int m_spriteTexture;
	int m_iszSpriteName;
	int m_frameStart;

	float m_radius;
};

LINK_ENTITY_TO_CLASS( env_lightning, CLightning );
LINK_ENTITY_TO_CLASS( env_beam, CLightning );

void CLightning::Spawn( void )
{
	if ( FStringNull( m_iszSpriteName ) )
	{
		SetThink( &CBaseEntity::SUB_Remove );
		return;
	}

	pev->solid = SOLID_NOT;

	Precache();

	pev->dmgtime = gpGlobals->time;

	if ( ServerSide() )
	{
		SetThink( NULL );

		if ( pev->dmg > 0 )
		{
			SetThink( &CLightning::DamageThink );
			pev->nextthink = gpGlobals->time + 0.1;
		}

		if ( pev->targetname )
		{
			if ( !( pev->spawnflags & SF_BEAM_STARTON ) )
			{
				pev->effects = EF_NODRAW;
				m_active = 0;
				pev->nextthink = 0;
			}
			else
				m_active = 1;

			SetUse( &CLightning::ToggleUse );
		}
	}
	else
	{
		m_active = 0;

		if ( !FStringNull( pev->targetname ) )
			SetUse( &CLightning::StrikeUse );

		if ( FStringNull( pev->targetname ) || FBitSet( pev->spawnflags, SF_BEAM_STARTON ) )
		{
			SetThink( &CLightning::StrikeThink );
			pev->nextthink = gpGlobals->time + 1.0;
		}
	}
}

void CLightning::Precache( void )
{
	m_spriteTexture = PRECACHE_MODEL( ( char * )STRING( m_iszSpriteName ) );
	CBeam::Precache();
}

void CLightning::Activate( void )
{
	if ( ServerSide() )
		BeamUpdateVars();
}

void CLightning::KeyValue( KeyValueData *pkvd )
{
	if ( FStrEq( pkvd->szKeyName, "LightningStart" ) )
	{
		m_iszStartEntity = ALLOC_STRING( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else if ( FStrEq( pkvd->szKeyName, "LightningEnd" ) )
	{
		m_iszEndEntity = ALLOC_STRING( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else if ( FStrEq( pkvd->szKeyName, "life" ) )
	{
		m_life = atof( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else if ( FStrEq( pkvd->szKeyName, "BoltWidth" ) )
	{
		m_boltWidth = atoi( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else if ( FStrEq( pkvd->szKeyName, "NoiseAmplitude" ) )
	{
		m_noiseAmplitude = atoi( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else if ( FStrEq( pkvd->szKeyName, "TextureScroll" ) )
	{
		m_speed = atoi( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else if ( FStrEq( pkvd->szKeyName, "StrikeTime" ) )
	{
		m_restrike = atof( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else if ( FStrEq( pkvd->szKeyName, "texture" ) )
	{
		m_iszSpriteName = ALLOC_STRING( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else if ( FStrEq( pkvd->szKeyName, "framestart" ) )
	{
		m_frameStart = atoi( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else if ( FStrEq( pkvd->szKeyName, "Radius" ) )
	{
		m_radius = atof( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else if ( FStrEq( pkvd->szKeyName, "damage" ) )
	{
		pev->dmg = atof( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else
		CBeam::KeyValue( pkvd );
}

void CLightning::ToggleUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	if ( !ShouldToggle( useType, m_active ) )
		return;

	if ( m_active )
	{
		m_active = 0;
		pev->effects |= EF_NODRAW;
		pev->nextthink = 0;
	}
	else
	{
		m_active = 1;
		pev->effects &= ~EF_NODRAW;

		DoSparks( GetStartPos(), GetEndPos() );

		if ( pev->dmg > 0 )
		{
			pev->nextthink = gpGlobals->time;
			pev->dmgtime = gpGlobals->time;
		}
	}
}

void CLightning::StrikeUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	if ( !ShouldToggle( useType, m_active ) )
		return;

	if ( m_active )
	{
		m_active = 0;
		SetThink( NULL );
	}
	else
	{
		SetThink( &CLightning::StrikeThink );
		pev->nextthink = gpGlobals->time + 0.1;
	}

	if ( !FBitSet( pev->spawnflags, SF_BEAM_TOGGLE ) )
		SetUse( NULL );
}

int IsPointEntity( CBaseEntity *pEnt )
{
	if ( !pEnt->pev->modelindex )
		return 1;

	if ( FClassnameIs( pEnt->pev, "info_target" ) || FClassnameIs( pEnt->pev, "info_landmark" ) || FClassnameIs( pEnt->pev, "path_corner" ) )
		return 1;

	return 0;
}

void CLightning::StrikeThink( void )
{
	if ( m_life != 0 )
	{
		if ( pev->spawnflags & SF_BEAM_RANDOM )
			pev->nextthink = gpGlobals->time + m_life + RANDOM_FLOAT( 0, m_restrike );
		else
			pev->nextthink = gpGlobals->time + m_life + m_restrike;
	}

	m_active = 1;

	if ( FStringNull( m_iszEndEntity ) )
	{
		if ( FStringNull( m_iszStartEntity ) )
			RandomArea();
		else
		{
			CBaseEntity *pStart = RandomTargetname( STRING( m_iszStartEntity ) );

			if ( pStart != NULL )
				RandomPoint( pStart->pev->origin );
			else
				ALERT( at_console, "env_beam: unknown entity \"%s\"\n", STRING( m_iszStartEntity ) );
		}

		return;
	}

	CBaseEntity *pStart = RandomTargetname( STRING( m_iszStartEntity ) );
	CBaseEntity *pEnd = RandomTargetname( STRING( m_iszEndEntity ) );

	if ( pStart != NULL && pEnd != NULL )
	{
		if ( IsPointEntity( pStart ) || IsPointEntity( pEnd ) )
		{
			if ( pev->spawnflags & SF_BEAM_RING )
				return;
		}

		MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );

		if ( IsPointEntity( pStart ) || IsPointEntity( pEnd ) )
		{
			if ( !IsPointEntity( pEnd ) )
			{
				CBaseEntity *pTemp;
				pTemp = pStart;
				pStart = pEnd;
				pEnd = pTemp;
			}

			if ( !IsPointEntity( pStart ) )
			{
				WRITE_BYTE( TE_BEAMENTPOINT );
				WRITE_SHORT( pStart->entindex() );
				WRITE_COORD( pEnd->pev->origin.x );
				WRITE_COORD( pEnd->pev->origin.y );
				WRITE_COORD( pEnd->pev->origin.z );
			}
			else
			{
				WRITE_BYTE( TE_BEAMPOINTS );
				WRITE_COORD( pStart->pev->origin.x );
				WRITE_COORD( pStart->pev->origin.y );
				WRITE_COORD( pStart->pev->origin.z );
				WRITE_COORD( pEnd->pev->origin.x );
				WRITE_COORD( pEnd->pev->origin.y );
				WRITE_COORD( pEnd->pev->origin.z );
			}
		}
		else
		{
			if ( pev->spawnflags & SF_BEAM_RING )
				WRITE_BYTE( TE_BEAMRING );
			else
				WRITE_BYTE( TE_BEAMENTS );

			WRITE_SHORT( pStart->entindex() );
			WRITE_SHORT( pEnd->entindex() );
		}

		WRITE_SHORT( m_spriteTexture );
		WRITE_BYTE( m_frameStart );
		WRITE_BYTE( ( int )pev->framerate );
		WRITE_BYTE( ( int )( m_life * 10.0 ) );
		WRITE_BYTE( m_boltWidth );
		WRITE_BYTE( m_noiseAmplitude );
		WRITE_BYTE( ( int )pev->rendercolor.x );
		WRITE_BYTE( ( int )pev->rendercolor.y );
		WRITE_BYTE( ( int )pev->rendercolor.z );
		WRITE_BYTE( pev->renderamt );
		WRITE_BYTE( m_speed );

		MESSAGE_END();

		DoSparks( pStart->pev->origin, pEnd->pev->origin );

		if ( pev->dmg > 0 )
		{
			TraceResult tr;
			UTIL_TraceLine( pStart->pev->origin, pEnd->pev->origin, dont_ignore_monsters, NULL, &tr );
			BeamDamageInstant( &tr, pev->dmg );
		}
	}
}

void CBeam::BeamDamage( TraceResult *ptr )
{
	RelinkBeam();

	if ( ptr->flFraction != 1.0 && ptr->pHit != NULL )
	{
		CBaseEntity *pHit = CBaseEntity::Instance( ptr->pHit );

		if ( pHit )
		{
			ClearMultiDamage();
			pHit->TraceAttack( pev, pev->dmg * ( gpGlobals->time - pev->dmgtime ), ( ptr->vecEndPos - pev->origin ).Normalize(), ptr, DMG_ENERGYBEAM );
			ApplyMultiDamage( pev, pev );

			if ( pev->spawnflags & SF_BEAM_DECALS )
			{
			}
		}
	}

	pev->dmgtime = gpGlobals->time;
}

void CLightning::DamageThink( void )
{
	pev->nextthink = gpGlobals->time + 0.1;

	TraceResult tr;
	UTIL_TraceLine( GetStartPos(), GetEndPos(), dont_ignore_monsters, NULL, &tr );
	BeamDamage( &tr );
}

void CLightning::Zap( Vector &vecSrc, Vector &vecDest )
{
	MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
	WRITE_BYTE( TE_BEAMPOINTS );
	WRITE_COORD( vecSrc.x );
	WRITE_COORD( vecSrc.y );
	WRITE_COORD( vecSrc.z );
	WRITE_COORD( vecDest.x );
	WRITE_COORD( vecDest.y );
	WRITE_COORD( vecDest.z );
	WRITE_SHORT( m_spriteTexture );
	WRITE_BYTE( m_frameStart );
	WRITE_BYTE( ( int )pev->framerate );
	WRITE_BYTE( ( int )( m_life * 10.0 ) );
	WRITE_BYTE( m_boltWidth );
	WRITE_BYTE( m_noiseAmplitude );
	WRITE_BYTE( ( int )pev->rendercolor.x );
	WRITE_BYTE( ( int )pev->rendercolor.y );
	WRITE_BYTE( ( int )pev->rendercolor.z );
	WRITE_BYTE( pev->renderamt );
	WRITE_BYTE( m_speed );
	MESSAGE_END();

	DoSparks( vecSrc, vecDest );
}

void CLightning::RandomArea( void )
{
	int iLoops = 0;

	for ( iLoops = 0; iLoops < 10; iLoops++ )
	{
		Vector vecSrc = pev->origin;

		Vector vecDir1 = Vector( RANDOM_FLOAT( -1.0, 1.0 ), RANDOM_FLOAT( -1.0, 1.0 ), RANDOM_FLOAT( -1.0, 1.0 ) );
		vecDir1 = vecDir1.Normalize();

		TraceResult tr1;
		UTIL_TraceLine( vecSrc, vecSrc + vecDir1 * m_radius, ignore_monsters, ENT( pev ), &tr1 );

		if ( tr1.flFraction == 1.0 )
			continue;

		Vector vecDir2;

		do
		{
			vecDir2 = Vector( RANDOM_FLOAT( -1.0, 1.0 ), RANDOM_FLOAT( -1.0, 1.0 ), RANDOM_FLOAT( -1.0, 1.0 ) );

		}
		while ( DotProduct( vecDir1, vecDir2 ) > 0 );

		vecDir2 = vecDir2.Normalize();

		TraceResult tr2;
		UTIL_TraceLine( vecSrc, vecSrc + vecDir2 * m_radius, ignore_monsters, ENT( pev ), &tr2 );

		if ( tr2.flFraction == 1.0 )
			continue;

		if ( ( tr1.vecEndPos - tr2.vecEndPos ).Length() < m_radius * 0.1 )
			continue;

		UTIL_TraceLine( tr1.vecEndPos, tr2.vecEndPos, ignore_monsters, ENT( pev ), &tr2 );

		if ( tr2.flFraction != 1.0 )
			continue;

		Zap( tr1.vecEndPos, tr2.vecEndPos );

		break;
	}
}

void CLightning::RandomPoint( Vector &vecSrc )
{
	int iLoops = 0;

	for ( iLoops = 0; iLoops < 10; iLoops++ )
	{
		Vector vecDir1 = Vector( RANDOM_FLOAT( -1.0, 1.0 ), RANDOM_FLOAT( -1.0, 1.0 ), RANDOM_FLOAT( -1.0, 1.0 ) );
		vecDir1 = vecDir1.Normalize();

		TraceResult tr1;
		UTIL_TraceLine( vecSrc, vecSrc + vecDir1 * m_radius, ignore_monsters, ENT( pev ), &tr1 );

		if ( ( tr1.vecEndPos - vecSrc ).Length() < m_radius * 0.1 )
			continue;

		if ( tr1.flFraction == 1.0 )
			continue;

		Zap( vecSrc, tr1.vecEndPos );

		break;
	}
}

void CLightning::BeamUpdateVars( void )
{
	int beamType;
	int pointStart, pointEnd;

	edict_t *pStart = FIND_ENTITY_BY_TARGETNAME( NULL, STRING( m_iszStartEntity ) );
	edict_t *pEnd = FIND_ENTITY_BY_TARGETNAME( NULL, STRING( m_iszEndEntity ) );

	pointStart = IsPointEntity( CBaseEntity::Instance( pStart ) );
	pointEnd = IsPointEntity( CBaseEntity::Instance( pEnd ) );

	pev->skin = 0;
	pev->sequence = 0;
	pev->rendermode = 0;
	pev->flags |= FL_CUSTOMENTITY;
	pev->model = m_iszSpriteName;

	SetTexture( m_spriteTexture );

	beamType = BEAM_ENTS;

	if ( pointStart || pointEnd )
	{
		if ( !pointStart )
		{
			edict_t *pTemp;
			pTemp = pStart;
			pStart = pEnd;
			pEnd = pTemp;
			int swap = pointStart;
			pointStart = pointEnd;
			pointEnd = swap;
		}

		if ( !pointEnd )
			beamType = BEAM_ENTPOINT;
		else
			beamType = BEAM_POINTS;
	}

	SetType( beamType );

	if ( beamType == BEAM_POINTS || beamType == BEAM_ENTPOINT || beamType == BEAM_HOSE )
	{
		SetStartPos( pStart->v.origin );

		if ( beamType == BEAM_POINTS || beamType == BEAM_HOSE )
			SetEndPos( pEnd->v.origin );
		else
			SetEndEntity( ENTINDEX( pEnd ) );
	}
	else
	{
		SetStartEntity( ENTINDEX( pStart ) );
		SetEndEntity( ENTINDEX( pEnd ) );
	}

	RelinkBeam();

	SetWidth( m_boltWidth );
	SetNoise( m_noiseAmplitude );
	SetFrame( m_frameStart );
	SetScrollRate( m_speed );

	if ( pev->spawnflags & SF_BEAM_SHADEIN )
		SetFlags( BEAM_FSHADEIN );
	else if ( pev->spawnflags & SF_BEAM_SHADEOUT )
		SetFlags( BEAM_FSHADEOUT );
}

LINK_ENTITY_TO_CLASS( env_laser, CLaser );

void CLaser::Spawn( void )
{
	if ( FStringNull( pev->model ) )
	{
		SetThink( &CBaseEntity::SUB_Remove );
		return;
	}

	pev->solid = SOLID_NOT;

	Precache();

	SetThink( &CLaser::StrikeThink );
	pev->flags |= FL_CUSTOMENTITY;

	PointsInit( pev->origin, pev->origin );

	if ( !m_pSprite && m_iszSpriteName )
		m_pSprite = CSprite::SpriteCreate( STRING( m_iszSpriteName ), pev->origin, TRUE );
	else
		m_pSprite = NULL;

	if ( m_pSprite )
		m_pSprite->SetTransparency( kRenderGlow, pev->rendercolor.x, pev->rendercolor.y, pev->rendercolor.z, pev->renderamt, pev->renderfx );

	if ( pev->targetname && !( pev->spawnflags & SF_BEAM_STARTON ) )
		TurnOff();
	else
		TurnOn();
}

void CLaser::Precache( void )
{
	pev->modelindex = PRECACHE_MODEL( ( char * )STRING( pev->model ) );

	if ( m_iszSpriteName )
		PRECACHE_MODEL( ( char * )STRING( m_iszSpriteName ) );
}

void CLaser::KeyValue( KeyValueData *pkvd )
{
	if ( FStrEq( pkvd->szKeyName, "LaserTarget" ) )
	{
		pev->message = ALLOC_STRING( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else if ( FStrEq( pkvd->szKeyName, "width" ) )
	{
		SetWidth( atof( pkvd->szValue ) );
		pkvd->fHandled = TRUE;
	}
	else if ( FStrEq( pkvd->szKeyName, "NoiseAmplitude" ) )
	{
		SetNoise( atoi( pkvd->szValue ) );
		pkvd->fHandled = TRUE;
	}
	else if ( FStrEq( pkvd->szKeyName, "TextureScroll" ) )
	{
		SetScrollRate( atoi( pkvd->szValue ) );
		pkvd->fHandled = TRUE;
	}
	else if ( FStrEq( pkvd->szKeyName, "texture" ) )
	{
		pev->model = ALLOC_STRING( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else if ( FStrEq( pkvd->szKeyName, "EndSprite" ) )
	{
		m_iszSpriteName = ALLOC_STRING( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else if ( FStrEq( pkvd->szKeyName, "framestart" ) )
	{
		pev->frame = atoi( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else if ( FStrEq( pkvd->szKeyName, "damage" ) )
	{
		pev->dmg = atof( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else
		CBeam::KeyValue( pkvd );
}

int CLaser::IsOn( void )
{
	if ( pev->effects & EF_NODRAW )
		return 0;

	return 1;
}

void CLaser::TurnOff( void )
{
	pev->effects |= EF_NODRAW;
	pev->nextthink = 0;

	if ( m_pSprite )
		m_pSprite->TurnOff();
}

void CLaser::TurnOn( void )
{
	pev->effects &= ~EF_NODRAW;

	if ( m_pSprite )
		m_pSprite->TurnOn();

	pev->dmgtime = gpGlobals->time;
	pev->nextthink = gpGlobals->time;
}

void CLaser::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	int active = IsOn();

	if ( !ShouldToggle( useType, active ) )
		return;

	if ( active )
		TurnOff();
	else
		TurnOn();
}

void CLaser::FireAtPoint( TraceResult &tr )
{
	SetEndPos( tr.vecEndPos );

	if ( m_pSprite )
		UTIL_SetOrigin( m_pSprite->pev, tr.vecEndPos );

	BeamDamage( &tr );
	DoSparks( GetStartPos(), tr.vecEndPos );
}

void CLaser::StrikeThink( void )
{
	CBaseEntity *pEnd = RandomTargetname( STRING( pev->message ) );

	if ( pEnd )
		m_firePosition = pEnd->pev->origin;

	TraceResult tr;
	UTIL_TraceLine( pev->origin, m_firePosition, dont_ignore_monsters, NULL, &tr );

	FireAtPoint( tr );

	pev->nextthink = gpGlobals->time + 0.1;
}

class CGlow : public CPointEntity
{
public:
	void Spawn( void );
	void Think( void );

	void Animate( float frames );

	float m_lastTime;
	float m_maxFrame;
};

LINK_ENTITY_TO_CLASS( env_glow, CGlow );

void CGlow::Spawn( void )
{
	pev->solid = SOLID_NOT;
	pev->movetype = MOVETYPE_NONE;
	pev->effects = 0;
	pev->frame = 0;

	PRECACHE_MODEL( ( char * )STRING( pev->model ) );
	SET_MODEL( ENT( pev ), STRING( pev->model ) );

	m_maxFrame = ( float )MODEL_FRAMES( pev->modelindex ) - 1;

	if ( m_maxFrame > 1.0 && pev->framerate != 0 )
		pev->nextthink = gpGlobals->time + 0.1;

	m_lastTime = gpGlobals->time;
}

void CGlow::Think( void )
{
	Animate( pev->framerate * ( gpGlobals->time - m_lastTime ) );

	pev->nextthink = gpGlobals->time + 0.1;
	m_lastTime = gpGlobals->time;
}

void CGlow::Animate( float frames )
{
	if ( m_maxFrame > 0 )
		pev->frame = fmod( pev->frame + frames, m_maxFrame );
}

LINK_ENTITY_TO_CLASS( env_sprite, CSprite );

void CSprite::Spawn( void )
{
	pev->solid = SOLID_NOT;
	pev->movetype = MOVETYPE_NONE;
	pev->effects = 0;
	pev->frame = 0;

	Precache();

	SET_MODEL( ENT( pev ), STRING( pev->model ) );

	m_maxFrame = ( float )MODEL_FRAMES( pev->modelindex ) - 1;

	if ( pev->targetname && !( pev->spawnflags & SF_SPRITE_STARTON ) )
		TurnOff();
	else
		TurnOn();

	if ( pev->angles.y != 0 && pev->angles.z == 0 )
	{
		pev->angles.z = pev->angles.y;
		pev->angles.y = 0;
	}
}

void CSprite::Precache( void )
{
	PRECACHE_MODEL( ( char * )STRING( pev->model ) );

	if ( pev->aiment )
		SetAttachment( pev->aiment, pev->body );
	else
	{
		pev->skin = 0;
		pev->body = 0;
	}
}

void CSprite::SpriteInit( const char *pSpriteName, Vector &origin )
{
	pev->model = MAKE_STRING( pSpriteName );
	pev->origin = origin;

	Spawn();
}

CSprite *CSprite::SpriteCreate( const char *pSpriteName, Vector &origin, BOOL animate )
{
	CSprite *pSprite = GetClassPtr( ( CSprite * )NULL );

	pSprite->SpriteInit( pSpriteName, origin );

	pSprite->pev->classname = MAKE_STRING( "env_sprite" );
	pSprite->pev->solid = SOLID_NOT;
	pSprite->pev->movetype = MOVETYPE_NOCLIP;

	if ( animate )
		pSprite->TurnOn();

	return pSprite;
}

void CSprite::AnimateThink( void )
{
	Animate( pev->framerate * ( gpGlobals->time - m_lastTime ) );

	pev->nextthink = gpGlobals->time + 0.1;
	m_lastTime = gpGlobals->time;
}

void CSprite::AnimateUntilDead( void )
{
	if ( gpGlobals->time > pev->dmgtime )
		UTIL_Remove( this );
	else
	{
		AnimateThink();
		pev->nextthink = gpGlobals->time;
	}
}

void CSprite::Expand( float scaleSpeed, float fadeSpeed )
{
	pev->speed = scaleSpeed;
	pev->health = fadeSpeed;

	SetThink( &CSprite::ExpandThink );

	pev->nextthink = gpGlobals->time;
	m_lastTime = gpGlobals->time;
}

void CSprite::ExpandThink( void )
{
	float frametime = gpGlobals->time - m_lastTime;

	pev->scale += pev->speed * frametime;
	pev->renderamt -= pev->health * frametime;

	if ( pev->renderamt <= 0 )
	{
		pev->renderamt = 0;
		UTIL_Remove( this );
	}
	else
	{
		pev->nextthink = gpGlobals->time + 0.1;
		m_lastTime = gpGlobals->time;
	}
}

void CSprite::Animate( float frames )
{
	pev->frame += frames;

	if ( pev->frame > m_maxFrame )
	{
		if ( pev->spawnflags & SF_SPRITE_ONCE )
			TurnOff();
		else
		{
			if ( m_maxFrame > 0 )
				pev->frame = fmod( pev->frame, m_maxFrame );
		}
	}
}

void CSprite::TurnOff( void )
{
	pev->effects = EF_NODRAW;
	pev->nextthink = 0;
}

void CSprite::TurnOn( void )
{
	pev->effects = 0;

	if ( ( pev->framerate && m_maxFrame > 1.0 ) || ( pev->spawnflags & SF_SPRITE_ONCE ) )
	{
		SetThink( &CSprite::AnimateThink );

		pev->nextthink = gpGlobals->time;
		m_lastTime = gpGlobals->time;
	}

	pev->frame = 0;
}

void CSprite::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	int on = pev->effects != EF_NODRAW;

	if ( ShouldToggle( useType, on ) )
	{
		if ( on )
			TurnOff();
		else
			TurnOn();
	}
}

class CShake : public CPointEntity
{
public:
	void Spawn( void );
	void Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	void KeyValue( KeyValueData *pkvd );

	inline float Amplitude( void ) { return pev->scale; }
	inline float Frequency( void ) { return pev->dmg_save; }
	inline float Duration( void ) { return pev->dmg_take; }
	inline float Radius( void ) { return pev->dmg; }

	inline void SetAmplitude( float amplitude ) { pev->scale = amplitude; }
	inline void SetFrequency( float frequency ) { pev->dmg_save = frequency; }
	inline void SetDuration( float duration ) { pev->dmg_take = duration; }
	inline void SetRadius( float radius ) { pev->dmg = radius; }
};

LINK_ENTITY_TO_CLASS( env_shake, CShake );

#define SF_SHAKE_EVERYONE	0x0001
#define SF_SHAKE_DISRUPT	0x0002
#define SF_SHAKE_INAIR		0x0004

void CShake::Spawn( void )
{
	pev->solid = SOLID_NOT;
	pev->movetype = MOVETYPE_NONE;
	pev->effects = 0;
	pev->frame = 0;

	if ( pev->spawnflags & SF_SHAKE_EVERYONE )
		pev->dmg = 0;
}

void CShake::KeyValue( KeyValueData *pkvd )
{
	if ( FStrEq( pkvd->szKeyName, "amplitude" ) )
	{
		SetAmplitude( atof( pkvd->szValue ) );
		pkvd->fHandled = TRUE;
	}
	else if ( FStrEq( pkvd->szKeyName, "frequency" ) )
	{
		SetFrequency( atof( pkvd->szValue ) );
		pkvd->fHandled = TRUE;
	}
	else if ( FStrEq( pkvd->szKeyName, "duration" ) )
	{
		SetDuration( atof( pkvd->szValue ) );
		pkvd->fHandled = TRUE;
	}
	else if ( FStrEq( pkvd->szKeyName, "radius" ) )
	{
		SetRadius( atof( pkvd->szValue ) );
		pkvd->fHandled = TRUE;
	}
	else
		CPointEntity::KeyValue( pkvd );
}

void CShake::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	UTIL_ScreenShake( pev->origin, Amplitude(), Frequency(), Duration(), Radius() );
}

class CFade : public CPointEntity
{
	void Spawn( void );
	void Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	void KeyValue( KeyValueData *pkvd );

	inline float Duration( void ) { return pev->dmg_take; }
	inline float HoldTime( void ) { return pev->dmg_save; }

	inline void SetDuration( float duration ) { pev->dmg_take = duration; }
	inline void SetHoldTime( float hold ) { pev->dmg_save = hold; }
};

LINK_ENTITY_TO_CLASS( env_fade, CFade );

#define SF_FADE_IN			0x0001
#define SF_FADE_MODULATE	0x0002
#define SF_FADE_ONLYONE		0x0004

void CFade::Spawn( void )
{
	pev->solid = SOLID_NOT;
	pev->movetype = MOVETYPE_NONE;
	pev->effects = 0;
	pev->frame = 0;
}

void CFade::KeyValue( KeyValueData *pkvd )
{
	if ( FStrEq( pkvd->szKeyName, "duration" ) )
	{
		SetDuration( atof( pkvd->szValue ) );
		pkvd->fHandled = TRUE;
	}
	else if ( FStrEq( pkvd->szKeyName, "holdtime" ) )
	{
		SetHoldTime( atof( pkvd->szValue ) );
		pkvd->fHandled = TRUE;
	}
	else
		CPointEntity::KeyValue( pkvd );
}

void CFade::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	int fadeFlags = 0;

	if ( !( pev->spawnflags & SF_FADE_IN ) )
		fadeFlags |= FFADE_OUT;

	if ( pev->spawnflags & SF_FADE_MODULATE )
		fadeFlags |= FFADE_MODULATE;

	if ( pev->spawnflags & SF_FADE_ONLYONE )
	{
		if ( pActivator->IsNetClient() )
			UTIL_ScreenFade( pActivator, pev->rendercolor, Duration(), HoldTime(), pev->renderamt, fadeFlags );
	}
	else
		UTIL_ScreenFadeAll( pev->rendercolor, Duration(), HoldTime(), pev->renderamt, fadeFlags );

	SUB_UseTargets( this, USE_TOGGLE, 0 );
}