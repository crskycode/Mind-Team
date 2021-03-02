#ifndef GAMERULE_H
#define GAMERULE_H

#define MAX_PLAYERS	16
#define MAX_TEAMPLAYERS	8

// Master playing team types
enum TeamType
{
	TEAM_NO,
	TEAM_GR,
	TEAM_BL,
};

// Send to client
#define GAMEMODE_TD	1
#define GAMEMODE_TM	2
#define GAMEMODE_BM	3
#define GAMEMODE_ES	4
#define GAMEMODE_GM	5

// Show in scoreboard
#define SCOREATTRIB_DEAD	(1<<0)
#define SCOREATTRIB_C4		(1<<1)

// Kill effect
#define KILLFLAG_SHOWMARK		(1<<0)
#define KILLFLAG_FIRSTKILL		(1<<1)
#define KILLFLAG_LASTKILL		(1<<2)
#define KILLFLAG_WALLSHOT		(1<<3)
#define KILLFLAG_HEADSHOT		(1<<4)
#define KILLFLAG_HEADSHOT_GOLD	(1<<5)
#define KILLFLAG_KNIFE			(1<<6)	// 刀子, 刺刀
#define KILLFLAG_GRENADE		(1<<7)
#define KILLFLAG_REVENGE		(1<<8)

// ACE Icon
#define ACETYPE_NONE	0
#define ACETYPE_GRAY	1
#define ACETYPE_GOLD	2

class IGameRule
{
public:
	virtual void ServerActivate(void) = 0;
	virtual void ServerDeactivate(void) = 0;
	virtual void Think(void) = 0;
	virtual BOOL TeamFull(int team) = 0;
	virtual int TotalPlayers(int team) = 0;
	virtual void PlayerConnect(CBasePlayer *pPlayer) = 0;
	virtual void PlayerDisconnect(CBasePlayer *pPlayer) = 0;
	virtual BOOL PlayerCanJoin(CBasePlayer *pPlayer) = 0;
	virtual void PlayerJoin(CBasePlayer *pPlayer) = 0;
	virtual BOOL PlayerCanSpawn(CBasePlayer *pPlayer) = 0;
	virtual void PlayerSpawn(CBasePlayer *pPlayer) = 0;
	virtual void PlayerKill(CBasePlayer *pPlayer) = 0;
};

IGameRule *InstallGameRules(void);

extern IGameRule *g_pGameRule;


//------------------------------------------------------------
// 团队竞技
//------------------------------------------------------------

class CGameRule_TD : public IGameRule
{
public:
	void ServerActivate(void);
	void ServerDeactivate(void);
	void Think(void);
	BOOL TeamFull(int team);
	int TotalPlayers(int team);
	void PlayerConnect(CBasePlayer *pPlayer);
	void PlayerDisconnect(CBasePlayer *pPlayer);
	BOOL PlayerCanJoin(CBasePlayer *pPlayer);
	void PlayerJoin(CBasePlayer *pPlayer);
	BOOL PlayerCanSpawn(CBasePlayer *pPlayer);
	void PlayerSpawn(CBasePlayer *pPlayer);
	void PlayerKill(CBasePlayer *pPlayer);

	void MakeAce(void);

	enum SubRuleType
	{
		SUBRULETYPE_KILL,
		SUBRULETYPE_TIME,
	};

	enum WinStatus
	{
		WINSTATUS_NO,
		WINSTATUS_GR,
		WINSTATUS_BL,
		WINSTATUS_DR,
	};

	CBaseEntity *m_rgSpawnSpot[32];
	int m_iRuleType;
	int m_iTargetKill;
	int m_iTargetTime;
	int m_iSpawnTime;
	int m_iMaxPlayer;
	int m_iMaxObserver;
	BOOL m_bGameOver;
	float m_flGameStarTime;
	float m_flGameEndTime;
	BOOL m_bFreezePeriod;
	BOOL m_bGameEnding;
	int m_iNumPlayerGR;
	int m_iNumPlayerBL;
	int m_iNumObserverGR;
	int m_iNumObserverBL;
	int m_iWinStatus;
	int m_iKillCountGR;
	int m_iKillCountBL;
};

//------------------------------------------------------------
// 歼灭模式
//------------------------------------------------------------

class CGameRule_TM : public IGameRule
{
public:
	void ServerActivate(void);
	void ServerDeactivate(void);
	void Think(void);
	void PlayerConnect(CBasePlayer *pPlayer);
	void PlayerDisconnect(CBasePlayer *pPlayer);
	BOOL PlayerCanJoin(CBasePlayer *pPlayer);
	void PlayerJoin(CBasePlayer *pPlayer);
	BOOL PlayerCanSpawn(CBasePlayer *pPlayer);
	void PlayerSpawn(CBasePlayer *pPlayer);
	void PlayerKill(CBasePlayer *pPlayer);
};

//------------------------------------------------------------
// 爆破模式
//------------------------------------------------------------

class CGameRule_BM : public IGameRule
{
public:
	void ServerActivate(void);
	void ServerDeactivate(void);
	void Think(void);
	void PlayerConnect(CBasePlayer *pPlayer);
	void PlayerDisconnect(CBasePlayer *pPlayer);
	BOOL PlayerCanJoin(CBasePlayer *pPlayer);
	void PlayerJoin(CBasePlayer *pPlayer);
	BOOL PlayerCanSpawn(CBasePlayer *pPlayer);
	void PlayerSpawn(CBasePlayer *pPlayer);
	void PlayerKill(CBasePlayer *pPlayer);
};

//------------------------------------------------------------
// 个人竞技
//------------------------------------------------------------

class CGameRule_DM : public IGameRule
{
public:
	void ServerActivate(void);
	void ServerDeactivate(void);
	void Think(void);
	void PlayerConnect(CBasePlayer *pPlayer);
	void PlayerDisconnect(CBasePlayer *pPlayer);
	BOOL PlayerCanJoin(CBasePlayer *pPlayer);
	void PlayerJoin(CBasePlayer *pPlayer);
	BOOL PlayerCanSpawn(CBasePlayer *pPlayer);
	void PlayerSpawn(CBasePlayer *pPlayer);
	void PlayerKill(CBasePlayer *pPlayer);
};

//------------------------------------------------------------
// 突围模式
//------------------------------------------------------------

class CGameRule_ES : public IGameRule
{
public:
	void ServerActivate(void);
	void ServerDeactivate(void);
	void Think(void);
	void PlayerConnect(CBasePlayer *pPlayer);
	void PlayerDisconnect(CBasePlayer *pPlayer);
	BOOL PlayerCanJoin(CBasePlayer *pPlayer);
	void PlayerJoin(CBasePlayer *pPlayer);
	BOOL PlayerCanSpawn(CBasePlayer *pPlayer);
	void PlayerSpawn(CBasePlayer *pPlayer);
	void PlayerKill(CBasePlayer *pPlayer);
};

//------------------------------------------------------------
// 幽灵模式
//------------------------------------------------------------

class CGameRule_GM : public IGameRule
{
public:
	void ServerActivate(void);
	void ServerDeactivate(void);
	void Think(void);
	void PlayerConnect(CBasePlayer *pPlayer);
	void PlayerDisconnect(CBasePlayer *pPlayer);
	BOOL PlayerCanJoin(CBasePlayer *pPlayer);
	void PlayerJoin(CBasePlayer *pPlayer);
	BOOL PlayerCanSpawn(CBasePlayer *pPlayer);
	void PlayerSpawn(CBasePlayer *pPlayer);
	void PlayerKill(CBasePlayer *pPlayer);
};

#endif