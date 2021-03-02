//========= Copyright ?1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

//
// Author: Michael S. Booth (mike@turtlerockstudios.com), 2003
//
// NOTE: The CS Bot code uses Doxygen-style comments. If you run Doxygen over this code, it will 
// auto-generate documentation.  Visit www.doxygen.org to download the system for free.
//

#ifndef BOT_H
#define BOT_H

#include "cbase.h"
#include "usercmd.h"

#include "bot_manager.h"
#include "bot_util.h"
#include "bot_constants.h"
#include "nav_mesh.h"
#include "pm_shared.h"


class BotProfile;


extern bool AreBotsAllowed();


//--------------------------------------------------------------------------------------------------------
// BOTPORT: Convert everything to assume "origin" means "feet"

//
// Utility function to get "centroid" or center of player or player equivalent
//
inline Vector GetCentroid( const CBaseEntity *player )
{
	Vector centroid = player->GetAbsOrigin();

	const Vector &mins = player->WorldAlignMins();
	const Vector &maxs = player->WorldAlignMaxs();

	centroid.z += (maxs.z - mins.z)/2.0f;

	//centroid.z += HalfHumanHeight;

	return centroid;
}


/// @todo Remove this nasty hack - CreateFakeClient() calls CBot::Spawn, which needs the profile
extern const BotProfile *g_botInitProfile;
extern int g_botInitTeam;
extern int g_nClientPutInServerOverrides;

//--------------------------------------------------------------------------------------------------------
template < class T > T * CreateBot( const BotProfile *profile, int team )
{
	if ( !AreBotsAllowed() )
		return NULL;

	if ( UTIL_ClientsInGame() >= gpGlobals->maxClients )
	{
		CONSOLE_ECHO( "Unable to create bot: Server is full (%d/%d clients).\n", UTIL_ClientsInGame(), gpGlobals->maxClients );
		return NULL;
	}

	// set the bot's name
	char botName[64];
	UTIL_ConstructBotNetName( botName, 64, profile );

	// get an edict for the bot
	// NOTE: This will ultimately invoke CBot::Spawn(), so set the profile now
	g_botInitProfile = profile;
	g_botInitTeam = team;
	edict_t *botEdict = CREATE_FAKE_CLIENT( botName );

	if ( botEdict == NULL )
	{
		CONSOLE_ECHO( "Unable to create bot: CreateFakeClient() returned null.\n" );
		return NULL;
	}

	// create an instance of the bot's class and bind it to the edict
	T *bot = GetClassPtr( (T *)VARS( botEdict ) );

	if ( bot == NULL )
	{
		Assert( false );
		Error( "Could not allocate and bind entity to bot edict.\n" );
		return NULL;
	}

	bot->ClearFlags();
	bot->AddFlag( FL_CLIENT | FL_FAKECLIENT );

	return bot;
}

//----------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------
/**
 * The base bot class from which bots for specific games are derived
 * A template is needed here because the CBot class must be derived from CBasePlayer, 
 * but also may need to be derived from a more specific player class, such as CCSPlayer
 */
class CBot : public CBasePlayer
{
public:

	CBot( void );												///< constructor initializes all values to zero
	virtual ~CBot();
	virtual bool Initialize( const BotProfile *profile, int team );		///< (EXTEND) prepare bot for action

	unsigned int GetID( void ) const		{ return m_id; }	///< return bot's unique ID

	virtual bool IsBot( void ) const { return true; }	
	virtual bool IsNetClient( void ) const { return false; }	// Bots should return FALSE for this, they can't receive NET messages

	virtual void Spawn( void );									///< (EXTEND) spawn the bot into the game

	virtual void Upkeep( void ) = 0;							///< lightweight maintenance, invoked frequently
	virtual void Update( void ) = 0;							///< heavyweight algorithms, invoked less often


	virtual void Run( void );
	virtual void Walk( void );
	virtual bool IsRunning( void ) const		{ return m_isRunning; }
	
	virtual void Crouch( void );
	virtual void StandUp( void );
	bool IsCrouching( void ) const	{ return m_isCrouching; }

	void PushPostureContext( void );							///< push the current posture context onto the top of the stack
	void PopPostureContext( void );								///< restore the posture context to the next context on the stack

	virtual void MoveForward( void );
	virtual void MoveBackward( void );
	virtual void StrafeLeft( void );
	virtual void StrafeRight( void );

