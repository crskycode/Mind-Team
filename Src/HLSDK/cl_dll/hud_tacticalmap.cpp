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

void CHudTacticalMap::VidInit(void)
{
	m_flStartIn = 0;
	m_flStartOut = 0;
}

void CHudTacticalMap::DataInit(void)
{
	m_pMapTexture = gTexAPI.LoadDTX("Tex/UI/MiniMap/TacticalMap_1_114.dtx");
}

void CHudTacticalMap::Redraw(void)
{
	const float InTime = 0.065;
	const float OutTime = 0.1;

	if (!m_flStartIn && !m_flStartOut)
		return;

	int iOffset = 0;
	int iAlpha = 0;

	if (m_flStartIn)
	{
		if (ClientTime < m_flStartIn)
		{
			iOffset = -100;
			iAlpha = 0;
		}
		else if (ClientTime > m_flStartIn && ClientTime < m_flStartIn + InTime)
		{
			iOffset = -100 + (ClientTime - m_flStartIn) / InTime * 100;
			iAlpha = (ClientTime - m_flStartIn) / InTime * 255;
		}
		else if (ClientTime > m_flStartIn + InTime)
		{
			iOffset = 0;
			iAlpha = 255;
		}
	}
	else if (m_flStartOut)
	{
		if (ClientTime < m_flStartOut)
		{
			iOffset = 0;
			iAlpha = 0;
		}
		else if (ClientTime > m_flStartOut && ClientTime < m_flStartOut + OutTime)
		{
			iOffset = 0;
			iAlpha = (m_flStartOut + OutTime - ClientTime) / OutTime * 255;
		}
		else if (ClientTime > m_flStartOut + OutTime)
		{
			m_flStartOut = 0;
			return;
		}
	}

	int iBaseX = ScreenWide / 2 - 246;
	int iBaseY = ScreenTall / 2 - 196;

	gTexAPI.RenderMode(RenderNormal);
	gTexAPI.DrawSetColor(255, 255, 255, iAlpha);

	gTexAPI.DrawSetTexture(m_pMapTexture);
	gTexAPI.DrawTexturedRect(iBaseX + iOffset, iBaseY, m_pMapTexture->wide, m_pMapTexture->tall);

	//gEngfuncs.pfnFillRGBA( iBaseX + 10 + (473 / 2), iBaseY + 10 + (372 / 2), 4, 4, 255, 0, 0, 255);

	int iLocal = LocalIndex - 1;

	for (int i = 0; i < 32; i++)
	{
		if (GameState.m_PlayerTeam[i] != GameState.m_PlayerTeam[iLocal])
			continue;

		Vector vecWorld, vecPoint;

		if (i == iLocal)
		{
			vecWorld = ClientState.vecOrigin;
			WorldToMap(vecWorld, vecPoint);

			int iDrawX = iBaseX + 10 + (472 / 2) + vecPoint.x + iOffset;
			int iDrawY = iBaseY + 10 + (372 / 2) + vecPoint.y;

			gEngfuncs.pfnFillRGBA(iDrawX, iDrawY, 4, 4, 0, 255, 0, iAlpha);

			gFontAPI.DrawSetTextFont(gHUD.m_iFont);
			gFontAPI.DrawSetTextColor(0, 255, 0, iAlpha);
			gFontAPI.DrawSetTextPos(iDrawX + 6, iDrawY - 4);
			gFontAPI.DrawPrintText(GameState.m_PlayerName[i]);
		}
		else
		{
			vecWorld = GameState.m_PlayerOrigin[i];
			WorldToMap(vecWorld, vecPoint);

			int iDrawX = iBaseX + 10 + (472 / 2) + vecPoint.x + iOffset;
			int iDrawY = iBaseY + 10 + (372 / 2) + vecPoint.y;

			gEngfuncs.pfnFillRGBA(iDrawX, iDrawY, 4, 4, 255, 255, 255, iAlpha);

			gFontAPI.DrawSetTextFont(gHUD.m_iFont);
			gFontAPI.DrawSetTextColor(255, 255, 255, iAlpha);
			gFontAPI.DrawSetTextPos(iDrawX + 6, iDrawY - 4);
			gFontAPI.DrawPrintText(GameState.m_PlayerName[i]);
		}
	}

	// box offset 10x 10y
	// box size 472x372
}

void CHudTacticalMap::WorldToMap(Vector &world, Vector &point)
{
	// fake
	float offsetX = -180;
	float offsetY =  680;

	float zoom = 0.115234375;

	point.x = (world.x - offsetX) /*/ 8.0*/ * zoom;
	point.y = (world.y - offsetY) /*/ 8.0*/ * zoom;

	point.y = -point.y;
}

void CHudTacticalMap::Show(void)
{
	m_flStartIn = ClientTime;
	m_flStartOut = 0;
}

void CHudTacticalMap::Hide(void)
{
	if (!m_flStartIn)
		return;

	m_flStartOut = ClientTime;
	m_flStartIn = 0;
}