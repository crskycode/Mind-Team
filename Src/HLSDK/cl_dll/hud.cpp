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
#include <triangleapi.h>
#include "demo.h"
#include "buffer.h"
#include <demo_api.h>

CHud gHUD;

void CHud::Init()
{
	// Called when client is initailized.

	m_scrinfo.iSize = sizeof(m_scrinfo);
	GetScreenInfo(&m_scrinfo);

	m_flTime = gEngfuncs.GetClientTime();

	m_pCvarHudDraw = CVAR_CREATE("hud_redraw", "1", FCVAR_CLIENTDLL);

	CVAR_CREATE("zoom_sensitivity_ratio", "1.2", FCVAR_CLIENTDLL);

	gFontAPI.AddCustomFontFile("resource/main.ttf");

	m_iFont = gFontAPI.CreateFont();
	gFontAPI.AddGlyphSetToFont(m_iFont, "Main", 12, FW_BOLD, 0, 0, 0);

	m_iFontOutline = gFontAPI.CreateFont();
	gFontAPI.AddGlyphSetToFont(m_iFontOutline, "Main", 12, FW_BOLD, 0, 0, FONTFLAG_OUTLINE);

	m_Character.Init();
	m_Health.Init();
	m_Ammo.Init();
	m_WeaponSlot.Init();
	m_Crosshair.Init();
	m_SniperCrosshair.Init();
	m_Radar.Init();
	m_Location.Init();
	m_KillMsg.Init();
	m_UnderMark.Init();
	m_GoalScore.Init();
	m_MsgBox.Init();
	m_Gauge.Init();
	m_GameTip.Init();
	m_Radio.Init();
	m_ScoreBoard.Init();
	m_Spectate.Init();
	m_Bag.Init();
	m_TacticalMap.Init();
	m_Sensity.Init();
	m_DamageArrow.Init();
	m_SpecMission.Init();
	m_Ace.Init();
	m_Teammate.Init();
	m_KillerMark.Init();
	m_Center.Init();
	m_WinStatus.Init();
	m_Mark.Init();
	m_Flashbang.Init();
}

void CHud::Shutdown(void)
{
	m_Character.Shutdown();
	m_Health.Shutdown();
	m_Ammo.Shutdown();
	m_WeaponSlot.Shutdown();
	m_Crosshair.Shutdown();
	m_SniperCrosshair.Shutdown();
	m_Radar.Shutdown();
	m_Location.Shutdown();
	m_KillMsg.Shutdown();
	m_UnderMark.Shutdown();
	m_GoalScore.Shutdown();
	m_MsgBox.Shutdown();
	m_Gauge.Shutdown();
	m_GameTip.Shutdown();
	m_Radio.Shutdown();
	m_ScoreBoard.Shutdown();
	m_Spectate.Shutdown();
	m_Bag.Shutdown();
	m_TacticalMap.Shutdown();
	m_Sensity.Shutdown();
	m_DamageArrow.Shutdown();
	m_SpecMission.Shutdown();
	m_Ace.Shutdown();
	m_Teammate.Shutdown();
	m_KillerMark.Shutdown();
	m_Center.Shutdown();
	m_WinStatus.Shutdown();
	m_Mark.Shutdown();
	m_Flashbang.Shutdown();
}

void CHud::VidInit(void)
{
	// Called when connected to server.

	m_Character.VidInit();
	m_Health.VidInit();
	m_Ammo.VidInit();
	m_WeaponSlot.VidInit();
	m_Crosshair.VidInit();
	m_SniperCrosshair.VidInit();
	m_Radar.VidInit();
	m_Location.VidInit();
	m_KillMsg.VidInit();
	m_UnderMark.VidInit();
	m_GoalScore.VidInit();
	m_MsgBox.VidInit();
	m_Gauge.VidInit();
	m_GameTip.VidInit();
	m_Radio.VidInit();
	m_ScoreBoard.VidInit();
	m_Spectate.VidInit();
	m_Bag.VidInit();
	m_TacticalMap.VidInit();
	m_Sensity.VidInit();
	m_DamageArrow.VidInit();
	m_SpecMission.VidInit();
	m_Ace.VidInit();
	m_Teammate.VidInit();
	m_KillerMark.VidInit();
	m_Center.VidInit();
	m_WinStatus.VidInit();
	m_Mark.VidInit();
	m_Flashbang.VidInit();
}

void CHud::Reset(void)
{
	// Called when local player is spawned.

	m_Character.Reset();
	m_Health.Reset();
	m_Ammo.Reset();
	m_WeaponSlot.Reset();
	m_Crosshair.Reset();
	m_SniperCrosshair.Reset();
	m_Radar.Reset();
	m_Location.Reset();
	m_KillMsg.Reset();
	m_UnderMark.Reset();
	m_GoalScore.Reset();
	m_MsgBox.Reset();
	m_Gauge.Reset();
	m_GameTip.Reset();
	m_Radio.Reset();
	m_ScoreBoard.Reset();
	m_Spectate.Reset();
	m_Bag.Reset();
	m_TacticalMap.Reset();
	m_Sensity.Reset();
	m_DamageArrow.Reset();
	m_SpecMission.Reset();
	m_Ace.Reset();
	m_Teammate.Reset();
	m_KillerMark.Reset();
	m_Center.Reset();
	m_WinStatus.Reset();
	m_Mark.Reset();
	m_Flashbang.Reset();
}

