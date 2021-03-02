//========= Copyright ?1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

// Author: Michael S. Booth (mike@turtlerockstudios.com), 2003

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "source.h"

#include "bot.h"
#include "bot_util.h"
#include "bot_profile.h"

#include "cs_bot.h"
#include <ctype.h>

static int s_iBeamSprite = 0;

//--------------------------------------------------------------------------------------------------------------
/**
 * Return true if given name is already in use by another player
 */
bool UTIL_IsNameTaken( const char *name, bool ignoreHumans )
{
	for ( int i = 1; i <= gpGlobals->maxClients; ++i )
	{
		CBasePlayer *player = static_cast<CBasePlayer *>( UTIL_PlayerByIndex( i ) );

		if (player == NULL)
			continue;

		if (player->IsPlayer() && player->IsBot())
		{
			// bots can have prefixes so we need to check the name
			// against the profile name instead.
			CCSBot *bot = dynamic_cast<CCSBot *>(player);
			if ( bot && bot->GetProfile()->GetName() && FStrEq(name, bot->GetProfile()->GetName()))
			{
				return true;
			}
		}
		else
		{
			if (!ignoreHumans)
			{
				if (FStrEq( name, player->GetPlayerName() ))
					return true;
			}
		}
	}

	return false;
}


//--------------------------------------------------------------------------------------------------------------
int UTIL_ClientsInGame( void )
{
	int count = 0;

	for ( int i = 1; i <= gpGlobals->maxClients; ++i )
	{
		CBaseEntity *player = UTIL_PlayerByIndex( i );

		if (player == NULL)
			continue;

		count++;
	}

	return count;
}

//--------------------------------------------------------------------------------------------------------------
/**
 * Return the number of non-bots on the given team
 */
int UTIL_HumansOnTeam( int teamID, bool isAlive )
{
	int count = 0;

	for ( int i = 1; i <= gpGlobals->maxClients; ++i )
	{
		CBaseEntity *entity = UTIL_PlayerByIndex( i );

		if ( entity == NULL )
			continue;

		CBasePlayer *player = static_cast<CBasePlayer *>( entity );

		if (player->IsBot())
			continue;

		if (player->GetTeamNumber() != teamID)
			continue;

		if (isAlive && !player->IsAlive())
			continue;

		count++;
	}

	return count;
}


