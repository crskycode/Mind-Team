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
#include "player.h"
#include "weapons.h"

//void CRifle::Spawn(void)
//{
//	Precache();
//	CBasePlayerWeapon::Spawn();
//}
//
//void CRifle::Precache(void)
//{
//	m_usRifle = PRECACHE_EVENT(1, "events/rifle.sc");
//}
//
//void CRifle::Deploy(void)
//{
//	DefaultDeploy(m_Info.animSelect.iSequence, m_Info.animSelect.flTime);
//}
//
//void CRifle::Reload(void)
//{
//	DefaultReload(m_Info.iMaxClip, m_Info.animReload.iSequence, m_Info.animReload.flTime);
//}
//
//void CRifle::PrimaryAttack(void)
//{
//	if (m_iClip <= 0)
//	{
//		m_flNextPrimaryAttack = 0.2;
//		return;
//	}
//
//	//m_iClip--;
//	//m_pPlayer->SetAnimation(PLAYER_ATTACK1);
//
//	SendWeaponAnim(m_Info.animPrefire.iSequence);
//
//	PLAYBACK_EVENT_FULL(0, NULL, m_usRifle, 0, (float *)&m_pPlayer->GetGunPosition(), (float *)&m_pPlayer->pev.angles, 0, 0, 0, 0, 0, 0);
//
//	m_flNextPrimaryAttack = m_Info.flCycleTime;
//	m_flTimeWeaponIdle = m_Info.animPrefire.flTime + 0.02;
//}
//
//void CRifle::SecondaryAttack(void)
//{
//}
//
//void CRifle::WeaponIdle(void)
//{
//	if (m_flTimeWeaponIdle <= 0)
//	{
//		SendWeaponAnim(m_Info.animIdle.iSequence);
//		m_flTimeWeaponIdle = 60;
//	}
//}