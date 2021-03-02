/****
 * 
 * Copyright (c) 2017, Crsky
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 * 
 * * Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
****/

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"

#include "cs_bot.h"


// Fire our active weapon towards our current enemy
// NOTE: Aiming our weapon is handled in RunBotUpkeep()
void CCSBot::FireWeaponAtEnemy()
{
	CBasePlayer *enemy = GetEnemy();
	if (enemy == NULL)
	{
		StopRapidFire();
		return;
	}

	if (IsUsingSniperRifle())
	{
		// if we're using a sniper rifle, don't fire until we are standing still, are zoomed in, and not rapidly moving our view
		if (!IsNotMoving())
		{
			return;
		}
	}

	if (gpGlobals->time > m_fireWeaponTimestamp && GetTimeSinceAcquiredCurrentEnemy() >= GetProfile()->GetAttackDelay() && GetTimeSinceAcquiredCurrentEnemy() >= GetSurpriseDelay())
	{
		ClearSurpriseDelay();

		if (!(IsRecognizedEnemyProtectedByShield() && IsPlayerFacingMe(enemy))		// dont shoot at enemies behind shields
			&& !IsActiveWeaponReloading()
			&& !IsActiveWeaponClipEmpty()
			&& IsEnemyVisible())
		{
			// we have a clear shot - pull trigger if we are aiming at enemy
			Vector2D toAimSpot = (m_aimSpot - pev->origin).Make2D();
			float rangeToEnemy = toAimSpot.NormalizeInPlace();

			const float_precision halfPI = (M_PI / 180.0f);
			float_precision yaw = pev->v_angle[ YAW ] * halfPI;

			Vector2D dir(Q_cos(yaw), Q_sin(yaw));
			float_precision onTarget = DotProduct(toAimSpot, dir);

			// aim more precisely with a sniper rifle
			// because rifles' bullets spray, dont have to be very precise
			const float_precision halfSize = (IsUsingSniperRifle()) ? HalfHumanWidth : 2.0f * HalfHumanWidth;

			// aiming tolerance depends on how close the target is - closer targets subtend larger angles
			float_precision aimTolerance = Q_cos(Q_atan(halfSize / rangeToEnemy));

			if (onTarget > aimTolerance)
			{
				bool doAttack;

				// if friendly fire is on, don't fire if a teammate is blocking our line of fire
				if (TheCSBots->AllowFriendlyFireDamage())
				{
					if (IsFriendInLineOfFire())
						doAttack = false;
					else
						doAttack = true;
				}
				else
				{
					// fire freely
					doAttack = true;
				}

				if (doAttack)
				{
					// if we are using a knife, only swing it if we're close
					if (IsUsingKnife())
					{
						const float knifeRange = 75.0f; // 50.0f
						if (rangeToEnemy < knifeRange)
						{
							// since we've given ourselves away - run!
							ForceRun(5.0f);

							// if our prey is facing away, backstab him!
							if (!IsPlayerFacingMe(enemy))
							{
								SecondaryAttack();
							}
							else
							{
								// randomly choose primary and secondary attacks with knife
								const float knifeStabChance = 33.3f;
								if (RANDOM_FLOAT(0, 100) < knifeStabChance)
									SecondaryAttack();
								else
									PrimaryAttack();
							}
						}
					}
					else
					{
						PrimaryAttack();
					}
				}

				if (IsUsingPistol())
				{
					// high-skill bots fire their pistols quickly at close range
					const float closePistolRange = 999999.9f;
					if (GetProfile()->GetSkill() > 0.75f && rangeToEnemy < closePistolRange)
					{
						StartRapidFire();

						// fire as fast as possible
						m_fireWeaponTimestamp = 0.0f;
					}
					else
					{
						// fire somewhat quickly
						m_fireWeaponTimestamp = RANDOM_FLOAT(0.15f, 0.4f);
					}
				}
				// not using a pistol
				else
				{
					const float sprayRange = 400.0f;
					if (GetProfile()->GetSkill() < 0.5f || rangeToEnemy < sprayRange || IsUsingMachinegun())
					{
						// spray 'n pray if enemy is close, or we're not that good, or we're using the big machinegun
						m_fireWeaponTimestamp = 0.0f;
					}
					else
					{
						const float distantTargetRange = 800.0f;
						if (!IsUsingSniperRifle() && rangeToEnemy > distantTargetRange)
						{
							// if very far away, fire slowly for better accuracy
							m_fireWeaponTimestamp = RANDOM_FLOAT(0.3f, 0.7f);
						}
						else
						{
							// fire short bursts for accuracy
							m_fireWeaponTimestamp = RANDOM_FLOAT(0.15f, 0.5f); // 0.15f, 0.25f
						}
					}
				}

				// subtract system latency
				m_fireWeaponTimestamp -= g_flBotFullThinkInterval;
				m_fireWeaponTimestamp += gpGlobals->time;
			}
		}
	}
}

