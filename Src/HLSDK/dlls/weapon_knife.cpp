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

// Knife attack action type
enum
{
	COMBO_FIRST,
	COMBO_SECOND,
	COMBO_BIGSHOT,
};

// Link entity to engine.
LINK_ENTITY_TO_CLASS(weapon_knife, CKnife);

//----------------------------------------------------------------
// Purpose : When the weapon is spawned.
//----------------------------------------------------------------
void CKnife::Spawn(void)
{
	// Entity setup.
	pev->classname = MAKE_STRING("weapon_knife");

	// Get resources index.
	Precache();

	// Weapon entity setup.
	CBasePlayerWeapon::Spawn();

	// Set weapon class.
	m_iWeaponClass = WeaponClass_KNIFE;
}

//----------------------------------------------------------------
// Purpose : Precache all resources.
//----------------------------------------------------------------
void CKnife::Precache(void)
{
	m_iKnifeAttack = PRECACHE_EVENT(1, "events/knifeattack.sc");
}

//----------------------------------------------------------------
// Purpose : When player takes out the weapon.
//----------------------------------------------------------------
void CKnife::Deploy(void)
{
	// Remove think function.
	SetThink(NULL);
	
	// Setup weapon models.
	DefaultDeploy(m_Info.iszPViewModelFileName, m_Info.iszGViewModelFileName, m_Info.m_AnimSelect.iSequence, m_Info.szGViewAnimName, m_Info.m_AnimSelect.flTime);
}

//----------------------------------------------------------------
// Purpose : If weapon is packed up, we need to cancel the attack action.
//----------------------------------------------------------------
void CKnife::Holster(void)
{
	// Remove think function.
	SetThink(NULL);

	// Clear weapon models.
	CBasePlayerWeapon::Holster();
}

//----------------------------------------------------------------
// Purpose : We can not discard this weapon, so returns FALSE.
//----------------------------------------------------------------
BOOL CKnife::CanDrop(void)
{
	return FALSE;
}

//----------------------------------------------------------------
// Purpose : When the player clicks the left button.
//----------------------------------------------------------------
void CKnife::PrimaryAttack(void)
{
	// First play COMBO_FIRST animation. If player clicks the left button again in a short time 
	// after COMBO_FIRST animation finished, that play COMBO_SECOND animation.
	if (m_iComboType == COMBO_FIRST)
	{
		if (gpGlobals->time - (m_flLastPrimaryAttackTime + m_Info.m_KnifeCombo1Attack.flAttackIntervalTime) < 0.2f)
		{
			// Next attack will play the COMBO_SECOND animation.
			m_iComboType = COMBO_SECOND;
		}
	}
	else if (m_iComboType == COMBO_SECOND)
	{
		// If previous combo is COMBO_SECOND, that switch to COMBO_FIRST.
		m_iComboType = COMBO_FIRST;
	}

	// Now play model animation and start attack action.
	if (m_iComboType == COMBO_FIRST)
	{
		// Play player's model animation.
		m_pPlayer->SetAnimation(PLAYER_ATTACK1);

		// Play gun model animation.
		SendWeaponAnim(m_Info.m_AnimCombo1.iSequence);

		// How long we can start next attack.
		m_flNextPrimaryAttack = m_Info.m_KnifeCombo1Attack.flAttackIntervalTime;
		m_flNextSecondaryAttack = m_Info.m_KnifeCombo1Attack.flAttackIntervalTime;

		// When Combo animation is finished, start Idle animation.
		m_flTimeWeaponIdle = m_Info.m_AnimCombo1.flTime + 0.1f;

		// On next think do COMBO_FIRST attack action, take damage.
		SetThink(&CKnife::Combo1Attack);

		// Next think time.
		pev->nextthink = gpGlobals->time + m_Info.m_KnifeCombo1Attack.flDamageDelay;
	}
	else if (m_iComboType == COMBO_SECOND)
	{
		// Play player's model animation.
		m_pPlayer->SetAnimation(PLAYER_ATTACK2);

		// Play gun model animation.
		SendWeaponAnim(m_Info.m_AnimCombo2.iSequence);

		// How long we can start next attack.
		m_flNextPrimaryAttack = m_Info.m_KnifeCombo2Attack.flAttackIntervalTime;
		m_flNextSecondaryAttack = m_Info.m_KnifeCombo2Attack.flAttackIntervalTime;

		// When Combo animation is finished, start Idle animation.
		m_flTimeWeaponIdle = m_Info.m_AnimCombo2.flTime + 0.1;

		// On next think do COMBO_SECOND attack action, take damage.
		SetThink(&CKnife::Combo2Attack);

		// Next think time.
		pev->nextthink = gpGlobals->time + m_Info.m_KnifeCombo2Attack.flDamageDelay;
	}

	// Save the attack time, because we need switch combo type.
	m_flLastPrimaryAttackTime = gpGlobals->time;
}

