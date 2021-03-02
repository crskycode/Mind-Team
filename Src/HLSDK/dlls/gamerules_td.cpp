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
#include "message.h"
#include "weapons.h"
#include "gamerules.h"

//----------------------------------------------------------------
// CVARS BEGIN
//----------------------------------------------------------------

static cvar_t mp_maxplayers = { "mp_maxplayers", "16", FCVAR_SERVER };
static cvar_t mp_maxspectators = { "mp_maxspectators", "0", FCVAR_SERVER };
static cvar_t td_round_type = { "td_round_type", "0", FCVAR_SERVER };
static cvar_t td_round_kill = { "td_round_kill", "40", FCVAR_SERVER };
static cvar_t td_round_time = { "td_round_time", "8", FCVAR_SERVER };	//minute

static cvar_t mp_trimround = { "mp_trimround", "0", FCVAR_SERVER };

//----------------------------------------------------------------
// CVARS END
//----------------------------------------------------------------

CGameRulesTD::CGameRulesTD()
{
	// Zeroize this instance
	memset((byte *)this + 4, 0, sizeof(CGameRulesTD) - 4);

	CVAR_REGISTER(&mp_maxplayers);
	CVAR_REGISTER(&mp_maxspectators);
	CVAR_REGISTER(&td_round_type);
	CVAR_REGISTER(&td_round_kill);
	CVAR_REGISTER(&td_round_time);
	CVAR_REGISTER(&mp_trimround);

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
}

void CGameRulesTD::ReadMultiplayCvars(void)
{
	m_iMaxPlayers = CVAR_GET_FLOAT("mp_maxplayers");
	m_iMaxSpectators = CVAR_GET_FLOAT("mp_maxspectators");
	m_iRoundType = CVAR_GET_FLOAT("td_round_type");
	m_iRoundTime = CVAR_GET_FLOAT("td_round_kill") * 60; //minute
	m_iRoundKill = CVAR_GET_FLOAT("td_round_time");

	if (m_iMaxPlayers < 2)
	{
		CVAR_SET_FLOAT("mp_maxplayers", 2);
		m_iMaxPlayers = 2;
	}
	else if (m_iMaxPlayers > 16)
	{
		CVAR_SET_FLOAT("mp_maxplayers", 16);
		m_iMaxPlayers = 16;
	}

	if (m_iMaxPlayers % 2 != 0)
	{
		m_iMaxPlayers++;	//align to 2
		CVAR_SET_FLOAT("mp_maxplayers", m_iMaxPlayers);
	}

	if (m_iMaxSpectators < 0)
	{
		CVAR_SET_FLOAT("mp_maxspectators", 0);
		m_iMaxSpectators = 0;
	}
	else if (m_iMaxSpectators > 14)
	{
		CVAR_SET_FLOAT("mp_maxspectators", 14);
		m_iMaxSpectators = 14;
	}

	if (m_iMaxSpectators % 2 != 0)
	{
		m_iMaxSpectators++;	//align to 2
		CVAR_SET_FLOAT("mp_maxspectators", m_iMaxSpectators);
	}

	if (m_iMaxPlayers + m_iMaxSpectators > 16)
	{
		m_iMaxSpectators = 16 - m_iMaxPlayers;
		CVAR_SET_FLOAT("mp_maxspectators", m_iMaxSpectators);
	}

	if (m_iRoundTime < 480)
	{
		CVAR_SET_FLOAT("td_round_time", 8);	//minute
		m_iRoundTime = 480;
	}
	else if (m_iRoundTime > 720)
	{
		CVAR_SET_FLOAT("td_round_time", 12);	//minute
		m_iRoundTime = 720;
	}

	if (m_iRoundKill < 40)
	{
		CVAR_SET_FLOAT("td_round_kill", 40);
		m_iRoundKill = 40;
	}
	else if (m_iRoundKill > 150)
	{
		CVAR_SET_FLOAT("td_round_kill", 150);
		m_iRoundKill = 150;
	}

	if (m_iRoundType < 0)
	{
		CVAR_SET_FLOAT("td_round_type", 0);
		m_iRoundType = 0;
	}
	else if (m_iRoundType > 1)
	{
		CVAR_SET_FLOAT("td_round_type", 1);
		m_iRoundType = 1;
	}
}

