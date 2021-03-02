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
#include "player.h"

typedef struct dynpitchvol
{
	int preset;
	int pitchrun;
	int pitchstart;
	int spinup;
	int spindown;
	int volrun;
	int volstart;
	int fadein;
	int fadeout;
	int lfotype;
	int lforate;
	int lfomodpitch;
	int lfomodvol;
	int cspinup;
	int cspincount;
	int pitch;
	int spinupsav;
	int spindownsav;
	int pitchfrac;
	int vol;
	int fadeinsav;
	int fadeoutsav;
	int volfrac;
	int lfofrac;
	int lfomult;

} dynpitchvol_t;

#define CDPVPRESETMAX 27

dynpitchvol_t rgdpvpreset[CDPVPRESETMAX] =
{
	{ 1,	255,	75,		95,		95,		10,		1,		50,		95, 	0,		0,		0,		0,		0,		0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 2,	255,	85,		70,		88,		10,		1,		20,		88,		0,		0,		0,		0,		0,		0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 3,	255,	100,	50,		75,		10,		1,		10,		75,		0,		0,		0,		0,		0,		0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 4,	100,	100,	0,		0,		10,		1,		90,		90,		0,		0,		0,		0,		0,		0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 5,	100,	100,	0,		0,		10,		1,		80,		80,		0,		0,		0,		0,		0,		0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 6,	100,	100,	0,		0,		10,		1,		50,		70,		0,		0,		0,		0,		0,		0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 7,	100,	100,	0,		0,		5,		1,		40,		50,		1,		50,		0,		10,		0,		0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 8,	100,	100,	0,		0,		5,		1,		40,		50,		1,		150,	0,		10,		0,		0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 9,	100,	100,	0,		0,		5,		1,		40,		50,		1,		750,	0,		10,		0,		0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 10,	128,	100,	50,		75,		10,		1,		30,		40,		2,		8,		20,		0,		0,		0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 11,	128,	100,	50,		75,		10,		1,		30,		40,		2,		25,		20,		0,		0,		0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 12,	128,	100,	50,		75,		10,		1,		30,		40,		2,		70,		20,		0,		0,		0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 13,	50,		50,		0,		0,		10,		1,		20,		50,		0,		0,		0,		0,		0,		0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 14,	70,		70,		0,		0,		10,		1,		20,		50,		0,		0,		0,		0,		0,		0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 15,	90,		90,		0,		0,		10,		1,		20,		50,		0,		0,		0,		0,		0,		0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 16,	120,	120,	0,		0,		10,		1,		20,		50,		0,		0,		0,		0,		0,		0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 17,	180,	180,	0,		0,		10,		1,		20,		50,		0,		0,		0,		0,		0,		0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 18,	255,	255,	0,		0,		10,		1,		20,		50,		0,		0,		0,		0,		0,		0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 19,	200,	75,		90,		90,		10,		1,		50,		90,		2,		100,	20,		0,		0,		0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 20,	255,	75,		97,		90,		10,		1,		50,		90,		1,		40,		50,		0,		0,		0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 21,	100,	100,	0,		0,		10,		1,		30,		50,		3,		15,		20,		0,		0,		0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 22,	160,	160,	0,		0,		10,		1,		50,		50,		3,		500,	25,		0,		0,		0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 23,	255,	75,		88,		0,		10,		1,		40,		0,		0,		0,		0,		0,		5,		0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 24,	200,	20,		95,		70,		10,		1,		70,		70,		3,		20,		50,		0,		0,		0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 25,	180,	100,	50,		60,		10,		1,		40,		60,		2,		90,		100,	100,	0,		0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 26,	60,		60,		0,		0,		10,		1,		40,		70,		3,		80,		20,		50,		0,		0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 27,	128,	90,		10,		10,		10,		1,		20,		40,		1,		5,		10,		20,		0,		0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
};

class CAmbientGeneric : public CBaseEntity
{
public:
	void KeyValue( KeyValueData *pkvd );
	void Spawn( void );
	void Precache( void );
	void ToggleUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	void RampThink( void );
	void InitModulationParms( void );

	virtual int ObjectCaps( void ) { return ( CBaseEntity::ObjectCaps() & ~FCAP_ACROSS_TRANSITION ); }

	float m_flAttenuation;
	dynpitchvol_t m_dpv;

	BOOL m_fActive;
	BOOL m_fLooping;
};

LINK_ENTITY_TO_CLASS( ambient_generic, CAmbientGeneric );

void CAmbientGeneric::Spawn( void )
{
	if ( FBitSet( pev->spawnflags, AMBIENT_SOUND_EVERYWHERE ) )
		m_flAttenuation = ATTN_NONE;
	else if ( FBitSet( pev->spawnflags, AMBIENT_SOUND_SMALLRADIUS ) )
		m_flAttenuation = ATTN_IDLE;
	else if ( FBitSet( pev->spawnflags, AMBIENT_SOUND_MEDIUMRADIUS ) )
		m_flAttenuation = ATTN_STATIC;
	else if ( FBitSet( pev->spawnflags, AMBIENT_SOUND_LARGERADIUS ) )
		m_flAttenuation = ATTN_NORM;
	else
		m_flAttenuation = ATTN_STATIC;

	char *szSoundFile = ( char * ) STRING( pev->message );

	if ( FStringNull( pev->message ) || strlen( szSoundFile ) < 1 )
	{
		ALERT( at_error, "EMPTY AMBIENT AT: %f, %f, %f\n", pev->origin.x, pev->origin.y, pev->origin.z );

		pev->nextthink = gpGlobals->time + 0.1;
		SetThink( &CBaseEntity::SUB_Remove );

		return;
	}

	pev->solid = SOLID_NOT;
	pev->movetype = MOVETYPE_NONE;

	SetThink( &CAmbientGeneric::RampThink );
	pev->nextthink = 0;

	SetUse( &CAmbientGeneric::ToggleUse );

	m_fActive = FALSE;

	if ( FBitSet( pev->spawnflags, AMBIENT_SOUND_NOT_LOOPING ) )
		m_fLooping = FALSE;
	else
		m_fLooping = TRUE;

	Precache();
}

void CAmbientGeneric::Precache( void )
{
	char *szSoundFile = ( char * ) STRING( pev->message );

	if ( !FStringNull( pev->message ) && strlen( szSoundFile ) > 1 )
	{
		if ( *szSoundFile != '!' )
			PRECACHE_SOUND( szSoundFile );
	}

	InitModulationParms();

	if ( !FBitSet( pev->spawnflags, AMBIENT_SOUND_START_SILENT ) )
	{
		if ( m_fLooping )
			m_fActive = TRUE;
	}

	if ( m_fActive )
	{
		UTIL_EmitAmbientSound ( ENT( pev ), pev->origin, szSoundFile,
		                        ( m_dpv.vol * 0.01 ), m_flAttenuation, SND_SPAWNING, m_dpv.pitch );

		pev->nextthink = gpGlobals->time + 0.1;
	}
}

void CAmbientGeneric::RampThink( void )
{
	char *szSoundFile = ( char * )STRING( pev->message );
	int pitch = m_dpv.pitch;
	int vol = m_dpv.vol;
	int flags = 0;
	int fChanged = 0;
	int prev;

	if ( !m_dpv.spinup && !m_dpv.spindown && !m_dpv.fadein && !m_dpv.fadeout && !m_dpv.lfotype )
		return;

	if ( m_dpv.spinup || m_dpv.spindown )
	{
		prev = m_dpv.pitchfrac >> 8;

		if ( m_dpv.spinup > 0 )
			m_dpv.pitchfrac += m_dpv.spinup;
		else if ( m_dpv.spindown > 0 )
			m_dpv.pitchfrac -= m_dpv.spindown;

		pitch = m_dpv.pitchfrac >> 8;

		if ( pitch > m_dpv.pitchrun )
		{
			pitch = m_dpv.pitchrun;
			m_dpv.spinup = 0;
		}

		if ( pitch < m_dpv.pitchstart )
		{
			pitch = m_dpv.pitchstart;
			m_dpv.spindown = 0;
			UTIL_EmitAmbientSound( ENT( pev ), pev->origin, szSoundFile,
			                       0, 0, SND_STOP, 0 );

			return;
		}

		if ( pitch > 255 ) pitch = 255;

		if ( pitch < 1 ) pitch = 1;

		m_dpv.pitch = pitch;

		fChanged |= ( prev != pitch );
		flags |= SND_CHANGE_PITCH;
	}

	if ( m_dpv.fadein || m_dpv.fadeout )
	{
		prev = m_dpv.volfrac >> 8;

		if ( m_dpv.fadein > 0 )
			m_dpv.volfrac += m_dpv.fadein;
		else if ( m_dpv.fadeout > 0 )
			m_dpv.volfrac -= m_dpv.fadeout;

		vol = m_dpv.volfrac >> 8;

		if ( vol > m_dpv.volrun )
		{
			vol = m_dpv.volrun;
			m_dpv.fadein = 0;
		}

		if ( vol < m_dpv.volstart )
		{
			vol = m_dpv.volstart;
			m_dpv.fadeout = 0;
			UTIL_EmitAmbientSound( ENT( pev ), pev->origin, szSoundFile,
			                       0, 0, SND_STOP, 0 );

			return;
		}

		if ( vol > 100 ) vol = 100;

		if ( vol < 1 ) vol = 1;

		m_dpv.vol = vol;

		fChanged |= ( prev != vol );
		flags |= SND_CHANGE_VOL;
	}

	if ( m_dpv.lfotype )
	{
		int pos;

		if ( m_dpv.lfofrac > 0x6fffffff )
			m_dpv.lfofrac = 0;

		m_dpv.lfofrac += m_dpv.lforate;
		pos = m_dpv.lfofrac >> 8;

		if ( m_dpv.lfofrac < 0 )
		{
			m_dpv.lfofrac = 0;
			m_dpv.lforate = abs( m_dpv.lforate );
			pos = 0;
		}
		else if ( pos > 255 )
		{
			pos = 255;
			m_dpv.lfofrac = ( 255 << 8 );
			m_dpv.lforate = -abs( m_dpv.lforate );
		}

		switch ( m_dpv.lfotype )
		{
			case LFO_SQUARE:
				if ( pos < 128 )
					m_dpv.lfomult = 255;
				else
					m_dpv.lfomult = 0;

				break;

			case LFO_RANDOM:
				if ( pos == 255 )
					m_dpv.lfomult = RANDOM_LONG( 0, 255 );

				break;

			case LFO_TRIANGLE:
			default:
				m_dpv.lfomult = pos;
				break;
		}

		if ( m_dpv.lfomodpitch )
		{
			prev = pitch;

			pitch += ( ( m_dpv.lfomult - 128 ) * m_dpv.lfomodpitch ) / 100;

			if ( pitch > 255 ) pitch = 255;

			if ( pitch < 1 ) pitch = 1;

			fChanged |= ( prev != pitch );
			flags |= SND_CHANGE_PITCH;
		}

		if ( m_dpv.lfomodvol )
		{
			prev = vol;

			vol += ( ( m_dpv.lfomult - 128 ) * m_dpv.lfomodvol ) / 100;

			if ( vol > 100 ) vol = 100;

			if ( vol < 0 ) vol = 0;

			fChanged |= ( prev != vol );
			flags |= SND_CHANGE_VOL;
		}
	}

	if ( flags && fChanged )
	{
		if ( pitch == PITCH_NORM )
			pitch = PITCH_NORM + 1;

		UTIL_EmitAmbientSound( ENT( pev ), pev->origin, szSoundFile,
		                       ( vol * 0.01 ), m_flAttenuation, flags, pitch );
	}

	pev->nextthink = gpGlobals->time + 0.2;
	return;
}

void CAmbientGeneric::InitModulationParms( void )
{
	int pitchinc;

	m_dpv.volrun = pev->health * 10;

	if ( m_dpv.volrun > 100 ) m_dpv.volrun = 100;

	if ( m_dpv.volrun < 0 ) m_dpv.volrun = 0;

	if ( m_dpv.preset != 0 && m_dpv.preset <= CDPVPRESETMAX )
	{
		m_dpv = rgdpvpreset[m_dpv.preset - 1];

		if ( m_dpv.spindown > 0 )
			m_dpv.spindown = ( 101 - m_dpv.spindown ) * 64;

		if ( m_dpv.spinup > 0 )
			m_dpv.spinup = ( 101 - m_dpv.spinup ) * 64;

		m_dpv.volstart *= 10;
		m_dpv.volrun *= 10;

		if ( m_dpv.fadein > 0 )
			m_dpv.fadein = ( 101 - m_dpv.fadein ) * 64;

		if ( m_dpv.fadeout > 0 )
			m_dpv.fadeout = ( 101 - m_dpv.fadeout ) * 64;

		m_dpv.lforate *= 256;

		m_dpv.fadeinsav = m_dpv.fadein;
		m_dpv.fadeoutsav = m_dpv.fadeout;
		m_dpv.spinupsav = m_dpv.spinup;
		m_dpv.spindownsav = m_dpv.spindown;
	}

	m_dpv.fadein = m_dpv.fadeinsav;
	m_dpv.fadeout = 0;

	if ( m_dpv.fadein )
		m_dpv.vol = m_dpv.volstart;
	else
		m_dpv.vol = m_dpv.volrun;

	m_dpv.spinup = m_dpv.spinupsav;
	m_dpv.spindown = 0;

	if ( m_dpv.spinup )
		m_dpv.pitch = m_dpv.pitchstart;
	else
		m_dpv.pitch = m_dpv.pitchrun;

	if ( m_dpv.pitch == 0 )
		m_dpv.pitch = PITCH_NORM;

	m_dpv.pitchfrac = m_dpv.pitch << 8;
	m_dpv.volfrac = m_dpv.vol << 8;

	m_dpv.lfofrac = 0;
	m_dpv.lforate = abs( m_dpv.lforate );

	m_dpv.cspincount = 1;

	if ( m_dpv.cspinup )
	{
		pitchinc = ( 255 - m_dpv.pitchstart ) / m_dpv.cspinup;

		m_dpv.pitchrun = m_dpv.pitchstart + pitchinc;

		if ( m_dpv.pitchrun > 255 ) m_dpv.pitchrun = 255;
	}

	if ( ( m_dpv.spinupsav || m_dpv.spindownsav || ( m_dpv.lfotype && m_dpv.lfomodpitch ) )
	     && ( m_dpv.pitch == PITCH_NORM ) )
		m_dpv.pitch = PITCH_NORM + 1;
}

void CAmbientGeneric::ToggleUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	char *szSoundFile = ( char * )STRING( pev->message );
	float fraction;

	if ( useType != USE_TOGGLE )
	{
		if ( ( m_fActive && useType == USE_ON ) || ( !m_fActive && useType == USE_OFF ) )
			return;
	}

	if ( useType == USE_SET && m_fActive )
	{
		fraction = value;

		if ( fraction > 1.0 )
			fraction = 1.0;

		if ( fraction < 0.0 )
			fraction = 0.01;

		m_dpv.pitch = fraction * 255;

		UTIL_EmitAmbientSound( ENT( pev ), pev->origin, szSoundFile,
		                       0, 0, SND_CHANGE_PITCH, m_dpv.pitch );

		return;
	}

	if ( m_fActive )
	{
		if ( m_dpv.cspinup )
		{
			if ( m_dpv.cspincount <= m_dpv.cspinup )
			{
				int pitchinc;

				m_dpv.cspincount++;

				pitchinc = ( 255 - m_dpv.pitchstart ) / m_dpv.cspinup;

				m_dpv.spinup = m_dpv.spinupsav;
				m_dpv.spindown = 0;

				m_dpv.pitchrun = m_dpv.pitchstart + pitchinc * m_dpv.cspincount;

				if ( m_dpv.pitchrun > 255 ) m_dpv.pitchrun = 255;

				pev->nextthink = gpGlobals->time + 0.1;
			}

		}
		else
		{
			m_fActive = FALSE;

			pev->spawnflags |= AMBIENT_SOUND_START_SILENT;

			if ( m_dpv.spindownsav || m_dpv.fadeoutsav )
			{
				m_dpv.spindown = m_dpv.spindownsav;
				m_dpv.spinup = 0;

				m_dpv.fadeout = m_dpv.fadeoutsav;
				m_dpv.fadein = 0;
				pev->nextthink = gpGlobals->time + 0.1;
			}
			else
				UTIL_EmitAmbientSound( ENT( pev ), pev->origin, szSoundFile,
				                       0, 0, SND_STOP, 0 );
		}
	}
	else
	{
		if ( m_fLooping )
			m_fActive = TRUE;
		else
			UTIL_EmitAmbientSound( ENT( pev ), pev->origin, szSoundFile,
			                       0, 0, SND_STOP, 0 );

		InitModulationParms();

		UTIL_EmitAmbientSound( ENT( pev ), pev->origin, szSoundFile,
		                       ( m_dpv.vol * 0.01 ), m_flAttenuation, 0, m_dpv.pitch );

		pev->nextthink = gpGlobals->time + 0.1;
	}
}

