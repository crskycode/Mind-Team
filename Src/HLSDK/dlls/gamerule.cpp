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
#include "weapons.h"
#include "gamerule.h"
#include "message.h"


cvar_t mp_ruletype = { "mp_ruletype", "0", FCVAR_SERVER };
cvar_t mp_subruletype = { "mp_subruletype", "0", FCVAR_SERVER };
cvar_t mp_targetkill = { "mp_targetkill", "0", FCVAR_SERVER };
cvar_t mp_targettime = { "mp_targettime", "0", FCVAR_SERVER };
cvar_t mp_maxplayer = { "mp_maxplayer", "0", FCVAR_SERVER };
cvar_t mp_maxobserver = { "mp_maxobserver", "0", FCVAR_SERVER };
cvar_t mp_spawntime = { "mp_spawntime", "10", FCVAR_SERVER };


//----------------------------------------------------------------------
// 团队竞技
//----------------------------------------------------------------------

//------------------------------------------------------------
// Purpose : 
//------------------------------------------------------------
void CGameRule_TD::ServerActivate(void)
{
	// Register to engine
	CVAR_REGISTER(&mp_ruletype);
	CVAR_REGISTER(&mp_subruletype);
	CVAR_REGISTER(&mp_targetkill);
	CVAR_REGISTER(&mp_targettime);
	CVAR_REGISTER(&mp_maxplayer);
	CVAR_REGISTER(&mp_maxobserver);
	CVAR_REGISTER(&mp_spawntime);

	// Read Config from file

	if (IS_DEDICATED_SERVER())
	{
		const char *servercfgfile = CVAR_GET_STRING("servercfgfile");

		if (servercfgfile && *servercfgfile)
		{
			char szCommand[256];

			ALERT(at_console, "Executing dedicated server config file\n");
			sprintf(szCommand, "exec %s\n", servercfgfile);
			SERVER_COMMAND(szCommand);
			SERVER_EXECUTE();
		}
	}
	else
	{
		const char *lservercfgfile = CVAR_GET_STRING("lservercfgfile");

		if (lservercfgfile && *lservercfgfile)
		{
			char szCommand[256];

			ALERT(at_console, "Executing listen server config file\n");
			sprintf(szCommand, "exec %s\n", lservercfgfile);
			SERVER_COMMAND(szCommand);
			SERVER_EXECUTE();
		}
	}

	int iNumGRSpot, iNumBLSpot;

	iNumGRSpot = UTIL_EntityCountByClassname("info_player_start");
	iNumBLSpot = UTIL_EntityCountByClassname("info_player_deathmatch");

	// Check total spawn spot
	if (iNumGRSpot < MAX_TEAMPLAYERS || iNumBLSpot < MAX_TEAMPLAYERS)
	{
		Sys_Error("This world(BSP) not enough spawn spot");
		return;
	}

	// Get Sub Rule from config
	m_iRuleType = CVAR_GET_FLOAT("mp_subruletype");

	// Check Sub Rule
	if (m_iRuleType != SUBRULETYPE_KILL && m_iRuleType != SUBRULETYPE_TIME)
	{
		m_iRuleType = SUBRULETYPE_KILL;
		CVAR_SET_FLOAT("mp_subruletype", SUBRULETYPE_KILL);
	}

	m_iTargetKill = CVAR_GET_FLOAT("mp_targetkill");

	// Check KillTarget
	if (m_iTargetKill != 40 && m_iTargetKill != 60 && m_iTargetKill != 80 && m_iTargetKill != 100 && m_iTargetKill != 150)
	{
		m_iTargetKill = 40;
		CVAR_SET_FLOAT("mp_targetkill", 40);
	}

	m_iTargetTime = CVAR_GET_FLOAT("mp_targettime");

	// Check GameTime
	if (m_iTargetTime != 5 && m_iTargetTime != 8 && m_iTargetTime != 10 && m_iTargetTime != 12)
	{
		m_iTargetTime = 5;
		CVAR_SET_FLOAT("mp_targettime", 5);
	}

	// Make sure 20minue
	if (m_iRuleType == SUBRULETYPE_TIME)
	{
		m_iTargetTime = 20;
		CVAR_SET_FLOAT("mp_targettime", 20);
	}

	m_iSpawnTime = CVAR_GET_FLOAT("mp_spawntime");

	// Check SpawnTime
	if (m_iSpawnTime != 0 && m_iSpawnTime != 3 && m_iSpawnTime != 5 && m_iSpawnTime != 10)
	{
		m_iSpawnTime = 0;
		CVAR_SET_FLOAT("mp_spawntime", 0);
	}

	m_iMaxPlayer = CVAR_GET_FLOAT("mp_maxplayer");

	// Check MaxPlayer
	if (m_iMaxPlayer != 2 && m_iMaxPlayer != 4 && m_iMaxPlayer != 6 && m_iMaxPlayer != 8 && m_iMaxPlayer != 10 && m_iMaxPlayer != 12 && m_iMaxPlayer != 14 && m_iMaxPlayer != 16)
	{
		m_iMaxPlayer = 16;
		CVAR_SET_FLOAT("mp_maxplayer", 16);
	}

	m_iMaxObserver = CVAR_GET_FLOAT("mp_maxobserver");

	// Check MaxObserver
	if (m_iMaxObserver != 0 && m_iMaxObserver != 2 && m_iMaxObserver != 4 && m_iMaxObserver != 6 && m_iMaxObserver != 8 && m_iMaxObserver != 10 && m_iMaxObserver != 12 && m_iMaxObserver != 14)
	{
		m_iMaxObserver = 0;
		CVAR_SET_FLOAT("mp_maxobserver", 0);
	}

	// Check observer enable
	if (m_iMaxObserver > 0)
	{
		m_iMaxPlayer = MAX_PLAYERS - m_iMaxObserver;
		CVAR_SET_FLOAT("mp_maxplayer", m_iMaxPlayer);
	}

	// Initialize Spawn spot
	for (int i = 0; i < 32; i++)
	{
		m_rgSpawnSpot[i] = NULL;
	}

	// Game Start !
	m_bGameOver = FALSE;
	m_flGameStarTime = gpGlobals->time;
	m_bFreezePeriod = TRUE;
	m_bGameEnding = FALSE;
	m_iWinStatus = WINSTATUS_NO;

	// Initialize player counter
	m_iNumPlayerGR = 0;
	m_iNumPlayerBL = 0;
	m_iNumObserverGR = 0;
	m_iNumObserverBL = 0;

	// Initialize team score info
	m_iKillCountGR = 0;
	m_iKillCountBL = 0;
}

