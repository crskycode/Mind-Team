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

#include "cl_dll.h"
#include "util.h"
#include "hud.h"

void CHudScoreBoard::VidInit(void)
{
	m_flStartIn = 0;
	m_flStartOut = 0;
	m_flNextUpdate = 0;

	m_pScoreBoard = gTexAPI.LoadDTX("Tex/UI/PlayMain/ScoreBoard.dtx");
	m_pClanMark = gTexAPI.LoadDTX("ClanMark/Default/Default_Small_1.dtx");
	m_pAceIcon = gTexAPI.LoadDTX("Tex/UI/PlayMain/ACE_Score.dtx");
	m_pDeathIcon = gTexAPI.LoadPNG("UI/UI_GameRoom/DeathIcon.png");

	m_pMissionBox = gTexAPI.LoadDTX("tex/UI/PlayMain/Mission/Mission_Box.dtx");
}

void CHudScoreBoard::Redraw(void)
{
	float percen;

	int offset, alpha;

	if (m_flStartIn)
	{
		// Calculate go-in animation
		gHUD.CalcFadePercen(m_flStartIn, 0.065, 9999.0, 0.0, percen);

		// Scale up the value
		offset = -100 + (100 * percen);
		alpha = 255 * percen;
	}
	else if (m_flStartOut)
	{
		// Calculate go-out animation
		if (gHUD.CalcFadePercen(m_flStartOut, 0.0, 0.0, 0.1, percen) == 2)
		{
			// End of go-out
			m_flStartOut = 0;
			return;
		}

		// Scale up the value
		offset = 0;
		alpha = 255 * percen;
	}
	else
	{
		return;
	}

	// Interval update in 0.5s
	if (ClientTime > m_flNextUpdate)
	{
		PreparePlayerInfo();
		m_flNextUpdate = ClientTime + 0.5;
	}

	// Center of screen
	int iBaseX = ScreenWide / 2 - 246;
	int iBaseY = ScreenTall / 2 - 260;

	// Current draw position
	int iDrawX, iDrawY;

	// The local player
	int self = LocalIndex - 1;

	// Temp variable...
	char szTemp[MAX_QPATH];
	wchar_t szTempW[MAX_QPATH];
	tex_t *pClassIcon;

	// Draw the scoreboard
	switch (GameState.iGameMode)
	{
	case GAMEMODE_TD:
		{
			// SetUp texture render mode
			gTexAPI.RenderMode(RenderNormal);
			gTexAPI.DrawSetColor(255, 255, 255, alpha);

			// SetUp BG texture
			gTexAPI.DrawSetTexture(m_pScoreBoard);

			// Set draw position to scoreboard
			iDrawX = iBaseX;
			iDrawY = iBaseY;

			if (GameState.m_PlayerTeam[self] == TEAM_GR)
			{
				// Draw GR header
				gTexAPI.TextureEnableScissor(0, 359, 512, 34);
				gTexAPI.DrawTexturedRect(iDrawX + offset, iDrawY, 512, 34);

				iDrawY += 34;

				// Draw GR tab
				gTexAPI.TextureEnableScissor(0, 0, 512, 178);
				gTexAPI.DrawTexturedRect(iDrawX + offset, iDrawY, 512, 178);

				iDrawY += 178;

				// Draw BL header
				gTexAPI.TextureEnableScissor(0, 394, 512, 22);
				gTexAPI.DrawTexturedRect(iDrawX + offset, iDrawY, 512, 22);

				iDrawY += 22;

				// Draw BL tab
				gTexAPI.TextureEnableScissor(0, 181, 512, 177);
				gTexAPI.DrawTexturedRect(iDrawX + offset, iDrawY, 512, 177);
			}
			else
			{
				// Draw BL header
				gTexAPI.TextureEnableScissor(0, 417, 512, 34);
				gTexAPI.DrawTexturedRect(iDrawX + offset, iDrawY, 512, 34);

				iDrawY += 34;

				// Draw BL tab
				gTexAPI.TextureEnableScissor(0, 0, 512, 178);
				gTexAPI.DrawTexturedRect(iDrawX + offset, iDrawY, 512, 178);

				iDrawY += 178;

				// Draw GR header
				gTexAPI.TextureEnableScissor(0, 452, 512, 22);
				gTexAPI.DrawTexturedRect(iDrawX + offset, iDrawY, 512, 22);

				iDrawY += 22;

				// Draw GR tab
				gTexAPI.TextureEnableScissor(0, 181, 512, 177);
				gTexAPI.DrawTexturedRect(iDrawX + offset, iDrawY, 512, 177);
			}

			// SetUp text font
			gFontAPI.DrawSetTextFont(gHUD.m_iFont);

			// Set draw position to Top list
			iDrawX = iBaseX + 14;
			iDrawY = iBaseY + 42;

			// Draw Top list
			for (int i = 0; i < 8; i++)
			{
				// Pointer to slot
				PlayerInfo *info;

				if (GameState.m_PlayerTeam[self] == TEAM_GR)
					info = &m_PlayerInfoGR[i];
				else
					info = &m_PlayerInfoBL[i];

				// This player is not connecting
				if (!info->index)
					continue;

				// SetUp icon color
				gTexAPI.DrawSetColor(255, 255, 255, alpha);

				// Draw bar for local player (self)
				if (info->index - 1 == self)
				{
					gTexAPI.DrawSetTexture(m_pScoreBoard);
					gTexAPI.TextureEnableScissor(0, 476, 512, 18);
					gTexAPI.DrawTexturedRect(iDrawX, iDrawY, 512, 18);
				}

				// Draw ACE icon
				switch (info->acetype)
				{
				case ACETYPE_GOLD:
					{
						gTexAPI.DrawSetTexture(m_pAceIcon);
						gTexAPI.TextureEnableScissor(0, 0, 48, 16);
						gTexAPI.DrawTexturedRect(iDrawX, iDrawY + 1, 48, 16);
						break;
					}
				case ACETYPE_GRAY:
					{
						gTexAPI.DrawSetTexture(m_pAceIcon);
						gTexAPI.TextureEnableScissor(48, 18, 32, 12);
						gTexAPI.DrawTexturedRect(iDrawX + 14, iDrawY + 3, 32, 12);
						break;
					}
				}

				// Draw 1st icon
				if (i == 0)
				{
					gTexAPI.DrawSetTexture(m_pAceIcon);
					gTexAPI.TextureEnableScissor(0, 18, 22, 14);
					gTexAPI.DrawTexturedRect(iDrawX + 44, iDrawY + 1, 22, 14);
				}

				// SetUp class icon
				sprintf(szTemp, "UI/Class/SClass_%d.png", info->iclass);
				pClassIcon = gTexAPI.LoadPNG(szTemp);

				// Draw class icon
				gTexAPI.DrawSetTexture(pClassIcon);
				gTexAPI.DrawTexturedRect(iDrawX + 68, iDrawY - 1, pClassIcon->wide, pClassIcon->tall);

				// Draw death icon
				if (!info->alive)
				{
					gTexAPI.DrawSetTexture(m_pDeathIcon);
					gTexAPI.DrawTexturedRect(iDrawX + 59, iDrawY - 4, m_pDeathIcon->wide, m_pDeathIcon->tall);
				}

				// Draw clan icon
				if (info->clan[0])
				{
					gTexAPI.DrawSetTexture(m_pClanMark);
					gTexAPI.DrawTexturedRect(iDrawX + 204, iDrawY, m_pClanMark->wide + 2, m_pClanMark->tall + 2);
				}

				// SetUp text color
				if (info->alive)
					gFontAPI.DrawSetTextColor(68, 181, 225, alpha);
				else
					gFontAPI.DrawSetTextColor(86, 112, 113, alpha);

				// Draw player name
				gFontAPI.DrawSetTextPos(iDrawX + 92, iDrawY + 3);
				gFontAPI.DrawPrintText(info->name);

				// Draw clan name
				gFontAPI.DrawSetTextPos(iDrawX + 224, iDrawY + 3);
				gFontAPI.DrawPrintText(info->clan);

				// Draw 'KILLS'
				swprintf(szTempW, L"%3d", info->kills);
				gFontAPI.DrawSetTextPos(iDrawX + 350 + offset, iDrawY + 3);
				gFontAPI.DrawPrintText(szTempW);

				// Draw 'DEADS'
				swprintf(szTempW, L"%3d", info->deads);
				gFontAPI.DrawSetTextPos(iDrawX + 388 + offset, iDrawY + 3);
				gFontAPI.DrawPrintText(szTempW);

				// Draw 'PING'
				if (!info->isbot)
				{
					swprintf(szTempW, L"%d", info->ping);
					gFontAPI.DrawSetTextPos(iDrawX + 436 + offset, iDrawY + 3);
					gFontAPI.DrawPrintText(szTempW);
				}

				// Set draw position to next row
				iDrawY += 21;
			}

			// Set draw position to Bottom list
			iDrawX = iBaseX + 14;
			iDrawY = iBaseY + 241;

			// Draw Bottom list
			for (int i = 0; i < 8; i++)
			{
				// Pointer to slot
				PlayerInfo *info;

				if (GameState.m_PlayerTeam[self] == TEAM_GR)
					info = &m_PlayerInfoBL[i];
				else
					info = &m_PlayerInfoGR[i];

				// This player is not connecting
				if (!info->index)
					continue;

				// SetUp icon color
				gTexAPI.DrawSetColor(255, 255, 255, alpha);

				// Draw bar for local player (self)
				if (info->index - 1 == self)
				{
					gTexAPI.DrawSetTexture(m_pScoreBoard);
					gTexAPI.TextureEnableScissor(0, 476, 512, 18);
					gTexAPI.DrawTexturedRect(iDrawX, iDrawY, 512, 18);
				}

				// Draw ACE icon
				switch (info->acetype)
				{
				case ACETYPE_GOLD:
					{
						gTexAPI.DrawSetTexture(m_pAceIcon);
						gTexAPI.TextureEnableScissor(0, 0, 48, 16);
						gTexAPI.DrawTexturedRect(iDrawX, iDrawY + 1, 48, 16);
						break;
					}
				case ACETYPE_GRAY:
					{
						gTexAPI.DrawSetTexture(m_pAceIcon);
						gTexAPI.TextureEnableScissor(48, 18, 32, 12);
						gTexAPI.DrawTexturedRect(iDrawX + 14, iDrawY + 3, 32, 12);
						break;
					}
				}

				// Draw 1st icon
				if (i == 0)
				{
					gTexAPI.DrawSetTexture(m_pAceIcon);
					gTexAPI.TextureEnableScissor(0, 18, 22, 14);
					gTexAPI.DrawTexturedRect(iDrawX + 44, iDrawY + 1, 22, 14);
				}

				// SetUp class icon
				sprintf(szTemp, "UI/Class/SClass_%d.png", info->iclass);
				pClassIcon = gTexAPI.LoadPNG(szTemp);

				// Draw class icon
				gTexAPI.DrawSetTexture(pClassIcon);
				gTexAPI.DrawTexturedRect(iDrawX + 68, iDrawY - 1, pClassIcon->wide, pClassIcon->tall);

				// Draw death icon
				if (!info->alive)
				{
					gTexAPI.DrawSetTexture(m_pDeathIcon);
					gTexAPI.DrawTexturedRect(iDrawX + 59, iDrawY - 4, m_pDeathIcon->wide, m_pDeathIcon->tall);
				}

				// Draw clan icon
				if (info->clan[0])
				{
					gTexAPI.DrawSetTexture(m_pClanMark);
					gTexAPI.DrawTexturedRect(iDrawX + 204, iDrawY, m_pClanMark->wide + 2, m_pClanMark->tall + 2);
				}

				// SetUp text color
				if (info->alive)
					gFontAPI.DrawSetTextColor(121, 150, 154, alpha);
				else
					gFontAPI.DrawSetTextColor(86, 112, 113, alpha);

				// Draw player name
				gFontAPI.DrawSetTextPos(iDrawX + 92, iDrawY + 3);
				gFontAPI.DrawPrintText(info->name);

				// Draw clan name
				gFontAPI.DrawSetTextPos(iDrawX + 224, iDrawY + 3);
				gFontAPI.DrawPrintText(info->clan);

				// Draw 'KILLS'
				swprintf(szTempW, L"%3d", info->kills);
				gFontAPI.DrawSetTextPos(iDrawX + 350 + offset, iDrawY + 3);
				gFontAPI.DrawPrintText(szTempW);

				// Draw 'DEADS'
				swprintf(szTempW, L"%3d", info->deads);
				gFontAPI.DrawSetTextPos(iDrawX + 388 + offset, iDrawY + 3);
				gFontAPI.DrawPrintText(szTempW);

				// Draw 'PING'
				if (!info->isbot)
				{
					swprintf(szTempW, L"%d", info->ping);
					gFontAPI.DrawSetTextPos(iDrawX + 436 + offset, iDrawY + 3);
					gFontAPI.DrawPrintText(szTempW);
				}

				// Set draw position to next row
				iDrawY += 21;
			}

			// End of draw
			break;
		}
	case GAMEMODE_TM:
	case GAMEMODE_BM:
	case GAMEMODE_ES:
	case GAMEMODE_GM:
		break;
	}

	// SetUp FPS font
	gFontAPI.DrawSetTextFont(gHUD.m_iFont);

	// Draw FPS
	swprintf(szTempW, L"%.1f FPS", gpMTEngine->GetFPS());
	gFontAPI.DrawSetTextColor(162, 247, 247, alpha);
	gFontAPI.DrawSetTextPos(iBaseX + offset, iBaseY + 417);
	gFontAPI.DrawPrintText(szTempW);

	// Set draw position to mission box
	iDrawX = iBaseX + 34 + offset;
	iDrawY = iBaseY + 434;

	// Draw mission box
	gTexAPI.DrawSetColor(255, 255, 255, alpha);
	gTexAPI.DrawSetTexture(m_pMissionBox);
	gTexAPI.DrawTexturedRect(iDrawX, iDrawY, m_pMissionBox->wide, m_pMissionBox->tall);

	// SetUp mission box font
	gFontAPI.DrawSetTextFont(gHUD.m_iFontOutline);

	// Draw day mission
	gFontAPI.DrawSetTextColor(114, 135, 162, alpha);
	gFontAPI.DrawSetTextPos(iDrawX + 12, iDrawY + 4);
	gFontAPI.DrawPrintText(L"每日任务");

	// Draw price mission
	gFontAPI.DrawSetTextColor(157, 148, 123, alpha);
	gFontAPI.DrawSetTextPos(iDrawX + 12, iDrawY + 88);
	gFontAPI.DrawPrintText(L"悬赏任务");

	// Draw mission details
	gFontAPI.DrawSetTextColor(165, 165, 165, alpha);
	gFontAPI.DrawSetTextPos(iDrawX + 102, iDrawY + 107);
	gFontAPI.DrawPrintText(L"选定悬赏任务之后即可开始执行任务。");
}

