/****
 * 
 * Copyright (c) 2017, Crsky
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 * 
 * * Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
****/

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"

#include "gamerule.h"
#include "cs_bot.h"

cvar_t cv_bot_traceview = { "bot_traceview", "0", FCVAR_SERVER, 0.0f, NULL };
cvar_t cv_bot_stop = { "bot_stop", "0", FCVAR_SERVER, 0.0f, NULL };
cvar_t cv_bot_show_nav = { "bot_show_nav", "0", FCVAR_SERVER, 0.0f, NULL };
cvar_t cv_bot_show_danger = { "bot_show_danger", "0", FCVAR_SERVER, 0.0f, NULL };
cvar_t cv_bot_nav_edit = { "bot_nav_edit", "0", FCVAR_SERVER, 0.0f, NULL };
cvar_t cv_bot_nav_zdraw = { "bot_nav_zdraw", "4", FCVAR_SERVER, 0.0f, NULL };
cvar_t cv_bot_walk = { "bot_walk", "0", FCVAR_SERVER, 0.0f, NULL };
cvar_t cv_bot_difficulty = { "bot_difficulty", "0", FCVAR_SERVER, 0.0f, NULL };
cvar_t cv_bot_debug = { "bot_debug", "0", FCVAR_SERVER, 0.0f, NULL };
cvar_t cv_bot_quicksave = { "bot_quicksave", "0", FCVAR_SERVER, 0.0f, NULL };
cvar_t cv_bot_quota = { "bot_quota", "0", FCVAR_SERVER, 0.0f, NULL };
cvar_t cv_bot_quota_match = { "bot_quota_match", "0", FCVAR_SERVER, 0.0f, NULL };
cvar_t cv_bot_prefix = { "bot_prefix", "", FCVAR_SERVER, 0.0f, NULL };
cvar_t cv_bot_allow_rogues = { "bot_allow_rogues", "1", FCVAR_SERVER, 0.0f, NULL };
cvar_t cv_bot_allow_pistols = { "bot_allow_pistols", "1", FCVAR_SERVER, 0.0f, NULL };
cvar_t cv_bot_allow_shotguns = { "bot_allow_shotguns", "1", FCVAR_SERVER, 0.0f, NULL };
cvar_t cv_bot_allow_sub_machine_guns = { "bot_allow_sub_machine_guns", "1", FCVAR_SERVER, 0.0f, NULL };
cvar_t cv_bot_allow_rifles = { "bot_allow_rifles", "1", FCVAR_SERVER, 0.0f, NULL };
cvar_t cv_bot_allow_machine_guns = { "bot_allow_machine_guns", "1", FCVAR_SERVER, 0.0f, NULL };
cvar_t cv_bot_allow_grenades = { "bot_allow_grenades", "1", FCVAR_SERVER, 0.0f, NULL };
cvar_t cv_bot_allow_snipers = { "bot_allow_snipers", "1", FCVAR_SERVER, 0.0f, NULL };
cvar_t cv_bot_allow_shield = { "bot_allow_shield", "1", FCVAR_SERVER, 0.0f, NULL };
cvar_t cv_bot_join_team = { "bot_join_team", "any", FCVAR_SERVER, 0.0f, NULL };
cvar_t cv_bot_join_after_player = { "bot_join_after_player", "1", FCVAR_SERVER, 0.0f, NULL };
cvar_t cv_bot_auto_vacate = { "bot_auto_vacate", "1", FCVAR_SERVER, 0.0f, NULL };
cvar_t cv_bot_zombie = { "bot_zombie", "0", FCVAR_SERVER, 0.0f, NULL };
cvar_t cv_bot_defer_to_human = { "bot_defer_to_human", "0", FCVAR_SERVER, 0.0f, NULL };
cvar_t cv_bot_chatter = { "bot_chatter", "normal", FCVAR_SERVER, 0.0f, NULL };
cvar_t cv_bot_profile_db = { "bot_profile_db", "BotProfile.db", FCVAR_SERVER, 0.0f, NULL };

void InstallBotControl()
{
	if (TheBots != NULL)
	{
		delete TheBots;
	}

	TheBots = TheCSBots = new CCSBotManager();
}

void Bot_ServerCommand(void)
{
	if (TheBots)
	{
		TheBots->ServerCommand(CMD_ARGV(0));
	}
}

