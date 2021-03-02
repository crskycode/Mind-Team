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
#include "weapons.h"
#include "player.h"

#include <usercmd.h>
#include <in_buttons.h>

#include "ClientState.h"
#include "GameState.h"

#include "WeaponData.h"
#include "CharacterData.h"

cvar_t *cl_lw = NULL;
cvar_t *cl_viewbody = NULL;
cvar_t *cl_viewfov = NULL;

CBasePlayer g_Player;

//CKnife g_Knife;
//CRifle g_Rifle;

void Cmd_Init( const char *text );
int Cmd_Argc( void );
char *Cmd_Argv( int i );

extern "C"
{
DLLExport void HUD_PostRunCmd(struct local_state_s *from, struct local_state_s *to, struct usercmd_s *cmd, int runfuncs, double time, unsigned int random_seed);
}

////----------------------------------------------------------------
//// LOCAL PLAYER ENTITY
////----------------------------------------------------------------
//
CBasePlayer::CBasePlayer()
{
	memset(&pev, 0, sizeof(pev));

	random_seed = 0;

	m_afButtonLast = 0;
	m_afButtonPressed = 0;
	m_afButtonReleased = 0;
	m_pActiveWeapon = 0;
	m_iWeaponEntityIndex = 0;
	m_iWeaponEntityClass = 0;
	m_iWeaponIndex = 0;
	m_flNextAttack = 0;
}
//
//BOOL CBasePlayer::IsAlive(void)
//{
//	return gClient.alive && pev.health > 0 && pev.deadflag == DEAD_NO;
//}
//
//void CBasePlayer::Spawn(void)
//{
//}
//
//void CBasePlayer::Killed(void)
//{
//}
//
//void CBasePlayer::ItemPostFrame(void)
//{
//	if (m_flNextAttack > 0)
//		return;
//
//	if (m_pActiveWeapon)
//		m_pActiveWeapon->ItemPostFrame();
//}
//
//Vector CBasePlayer::GetGunPosition(void)
//{
//	return Vector(pev.origin) + Vector(pev.view_ofs);
//}
//
////----------------------------------------------------------------
//// LOCAL WEAPONS
////----------------------------------------------------------------
//
//void CL_Weapon_GetInfo(const char **name, const char **icon, int *entity, int *clip, int *ammo)
//{
//	*name = NULL;
//	*icon = NULL;
//
//	if (!g_Player.m_pActiveWeapon)
//		return;
//
//	*name = g_Player.m_pActiveWeapon->m_Info.szName;
//
//	*entity = g_Player.m_iWeaponEntityType;
//	*clip = g_Player.m_pActiveWeapon->m_iClip;
//	*ammo = g_Player.m_pActiveWeapon->m_iAmmo;
//}
//
//void CBasePlayerWeapon::Spawn(void)
//{
//}
//
//void CBasePlayerWeapon::Deploy(void)
//{
//}
//
//void CBasePlayerWeapon::Holster(void)
//{
//}
//
//BOOL CBasePlayerWeapon::DefaultDeploy(int iAnim, float flDelay)
//{
//	m_flNextPrimaryAttack = -1;
//	m_flNextSecondaryAttack = -1;
//
//	m_pPlayer->m_flNextAttack = flDelay;
//	m_flTimeWeaponIdle = flDelay + 0.02;
//
//	SendWeaponAnim(iAnim);
//
//	return TRUE;
//}
//
//BOOL CBasePlayerWeapon::DefaultReload(int iClipSize, int iAnim, float flDelay)
//{
//	if (m_fInReload)
//		return FALSE;
//
//	if (m_iAmmo <= 0)
//		return FALSE;
//
//	int j = min(iClipSize - m_iClip, m_iAmmo);
//
//	if (j <= 0)
//		return FALSE;
//
//	SendWeaponAnim(iAnim);
//
//	m_pPlayer->m_flNextAttack = flDelay;	// Waiting for ammo change from server
//	m_flTimeWeaponIdle = flDelay + 0.02;
//
//	m_fInReload = TRUE;
//
//	return TRUE;
//}
//
//void CBasePlayerWeapon::ItemPostFrame(void)
//{
//	if (m_fInReload && m_pPlayer->m_flNextAttack <= 0)
//	{
//		m_iClip += 1;
//		m_fInReload = FALSE;
//	}
//
//	if ((m_pPlayer->pev.button & IN_ATTACK2) && m_flNextSecondaryAttack <= 0)
//	{
//		SecondaryAttack();
//	}
//	else if ((m_pPlayer->pev.button & IN_ATTACK) && m_flNextPrimaryAttack <= 0)
//	{
//		PrimaryAttack();
//	}
//	else if ((m_pPlayer->pev.button & IN_RELOAD) && !m_fInReload)
//	{
//		Reload();
//	}
//	else if (!(m_pPlayer->pev.button & (IN_ATTACK | IN_ATTACK2)))
//	{
//		if (m_iClip <= 0 && m_iAmmo > 0)
//		{
//			Reload();
//			return;
//		}
//
//		WeaponIdle();
//	}
//}
//
//void CBasePlayerWeapon::SendWeaponAnim(int iAnim)
//{
//	gEngfuncs.pfnWeaponAnim(iAnim, 0);
//}
//
//void CBasePlayerWeapon::PrimaryAttack(void)
//{
//}
//
//void CBasePlayerWeapon::SecondaryAttack(void)
//{
//}
//
//void CBasePlayerWeapon::Reload(void)
//{
//}
//
//void CBasePlayerWeapon::WeaponIdle(void)
//{
//}
//
void HUD_InitClientWeapons(void)
{
	static bool initialized = false;

	if (initialized)
		return;

	initialized = true;

	cl_lw = gEngfuncs.pfnGetCvarPointer("cl_lw");
	cl_viewbody = gEngfuncs.pfnGetCvarPointer("cl_viewbody");
	cl_viewfov = gEngfuncs.pfnGetCvarPointer("cl_viewfov");
}
//
//static float W_GetFrameTime(void)
//{
//	float frametime;
//
//	static double lasttime = 0;
//	double curtime = gEngfuncs.GetClientTime();
//	
//	frametime = (float)(curtime - lasttime);
//
//	if (frametime < 0)
//		frametime = 0;
//
//	lasttime = curtime;
//
//	return frametime;
//}

