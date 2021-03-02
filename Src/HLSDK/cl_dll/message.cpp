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
#include "parsemsg.h"
#include "ClientState.h"
#include "GameState.h"
#include "hud.h"
#include "fov.h"
#include "vgui_buysetupdialog.h"

#define HOOK_MESSAGE(name) gEngfuncs.pfnHookUserMsg(#name, MsgFunc_##name);

//-----------------------------------------------------------------
// USER MESSAGE FUNC BEGIN
//-----------------------------------------------------------------

BOOL MsgFunc_TextMsg(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);

	gEngfuncs.Con_Printf(READ_STRING());

	return TRUE;
}

BOOL MsgFunc_SetFOV(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);

	int fov = READ_BYTE();
	float time = READ_SHORT() / 1000.0;

	ClientState.iFOV = fov;
	gFOV.Set(fov, time);

	return TRUE;
}

BOOL MsgFunc_Connected(const char *pszName, int iSize, void *pbuf)
{
	gEngfuncs.pfnServerCmd( "join 1 6\n" );
	return TRUE;
}

BOOL MsgFunc_GameMode(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);

	GameState.iGameMode = READ_BYTE();
	GameState.iSubRuleType = READ_BYTE();

	return TRUE;
}

BOOL MsgFunc_RoundTime(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);

	GameState.iRoundTime = gEngfuncs.GetClientTime() + READ_SHORT();

	return TRUE;
}

BOOL MsgFunc_InitHUD(const char *pszName, int iSize, void *pbuf)
{
	gHUD.DataInit();
	return TRUE;
}

BOOL MsgFunc_ResetHUD(const char *pszName, int iSize, void *pbuf)
{
	gHUD.Reset();
	return TRUE;
}

BOOL MsgFunc_Armor(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);

	ClientState.iArmor = READ_SHORT();

	return TRUE;
}

BOOL MsgFunc_Health(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);

	ClientState.iHealth = READ_SHORT();

	return TRUE;
}

BOOL MsgFunc_TeamInfo(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);

	int num = READ_BYTE();
	int team = READ_BYTE();

	if (num == LocalIndex)
	{
		ClientState.iTeam = team;
	}

	// it's a good time to get the player's name.

	hud_player_info_t pl;
	gEngfuncs.pfnGetPlayerInfo(num, &pl);

	if (pl.name)
	{
		wcscpy( GameState.m_PlayerName[num - 1], ANSIToUnicode(pl.name) );
	}

	GameState.m_PlayerTeam[num - 1] = team;

	return TRUE;
}

BOOL MsgFunc_TeamScore(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);

	GameState.iRoundKill = READ_SHORT();
	GameState.iScoreGR = READ_SHORT();
	GameState.iScoreBL = READ_SHORT();

	return TRUE;
}

BOOL MsgFunc_ScoreAttrib(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);

	int num = READ_BYTE();
	int att = READ_BYTE();

	GameState.m_PlayerAlive[num - 1] = (att & SCOREATTRIB_DEAD) ? FALSE : TRUE;

	return TRUE;
}

BOOL MsgFunc_ScoreInfo(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);

	int num = READ_BYTE();

	GameState.m_PlayerScore[num - 1].iKills = READ_SHORT();
	GameState.m_PlayerScore[num - 1].iDeads = READ_SHORT();

	return TRUE;
}

BOOL MsgFunc_MsgAudio(const char *pszName, int iSize, void *pbuf);

BOOL MsgFunc_MsgBox(const char *pszName, int iSize, void *pbuf)
{
	gHUD.m_MsgBox.MsgFunc_Notify(pszName, iSize, pbuf);
	return TRUE;
}

BOOL MsgFunc_Radio(const char *pszName, int iSize, void *pbuf);

BOOL MsgFunc_Character(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);

	ClientState.iCharacterId = READ_SHORT();

	return TRUE;
}

BOOL MsgFunc_CurWeapon(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);

	ClientState.iWeaponId = READ_SHORT();
	ClientState.iClip = READ_SHORT();
	ClientState.iAmmo = READ_SHORT();

	return TRUE;
}

BOOL MsgFunc_Gauge(const char *pszName, int iSize, void *pbuf)
{
	gHUD.m_Gauge.MsgFunc_Gauge(pszName, iSize, pbuf);
	return TRUE;
}

BOOL MsgFunc_GameTip(const char *pszName, int iSize, void *pbuf)
{
	gHUD.m_GameTip.MsgFunc_GameTip(pszName, iSize, pbuf);
	return TRUE;
}

BOOL MsgFunc_WeapSlot(const char *pszName, int iSize, void *pbuf)
{
	gHUD.m_WeaponSlot.MsgFunc_WeapSlot(pszName, iSize, pbuf);
	return TRUE;
}

BOOL MsgFunc_WeapSlotA(const char *pszName, int iSize, void *pbuf)
{
	gHUD.m_WeaponSlot.MsgFunc_WeapSlotA(pszName, iSize, pbuf);
	return TRUE;
}

BOOL MsgFunc_ClassInfo(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);

	int num = READ_BYTE();

	GameState.m_PlayerClass[num - 1] = READ_BYTE();

	return TRUE;
}