void Bot_RegisterCvars(void)
{
	CVAR_REGISTER(&cv_bot_traceview);
	CVAR_REGISTER(&cv_bot_stop);
	CVAR_REGISTER(&cv_bot_show_nav);
	CVAR_REGISTER(&cv_bot_show_danger);
	CVAR_REGISTER(&cv_bot_nav_edit);
	CVAR_REGISTER(&cv_bot_nav_zdraw);
	CVAR_REGISTER(&cv_bot_walk);
	CVAR_REGISTER(&cv_bot_difficulty);
	CVAR_REGISTER(&cv_bot_debug);
	CVAR_REGISTER(&cv_bot_quicksave);
	CVAR_REGISTER(&cv_bot_quota);
	CVAR_REGISTER(&cv_bot_quota_match);
	CVAR_REGISTER(&cv_bot_prefix);
	CVAR_REGISTER(&cv_bot_allow_rogues);
	CVAR_REGISTER(&cv_bot_allow_pistols);
	CVAR_REGISTER(&cv_bot_allow_shotguns);
	CVAR_REGISTER(&cv_bot_allow_sub_machine_guns);
	CVAR_REGISTER(&cv_bot_allow_rifles);
	CVAR_REGISTER(&cv_bot_allow_machine_guns);
	CVAR_REGISTER(&cv_bot_allow_grenades);
	CVAR_REGISTER(&cv_bot_allow_snipers);
	CVAR_REGISTER(&cv_bot_allow_shield);
	CVAR_REGISTER(&cv_bot_join_team);
	CVAR_REGISTER(&cv_bot_join_after_player);
	CVAR_REGISTER(&cv_bot_auto_vacate);
	CVAR_REGISTER(&cv_bot_zombie);
	CVAR_REGISTER(&cv_bot_defer_to_human);
	CVAR_REGISTER(&cv_bot_chatter);
	CVAR_REGISTER(&cv_bot_profile_db);
}


// Constructor
CCSBot::CCSBot() //: m_chatter(this), m_gameState(this)
{
	;
}

// Prepare bot for action
bool CCSBot::Initialize(const BotProfile *profile)
{
	// extend
	CBot::Initialize(profile);

	// CS bot initialization
	m_diedLastRound = false;
	m_morale = POSITIVE;		// starting a new round makes everyone a little happy

	m_combatRange = RANDOM_FLOAT(325, 425);

	m_navNodeList = NULL;
	m_currentNode = NULL;

	// set initial safe time guess for this map
	m_safeTime = 15.0f + 5.0f * GetProfile()->GetAggression();

	m_name[0] = '\0';

	ResetValues();
	StartNormalProcess();

	return true;
}

