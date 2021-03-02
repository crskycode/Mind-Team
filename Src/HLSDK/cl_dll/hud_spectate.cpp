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
#include "WeaponData.h"

void CHudSpectate::VidInit(void)
{
	m_bActiveUnderUserInfo = false;
	m_bActiveUnderBar = false;
	m_bActiveInformation = false;

	m_flInfoStartIn = 0;
	m_flInfoStartOut = 0;

	m_pUnderUserInfoGR = gTexAPI.LoadDTX("Tex/UI/SpectateUI/Spectate_Under_UserInfo_GR.dtx");
	m_pUnderUserInfoBL = gTexAPI.LoadDTX("Tex/UI/SpectateUI/Spectate_Under_UserInfo_BL.dtx");

	m_pNoClanMark = gTexAPI.LoadDTX("Tex/UI/SpectateUI/Spectate_No_Clan.dtx");
	m_pDefaultClanMark = gTexAPI.LoadDTX("ClanMark/Default/Default_1.dtx");

	m_pHPNumber = gTexAPI.LoadDTX("Tex/UI/PlayMain/HP_Num.dtx");

	m_pSpecUnder = gTexAPI.LoadDTX("Tex/UI/SpectateUI/Spectate_Under.dtx");
	m_pSpecMain = gTexAPI.LoadDTX("Tex/UI/SpectateUI/Spectate_Main.dtx");
}

static tex_t *hud_GetClassIcon(int iclass)
{
	static char name[MAX_QPATH];
	sprintf(name, "UI/Class/SClass_%d.png", iclass);
	return gTexAPI.LoadPNG(name);
}

void CHudSpectate::Redraw(void)
{
	if ( !ClientState.bIsAlive )
	{
		m_bActiveUnderUserInfo = false;
		m_bActiveUnderBar = false;
	}

	int iLocal = gEngfuncs.GetLocalPlayer()->index;

	gTexAPI.RenderMode(RenderNormal);

	if (m_bActiveUnderUserInfo)
	{
		int iBaseX = 10;
		int iBaseY = ScreenTall - 93;

		gTexAPI.DrawSetColor(255, 255, 255, 255);
		
		if (GameState.m_PlayerTeam[iLocal - 1] == TEAM_GR)
			gTexAPI.DrawSetTexture(m_pUnderUserInfoGR);
		else
			gTexAPI.DrawSetTexture(m_pUnderUserInfoBL);

		gTexAPI.DrawTexturedRect(iBaseX, iBaseY, 256, 128);

		if (m_szSpecWeaponName[0])
		{
			gFontAPI.DrawSetTextFont(gHUD.m_iFontOutline);
			gFontAPI.DrawSetTextColor(185, 213, 217, 255);
			gFontAPI.DrawSetTextPos(iBaseX + 104 - m_iSpecWeaponNameLength / 2, iBaseY + 5);
			gFontAPI.DrawPrintText(m_szSpecWeaponName);
		}

		gTexAPI.DrawSetColor(255, 255, 255, 255);
		gTexAPI.DrawSetTexture(m_pDefaultClanMark);
		gTexAPI.DrawTexturedRect(iBaseX + 3, iBaseY + 23, 45, 45);

		gFontAPI.DrawSetTextFont(gHUD.m_iFont);
		gFontAPI.DrawSetTextColor(185, 213, 217, 255);
		gFontAPI.DrawSetTextPos(iBaseX + 52, iBaseY + 30);
		gFontAPI.DrawPrintText(L"Mind-Team");

		tex_t *pClassIcon = hud_GetClassIcon(GameState.m_PlayerClass[m_iSpecTarget - 1]);
		gTexAPI.RenderMode(RenderNormal);
		gTexAPI.DrawSetColor(255, 255, 255, 255);
		gTexAPI.DrawSetTexture(pClassIcon);
		gTexAPI.DrawTexturedRect(iBaseX + 52, iBaseY + 46, 17, 19);

		if (m_szSpecTargetName[0])
		{
			gFontAPI.DrawSetTextFont(gHUD.m_iFont);
			gFontAPI.DrawSetTextColor(185, 213, 217, 255);
			gFontAPI.DrawSetTextPos(iBaseX + 74, iBaseY + 50);
			gFontAPI.DrawPrintText(m_szSpecTargetName);
		}

		DrawHPNumber(m_iSpecHealth, iBaseX + 174, iBaseY + 73);
	}

	if (m_bActiveInformation)
	{
		const float InTime = 0.15;
		const float OutTime = 0.15;

		if (m_flInfoStartIn || m_flInfoStartOut)
		{
			int iOffset = 0;
			int iAlpha = 0;

			if (m_flInfoStartIn)
			{
				if (ClientTime < m_flInfoStartIn)
				{
					iOffset = 410;
					iAlpha = 0;
				}
				else if (ClientTime > m_flInfoStartIn && ClientTime < m_flInfoStartIn + InTime)
				{
					iOffset = (m_flInfoStartIn + InTime - ClientTime) / InTime * 410;
					iAlpha = (ClientTime - m_flInfoStartIn) / InTime * 255;
				}
				else if (ClientTime > m_flInfoStartIn + InTime)
				{
					iOffset = 0;
					iAlpha = 255;
				}
			}
			else if (m_flInfoStartOut)
			{
				if (ClientTime < m_flInfoStartOut)
				{
					iOffset = 0;
					iAlpha = 255;
				}
				else if (ClientTime > m_flInfoStartOut && ClientTime < m_flInfoStartOut + OutTime)
				{
					iOffset = (ClientTime - m_flInfoStartOut) / OutTime * 410;
					iAlpha = (m_flInfoStartOut + OutTime - ClientTime) / OutTime * 255;
				}
				else if (ClientTime > m_flInfoStartOut + OutTime)
				{
					iOffset = 410;
					iAlpha = 0;
				}
			}

			if (iAlpha > 0)
			{
				int iBaseX = ScreenWide - 219;
				int iBaseY = ScreenTall - 440;

				gTexAPI.DrawSetColor(255, 255, 255, iAlpha);
				gTexAPI.DrawSetTexture(m_pSpecMain);
				gTexAPI.DrawTexturedRect(iBaseX, iBaseY + iOffset, m_pSpecMain->wide, m_pSpecMain->tall);
			}
		}
	}

	if (m_bActiveUnderBar)
	{
		int iBaseX = ScreenWide - 219;
		int iBaseY = ScreenTall - 28;

		gTexAPI.DrawSetColor(255, 255, 255, 255);
		gTexAPI.DrawSetTexture(m_pSpecUnder);
		gTexAPI.DrawTexturedRect(iBaseX, iBaseY, m_pSpecUnder->wide, m_pSpecUnder->tall);

		gFontAPI.DrawSetTextColor(185, 213, 217, 255);
		gFontAPI.DrawSetTextFont(gHUD.m_iFont);
		gFontAPI.DrawSetTextPos(iBaseX + 9, iBaseY + 9);

		
		if (m_flInfoStartIn)
			gFontAPI.DrawPrintText(L"Back Space 键 : 隐藏信息");
		else
			gFontAPI.DrawPrintText(L"查看无线电信息 : Back Space");
	}
}

