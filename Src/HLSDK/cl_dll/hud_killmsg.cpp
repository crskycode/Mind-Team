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

void CHudKillMsg::VidInit(void)
{
	memset( &m_MsgList, 0, sizeof(m_MsgList) );

	m_pWeaponSkull = gTexAPI.LoadDTX("Tex/UI/KillMsg/Shot_Skull.dtx");

	m_pShotHead = gTexAPI.LoadDTX("Tex/UI/KillMsg/Shot_Head.dtx");
	m_pShotGoldHead = gTexAPI.LoadDTX("Tex/UI/KillMsg/Shot_GoldHead.dtx");
	m_pShotWall = gTexAPI.LoadDTX("Tex/UI/KillMsg/Shot_Wall.dtx");
	m_pShotWallHead = gTexAPI.LoadDTX("Tex/UI/KillMsg/Shot_Wall_Head.dtx");
	m_pShotWallGoldHead = gTexAPI.LoadDTX("Tex/UI/KillMsg/Shot_Wall_GoldHead.dtx");

	m_pMultiKill2 = gTexAPI.LoadDTX("Tex/UI/KillMsg/Shot_MultiKill2.dtx");
	m_pMultiKill3 = gTexAPI.LoadDTX("Tex/UI/KillMsg/Shot_MultiKill3.dtx");
	m_pMultiKill4 = gTexAPI.LoadDTX("Tex/UI/KillMsg/Shot_MultiKill4.dtx");
	m_pMultiKill5 = gTexAPI.LoadDTX("Tex/UI/KillMsg/Shot_MultiKill5.dtx");
	m_pMultiKill6 = gTexAPI.LoadDTX("Tex/UI/KillMsg/Shot_MultiKill6.dtx");
	m_pMultiKillMax = gTexAPI.LoadDTX("Tex/UI/KillMsg/Shot_MultiKillMax.dtx");

	m_pFirstKill = gTexAPI.LoadDTX("Tex/UI/KillMsg/Shot_FirstKill.dtx");
	m_pLastKill = gTexAPI.LoadDTX("Tex/UI/KillMsg/Shot_LastKill.dtx");
}

void CHudKillMsg::Think(void)
{
	if (ClientTime > m_flNextScrollUp)
	{
		ScrollUp();
		m_flNextScrollUp = ClientTime + 5;
	}
}

void CHudKillMsg::Redraw(void)
{
	int iBaseX = ScreenWide - 10;
	int iBaseY = 40;

	//gFontAPI.DrawSetTextFont(gHUD.m_iFontOutline);
	gFontAPI.DrawSetTextFont(gHUD.m_iFont);

	int iDrawX = iBaseX;
	int iDrawY = iBaseY;

	for (int i = 0; i < 4; i++)
	{
		KillMsg &item = m_MsgList[i];

		if ( !item.active )
			continue;

		iDrawX = iBaseX;

		iDrawX -= item.victim_length;
		gFontAPI.DrawSetTextColor(item.victim_color.r, item.victim_color.g, item.victim_color.b, 255);
		gFontAPI.DrawSetTextPos(iDrawX, iDrawY + 11);
		gFontAPI.DrawPrintText(item.victim_name);

		iDrawX -= item.weapon_wide;
		gTexAPI.RenderMode(RenderAdditive);
		gTexAPI.DrawSetColor(255, 255, 255, 255);
		gTexAPI.DrawSetTexture(item.weapon_icon);
		gTexAPI.TextureEnableScissor(0, 0, item.weapon_wide, 32);	// weapon_wide 是从icon图片最左边算到间隔为止
		gTexAPI.DrawTexturedRect(iDrawX, iDrawY, item.weapon_wide, 32);

		iDrawX -= item.killer_length;
		gFontAPI.DrawSetTextColor(item.killer_color.r, item.killer_color.g, item.killer_color.b, 255);
		gFontAPI.DrawSetTextPos(iDrawX, iDrawY + 11);
		gFontAPI.DrawPrintText(item.killer_name);

		gTexAPI.DrawSetColor(255, 255, 255, 255);

		if (item.flags & (KILLFLAG_HEADSHOT | KILLFLAG_WALLSHOT))
		{
			iDrawX -= 42;

			gTexAPI.RenderMode(RenderAdditive);

			if (item.flags & KILLFLAG_HEADSHOT)
			{
				if (item.flags & KILLFLAG_WALLSHOT)
				{
					if (item.flags & KILLFLAG_HEADSHOT_GOLD)
						gTexAPI.DrawSetTexture(m_pShotWallGoldHead);
					else
						gTexAPI.DrawSetTexture(m_pShotWallHead);
				}
				else
				{
					if (item.flags & KILLFLAG_HEADSHOT_GOLD)
						gTexAPI.DrawSetTexture(m_pShotGoldHead);
					else
						gTexAPI.DrawSetTexture(m_pShotHead);
				}
			}
			else
			{
				gTexAPI.DrawSetTexture(m_pShotWall);
			}

			gTexAPI.TextureEnableScissor(0, 0, 42, 32);
			gTexAPI.DrawTexturedRect(iDrawX, iDrawY, 42, 32);
		}

		if (item.multiKill > 1)
		{
			int _wide;

			if (item.flags & (KILLFLAG_HEADSHOT | KILLFLAG_WALLSHOT))
				_wide = 50;
			else
				_wide = 60;

			iDrawX -= _wide;

			gTexAPI.RenderMode(RenderNormal);

			switch (item.multiKill)
			{
				case 2: gTexAPI.DrawSetTexture(m_pMultiKill2); break;
				case 3: gTexAPI.DrawSetTexture(m_pMultiKill3); break;
				case 4: gTexAPI.DrawSetTexture(m_pMultiKill4); break;
				case 5: gTexAPI.DrawSetTexture(m_pMultiKill5); break;
				case 6: gTexAPI.DrawSetTexture(m_pMultiKill6); break;
				default: gTexAPI.DrawSetTexture(m_pMultiKillMax); break;
			}

			gTexAPI.TextureEnableScissor(0, 0, _wide, 32);
			gTexAPI.DrawTexturedRect(iDrawX, iDrawY + 5, _wide, 32);
		}

		if (item.flags & (KILLFLAG_FIRSTKILL | KILLFLAG_LASTKILL))
		{
			iDrawX -= 78;

			gTexAPI.RenderMode(RenderNormal);

			if (item.flags & KILLFLAG_FIRSTKILL)
				gTexAPI.DrawSetTexture(m_pFirstKill);
			else if (item.flags & KILLFLAG_LASTKILL)
				gTexAPI.DrawSetTexture(m_pLastKill);

			gTexAPI.TextureEnableScissor(0, 0, 78, 32);
			gTexAPI.DrawTexturedRect(iDrawX, iDrawY + 7, 78, 32);
		}

		iDrawY += 32;
	}
}