// Reset internal data to initial state
void CCSBot::ResetValues()
{
//	m_chatter.Reset();
	m_gameState.Reset();

	m_avoid = NULL;
	m_avoidTimestamp = 0.0f;

	m_hurryTimer.Invalidate();

	m_isStuck = false;
	m_stuckTimestamp = 0.0f;
	m_wiggleTimestamp = 0.0f;
	m_stuckJumpTimestamp = 0.0f;

	m_pathLength = 0;
	m_pathIndex = 0;
	m_areaEnteredTimestamp = 0.0f;
	m_currentArea = NULL;
	m_lastKnownArea = NULL;

	m_avoidFriendTimer.Invalidate();
	m_isFriendInTheWay = false;
	m_isWaitingBehindFriend = false;

	m_disposition = ENGAGE_AND_INVESTIGATE;

	m_enemy = NULL;

	m_isWaitingToTossGrenade = false;
	m_wasSafe = true;

	m_nearbyEnemyCount = 0;
	m_enemyPlace = 0;
	m_nearbyFriendCount = 0;
	m_closestVisibleFriend = NULL;
	m_closestVisibleHumanFriend = NULL;

	for (int w = 0; w < ARRAYSIZE(m_watchInfo); ++w)
	{
		m_watchInfo[w].timestamp = 0.0f;
		m_watchInfo[w].isEnemy = false;
	}

	m_isEnemyVisible = false;
	m_visibleEnemyParts = NONE;
	m_lastSawEnemyTimestamp = 0.0f;
	m_firstSawEnemyTimestamp = 0.0f;
	m_currentEnemyAcquireTimestamp = 0.0f;
	m_isLastEnemyDead = true;
	m_attacker = NULL;
	m_attackedTimestamp = 0.0f;
	m_enemyDeathTimestamp = 0.0f;
	m_lastVictimID = 0;
	m_isAimingAtEnemy = false;
	m_fireWeaponTimestamp = 0.0f;
	m_equipTimer.Invalidate();

	m_isFollowing = false;
	m_leader = NULL;
	m_followTimestamp = 0.0f;
	m_allowAutoFollowTime = 0.0f;

	m_enemyQueueIndex = 0;
	m_enemyQueueCount = 0;
	m_enemyQueueAttendIndex = 0;
	m_bomber = NULL;

	m_lookAroundStateTimestamp = 0.0f;
	m_inhibitLookAroundTimestamp = 0.0f;

	m_lookPitch = 0.0f;
	m_lookPitchVel = 0.0f;
	m_lookYaw = 0.0f;
	m_lookYawVel = 0.0f;

	m_aimOffsetTimestamp = 0.0f;
	m_aimSpreadTimestamp = 0.0f;
	m_lookAtSpotState = NOT_LOOKING_AT_SPOT;

	m_spotEncounter = NULL;
	m_spotCheckTimestamp = 0.0f;
	m_peripheralTimestamp = 0.0f;

	m_avgVelIndex = 0;
	m_avgVelCount = 0;

	m_lastOrigin = (pev != NULL) ? pev->origin : Vector(0, 0, 0);

//	m_lastRadioCommand = EVENT_INVALID;
//	m_lastRadioRecievedTimestamp = 0.0f;
//	m_lastRadioSentTimestamp = 0.0f;
//	m_radioSubject = NULL;
//	m_voiceFeedbackEndTimestamp = 0.0f;

//	m_hostageEscortCount = 0;
//	m_hostageEscortCountTimestamp = 0.0f;

	m_noisePosition = Vector(0, 0, 0);
	m_noiseTimestamp = 0.0f;
	m_noiseCheckTimestamp = 0.0f;
	m_isNoiseTravelRangeChecked = false;

	m_stateTimestamp = 0.0f;
	m_task = SEEK_AND_DESTROY;
	m_taskEntity = NULL;

	m_approachPointCount = 0;
	m_approachPointViewPosition = Vector(0, 0, 0);

	m_checkedHidingSpotCount = 0;
	m_isJumpCrouching = false;

	StandUp();
	Run();
	m_mustRunTimer.Invalidate();
	m_repathTimer.Invalidate();
	m_pathLadder = NULL;

	m_huntState.ClearHuntArea();

	// adjust morale - if we died, our morale decreased,
	// but if we live, no adjustement (round win/loss also adjusts morale
	if (m_diedLastRound)
		DecreaseMorale();

	m_diedLastRound = false;

	// IsRogue() randomly changes this
	m_isRogue = false;

	m_surpriseDelay = 0.0f;
	m_surpriseTimestamp = 0.0f;

	// even though these are EHANDLEs, they need to be NULL-ed
	m_goalEntity = NULL;
	m_avoid = NULL;
	m_enemy = NULL;

#ifdef REGAMEDLL_FIXES
	for (int i = 0; i < MAX_ENEMY_QUEUE; ++i)
	{
		m_enemyQueue[i].player = NULL;
		m_enemyQueue[i].isReloading = false;
		m_enemyQueue[i].isProtectedByShield = false;
	}
#endif

	// start in idle state
	StopAttacking();
	Idle();
}

// Called when bot is placed in map, and when bots are reset after a round ends.
// NOTE: For some reason, this can be called twice when a bot is added.
void CCSBot::SpawnBot()
{
	TheCSBots->ValidateMapData();
	ResetValues();

	Q_strcpy(m_name, STRING(pev->netname));

//	SetState(&m_buyState);
	SetTouch(&CCSBot::BotTouch);

	if (TheNavAreaList.empty() && !TheCSBots->IsLearningMap())
	{
		TheCSBots->SetLearningMapFlag();
		StartLearnProcess();
	}
}

void CCSBot::RoundRespawn()
{
	// do the normal player spawn process
//	CBasePlayer::RoundRespawn();
//	EndVoiceFeedback();
}

void CCSBot::Disconnect()
{
//	EndVoiceFeedback();

	if (m_processMode != PROCESS_NORMAL)
	{
		hideProgressMeter();
	}
}