// Set the current aim offset using given accuracy (1.0 = perfect aim, 0.0f = terrible aim)
void CCSBot::SetAimOffset(float accuracy)
{
	// if our accuracy is less than perfect, it will improve as we "focus in" while not rotating our view
	if (accuracy < 1.0f)
	{
		// if we moved our view, reset our "focus" mechanism
		if (IsViewMoving(100.0f))
		{
			m_aimSpreadTimestamp = gpGlobals->time;
		}

		// focusTime is the time it takes for a bot to "focus in" for very good aim, from 2 to 5 seconds
		const float focusTime = Q_max(5.0f * (1.0f - accuracy), 2.0f);

		float focusInterval = gpGlobals->time - m_aimSpreadTimestamp;
		float focusAccuracy = focusInterval / focusTime;

		// limit how much "focus" will help
		const float maxFocusAccuracy = 0.75f;

		if (focusAccuracy > maxFocusAccuracy)
			focusAccuracy = maxFocusAccuracy;

		accuracy = Q_max(accuracy, focusAccuracy);
	}

	PrintIfWatched("Accuracy = %4.3f\n", accuracy);

	float range = (m_lastEnemyPosition - pev->origin).Length();
	const float_precision maxOffset = range * (float_precision(m_iFOV) / DEFAULT_FOV) * 0.1;
	float error = maxOffset * (1 - accuracy);

	m_aimOffsetGoal[0] = RANDOM_FLOAT(-error, error);
	m_aimOffsetGoal[1] = RANDOM_FLOAT(-error, error);
	m_aimOffsetGoal[2] = RANDOM_FLOAT(-error, error);

	// define time when aim offset will automatically be updated
	m_aimOffsetTimestamp = gpGlobals->time + RANDOM_FLOAT(0.25f, 1.0f);
}

// Wiggle aim error based on GetProfile()->GetSkill()
void CCSBot::UpdateAimOffset()
{
	if (gpGlobals->time >= m_aimOffsetTimestamp)
	{
		SetAimOffset(GetProfile()->GetSkill());
	}

	// move current offset towards goal offset
	Vector d = m_aimOffsetGoal - m_aimOffset;
	const float stiffness = 0.1f;

	m_aimOffset.x += stiffness * d.x;
	m_aimOffset.y += stiffness * d.y;
	m_aimOffset.z += stiffness * d.z;
}

// Change our zoom level to be appropriate for the given range.
// Return true if the zoom level changed.
bool CCSBot::AdjustZoom(float range)
{
	bool adjustZoom = false;

	if (IsUsingSniperRifle())
	{
		// NOTE: This must be less than sniperMinRange in AttackState
		const float sniperZoomRange = 300.0f; //150.0f
		const float sniperFarZoomRange = 1500.0f;

		// if range is too close, don't zoom
		if (range <= sniperZoomRange)
		{
			// zoom out
			if (GetZoomLevel() != NO_ZOOM)
			{
				adjustZoom = true;
			}
		}
		else if (range < sniperFarZoomRange)
		{
			// maintain low zoom
			if (GetZoomLevel() != LOW_ZOOM)
			{
				adjustZoom = true;
			}
		}
		else
		{
			// maintain high zoom
			if (GetZoomLevel() != HIGH_ZOOM)
			{
				adjustZoom = true;
			}
		}
	}
	else
	{
		// zoom out
		if (GetZoomLevel() != NO_ZOOM)
		{
			adjustZoom = true;
		}
	}

	if (adjustZoom)
	{
		SecondaryAttack();
	}

	return adjustZoom;
}

