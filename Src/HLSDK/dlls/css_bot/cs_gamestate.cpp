//========= Copyright ?1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: Encapsulation of the current scenario/game state. Allows each bot imperfect knowledge.
//
// $NoKeywords: $
//=============================================================================//

// Author: Michael S. Booth (mike@turtlerockstudios.com), 2003

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "source.h"
//!#include "KeyValues.h"

#include "cs_bot.h"
#include "cs_bot_manager.h"
#include "cs_gamestate.h"
//!#include "cs_simple_hostage.h"

// memdbgon must be the last include file in a .cpp file!!!
//!#include "tier0/memdbgon.h"

//--------------------------------------------------------------------------------------------------------------
CSGameState::CSGameState( CCSBot *owner )
{
	m_owner = owner;
	m_isRoundOver = false;

	m_bombState = MOVING;
	m_lastSawBomber.Invalidate();
	m_lastSawLooseBomb.Invalidate();
	m_isPlantedBombPosKnown = false;
	m_plantedBombsite = UNKNOWN;

	m_bombsiteCount = 0;
	m_bombsiteSearchIndex = 0;

//	for( int i=0; i<MAX_HOSTAGES; ++i )
//	{
//		m_hostage[i].hostage = NULL;
//		m_hostage[i].isValid = false;
//		m_hostage[i].isAlive = false;
//		m_hostage[i].isFree = true;
//		m_hostage[i].knownPos = Vector( 0, 0, 0 );
//	}
}

//--------------------------------------------------------------------------------------------------------------
/**
 * Reset at round start
 */
void CSGameState::Reset( void )
{
	m_isRoundOver = false;

	// bomb -----------------------------------------------------------------------
	m_bombState = MOVING;
	m_lastSawBomber.Invalidate();
	m_lastSawLooseBomb.Invalidate();
	m_isPlantedBombPosKnown = false;
	m_plantedBombsite = UNKNOWN;

	m_bombsiteCount = TheCSBots()->GetZoneCount();

	int i;
//	for( i=0; i<m_bombsiteCount; ++i )
//	{
//		m_isBombsiteClear[i] = false;
//		m_bombsiteSearchOrder[i] = i;
//	}

	// shuffle the bombsite search order
	// allows T's to plant at random site, and TEAM_CT's to search in a random order
	// NOTE: VS6 std::random_shuffle() doesn't work well with an array of two elements (most maps)
//	for( i=0; i < m_bombsiteCount; ++i )
//	{
//		int swap = m_bombsiteSearchOrder[i];
//		int rnd = RandomInt( i, m_bombsiteCount-1 );
//		m_bombsiteSearchOrder[i] = m_bombsiteSearchOrder[ rnd ];
//		m_bombsiteSearchOrder[ rnd ] = swap;
//	}

//	m_bombsiteSearchIndex = 0;

	// hostage ---------------------------------------------------------------------
//	InitializeHostageInfo();
}

//--------------------------------------------------------------------------------------------------------------
/**
 * Returns position of bomb in its various states (moving, loose, planted),
 * or NULL if we don't know where the bomb is
 */
const Vector *CSGameState::GetBombPosition( void ) const
{
	//switch( m_bombState )
	//{
	//	case MOVING:
	//	{
	//		if (!m_lastSawBomber.HasStarted())
	//			return NULL;

	//		return &m_bomberPos;
	//	}

	//	case LOOSE:
	//	{
	//		if (IsLooseBombLocationKnown())
	//			return &m_looseBombPos;

	//		return NULL;
	//	}

	//	case PLANTED:
	//	{
	//		if (IsPlantedBombLocationKnown())
	//			return &m_plantedBombPos;

	//		return NULL;
	//	}
	//}

	return NULL;
}


//--------------------------------------------------------------------------------------------------------------
/**
 * Return true if we are currently in the bombsite where the bomb is planted
 */
bool CSGameState::IsAtPlantedBombsite( void ) const
{
	if (m_bombState != PLANTED)
		return false;

	Vector myOrigin = GetCentroid( m_owner );
	const CCSBotManager::Zone *zone = TheCSBots()->GetClosestZone( myOrigin );

	if (zone)
	{
		return (m_plantedBombsite == zone->m_index);
	}

	return false;
}

//--------------------------------------------------------------------------------------------------------------
bool CSGameState::IsPlantedBombLocationKnown( void ) const
{
	if (m_bombState != PLANTED)
		return false;

	return m_isPlantedBombPosKnown;
}

//--------------------------------------------------------------------------------------------------------------
bool CSGameState::IsLooseBombLocationKnown( void ) const
{
	/*if (m_bombState != LOOSE)
		return false;

	return (m_lastSawLooseBomb.HasStarted()) ? true : false;*/
	return false;
}

//--------------------------------------------------------------------------------------------------------------
bool CSGameState::IsBombsiteClear( int zoneIndex ) const
{
	/*if (zoneIndex >= 0 && zoneIndex < m_bombsiteCount)
		return m_isBombsiteClear[ zoneIndex ];*/

	return false;
}


//--------------------------------------------------------------------------------------------------------------
/**
 * Return the zone index of the next bombsite to search
 */
int CSGameState::GetNextBombsiteToSearch( void )
{
/*	if (m_bombsiteCount <= 0)
		return 0;

	int i;

	// return next non-cleared bombsite index
	for( i=m_bombsiteSearchIndex; i<m_bombsiteCount; ++i )
	{
		int z = m_bombsiteSearchOrder[i];
		if (!m_isBombsiteClear[z])
		{
			m_bombsiteSearchIndex = i;
			return z;
		}
	}

	// all the bombsites are clear, someone must have been mistaken - start search over
	for( i=0; i<m_bombsiteCount; ++i )
		m_isBombsiteClear[i] = false;
	m_bombsiteSearchIndex = 0;

	return GetNextBombsiteToSearch();*/
	return 0;
}

//--------------------------------------------------------------------------------------------------------------
/**
 * Return the zone index of the planted bombsite, or UNKNOWN
 */
int CSGameState::GetPlantedBombsite( void ) const
{
	/*if (m_bombState != PLANTED)
		return UNKNOWN;

	return m_plantedBombsite;*/
	return 0;
}

//--------------------------------------------------------------------------------------------------------------
/**
 * True if round has been won or lost (but not yet reset)
 */
bool CSGameState::IsRoundOver( void ) const
{
	//return m_isRoundOver;
	return false;
}


//--------------------------------------------------------------------------------------------------------------
/**
 * Someone told us a bombsite is clear
 */
void CSGameState::ClearBombsite( int zoneIndex )
{
	/*if (zoneIndex >= 0 && zoneIndex < m_bombsiteCount)
		m_isBombsiteClear[ zoneIndex ] = true;	*/
}
