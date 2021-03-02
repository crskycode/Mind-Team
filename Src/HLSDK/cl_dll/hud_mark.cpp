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
#include "cl_util.h"
#include "hud.h"
#include "AnimationScript.h"
#include "CharacterData.h"

void CHudMark::VidInit(void)
{
	AnimScript_LoadFile("Script/HUDScript/MarkEffect.txt");

	m_iScript = AnimScript_FindScript("MarkEffect");
	m_iBadgeLayout = -1;
	m_iMultiFxLayout = -1;

	if (m_iScript != -1)
	{
		m_iBadgeLayout = AnimScript_FindLayout(m_iScript, "Badge");
		m_iMultiFxLayout = AnimScript_FindLayout(m_iScript, "MultiFx");
	}

	m_pBadgeHeadshot = gTexAPI.LoadPNG("UI/Mark/Badge_Headshot.png");
	m_pBadgeHeadshotGold = gTexAPI.LoadPNG("UI/Mark/Badge_Headshot_Gold.png");
	m_pBadgeHeadshotVVIP = gTexAPI.LoadPNG("UI/Mark/Badge_Headshot_VVIP.png");
	m_pBadgeHeadshotGoldVVIP = gTexAPI.LoadPNG("UI/Mark/Badge_Headshot_Gold_VVIP.png");
	m_pBadgeKnife0 = gTexAPI.LoadPNG("UI/Mark/Badge_Knife.png");
	m_pBadgeKnife1 = gTexAPI.LoadPNG("UI/Mark/Badge_Knife_1.png");
	m_pBadgeKnife2 = gTexAPI.LoadPNG("UI/Mark/Badge_Knife_2.png");
	m_pBadgeKnife3 = gTexAPI.LoadPNG("UI/Mark/Badge_Knife_3.png");
	m_pBadgeGrenade = gTexAPI.LoadPNG("UI/Mark/Badge_Grenade.png");
	m_pBadgeWallshot = gTexAPI.LoadPNG("UI/Mark/Badge_Wallshot.png");
	m_pBadgeHeadWallshot = gTexAPI.LoadPNG("UI/Mark/Badge_Headwallshot.png");
	m_pBadgeHeadWallshotGold = gTexAPI.LoadPNG("UI/Mark/Badge_Headwallshot_Gold.png");
	m_pBadgeMulti1 = gTexAPI.LoadPNG("UI/Mark/Badge_Multi1.png");
	m_pBadgeMulti2 = gTexAPI.LoadPNG("UI/Mark/Badge_Multi2.png");
	m_pBadgeMulti3 = gTexAPI.LoadPNG("UI/Mark/Badge_Multi3.png");
	m_pBadgeMulti4 = gTexAPI.LoadPNG("UI/Mark/Badge_Multi4.png");
	m_pBadgeMulti5 = gTexAPI.LoadPNG("UI/Mark/Badge_Multi5.png");
	m_pBadgeMulti6 = gTexAPI.LoadPNG("UI/Mark/Badge_Multi6.png");
	m_pMulti2Fx = gTexAPI.LoadPNG("UI/Mark/Multi2_Fx.png");
	m_pMulti3Fx = gTexAPI.LoadPNG("UI/Mark/Multi3_Fx.png");
	m_pMulti4Fx = gTexAPI.LoadPNG("UI/Mark/Multi4_Fx.png");
	m_pMulti5Fx = gTexAPI.LoadPNG("UI/Mark/Multi5_Fx.png");
	m_pMulti6Fx = gTexAPI.LoadPNG("UI/Mark/Multi6_Fx.png");
	m_pUpgrade1 = gTexAPI.LoadPNG("UI/Mark/KillMark_Upgrade1.png");
	m_pUpgrade2 = gTexAPI.LoadPNG("UI/Mark/KillMark_Upgrade2.png");
	m_pUpgrade3 = gTexAPI.LoadPNG("UI/Mark/KillMark_Upgrade3.png");
	m_pBadgeExAssault1 = gTexAPI.LoadPNG("UI/Mark/BadgeEx/badge_Assault1.png");
	m_pBadgeExAssault2 = gTexAPI.LoadPNG("UI/Mark/BadgeEx/badge_Assault2.png");
	m_pBadgeExAssault3 = gTexAPI.LoadPNG("UI/Mark/BadgeEx/badge_Assault3.png");
	m_pBadgeExElite1 = gTexAPI.LoadPNG("UI/Mark/BadgeEx/badge_Elite1.png");
	m_pBadgeExElite2 = gTexAPI.LoadPNG("UI/Mark/BadgeEx/badge_Elite2.png");
	m_pBadgeExElite3 = gTexAPI.LoadPNG("UI/Mark/BadgeEx/badge_Elite3.png");
	m_pBadgeExKnife1 = gTexAPI.LoadPNG("UI/Mark/BadgeEx/badge_Knife1.png");
	m_pBadgeExKnife2 = gTexAPI.LoadPNG("UI/Mark/BadgeEx/badge_Knife2.png");
	m_pBadgeExKnife3 = gTexAPI.LoadPNG("UI/Mark/BadgeEx/badge_Knife3.png");
	m_pBadgeExScout1 = gTexAPI.LoadPNG("UI/Mark/BadgeEx/badge_Scout1.png");
	m_pBadgeExScout2 = gTexAPI.LoadPNG("UI/Mark/BadgeEx/badge_Scout2.png");
	m_pBadgeExScout3 = gTexAPI.LoadPNG("UI/Mark/BadgeEx/badge_Scout3.png");
	m_pBadgeExSniper1 = gTexAPI.LoadPNG("UI/Mark/BadgeEx/badge_Sniper1.png");
	m_pBadgeExSniper2 = gTexAPI.LoadPNG("UI/Mark/BadgeEx/badge_Sniper2.png");
	m_pBadgeExSniper3 = gTexAPI.LoadPNG("UI/Mark/BadgeEx/badge_Sniper3.png");

	m_flStartTime = 0;

	m_pBadgeMain = NULL;
	m_pBadgeEx = NULL;
	m_pBadgeFx = NULL;
	m_pBadgeUp = NULL;
}

