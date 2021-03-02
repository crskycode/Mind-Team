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

#ifndef GAMESTATE_H
#define GAMESTATE_H

#define TEAM_NO	0
#define TEAM_GR	1		/* 保卫者 */
#define TEAM_BL	2		/* 潜伏者 */

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

struct ScoreInfo
{
	int iKills;
	int iDeads;
};

#define SCOREATTRIB_DEAD	(1<<0)
#define SCOREATTRIB_C4		(1<<1)

// ACE Icon
#define ACETYPE_NONE	0
#define ACETYPE_GRAY	1
#define ACETYPE_GOLD	2

typedef struct game_state_s
{
	int iGameMode;		/* 游戏模式 */
	int iSubRuleType;
	int iRoundTime;
	int iRoundKill;
	int iScoreBL;		/* BL分数 */
	int iScoreGR;		/* GR分数 */

	wchar_t		m_PlayerName[32][32];
	int			m_PlayerTeam[32];
	int			m_PlayerClass[32];
	bool		m_PlayerAlive[32];
	ScoreInfo	m_PlayerScore[32];
	int			m_PlayerAceType[32];
	float		m_PlayerOrigin[32][3];
}
game_state_t;

extern game_state_t GameState;

#endif