//------------------------------------------------------------
// Purpose : The server want to shutdown
//------------------------------------------------------------
void CGameRule_TD::ServerDeactivate(void)
{
	m_bGameOver = TRUE;
}

//------------------------------------------------------------
// Purpose : Running in every frame, check time out for game
//------------------------------------------------------------
void CGameRule_TD::Think(void)
{
	// Don't do the Think, if game over
	if (m_bGameOver)
	{
		return;
	}

	// Check time out for freeze period
	if (m_bFreezePeriod && gpGlobals->time - m_flGameStarTime > 3)
	{
		m_bFreezePeriod = FALSE;

		// go go go !
		MESSAGE_BEGIN(MSG_ALL, gmsgMsgBox);
		WRITE_STRING("消灭你看到的每个敌人。");
		MESSAGE_END();
	}

	int iRoundTime = m_iTargetTime * 60;

	// Check time out for game
	if (!m_bGameEnding && gpGlobals->time - m_flGameStarTime > iRoundTime)
	{
		if (m_iKillCountGR > m_iKillCountBL)
		{
			m_iWinStatus = WINSTATUS_GR;

			MESSAGE_BEGIN(MSG_ALL, gmsgWinStatus);
			WRITE_BYTE(WINSTATUS_GR);
			MESSAGE_END();
		}
		else if (m_iKillCountBL > m_iKillCountGR)
		{
			m_iWinStatus = WINSTATUS_BL;

			MESSAGE_BEGIN(MSG_ALL, gmsgWinStatus);
			WRITE_BYTE(WINSTATUS_BL);
			MESSAGE_END();
		}
		else
		{
			m_iWinStatus = WINSTATUS_DR;

			MESSAGE_BEGIN(MSG_ALL, gmsgWinStatus);
			WRITE_BYTE(WINSTATUS_DR);
			MESSAGE_END();
		}

		// Wait some time, client need to display message
		m_bGameEnding = TRUE;
		m_flGameEndTime = gpGlobals->time;
	}

	// Check time out for game over, all client should disconnect now
	if (m_bGameEnding && gpGlobals->time - m_flGameEndTime > 5)
	{
		//m_bGameEnding = FALSE;	//不用改回来也可以
		m_bGameOver = TRUE;

		// Tell all player's client need to disconnect
		// TODO: Kick the Bot
		MESSAGE_BEGIN(MSG_ALL, gmsgGameOver);
		MESSAGE_END();
	}
}