void CGameRulesTD::ServerActivate(void)
{
	ReadMultiplayCvars();

	int iSpotGR = UTIL_EntityCountByClassname("info_player_gr") + UTIL_EntityCountByClassname("info_player_start");
	int iSpotBL = UTIL_EntityCountByClassname("info_player_bl") + UTIL_EntityCountByClassname("info_player_deathmatch");

	if (iSpotGR < 8 || iSpotBL < 8)
	{
		Sys_Error("The world(BSP) not enough spawn spot");
		return;
	}

	m_bGameOver = FALSE;

	m_flNextRound = 0;
	m_flRoundStartTime = gpGlobals->time;
	m_bFreezePeriod = TRUE;

	m_bRoundTerminating = FALSE;
	m_iRoundWinStatus = RoundWinStatusNo;

	m_iScoreGR = 0;
	m_iScoreBL = 0;

	ResetSpawnSpot();
}

void CGameRulesTD::ServerDeactivate(void)
{
}

BOOL CGameRulesTD::PlayerConnected(CBasePlayer *pPlayer)
{
	return TRUE;
}

void CGameRulesTD::PlayerDisconnected(CBasePlayer *pPlayer)
{
	InitializePlayerCounts();
	CheckDisconnect();
	RankingForACE();
	FreeSpawnSpot(pPlayer);
}

void CGameRulesTD::ResetSpawnSpot(void)
{
	for (int i = 0; i < 32; i++)
	{
		m_rgSpawnSpot[i] = NULL;
	}
}

void CGameRulesTD::SelectSpawnSpot(CBasePlayer *pPlayer)
{
	int Id = pPlayer->GetId() - 1;

	if (m_rgSpawnSpot[Id])
	{
		SET_ORIGIN(pPlayer->edict(), m_rgSpawnSpot[Id]->pev->origin);

		pPlayer->pev->angles = m_rgSpawnSpot[Id]->pev->angles;
		pPlayer->pev->v_angle = m_rgSpawnSpot[Id]->pev->angles;
		pPlayer->pev->fixangle = TRUE;

		return;
	}
	else
	{
		char *szName;
		std::vector<CBaseEntity *> pList;
		CBaseEntity *pSpot = NULL;

		if (pPlayer->m_iTeam == TEAM_GR)
			szName = "info_player_gr";
		else
			szName = "info_player_bl";

retry:
		while ((pSpot = UTIL_FindEntityByClassname(pSpot, szName)) != NULL)
		{
			BOOL bSelect = TRUE;

			for (int i = 0; i < 32; i++)
			{
				if (m_rgSpawnSpot[i] == pSpot)
				{
					bSelect = FALSE;
					break;
				}
			}

			if (bSelect)
			{
				pList.push_back(pSpot);
			}
		}

		if (pList.size())
		{
			pSpot = pList[RANDOM_LONG(0, pList.size() - 1)];

			m_rgSpawnSpot[Id] = pSpot;
			
			SET_ORIGIN(pPlayer->edict(), pSpot->pev->origin);
			
			pPlayer->pev->angles = pSpot->pev->angles;
			pPlayer->pev->v_angle = pSpot->pev->angles;
			pPlayer->pev->fixangle = TRUE;

			return;
		}

		if (pPlayer->m_iTeam == TEAM_GR)
			szName = "info_player_start";
		else
			szName = "info_player_deathmatch";

		goto retry;
	}
}

void CGameRulesTD::FreeSpawnSpot(CBasePlayer *pPlayer)
{
	m_rgSpawnSpot[pPlayer->GetId() - 1] = NULL;
}

void CGameRulesTD::PlayerJoined(CBasePlayer *pPlayer)
{
	InitializePlayerCounts();

	MESSAGE_BEGIN(MSG_ONE, gmsgGameTip, NULL, pPlayer->edict());
	WRITE_STRING("是 [团队竞技], [普通赛]");
	WRITE_STRING("可以使用所有武器");
	MESSAGE_END();
}

