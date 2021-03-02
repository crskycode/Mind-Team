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

void CHudUnderMark::VidInit(void)
{
	Clear();
	m_pMarkTexture = gTexAPI.LoadDTX("Tex/UI/KillMark/KillMarkUnder.dtx");
}

void CHudUnderMark::Redraw(void)
{
	if (!m_iNumMark)
		return;

	int iBaseX = ScreenWide / 2 + 116;
	int iBaseY = ScreenTall - 36;

	int iDrawX = iBaseX;

	gTexAPI.RenderMode(RenderNormal);
	gTexAPI.DrawSetColor(255, 255, 255, 255);
	gTexAPI.DrawSetTexture(m_pMarkTexture);

	int iCurrentMark = 0;

	for (int i = m_iNumMark - 10; i >= 0; i = i - 10)
	{
		iCurrentMark += 10;

		DrawMark(iDrawX, iBaseY, 4);

		iDrawX -= 26;
	}

	for (int i = iCurrentMark; iCurrentMark < m_iNumMark; iCurrentMark++)
	{
		DrawMark(iDrawX, iBaseY, m_rgMark[iCurrentMark]);

		iDrawX -= 26;
	}
}

void CHudUnderMark::DrawMark(int x, int y, int type)
{
	static int rc[5][4] = 
	{
		{  52, 0, 26, 64 },	// 0 NORMAL
		{  26, 0, 26, 64 },	// 1 HEADSHOT
		{ 103, 0, 25, 64 },	// 2 GOLDHEADSHOT
		{   0, 0, 26, 64 },	// 3 KNIFE
		{  78, 0, 26, 64 },	// 4 10KILLS
	};

	gTexAPI.TextureEnableScissor(rc[type][0], rc[type][1], rc[type][2], rc[type][3]);
	gTexAPI.DrawTexturedRect(x, y, rc[type][2], rc[type][3]);
}

void CHudUnderMark::Add(int iType)
{
	if (m_iNumMark < 1000)
	{
		m_rgMark[m_iNumMark++] = iType;
	}
}

void CHudUnderMark::Clear(void)
{
	m_iNumMark = 0;
	memset(&m_rgMark, 0, sizeof(m_rgMark));
}

void CHudUnderMark::MsgFunc_ResetRound(const char *pszName, int iSize, void *pbuf)
{
	Clear();
}

void CHudUnderMark::MsgFunc_KillMsg(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);

	int iKillerId = READ_BYTE();
	int iVictimId = READ_BYTE();
	int iWeaponId = READ_SHORT();
	int iMultiKill = READ_BYTE();
	int iFlags = READ_SHORT();

	int iLocal = gEngfuncs.GetLocalPlayer()->index;

	if (iKillerId != iLocal || iKillerId == iVictimId)
		return;

	if (!(iFlags & KILLFLAG_SHOWMARK))
		return;

	if (iFlags & KILLFLAG_HEADSHOT)
	{
		if (iFlags & KILLFLAG_HEADSHOT_GOLD)
			Add(2);
		else
			Add(1);
	}
	else if (iFlags & KILLFLAG_KNIFE)
	{
		Add(3);
	}
	else
	{
		Add(0);
	}
}