//------------------------------------------------------------
// Purpose : A client put in the server, make some message ?
//------------------------------------------------------------
void CGameRule_TD::PlayerConnect(CBasePlayer *pPlayer)
{
}

//------------------------------------------------------------
// Purpose : A client has been disconnected from server,
//  check team member for game
//------------------------------------------------------------
void CGameRule_TD::PlayerDisconnect(CBasePlayer *pPlayer)
{
	// Decrement player counter
	if (!pPlayer->m_bIsObserver)
	{
		if (pPlayer->m_iTeam == TEAM_GR)
			m_iNumPlayerGR--;
		else
			m_iNumPlayerBL--;
	}
	else
	{
		if (pPlayer->m_iTeam == TEAM_GR)
			m_iNumObserverGR--;
		else
			m_iNumObserverBL--;
	}

	// Remake the 'ACE'
	MakeAce();

	if (m_iNumPlayerGR == 0 && m_iNumPlayerBL == 0)
	{
		// Not have any player currently, don't do the Think
		m_bGameOver = TRUE;
		return;
	}

	// If not have any member in GR team, so Winner is the BL team
	if (m_iNumPlayerGR == 0)
	{
		m_iWinStatus = WINSTATUS_BL;

		MESSAGE_BEGIN(MSG_ALL, gmsgWinStatus);
		WRITE_BYTE(WINSTATUS_BL);
		MESSAGE_END();

		m_bGameEnding = TRUE;
		m_flGameEndTime = gpGlobals->time;

		return;
	}

	// If not have any member in BL team, so Winner is the GR team
	if (m_iNumPlayerBL == 0)
	{
		m_iWinStatus = WINSTATUS_GR;

		MESSAGE_BEGIN(MSG_ALL, gmsgWinStatus);
		WRITE_BYTE(WINSTATUS_GR);
		MESSAGE_END();

		m_bGameEnding = TRUE;
		m_flGameEndTime = gpGlobals->time;

		return;
	}
}

//------------------------------------------------------------
// Purpose : 
//------------------------------------------------------------
BOOL CGameRule_TD::TeamFull(int team)
{
	if (team == TEAM_GR)
	{
		if (m_iNumPlayerGR + m_iNumObserverGR < m_iMaxPlayer / 2)
			return TRUE;
	}
	else if (team == TEAM_BL)
	{
		if (m_iNumPlayerBL + m_iNumObserverBL < m_iMaxPlayer / 2)
			return TRUE;
	}

	return FALSE;
}

//------------------------------------------------------------
// Purpose : 
//------------------------------------------------------------
int CGameRule_TD::TotalPlayers(int team)
{
	if (team = TEAM_GR)
	{
		return m_iNumPlayerGR;
	}
	else if (team == TEAM_BL)
	{
		return m_iNumPlayerBL;
	}

	return 0;
}


//------------------------------------------------------------
// Purpose : Called from CBasePlayer::JoiningThink,
// return TRUE, this player can join to game, or failed to join
//------------------------------------------------------------
BOOL CGameRule_TD::PlayerCanJoin(CBasePlayer *pPlayer)
{
	// Can't join if game over
	if (m_bGameEnding || m_bGameOver)
		return FALSE;

	// Check current team member full

	if (!pPlayer->m_bIsObserver)
	{
		if (pPlayer->m_iTeam == TEAM_GR)
		{
			if (m_iNumPlayerGR == m_iMaxPlayer / 2)
				return FALSE;
		}
		else
		{
			if (m_iNumPlayerBL == m_iMaxPlayer / 2)
				return FALSE;
		}
	}
	else
	{
		if (pPlayer->m_iTeam == TEAM_GR)
		{
			if (m_iNumObserverGR == m_iMaxObserver / 2)
				return FALSE;
		}
		else
		{
			if (m_iNumObserverBL == m_iMaxObserver / 2)
				return FALSE;
		}
	}

	// Successfully
	return TRUE;
}