	#define MUST_JUMP true
	virtual bool Jump( bool mustJump = false );					///< returns true if jump was started
	bool IsJumping( void );										///< returns true if we are in the midst of a jump
	float GetJumpTimestamp( void ) const	{ return m_jumpTimestamp; }	///< return time last jump began

	virtual void ClearMovement( void );						///< zero any MoveForward(), Jump(), etc

	const Vector &GetViewVector( void );						///< return the actual view direction


	//------------------------------------------------------------------------------------
	// Weapon interface
	//
	virtual void UseEnvironment( void );
	virtual void PrimaryAttack( void );
	virtual void ClearPrimaryAttack( void );
	virtual void TogglePrimaryAttack( void );
	virtual void SecondaryAttack( void );
	virtual void Reload( void );

	float GetActiveWeaponAmmoRatio( void ) const;				///< returns ratio of ammo left to max ammo (1 = full clip, 0 = empty)
	bool IsActiveWeaponClipEmpty( void ) const;					///< return true if active weapon has any empty clip
	bool IsActiveWeaponOutOfAmmo( void ) const;					///< return true if active weapon has no ammo at all
	bool IsActiveWeaponRecoilHigh( void ) const;				///< return true if active weapon's bullet spray has become large and inaccurate
	bool IsUsingScope( void );									///< return true if looking thru weapon's scope


	//------------------------------------------------------------------------------------
	// Event hooks
	//

	/// invoked when injured by something (EXTEND) - returns the amount of damage inflicted
//!	virtual int OnTakeDamage( const CTakeDamageInfo &info )
//!	{
//!		return PlayerType::OnTakeDamage( info );
//!	}

	/// invoked when killed (EXTEND)
//!	virtual void Event_Killed( const CTakeDamageInfo &info )
//!	{ 
//!		PlayerType::Event_Killed( info );
//!	}

	bool IsEnemy( CBaseEntity *ent ) const;						///< returns TRUE if given entity is our enemy
	int GetEnemiesRemaining( void ) const;						///< return number of enemies left alive
	int GetFriendsRemaining( void ) const;						///< return number of friends left alive

	bool IsPlayerFacingMe( CBasePlayer *enemy ) const;			///< return true if player is facing towards us
	bool IsPlayerLookingAtMe( CBasePlayer *enemy, float cosTolerance = 0.9f ) const;		///< returns true if other player is pointing right at us
	bool IsLookingAtPosition( const Vector &pos, float angleTolerance = 20.0f ) const;	///< returns true if looking (roughly) at given position

	bool IsLocalPlayerWatchingMe( void ) const;					///< return true if local player is observing this bot

	void PrintIfWatched( char *format, ... ) const;				///< output message to console if we are being watched by the local player

	virtual void UpdatePlayer( void );							///< update player physics, movement, weapon firing commands, etc
	virtual void SetModel( const char *modelName );

	virtual void BotThink( void );

	const BotProfile *GetProfile( void ) const		{ return m_profile; }	///< return our personality profile

	virtual bool ClientCommand( const char *args );				///< Do a "client command" - useful for invoking menu choices, etc.

protected:
	const BotProfile *m_profile;								///< the "personality" profile of this bot

private:
	friend class CBotManager;

	unsigned int m_id;											///< unique bot ID

	bool m_isRunning;											///< run/walk mode
	bool m_isCrouching;											///< true if crouching (ducking)
	float m_forwardSpeed;
	float m_strafeSpeed;
	float m_verticalSpeed;
	int m_buttonFlags;											///< bitfield of movement buttons

	float m_jumpTimestamp;										///< time when we last began a jump
	float m_flPreviousCommandTime;
	float m_flNextBotThink;
	float m_flNextFullBotThink;

	Vector m_viewForward;										///< forward view direction (only valid when GetViewVector() is used)

	/// the PostureContext represents the current settings of walking and crouching
	struct PostureContext
	{
		bool isRunning;
		bool isCrouching;
	};
	enum { MAX_POSTURE_STACK = 8 };
	PostureContext m_postureStack[ MAX_POSTURE_STACK ];
	int m_postureStackIndex;									///< index of top of stack

	void ResetCommand( void );
	byte ThrottledMsec( void ) const;

protected:
	virtual float GetMoveSpeed( void );									///< returns current movement speed (for walk/run)
};


//-----------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------
//
// Inlines
//

//--------------------------------------------------------------------------------------------------------------
inline void CBot::SetModel( const char *modelName )
{
}

//-----------------------------------------------------------------------------------------------------------
inline float CBot::GetMoveSpeed( void )
{
	return MaxSpeed();
}

