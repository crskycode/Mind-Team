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
#include "hud.h"

void CHudWinStatus::VidInit(void)
{
	m_pSucc = gTexAPI.LoadDTX("Tex/UI/Help/MISuccess.dtx");
	m_pFail = gTexAPI.LoadDTX("Tex/UI/Help/MIFail.dtx");
	m_pDraw = gTexAPI.LoadDTX("Tex/UI/Help/BattleDraw.dtx");

	m_flStartTime = 0;
}

void CHudWinStatus::Redraw(void)
{
	if (!m_flStartTime)
		return;

	int iBaseX = ScreenWide / 2 - 256;
	int iBaseY = ScreenTall / 2 - 120;

	float percen;

	if (gHUD.CalcFadePercen(m_flStartTime, 2, 3, 2, percen) == 2)
	{
		m_flStartTime = 0;
		return;
	}

	gTexAPI.RenderMode(RenderNormal);
	gTexAPI.DrawSetColor(255, 255, 255, 255 * percen);

	switch (m_iHelpType)
	{
		case 0:
		{
			gTexAPI.DrawSetTexture(m_pDraw);
			gTexAPI.DrawTexturedRect(iBaseX, iBaseY, m_pDraw->wide, m_pDraw->tall);
			break;
		}
		case 1:
		{
			gTexAPI.DrawSetTexture(m_pSucc);
			gTexAPI.DrawTexturedRect(iBaseX, iBaseY, m_pSucc->wide, m_pSucc->tall);
			break;
		}
		case 2:
		{
			gTexAPI.DrawSetTexture(m_pFail);
			gTexAPI.DrawTexturedRect(iBaseX, iBaseY, m_pFail->wide, m_pFail->tall);
			break;
		}
	}
}

void CHudWinStatus::MsgFunc_WinStatus(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);

	int iStatus = READ_BYTE();
	int iTeam = GameState.m_PlayerTeam[ LocalIndex - 1 ];

	if (GameState.iGameMode == GAMEMODE_TD)
	{
		switch (iTeam)
		{
			case TEAM_GR:
			{
				if (iStatus == 1)
					m_iHelpType = 1;	//win
				else if (iStatus == 2)
					m_iHelpType = 2;	//lose
				else
					m_iHelpType = 0;	//draw

				break;
			}
			case TEAM_BL:
			{
				if (iStatus == 1)
					m_iHelpType = 2;	//lose
				else if (iStatus == 2)
					m_iHelpType = 1;	//win
				else
					m_iHelpType = 0;	//draw

				break;
			}
		}
	}
	else
	{
		return;
	}

	m_flStartTime = ClientTime;
}