static wchar_t *hud_GetPlayerName(int index)
{
	wchar_t *name = gHUD.GetPlayerName(index);

	if (name)
		return name;
	else
		return L"";
}

static int hud_GetStringLength(int font, const wchar_t *text)
{
	int wide, tall;
	gFontAPI.GetTextSize(font, text, wide, tall);
	return wide;
}

void CHudKillMsg::Add(int iKillerId, int iVictimId, int iWeaponId, int iMultiKill, int iFlags)
{
	if (m_MsgList[3].active)
	{
		ScrollUp();
	}

	int i;

	for (i = 0; i < 4; i++)
	{
		if (!m_MsgList[i].active)
			break;
	}

	if (i == 0)
	{
		m_flNextScrollUp = ClientTime + 5;
	}

	KillMsg &item = m_MsgList[i];

	memset( &item, 0, sizeof(item) );

	item.active = true;

	wcscpy(item.killer_name, hud_GetPlayerName(iKillerId));
	wcscpy(item.victim_name, hud_GetPlayerName(iVictimId));

	item.killer_length = hud_GetStringLength(gHUD.m_iFontOutline, item.killer_name);
	item.victim_length = hud_GetStringLength(gHUD.m_iFontOutline, item.victim_name);

	if (GameState.m_PlayerTeam[iKillerId - 1] == TEAM_GR)
	{
		item.killer_color.r = 110;
		item.killer_color.g = 177;
		item.killer_color.b = 207;
	}
	else
	{
		item.killer_color.r = 198;
		item.killer_color.g = 140;
		item.killer_color.b = 66;
	}

	if (GameState.m_PlayerTeam[iVictimId - 1] == TEAM_GR)
	{
		item.victim_color.r = 110;
		item.victim_color.g = 177;
		item.victim_color.b = 207;
	}
	else
	{
		item.victim_color.r = 198;
		item.victim_color.g = 140;
		item.victim_color.b = 66;
	}

	WeaponData *pWeapon = WeaponData_GetData(iWeaponId);

	if (pWeapon)
	{
		static char szIconPath[MAX_QPATH];

		sprintf(szIconPath, "Tex/UI/KillMsg/SHOT_WEAPON_%s.dtx", pWeapon->szSmallIconName);

		item.weapon_icon = gTexAPI.LoadDTX(szIconPath);
		// TODO:
		item.weapon_wide = 84;
	}

	if (!item.weapon_icon || !item.weapon_icon->id)
	{
		item.weapon_icon = m_pWeaponSkull;
		item.weapon_wide = 35;
	}

	item.multiKill = iMultiKill;
	item.flags = iFlags;
}

void CHudKillMsg::Clear(void)
{
	memset( &m_MsgList, 0, sizeof(m_MsgList) );
}

void CHudKillMsg::MsgFunc_ResetRound(const char *pszName, int iSize, void *pbuf)
{
	Clear();
}

void CHudKillMsg::MsgFunc_KillMsg(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);

	int iKillerId = READ_BYTE();
	int iVictimId = READ_BYTE();
	int iWeaponId = READ_SHORT();
	int iMultiKill = READ_BYTE();
	int iFlags = READ_SHORT();

	Add(iKillerId, iVictimId, iWeaponId, iMultiKill, iFlags);
}

void CHudKillMsg::ScrollUp(void)
{
	memmove( &m_MsgList, &m_MsgList[1], sizeof(KillMsg) * 3 );
	memset( &m_MsgList[3], 0, sizeof(KillMsg) );
}