//-----------------------------------------------------------------------------------------------------------
inline void CBot::Run( void )
{
	m_isRunning = true;
}

//-----------------------------------------------------------------------------------------------------------
inline void CBot::Walk( void )
{
	m_isRunning = false;
}

//-----------------------------------------------------------------------------------------------------------
inline bool CBot::IsActiveWeaponRecoilHigh( void ) const
{
	const Vector &angles = const_cast< CBot * >( this )->GetPunchAngle();
	const float highRecoil = -1.5f;
	return (angles.x < highRecoil);
}

//-----------------------------------------------------------------------------------------------------------
inline void CBot::PushPostureContext( void )
{
	if (m_postureStackIndex == MAX_POSTURE_STACK)
	{
		PrintIfWatched( "PushPostureContext() overflow error!\n" );
		return;
	}

	m_postureStack[ m_postureStackIndex ].isRunning = m_isRunning;
	m_postureStack[ m_postureStackIndex ].isCrouching = m_isCrouching;
	++m_postureStackIndex;
}

//-----------------------------------------------------------------------------------------------------------
inline void CBot::PopPostureContext( void )
{
	if (m_postureStackIndex == 0)
	{
		PrintIfWatched( "PopPostureContext() underflow error!\n" );
		m_isRunning = true;
		m_isCrouching = false;
		return;
	}

	--m_postureStackIndex;
	m_isRunning = m_postureStack[ m_postureStackIndex ].isRunning;
	m_isCrouching = m_postureStack[ m_postureStackIndex ].isCrouching;
}

//-----------------------------------------------------------------------------------------------------------
inline bool CBot::IsPlayerFacingMe( CBasePlayer *other ) const
{
	Vector toOther = other->GetAbsOrigin() - GetAbsOrigin();

	Vector otherForward;
	AngleVectors( other->EyeAngles() + other->GetPunchAngle(), &otherForward );

	if (DotProduct( otherForward, toOther ) < 0.0f)
		return true;

	return false;
}

//-----------------------------------------------------------------------------------------------------------
inline bool CBot::IsPlayerLookingAtMe( CBasePlayer *other, float cosTolerance ) const
{
	Vector toOther = other->GetAbsOrigin() - GetAbsOrigin();
	toOther.NormalizeInPlace();

	Vector otherForward;
	AngleVectors( other->EyeAngles() + other->GetPunchAngle(), &otherForward );

	// other player must be pointing nearly right at us to be "looking at" us
	if (DotProduct( otherForward, toOther ) < -cosTolerance)
		return true;

	return false;
}

//-----------------------------------------------------------------------------------------------------------
inline const Vector &CBot::GetViewVector( void )
{
	AngleVectors( EyeAngles() + GetPunchAngle(), &m_viewForward );
	return m_viewForward;
}

//-----------------------------------------------------------------------------------------------------------
inline bool CBot::IsLookingAtPosition( const Vector &pos, float angleTolerance ) const
{
	// forced to do this since many methods in CBaseEntity are not const, but should be
	CBot *me = const_cast< CBot * >( this );

	Vector to = pos - me->EyePosition();

	Vector idealAngles;
	VectorAngles( to, idealAngles );

	Vector viewAngles = me->EyeAngles();

	float deltaYaw = AngleNormalize( idealAngles.y - viewAngles.y );
	float deltaPitch = AngleNormalize( idealAngles.x - viewAngles.x );

	if (fabs( deltaYaw ) < angleTolerance && abs( deltaPitch ) < angleTolerance)
		return true;

	return false;
}

//--------------------------------------------------------------------------------------------------------------
inline CBot::CBot( void )
{
	// the profile will be attached after this instance is constructed
	m_profile = NULL;

	// assign this bot a unique ID
	static unsigned int nextID = 1;

	// wraparound (highly unlikely)
	if (nextID == 0)
		++nextID;

	m_id = nextID;
	++nextID;

	m_postureStackIndex = 0;
}

//--------------------------------------------------------------------------------------------------------------
inline CBot::~CBot( void )
{
}

//--------------------------------------------------------------------------------------------------------------
/**
 * Prepare bot for action
 */
inline bool CBot::Initialize( const BotProfile *profile, int team )
{
	m_profile = profile;
	return true;
}

