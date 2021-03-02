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

void CHudBagIcon::VidInit(void)
{
	m_flStartIn = 0;
	m_bEnableBag = false;
	m_iBagNumber = 1;

	m_pBagBG = gTexAPI.LoadDTX("Tex/UI/PlayMain/Bag_BG.dtx");
	m_pBagLine = gTexAPI.LoadDTX("Tex/UI/PlayMain/Bag_Line.dtx");

	m_pNumber = gTexAPI.LoadDTX("Tex/UI/PlayMain/HP_Num.dtx");
}

void CHudBagIcon::Redraw(void)
{
	if (!m_bEnableBag)
		return;

	int iBaseX = ScreenWide * 0.018750;
	int iBaseY = ScreenTall / 2 - ScreenTall * 0.037037;

	const float InTime = 0.2;

	int iOffset = 0;

	if (ClientTime < m_flStartIn)
	{
		iOffset = -(iBaseX + 50);
	}
	else if (ClientTime > m_flStartIn && ClientTime < m_flStartIn + InTime)
	{
		iOffset = -(iBaseX + 50) + (ClientTime - m_flStartIn) / InTime * (iBaseX + 50);
	}
	else if (ClientTime > m_flStartIn + InTime)
	{
		iOffset = 0;
	}

	gTexAPI.RenderMode(RenderNormal);
	gTexAPI.DrawSetColor(255, 255, 255, 180);
	gTexAPI.DrawSetTexture(m_pBagBG);
	gTexAPI.DrawTexturedRect(iBaseX + iOffset, iBaseY, m_pBagBG->wide, m_pBagBG->tall);

	gTexAPI.RenderMode(RenderNormal);
	gTexAPI.DrawSetColor(255, 255, 255, 255);
	gTexAPI.DrawSetTexture(m_pBagLine);
	gTexAPI.DrawTexturedRect(iBaseX + iOffset, iBaseY, m_pBagLine->wide, m_pBagLine->tall);

	DrawNumber(m_iBagNumber, iBaseX + 6 + iOffset, iBaseY + 31);
}

void CHudBagIcon::DrawNumber(int n, int x, int y)
{
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

	gTexAPI.DrawSetTexture(m_pNumber);
	gTexAPI.TextureEnableScissor(rc[n][0], rc[n][1], rc[n][2], rc[n][3]);
	gTexAPI.DrawTexturedRect(x, y, 10, 11);
}

void CHudBagIcon::MsgFunc_EnableBag(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);

	m_bEnableBag = READ_BYTE();

	if (m_bEnableBag)
	{
		m_flStartIn = ClientTime;
	}
}