//========= Copyright © 1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

// Author: Michael S. Booth (mike@turtlerockstudios.com), 2003

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"

#include "bot.h"
#include "bot_util.h"
#include "bot_profile.h"
#include "nav.h"

static short s_iBeamSprite = 0;

//--------------------------------------------------------------------------------------------------------------
/**
 * Return true if given name is already in use by another player
 */
bool UTIL_IsNameTaken( const char *name, bool ignoreHumans )
{
	for ( int i = 1; i <= gpGlobals->maxClients; ++i )
	{
		CBaseEntity * player = UTIL_PlayerByIndex( i );

		if (player == NULL)
			continue;

		if (FNullEnt( player->pev ))
			continue;

		if (FStrEq( STRING( player->pev->netname ), "" ))
			continue;

		if (player->IsPlayer() && (((CBasePlayer *)player)->IsBot() == TRUE))
		{
			// bots can have prefixes so we need to check the name
			// against the profile name instead.
			CBot *bot = (CBot *)player;
			if (FStrEq(name, bot->GetProfile()->GetName()))
			{
				return true;
			}
		}
		else
		{
			if (!ignoreHumans)
			{
				if (FStrEq( name, STRING( player->pev->netname ) ))
					return true;
			}
		}
	}

	return false;
}


//--------------------------------------------------------------------------------------------------------------
int UTIL_ClientsInGame( void )
{
	int iCount = 0;

	for ( int iIndex = 1; iIndex <= gpGlobals->maxClients; iIndex++ )
	{
		CBaseEntity * pPlayer = UTIL_PlayerByIndex( iIndex );

		if ( pPlayer == NULL )
			continue;

		if ( FNullEnt( pPlayer->pev ) )
			continue;

		if ( FStrEq( STRING( pPlayer->pev->netname ), "" ) )
			continue;

		iCount++;
	}

	return iCount;
}

//--------------------------------------------------------------------------------------------------------------
/**
 * Return number of active players (not spectators) in the game
 */
int UTIL_ActivePlayersInGame( void )
{
	int iCount = 0;

	for (int iIndex = 1; iIndex <= gpGlobals->maxClients; iIndex++ )
	{
		CBaseEntity *entity = UTIL_PlayerByIndex( iIndex );

		if ( entity == NULL )
			continue;

		if ( FNullEnt( entity->pev ) )
			continue;

		if ( FStrEq( STRING( entity->pev->netname ), "" ) )
			continue;

		CBasePlayer *player = static_cast<CBasePlayer *>( entity );

		// ignore spectators
		if (player->m_iTeam != 1 && player->m_iTeam != 2)
			continue;

		if (player->m_iJoiningState != JOINED)
			continue;

		iCount++;
	}

	return iCount;
}



//--------------------------------------------------------------------------------------------------------------
int UTIL_HumansInGame( bool ignoreSpectators )
{
	int iCount = 0;

	for (int iIndex = 1; iIndex <= gpGlobals->maxClients; iIndex++ )
	{
		CBaseEntity *entity = UTIL_PlayerByIndex( iIndex );

		if ( entity == NULL )
			continue;

		if ( FNullEnt( entity->pev ) )
			continue;

		if ( FStrEq( STRING( entity->pev->netname ), "" ) )
			continue;

		CBasePlayer *player = static_cast<CBasePlayer *>( entity );

		if (player->IsBot())
			continue;

		if (ignoreSpectators && player->m_iTeam != 1 && player->m_iTeam != 2)
			continue;

		if (ignoreSpectators && player->m_iJoiningState != JOINED)
			continue;

		iCount++;
	}

	/*
	if ( IS_DEDICATED_SERVER() && !ignoreSpectators )
	{
		// If we're counting humans, including spectators, don't count the dedicated server
		--iCount;
	}
	*/

	return iCount;
}

//--------------------------------------------------------------------------------------------------------------
/**
 * Return the number of non-bots on the given team
 */
int UTIL_HumansOnTeam( int teamID, bool isAlive )
{
	int iCount = 0;

	for (int iIndex = 1; iIndex <= gpGlobals->maxClients; iIndex++ )
	{
		CBaseEntity *entity = UTIL_PlayerByIndex( iIndex );

		if ( entity == NULL )
			continue;

		if ( FNullEnt( entity->pev ) )
			continue;

		if ( FStrEq( STRING( entity->pev->netname ), "" ) )
			continue;

		CBasePlayer *player = static_cast<CBasePlayer *>( entity );

		if (player->IsBot())
			continue;

		if (player->m_iTeam != teamID)
			continue;

		if (isAlive && !player->IsAlive())
			continue;

		iCount++;
	}

	return iCount;
}