static int PlayerCompare(const void *_Arg1, const void *_Arg2)
{
	CHudScoreBoard::PlayerInfo *p1 = (CHudScoreBoard::PlayerInfo *)_Arg1;
	CHudScoreBoard::PlayerInfo *p2 = (CHudScoreBoard::PlayerInfo *)_Arg2;

	if (p1->index && !p2->index)
		return -1;
	else if (!p1->index && p2->index)
		return 1;
	else if (!p1->index && !p2->index)
		return 0;

	if (p1->kills > p2->kills)
		return -1;
	else if (p1->kills == p2->kills)
	{
		if (p1->deads < p2->deads)
			return -1;
		else if (p1->deads > p2->deads)
			return 1;
	}
	else //if (p1->kills < p2->kills)
		return 1;

	return 0;
}

void CHudScoreBoard::PreparePlayerInfo(void)
{
	// Zeroize the list
	memset(m_PlayerInfoGR, 0, sizeof(m_PlayerInfoGR));
	memset(m_PlayerInfoBL, 0, sizeof(m_PlayerInfoBL));

	int index, num;

	// Fill GR players to list
	for (index = 0, num = 0; index < 32; index++)
	{
		// Get player info from engine
		hud_player_info_t engplayer;
		gEngfuncs.pfnGetPlayerInfo(index + 1, &engplayer);

		// This player is not connecting
		if (!engplayer.name)
			continue;

		// Compare team
		if (GameState.m_PlayerTeam[index] != TEAM_GR)
			continue;

		// Pointer to slot
		PlayerInfo *info = &m_PlayerInfoGR[num++];

		// The index(1~32) of player
		info->index = index + 1;

		// Convert char
		UTIL_ANSIToUnicode(info->name, engplayer.name);
		UTIL_ANSIToUnicode(info->clan, "Mind-Team");

		// Copy info...
		info->iclass = GameState.m_PlayerClass[index];
		info->kills = GameState.m_PlayerScore[index].iKills;
		info->deads = GameState.m_PlayerScore[index].iDeads;
		info->ping = engplayer.ping;
		info->alive = GameState.m_PlayerAlive[index];
		info->acetype = GameState.m_PlayerAceType[index];

		// Check the bot
		info->isbot = UTIL_IsPlayerBot(index + 1);
	}

	// Fill BL players to list
	for (index = 0, num = 0; index < 32; index++)
	{
		// Get player info from engine
		hud_player_info_t engplayer;
		gEngfuncs.pfnGetPlayerInfo(index + 1, &engplayer);

		// This player is not connecting
		if (!engplayer.name)
			continue;

		// Compare team
		if (GameState.m_PlayerTeam[index] != TEAM_BL)
			continue;

		// Pointer to slot
		PlayerInfo *info = &m_PlayerInfoBL[num++];

		// The index(1~32) of player
		info->index = index + 1;

		// Convert char
		UTIL_ANSIToUnicode(info->name, engplayer.name);
		UTIL_ANSIToUnicode(info->clan, "Mind-Team");

		// Copy info...
		info->iclass = GameState.m_PlayerClass[index];
		info->kills = GameState.m_PlayerScore[index].iKills;
		info->deads = GameState.m_PlayerScore[index].iDeads;
		info->ping = engplayer.ping;
		info->alive = GameState.m_PlayerAlive[index];
		info->acetype = GameState.m_PlayerAceType[index];

		// Check the bot
		info->isbot = UTIL_IsPlayerBot(index + 1);
	}

	// Sort all players
	qsort(m_PlayerInfoGR, 32, sizeof(PlayerInfo), &PlayerCompare);
	qsort(m_PlayerInfoBL, 32, sizeof(PlayerInfo), &PlayerCompare);
}

void CHudScoreBoard::Show(void)
{
	m_flStartIn = ClientTime;
	m_flStartOut = 0;
}

void CHudScoreBoard::Hide(void)
{
	if (!m_flStartIn)
		return;

	m_flStartOut = ClientTime;
	m_flStartIn = 0;
}