void CGameRulesTD::InitializePlayerCounts(void)
{
	CBaseEntity *pEntity = NULL;

	m_iNumBL = 0;
	m_iNumGR = 0;

	while ((pEntity = UTIL_FindEntityByClassname(pEntity, "player")) != NULL)
	{
		CBasePlayer *pPlayer = (CBasePlayer *)pEntity;

		if (pPlayer->m_iJoiningState != JOINED)
			continue;

		switch (pPlayer->m_iTeam)
		{
			case TEAM_BL:
			{
				m_iNumBL++;
				break;
			}
			case TEAM_GR:
			{
				m_iNumGR++;
				break;
			}
		}
	}
}

void CGameRulesTD::Think(void)
{
	if (m_bRoundTerminating && gpGlobals->time > m_flNextRound)
	{
		if (!m_bGameOver)
		{
			RestartRound();
		}
		else
		{
			// all client should exit from server
			ALERT(at_console, "[SV] Game Over\n");
			m_bRoundTerminating = FALSE;
		}
	}

	if (m_bGameOver)
	{
		return;
	}

	if (IsFreezePeriod())
		CheckFreezePeriodExpired();
	else
		CheckRoundTimeExpired();

	/*if (CVAR_GET_FLOAT("mp_trimround") > 0)
	{
		TerminateRound(1, RoundWinStatusDraw);
		CVAR_SET_FLOAT("mp_trimround", 0);
	}*/
}

void CGameRulesTD::InitHUD(CBasePlayer *pPlayer)
{
	const char *name = STRING(pPlayer->pev->netname);

	if (name && *name)
	{
		ClientPrintAll(HUD_PRINTCONSOLE, "%s has joined the game\n", name);
	}

	int iGameMode = (m_iRoundType == 0) ? GameRulesTDKill : GameRulesTDTime;

	MESSAGE_BEGIN(MSG_ONE, gmsgGameMode, NULL, pPlayer->edict());
	WRITE_BYTE(iGameMode);
	MESSAGE_END();

	MESSAGE_BEGIN(MSG_ONE, gmsgRoundTime, NULL, pPlayer->edict());
	WRITE_SHORT(TimeRemaining());
	MESSAGE_END();

	MESSAGE_BEGIN(MSG_ALL, gmsgTeamScore);
	WRITE_SHORT(m_iRoundKill);	// ROUND TARGET
	WRITE_SHORT(m_iScoreBL);
	WRITE_SHORT(m_iScoreGR);
	MESSAGE_END();

	for (int i = 1; i <= gpGlobals->maxClients; i++)
	{
		CBasePlayer *pl = (CBasePlayer *)UTIL_PlayerByIndex(i);

		if (!pl)
			continue;

		MESSAGE_BEGIN(MSG_ONE, gmsgClassInfo, NULL, pPlayer->edict());
		WRITE_BYTE(pl->entindex());
		WRITE_BYTE(pl->m_iClass);
		MESSAGE_END();

		MESSAGE_BEGIN(MSG_ONE, gmsgTeamInfo, NULL, pPlayer->edict());
		WRITE_BYTE(pl->entindex());
		WRITE_BYTE(pl->m_iTeam);
		MESSAGE_END();

		MESSAGE_BEGIN(MSG_ONE, gmsgScoreAttrib, NULL, pPlayer->edict());
		WRITE_BYTE(pl->entindex());
		WRITE_BYTE(pl->m_iScoreAttrib);
		MESSAGE_END();

		MESSAGE_BEGIN(MSG_ONE, gmsgScoreInfo, NULL, pPlayer->edict());
		WRITE_BYTE(pl->entindex());
		WRITE_SHORT(pl->m_iKills);
		WRITE_SHORT(pl->m_iDeads);
		MESSAGE_END();
	}
}

BOOL CGameRulesTD::TeamCanJoin(int iTeam)
{
	if (m_bGameOver)
	{
		return FALSE;
	}

	if (m_iNumBL + m_iNumGR == m_iMaxPlayers)
	{
		return FALSE;
	}

	switch (iTeam)
	{
		case TEAM_BL:
		{
			if (m_iNumBL == m_iMaxPlayers / 2)
			{
				return FALSE;
			}

			return TRUE;
		}
		case TEAM_GR:
		{
			if (m_iNumGR == m_iMaxPlayers / 2)
			{
				return FALSE;
			}

			return TRUE;
		}
	}

	return FALSE;
}

BOOL CGameRulesTD::PlayerCanSpawn(CBasePlayer *pPlayer)
{
	if (HasRoundTimeExpired())
		return FALSE;

	return TRUE;
}


