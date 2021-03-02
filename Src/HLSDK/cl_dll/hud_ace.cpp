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

void CHudAceIcon::VidInit(void)
{
	m_pAceIcon = gTexAPI.LoadDTX("tex/UI/PlayMain/Ace_Main.dtx");
	m_iType = ACETYPE_NONE;
}

void CHudAceIcon::Redraw(void)
{
	if (!ClientState.bIsAlive)
		return;

	int iBaseX = 106;
	int iBaseY = ScreenTall - 85;

	gTexAPI.RenderMode(RenderNormal);
	gTexAPI.DrawSetColor(255, 255, 255, 255);

	gTexAPI.DrawSetTexture(m_pAceIcon);

	switch (m_iType)
	{
		case 1:
		{
			gTexAPI.TextureEnableScissor(0, 25, 128, 24);
			gTexAPI.DrawTexturedRect(iBaseX, iBaseY, 128, 24);
			break;
		}
		case 2:
		{
			gTexAPI.TextureEnableScissor(0, 0, 128, 24);
			gTexAPI.DrawTexturedRect(iBaseX, iBaseY, 128, 24);
			break;
		}
	}
}

void CHudAceIcon::MsgFunc_AceType(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);

	if (READ_BYTE() != LocalIndex)
		return;

	m_iType = READ_BYTE();

	switch (m_iType)
	{
		case ACETYPE_GRAY:
		{
			HUD_PlaySound("Snd2/WebUISnd/UI_Ace_Inform_Gray.wav", 1.0);
			break;
		}
		case ACETYPE_GOLD:
		{
			HUD_PlaySound("Snd2/WebUISnd/UI_Ace_Inform.wav", 1.0);
			break;
		}
	}
}