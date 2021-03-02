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

LINK_ENTITY_TO_CLASS(weapon_pistol, CPistol);

void CPistol::Spawn(void)
{
	pev->classname = MAKE_STRING("weapon_pistol");

	Precache();
	CBasePlayerWeapon::Spawn();

	m_iWeaponClass = WeaponClass_PISTOL;

	m_iClip = m_Info.iAmmoPerMagazine;
	m_iAmmo = m_Info.iMaxAmmo;
}

void CPistol::Precache(void)
{
	m_iPistolFire = PRECACHE_EVENT(1, "events/pistolfire.sc");
}

void CPistol::Deploy(void)
{
	m_fInReload = FALSE;

	m_flAccuracy = 0.9;
	m_iShotsFired = 0;
	m_flLastFire = 0;

	DefaultDeploy(m_Info.iszPViewModelFileName, m_Info.iszGViewModelFileName, m_Info.m_AnimSelect.iSequence, m_Info.szGViewAnimName, m_Info.m_AnimSelect.flTime);
}

void CPistol::Reload(void)
{
	if (DefaultReload(m_Info.iAmmoPerMagazine, m_Info.m_AnimReload.iSequence, m_Info.m_AnimReload.flTime))
	{
		m_pPlayer->SetAnimation(PLAYER_RELOAD);

		m_flAccuracy = 0.9;
		m_iShotsFired = 0;
		m_flLastFire = 0;
	}
}

void CPistol::SinglePrimaryAttack(void)
{
	if (m_pPlayer->pev->velocity.Length2D() > 140)
		PistolFire(m_Info.m_AccuracyRunning.flSpreadBase * (1 - m_flAccuracy), m_Info.flShotIntervalTime);
	else if (!FBitSet(m_pPlayer->pev->flags, FL_ONGROUND))
		PistolFire(m_Info.m_AccuracyJumping.flSpreadBase * (1 - m_flAccuracy), m_Info.flShotIntervalTime);
	else if (FBitSet(m_pPlayer->pev->flags, FL_DUCKING))
		PistolFire(m_Info.m_AccuracyDucking.flSpreadBase * (1 - m_flAccuracy), m_Info.flShotIntervalTime);
	else
		PistolFire(m_Info.m_AccuracyDefault.flSpreadBase * (1 - m_flAccuracy), m_Info.flShotIntervalTime);
}

void CPistol::PistolFire(float flSpread, float flCycleTime)
{
	m_iShotsFired++;

	//if (m_iShotsFired > 1)
	//	return;

	if (m_flLastFire)
	{
		m_flAccuracy -= (m_Info.m_AccuracyDefault.flAccuracyBase - (gpGlobals->time - m_flLastFire)) * m_Info.m_AccuracyDefault.flAccuracyModifier;

		if (m_flAccuracy > m_Info.m_AccuracyDefault.flAccuracyMax)
			m_flAccuracy = m_Info.m_AccuracyDefault.flAccuracyMax;

		if (m_flAccuracy < m_Info.m_AccuracyDefault.flAccuracyMin)
			m_flAccuracy = m_Info.m_AccuracyDefault.flAccuracyMin;
	}

	m_flLastFire = gpGlobals->time;

	if (m_iClip <= 0)
	{
		EMIT_SOUND_DYN(m_pPlayer->edict(), CHAN_WEAPON, "weapon/others/g_nobullet_pistol.wav", 1.0, ATTN_NORM, 0, PITCH_NORM);
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.2;
		return;
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
		m_iPistolFire,
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

void CPistol::WeaponIdle(void)
{
}

float CPistol::GetMaxSpeed(void)
{
	return m_Info.flMaxMoveSpeed;
}