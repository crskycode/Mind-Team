//========= Copyright ?1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

// Author: Michael S. Booth (mike@turtlerockstudios.com), 2003

#ifndef _BOT_PROFILE_H_
#define _BOT_PROFILE_H_

#pragma warning( disable : 4786 )	// long STL names get truncated in browse info.

#include "bot_constants.h"
#include "bot_util.h"
//!#include "cs_weapon_parse.h"

enum
{
	FirstCustomSkin = 100,
	NumCustomSkins = 100,
	LastCustomSkin = FirstCustomSkin + NumCustomSkins - 1,
};

	
//--------------------------------------------------------------------------------------------------------------
/**
 * A BotProfile describes the "personality" of a given bot
 */
class BotProfile
{
public:
	BotProfile( void )
	{
		m_name = NULL;
		m_aggression = 0.0f;
		m_skill = 0.5f;	
		m_teamwork = 1.0f;
//!		m_weaponPreferenceCount = 0;
		m_cost = 0;
		m_skin = 0;	
		m_difficultyFlags = 0;
		m_voicePitch = 100;
		m_reactionTime = 0.3f;
		m_attackDelay = 0.1f;
		m_teams = 0;//TEAM_UNASSIGNED;
//!		m_voiceBank = 0;
//!		m_prefersSilencer = false;
	}

	~BotProfile( void )
	{
		if ( m_name )
			delete [] m_name;
	}

	const char *GetName( void ) const					{ return m_name; }		///< return bot's name
	float GetAggression( void ) const					{ return m_aggression; }
	float GetSkill( void ) const						{ return m_skill; }
	float GetTeamwork( void ) const						{ return m_teamwork; }

//!	int GetWeaponPreference( int i ) const				{ return m_weaponPreference[ i ]; }
//!	const char *GetWeaponPreferenceAsString( int i ) const;
//!	int GetWeaponPreferenceCount( void ) const			{ return m_weaponPreferenceCount; }
//!	bool HasPrimaryPreference( void ) const;			///< return true if this profile has a primary weapon preference
//!	bool HasPistolPreference( void ) const;				///< return true if this profile has a pistol weapon preference

	int GetCost( void ) const							{ return m_cost; }
	int GetSkin( void ) const							{ return m_skin; }
	bool IsDifficulty( BotDifficultyType diff ) const;	///< return true if this profile can be used for the given difficulty level
	int GetVoicePitch( void ) const						{ return m_voicePitch; }
	float GetReactionTime( void ) const					{ return m_reactionTime; }
	float GetAttackDelay( void ) const					{ return m_attackDelay; }
//!	int GetVoiceBank() const							{ return m_voiceBank; }

//!	bool IsValidForTeam( int team ) const;

//!	bool PrefersSilencer() const						{ return m_prefersSilencer; }

//!	bool InheritsFrom( const char *name ) const;

private:	
	friend class BotProfileManager;						///< for loading profiles

//!	void Inherit( const BotProfile *parent, const BotProfile *baseline );	///< copy values from parent if they differ from baseline

	char *m_name;										///< the bot's name
	float m_aggression;									///< percentage: 0 = coward, 1 = berserker
	float m_skill;										///< percentage: 0 = terrible, 1 = expert
	float m_teamwork;									///< percentage: 0 = rogue, 1 = complete obeyance to team, lots of comm

//!	enum { MAX_WEAPON_PREFS = 16 };
//!	int m_weaponPreference[ MAX_WEAPON_PREFS ];			///< which weapons this bot likes to use, in order of priority
//!	int m_weaponPreferenceCount;

	int m_cost;											///< reputation point cost for career mode
	int m_skin;											///< "skin" index
	unsigned char m_difficultyFlags;					///< bits set correspond to difficulty levels this is valid for
	int m_voicePitch;									///< the pitch shift for bot chatter (100 = normal)
	float m_reactionTime;								//< our reaction time in seconds
	float m_attackDelay;								///< time in seconds from when we notice an enemy to when we open fire
	int m_teams;										///< teams for which this profile is valid

//!	bool m_prefersSilencer;								///< does the bot prefer to use silencers?

//!	int m_voiceBank;									///< Index of the BotChatter.db voice bank this profile uses (0 is the default)

	CUtlVector< const BotProfile * > m_templates;		///< List of templates we inherit from
};
typedef CUtlLinkedList<BotProfile *> BotProfileList;


#endif