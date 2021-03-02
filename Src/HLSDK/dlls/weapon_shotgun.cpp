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

LINK_ENTITY_TO_CLASS(weapon_shotgun, CShotgun);

void CShotgun::Spawn(void)
{
	pev->classname = MAKE_STRING("weapon_shotgun");

	Precache();
	CBasePlayerWeapon::Spawn();

	m_iWeaponClass = WeaponClass_Shotgun;

	m_iClip = m_Info.iAmmoPerMagazine;
	m_iAmmo = m_Info.iMaxAmmo;
}

void CShotgun::Precache(void)
{
	m_iShotgunFire = PRECACHE_EVENT(1, "events/shotgunfire.sc");
}

void CShotgun::Deploy(void)
{
	m_fInSpecialReload = FALSE;
	DefaultDeploy(m_Info.iszPViewModelFileName, m_Info.iszGViewModelFileName, m_Info.m_AnimSelect.iSequence, m_Info.szGViewAnimName, m_Info.m_AnimSelect.flTime);
}

void CShotgun::Reload(void)
{
	if (m_iClip >= m_Info.iAmmoPerMagazine || m_iAmmo <= 0)
		return;

	if (m_flNextPrimaryAttack > UTIL_WeaponTimeBase())
		return;

	if (!m_fInSpecialReload)
	{
		m_pPlayer->SetAnimation(PLAYER_RELOAD);
		SendWeaponAnim(m_Info.m_AnimStartReload.iSequence);

		m_fInSpecialReload = 1;
		m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + m_Info.m_AnimStartReload.flTime;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + m_Info.m_AnimStartReload.flTime;
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + m_Info.m_AnimStartReload.flTime;
		m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + m_Info.m_AnimStartReload.flTime;
	}
	else if (m_fInSpecialReload == 1)
	{
		if (m_flTimeWeaponIdle > UTIL_WeaponTimeBase())
			return;

		m_fInSpecialReload = 2;

		SendWeaponAnim(m_Info.m_AnimReload.iSequence);

		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + m_Info.m_AnimReload.flTime;
	}
	else
	{
		m_iClip++;
		m_iAmmo--;
		m_fInSpecialReload = 1;
	}
}

void CShotgun::PrimaryAttack(void)
{
	if (m_pPlayer->pev->velocity.Length2D() > 0)
		ShotgunFire(m_Info.m_AccuracyRunning.flSpreadBase, m_Info.flShotIntervalTime);
	else if (!FBitSet(m_pPlayer->pev->flags, FL_ONGROUND))
		ShotgunFire(m_Info.m_AccuracyJumping.flSpreadBase, m_Info.flShotIntervalTime);
	else if (FBitSet(m_pPlayer->pev->flags, FL_DUCKING))
		ShotgunFire(m_Info.m_AccuracyDucking.flSpreadBase, m_Info.flShotIntervalTime);
	else
		ShotgunFire(m_Info.m_AccuracyDefault.flSpreadBase, m_Info.flShotIntervalTime);
}

void CShotgun::ShotgunFire(float flSpread, float flCycleTime)
{
	if (m_iClip <= 0)
	{
		EMIT_SOUND_DYN(m_pPlayer->edict(), CHAN_WEAPON, "weapon/others/g_nobullet_shotgun.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM);
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.2;
		return;
	}

	m_iClip--;
	m_pPlayer->SetAnimation(PLAYER_ATTACK1);

	SendWeaponAnim(m_Info.m_AnimFire.iSequence);
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + m_Info.m_AnimFire.flTime + 0.1;

	UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle * 2);

	m_pPlayer->FireShotgun(m_Info.m_FireBullets.iShots, m_pPlayer->GetGunPosition(), gpGlobals->v_forward, flSpread, m_Info.m_FireBullets.flDistance, m_Info.m_FireBullets.iDamage, m_Info.m_FireBullets.flRangeModifier, m_pPlayer->pev, m_pPlayer->random_seed);
	PLAYBACK_EVENT_FULL(FEV_NOTHOST, m_pPlayer->edict(), m_iShotgunFire, 0, g_vecZero, g_vecZero, flSpread, 0, (int)(m_pPlayer->pev->punchangle.x * 100), (int)(m_pPlayer->pev->punchangle.y * 100), m_pPlayer->random_seed, m_Info.iWeaponIndex);

	m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + flCycleTime;

	m_fInSpecialReload = FALSE;

	if (m_pPlayer->pev->velocity.Length2D() > 0)
		m_pPlayer->pev->punchangle.x -= m_Info.m_KickBackRunning.flUpBase;
	else if (!FBitSet(m_pPlayer->pev->flags, FL_ONGROUND))
		m_pPlayer->pev->punchangle.x -= m_Info.m_KickBackJumping.flUpBase;
	else if (FBitSet(m_pPlayer->pev->flags, FL_DUCKING))
		m_pPlayer->pev->punchangle.x -= m_Info.m_KickBackDucking.flUpBase;
	else
		m_pPlayer->pev->punchangle.x -= m_Info.m_KickBackDefault.flUpBase;
}

void CShotgun::WeaponIdle(void)
{
	if (m_flTimeWeaponIdle <= UTIL_WeaponTimeBase())
	{
		if (m_iClip <= 0 && !m_fInSpecialReload && m_iAmmo > 0)
		{
			Reload();
		}
		else if (m_fInSpecialReload)
		{
			if (m_iClip < m_Info.iAmmoPerMagazine && m_iAmmo > 0)
			{
				Reload();
			}
			else
			{
				SendWeaponAnim(m_Info.m_AnimEndReload.iSequence);

				m_fInSpecialReload = FALSE;
				m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + m_Info.m_AnimEndReload.flTime;
			}
		}
	}
}

float CShotgun::GetMaxSpeed(void)
{
	return m_Info.flMaxMoveSpeed;
}