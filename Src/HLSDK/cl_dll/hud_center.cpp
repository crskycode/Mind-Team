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

void CHudCenterTips::VidInit(void)
{
	m_flStartTime = 0;
	m_flHoldTime = 0;
}

void CHudCenterTips::Redraw(void)
{
	if (!m_flStartTime)
		return;

	if (ClientTime - m_flStartTime > m_flHoldTime)
	{
		m_flStartTime = 0;
		return;
	}

	int iBaseX = ScreenWide / 2;
	int iBaseY = ScreenTall / 2 - 80;

	int iTextW, iTextH;
	gFontAPI.GetTextSize(gHUD.m_iFontOutline, m_szText, iTextW, iTextH);

	gFontAPI.DrawSetTextFont(gHUD.m_iFontOutline);
	gFontAPI.DrawSetTextColor(255, 180, 30, 255);
	gFontAPI.DrawSetTextPos(iBaseX - iTextW / 2, iBaseY);
	gFontAPI.DrawPrintText(m_szText);
}

void CHudCenterTips::MsgFunc_Center(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);

	wcscpy(m_szText, ANSIToUnicode(READ_STRING()));
	m_flHoldTime = READ_BYTE();	//sec
	m_flStartTime = ClientTime;
}