// Return true if the given weapon is a sniper rifle
bool isSniperRifle(CBasePlayerWeapon *item)
{
//	switch (item->m_iId)
//	{
//	case WEAPON_SCOUT:
//	case WEAPON_SG550:
//	case WEAPON_AWP:
//	case WEAPON_G3SG1:
//		return true;
//
//	default:
//		return false;
//	}
	return false;
}

bool CCSBot::IsUsingAWP() const
{
//	CBasePlayerWeapon *weapon = GetActiveWeapon();

//	if (weapon != NULL && weapon->m_iId == WEAPON_AWP)
//		return true;

	return false;
}

// Returns true if we are using a weapon with a removable silencer
bool CCSBot::DoesActiveWeaponHaveSilencer() const
{
//	CBasePlayerWeapon *weapon = GetActiveWeapon();

//	if (weapon == NULL)
//		return false;

//	if (weapon->m_iId == WEAPON_M4A1 || weapon->m_iId == WEAPON_USP)
//		return true;

	return false;
}

// Return true if we are using a sniper rifle
bool CCSBot::IsUsingSniperRifle() const
{
	CBasePlayerWeapon *weapon = GetActiveWeapon();

	if (weapon != NULL && isSniperRifle(weapon))
		return true;

	return false;
}

// Return true if we have a sniper rifle in our inventory
bool CCSBot::IsSniper() const
{
//	for (int i = 0; i < MAX_ITEM_TYPES; ++i)
//	{
//		CBasePlayerItem *item = m_rgpPlayerItems[i];
//
//		while (item != NULL)
//		{
//			if (isSniperRifle(item))
//				return true;
//
//			item = item->m_pNext;
//		}
//	}

	return false;
}

// Return true if we are actively sniping (moving to sniper spot or settled in)
bool CCSBot::IsSniping() const
{
	if (GetTask() == MOVE_TO_SNIPER_SPOT || GetTask() == SNIPING)
		return true;

	return false;
}

// Return true if we are using a shotgun
bool CCSBot::IsUsingShotgun() const
{
//	CBasePlayerWeapon *weapon = GetActiveWeapon();

//	if (weapon == NULL)
//		return false;

//	if (weapon->m_iId == WEAPON_XM1014 || weapon->m_iId == WEAPON_M3)
//		return true;

	return false;
}

// Returns true if using the big 'ol machinegun
bool CCSBot::IsUsingMachinegun() const
{
//	CBasePlayerWeapon *weapon = GetActiveWeapon();

//	if (weapon != NULL && weapon->m_iId == WEAPON_M249)
//		return true;

	return false;
}

// Return true if primary weapon doesn't exist or is totally out of ammo
bool CCSBot::IsPrimaryWeaponEmpty() const
{
//	CBasePlayerWeapon *weapon = static_cast<CBasePlayerWeapon *>(m_rgpPlayerItems[ PRIMARY_WEAPON_SLOT ]);

//	if (weapon == NULL)
//		return true;

//	// check if gun has any ammo left
//	if (HasAnyAmmo(weapon))
//		return false;

	return true;
}

// Return true if pistol doesn't exist or is totally out of ammo
bool CCSBot::IsPistolEmpty() const
{
//	CBasePlayerWeapon *weapon = static_cast<CBasePlayerWeapon *>(m_rgpPlayerItems[ PISTOL_SLOT ]);

//	if (weapon == NULL)
//		return true;

	// check if gun has any ammo left
//	if (HasAnyAmmo(weapon))
//	{
//		return false;
//	}

	return true;
}

// Equip the given item
bool CCSBot::DoEquip(CBasePlayerWeapon *gun)
{
//	if (gun == NULL)
//		return false;

	// check if weapon has any ammo left
//	if (!HasAnyAmmo(gun))
//		return false;

	// equip it
//	SelectItem(STRING(gun->pev->classname));
//	m_equipTimer.Start();

	return true;
}

