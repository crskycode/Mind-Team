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
#include "GameState.h"

#include <triangleapi.h>

void CHudTeammateInfo::VidInit(void)
{
	m_pMeterNum = gTexAPI.LoadDTX("tex/UI/ProjectionID/MeterNum.dtx");
	m_iType = 0;
}

void CHudTeammateInfo::Redraw(void)
{
	if (!m_iType)
		return;

	int iLocal = LocalIndex - 1;

	for (int i = 0; i < 32; i++)
	{
		// Skip self
		if ( i == iLocal )
			continue;

		wchar_t *name = gHUD.GetPlayerName(i + 1);

		// This player is not connected.
		if ( !name )
			continue;

		Vector self = gEngfuncs.GetEntityByIndex( iLocal + 1 )->origin;
		Vector world = Vector( GameState.m_PlayerOrigin[i] );
		//Vector world = gEngfuncs.GetEntityByIndex( i + 1 )->origin;
		Vector point;

		// Make point on player's head
		world.z += 45;

		if ( gEngfuncs.pTriAPI->WorldToScreen(world, point) )
			continue;

		point.x = XPROJECT(point.x);
		point.y = YPROJECT(point.y);

		// Check point out of screen
		if (point.x < -80 || point.x > ScreenWide)
			continue;

		if (point.y < -16 || point.y > ScreenTall)
			continue;

		int iNameWide, iNameTall;
		gFontAPI.GetTextSize(gHUD.m_iFont, name, iNameWide, iNameTall);

		gFontAPI.DrawSetTextFont(gHUD.m_iFont);

		if (GameState.m_PlayerTeam[iLocal] == TEAM_GR)
			gFontAPI.DrawSetTextColor(5, 132, 192, 150);	// blue
		else
			gFontAPI.DrawSetTextColor(207, 181, 147, 150);	// yellow

		int iBaseX = point.x - iNameWide / 2;
		int iBaseY = point.y - iNameTall / 2;

		// Draw player name
		gFontAPI.DrawSetTextPos(iBaseX, iBaseY);
		gFontAPI.DrawPrintText(name);

		if (m_iType == 2)
		{
			int distance = (world - self).Length() * 0.02;

			// Draw meter number
			gTexAPI.RenderMode(RenderNormal);
			gTexAPI.DrawSetColor(255, 255, 255, 255);

			DrawMeter(distance, iBaseX + 16, iBaseY - 10);
		}
	}
}

void CHudTeammateInfo::DrawMeter(int n, int x, int y)
{
	int c;
	int dx, dy;
	int i;
	int s;
	int v;

	static int rc[10][4] = 
	{
		{ 72, 0, 7, 7 },
		{  0, 0, 7, 7 },
		{  8, 0, 7, 7 },
		{ 16, 0, 7, 7 },
		{ 24, 0, 7, 7 },
		{ 32, 0, 7, 7 },
		{ 40, 0, 7, 7 },
		{ 48, 0, 7, 7 },
		{ 56, 0, 7, 7 },
		{ 64, 0, 7, 7 },
	};

	dx = x;
	dy = y;

	c = 0;
	s = 1;

	gTexAPI.DrawSetTexture(m_pMeterNum);

	for (i = 0; i < 5; i++)
	{
		v = n / s % 10;

		if (i == 0 || v || n >= s)
		{
			c = v;

			gTexAPI.TextureEnableScissor(rc[c][0], rc[c][1], rc[c][2], rc[c][3]);
			gTexAPI.DrawTexturedRect(dx, dy, rc[c][2], rc[c][3]);

			dx = dx - 8;
		}
		else
		{
			break;
		}

		s = s * 10;
	}
}

void CHudTeammateInfo::Toggle(void)
{
	switch (m_iType)
	{
		case 0: m_iType = 1; break;
		case 1: m_iType = 2; break;
		case 2: m_iType = 0; break;
	}
}