void CHudMark::Redraw(void)
{
	if (!m_flStartTime)
		return;

	if (m_iBadgeLayout == -1 || m_iMultiFxLayout == -1)
		return;

	int iBaseX = ScreenWide / 2;
	int iBaseY = ScreenTall - 154;


	if (m_pBadgeMain)
	{
		AnimData *pAnim = AnimScript_GetData(m_iScript, m_iBadgeLayout, (int)((ClientTime - m_flStartTime) * 1000));

		if (pAnim && !pAnim->bLastFrame)
		{
			int wide, tall;

			gTexAPI.DrawSetColor(255, 255, 255, pAnim->fAlpha * 255);

			wide = m_pBadgeMain->wide * pAnim->vScale[0];
			tall = m_pBadgeMain->tall * pAnim->vScale[1];

			gTexAPI.RenderMode(RenderAdditive);
			gTexAPI.DrawSetTexture(m_pBadgeMain);
			gTexAPI.DrawTexturedRect(iBaseX + (pAnim->vOrigin[0] - wide / 2), iBaseY + (pAnim->vOrigin[1] - tall / 2), wide, tall);

			if (m_pBadgeEx)
			{
				wide = m_pBadgeEx->wide * pAnim->vScale[0];
				tall = m_pBadgeEx->tall * pAnim->vScale[1];

				gTexAPI.RenderMode(RenderNormal);
				gTexAPI.DrawSetTexture(m_pBadgeEx);
				gTexAPI.DrawTexturedRect(iBaseX + (pAnim->vOrigin[0] - wide / 2), iBaseY + (pAnim->vOrigin[1] - tall / 2), wide, tall);
			}

			if (m_pBadgeUp)
			{
				wide = m_pBadgeUp->wide * pAnim->vScale[0];
				tall = m_pBadgeUp->tall * pAnim->vScale[1];

				gTexAPI.RenderMode(RenderAdditive);
				gTexAPI.DrawSetTexture(m_pBadgeUp);
				gTexAPI.DrawTexturedRect(iBaseX + (pAnim->vOrigin[0] - wide / 2), iBaseY + (pAnim->vOrigin[1] - tall / 2), wide, tall);
			}
		}
		else
		{
			m_flStartTime = 0;
			return;
		}
	}

	if (m_pBadgeFx)
	{
		AnimData *pAnim = AnimScript_GetData(m_iScript, m_iMultiFxLayout, (int)((ClientTime - m_flStartTime) * 1000));

		if (pAnim && !pAnim->bLastFrame)
		{
			int wide = m_pBadgeFx->wide * pAnim->vScale[0];
			int tall = m_pBadgeFx->tall * pAnim->vScale[1];

			gTexAPI.RenderMode(RenderAdditive);
			gTexAPI.DrawSetColor(255, 255, 255, pAnim->fAlpha * 255);
			gTexAPI.DrawSetTexture(m_pBadgeFx);
			gTexAPI.DrawTexturedRect(iBaseX + (pAnim->vOrigin[0] - wide / 2), iBaseY + (pAnim->vOrigin[1] - tall / 2), wide, tall);
		}
	}
}