//--------------------------------------------------------------------------------------------------------------
int UTIL_BotsInGame( void )
{
	int count = 0;

	for (int i = 1; i <= gpGlobals->maxClients; ++i )
	{
		CBasePlayer *player = static_cast<CBasePlayer *>(UTIL_PlayerByIndex( i ));

		if ( player == NULL )
			continue;

		if ( !player->IsBot() )
			continue;

		count++;
	}

	return count;
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

		if (!player->IsBot())
			continue;	

		if (!player->IsAlive() && player->GetTeamNumber() == kickTeam)
		{
			// its a bot on the right team - kick it
			UTIL_ServerCommand( UTIL_VarArgs( "kick \"%s\"\n", player->GetPlayerName() ) );

			return true;
		}
	}

	// no dead bots, kick any bot on the given team
	for ( i = 1; i <= gpGlobals->maxClients; ++i )
	{
		CBasePlayer *player = static_cast<CBasePlayer *>( UTIL_PlayerByIndex( i ) );

		if (player == NULL)
			continue;

		if (!player->IsBot())
			continue;	

		if (player->GetTeamNumber() == kickTeam)
		{
			// its a bot on the right team - kick it
			UTIL_ServerCommand( UTIL_VarArgs( "kick \"%s\"\n", player->GetPlayerName() ) );

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
		if (player->GetTeamNumber() != team)
			continue;

		// if not a bot, fail the test
		if (!player->IsBot())
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
extern CBasePlayer *UTIL_GetClosestPlayer( const Vector &pos, float *distance )
{
	CBasePlayer *closePlayer = NULL;
	float closeDistSq = 999999999999.9f;

	for ( int i = 1; i <= gpGlobals->maxClients; ++i )
	{
		CBasePlayer *player = static_cast<CBasePlayer *>( UTIL_PlayerByIndex( i ) );

		if (!IsEntityValid( player ))
			continue;

		if (!player->IsAlive())
			continue;

		Vector playerOrigin = GetCentroid( player );
		float distSq = (playerOrigin - pos).LengthSqr();
		if (distSq < closeDistSq)
		{
			closeDistSq = distSq;
			closePlayer = static_cast<CBasePlayer *>( player );
		}
	}
	
	if (distance)
		*distance = (float)sqrt( closeDistSq );

	return closePlayer;
}

//--------------------------------------------------------------------------------------------------------------
/**
 * Return the closest active player on the given team to the given position.
 * If 'distance' is non-NULL, the distance to the closest player is returned in it.
 */
extern CBasePlayer *UTIL_GetClosestPlayer( const Vector &pos, int team, float *distance )
{
	CBasePlayer *closePlayer = NULL;
	float closeDistSq = 999999999999.9f;

	for ( int i = 1; i <= gpGlobals->maxClients; ++i )
	{
		CBasePlayer *player = static_cast<CBasePlayer *>( UTIL_PlayerByIndex( i ) );

		if (!IsEntityValid( player ))
			continue;

		if (!player->IsAlive())
			continue;

		if (player->GetTeamNumber() != team)
			continue;

		Vector playerOrigin = GetCentroid( player );
		float distSq = (playerOrigin - pos).LengthSqr();
		if (distSq < closeDistSq)
		{
			closeDistSq = distSq;
			closePlayer = static_cast<CBasePlayer *>( player );
		}
	}
	
	if (distance)
		*distance = (float)sqrt( closeDistSq );

	return closePlayer;
}

//--------------------------------------------------------------------------------------------------------------
// Takes the bot pointer and constructs the net name using the current bot name prefix.
void UTIL_ConstructBotNetName( char *name, int nameLength, const BotProfile *profile )
{
	if (profile == NULL)
	{
		name[0] = 0;
		return;
	}

	// if there is no bot prefix just use the profile name.
//!	if ((cv_bot_prefix.GetString() == NULL) || (strlen(cv_bot_prefix.GetString()) == 0))
//!	{
//!		Q_strncpy( name, profile->GetName(), nameLength );
//!		return;
//!	}

	// find the highest difficulty
//!	const char *diffStr = BotDifficultyName[0];
//!	for ( int i=BOT_EXPERT; i>0; --i )
//!	{
//!		if ( profile->IsDifficulty( (BotDifficultyType)i ) )
//!		{
//!			diffStr = BotDifficultyName[i];
//!			break;
//!		}
//!	}

//!	const char *weaponStr = NULL;
//!	if ( profile->GetWeaponPreferenceCount() )
//!	{
//!		weaponStr = profile->GetWeaponPreferenceAsString( 0 );
//!
//!		const char *translatedAlias = GetTranslatedWeaponAlias( weaponStr );
//!
//!		char wpnName[128];
//!		Q_snprintf( wpnName, sizeof( wpnName ), "weapon_%s", translatedAlias );
//!		WEAPON_FILE_INFO_HANDLE	hWpnInfo = LookupWeaponInfoSlot( wpnName );
//!		if ( hWpnInfo != GetInvalidWeaponInfoHandle() )
//!		{
//!			CCSWeaponInfo *pWeaponInfo = dynamic_cast< CCSWeaponInfo* >( GetFileWeaponInfoFromHandle( hWpnInfo ) );
//!			if ( pWeaponInfo )
//!			{
//!				CSWeaponType weaponType = pWeaponInfo->m_WeaponType;
//!				weaponStr = WeaponClassAsString( weaponType );
//!			}
//!		}
//!	}
//!	if ( !weaponStr )
//!	{
//!		weaponStr = "";
//!	}

//!	char skillStr[16];
//!	Q_snprintf( skillStr, sizeof( skillStr ), "%.0f", profile->GetSkill()*100 );

//!	char temp[MAX_PLAYER_NAME_LENGTH*2];
//!	char prefix[MAX_PLAYER_NAME_LENGTH*2];
//!	Q_strncpy( temp, cv_bot_prefix.GetString(), sizeof( temp ) );
//!	Q_StrSubst( temp, "<difficulty>", diffStr, prefix, sizeof( prefix ) );
//!	Q_StrSubst( prefix, "<weaponclass>", weaponStr, temp, sizeof( temp ) );
//!	Q_StrSubst( temp, "<skill>", skillStr, prefix, sizeof( prefix ) );
//!	Q_snprintf( name, nameLength, "%s %s", prefix, profile->GetName() );
	strcpy( name, "233" );
}


//--------------------------------------------------------------------------------------------------------------
/**
 * Return true if anyone on the given team can see the given spot
 */
bool UTIL_IsVisibleToTeam( const Vector &spot, int team )
{
	for( int i = 1; i <= gpGlobals->maxClients; ++i )
	{
		CBasePlayer *player = static_cast<CBasePlayer *>( UTIL_PlayerByIndex( i ) );

		if (player == NULL)
			continue;

		if (!player->IsAlive())
			continue;

		if (player->GetTeamNumber() != team)
			continue;

		TraceResult result;
		UTIL_TraceLine( player->EyePosition(), spot, ignore_monsters, ignore_glass, player->edict(), &result );

		if (result.flFraction == 1.0f)
			return true;
	}

	return false;
}


//------------------------------------------------------------------------------------------------------------
void CONSOLE_ECHO( char * pszMsg, ... )
{
	va_list     argptr;
	static char szStr[1024];

	va_start( argptr, pszMsg );
	vsprintf( szStr, pszMsg, argptr );
	va_end( argptr );

	Msg( "%s", szStr );
}


//------------------------------------------------------------------------------------------------------------
void BotPrecache( void )
{
	s_iBeamSprite = PRECACHE_MODEL( "sprites/smoke.spr" );
}

//------------------------------------------------------------------------------------------------------------
#define COS_TABLE_SIZE 256
static float cosTable[ COS_TABLE_SIZE ];

void InitBotTrig( void )
{
	for( int i=0; i<COS_TABLE_SIZE; ++i )
	{
		float angle = (float)(2.0f * M_PI * i / (float)(COS_TABLE_SIZE-1));
		cosTable[i] = (float)cos( angle ); 
	}
}

float BotCOS( float angle )
{
	angle = AngleNormalizePositive( angle );
	int i = (int)( angle * (COS_TABLE_SIZE-1) / 360.0f );
	return cosTable[i];
}

float BotSIN( float angle )
{
	angle = AngleNormalizePositive( angle - 90 );
	int i = (int)( angle * (COS_TABLE_SIZE-1) / 360.0f );
	return cosTable[i];
}


//--------------------------------------------------------------------------------------------------------------
/**
 * Send a "hint" message to all players, dead or alive.
 */
void HintMessageToAllPlayers( const char *message )
{
}

//--------------------------------------------------------------------------------------------------------------------
/**
 * Return true if moving from "start" to "finish" will cross a player's line of fire.
 * The path from "start" to "finish" is assumed to be a straight line.
 * "start" and "finish" are assumed to be points on the ground.
 */
bool IsCrossingLineOfFire( const Vector &start, const Vector &finish, CBaseEntity *ignore, int ignoreTeam  )
{
	for ( int p=1; p <= gpGlobals->maxClients; ++p )
	{
		CBasePlayer *player = static_cast<CBasePlayer *>( UTIL_PlayerByIndex( p ) );

		if (!IsEntityValid( player ))
			continue;

		if (player == ignore)
			continue;

		if (!player->IsAlive())
			continue;

		if (ignoreTeam && player->GetTeamNumber() == ignoreTeam)
			continue;

		// compute player's unit aiming vector 
		Vector viewForward;
		AngleVectors( player->EyeAngles() + player->GetPunchAngle(), &viewForward );

		const float longRange = 5000.0f;
		Vector playerOrigin = GetCentroid( player );
		Vector playerTarget = playerOrigin + longRange * viewForward;

		Vector result( 0, 0, 0 );
		if (IsIntersecting2D( start, finish, playerOrigin, playerTarget, &result ))
		{
			// simple check to see if intersection lies in the Z range of the path
			float loZ, hiZ;

			if (start.z < finish.z)
			{
				loZ = start.z;
				hiZ = finish.z;
			}
			else
			{
				loZ = finish.z;
				hiZ = start.z;
			}

			if (result.z >= loZ && result.z <= hiZ + HumanHeight)
				return true;
		}
	}

	return false;
}


//--------------------------------------------------------------------------------------------------------------
/**
* Performs a simple case-insensitive string comparison, honoring trailing * wildcards
*/
bool WildcardMatch( const char *query, const char *test )
{
	if ( !query || !test )
		return false;

	while ( *test && *query )
	{
		char nameChar = *test;
		char queryChar = *query;
		if ( tolower(nameChar) != tolower(queryChar) ) // case-insensitive
			break;
		++test;
		++query;
	}

	if ( *query == 0 && *test == 0 )
		return true;

	// Support trailing *
	if ( *query == '*' )
		return true;

	return false;
}