//--------------------------------------------------------------------------------------------------------------
inline void CBot::Spawn( void )
{
	// initialize the bot (thus setting its profile)
	if (m_profile == NULL)
		Initialize( g_botInitProfile, g_botInitTeam );

	// let the base class set some things up
	CBasePlayer::Spawn();

	// Make sure everyone knows we are a bot
	AddFlag( FL_CLIENT | FL_FAKECLIENT );

	// Bots use their own thinking mechanism
	SetThink( NULL );

	m_isRunning = true;
	m_isCrouching = false;
	m_postureStackIndex = 0;

	m_jumpTimestamp = 0.0f;

	// Command interface variable initialization
	ResetCommand();
}

//--------------------------------------------------------------------------------------------------------------
inline void CBot::BotThink( void )
{
float g_flBotCommandInterval	= 1.0 / 30.0;	// 30 times per second, just like human clients
float g_flBotFullThinkInterval	= 1.0 / 15.0;	// full AI only 10 times per second

	if ( gpGlobals->time >= m_flNextBotThink )
	{
		m_flNextBotThink = gpGlobals->time + g_flBotCommandInterval;

		Upkeep();

		if ( gpGlobals->time >= m_flNextFullBotThink )
		{
			m_flNextFullBotThink = gpGlobals->time + g_flBotFullThinkInterval;

			ResetCommand();
			Update();
		}

		UpdatePlayer();
	}
}

//--------------------------------------------------------------------------------------------------------------
inline void CBot::MoveForward( void )
{
	m_forwardSpeed = GetMoveSpeed();
	SETBITS( m_buttonFlags, IN_FORWARD );

	// make mutually exclusive
	CLEARBITS( m_buttonFlags, IN_BACK );
}


//--------------------------------------------------------------------------------------------------------------
inline void CBot::MoveBackward( void )
{
	m_forwardSpeed = -GetMoveSpeed();
	SETBITS( m_buttonFlags, IN_BACK );

	// make mutually exclusive
	CLEARBITS( m_buttonFlags, IN_FORWARD );
}

//--------------------------------------------------------------------------------------------------------------
inline void CBot::StrafeLeft( void )
{
	m_strafeSpeed = -GetMoveSpeed();
	SETBITS( m_buttonFlags, IN_MOVELEFT );

	// make mutually exclusive
	CLEARBITS( m_buttonFlags, IN_MOVERIGHT );
}

//--------------------------------------------------------------------------------------------------------------
inline void CBot::StrafeRight( void )
{
	m_strafeSpeed = GetMoveSpeed();
	SETBITS( m_buttonFlags, IN_MOVERIGHT );

	// make mutually exclusive
	CLEARBITS( m_buttonFlags, IN_MOVELEFT );
}

//--------------------------------------------------------------------------------------------------------------
inline bool CBot::Jump( bool mustJump )
{
	if (IsJumping() || IsCrouching())
		return false;

	if (!mustJump)
	{
		const float minJumpInterval = 0.9f; // 1.5f;
		if (gpGlobals->time - m_jumpTimestamp < minJumpInterval)
			return false;
	}

	// still need sanity check for jumping frequency
	const float sanityInterval = 0.3f;
	if (gpGlobals->time - m_jumpTimestamp < sanityInterval)
		return false;

	// jump
	SETBITS( m_buttonFlags, IN_JUMP );
	m_jumpTimestamp = gpGlobals->time;
	return true;
}

//--------------------------------------------------------------------------------------------------------------
/**
 * Zero any MoveForward(), Jump(), etc
 */
inline void CBot::ClearMovement( void )
{
	m_forwardSpeed = 0.0;
	m_strafeSpeed = 0.0;
	m_verticalSpeed	= 100.0; // stay at the top of water, so we don't drown.  TODO: swim logic
	m_buttonFlags &= ~(IN_FORWARD | IN_BACK | IN_MOVELEFT | IN_MOVERIGHT | IN_JUMP);
}

//--------------------------------------------------------------------------------------------------------------
/**
 * Returns true if we are in the midst of a jump
 */
inline bool CBot::IsJumping( void )
{
	// if long time after last jump, we can't be jumping
	if (gpGlobals->time - m_jumpTimestamp > 3.0f)
		return false;

	// if we just jumped, we're still jumping
	if (gpGlobals->time - m_jumpTimestamp < 0.9f) // 1.0f
		return true;

	// a little after our jump, we're jumping until we hit the ground
	if (FBitSet( GetFlags(), FL_ONGROUND ))
		return false;

	return true;
}

//--------------------------------------------------------------------------------------------------------------
inline void CBot::Crouch( void )
{
	m_isCrouching = true;
}

