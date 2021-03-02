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

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "weapons.h"

LINK_ENTITY_TO_CLASS(weapon_hegrenade, CHEGrenade);

void CHEGrenade::Spawn(void)
{
	pev->classname = MAKE_STRING("weapon_hegrenade");

	Precache();
	CBasePlayerWeapon::Spawn();

	m_iWeaponClass = WeaponClass_HEGrenade;

	m_iClip = m_Info.iAmmoPerMagazine;
}

void CHEGrenade::Precache(void)
{
	// Event for explode ?
}

void CHEGrenade::Deploy(void)
{
	m_bStartThrow = FALSE;
	m_bReleaseThrow = FALSE;
	m_bCanHolster = TRUE;

	DefaultDeploy(m_Info.iszPViewModelFileName, m_Info.iszGViewModelFileName, m_Info.m_AnimSelect.iSequence, m_Info.szGViewAnimName, m_Info.m_AnimSelect.flTime);
}

BOOL CHEGrenade::CanHolster(void)
{
	return m_bCanHolster;
}

void CHEGrenade::Holster(void)
{
	m_bStartThrow = FALSE;
	m_bReleaseThrow = FALSE;
	m_bCanHolster = TRUE;
}

BOOL CHEGrenade::CanDrop(void)
{
	return FALSE;
}

void CHEGrenade::PrimaryAttack(void)
{
	if (m_iClip <= 0)
	{
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.2;
		return;
	}

	if (!m_bStartThrow && m_iClip > 0)
	{
		m_bStartThrow = TRUE;
		m_bReleaseThrow = FALSE;

		SendWeaponAnim(m_Info.m_AnimStartThrow.iSequence);
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + m_Info.m_AnimStartThrow.flTime;
	}
}

void CHEGrenade::WeaponIdle(void)
{
	if (m_flTimeWeaponIdle > UTIL_WeaponTimeBase())
		return;

	if (!m_bReleaseThrow && m_bStartThrow)
		m_bReleaseThrow = TRUE;

	if (m_bStartThrow)
	{
		m_pPlayer->MessageAudio("Fireinthehole_Grenade", FALSE);

		Vector vecForward, vecThrowStart, vecThrowAngle, vecThrowVelocity;

		UTIL_MakeVectors(m_pPlayer->pev->v_angle);
		vecForward = gpGlobals->v_forward;
		vecThrowStart = m_pPlayer->GetGunPosition() + gpGlobals->v_forward * 15 + gpGlobals->v_up * 10;

		vecThrowAngle = m_pPlayer->pev->v_angle;
		vecThrowAngle.x += m_Info.m_GrenadeThrow.flAngle;

		if (vecThrowAngle.x < -85)
			vecThrowAngle.x = -85;

		UTIL_MakeVectors(vecThrowAngle);
		vecThrowVelocity = vecForward + gpGlobals->v_forward * m_Info.m_GrenadeThrow.flVelocity;

		CWorldHEGrenade::Create(m_pPlayer->pev, m_Info.iszModelFileName, vecThrowStart, vecThrowVelocity, m_Info.m_GrenadeThrow.flGravity, m_Info.m_GrenadeThrow.flFriction, m_Info.m_GrenadeThrow.flAirTime, m_Info.iWeaponIndex);

		SendWeaponAnim(m_Info.m_AnimThrow.iSequence);

		m_iClip--;

		m_bStartThrow = FALSE;
		m_bCanHolster = FALSE;

		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + m_Info.m_AnimThrow.flTime;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + m_Info.m_AnimThrow.flTime + 0.1;
	}
	else if (m_bReleaseThrow)
	{
		m_bReleaseThrow = FALSE;
		m_bCanHolster = TRUE;

		if (m_iClip > 0)
		{
			SendWeaponAnim(m_Info.m_AnimSelect.iSequence);

			m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + m_Info.m_AnimSelect.flTime;
			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + m_Info.m_AnimSelect.flTime + 0.1;
		}
		else
		{
			m_pPlayer->RetireWeapon();
			m_pPlayer->RemovePlayerWeapon(this);
			Kill();
		}
	}
}

float CHEGrenade::GetMaxSpeed(void)
{
	return m_Info.flMaxMoveSpeed;
}