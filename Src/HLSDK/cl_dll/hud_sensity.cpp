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

static cvar_t *cl_sensity = NULL;

void CHudSensity::VidInit(void)
{
	m_pBG = gTexAPI.LoadDTX("tex/UI/Gauge/GaugeBox_R.dtx");
	m_pGauge = gTexAPI.LoadDTX("tex/UI/Gauge/Gauge.dtx");
	m_pNum = gTexAPI.LoadDTX("tex/UI/PlayMain/HP_NUM.dtx");

	m_flDisplayTime = 0;

	// get the pointer
	cl_sensity = gEngfuncs.pfnGetCvarPointer("cl_sensity");

	// pre calc the value
	m_iValue = cl_sensity->value / 20.0 * 100;
}

void CHudSensity::Redraw(void)
{
	if (!m_flDisplayTime)
		return;

	// time out
	if (ClientTime > m_flDisplayTime)
	{
		m_flDisplayTime = 0;
		return;
	}

	int iBaseX = ScreenWide / 2 - 160;
	int iBaseY = ScreenTall / 2 - 140;

	gTexAPI.RenderMode(RenderNormal);
	gTexAPI.DrawSetColor(255, 255, 255, 255);

	gTexAPI.DrawSetTexture(m_pBG);
	gTexAPI.DrawTexturedRect(iBaseX, iBaseY, m_pBG->wide, m_pBG->tall);

	float percen = m_iValue / 100.0;

	int length = 302 * percen;

	gTexAPI.DrawSetTexture(m_pGauge);
	gTexAPI.TextureEnableScissor(0, 0, 8, 13);
	gTexAPI.DrawTexturedRect(iBaseX + 9, iBaseY + 5, length, 13);

	gFontAPI.DrawSetTextFont(gHUD.m_iFont);
	gFontAPI.DrawSetTextColor(255, 255, 255, 255);
	gFontAPI.DrawSetTextPos(iBaseX + 124, iBaseY + 6);
	gFontAPI.DrawPrintText(L" Û±ÍÀŸ∂»");

	DrawNum(m_iValue, iBaseX + 302, iBaseY + 6);
}

void CHudSensity::DrawNum(int n, int x, int y)
{
	int c;
	int dx, dy;
	int i;
	int s;
	int v;

	static int rc[10][4] = 
	{
		{ 109, 0, 10, 11 },
		{   1, 0, 10, 11 },
		{  13, 0, 10, 11 },
		{  25, 0, 10, 11 },
		{  37, 0, 10, 11 },
		{  49, 0, 10, 11 },
		{  61, 0, 10, 11 },
		{  73, 0, 10, 11 },
		{  85, 0, 10, 11 },
		{  97, 0, 10, 11 },
	};

	dx = x;
	dy = y;

	c = 0;
	s = 1;

	gTexAPI.DrawSetTexture(m_pNum);

	for (i = 0; i < 5; i++)
	{
		v = n / s % 10;

		if (i == 0 || v || n >= s)
		{
			c = v;

			gTexAPI.TextureEnableScissor(rc[c][0], rc[c][1], rc[c][2], rc[c][3]);
			gTexAPI.DrawTexturedRect(dx, dy, rc[c][2], rc[c][3]);

			dx = dx - 12;
		}
		else
		{
			break;
		}

		s = s * 10;
	}
}

void CHudSensity::Inc(void)
{
	if (!m_flDisplayTime)
	{
		m_flDisplayTime = ClientTime + 3;
		return;
	}

	m_flDisplayTime = ClientTime + 3;

	if (m_iValue >= 100)
		return;

	m_iValue++;

	float percen = m_iValue / 100.0;
	cl_sensity->value = percen * 20.0;
}

void CHudSensity::Dec(void)
{
	if (!m_flDisplayTime)
	{
		m_flDisplayTime = ClientTime + 3;
		return;
	}

	m_flDisplayTime = ClientTime + 3;

	if (m_iValue <= 0)
		return;

	m_iValue--;

	float percen = m_iValue / 100.0;
	cl_sensity->value = percen * 20.0;
}