//--------------------------------------------------------------------------------------------------------------
int UTIL_BotsInGame( void )
{
	int iCount = 0;

	for (int iIndex = 1; iIndex <= gpGlobals->maxClients; iIndex++ )
	{
		CBasePlayer *pPlayer = static_cast<CBasePlayer *>(UTIL_PlayerByIndex( iIndex ));

		if ( pPlayer == NULL )
			continue;

		if ( FNullEnt( pPlayer->pev ) )
			continue;

		if ( FStrEq( STRING( pPlayer->pev->netname ), "" ) )
			continue;

		if ( !pPlayer->IsBot() )
			continue;

		iCount++;
	}

	return iCount;
}

//--------------------------------------------------------------------------------------------------------------
/**
 * Kick a bot from the given team. If no bot exists on the team, return false.
 */
bool UTIL_KickBotFromTeam( int kickTeam )
{
	int i;

	// try to kick a dead bot first
	for ( i = 1; i <= gpGlobals->maxClients; ++i )
	{
		CBasePlayer *player = static_cast<CBasePlayer *>( UTIL_PlayerByIndex( i ) );

		if (player == NULL)
			continue;

		if (FNullEnt( player->pev ))
			continue;

		const char *name = STRING( player->pev->netname );
		if (FStrEq( name, "" ))
			continue;

		if (!player->IsBot())
			continue;	

		if (!player->IsAlive() && player->m_iTeam == kickTeam)
		{
			// its a bot on the right team - kick it
			SERVER_COMMAND( UTIL_VarArgs( "kick \"%s\"\n", STRING( player->pev->netname ) ) );

			return true;
		}
	}

	// no dead bots, kick any bot on the given team
	for ( i = 1; i <= gpGlobals->maxClients; ++i )
	{
		CBasePlayer *player = static_cast<CBasePlayer *>( UTIL_PlayerByIndex( i ) );

		if (player == NULL)
			continue;

		if (FNullEnt( player->pev ))
			continue;

		const char *name = STRING( player->pev->netname );
		if (FStrEq( name, "" ))
			continue;

		if (!player->IsBot())
			continue;	

		if (player->m_iTeam == kickTeam)
		{
			// its a bot on the right team - kick it
			SERVER_COMMAND( UTIL_VarArgs( "kick \"%s\"\n", STRING( player->pev->netname ) ) );

			return true;
		}
	}

	return false;
}

//--------------------------------------------------------------------------------------------------------------
/**
 * Return true if all of the members of the given team are bots
 */
bool UTIL_IsTeamAllBots( int team )
{
	int botCount = 0;

	for( int i=1; i <= gpGlobals->maxClients; ++i )
	{
		CBasePlayer *player = static_cast<CBasePlayer *>( UTIL_PlayerByIndex( i ) );

		if (player == NULL)
			continue;

		// skip players on other teams
		if (player->m_iTeam != team)
			continue;

		if (FNullEnt( player->pev ))
			continue;

		if (FStrEq( STRING( player->pev->netname ), "" ))
			continue;

		// if not a bot, fail the test
		if (!FBitSet( player->pev->flags, FL_FAKECLIENT ))
			return false;

		// is a bot on given team
		++botCount;
	}

	// if team is empty, there are no bots
	return (botCount) ? true : false;
}

//--------------------------------------------------------------------------------------------------------------
/**
 * Return the closest active player to the given position.
 * If 'distance' is non-NULL, the distance to the closest player is returned in it.
 */
extern CBasePlayer *UTIL_GetClosestPlayer( const Vector *pos, float *distance )
{
	CBasePlayer *closePlayer = NULL;
	float closeDistSq = 999999999999.9f;

	for ( int i = 1; i <= gpGlobals->maxClients; i++ )
	{
		CBasePlayer *player = static_cast<CBasePlayer *>( UTIL_PlayerByIndex( i ) );

		if (!IsEntityValid( player ))
			continue;

		if (!player->IsAlive())
			continue;

		float distSq = (player->pev->origin - *pos).LengthSquared();
		if (distSq < closeDistSq)
		{
			closeDistSq = distSq;
			closePlayer = static_cast<CBasePlayer *>( player );
		}
	}
	
	if (distance)
		*distance = sqrt( closeDistSq );

	return closePlayer;
}

//--------------------------------------------------------------------------------------------------------------
/**
 * Return the closest active player on the given team to the given position.
 * If 'distance' is non-NULL, the distance to the closest player is returned in it.
 */
extern CBasePlayer *UTIL_GetClosestPlayer( const Vector *pos, int team, float *distance )
{
	CBasePlayer *closePlayer = NULL;
	float closeDistSq = 999999999999.9f;

	for ( int i = 1; i <= gpGlobals->maxClients; i++ )
	{
		CBasePlayer *player = static_cast<CBasePlayer *>( UTIL_PlayerByIndex( i ) );

		if (!IsEntityValid( player ))
			continue;

		if (!player->IsAlive())
			continue;

		if (player->m_iTeam != team)
			continue;

		float distSq = (player->pev->origin - *pos).LengthSquared();
		if (distSq < closeDistSq)
		{
			closeDistSq = distSq;
			closePlayer = static_cast<CBasePlayer *>( player );
		}
	}
	
	if (distance)
		*distance = sqrt( closeDistSq );

	return closePlayer;
}