BOOL CGameRulesTD::FPlayerCanTakeDamage(CBasePlayer *pPlayer, CBaseEntity *pAttacker)
{
	if (!HasRoundTimeExpired())
		return TRUE;

	if (!pAttacker->IsPlayer())
		return TRUE;

	CBasePlayer *pl = (CBasePlayer *)pAttacker;

	if (pl->m_pActiveWeapon)
	{
		if (pl->m_pActiveWeapon->m_Info.iTargetSlot == WPNSLOT_KNIFE)
			return TRUE;
	}

	return FALSE;
}

void CGameRulesTD::PlayerSpawn(CBasePlayer *pPlayer)
{
	pPlayer->SetGodMode(TRUE, 3.0);

	MESSAGE_BEGIN(MSG_ONE, gmsgGauge, NULL, pPlayer->edict());
	WRITE_BYTE(0);
	WRITE_BYTE(3);
	MESSAGE_END();

	pPlayer->HandleSignals();
	pPlayer->BagRebuy();
	pPlayer->RetireWeapon();

	pPlayer->m_iScoreAttrib &= ~SCOREATTRIB_DEAD;

	MESSAGE_BEGIN(MSG_ALL, gmsgScoreAttrib);
	WRITE_BYTE(pPlayer->entindex());
	WRITE_BYTE(pPlayer->m_iScoreAttrib);
	MESSAGE_END();

	pPlayer->m_flLastKillTime = 0;
	pPlayer->m_iMultiKillCount = 1;
	//pPlayer->m_iRevengeTarget = 0;
}

void CGameRulesTD::PlayerKilled(CBasePlayer *pVictim, entvars_t *pevKiller, entvars_t *pevInflictor)
{
	CBaseEntity *pEntity = CBaseEntity::Instance(pevKiller);

	if (pEntity && pEntity->IsPlayer())
	{
		CBasePlayer *pKiller = (CBasePlayer *)pEntity;

		if (pKiller != pVictim && pKiller->m_iTeam != pVictim->m_iTeam)
		{
			pKiller->m_iKills++;

			MESSAGE_BEGIN(MSG_ALL, gmsgScoreInfo);
			WRITE_BYTE(pKiller->GetId());
			WRITE_SHORT(pKiller->m_iKills);
			WRITE_SHORT(pKiller->m_iDeads);
			MESSAGE_END();

			MESSAGE_BEGIN(MSG_ONE, gmsgKillerIcon, NULL, pVictim->edict());
			WRITE_BYTE(pKiller->GetId());
			MESSAGE_END();

			switch (pKiller->m_iTeam)
			{
				case TEAM_GR: m_iScoreGR++; break;
				case TEAM_BL: m_iScoreBL++; break;
			}
		}
	}

	pVictim->m_iDeads++;

	MESSAGE_BEGIN(MSG_ALL, gmsgScoreInfo);
	WRITE_BYTE(pVictim->GetId());
	WRITE_SHORT(pVictim->m_iKills);
	WRITE_SHORT(pVictim->m_iDeads);
	MESSAGE_END();

	MESSAGE_BEGIN(MSG_ALL, gmsgTeamScore);
	WRITE_SHORT(m_iRoundKill);
	WRITE_SHORT(m_iScoreBL);
	WRITE_SHORT(m_iScoreGR);
	MESSAGE_END();

	pVictim->m_iScoreAttrib |= SCOREATTRIB_DEAD;

	MESSAGE_BEGIN(MSG_ALL, gmsgScoreAttrib);
	WRITE_BYTE(pVictim->GetId());
	WRITE_BYTE(pVictim->m_iScoreAttrib);
	MESSAGE_END();

	KillMessage(pVictim, pevKiller, pevInflictor);

	RankingForACE();

	pVictim->SetRespawn(1.8);
}