//------------------------------------------------------------
// Purpose : A player has been joined, tall all player's client,
//  and tell this player's client other player's info
//------------------------------------------------------------
void CGameRule_TD::PlayerJoin(CBasePlayer *pPlayer)
{
	// Increment player counter
	if (!pPlayer->m_bIsObserver)
	{
		// Joined as player
		if (pPlayer->m_iTeam == TEAM_GR)
			m_iNumPlayerGR++;
		else
			m_iNumPlayerBL++;
	}
	else
	{
		// Joined as observer
		if (pPlayer->m_iTeam == TEAM_GR)
			m_iNumObserverGR++;
		else
			m_iNumObserverBL++;
	}

	// Initialize this player's data
	pPlayer->m_iNumSpawns = 0;
	pPlayer->m_iScoreAttrib = 0;
	pPlayer->m_iKills = 0;
	pPlayer->m_iDeads = 0;

	// Tell this player's client current game mode playing
	MESSAGE_BEGIN(MSG_ONE, gmsgGameMode, NULL, pPlayer->edict());
	WRITE_BYTE(GAMEMODE_TD);
	WRITE_BYTE(m_iRuleType);
	MESSAGE_END();

	// Tell this player's client current team score info
	MESSAGE_BEGIN(MSG_ONE, gmsgTeamScore, NULL, pPlayer->edict());
	WRITE_SHORT(m_iTargetKill);
	WRITE_SHORT(m_iKillCountGR);
	WRITE_SHORT(m_iKillCountBL);
	MESSAGE_END();

	short sTimeRemaining = (m_iTargetTime * 60) - (gpGlobals->time - m_flGameStarTime);

	// Tell this player's client current time remaining
	MESSAGE_BEGIN(MSG_ONE, gmsgRoundTime, NULL, pPlayer->edict());
	WRITE_SHORT(sTimeRemaining);
	MESSAGE_END();

	// Tell all clients this player's class
	MESSAGE_BEGIN(MSG_ALL, gmsgClassInfo);
	WRITE_BYTE(pPlayer->GetId());
	WRITE_BYTE(pPlayer->m_iClass);
	MESSAGE_END();

	// Tell all clients this player's team
	MESSAGE_BEGIN(MSG_ALL, gmsgTeamInfo);
	WRITE_BYTE(pPlayer->GetId());
	WRITE_BYTE(pPlayer->m_iTeam);
	MESSAGE_END();

	// Tell all clients this player's scoreboard attrib
	MESSAGE_BEGIN(MSG_ALL, gmsgScoreAttrib);
	WRITE_BYTE(pPlayer->GetId());
	WRITE_BYTE(pPlayer->m_iScoreAttrib);
	MESSAGE_END();

	// Tell all clients this player's score
	MESSAGE_BEGIN(MSG_ALL, gmsgScoreInfo);
	WRITE_BYTE(pPlayer->GetId());
	WRITE_SHORT(pPlayer->m_iKills);
	WRITE_SHORT(pPlayer->m_iDeads);
	MESSAGE_END();

	// Tell all clients this player's ACE type
	MESSAGE_BEGIN(MSG_ALL, gmsgAceType);
	WRITE_BYTE(pPlayer->GetId());
	WRITE_BYTE(pPlayer->m_iAceType);
	MESSAGE_END();

	// Tell this player's client other player's info
	for (int i = 1; i <= gpGlobals->maxClients; i++)
	{
		// Get player pointer in 1~maxClients
		CBasePlayer *pl = UTIL_PlayerByIndex(i);

		// Invalid pointer
		if (!pl)
			continue;

		// This player did not join
		if (pl->m_iJoiningState != JOINED)
			continue;

		// Already telled
		if (pl == pPlayer)
			continue;

		// Class info
		MESSAGE_BEGIN(MSG_ONE, gmsgClassInfo, NULL, pPlayer->edict());
		WRITE_BYTE(pl->GetId());
		WRITE_BYTE(pl->m_iClass);
		MESSAGE_END();

		// Team info
		MESSAGE_BEGIN(MSG_ONE, gmsgTeamInfo, NULL, pPlayer->edict());
		WRITE_BYTE(pl->GetId());
		WRITE_BYTE(pl->m_iTeam);
		MESSAGE_END();

		// Scoreboard attrib
		MESSAGE_BEGIN(MSG_ONE, gmsgScoreAttrib, NULL, pPlayer->edict());
		WRITE_BYTE(pl->GetId());
		WRITE_BYTE(pl->m_iScoreAttrib);
		MESSAGE_END();

		// Score info
		MESSAGE_BEGIN(MSG_ONE, gmsgScoreInfo, NULL, pPlayer->edict());
		WRITE_BYTE(pl->GetId());
		WRITE_SHORT(pl->m_iKills);
		WRITE_SHORT(pl->m_iDeads);
		MESSAGE_END();

		// ACE type
		MESSAGE_BEGIN(MSG_ONE, gmsgAceType, NULL, pPlayer->edict());
		WRITE_BYTE(pl->GetId());
		WRITE_BYTE(pl->m_iAceType);
		MESSAGE_END();
	}

	// Successfully join, try to Spawn
	if (!pPlayer->m_bIsObserver && PlayerCanSpawn(pPlayer))
	{
		pPlayer->Spawn();
	}
}