void HUD_WeaponsPostThink(local_state_s *from, local_state_s *to, usercmd_t *cmd, double time, unsigned int random_seed)
{
	//// Calculate the frame time
	//float frametime = W_GetFrameTime();

	//// For random weapon events, use this seed to seed random # generator
	//g_Player.random_seed = random_seed;

	//// Get old buttons from previous state.
	//g_Player.m_afButtonLast = from->playerstate.oldbuttons;

	//// Which buttsons chave changed
	//int buttonsChanged = (g_Player.m_afButtonLast ^ cmd->buttons);

	//// Debounced button codes for pressed/released
	//// The changed ones still down are "pressed"
	//g_Player.m_afButtonPressed = buttonsChanged & cmd->buttons;
	//// The ones not down are "released"
	//g_Player.m_afButtonReleased = buttonsChanged & (~cmd->buttons);

	//// Set player variables that weapons code might check/alter
	//g_Player.pev.button = cmd->buttons;

	//VectorCopy(from->client.origin, g_Player.pev.origin);
	//VectorCopy(from->client.velocity, g_Player.pev.velocity);

	//g_Player.pev.flags = from->client.flags;
	//g_Player.pev.deadflag = from->client.deadflag;
	//g_Player.pev.health = from->client.health;
	//g_Player.pev.waterlevel = from->client.waterlevel;
	//g_Player.pev.maxspeed = from->client.maxspeed;
	//g_Player.pev.fov = from->client.fov;
	//g_Player.pev.weaponanim = from->client.weaponanim;
	//g_Player.pev.viewmodel = from->client.viewmodel;

	////ammo_shells As EntityType
	////ammo_nails As WeaponID
	////ammo_cells As Clip
	////ammo_rockets As Ammo

	//if (from->client.m_iId != g_Player.m_iWeaponEntityIndex)
	//{
	//	g_Player.m_iWeaponEntityIndex = from->client.m_iId;
	//	g_Player.m_iWeaponEntityType = from->client.ammo_shells;
	//	g_Player.m_iWeaponIndex = from->client.ammo_nails;

	//	if (g_Player.m_pActiveWeapon)
	//		g_Player.m_pActiveWeapon->Holster();

	//	if (g_Player.m_iWeaponEntityIndex)
	//	{
	//		switch (g_Player.m_iWeaponEntityType)
	//		{
	//			case WeaponEntityType_RIFLE:
	//			{
	//				g_Player.m_pActiveWeapon = &g_Rifle;
	//				break;
	//			}
	//			case WeaponEntityType_PISTOL:
	//			{
	//				g_Player.m_pActiveWeapon = NULL;
	//				break;
	//			}
	//			case WeaponEntityType_KNIFE:
	//			{
	//				g_Player.m_pActiveWeapon = &g_Knife;
	//				break;
	//			}
	//			case WeaponEntityType_GRENADE:
	//			{
	//				g_Player.m_pActiveWeapon = NULL;
	//				break;
	//			}

	//			default:
	//			{
	//				g_Player.m_pActiveWeapon = NULL;
	//				gEngfuncs.Con_Printf("Invalid weapon entity type !\n");
	//				break;
	//			}
	//		}

	//		if (g_Player.m_pActiveWeapon)
	//		{
	//			if (g_Player.m_iWeaponIndex)
	//			{
	//				WeaponInfo *info = W_FindData(g_Player.m_iWeaponIndex);

	//				if (info)
	//				{
	//					memcpy(&g_Player.m_pActiveWeapon->m_Info, info, sizeof(WeaponInfo));
	//					g_Player.m_pActiveWeapon->Spawn();
	//				}
	//				else
	//				{
	//					g_Player.m_pActiveWeapon = NULL;
	//					gEngfuncs.Con_Printf("Invalid weapon index !\n");
	//				}
	//			}
	//		}

	//		if (g_Player.m_pActiveWeapon)
	//		{
	//			g_Player.m_pActiveWeapon->m_pPlayer = &g_Player;
	//			g_Player.m_pActiveWeapon->Deploy();
	//		}
	//	}
	//	else
	//	{
	//		g_Player.m_pActiveWeapon = NULL;
	//	}
	//}

	//if (g_Player.m_pActiveWeapon)
	//{
	//	CBasePlayerWeapon *weapon = (CBasePlayerWeapon *)g_Player.m_pActiveWeapon;

	//	if (weapon)
	//	{
	//		weapon->m_iClip = from->client.ammo_cells;
	//		weapon->m_iAmmo = from->client.ammo_rockets;

	//		weapon->m_flNextPrimaryAttack = max(weapon->m_flNextPrimaryAttack - frametime, -1.0);
	//		weapon->m_flNextSecondaryAttack = max(weapon->m_flNextSecondaryAttack - frametime, -0.001);
	//		weapon->m_flTimeWeaponIdle = max(weapon->m_flTimeWeaponIdle - frametime, -0.001);
	//	}
	//}

	//g_Player.m_flNextAttackServer = from->client.m_flNextAttack;

	//g_Player.m_flNextAttack = max(g_Player.m_flNextAttack - frametime, -0.001);

	//if (g_Player.IsAlive())
	//{
	//	g_Player.ItemPostFrame();
	//}
}

void HUD_WeaponsPostThink(void)
{
}

DLLExport void HUD_PostRunCmd(struct local_state_s *from, struct local_state_s *to, struct usercmd_s *cmd, int runfuncs, double time, unsigned int random_seed)
{
	HUD_InitClientWeapons();

	ClientState.iRandomSeed = random_seed;

	// 区分预测和不预测的数据更新函数以免代码混乱
	if (cl_lw && cl_lw->value)
	{
		HUD_WeaponsPostThink(from, to, cmd, time, random_seed);
	}
	else
	{
		HUD_WeaponsPostThink();
	}
}