void CGameRulesTD::KillMessage(CBasePlayer *pVictim, entvars_t *pevKiller, entvars_t *pevInflictor)
{
	CBaseEntity *pEntity = CBaseEntity::Instance(pevKiller);

	int iVictimId = pVictim->GetId();
	int iKillerId = 0;
	int iWeaponId = 0;

	// maby it's worldspawn
	if (pEntity->IsPlayer())
	{
		CBasePlayer *pKiller = dynamic_cast<CBasePlayer *>(pEntity);

		iKillerId = pKiller->GetId();
		iWeaponId = pKiller->GetActiveWeaponId();
	}

	// killed by grenade ?
	if (FClassnameIs(pevInflictor, "grenade_hegrenade"))
	{
		CWorldHEGrenade *pGrenade = dynamic_cast<CWorldHEGrenade *>(CBaseEntity::Instance(pevInflictor));

		if (pGrenade)
		{
			iKillerId = ENTINDEX(pGrenade->pev->owner);
			iWeaponId = pGrenade->GetWeaponID();
		}
	}

	int iMultiKill = 0;
	int iFlags = 0;

	if (pVictim->m_bitsDamageType == DMG_KNIFE)
	{
		iFlags |= KILLFLAG_KNIFE;
	}

	if (pVictim->m_LastHitGroup == HITGROUP_HEAD)
	{
		iFlags |= KILLFLAG_HEADSHOT;
	}

	if (pEntity->IsPlayer())
	{
		CBasePlayer *pKiller = dynamic_cast<CBasePlayer *>(pEntity);

		if (gpGlobals->time - pKiller->m_flLastKillTime < 5)
		{
			pKiller->m_iMultiKillCount++;
			pKiller->m_flLastKillTime = gpGlobals->time;
		}
		else
		{
			pKiller->m_iMultiKillCount = 1;
			pKiller->m_flLastKillTime = gpGlobals->time;
		}

		if (pKiller->m_iMultiKillCount > 255)
		{
			pKiller->m_iMultiKillCount = 1;
		}

		pVictim->m_iRevengeTarget = iKillerId;

		if (pKiller->m_iRevengeTarget == iVictimId)
		{
			pKiller->m_iRevengeTarget = 0;
			iFlags |= KILLFLAG_REVENGE;
		}

		iMultiKill = pKiller->m_iMultiKillCount;
	}

	MESSAGE_BEGIN(MSG_ALL, gmsgKillMsg);
	WRITE_BYTE(iKillerId);
	WRITE_BYTE(iVictimId);
	WRITE_SHORT(iWeaponId);
	WRITE_BYTE(iMultiKill);	// MultiKill
	WRITE_SHORT(iFlags);	//FLAGS
	MESSAGE_END();
}

//-----------------------------------------------------------------------------
// Purpose : Remove all WeaponBox and Grenade etc.
//-----------------------------------------------------------------------------
void CGameRulesTD::CleanUpMap(void)
{
	CBaseEntity *pEntity;

	pEntity = NULL;
	while ((pEntity = UTIL_FindEntityByClassname(pEntity, "weaponbox")) != NULL)
	{
		pEntity->pev->nextthink = gpGlobals->time + 0.1;
	}

	pEntity = NULL;
	while ((pEntity = UTIL_FindEntityByClassname(pEntity, "grenade_hegrenade")) != NULL)
	{
		pEntity->SetTouch(NULL);
		pEntity->SetThink(&CBaseEntity::SUB_Remove);
		pEntity->pev->nextthink = gpGlobals->time + 0.1;
	}

	pEntity = NULL;
	while ((pEntity = UTIL_FindEntityByClassname(pEntity, "grenade_flashbang")) != NULL)
	{
		pEntity->SetTouch(NULL);
		pEntity->SetThink(&CBaseEntity::SUB_Remove);
		pEntity->pev->nextthink = gpGlobals->time + 0.1;
	}

	pEntity = NULL;
	while ((pEntity = UTIL_FindEntityByClassname(pEntity, "grenade_smokegrenade")) != NULL)
	{
		pEntity->SetTouch(NULL);
		pEntity->SetThink(&CBaseEntity::SUB_Remove);
		pEntity->pev->nextthink = gpGlobals->time + 0.1;
	}
}