void CAmbientGeneric::KeyValue( KeyValueData *pkvd )
{
	if ( FStrEq( pkvd->szKeyName, "preset" ) )
	{
		m_dpv.preset = atoi( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else if ( FStrEq( pkvd->szKeyName, "pitch" ) )
	{
		m_dpv.pitchrun = atoi( pkvd->szValue );
		pkvd->fHandled = TRUE;

		if ( m_dpv.pitchrun > 255 ) m_dpv.pitchrun = 255;

		if ( m_dpv.pitchrun < 0 ) m_dpv.pitchrun = 0;
	}
	else if ( FStrEq( pkvd->szKeyName, "pitchstart" ) )
	{
		m_dpv.pitchstart = atoi( pkvd->szValue );
		pkvd->fHandled = TRUE;

		if ( m_dpv.pitchstart > 255 ) m_dpv.pitchstart = 255;

		if ( m_dpv.pitchstart < 0 ) m_dpv.pitchstart = 0;
	}
	else if ( FStrEq( pkvd->szKeyName, "spinup" ) )
	{
		m_dpv.spinup = atoi( pkvd->szValue );

		if ( m_dpv.spinup > 100 ) m_dpv.spinup = 100;

		if ( m_dpv.spinup < 0 ) m_dpv.spinup = 0;

		if ( m_dpv.spinup > 0 )
			m_dpv.spinup = ( 101 - m_dpv.spinup ) * 64;

		m_dpv.spinupsav = m_dpv.spinup;
		pkvd->fHandled = TRUE;
	}
	else if ( FStrEq( pkvd->szKeyName, "spindown" ) )
	{
		m_dpv.spindown = atoi( pkvd->szValue );

		if ( m_dpv.spindown > 100 ) m_dpv.spindown = 100;

		if ( m_dpv.spindown < 0 ) m_dpv.spindown = 0;

		if ( m_dpv.spindown > 0 )
			m_dpv.spindown = ( 101 - m_dpv.spindown ) * 64;

		m_dpv.spindownsav = m_dpv.spindown;
		pkvd->fHandled = TRUE;
	}
	else if ( FStrEq( pkvd->szKeyName, "volstart" ) )
	{
		m_dpv.volstart = atoi( pkvd->szValue );

		if ( m_dpv.volstart > 10 ) m_dpv.volstart = 10;

		if ( m_dpv.volstart < 0 ) m_dpv.volstart = 0;

		m_dpv.volstart *= 10;

		pkvd->fHandled = TRUE;
	}
	else if ( FStrEq( pkvd->szKeyName, "fadein" ) )
	{
		m_dpv.fadein = atoi( pkvd->szValue );

		if ( m_dpv.fadein > 100 ) m_dpv.fadein = 100;

		if ( m_dpv.fadein < 0 ) m_dpv.fadein = 0;

		if ( m_dpv.fadein > 0 )
			m_dpv.fadein = ( 101 - m_dpv.fadein ) * 64;

		m_dpv.fadeinsav = m_dpv.fadein;
		pkvd->fHandled = TRUE;
	}
	else if ( FStrEq( pkvd->szKeyName, "fadeout" ) )
	{
		m_dpv.fadeout = atoi( pkvd->szValue );

		if ( m_dpv.fadeout > 100 ) m_dpv.fadeout = 100;

		if ( m_dpv.fadeout < 0 ) m_dpv.fadeout = 0;

		if ( m_dpv.fadeout > 0 )
			m_dpv.fadeout = ( 101 - m_dpv.fadeout ) * 64;

		m_dpv.fadeoutsav = m_dpv.fadeout;
		pkvd->fHandled = TRUE;
	}
	else if ( FStrEq( pkvd->szKeyName, "lfotype" ) )
	{
		m_dpv.lfotype = atoi( pkvd->szValue );

		if ( m_dpv.lfotype > 4 ) m_dpv.lfotype = LFO_TRIANGLE;

		pkvd->fHandled = TRUE;
	}
	else if ( FStrEq( pkvd->szKeyName, "lforate" ) )
	{
		m_dpv.lforate = atoi( pkvd->szValue );

		if ( m_dpv.lforate > 1000 ) m_dpv.lforate = 1000;

		if ( m_dpv.lforate < 0 ) m_dpv.lforate = 0;

		m_dpv.lforate *= 256;

		pkvd->fHandled = TRUE;
	}
	else if ( FStrEq( pkvd->szKeyName, "lfomodpitch" ) )
	{
		m_dpv.lfomodpitch = atoi( pkvd->szValue );

		if ( m_dpv.lfomodpitch > 100 ) m_dpv.lfomodpitch = 100;

		if ( m_dpv.lfomodpitch < 0 ) m_dpv.lfomodpitch = 0;

		pkvd->fHandled = TRUE;
	}

	else if ( FStrEq( pkvd->szKeyName, "lfomodvol" ) )
	{
		m_dpv.lfomodvol = atoi( pkvd->szValue );

		if ( m_dpv.lfomodvol > 100 ) m_dpv.lfomodvol = 100;

		if ( m_dpv.lfomodvol < 0 ) m_dpv.lfomodvol = 0;

		pkvd->fHandled = TRUE;
	}
	else if ( FStrEq( pkvd->szKeyName, "cspinup" ) )
	{
		m_dpv.cspinup = atoi( pkvd->szValue );

		if ( m_dpv.cspinup > 100 ) m_dpv.cspinup = 100;

		if ( m_dpv.cspinup < 0 ) m_dpv.cspinup = 0;

		pkvd->fHandled = TRUE;
	}
	else
		CBaseEntity::KeyValue( pkvd );
}

class CEnvSound : public CPointEntity
{
public:
	void KeyValue( KeyValueData *pkvd );
	void Spawn( void );
	void Think( void );

	float m_flRadius;
	float m_flRoomtype;
};

LINK_ENTITY_TO_CLASS( env_sound, CEnvSound );

void CEnvSound::KeyValue( KeyValueData *pkvd )
{
	if ( FStrEq( pkvd->szKeyName, "radius" ) )
	{
		m_flRadius = atof( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}

	if ( FStrEq( pkvd->szKeyName, "roomtype" ) )
	{
		m_flRoomtype = atof( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
}

BOOL FEnvSoundInRange( entvars_t *pev, entvars_t *pevTarget, float *pflRange )
{
	CEnvSound *pSound = GetClassPtr( ( CEnvSound * )pev );

	Vector vecSpot1 = pev->origin + pev->view_ofs;
	Vector vecSpot2 = pevTarget->origin + pevTarget->view_ofs;

	Vector vecRange;
	float flRange;
	TraceResult tr;

	UTIL_TraceLine( vecSpot1, vecSpot2, ignore_monsters, ENT( pev ), &tr );

	if ( ( tr.fInOpen && tr.fInWater ) || tr.flFraction != 1 )
		return FALSE;

	vecRange = tr.vecEndPos - vecSpot1;
	flRange = vecRange.Length();

	if ( pSound->m_flRadius < flRange )
		return FALSE;

	if ( pflRange )
		*pflRange = flRange;

	return TRUE;
}

void CEnvSound::Think( void )
{
	edict_t *pentPlayer = FIND_CLIENT_IN_PVS( edict() );
	CBasePlayer *pPlayer = NULL;

	if ( FNullEnt( pentPlayer ) )
		goto env_sound_Think_slow;

	pPlayer = GetClassPtr( ( CBasePlayer * )VARS( pentPlayer ) );
	float flRange;

	if ( !FNullEnt( pPlayer->m_pentSndLast ) && ( pPlayer->m_pentSndLast == ENT( pev ) ) )
	{
		if ( pPlayer->m_flSndRoomtype != 0 && pPlayer->m_flSndRange != 0 )
		{
			if ( FEnvSoundInRange( pev, VARS( pentPlayer ), &flRange ) )
			{
				pPlayer->m_flSndRange = flRange;
				goto env_sound_Think_fast;
			}
			else
			{
				pPlayer->m_flSndRange = 0;
				pPlayer->m_flSndRoomtype = 0;
				goto env_sound_Think_slow;
			}
		}
		else
			goto env_sound_Think_slow;
	}

	if ( FEnvSoundInRange( pev, VARS( pentPlayer ), &flRange ) )
	{
		if ( flRange < pPlayer->m_flSndRange || pPlayer->m_flSndRange == 0 )
		{
			pPlayer->m_pentSndLast = ENT( pev );
			pPlayer->m_flSndRoomtype = m_flRoomtype;
			pPlayer->m_flSndRange = flRange;

			MESSAGE_BEGIN( MSG_ONE, SVC_ROOMTYPE, NULL, pentPlayer );
			WRITE_SHORT( ( short )m_flRoomtype );
			MESSAGE_END();
		}
	}

env_sound_Think_fast:
	pev->nextthink = gpGlobals->time + 0.25;
	return;

env_sound_Think_slow:
	pev->nextthink = gpGlobals->time + 0.75;
	return;
}

void CEnvSound::Spawn( void )
{
	pev->nextthink = gpGlobals->time + RANDOM_FLOAT( 0.0, 0.5 );
}