//--------------------------------------------------------------------------------------------------------------
inline void CBot::StandUp( void )
{
	m_isCrouching = false;
}


//--------------------------------------------------------------------------------------------------------------
inline void CBot::UseEnvironment( void )
{
	SETBITS( m_buttonFlags, IN_USE );
}


//--------------------------------------------------------------------------------------------------------------
inline void CBot::PrimaryAttack( void )
{
	SETBITS( m_buttonFlags, IN_ATTACK );
}

//--------------------------------------------------------------------------------------------------------------
inline void CBot::ClearPrimaryAttack( void )
{
	CLEARBITS( m_buttonFlags, IN_ATTACK );
}

//--------------------------------------------------------------------------------------------------------------
inline void CBot::TogglePrimaryAttack( void )
{
	if (FBitSet( m_buttonFlags, IN_ATTACK ))
	{
		CLEARBITS( m_buttonFlags, IN_ATTACK );
	}
	else
	{
		SETBITS( m_buttonFlags, IN_ATTACK );
	}
}


//--------------------------------------------------------------------------------------------------------------
inline void CBot::SecondaryAttack( void )
{
	SETBITS( m_buttonFlags, IN_ATTACK2 );
}

//--------------------------------------------------------------------------------------------------------------
inline void CBot::Reload( void )
{
	SETBITS( m_buttonFlags, IN_RELOAD );
}

//--------------------------------------------------------------------------------------------------------------
/**
 * Returns ratio of ammo left to max ammo (1 = full clip, 0 = empty)
 */
inline float CBot::GetActiveWeaponAmmoRatio( void ) const
{
	return 0;
}

//--------------------------------------------------------------------------------------------------------------
/**
 * Return true if active weapon has an empty clip
 */
inline bool CBot::IsActiveWeaponClipEmpty( void ) const
{
	return true;
}

//--------------------------------------------------------------------------------------------------------------
/**
 * Return true if active weapon has no ammo at all
 */
inline bool CBot::IsActiveWeaponOutOfAmmo( void ) const
{
	return true;
}

//--------------------------------------------------------------------------------------------------------------
/**
 * Return true if looking thru weapon's scope
 */
inline bool CBot::IsUsingScope( void )
{
	// if our field of view is less than 90, we're looking thru a scope (maybe only true for CS...)
	if (GetFOV() < GetDefaultFOV())
		return true;

	return false;
}

//--------------------------------------------------------------------------------------------------------------
/**
 * Update player physics, movement, weapon firing commands, etc
 */
inline void CBot::UpdatePlayer( void )
{
	byte adjustedMSec;

	// Adjust msec to command time interval
	adjustedMSec = ThrottledMsec();

	// player model is "munged"
	pev->angles = pev->v_angle;
	pev->angles.x /= -3.0f;

	// save the command time
	m_flPreviousCommandTime = gpGlobals->time;

	if (m_isCrouching)
	{
		SETBITS( m_buttonFlags, IN_DUCK );
	}
	else if (!m_isRunning)
	{
		SETBITS( m_buttonFlags, IN_SPEED );
	}

	// Run the command
	RUN_PLAYER_MOVE( edict(), pev->v_angle, m_forwardSpeed, m_strafeSpeed, m_verticalSpeed, m_buttonFlags, 0, adjustedMSec );
}


//--------------------------------------------------------------------------------------------------------------
inline void CBot::ResetCommand( void )
{
	m_forwardSpeed = 0.0;
	m_strafeSpeed = 0.0;
	m_verticalSpeed	= 100.0; // stay at the top of water, so we don't drown.  TODO: swim logic
	m_buttonFlags = 0;
}


//--------------------------------------------------------------------------------------------------------------

inline byte CBot::ThrottledMsec( void ) const
{
	int iNewMsec;

	// Estimate Msec to use for this command based on time passed from the previous command
	iNewMsec = (int)( (gpGlobals->time - m_flPreviousCommandTime) * 1000 );
	if (iNewMsec > 255)		// Doh, bots are going to be slower than they should if this happens.
		iNewMsec = 255;		// Upgrade that CPU or use less bots!

	return (byte)iNewMsec;
}


//--------------------------------------------------------------------------------------------------------------
/**
 * Do a "client command" - useful for invoking menu choices, etc.
 */
inline bool CBot::ClientCommand( const char *args )
{
	return false;
}


//--------------------------------------------------------------------------------------------------------------
/**
 * Returns TRUE if given entity is our enemy
 */
