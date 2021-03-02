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

void CHudFlashbangEffect::Init(void)
{
}

void CHudFlashbangEffect::Shutdown(void)
{
}

void CHudFlashbangEffect::VidInit(void)
{
}

void CHudFlashbangEffect::Reset(void)
{
}

void CHudFlashbangEffect::Redraw(void)
{
}

void CHudFlashbangEffect::MsgFunc_Flashbang(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);

	m_flFlashTime = READ_BYTE() / 10.0;
	m_flFadeOutTime = READ_BYTE() / 10.0;
	m_flStartTime = ClientTime;
	m_bGrabScreen = true;
}