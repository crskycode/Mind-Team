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

#ifndef DOORS_H
#define DOORS_H

#define SF_DOOR_ROTATE_Y			0x00000000
#define	SF_DOOR_START_OPEN			0x00000001
#define SF_DOOR_ROTATE_BACKWARDS	0x00000002
#define SF_DOOR_PASSABLE			0x00000008
#define SF_DOOR_ONEWAY				0x00000010
#define	SF_DOOR_NO_AUTO_RETURN		0x00000020
#define SF_DOOR_ROTATE_Z			0x00000040
#define SF_DOOR_ROTATE_X			0x00000080
#define SF_DOOR_USE_ONLY			0x00000100
#define SF_DOOR_NOMONSTERS			0x00000200
#define SF_DOOR_SILENT				0x80000000


class CBaseDoor : public CBaseToggle
{
public:
	virtual void Spawn( void );
	virtual void Precache( void );
	virtual void KeyValue( KeyValueData *pkvd );

	virtual void Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	virtual void Blocked( CBaseEntity *pOther );
	virtual int ObjectCaps( void )
	{
		if ( pev->spawnflags & SF_ITEM_USE_ONLY )
			return ( CBaseToggle::ObjectCaps() & ~FCAP_ACROSS_TRANSITION ) | FCAP_IMPULSE_USE;
		else
			return ( CBaseToggle::ObjectCaps() & ~FCAP_ACROSS_TRANSITION );
	};
	virtual void SetToggleState( int state );

	void DoorTouch( CBaseEntity *pOther );

	int DoorActivate( void );
	void DoorGoUp( void );
	void DoorGoDown( void );
	void DoorHitTop( void );
	void DoorHitBottom( void );

	BYTE m_bHealthValue;

	BYTE m_bMoveSnd;
	BYTE m_bStopSnd;

	locksound_t m_ls;

	BYTE m_bLockedSound;
	BYTE m_bLockedSentence;
	BYTE m_bUnlockedSound;
	BYTE m_bUnlockedSentence;
};

void PlayLockSounds(entvars_t *pev, locksound_t *pls, int flocked, int fbutton);

#endif