void CGameRulesTD::RestartRound(void)
{
	ReadMultiplayCvars();

	m_flNextRound = 0;
	m_flRoundStartTime = gpGlobals->time;

	m_bFreezePeriod = TRUE;
	m_iRoundWinStatus = RoundWinStatusNo;
	m_bRoundTerminating = FALSE;

	// tell client new round was started
	MESSAGE_BEGIN(MSG_ALL, gmsgResetRound);
	MESSAGE_END();

	// update client timer
	MESSAGE_BEGIN(MSG_ALL, gmsgRoundTime);
	WRITE_SHORT(TimeRemaining());
	MESSAGE_END();

	m_iScoreBL = 0;
	m_iScoreGR = 0;

	// update global scoreboard
	MESSAGE_BEGIN(MSG_ALL, gmsgTeamScore);
	WRITE_SHORT(m_iRoundKill);	// ROUND TARGET
	WRITE_SHORT(m_iScoreBL);
	WRITE_SHORT(m_iScoreGR);
	MESSAGE_END();

	ResetSpawnSpot();

	for (int i = 1; i <= gpGlobals->maxClients; i++)
	{
		CBasePlayer *pl = UTIL_PlayerByIndex(i);

		if (!pl)
			continue;

		if (pl->m_iJoiningState != JOINED)
			continue;

		if (pl->m_bIsObserver)
			continue;

		pl->m_iKills = 0;
		pl->m_iDeads = 0;

		pl->SetACE(ACETYPE_NONE);

		pl->Spawn();
	}

	CleanUpMap();

	for (int i = 0; i < gpGlobals->maxClients; i++)
	{
		m_rgPlayerLastKillTime[i] = 0;
		m_rgPlayerMultiKillNumber[i] = 1;
		m_rgPlayerRevengeTarget[i] = 0;
	}
}

//-----------------------------------------------------------------------------
// Purpose : If set to FALSE, all players can't move
//-----------------------------------------------------------------------------
void CGameRulesTD::SetMovementEnable(BOOL bEnable)
{
	for (int i = 1; i <= gpGlobals->maxClients; i++)
	{
		CBasePlayer *pl = UTIL_PlayerByIndex(i);

		if (!pl)
			continue;

		if (pl->m_iJoiningState != JOINED)
			continue;

		if (pl->m_bIsObserver)
			continue;

		if (!bEnable)
			g_engfuncs.pfnSetClientMaxspeed(pl->edict(), 1);
		else
			pl->ResetMaxSpeed();
	}
}

void CGameRulesTD::NeededPlayersCheck(void)
{
	/*if (m_iNumBL == 0 && m_iNumGR == 0)
	{
		m_bFirstConnected = FALSE;
	}

	if (!m_bFirstConnected && (m_iNumBL != 0 || m_iNumGR != 0))
	{
		TerminateRound(3, RoundWinStatusDraw);
		m_bFirstConnected = TRUE;
	}*/
}

void CGameRulesTD::TerminateRound(float tmDelay, int iWinStatus)
{
	m_iRoundWinStatus = iWinStatus;
	m_bRoundTerminating = TRUE;
	m_flNextRound = gpGlobals->time + tmDelay;

	m_bGameOver = TRUE;

// tell client the round was end
	MESSAGE_BEGIN(MSG_ALL, gmsgWinStatus);
	WRITE_BYTE(iWinStatus);
	MESSAGE_END();
}

void CGameRulesTD::CheckFreezePeriodExpired(void)
{
	if (!m_bFreezePeriod)
		return;

	if (gpGlobals->time - m_flRoundStartTime < 1)
		return;

	m_bFreezePeriod = FALSE;

	MESSAGE_BEGIN(MSG_BROADCAST, gmsgMsgBox);
	WRITE_STRING("消灭你看到的每个敌人。");
	MESSAGE_END();
}

void CGameRulesTD::CheckRoundTimeExpired(void)
{
	if (HasRoundTimeExpired() == FALSE)
		return;

	CheckRoundEnd();
}