// throttle how often equipping is allowed
const float minEquipInterval = 5.0f;

// Equip the best weapon we are carrying that has ammo
void CCSBot::EquipBestWeapon(bool mustEquip)
{
	// throttle how often equipping is allowed
//	if (!mustEquip && m_equipTimer.GetElapsedTime() < minEquipInterval)
//		return;

//	CBasePlayerWeapon *primary = static_cast<CBasePlayerWeapon *>(m_rgpPlayerItems[ PRIMARY_WEAPON_SLOT ]);

//	if (primary != NULL)
//	{
//		WeaponClassType weaponClass = WeaponIDToWeaponClass(primary->m_iId);
//
//		if ((TheCSBots->AllowShotguns() && weaponClass == WEAPONCLASS_SHOTGUN)
//			|| (TheCSBots->AllowMachineGuns() && weaponClass == WEAPONCLASS_MACHINEGUN)
//			|| (TheCSBots->AllowRifles() && weaponClass == WEAPONCLASS_RIFLE)
//#ifndef REGAMEDLL_FIXES
//			// TODO: already is checked shotguns!
//			|| (TheCSBots->AllowShotguns() && weaponClass == WEAPONCLASS_SHOTGUN)
//#endif
//			|| (TheCSBots->AllowSnipers() && weaponClass == WEAPONCLASS_SNIPERRIFLE)
//			|| (TheCSBots->AllowSubMachineGuns() && weaponClass == WEAPONCLASS_SUBMACHINEGUN)
//			|| (TheCSBots->AllowTacticalShield() && primary->m_iId == WEAPON_SHIELDGUN))
//		{
//			if (DoEquip(primary))
//				return;
//		}
//	}

//	if (TheCSBots->AllowPistols())
//	{
//		if (DoEquip(static_cast<CBasePlayerWeapon *>(m_rgpPlayerItems[ PISTOL_SLOT ])))
//			return;
//	}

	// always have a knife
	EquipKnife();
}

// Equip our pistol
void CCSBot::EquipPistol()
{
	// throttle how often equipping is allowed
	if (m_equipTimer.GetElapsedTime() < minEquipInterval)
		return;

//	if (TheCSBots()->AllowPistols() && !IsUsingPistol())
//	{
//		CBasePlayerWeapon *pistol = static_cast<CBasePlayerWeapon *>(m_rgpPlayerItems[ PISTOL_SLOT ]);
//		DoEquip(pistol);
//	}
}

// Equip the knife
void CCSBot::EquipKnife()
{
//	if (!IsUsingKnife())
//	{
//		CBasePlayerWeapon *knife = static_cast<CBasePlayerWeapon *>(m_rgpPlayerItems[ KNIFE_SLOT ]);
//		if (knife != NULL)
//		{
//			SelectItem(STRING(knife->pev->classname));
//		}
//	}
}

// Return true if we have a grenade in our inventory
bool CCSBot::HasGrenade() const
{
//	CBasePlayerWeapon *grenade = static_cast<CBasePlayerWeapon *>(m_rgpPlayerItems[ GRENADE_SLOT ]);
//	return grenade != NULL;
	return false;
}

// Equip a grenade, return false if we cant
bool CCSBot::EquipGrenade(bool noSmoke)
{
	// snipers don't use grenades
//	if (IsSniper())
//		return false;

//	if (IsUsingGrenade())
//		return true;

//	if (HasGrenade())
//	{
//		CBasePlayerWeapon *grenade = static_cast<CBasePlayerWeapon *>(m_rgpPlayerItems[ GRENADE_SLOT ]);
//
//		if (grenade != NULL)
//		{
//			if (noSmoke && grenade->m_iId == WEAPON_SMOKEGRENADE)
//				return false;
//
//			SelectItem(STRING(grenade->pev->classname));
//			return true;
//		}
//	}

	return false;
}

// Returns true if we have knife equipped
bool CCSBot::IsUsingKnife() const
{
//	CBasePlayerWeapon *weapon = GetActiveWeapon();

//	if (weapon != NULL && weapon->m_iId == WEAPON_KNIFE)
//		return true;

	return false;
}

