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
#include "animation.h"
#include "studio.h"

float CBaseAnimating::StudioFrameAdvance( float flInterval )
{
	if ( flInterval == 0.0 )
	{
		flInterval = ( gpGlobals->time - pev->animtime );

		if ( flInterval <= 0.001 )
		{
			pev->animtime = gpGlobals->time;
			return 0.0;
		}
	}

	if ( !pev->animtime )
		flInterval = 0.0;

	pev->frame += flInterval * m_flFrameRate * pev->framerate;
	pev->animtime = gpGlobals->time;

	if ( pev->frame < 0.0 || pev->frame >= 256.0 )
	{
		if ( m_fSequenceLoops )
			pev->frame -= ( int )( pev->frame / 256.0 ) * 256.0;
		else
			pev->frame = ( pev->frame < 0.0 ) ? 0 : 255;

		m_fSequenceFinished = TRUE;
	}

	return flInterval;
}

int CBaseAnimating::LookupActivity( int activity )
{
	void *pmodel = GET_MODEL_PTR( ENT( pev ) );
	return ::LookupActivity( pmodel, pev, activity );
}

int CBaseAnimating::LookupSequence( const char *label )
{
	void *pmodel = GET_MODEL_PTR( ENT( pev ) );
	return ::LookupSequence( pmodel, label );
}

void CBaseAnimating::ResetSequenceInfo()
{
	void *pmodel = GET_MODEL_PTR( ENT( pev ) );

	GetSequenceInfo( pmodel, pev, &m_flFrameRate, &m_flGroundSpeed );
	m_fSequenceLoops = ( ( GetSequenceFlags() & STUDIO_LOOPING ) != 0 );
	pev->animtime = gpGlobals->time;
	pev->framerate = 1.0;
	m_fSequenceFinished = FALSE;
	m_flLastEventCheck = gpGlobals->time;
}

BOOL CBaseAnimating::GetSequenceFlags()
{
	void *pmodel = GET_MODEL_PTR( ENT( pev ) );
	return ::GetSequenceFlags( pmodel, pev );
}

float CBaseAnimating::SetBoneController( int iController, float flValue )
{
	void *pmodel = GET_MODEL_PTR( ENT( pev ) );
	return SetController( pmodel, pev, iController, flValue );
}

void CBaseAnimating::InitBoneControllers( void )
{
	void *pmodel = GET_MODEL_PTR( ENT( pev ) );

	SetController( pmodel, pev, 0, 0.0 );
	SetController( pmodel, pev, 1, 0.0 );
	SetController( pmodel, pev, 2, 0.0 );
	SetController( pmodel, pev, 3, 0.0 );
}

float CBaseAnimating::SetBlending( int iBlender, float flValue )
{
	void *pmodel = GET_MODEL_PTR( ENT( pev ) );
	return ::SetBlending( pmodel, pev, iBlender, flValue );
}

void CBaseAnimating::GetBonePosition( int iBone, Vector &origin, Vector &angles )
{
	GET_BONE_POSITION( ENT( pev ), iBone, origin, angles );
}

void CBaseAnimating::GetAttachment( int iAttachment, Vector &origin, Vector &angles )
{
	GET_ATTACHMENT( ENT( pev ), iAttachment, origin, angles );
}

void CBaseAnimating::SetBodygroup( int iGroup, int iValue )
{
	::SetBodygroup( GET_MODEL_PTR( ENT( pev ) ), pev, iGroup, iValue );
}

int CBaseAnimating::GetBodygroup( int iGroup )
{
	return ::GetBodygroup( GET_MODEL_PTR( ENT( pev ) ), pev, iGroup );
}