inline bool CBot::IsEnemy( CBaseEntity *ent ) const
{
	// only Players (real and AI) can be enemies
	if (!ent->IsPlayer())
		return false;

	// corpses are no threat
	if (!ent->IsAlive())
		return false;	

	CBasePlayer *player = static_cast<CBasePlayer *>( ent );

	// if they are on our team, they are our friends
	if (player->GetTeamNumber() == GetTeamNumber())
		return false;

	// yep, we hate 'em
	return true;
}

//--------------------------------------------------------------------------------------------------------------
/**
 * Return number of enemies left alive
 */
inline int CBot::GetEnemiesRemaining( void ) const
{
	int count = 0;

	for ( int i = 1; i <= gpGlobals->maxClients; ++i )
	{
		CBaseEntity *player = UTIL_PlayerByIndex( i );

		if (player == NULL)
			continue;

		if (!IsEnemy( player ))
			continue;

		if (!player->IsAlive())
			continue;

		count++;
	}

	return count;
}

//--------------------------------------------------------------------------------------------------------------
/**
 * Return number of friends left alive
 */
inline int CBot::GetFriendsRemaining( void ) const
{
	int count = 0;

	for ( int i = 1; i <= gpGlobals->maxClients; ++i )
	{
		CBaseEntity *player = UTIL_PlayerByIndex( i );

		if (player == NULL)
			continue;

		if (IsEnemy( player ))
			continue;

		if (!player->IsAlive())
			continue;

		if (player == static_cast<CBaseEntity *>( const_cast<CBot *>( this ) ))
			continue;

		count++;
	}

	return count;
}

//--------------------------------------------------------------------------------------------------------------
/**
 * Return true if the local player is currently in observer mode watching this bot.
 */
inline bool CBot::IsLocalPlayerWatchingMe( void ) const
{
	if ( IS_DEDICATED_SERVER() )
		return false;

	CBasePlayer *player = UTIL_GetListenServerHost();
	if ( player == NULL )
		return false;

	if ( cv_bot_debug_target.GetInt() > 0 )
	{
		return entindex() == cv_bot_debug_target.GetInt();
	}

	if ( player->pev->iuser1 || !player->IsAlive() )
	{
		if ( const_cast<CBot *>(this)->entindex() == player->pev->iuser2 )
		{
			switch( player->pev->iuser1 )
			{
				case OBS_IN_EYE:
				case OBS_CHASE_LOCKED:
				case OBS_CHASE_FREE:
					return true;
			}
		}
	}

	return false;
}


//--------------------------------------------------------------------------------------------------------------
/**
 * Output message to console if we are being watched by the local player
 */
inline void CBot::PrintIfWatched( char *format, ... ) const
{
	if (cv_bot_debug.GetInt() == 0)
	{
		return;
	}

	if ((IsLocalPlayerWatchingMe() && (cv_bot_debug.GetInt() == 1 || cv_bot_debug.GetInt() == 3)) ||
		(cv_bot_debug.GetInt() == 2 || cv_bot_debug.GetInt() == 4))
	{
		va_list varg;
		char buffer[ CBotManager::MAX_DBG_MSG_SIZE ];
		const char *name = const_cast< CBot * >( this )->GetPlayerName();

		va_start( varg, format );
		vsprintf( buffer, format, varg );
		va_end( varg );

		// prefix the console message with the bot's name (this can be NULL if bot was just added)
		ClientPrint( UTIL_GetListenServerHost(),
			HUD_PRINTCONSOLE,
			UTIL_VarArgs( "%s: %s",
			(name) ? name : "(NULL netname)", buffer ) );

		TheBots->AddDebugMessage( buffer );
	}
}

//-----------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------

extern void InstallBotControl( void );
extern void RemoveBotControl( void );
extern void Bot_ServerCommand( void );
extern void Bot_RegisterCvars( void );

extern bool IsSpotOccupied( CBaseEntity *me, const Vector &pos );	// if a player is at the given spot, return true
extern const Vector *FindNearbyHidingSpot( CBaseEntity *me, const Vector &pos, float maxRange = 1000.0f, bool isSniper = false, bool useNearest = false );
extern const Vector *FindRandomHidingSpot( CBaseEntity *me, Place place, bool isSniper = false );
extern const Vector *FindNearbyRetreatSpot( CBaseEntity *me, const Vector &start, float maxRange = 1000.0f, int avoidTeam = 0 );


#endif // BOT_H