//------------------------------------------------------------
// Purpose : Called from CBasePlayer::PreThink try to spawn,
//  return TRUE, this player can Spawn
//------------------------------------------------------------
BOOL CGameRule_TD::PlayerCanSpawn(CBasePlayer *pPlayer)
{
	// Can't Spawn, if game over
	if (m_bGameEnding || m_bGameOver)
		return FALSE;

	return TRUE;
}

//------------------------------------------------------------
// Purpose : A player has been spawned, he need to move to spawn
//  spot, and update the scoreborad attrib
//------------------------------------------------------------
void CGameRule_TD::PlayerSpawn(CBasePlayer *pPlayer)
{
	// Shift to 0~31
	int nIndex = pPlayer->GetId() - 1;

	// Move to Spawn spot
	if (m_rgSpawnSpot[nIndex])
	{
		SET_ORIGIN(pPlayer->edict(), m_rgSpawnSpot[nIndex]->pev->origin);

		pPlayer->pev->angles = m_rgSpawnSpot[nIndex]->pev->angles;
		pPlayer->pev->v_angle = m_rgSpawnSpot[nIndex]->pev->angles;
		pPlayer->pev->fixangle = TRUE;
	}
	else
	{
		// We want to find a spot ...

		std::vector<CBaseEntity*> randList;

		const char *pClassname = (pPlayer->m_iTeam == TEAM_GR) ? "info_player_start" : "info_player_deathmatch";

		CBaseEntity *pEntity = NULL;

		while ((pEntity = UTIL_FindEntityByClassname(pEntity, pClassname)) != NULL)
		{
			BOOL bValid = TRUE;

			// Check already used
			for (int i = 0; i < 32; i++)
			{
				if (m_rgSpawnSpot[i] == pEntity)
				{
					bValid = FALSE;
					break;
				}
			}

			// Found a valid, push to list
			if (bValid)
			{
				randList.push_back(pEntity);
			}
		}

		// Some spot found
		if (!randList.empty())
		{
			int iRand = RANDOM_LONG(0, randList.size()-1);

			m_rgSpawnSpot[nIndex] = randList[iRand];

			// Move to Spawn spot

			SET_ORIGIN(pPlayer->edict(), m_rgSpawnSpot[nIndex]->pev->origin);

			pPlayer->pev->angles = m_rgSpawnSpot[nIndex]->pev->angles;
			pPlayer->pev->v_angle = m_rgSpawnSpot[nIndex]->pev->angles;
			pPlayer->pev->fixangle = TRUE;
		}
	}

	// Increase the spawn counter
	pPlayer->m_iNumSpawns++;

	// Set godmode
	pPlayer->SetGodMode(TRUE, 3.0);

	// Display 'GODTIME' gauge
	MESSAGE_BEGIN(MSG_ONE, gmsgGauge, NULL, pPlayer->edict());
	WRITE_BYTE(0);	// Type 0 is the godtime gauge
	WRITE_BYTE(3);
	MESSAGE_END();

	// Check buyzone now
	pPlayer->HandleSignals();

	// Recovery weapon
	pPlayer->BagRebuy();
	pPlayer->RetireWeapon();

	// Remove dead flag
	pPlayer->m_iScoreAttrib &= ~SCOREATTRIB_DEAD;

	// Tell all clients, You are alive !
	MESSAGE_BEGIN(MSG_ALL, gmsgScoreAttrib);
	WRITE_BYTE(pPlayer->GetId());
	WRITE_BYTE(pPlayer->m_iScoreAttrib);
	MESSAGE_END();

	// Reset MultiKill effect
	pPlayer->m_flLastKillTime = 0;
	pPlayer->m_iMultiKillCount = 1;
}