//--------------------------------------------------------------------------------------------------------------
// returns the string to be used for the bot name prefix.
const char * UTIL_GetBotPrefix()
{
	return cv_bot_prefix.string;
}

//--------------------------------------------------------------------------------------------------------------
// Takes the bot pointer and constructs the net name using the current bot name prefix.
void UTIL_ConstructBotNetName(char *name, int nameLength, const BotProfile *profile)
{
	strcpy(name, "233");
}

//--------------------------------------------------------------------------------------------------------------
/**
 * Return true if anyone on the given team can see the given spot
 */
bool UTIL_IsVisibleToTeam( const Vector &spot, int team, float maxRange )
{
	for( int i = 1; i <= gpGlobals->maxClients; ++i )
	{
		CBasePlayer *player = static_cast<CBasePlayer *>( UTIL_PlayerByIndex( i ) );

		if (player == NULL)
			continue;

		if (FNullEnt( player->pev ))
			continue;

		if (FStrEq( STRING( player->pev->netname ), "" ))
			continue;

		if (!player->IsAlive())
			continue;

		if (player->m_iTeam != team)
			continue;

		if (maxRange > 0.0f && (spot - player->Center()).IsLengthGreaterThan( maxRange ))
			continue;

		TraceResult result;
		UTIL_TraceLine( player->EyePosition(), spot, ignore_monsters, ignore_glass, ENT( player->pev ), &result );

		if (result.flFraction == 1.0f)
			return true;
	}

	return false;
}


//--------------------------------------------------------------------------------------------------------------
/**
 * Return the local player
 */
CBasePlayer *UTIL_GetLocalPlayer( void )
{
	if ( IS_DEDICATED_SERVER() )
	{
		return NULL;
	}
	return static_cast<CBasePlayer *>( UTIL_PlayerByIndex( 1 ) );
}


//------------------------------------------------------------------------------------------------------------
// Some types of entities have no origin set, so we use this instead.
Vector UTIL_ComputeOrigin( entvars_t * pevVars )
{
	if ( ( pevVars->origin.x == 0.0 ) && ( pevVars->origin.y == 0.0 ) && ( pevVars->origin.z == 0.0 ) )
		return ( pevVars->absmax + pevVars->absmin ) * 0.5;
	else
		return pevVars->origin;
}


Vector UTIL_ComputeOrigin( CBaseEntity * pEntity )
{
	return UTIL_ComputeOrigin( pEntity->pev );
}


Vector UTIL_ComputeOrigin( edict_t * pentEdict )
{
	return UTIL_ComputeOrigin( VARS( pentEdict ) );
}


//------------------------------------------------------------------------------------------------------------
void UTIL_DrawBeamFromEnt( int iIndex, Vector vecEnd, int iLifetime, byte bRed, byte bGreen, byte bBlue )
{
}




//------------------------------------------------------------------------------------------------------------
void CONSOLE_ECHO( char * pszMsg, ... )
{
	va_list     argptr;
	static char szStr[1024];

	va_start( argptr, pszMsg );
	vsprintf( szStr, pszMsg, argptr );
	va_end( argptr );

	(*g_engfuncs.pfnServerPrint)( szStr );
}


//------------------------------------------------------------------------------------------------------------
void CONSOLE_ECHO_LOGGED( char * pszMsg, ... )
{
	va_list     argptr;
	static char szStr[1024];

	va_start( argptr, pszMsg );
	vsprintf( szStr, pszMsg, argptr );
	va_end( argptr );

	(*g_engfuncs.pfnServerPrint)( szStr );
}


//------------------------------------------------------------------------------------------------------------
void BotPrecache( void )
{
}

//------------------------------------------------------------------------------------------------------------
#define COS_TABLE_SIZE 256
static float cosTable[ COS_TABLE_SIZE ];

void InitBotTrig( void )
{
	for( int i=0; i<COS_TABLE_SIZE; ++i )
	{
		float angle = 2.0f * M_PI * (float)i / (float)(COS_TABLE_SIZE-1);
		cosTable[i] = cos(angle); 
	}
}

float BotCOS( float angle )
{
	angle = NormalizeAnglePositive( angle );
	int i = angle * (COS_TABLE_SIZE-1) / 360.0f;
	return cosTable[i];
}

float BotSIN( float angle )
{
	angle = NormalizeAnglePositive( angle - 90 );
	int i = angle * (COS_TABLE_SIZE-1) / 360.0f;
	return cosTable[i];
}



//--------------------------------------------------------------------------------------------------------------
/**
 * Send a "hint" message to all players, dead or alive.
 */
void HintMessageToAllPlayers( const char *message )
{
}

