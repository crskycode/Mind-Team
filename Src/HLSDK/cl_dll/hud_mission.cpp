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

void CHudMissionLeft::VidInit(void)
{
	m_pAltKey = gTexAPI.LoadDTX("tex/UI/PlayMain/Mission/Mission_AltKey.dtx");
	m_pMissionBG = gTexAPI.LoadDTX("tex/UI/PlayMain/Mission/SpecialMissionBG.dtx");

	m_flInTime = 0;
	m_flOutTime = 0;
}

void CHudMissionLeft::Redraw(void)
{
	gTexAPI.RenderMode(RenderNormal);
	gTexAPI.DrawSetColor(255, 255, 255, 150);

	int iBaseX = 0;
	int iBaseY = 178;

	if (!m_flInTime && !m_flOutTime)
		gTexAPI.DrawSetColor(255, 255, 255, 150);
	else
		gTexAPI.DrawSetColor(255, 255, 255, 255);

	gTexAPI.DrawSetTexture(m_pAltKey);
	gTexAPI.DrawTexturedRect(iBaseX, iBaseY, m_pAltKey->wide, m_pAltKey->tall);

	if (!m_flInTime && !m_flOutTime)
		return;

	const float InTime = 0.18;
	const float OutTime = 0.18;

	int offset = -256;
	int alpha = 0;

	if (m_flInTime)
	{
		if (ClientTime < m_flInTime)
		{
			offset = -256;
			alpha = 0;
		}
		else if (ClientTime > m_flInTime && ClientTime < m_flInTime + InTime)
		{
			offset = -256 + (ClientTime - m_flInTime) / InTime * 256;
			alpha = (ClientTime - m_flInTime) / InTime * 255;
		}
		else if (ClientTime > m_flInTime + InTime)
		{
			offset = 0;
			alpha = 255;

			// hold to display >_<
		}
	}
	else if (m_flOutTime)
	{
		if (ClientTime < m_flOutTime)
		{
			offset = 0;
			alpha = 255;
		}
		else if (ClientTime > m_flOutTime && ClientTime < m_flOutTime + OutTime)
		{
			offset = -( (ClientTime - m_flOutTime) / OutTime * 256 );
			alpha = -( (ClientTime - m_flOutTime) / OutTime * 255 );
		}
		else if (ClientTime > m_flOutTime + OutTime)
		{
			offset = -256;
			alpha = 0;

			// hide it
			m_flInTime = 0;
			m_flOutTime = 0;

			return;
		}
	}

	gTexAPI.DrawSetColor(255, 255, 255, alpha);
	gTexAPI.DrawSetTexture(m_pMissionBG);
	gTexAPI.DrawTexturedRect(iBaseX + offset, iBaseY + 48, m_pMissionBG->wide, m_pMissionBG->tall);
}

bool CHudMissionLeft::IsVisible(void)
{
	return m_flInTime != 0;
}

void CHudMissionLeft::Show(void)
{
	m_flInTime = ClientTime;
	m_flOutTime = 0;

	HUD_PlaySound("Snd2/WebUISnd/Mission_Open.wav", 1.0);
}

void CHudMissionLeft::Hide(void)
{
	m_flOutTime = ClientTime;
	m_flInTime = 0;

	HUD_PlaySound("Snd2/WebUISnd/Mission_Close.wav", 1.0);
}