//------------------------------------------------------------
// Purpose : A player has been die, update the scoreborad and
//  make the killer effect
//------------------------------------------------------------
void CGameRule_TD::PlayerKill(CBasePlayer *pPlayer)
{
	// SetUp Respawn
//	pPlayer->SetRespawn(1.8 + m_iSpawnTime);

	// Increase the dead counter
	pPlayer->m_iDeads++;

	// You die
	pPlayer->m_iScoreAttrib |= SCOREATTRIB_DEAD;

	// Tell all clients this player's new attrib
	MESSAGE_BEGIN(MSG_ALL, gmsgScoreAttrib);
	WRITE_BYTE(pPlayer->GetId());
	WRITE_BYTE(pPlayer->m_iScoreAttrib);
	MESSAGE_END();

	// Tell all clients this player's new score
	MESSAGE_BEGIN(MSG_ALL, gmsgScoreInfo);
	WRITE_BYTE(pPlayer->GetId());
	WRITE_SHORT(pPlayer->m_iKills);
	WRITE_SHORT(pPlayer->m_iDeads);
	MESSAGE_END();

	// SetUp message's params
	int iKillerID, iVictimID, iWeaponID, iMultiKill, iFlags;

	// SetUp 'MULTIKILL' effect
	iMultiKill = 0;

	// SetUp 'FLAGS'
	iFlags = 0;

	// Maby killed by world, fall and die etc...
	if (pPlayer->m_pAttacker->IsPlayer())
	{
		// If killer is a player
		CBasePlayer *pKiller = dynamic_cast<CBasePlayer*>(pPlayer->m_pAttacker);

		// SetUp 'KILLER'
		iKillerID = pKiller->GetId();

		if (FClassnameIs(pPlayer->m_pInflictor->pev, "grenade_hegrenade"))
		{
			// If killed by grenade, get 'WEAPON' from grenade
			iWeaponID = dynamic_cast<CWorldHEGrenade*>(pPlayer->m_pInflictor)->GetWeaponID();
		}
		else
		{
			// Get 'WEAPON' from killer current active weapon
			iWeaponID = pKiller->GetActiveWeaponId();
		}

		// Don't allow team killer
		if (pPlayer->m_iTeam != pKiller->m_iTeam)
		{
			if (!m_bGameEnding)
			{
				// Increase the counter
				if (pKiller->m_iTeam == TEAM_GR)
					m_iKillCountGR++;
				else
					m_iKillCountBL++;

				// Update the global scoreboard
				MESSAGE_BEGIN(MSG_ALL, gmsgTeamScore);
				WRITE_SHORT(m_iTargetKill);
				WRITE_SHORT(m_iKillCountGR);
				WRITE_SHORT(m_iKillCountBL);
				MESSAGE_END();

				pKiller->m_iKills++;

				// Tell all clients the killer's new score
				MESSAGE_BEGIN(MSG_ALL, gmsgScoreInfo);
				WRITE_BYTE(pKiller->GetId());
				WRITE_SHORT(pKiller->m_iKills);
				WRITE_SHORT(pKiller->m_iDeads);
				MESSAGE_END();

				// Flag to display killer effect
				iFlags |= KILLFLAG_SHOWMARK;

				// Check score for team
				if (m_iRuleType == SUBRULETYPE_KILL)
				{
					// GR team is the Winner
					if (m_iKillCountGR == m_iTargetKill)
					{
						m_iWinStatus = WINSTATUS_GR;

						MESSAGE_BEGIN(MSG_ALL, gmsgWinStatus);
						WRITE_BYTE(WINSTATUS_GR);
						MESSAGE_END();

						m_bGameEnding = TRUE;
						m_flGameEndTime = gpGlobals->time;
					}

					// BL team is the Winner
					else if (m_iKillCountBL == m_iTargetKill)
					{
						m_iWinStatus = WINSTATUS_BL;

						MESSAGE_BEGIN(MSG_ALL, gmsgWinStatus);
						WRITE_BYTE(WINSTATUS_BL);
						MESSAGE_END();

						m_bGameEnding = TRUE;
						m_flGameEndTime = gpGlobals->time;
					}
				}
			}

			// Increase or Reset the 'MULTIKILL' counter
			if (gpGlobals->time - pKiller->m_flLastKillTime <= 5)
			{
				pKiller->m_iMultiKillCount++;
			}
			else
			{
				pKiller->m_iMultiKillCount = 1;
			}

			// Save for this time
			pKiller->m_flLastKillTime = gpGlobals->time;

			// SetUp 'MULTIKILL' effect
			iMultiKill = pKiller->m_iMultiKillCount;

			// If kill the revenge target...
			if (pKiller->m_iRevengeTarget == pPlayer->GetId())
			{
				iFlags |= KILLFLAG_REVENGE;
				pKiller->m_iRevengeTarget = 0;
			}

			// Save revenge target for victim
			pPlayer->m_iRevengeTarget = pKiller->GetId();
		}
	}
	else
	{
		// Default set it
		iKillerID = 0;
	}

	// Remake the 'ACE'
	MakeAce();

	// SetUp 'VICTIM'
	iVictimID = pPlayer->GetId();

	// Kill with 'HEADSHOT'
	if (pPlayer->m_LastHitGroup == HITGROUP_HEAD)
	{
		iFlags |= KILLFLAG_HEADSHOT;
	}

	// Kill with 'KNIFE'
	if (pPlayer->m_bitsDamageType == DMG_KNIFE)
	{
		iFlags |= KILLFLAG_KNIFE;
	}

	// Broadcast to all player's client !

	MESSAGE_BEGIN(MSG_ALL, gmsgKillMsg);
	WRITE_BYTE(iKillerID);
	WRITE_BYTE(iVictimID);
	WRITE_SHORT(iWeaponID);
	WRITE_BYTE(iMultiKill);
	WRITE_SHORT(iFlags);
	MESSAGE_END();
}