// Returns true if we have pistol equipped
bool CCSBot::IsUsingPistol() const
{
//	CBasePlayerWeapon *weapon = GetActiveWeapon();

//	if (weapon != NULL && weapon->IsPistol())
//		return true;

	return false;
}

// Returns true if we have a grenade equipped
bool CCSBot::IsUsingGrenade() const
{
//	CBasePlayerWeapon *weapon = GetActiveWeapon();

//	if (weapon == NULL)
//		return false;

//	if (weapon->m_iId == WEAPON_SMOKEGRENADE
//		|| weapon->m_iId == WEAPON_FLASHBANG
//		|| weapon->m_iId == WEAPON_HEGRENADE)
//		return true;

	return false;
}

bool CCSBot::IsUsingHEGrenade() const
{
//	CBasePlayerWeapon *weapon = GetActiveWeapon();

//	if (weapon != NULL && weapon->m_iId == WEAPON_HEGRENADE)
//		return true;

	return false;
}

// Begin the process of throwing the grenade
void CCSBot::ThrowGrenade(const Vector *target)
{
	if (IsUsingGrenade() && !m_isWaitingToTossGrenade)
	{
		const float angleTolerance = 1.0f;

		SetLookAt("GrenadeThrow", target, PRIORITY_UNINTERRUPTABLE, 3.0f, false, angleTolerance);

		m_isWaitingToTossGrenade = true;
		m_tossGrenadeTimer.Start(3.0f);
	}
}

// Find spot to throw grenade ahead of us and "around the corner" along our path
bool CCSBot::FindGrenadeTossPathTarget(Vector *pos)
{
//	if (!HasPath())
//		return false;
//
//	// find farthest point we can see on the path
//	int i;
//	for (i = m_pathIndex; i < m_pathLength; ++i)
//	{
//		if (!FVisible(m_path[i].pos + Vector(0, 0, HalfHumanHeight)))
//			break;
//	}
//
//	if (i == m_pathIndex)
//		return false;
//
//	// find exact spot where we lose sight
//	Vector dir = m_path[i].pos - m_path[i - 1].pos;
//	float length = dir.NormalizeInPlace();
//
//	const float inc = 25.0f;
//	Vector p;
//	Vector visibleSpot = m_path[i - 1].pos;
//	for (float t = 0.0f; t < length; t += inc)
//	{
//		p = m_path[i - 1].pos + t * dir;
//		p.z += HalfHumanHeight;
//
//		if (!FVisible(p))
//			break;
//
//		visibleSpot = p;
//	}
//
//	// massage the location a bit
//	visibleSpot.z += 10.0f;
//
//	const float bufferRange = 50.0f;
//	TraceResult result;
//	Vector check;
//
//	// check +X
//	check = visibleSpot + Vector(999.9f, 0, 0);
//	UTIL_TraceLine(visibleSpot, check, dont_ignore_monsters, ignore_glass, ENT(pev), &result);
//
//	if (result.flFraction < 1.0f)
//	{
//		float range = result.vecEndPos.x - visibleSpot.x;
//		if (range < bufferRange)
//		{
//			visibleSpot.x = result.vecEndPos.x - bufferRange;
//		}
//	}
//
//	// check -X
//	check = visibleSpot + Vector(-999.9f, 0, 0);
//	UTIL_TraceLine(visibleSpot, check, dont_ignore_monsters, ignore_glass, ENT(pev), &result);
//
//	if (result.flFraction < 1.0f)
//	{
//		float range = visibleSpot.x - result.vecEndPos.x;
//		if (range < bufferRange)
//		{
//			visibleSpot.x = result.vecEndPos.x + bufferRange;
//		}
//	}
//
//	// check +Y
//	check = visibleSpot + Vector(0, 999.9f, 0);
//	UTIL_TraceLine(visibleSpot, check, dont_ignore_monsters, ignore_glass, ENT(pev), &result);
//
//	if (result.flFraction < 1.0f)
//	{
//		float range = result.vecEndPos.y - visibleSpot.y;
//		if (range < bufferRange)
//		{
//			visibleSpot.y = result.vecEndPos.y - bufferRange;
//		}
//	}
//
//	// check -Y
//	check = visibleSpot + Vector(0, -999.9f, 0);
//	UTIL_TraceLine(visibleSpot, check, dont_ignore_monsters, ignore_glass, ENT(pev), &result);
//
//	if (result.flFraction < 1.0f)
//	{
//		float range = visibleSpot.y - result.vecEndPos.y;
//		if (range < bufferRange)
//		{
//			visibleSpot.y = result.vecEndPos.y + bufferRange;
//		}
//	}
//
//	*pos = visibleSpot;
//	return true;
	return false;
}

