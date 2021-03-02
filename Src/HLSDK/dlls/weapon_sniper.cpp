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

LINK_ENTITY_TO_CLASS(weapon_sniper, CSniper);

void CSniper::Spawn(void)
{
	pev->classname = MAKE_STRING("weapon_sniper");

	Precache();
	CBasePlayerWeapon::Spawn();

	m_iWeaponClass = WeaponClass_Sniper;

	m_iClip = m_Info.iAmmoPerMagazine;
	m_iAmmo = m_Info.iMaxAmmo;
}

void CSniper::Precache(void)
{
	m_iSniperFire = PRECACHE_EVENT(1, "events/sniperfire.sc");
}

void CSniper::Deploy(void)
{
	m_fInReload = FALSE;
	m_bResumeZoom = FALSE;
	DefaultDeploy(m_Info.iszPViewModelFileName, m_Info.iszGViewModelFileName, m_Info.m_AnimSelect.iSequence, m_Info.szGViewAnimName, m_Info.m_AnimSelect.flTime);
}

void CSniper::Holster(void)
{
	m_pPlayer->SetFOV(90, 0.05);
	m_bResumeZoom = FALSE;
	CBasePlayerWeapon::Holster();
}

void CSniper::Reload(void)
{
	if (DefaultReload(m_Info.iAmmoPerMagazine, m_Info.m_AnimReload.iSequence, m_Info.m_AnimReload.flTime))
	{
		m_pPlayer->SetAnimation(PLAYER_RELOAD);

		m_flNextPrimaryAttack = m_flNextSecondaryAttack = -1;

		if (m_pPlayer->GetFOV() != 90)
		{
			m_bResumeZoom = FALSE;
			m_pPlayer->SetFOV(90, 0.05);
			m_pPlayer->ResetMaxSpeed();
		}
	}
}

void CSniper::PrimaryAttack(void)
{
	if (m_pPlayer->pev->velocity.Length2D() > 140)
		SniperFire(m_Info.m_AccuracyRunning.flSpreadBase, m_Info.flShotIntervalTime);
	else if (!FBitSet(m_pPlayer->pev->flags, FL_ONGROUND))
		SniperFire(m_Info.m_AccuracyJumping.flSpreadBase, m_Info.flShotIntervalTime);
	else if (FBitSet(m_pPlayer->pev->flags, FL_DUCKING))
		SniperFire(m_Info.m_AccuracyDucking.flSpreadBase, m_Info.flShotIntervalTime);
	else
		SniperFire(m_Info.m_AccuracyDefault.flSpreadBase, m_Info.flShotIntervalTime);
}

void CSniper::SingleSecondaryAttack(void)
{
	if (m_fInReload)
		return;

	if (m_flNextPrimaryAttack > 0)
		return;

	switch (m_pPlayer->GetFOV())
	{
		case 90: m_pPlayer->SetFOV(40, 0.06); break;
		case 40: m_pPlayer->SetFOV(20, 0.06);; break;
		case 20: m_pPlayer->SetFOV(90, 0.06);; break;
	}

	EMIT_SOUND_DYN(m_pPlayer->edict(), CHAN_WEAPON, "weapon/others/g_zoom.wav", 1.0, ATTN_NORM, 0, PITCH_NORM);

	m_pPlayer->ResetMaxSpeed();

	m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.1;
}

void CSniper::SniperFire(float flSpread, float flCycleTime)
{
	if (m_iClip <= 0)
	{
		EMIT_SOUND_DYN(m_pPlayer->edict(), CHAN_WEAPON, "weapon/others/g_nobullet_rifle.wav", 1.0, ATTN_NORM, 0, PITCH_LOW);
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.2;
		return;
	}

	if (m_pPlayer->GetFOV() != 90)
	{
		m_bResumeZoom = TRUE;
		m_iLastZoom = m_pPlayer->GetFOV();
		m_pPlayer->SetFOV(90, 0.05);
		m_pPlayer->ResetMaxSpeed();
	}

	m_iClip--;
	m_pPlayer->SetAnimation(PLAYER_ATTACK1);

	SendWeaponAnim(m_Info.m_AnimFire.iSequence);
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + m_Info.m_AnimFire.flTime + 0.1;

	UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle + m_pPlayer->pev->punchangle);

	Vector vecDir = m_pPlayer->FireBullets(
		m_pPlayer->GetGunPosition(),
		gpGlobals->v_forward,
		flSpread,
		8192,
		m_Info.m_FireBullets.iPenetration,
		m_Info.m_FireBullets.iPenetrationPower,
		m_Info.m_FireBullets.iPenetrationDistance,
		m_Info.m_FireBullets.iDamage,
		m_Info.m_FireBullets.flRangeModifier,
		m_pPlayer->pev,
		m_pPlayer->random_seed);

	PLAYBACK_EVENT_FULL(
		FEV_NOTHOST,
		m_pPlayer->edict(),
		m_iSniperFire,
		0,
		g_vecZero,
		g_vecZero,
		vecDir.x,
		vecDir.y,
		(int)(m_pPlayer->pev->punchangle.x * 100),
		(int)(m_pPlayer->pev->punchangle.y * 100),
		m_pPlayer->random_seed,
		m_Info.iWeaponIndex);

	m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + flCycleTime;

	if (m_pPlayer->pev->velocity.Length2D() > 0)
		m_pPlayer->pev->punchangle.x -= m_Info.m_KickBackRunning.flUpBase;
	else if (!FBitSet(m_pPlayer->pev->flags, FL_ONGROUND))
		m_pPlayer->pev->punchangle.x -= m_Info.m_KickBackJumping.flUpBase;
	else if (FBitSet(m_pPlayer->pev->flags, FL_DUCKING))
		m_pPlayer->pev->punchangle.x -= m_Info.m_KickBackDucking.flUpBase;
	else
		m_pPlayer->pev->punchangle.x -= m_Info.m_KickBackDefault.flUpBase;
}

void CSniper::WeaponIdle(void)
{
}

float CSniper::GetMaxSpeed(void)
{
	if (m_pPlayer->GetFOV() != 90)
		return m_Info.flMaxMoveSpeedInZoom;
	else
		return m_Info.flMaxMoveSpeed;
}