static char * _MakeSoundPath(char *language, char *team, char *sentence)
{
	static char path[MAX_QPATH];
	_snprintf(path, sizeof(path), "Snd2/%s/Message/%s_%s.wav", language, sentence, team);
	return path;
}

static void _PlaySound(char *sentence)
{
	char *language = "english";
	char *team = "GR";

	CharacterData *pCharacter = CharacterData_GetData(ClientState.iCharacterId);

	if (pCharacter)
	{
		if (pCharacter->bIsChinese)
		{
			if (pCharacter->bIsWoman)
				language = "chineseWoman";
			else
				language = "chinese";
		}
		else
		{
			if (pCharacter->bIsWoman)
				language = "woman";
			else
				language = "english";
		}
	}

	if (ClientState.iTeam == TEAM_GR)
		team = "GR";
	else
		team = "BL";

	HUD_PlaySound(_MakeSoundPath(language, team, sentence), 1);
}

void CHudMark::MsgFunc_KillMsg(const char *pszName, int iSize, void *pbuf)
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

	m_pBadgeMain = NULL;
	m_pBadgeEx = NULL;
	m_pBadgeFx = NULL;
	m_pBadgeUp = NULL;

	HUD_PlaySound("Common/UI_SpecialKill2.wav", 1);

	bool bShowExtra = true;

	switch (iMultiKill)
	{
		case 1:
		{
			if (iFlags & KILLFLAG_HEADSHOT)
			{
				if (iFlags & KILLFLAG_WALLSHOT)
				{
					if (iFlags & KILLFLAG_HEADSHOT_GOLD)
						m_pBadgeMain = m_pBadgeHeadWallshotGold;
					else
						m_pBadgeMain = m_pBadgeHeadWallshot;
				}
				else
				{
					if (iFlags & KILLFLAG_HEADSHOT_GOLD)
						m_pBadgeMain = m_pBadgeHeadshotGold;
					else
						m_pBadgeMain = m_pBadgeHeadshot;
				}

				bShowExtra = false;

				_PlaySound("Headshot");
			}
			else
			{
				if (iFlags & KILLFLAG_KNIFE)
				{
					m_pBadgeMain = m_pBadgeKnife0;
					bShowExtra = false;
				}
				else if (iFlags & KILLFLAG_GRENADE)
				{
					m_pBadgeMain = m_pBadgeGrenade;
					bShowExtra = false;
				}
				else if (iFlags & KILLFLAG_WALLSHOT)
				{
					m_pBadgeMain = m_pBadgeWallshot;
					bShowExtra = false;
				}
				else
				{
					m_pBadgeMain = m_pBadgeMulti1;
				}
			}

			break;
		}
		case 2:
		{
			m_pBadgeMain = m_pBadgeMulti2;
			m_pBadgeFx = m_pMulti2Fx;
			_PlaySound("MultiKill_2");
			break;
		}
		case 3:
		{
			m_pBadgeMain = m_pBadgeMulti3;
			m_pBadgeFx = m_pMulti3Fx;
			_PlaySound("MultiKill_3");
			break;
		}
		case 4:
		{
			m_pBadgeMain = m_pBadgeMulti4;
			m_pBadgeFx = m_pMulti4Fx;
			_PlaySound("MultiKill_4");
			break;
		}
		case 5:
		{
			m_pBadgeMain = m_pBadgeMulti5;
			m_pBadgeFx = m_pMulti5Fx;
			_PlaySound("MultiKill_5");
			break;
		}
		case 6:
		{
			m_pBadgeMain = m_pBadgeMulti6;
			m_pBadgeFx = m_pMulti6Fx;
			_PlaySound("MultiKill_6");
			break;
		}
		case 7:
		{
			m_pBadgeMain = m_pBadgeMulti6;
			m_pBadgeFx = m_pMulti6Fx;
			_PlaySound("MultiKill_7");
			break;
		}
		case 8:
		{
			m_pBadgeMain = m_pBadgeMulti6;
			m_pBadgeFx = m_pMulti6Fx;
			_PlaySound("MultiKill_8");
			break;
		}
		default:
		{
			if (iMultiKill > 8)
			{
				m_pBadgeMain = m_pBadgeMulti6;
				m_pBadgeFx = m_pMulti6Fx;
			}

			break;
		}
	}

	if (bShowExtra)
	{
		m_pBadgeEx = m_pBadgeExElite3;
		m_pBadgeUp = m_pUpgrade3;
	}

	m_flStartTime = ClientTime;
}