//----------------------------------------------------------------
// Purpose : When the player clicks the right button.
//----------------------------------------------------------------
void CKnife::SingleSecondaryAttack(void)
{
	// Play player's model animation.
	m_pPlayer->SetAnimation(PLAYER_ATTACK3);

	// Play gun model animation.
	SendWeaponAnim(m_Info.m_AnimBigshot.iSequence);

	// How long we can start next attack.
	m_flNextPrimaryAttack = m_Info.m_KnifeBigshotAttack.flAttackIntervalTime;
	m_flNextSecondaryAttack = m_Info.m_KnifeBigshotAttack.flAttackIntervalTime;

	// When BigShot animation is finished, start Idle animation.
	m_flTimeWeaponIdle = m_Info.m_AnimBigshot.flTime + 0.1;

	// On next think do BigShot attack action, take damage.
	SetThink(&CKnife::BigshotAttack);

	// Next think time.
	pev->nextthink = gpGlobals->time + m_Info.m_KnifeBigshotAttack.flDamageDelay;
}

//----------------------------------------------------------------
// Purpose : The Combo1 attack action.
//----------------------------------------------------------------
void CKnife::Combo1Attack(void)
{
	// Do knife range attack
	m_pPlayer->KnifeAttack(m_Info.m_KnifeCombo1Attack.flAttackRange, m_Info.m_KnifeCombo1Attack.flAttackAngle, m_Info.m_KnifeCombo1Attack.flDamageValue);

	// Send the action event to client, make some hit effects.
	PLAYBACK_EVENT_FULL(FEV_NOTHOST, m_pPlayer->edict(), m_iKnifeAttack, 0.0f, g_vecZero, g_vecZero, 0.0f, 0.0f, m_Info.iWeaponIndex, COMBO_FIRST, FALSE, FALSE);
}

//----------------------------------------------------------------
// Purpose : The Combo2 attack action.
//----------------------------------------------------------------
void CKnife::Combo2Attack(void)
{
	// Do knife range attack
	m_pPlayer->KnifeAttack(m_Info.m_KnifeCombo2Attack.flAttackRange, m_Info.m_KnifeCombo2Attack.flAttackAngle, m_Info.m_KnifeCombo2Attack.flDamageValue);

	// Send the action event to client, make some hit effects.
	PLAYBACK_EVENT_FULL(FEV_NOTHOST, m_pPlayer->edict(), m_iKnifeAttack, 0.0f, g_vecZero, g_vecZero, 0.0f, 0.0f, m_Info.iWeaponIndex, COMBO_SECOND, FALSE, FALSE);
}

//----------------------------------------------------------------
// Purpose : The BigShot attack action.
//----------------------------------------------------------------
void CKnife::BigshotAttack(void)
{
	// Do knife range attack
	m_pPlayer->KnifeAttack(m_Info.m_KnifeBigshotAttack.flAttackRange, m_Info.m_KnifeBigshotAttack.flAttackAngle, m_Info.m_KnifeBigshotAttack.flDamageValue);

	// Send the action event to client, make some hit effects.
	PLAYBACK_EVENT_FULL(FEV_NOTHOST, m_pPlayer->edict(), m_iKnifeAttack, 0.0f, g_vecZero, g_vecZero, 0.0f, 0.0f, m_Info.iWeaponIndex, COMBO_BIGSHOT, FALSE, FALSE);
}

//----------------------------------------------------------------
// Purpose : Auto replay Idle animation.
//----------------------------------------------------------------
void CKnife::WeaponIdle(void)
{
}

//----------------------------------------------------------------
// Purpose : Max movement speed when player hold this weapon.
//----------------------------------------------------------------
float CKnife::GetMaxSpeed(void)
{
	return m_Info.flMaxMoveSpeed;
}