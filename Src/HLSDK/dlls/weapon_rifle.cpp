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

LINK_ENTITY_TO_CLASS(weapon_rifle, CRifle);

//----------------------------------------------------------------
// Purpose : When the weapon is spawned.
//----------------------------------------------------------------
void CRifle::Spawn(void)
{
	// Entity setup.
	pev->classname = MAKE_STRING("weapon_rifle");

	// Get resources index.
	Precache();

	// Weapon entity setup.
	CBasePlayerWeapon::Spawn();

	// Set weapon class.
	m_iWeaponClass = WeaponClass_RIFLE;

	// Give ammo.
	m_iClip = m_Info.iAmmoPerMagazine;
	m_iAmmo = m_Info.iMaxAmmo;
}

//----------------------------------------------------------------
// Purpose : Precache all resources.
//----------------------------------------------------------------
void CRifle::Precache(void)
{
	m_iRifleFire = PRECACHE_EVENT(1, "events/riflefire.sc");
}

//----------------------------------------------------------------
// Purpose : When player takes out the weapon.
//----------------------------------------------------------------
void CRifle::Deploy(void)
{
	// Cancel Reload action.
	m_fInReload = FALSE;

	// Don't send the gun animation.
	m_fSendPrefireAnim = FALSE;
	m_fSendPostfireAnim = FALSE;

	// Reset the shoot state.
	m_flAccuracy = 0.2;
	m_iContinuousShooting = 0;
	m_iShotsFired = 0;
	m_bDelayFire = FALSE;

	// Setup weapon models.
	DefaultDeploy(m_Info.iszPViewModelFileName, m_Info.iszGViewModelFileName, m_Info.m_AnimSelect.iSequence, m_Info.szGViewAnimName, m_Info.m_AnimSelect.flTime);
}

//----------------------------------------------------------------
// Purpose : Reload ammunition.
//----------------------------------------------------------------
void CRifle::Reload(void)
{
	// Try to start reload action.
	if (DefaultReload(m_Info.iAmmoPerMagazine, m_Info.m_AnimReload.iSequence, m_Info.m_AnimReload.flTime))
	{
		// Reload action started, start player's model animation.
		m_pPlayer->SetAnimation(PLAYER_RELOAD);

		// Don't send the gun animation.
		m_fSendPrefireAnim = FALSE;
		m_fSendPostfireAnim = FALSE;

		// Reset the shoot state.
		m_flAccuracy = 0.2;
		m_iContinuousShooting = 0;
		m_iShotsFired = 0;
		m_bDelayFire = FALSE;
	}
}

//----------------------------------------------------------------
// Purpose : When player clicks left button, shoot now.
//----------------------------------------------------------------
void CRifle::PrimaryAttack(void)
{
	// On first shoot play the PreFire animation.
	bool bFirstShoot = FBitSet(m_pPlayer->m_afButtonPressed, IN_ATTACK) ? true : false;

	// If the player is in a special state, the bullet may not be accurate.

	if (m_pPlayer->pev->velocity.Length2D() > 140.0f)
	{
		// The player is running.
		RifleFire(m_Info.m_AccuracyRunning.flSpreadBase + m_Info.m_AccuracyRunning.flSpreadModifier * m_flAccuracy, m_Info.flShotIntervalTime, MOVESTATE_RUNNING, bFirstShoot);
	}
	else if (!FBitSet(m_pPlayer->pev->flags, FL_ONGROUND))
	{
		// The player in air.
		RifleFire(m_Info.m_AccuracyJumping.flSpreadBase + m_Info.m_AccuracyJumping.flSpreadModifier * m_flAccuracy, m_Info.flShotIntervalTime, MOVESTATE_JUMPING, bFirstShoot);
	}
	else if (FBitSet(m_pPlayer->pev->flags, FL_DUCKING))
	{
		// The player is ducking.
		RifleFire(m_Info.m_AccuracyDucking.flSpreadBase + m_Info.m_AccuracyDucking.flSpreadModifier * m_flAccuracy, m_Info.flShotIntervalTime, MOVESTATE_DUCKING, bFirstShoot);
	}
	else
	{
		// The player is walking.
		RifleFire(m_Info.m_AccuracyDefault.flSpreadBase + m_Info.m_AccuracyDefault.flSpreadModifier * m_flAccuracy, m_Info.flShotIntervalTime, MOVESTATE_WALKING, bFirstShoot);
	}
}