void CHudSpectate::DrawHPNumber(int n ,int x, int y)
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

	gTexAPI.DrawSetColor(255, 255, 255, 255);
	gTexAPI.DrawSetTexture(m_pHPNumber);

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

void CHudSpectate::PopupInfo(void)
{
	m_flInfoStartIn = ClientTime;
	m_flInfoStartOut = 0;
}

void CHudSpectate::HolsterInfo(void)
{
	m_flInfoStartOut = ClientTime;
	m_flInfoStartIn = 0;
}

bool CHudSpectate::IsInfoPopup(void)
{
	return m_flInfoStartIn != 0;
}

static void strcpy_ANSIToUnicode(wchar_t *_Dst, const char *_Src)
{
	wchar_t *_Res = ANSIToUnicode(_Src);
	wcscpy(_Dst, _Res);
}

static char *hud_GetPlayerName(int index)
{
	hud_player_info_t info;
	gEngfuncs.pfnGetPlayerInfo(index, &info);

	if (info.name)
		return info.name;
	else
		return "";
}

static int hud_GetStringLength(int font, const wchar_t *text)
{
	int wide, tall;
	gFontAPI.GetTextSize(font, text, wide, tall);
	return wide;
}

void CHudSpectate::MsgFunc_SpecTarget(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);

	m_iSpecTarget = READ_BYTE();

	if (m_iSpecTarget)
		strcpy_ANSIToUnicode(m_szSpecTargetName, hud_GetPlayerName(m_iSpecTarget));
	else
		strcpy_ANSIToUnicode(m_szSpecTargetName, "");
}

void CHudSpectate::MsgFunc_SpecHealth(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);

	m_iSpecHealth = READ_SHORT();
}

void CHudSpectate::MsgFunc_SpecWeapon(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);

	m_iSpecWeapon = READ_SHORT();

	if (m_iSpecWeapon)
	{
		WeaponData *pWeapon = WeaponData_GetData(m_iSpecWeapon);

		if (pWeapon)
		{
			strcpy_ANSIToUnicode(m_szSpecWeaponName, pWeapon->szWeaponName);
			m_iSpecWeaponNameLength = hud_GetStringLength(gHUD.m_iFontOutline, m_szSpecWeaponName);
		}
		else
			goto NULLNAME;
	}
	else
	{
NULLNAME:
		strcpy_ANSIToUnicode(m_szSpecWeaponName, "");
		m_iSpecWeaponNameLength = 0;
	}
}