//========= Copyright ?1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#ifndef OBSTACLE_PUSHAWAY_H
#define OBSTACLE_PUSHAWAY_H
#ifdef _WIN32
#pragma once
#endif

#include "ispatialpartition.h"
#include "doors.h"

inline bool IsPushAwayEntity( CBaseEntity *pEntity )
{
	if ( !pEntity )
		return false;

	if ( FClassnameIs(pEntity, "func_door") || FClassnameIs(pEntity, "func_door_rotating") )
		return true;

	return false;
}

inline bool IsBreakableEntity( CBaseEntity *pEntity )
{
	if ( !pEntity )
		return false;

	if ( FClassnameIs(pEntity, "func_breakable") )
		return true;

	return false;
}

//--------------------------------------------------------------------------------------------------------------
class CPushAwayEnumerator : public IPartitionEnumerator
{
public:
	CPushAwayEnumerator(CBaseEntity **ents, int nMaxEnts)
	{
		m_nAlreadyHit = 0;
		m_AlreadyHit = ents;
		m_nMaxHits = nMaxEnts;
	}
	
	virtual IterationRetval_t EnumElement( CBaseEntity *pEntity )
	{
		if ( IsPushAwayEntity( pEntity ) && m_nAlreadyHit < m_nMaxHits )
		{
			m_AlreadyHit[m_nAlreadyHit] = pEntity;
			m_nAlreadyHit++;
		}

		return ITERATION_CONTINUE;
	}

public:

	CBaseEntity **m_AlreadyHit;
	int m_nAlreadyHit;
	int m_nMaxHits;
};

//--------------------------------------------------------------------------------------------------------------
/**
 * This class will collect breakable objects in a volume.  Physics props that can be damaged, func_breakable*, etc
 * are all collected by this class.
 */
class CBotBreakableEnumerator : public CPushAwayEnumerator
{
public:
	CBotBreakableEnumerator(CBaseEntity **ents, int nMaxEnts) : CPushAwayEnumerator(ents, nMaxEnts)
	{
	}

	virtual IterationRetval_t EnumElement( CBaseEntity *pEntity )
	{
		if ( IsBreakableEntity( pEntity ) && m_nAlreadyHit < m_nMaxHits )
		{
			m_AlreadyHit[m_nAlreadyHit] = pEntity;
			m_nAlreadyHit++;
		}

		return ITERATION_CONTINUE;
	}
};


//--------------------------------------------------------------------------------------------------------------
/**
 * This class will collect door objects in a volume.
 */
class CBotDoorEnumerator : public CPushAwayEnumerator
{
public:
	CBotDoorEnumerator(CBaseEntity **ents, int nMaxEnts) : CPushAwayEnumerator(ents, nMaxEnts)
	{
	}

	virtual IterationRetval_t EnumElement( CBaseEntity *pEntity )
	{
		if ( FClassnameIs( pEntity, "func_door" ) )
		{
			CBaseDoor *door = dynamic_cast<CBaseDoor *>(pEntity);
			if ( !door )
			{
				return ITERATION_CONTINUE;
			}

			if ( door->m_toggle_state == TS_GOING_UP || door->m_toggle_state == TS_GOING_DOWN )
			{
				return ITERATION_CONTINUE;
			}
		}
		else
		{
			return ITERATION_CONTINUE;
		}

		if ( m_nAlreadyHit < m_nMaxHits )
		{
			m_AlreadyHit[m_nAlreadyHit] = pEntity;
			m_nAlreadyHit++;
		}

		return ITERATION_CONTINUE;
	}
};


#endif