//----------------------------------------------------------------
// Purpose : Fire bullets.
//----------------------------------------------------------------
void CRifle::RifleFire(float flSpread, float flCycleTime, MoveState iMoveState, bool bFirstShoot)
{
	if (m_iClip <= 0)
	{
		EMIT_SOUND_DYN(m_pPlayer->edict(), CHAN_WEAPON, "weapon/others/g_nobullet_rifle.wav", 1.0, ATTN_NORM, 0, PITCH_NORM);
		m_flNextPrimaryAttack = 0.2f;
		return;
	}

	m_bDelayFire = true;

	m_iShotsFired++;

	switch (iMoveState)
	{
	case MOVESTATE_WALKING:
		{
			m_flAccuracy = ((m_iShotsFired * m_iShotsFired * m_iShotsFired) / m_Info.m_AccuracyDefault.flAccuracyModifier) + m_Info.m_AccuracyDefault.flAccuracyBase;

			if (m_flAccuracy > m_Info.m_AccuracyDefault.flAccuracyMax)
			{
				m_flAccuracy = m_Info.m_AccuracyDefault.flAccuracyMax;
			}

			break;
		}
	case MOVESTATE_RUNNING:
		{
			m_flAccuracy = ((m_iShotsFired * m_iShotsFired * m_iShotsFired) / m_Info.m_AccuracyRunning.flAccuracyModifier) + m_Info.m_AccuracyRunning.flAccuracyBase;

			if (m_flAccuracy > m_Info.m_AccuracyRunning.flAccuracyMax)
			{
				m_flAccuracy = m_Info.m_AccuracyRunning.flAccuracyMax;
			}

			break;
		}
	case MOVESTATE_JUMPING:
		{
			m_flAccuracy = ((m_iShotsFired * m_iShotsFired * m_iShotsFired) / m_Info.m_AccuracyJumping.flAccuracyModifier) + m_Info.m_AccuracyJumping.flAccuracyBase;

			if (m_flAccuracy > m_Info.m_AccuracyJumping.flAccuracyMax)
			{
				m_flAccuracy = m_Info.m_AccuracyJumping.flAccuracyMax;
			}

			break;
		}
	case MOVESTATE_DUCKING:
		{
			m_flAccuracy = ((m_iShotsFired * m_iShotsFired * m_iShotsFired) / m_Info.m_AccuracyDucking.flAccuracyModifier) + m_Info.m_AccuracyDucking.flAccuracyBase;

			if (m_flAccuracy > m_Info.m_AccuracyDucking.flAccuracyMax)
			{
				m_flAccuracy = m_Info.m_AccuracyDucking.flAccuracyMax;
			}

			break;
		}
	}

	m_iClip--;

	m_pPlayer->SetAnimation(PLAYER_ATTACK1);

	if (bFirstShoot)
	{
		SendWeaponAnim(m_Info.m_AnimPrefire.iSequence);
		m_flTimeWeaponIdle = m_Info.m_AnimPrefire.flTime + 0.1;
		m_iContinuousShooting = 1;
	}
	else
	{
		if (m_iClip > 0)
		{
			SendWeaponAnim(m_Info.m_AnimFire.iSequence);
			m_flTimeWeaponIdle = m_Info.m_AnimFire.flTime + 0.1;
			m_iContinuousShooting++;
		}
		else
		{
			// Last shoot
			SendWeaponAnim(m_Info.m_AnimPostfire.iSequence);
			m_flTimeWeaponIdle = m_Info.m_AnimPostfire.flTime + 0.1;
			m_iContinuousShooting = 1;
		}
	}

	UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle + m_pPlayer->pev->punchangle);

	Vector vecSrc = m_pPlayer->GetGunPosition();

	Vector vecDir = m_pPlayer->FireBullets(vecSrc, 
										   gpGlobals->v_forward, 
										   flSpread, 
										   8192.0f, 
										   m_Info.m_FireBullets.iPenetration, 
										   m_Info.m_FireBullets.iPenetrationPower, 
										   m_Info.m_FireBullets.iPenetrationDistance, 
										   m_Info.m_FireBullets.iDamage, 
										   m_Info.m_FireBullets.flRangeModifier, 
										   m_pPlayer->pev, 
										   m_pPlayer->random_seed);

	PLAYBACK_EVENT_FULL(FEV_NOTHOST, m_pPlayer->edict(), m_iRifleFire, 0.0f, g_vecZero, g_vecZero, 0.0f, 0.0f, m_Info.iWeaponIndex, 0, FALSE, FALSE);

	m_flNextPrimaryAttack = flCycleTime;
	m_flNextSecondaryAttack = flCycleTime;

	switch (iMoveState)
	{
	case MOVESTATE_WALKING:
	case MOVESTATE_RUNNING:
		{
			if (m_pPlayer->pev->velocity.Length2D() > 0.0f)
			{
				KickBack(m_Info.m_KickBackRunning.flUpBase, 
						 m_Info.m_KickBackRunning.flLateralBase, 
						 m_Info.m_KickBackRunning.flUpModifier, 
						 m_Info.m_KickBackRunning.flLateralModifier, 
						 m_Info.m_KickBackRunning.flUpMax, 
						 m_Info.m_KickBackRunning.flLateralMax, 
						 m_Info.m_KickBackRunning.iDirectionChange);
			}
			else
			{
				KickBack(m_Info.m_KickBackDefault.flUpBase, 
						 m_Info.m_KickBackDefault.flLateralBase, 
						 m_Info.m_KickBackDefault.flUpModifier, 
						 m_Info.m_KickBackDefault.flLateralModifier, 
						 m_Info.m_KickBackDefault.flUpMax, 
						 m_Info.m_KickBackDefault.flLateralMax, 
						 m_Info.m_KickBackDefault.iDirectionChange);
			}

			break;
		}
	case MOVESTATE_JUMPING:
		{
			KickBack(m_Info.m_KickBackJumping.flUpBase, 
					 m_Info.m_KickBackJumping.flLateralBase, 
					 m_Info.m_KickBackJumping.flUpModifier, 
					 m_Info.m_KickBackJumping.flLateralModifier, 
					 m_Info.m_KickBackJumping.flUpMax, 
					 m_Info.m_KickBackJumping.flLateralMax, 
					 m_Info.m_KickBackJumping.iDirectionChange);

			break;
		}
	case MOVESTATE_DUCKING:
		{
			KickBack(m_Info.m_KickBackDucking.flUpBase, 
					 m_Info.m_KickBackDucking.flLateralBase, 
					 m_Info.m_KickBackDucking.flUpModifier, 
					 m_Info.m_KickBackDucking.flLateralModifier, 
					 m_Info.m_KickBackDucking.flUpMax, 
					 m_Info.m_KickBackDucking.flLateralMax, 
					 m_Info.m_KickBackDucking.iDirectionChange);

			break;
		}
	}
}

void CRifle::WeaponIdle(void)
{
	if (FBitSet(m_pPlayer->m_afButtonReleased, IN_ATTACK))
	{
		if (m_iContinuousShooting > 1)
		{
			SendWeaponAnim(m_Info.m_AnimPostfire.iSequence);
			m_flTimeWeaponIdle = m_Info.m_AnimPostfire.flTime + 0.1f;
			m_iContinuousShooting = 1;
		}
	}
}

float CRifle::GetMaxSpeed(void)
{
	return m_Info.flMaxMoveSpeed;
}