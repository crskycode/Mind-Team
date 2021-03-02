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
	GameRulesNo,			/* û�й��� */
	GameRulesDeathmatch,	/* ����ģʽ */
	GameRulesTDTime,		/* �ŶӾ�����ʱ�䣩 */
	GameRulesTDKill,		/* �ŶӾ�����ɱ������ */
	GameRulesBomb,			/* ����ģʽ */
	GameRulesGhostBomb,		/* ����ģʽ */
};

enum RoundWinStatus
{
	RoundWinStatusNo,		/* û������ʤ���Ķ��� */
	RoundWinStatusDraw,		/* ƽ�� */
	RoundWinStatusBL,		/* BlackListʤ�� */
	RoundWinStatusGR,		/* GlobalRiskʤ�� */
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
#define KILLFLAG_KNIFE				(1<<6)	// ����, �̵�
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

	// ����������
	int m_iMaxPlayers;
	// ���۲�������
	int m_iMaxSpectators;

	// �������ͣ�ʱ�䣬ɱ������
	int m_iRoundType;

	// ��ǰBL�����
	int m_iNumBL;
	// ��ǰGR�����
	int m_iNumGR;

	// �غ�ʱ�䳤��
	int m_iRoundTime;
	// ɱ����
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