void CHud::DataInit(void)
{
	// Called when map is loaded.

	m_Character.DataInit();
	m_Health.DataInit();
	m_Ammo.DataInit();
	m_WeaponSlot.DataInit();
	m_Crosshair.DataInit();
	m_SniperCrosshair.DataInit();
	m_Radar.DataInit();
	m_Location.DataInit();
	m_KillMsg.DataInit();
	m_UnderMark.DataInit();
	m_GoalScore.DataInit();
	m_MsgBox.DataInit();
	m_Gauge.DataInit();
	m_GameTip.DataInit();
	m_Radio.DataInit();
	m_ScoreBoard.DataInit();
	m_Spectate.DataInit();
	m_Bag.DataInit();
	m_TacticalMap.DataInit();
	m_Sensity.DataInit();
	m_DamageArrow.DataInit();
	m_SpecMission.DataInit();
	m_Ace.DataInit();
	m_Teammate.DataInit();
	m_KillerMark.DataInit();
	m_Center.DataInit();
	m_WinStatus.DataInit();
	m_Mark.DataInit();
	m_Flashbang.DataInit();
}

void CHud::Think(void)
{
	// Called when client data is updated.

	m_Character.Think();
	m_Health.Think();
	m_Ammo.Think();
	m_WeaponSlot.Think();
	m_Crosshair.Think();
	m_SniperCrosshair.Think();
	m_Radar.Think();
	m_Location.Think();
	m_KillMsg.Think();
	m_UnderMark.Think();
	m_GoalScore.Think();
	m_MsgBox.Think();
	m_Gauge.Think();
	m_GameTip.Think();
	m_Radio.Think();
	m_ScoreBoard.Think();
	m_Spectate.Think();
	m_Bag.Think();
	m_TacticalMap.Think();
	m_Sensity.Think();
	m_DamageArrow.Think();
	m_SpecMission.Think();
	m_Ace.Think();
	m_Teammate.Think();
	m_KillerMark.Think();
	m_Center.Think();
	m_WinStatus.Think();
	m_Mark.Think();
	m_Flashbang.Think();
}

void CHud::Frame(void)
{
	// Update the timer in every frame !
	m_flTime = gEngfuncs.GetClientTime();
}

void CHud::Redraw(void)
{
	// Called when draw viewport.

	if (!m_pCvarHudDraw->value)
		return;

	m_flTime = gEngfuncs.GetClientTime();

	m_Flashbang.Redraw();
	m_SniperCrosshair.Redraw();
	m_DamageArrow.Redraw();
	m_Teammate.Redraw();
	m_KillerMark.Redraw();
	m_Character.Redraw();
	m_Ace.Redraw();
	m_Health.Redraw();
	m_WeaponSlot.Redraw();
	m_Ammo.Redraw();
	m_Crosshair.Redraw();
	m_Radar.Redraw();
	m_Location.Redraw();
	m_KillMsg.Redraw();
	m_UnderMark.Redraw();
	m_GoalScore.Redraw();
	m_MsgBox.Redraw();
	m_Gauge.Redraw();
	m_GameTip.Redraw();
	m_Radio.Redraw();
	m_Spectate.Redraw();
	m_ScoreBoard.Redraw();
	m_Bag.Redraw();
	m_TacticalMap.Redraw();
	m_Sensity.Redraw();
	m_SpecMission.Redraw();
	m_Center.Redraw();
	m_WinStatus.Redraw();
	m_Mark.Redraw();
}

void CHud::Save(CBufferWriter &buf)
{
	m_Character.Save(buf);
}

void CHud::Restore(CBufferReader &buf)
{
	m_Character.Restore(buf);
}

wchar_t *CHud::GetPlayerName(int index)
{
	static wchar_t name[60];

	hud_player_info_t info;
	gEngfuncs.pfnGetPlayerInfo(index, &info);

	if (!info.name)
		return 0;

	wchar_t *s = ANSIToUnicode(info.name);

	wcscpy(name, s);

	return name;
}

int CHud::CalcFadePercen(float m_flStartTime, float flInTime, float flHoldTime, float flOutTime, float &flPrecen)
{
	if (ClientTime < m_flStartTime)
	{
		flPrecen = 0;
		return -2;
	}
	else if (ClientTime > m_flStartTime && ClientTime < m_flStartTime + flInTime)
	{
		flPrecen = (ClientTime - m_flStartTime) / flInTime;
		return -1;
	}
	else if (ClientTime > m_flStartTime + flInTime && ClientTime < m_flStartTime + flInTime + flHoldTime)
	{
		flPrecen = 1;
		return 0;
	}
	else if (ClientTime > m_flStartTime + flInTime + flHoldTime && ClientTime < m_flStartTime + flInTime + flHoldTime + flOutTime)
	{
		flPrecen = (m_flStartTime + flInTime + flHoldTime + flOutTime - ClientTime) / flOutTime;
		return 1;
	}
	else
	{
		flPrecen = 0;
		return 2;
	}
}

bool CHud::Project(float *world, float *screen)
{
	if (gEngfuncs.pTriAPI->WorldToScreen(world, screen))
		return false;

	screen[0] = XPROJECT(screen[0]);
	screen[1] = YPROJECT(screen[1]);

	return true;
}