// Reload our weapon if we must
void CCSBot::ReloadCheck()
{
//	const float safeReloadWaitTime = 3.0f;
//	const float reloadAmmoRatio = 0.6f;
//
//	// don't bother to reload if there are no enemies left
//	if (GetEnemiesRemaining() == 0)
//		return;
//
//	if (IsDefusingBomb() || IsActiveWeaponReloading())
//		return;
//
//	if (IsActiveWeaponClipEmpty())
//	{
//		// high-skill players switch to pistol instead of reloading during combat
//		if (GetProfile()->GetSkill() > 0.5f && IsAttacking())
//		{
//			if (!GetActiveWeapon()->IsPistol() && !IsPistolEmpty())
//			{
//				// switch to pistol instead of reloading
//				EquipPistol();
//				return;
//			}
//		}
//	}
//	else if (GetTimeSinceLastSawEnemy() > safeReloadWaitTime && GetActiveWeaponAmmoRatio() <= reloadAmmoRatio)
//	{
//		// high-skill players use all their ammo and switch to pistol instead of reloading during combat
//		if (GetProfile()->GetSkill() > 0.5f && IsAttacking())
//			return;
//	}
//	else
//	{
//		// do not need to reload
//		return;
//	}
//
//	// don't reload the AWP until it is totally out of ammo
//	if (IsUsingAWP() && !IsActiveWeaponClipEmpty())
//		return;
//
//	Reload();
//
//	// move to cover to reload if there are enemies nearby
//	if (GetNearbyEnemyCount())
//	{
//		// avoid enemies while reloading (above 0.75 skill always hide to reload)
//		const float hideChance = 25.0f + 100.0f * GetProfile()->GetSkill();
//
//		if (!IsHiding() && RANDOM_FLOAT(0.0f, 100.0f) < hideChance)
//		{
//			const float safeTime = 5.0f;
//			if (GetTimeSinceLastSawEnemy() < safeTime)
//			{
//				PrintIfWatched("Retreating to a safe spot to reload!\n");
//				const Vector *spot = FindNearbyRetreatSpot(this, 1000.0f);
//				if (spot != NULL)
//				{
//					// ignore enemies for a second to give us time to hide
//					// reaching our hiding spot clears our disposition
//					IgnoreEnemies(10.0f);
//
//					Run();
//					StandUp();
//					Hide(spot, 0.0f);
//				}
//			}
//		}
//	}
}

// Silence/unsilence our weapon if we must
void CCSBot::SilencerCheck()
{
//	// longer than reload check because reloading should take precedence
//	const float safeSilencerWaitTime = 3.5f;
//
//	if (IsDefusingBomb() || IsActiveWeaponReloading() || IsAttacking())
//		return;
//
//	// M4A1 and USP are the only weapons with removable silencers
//	if (!DoesActiveWeaponHaveSilencer())
//		return;
//
//#ifdef REGAMEDLL_FIXES
//	if (GetTimeSinceLastSawEnemy() < safeSilencerWaitTime)
//		return;
//#endif
//
//	// don't touch the silencer if there are enemies nearby
//	if (GetNearbyEnemyCount() == 0)
//	{
//		CBasePlayerWeapon *myGun = GetActiveWeapon();
//		if (myGun == NULL)
//			return;
//
//		bool isSilencerOn = (myGun->m_iWeaponState & (WPNSTATE_M4A1_SILENCED | WPNSTATE_USP_SILENCED)) != 0;
//
//#ifndef REGAMEDLL_FIXES
//		if (isSilencerOn != GetProfile()->PrefersSilencer() && !HasShield())
//#else
//
//		if (myGun->m_flNextSecondaryAttack >= gpGlobals->time)
//			return;
//
//		// equip silencer if we want to and we don't have a shield.
//		if (isSilencerOn != (GetProfile()->PrefersSilencer() || GetProfile()->GetSkill() > 0.7f) && !HasShield())
//#endif
//		{
//			PrintIfWatched("%s silencer!\n", (isSilencerOn) ? "Unequipping" : "Equipping");
//			myGun->SecondaryAttack();
//		}
//	}
}