//------------------------------------------------------------
// Purpose : 
//------------------------------------------------------------
static int CompareAce(const void *_Arg1, const void *_Arg2)
{
	CBasePlayer *p1, *p2;

	p1 = *(CBasePlayer **)_Arg1;
	p2 = *(CBasePlayer **)_Arg2;

	if (p1 && !p2)
		return -1;
	else if (!p1 && p2)
		return 1;
	else if (!p1 && !p2)
		return 0;

	if (p1->m_iKills > p2->m_iKills)
		return -1;
	else if (p1->m_iKills == p2->m_iKills)
	{
		if (p1->m_iDeads < p2->m_iDeads)
			return -1;
		else if (p1->m_iDeads > p2->m_iDeads)
			return 1;
	}
	else// if (p1->m_iKills < p2->m_iKills)
		return 1;

	return 0;
}

//------------------------------------------------------------
// Purpose : 
//------------------------------------------------------------
void CGameRule_TD::MakeAce(void)
{
	CBasePlayer *pList[32] = { NULL };

	// Fill player list
	for (int i = 0; i < gpGlobals->maxClients; i++)
	{
		CBasePlayer *pl = UTIL_PlayerByIndex(i + 1);

		// Just joined player...
		if (pl && pl->m_iJoiningState == JOINED && !pl->m_bIsObserver)
		{
			pList[i] = pl;
		}
	}

	// Sort all players
	qsort(pList, 32, sizeof(CBasePlayer*), &CompareAce);

	// The ACE player
	CBasePlayer *pAce = NULL;

	// Process all players
	for (int i = 0; i < gpGlobals->maxClients; i++)
	{
		CBasePlayer *pl = pList[i];

		// Empty slot moved to tail of list
		if (!pl)
			continue;

		// The first will be the ACE
		if (i == 0)
		{
			if (pl->m_iKills > 0)
			{
				// Save ACE player for make Gray
				pAce = pl;
				pAce->SetACE(ACETYPE_GOLD);
			}
		}
		else
		{
			// Make the Gray ACE
			if (pAce && pAce->m_iKills >= 10 && pAce->m_iKills - pl->m_iKills <= 3)
				pl->SetACE(ACETYPE_GRAY);
			else
				pl->SetACE(ACETYPE_NONE);
		}
	}
}