BOOL MsgFunc_KillMsg(const char *pszName, int iSize, void *pbuf)
{
	gHUD.m_KillMsg.MsgFunc_KillMsg(pszName, iSize, pbuf);
	gHUD.m_Mark.MsgFunc_KillMsg(pszName, iSize, pbuf);
	gHUD.m_UnderMark.MsgFunc_KillMsg(pszName, iSize, pbuf);
	return TRUE;
}

BOOL MsgFunc_ResetRound(const char *pszName, int iSize, void *pbuf)
{
	gHUD.m_KillMsg.MsgFunc_ResetRound(pszName, iSize, pbuf);
	gHUD.m_UnderMark.MsgFunc_ResetRound(pszName, iSize, pbuf);
	return TRUE;
}

BOOL MsgFunc_SpecTarget(const char *pszName, int iSize, void *pbuf)
{
	gHUD.m_Spectate.MsgFunc_SpecTarget(pszName, iSize, pbuf);
	return TRUE;
}

BOOL MsgFunc_SpecHealth(const char *pszName, int iSize, void *pbuf)
{
	gHUD.m_Spectate.MsgFunc_SpecHealth(pszName, iSize, pbuf);
	return TRUE;
}

BOOL MsgFunc_SpecWeapon(const char *pszName, int iSize, void *pbuf)
{
	gHUD.m_Spectate.MsgFunc_SpecWeapon(pszName, iSize, pbuf);
	return TRUE;
}

BOOL MsgFunc_EnableBag(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);

	ClientState.bEanbleBag = READ_BYTE();

	if (!ClientState.bEanbleBag)
	{
		if (BuySetupDialog_IsVisible())
		{
			BuySetupDialog_Close();
		}
	}

	gHUD.m_Bag.MsgFunc_EnableBag(pszName, iSize, pbuf);
	return TRUE;
}

BOOL MsgFunc_BagRebuy(const char *pszName, int iSize, void *pbuf);

BOOL MsgFunc_Center(const char *pszName, int iSize, void *pbuf)
{
	gHUD.m_Center.MsgFunc_Center(pszName, iSize, pbuf);
	return TRUE;
}

BOOL MsgFunc_WinStatus(const char *pszName, int iSize, void *pbuf)
{
	gHUD.m_WinStatus.MsgFunc_WinStatus(pszName, iSize, pbuf);
	return TRUE;
}

BOOL MsgFunc_AceType(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);

	int num = READ_BYTE();

	GameState.m_PlayerAceType[num - 1] = READ_BYTE();

	gHUD.m_Ace.MsgFunc_AceType(pszName, iSize, pbuf);

	return TRUE;
}

BOOL MsgFunc_KillerIcon(const char *pszName, int iSize, void *pbuf)
{
	gHUD.m_KillerMark.MsgFunc_KillerIcon(pszName, iSize, pbuf);
	return TRUE;
}

BOOL MsgFunc_DmgArrow(const char *pszName, int iSize, void *pbuf)
{
	gHUD.m_DamageArrow.MsgFunc_DmgArrow(pszName, iSize, pbuf);
	return TRUE;
}

BOOL MsgFunc_GameOver(const char *pszName, int iSize, void *pbuf)
{
//	gEngfuncs.pfnClientCmd("disconnect\n");
	// TODO: Activate The Game Resule UI
	return TRUE;
}

BOOL MsgFunc_Flashbang(const char *pszName, int iSize, void *pbuf)
{
	gHUD.m_Flashbang.MsgFunc_Flashbang(pszName, iSize, pbuf);
	return TRUE;
}

//-----------------------------------------------------------------
// USER MESSAGE FUNC END
//-----------------------------------------------------------------

void MSG_HookUserMsg(void)
{
	HOOK_MESSAGE(Armor);
	HOOK_MESSAGE(Character);
	HOOK_MESSAGE(ClassInfo);
	HOOK_MESSAGE(Connected);
	HOOK_MESSAGE(CurWeapon);
	HOOK_MESSAGE(GameMode);
	HOOK_MESSAGE(GameTip);
	HOOK_MESSAGE(Gauge);
	HOOK_MESSAGE(Health);
	HOOK_MESSAGE(InitHUD);
	HOOK_MESSAGE(MsgAudio);
	HOOK_MESSAGE(MsgBox);
	HOOK_MESSAGE(Radio);
	HOOK_MESSAGE(ResetHUD);
	HOOK_MESSAGE(RoundTime);
	HOOK_MESSAGE(SetFOV);
	HOOK_MESSAGE(TeamInfo);
	HOOK_MESSAGE(TeamScore);
	HOOK_MESSAGE(ScoreAttrib);
	HOOK_MESSAGE(ScoreInfo);
	HOOK_MESSAGE(TextMsg);
	HOOK_MESSAGE(WeapSlot);
	HOOK_MESSAGE(WeapSlotA);
	HOOK_MESSAGE(KillMsg);
	HOOK_MESSAGE(ResetRound);
	HOOK_MESSAGE(SpecTarget);
	HOOK_MESSAGE(SpecHealth);
	HOOK_MESSAGE(SpecWeapon);
	HOOK_MESSAGE(EnableBag);
	HOOK_MESSAGE(BagRebuy);
	HOOK_MESSAGE(Center);
	HOOK_MESSAGE(WinStatus);
	HOOK_MESSAGE(AceType);
	HOOK_MESSAGE(KillerIcon);
	HOOK_MESSAGE(DmgArrow);
	HOOK_MESSAGE(GameOver);
	HOOK_MESSAGE(Flashbang);
}