// Invoked when in contact with a CWeaponBox
void CCSBot::OnTouchingWeapon(CWeaponBox *box)
{
//	auto pDroppedWeapon = box->m_rgpPlayerItems[ PRIMARY_WEAPON_SLOT ];
//
//	// right now we only care about primary weapons on the ground
//	if (pDroppedWeapon)
//	{
//		CBasePlayerWeapon *pWeapon = (CBasePlayerWeapon *)m_rgpPlayerItems[ PRIMARY_WEAPON_SLOT ];
//
//		// if the gun on the ground is the same one we have, dont bother
//		if (pWeapon && pWeapon->IsWeapon() && pDroppedWeapon->m_iId != pWeapon->m_iId)
//		{
//			// if we don't have a weapon preference, give up
//			if (GetProfile()->HasPrimaryPreference())
//			{
//				// don't change weapons if we've seen enemies recently
//				const float safeTime = 2.5f;
//				if (GetTimeSinceLastSawEnemy() >= safeTime)
//				{
//					// we have a primary weapon - drop it if the one on the ground is better
//					for (int i = 0; i < GetProfile()->GetWeaponPreferenceCount(); ++i)
//					{
//						int prefID = GetProfile()->GetWeaponPreference(i);
//						if (!IsPrimaryWeapon(prefID))
//							continue;
//
//						// if the gun we are using is more desirable, give up
//						if (prefID == pWeapon->m_iId)
//							break;
//
//						if (prefID == pDroppedWeapon->m_iId)
//						{
//							// the gun on the ground is better than the one we have - drop our gun
//							DropPrimary();
//							break;
//						}
//					}
//				}
//			}
//		}
//	}
}

// Return true if a friend is in our weapon's way
// TODO: Check more rays for safety.
bool CCSBot::IsFriendInLineOfFire()
{
//	if (CSGameRules()->IsFreeForAll())
//		return false;

	UTIL_MakeVectors(pev->punchangle + pev->v_angle);

	// compute the unit vector along our view
	Vector aimDir = gpGlobals->v_forward;
	Vector target = GetGunPosition();

	// trace the bullet's path
	TraceResult result;
	UTIL_TraceLine(GetGunPosition(), target + 10000.0f * aimDir, dont_ignore_monsters, ignore_glass, ENT(pev), &result);

	if (result.pHit != NULL)
	{
		CBaseEntity *victim = CBaseEntity::Instance(result.pHit);

		if (victim != NULL && victim->IsPlayer() && victim->IsAlive())
		{
			CBasePlayer *player = static_cast<CBasePlayer *>(victim);

//			if (BotRelationship(player) == BOT_TEAMMATE)
			if (player->m_iTeam == m_iTeam)
				return true;
		}
	}

	return false;
}

// Return line-of-sight distance to obstacle along weapon fire ray
// TODO: Re-use this computation with IsFriendInLineOfFire()
float CCSBot::ComputeWeaponSightRange()
{
	UTIL_MakeVectors(pev->punchangle + pev->v_angle);

	// compute the unit vector along our view
	Vector aimDir = gpGlobals->v_forward;
	Vector target = GetGunPosition();

	// trace the bullet's path
	TraceResult result;
	UTIL_TraceLine(GetGunPosition(), target + 10000.0f * aimDir, dont_ignore_monsters, ignore_glass, ENT(pev), &result);

	return (GetGunPosition() - result.vecEndPos).Length();
}