void CGameRulesTD::CheckRoundEnd(void)
{
	if (m_iRoundType == 0)	//kill
	{
		if (m_iScoreGR >= m_iRoundKill)
		{
			TerminateRound(10, RoundWinStatusGR);//before new round start at 10sec
			return;
		}
		else if (m_iScoreBL >= m_iRoundKill)
		{
			TerminateRound(10, RoundWinStatusBL);
			return;
		}
		else
		{
			TerminateRound(10, RoundWinStatusDraw);
			return;
		}
	}
	else
	{
		if (m_iScoreGR > m_iScoreBL)
		{
			TerminateRound(10, RoundWinStatusGR);
			return;
		}
		else if (m_iScoreBL > m_iScoreGR)
		{
			TerminateRound(10, RoundWinStatusBL);
			return;
		}
		else
		{
			TerminateRound(10, RoundWinStatusDraw);
			return;
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose : When a player has been disconnected from server, check the team member.
//  If a team doesn't have any players, we need to end the game.
//-----------------------------------------------------------------------------
void CGameRulesTD::CheckDisconnect(void)
{
	if (m_iNumGR == 0 && m_iNumBL == 0)
	{
		m_bGameOver = TRUE;
		return;
	}

	if (m_iNumGR == 0)
	{
		m_bGameOver = TRUE;
		TerminateRound(5.0, RoundWinStatusBL);
	}
	else if (m_iNumBL == 0)
	{
		m_bGameOver = TRUE;
		TerminateRound(5.0, RoundWinStatusGR);
	}
}

//-----------------------------------------------------------------------------
// Purpose : Return the remaining round time, it will not be a negative number.
// Return : Second
//-----------------------------------------------------------------------------
float CGameRulesTD::TimeRemaining(void)
{
	float time = m_iRoundTime - (gpGlobals->time - m_flRoundStartTime);

	if (time < 0)
		time = 0;

	return time;
}

BOOL CGameRulesTD::IsFreezePeriod(void)
{
	return m_bFreezePeriod;
}

BOOL CGameRulesTD::HasRoundTimeExpired(void)
{
	if (TimeRemaining() > 0 || m_iRoundWinStatus != RoundWinStatusNo)
		return FALSE;

	return TRUE;
}

static int _RankingCompare(const void *_Arg1, const void *_Arg2)
{
	// Compare callback
	// returns -1 put p1 before p2
	// returns  1 put p1 after p2
	// returns  0 not change

	CBasePlayer *p1 = *(CBasePlayer **)_Arg1;
	CBasePlayer *p2 = *(CBasePlayer **)_Arg2;

	if (p1 != NULL && p2 == NULL)
	{
		// p1  1/1 ↑
		// p2  -/-
		return -1;
	}
	else if (p1 == NULL && p2 != NULL)
	{
		// p1  -/- ↓
		// p2  1/1
		return 1;
	}
	else if (p1 == NULL && p2 == NULL)
	{
		// p1  -/-
		// p2  -/-
		return 0;
	}

	if (p1->m_iKills > p2->m_iKills)
	{
		// p1  2/1 ↑
		// p2  1/1
		return -1;
	}
	else if (p1->m_iKills == p2->m_iKills)
	{
		if (p1->m_iDeads > p2->m_iDeads)
		{
			// p1  1/2 ↓
			// p2  1/1
			return 1;
		}
		else if (p1->m_iDeads < p2->m_iDeads)
		{
			// p1  1/1 ↑
			// p2  1/2
			return -1;
		}
	}
	else
	{
		// p1  1/1 ↓
		// p2  2/1
		return 1;
	}

	// p1  1/1
	// p2  1/1
	return 0;
}

void CGameRulesTD::RankingForACE(void)
{
	CBasePlayer *pList[32];

	// zeroize this array
	memset(pList, 0, sizeof(pList));

	// fill it
	for (int i = 0; i < gpGlobals->maxClients; i++)
	{
		pList[i] = UTIL_PlayerByIndex(i + 1);
	}

	// sort player by score
	qsort(pList, 32, sizeof(CBasePlayer *), &_RankingCompare);

	// choose players
	for (int i = 0; i < gpGlobals->maxClients; i++)
	{
		CBasePlayer *pl = pList[i];

		// empty slot, skip it
		if (!pl)
			continue;

		// the first one is gold ACE
		if (i == 0)
		{
			if (pl->m_iKills > 0)
			{
				pl->SetACE(ACETYPE_GOLD);
			}
		}
		else
		{
			CBasePlayer *ace = pList[0];

			// after gold ACE ..
			if (ace)
			{
				// choose the gray ACE
				if (ace->m_iKills >= 10 && ace->m_iKills - pl->m_iKills <= 3)
				{
					pl->SetACE(ACETYPE_GRAY);
					continue;
				}
			}

			// not is ACE
			pl->SetACE(ACETYPE_NONE);
		}
	}
}