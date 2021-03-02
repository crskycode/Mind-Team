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

#ifndef GAMERULES_H
#define GAMERULES_H

class CBasePlayerWeapon;
class CBasePlayer;

class CGameRules
{
public:
	virtual void ReadMultiplayCvars(void) = 0;
	virtual void ServerActivate(void) = 0;
	virtual void ServerDeactivate(void) = 0;
	virtual BOOL PlayerConnected(CBasePlayer *pPlayer) = 0;
	virtual void PlayerDisconnected(CBasePlayer *pPlayer) = 0;
	virtual void ResetSpawnSpot(void) = 0;
	virtual void SelectSpawnSpot(CBasePlayer *pPlayer) = 0;
	virtual void FreeSpawnSpot(CBasePlayer *pPlayer) = 0;
	virtual void PlayerJoined(CBasePlayer *pPlayer) = 0;
	virtual void InitializePlayerCounts(void) = 0;
	virtual void Think(void) = 0;
	virtual void InitHUD(CBasePlayer *pl) = 0;
	virtual BOOL TeamCanJoin(int iTeam) = 0;
	virtual BOOL PlayerCanSpawn(CBasePlayer *pPlayer) = 0;
	virtual BOOL FPlayerCanTakeDamage(CBasePlayer *pPlayer, CBaseEntity *pAttacker) = 0;
	virtual void PlayerSpawn(CBasePlayer *pPlayer) = 0;
	virtual void PlayerKilled(CBasePlayer *pVictim, entvars_t *pevKiller, entvars_t *pevInflictor) = 0;
	virtual void KillMessage(CBasePlayer *pVictim, entvars_t *pevKiller, entvars_t *pevInflictor) = 0;
	virtual void CleanUpMap(void) = 0;
};

enum GameRulesType
{
	GameRulesNo,			/* 没有规则 */
	GameRulesDeathmatch,	/* 歼灭模式 */
	GameRulesTDTime,		/* 团队竞技（时间） */
	GameRulesTDKill,		/* 团队竞技（杀敌数） */
	GameRulesBomb,			/* 爆破模式 */
	GameRulesGhostBomb,		/* 幽灵模式 */
};

enum RoundWinStatus
{
	RoundWinStatusNo,		/* 没有设置胜利的队伍 */
	RoundWinStatusDraw,		/* 平局 */
	RoundWinStatusBL,		/* BlackList胜利 */
	RoundWinStatusGR,		/* GlobalRisk胜利 */
};

#define SCOREATTRIB_DEAD	(1<<0)
#define SCOREATTRIB_C4		(1<<1)

#define ROUNDEND_DRAW	0
#define ROUNDEND_WIN	1
#define ROUNDEND_LOSE	2

#define KILLFLAG_FIRSTKILL			(1<<0)
#define KILLFLAG_LASTKILL			(1<<1)
#define KILLFLAG_WALLSHOT			(1<<2)
#define KILLFLAG_HEADSHOT			(1<<3)
#define KILLFLAG_HEADSHOT_GOLD		(1<<5)
#define KILLFLAG_KNIFE				(1<<6)	// 刀子, 刺刀
#define KILLFLAG_GRENADE			(1<<7)
#define KILLFLAG_REVENGE			(1<<8)

#define ACETYPE_NONE	0
#define ACETYPE_GRAY	1
#define ACETYPE_GOLD	2

class CGameRulesTD : public CGameRules
{
public:
	CGameRulesTD();

	virtual void ReadMultiplayCvars(void);
	virtual void ServerActivate(void);
	virtual void ServerDeactivate(void);
	virtual BOOL PlayerConnected(CBasePlayer *pPlayer);
	virtual void PlayerDisconnected(CBasePlayer *pPlayer);
	virtual void ResetSpawnSpot(void);
	virtual void SelectSpawnSpot(CBasePlayer *pPlayer);
	virtual void FreeSpawnSpot(CBasePlayer *pPlayer);
	virtual void PlayerJoined(CBasePlayer *pPlayer);
	virtual void InitializePlayerCounts(void);
	virtual void Think(void);
	virtual void InitHUD(CBasePlayer *pl);
	virtual BOOL TeamCanJoin(int iTeam);
	virtual BOOL PlayerCanSpawn(CBasePlayer *pPlayer);
	virtual BOOL FPlayerCanTakeDamage(CBasePlayer *pPlayer, CBaseEntity *pAttacker);
	virtual void PlayerSpawn(CBasePlayer *pPlayer);
	virtual void PlayerKilled(CBasePlayer *pVictim, entvars_t *pevKiller, entvars_t *pevInflictor);
	virtual void KillMessage(CBasePlayer *pVictim, entvars_t *pevKiller, entvars_t *pevInflictor);
	virtual void CleanUpMap(void);

	virtual void RestartRound(void);
	virtual void SetMovementEnable(BOOL bEnable);
	virtual void NeededPlayersCheck(void);
	virtual void TerminateRound(float tmDelay, int iWinStatus);
	virtual void CheckFreezePeriodExpired(void);
	virtual void CheckRoundTimeExpired(void);
	virtual void CheckRoundEnd(void);
	virtual void CheckDisconnect(void);
	virtual float TimeRemaining(void);
	virtual BOOL IsFreezePeriod(void);
	virtual BOOL HasRoundTimeExpired(void);
	virtual void RankingForACE(void);

public:
	// Player spawn point
	CBaseEntity *m_rgSpawnSpot[32];

	// 最多玩家数量
	int m_iMaxPlayers;
	// 最多观察者数量
	int m_iMaxSpectators;

	// 规则类型（时间，杀敌数）
	int m_iRoundType;

	// 当前BL玩家数
	int m_iNumBL;
	// 当前GR玩家数
	int m_iNumGR;

	// 回合时间长度
	int m_iRoundTime;
	// 杀敌数
	int m_iRoundKill;

	BOOL m_bGameOver;
	float m_flNextRound;
	float m_flRoundStartTime;
	BOOL m_bFreezePeriod;
	int m_iScoreBL;
	int m_iScoreGR;
	BOOL m_bRoundTerminating;
	int m_iRoundWinStatus;

	// Multi Kill Manager
	float m_rgPlayerLastKillTime[32];
	int m_rgPlayerMultiKillNumber[32];
	int m_rgPlayerRevengeTarget[32];
};

CGameRules *InstallGameRules(void);

extern  CGameRules *g_pGameRules;

#endif