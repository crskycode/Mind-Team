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
#include "in_buttons.h"
#include "cbase.h"
#include "pm_shared.h"
#include "player.h"
#include "weapons.h"
#include "animation.h"

short g_sModelIndexFireball;
short g_sModelIndexWExplosion;
short g_sModelIndexSmoke;
short g_sModelIndexBubbles;

extern int gmsgCurWeapon;

MULTIDAMAGE gMultiDamage;

void ClearMultiDamage(void)
{
	gMultiDamage.pEntity = NULL;
	gMultiDamage.amount = 0;
	gMultiDamage.type = 0;
}

void ApplyMultiDamage(entvars_t *pevInflictor, entvars_t *pevAttacker)
{
	if (!gMultiDamage.pEntity)
		return;

	gMultiDamage.pEntity->TakeDamage(pevInflictor, pevAttacker, gMultiDamage.amount, gMultiDamage.type);
}

void AddMultiDamage(entvars_t *pevInflictor, CBaseEntity *pEntity, float flDamage, int bitsDamageType)
{
	if (!pEntity)
		return;

	gMultiDamage.type |= bitsDamageType;

	if (pEntity != gMultiDamage.pEntity)
	{
		ApplyMultiDamage(pevInflictor, pevInflictor);
		gMultiDamage.pEntity = pEntity;
		gMultiDamage.amount = 0;
	}

	gMultiDamage.amount += flDamage;
}

void DecalGunshot(TraceResult &tr, int iBulletType)
{
}

void UTIL_PrecacheOtherWeapon(const char *classname)
{
	edict_t *pent = CREATE_NAMED_ENTITY(MAKE_STRING(classname));

	if (!pent)
		return;

	CBaseEntity *pEntity = CBaseEntity::Instance(pent);

	if (pEntity)
		pEntity->Precache();

	REMOVE_ENTITY(pent);
}

void W_Precache(void)
{
	g_sModelIndexFireball = PRECACHE_MODEL("sprites/zerogxplode.spr");
	g_sModelIndexWExplosion = PRECACHE_MODEL("sprites/WXplo1.spr");
	g_sModelIndexSmoke = PRECACHE_MODEL("sprites/steam1.spr");
	g_sModelIndexBubbles = PRECACHE_MODEL("sprites/bubble.spr");

	UTIL_PrecacheOtherWeapon("weapon_knife");
	UTIL_PrecacheOtherWeapon("weapon_rifle");
	UTIL_PrecacheOtherWeapon("weapon_pistol");
	UTIL_PrecacheOtherWeapon("weapon_sniper");
	UTIL_PrecacheOtherWeapon("weapon_shotgun");
	UTIL_PrecacheOtherWeapon("weapon_hegrenade");
	UTIL_PrecacheOtherWeapon("weapon_smokegrenade");
	UTIL_PrecacheOtherWeapon("weapon_flashbang");

	UTIL_PrecacheOtherWeapon("grenade_hegrenade");
	UTIL_PrecacheOtherWeapon("grenade_flashbang");
}

void CBasePlayerWeapon::Spawn(void)
{
	pev->solid = SOLID_NOT;
	pev->effects = EF_NODRAW;
	pev->modelindex = 0;
	pev->model = 0;
	pev->nextthink = -1;

	SET_ORIGIN(edict(), pev->origin);
	SET_SIZE(edict(), g_vecZero, g_vecZero);
}

BOOL CBasePlayerWeapon::CanDeploy(void)
{
	return TRUE;
}

void CBasePlayerWeapon::Deploy(void)
{
}

BOOL CBasePlayerWeapon::CanHolster(void)
{
	return TRUE;
}

void CBasePlayerWeapon::Holster(void)
{
	m_pPlayer->pev->viewmodel = 0;
	m_pPlayer->pev->weaponmodel = 0;

	SetThink(NULL);
	SetTouch(NULL);

	pev->nextthink = -1;
}

BOOL CBasePlayerWeapon::CanDrop(void)
{
	return TRUE;
}

void CBasePlayerWeapon::GetDropPunchAngle(float &base, float &modifier)
{
	base = m_Info.m_DropPunchAngle.flBase;
	modifier = m_Info.m_DropPunchAngle.flModifier;
}

void CBasePlayerWeapon::ItemPreFrame(void)
{
}

void CBasePlayerWeapon::Kill(void)
{
	SetThink(&CBaseEntity::SUB_Remove);
	pev->nextthink = gpGlobals->time + 0.1;
}

void CBasePlayerWeapon::AttachToPlayer(CBasePlayer *pPlayer)
{
	m_pPlayer = pPlayer;

	pev->movetype = MOVETYPE_FOLLOW;
	pev->aiment = pPlayer->edict();
	pev->owner = pPlayer->edict();
}

BOOL CBasePlayerWeapon::DefaultDeploy(int iszViewModel, int iszWeaponModel, int iAnim, const char *szAnimExt, float flDelay)
{
	if (!CanDeploy())
		return FALSE;

	m_pPlayer->pev->viewmodel = MODEL_INDEX(STRING(iszViewModel));
	m_pPlayer->pev->weaponmodel = MODEL_INDEX(STRING(iszWeaponModel));

	strcpy(m_pPlayer->m_szAnimExtention, szAnimExt);

	SendWeaponAnim(iAnim);

	m_flNextPrimaryAttack = flDelay;
	m_flNextSecondaryAttack = flDelay;
	m_flTimeWeaponIdle = flDelay + 0.1;

	m_pPlayer->m_flNextAttack = flDelay;

	return TRUE;
}

BOOL CBasePlayerWeapon::DefaultReload(int iClipSize, int iAnim, float flDelay)
{
	if (m_fInReload)
		return FALSE;

	if (m_iAmmo <= 0)
		return FALSE;

	int j = min(iClipSize - m_iClip, m_iAmmo);

	if (j <= 0)
		return FALSE;

	SendWeaponAnim(iAnim);

	m_pPlayer->m_flNextAttack = flDelay;
	m_flTimeWeaponIdle = flDelay + 0.02;

	m_fInReload = TRUE;

	return TRUE;
}

void CBasePlayerWeapon::ItemPostFrame(void)
{
	if (FBitSet(m_pPlayer->pev->button, IN_FORWARD | IN_BACK | IN_MOVELEFT | IN_MOVERIGHT) && !FBitSet(m_pPlayer->pev->flags, FL_DUCKING))
	{
		if (!m_fRunStart)
		{
			m_flRunStartTime = gpGlobals->time;
			m_fRunStart = TRUE;
		}
	}
	else
	{
		if (m_fRunStart)
		{
			m_fRunStart = FALSE;
		}
	}

	if (FBitSet(m_pPlayer->pev->button, IN_ATTACK2) && m_flNextSecondaryAttack <= 0.0f)
	{
		if (m_pPlayer->m_flNextAttack > 0.0f)
		{
			if (m_fInReload)
			{
				SecondaryAttack();

				if (FBitSet(m_pPlayer->m_afButtonPressed, IN_ATTACK2))
				{
					SingleSecondaryAttack();
				}
			}
		}
		else
		{
			SecondaryAttack();

			if (FBitSet(m_pPlayer->m_afButtonPressed, IN_ATTACK2))
			{
				SingleSecondaryAttack();
			}
		}
	}

	if (m_pPlayer->m_flNextAttack <= 0.0f)
	{
		if (m_fInReload)
		{
			int j = min(m_Info.iAmmoPerMagazine - m_iClip, m_iAmmo);

			m_iClip += j;
			m_iAmmo -= j;

			m_fInReload = FALSE;
		}

		if (m_bResumeZoom && m_flNextPrimaryAttack <= 0.0f)
		{
			m_bResumeZoom = FALSE;
			m_pPlayer->SetFOV(m_iLastZoom, 0.05);
			m_pPlayer->ResetMaxSpeed();
		}

		if (FBitSet(m_pPlayer->pev->button, IN_RELOAD))
		{
			Reload();
		}
		else if (FBitSet(m_pPlayer->pev->button, IN_ATTACK) && m_flNextPrimaryAttack <= 0.0f)
		{
			if (m_iClip <= 0 && m_iAmmo > 0)
			{
				Reload();
			}
			else
			{
				PrimaryAttack();

				if (FBitSet(m_pPlayer->m_afButtonPressed, IN_ATTACK))
				{
					SinglePrimaryAttack();
				}
			}
		}
		else
		{
			if (m_bDelayFire)
			{
				m_bDelayFire = false;

				if (m_iShotsFired > 15)
					m_iShotsFired = 15;

				m_flDecreaseShotsFired = gpGlobals->time + 0.4;
			}

			if (m_iShotsFired > 0)
			{
				if (gpGlobals->time > m_flDecreaseShotsFired)
				{
					m_iShotsFired--;
					m_flDecreaseShotsFired = gpGlobals->time + 0.0225;
				}
			}

			WeaponIdle();

			if (m_fRunStart)
			{
				if (gpGlobals->time - m_flRunStartTime > 0.2f)
				{
					if (!m_fRunStartAnim)
					{
						if (m_flNextPrimaryAttack <= 0.0f && m_flNextSecondaryAttack <= 0.0f && !m_fInSpecialReload)
						{
							SendWeaponAnim(m_Info.m_AnimRun.iSequence);
							m_flTimeWeaponIdle = 60.0f;
						}

						m_fRunStartAnim = TRUE;
					}
				}
			}
			else
			{
				if (m_fRunStartAnim)
				{
					if (m_flNextPrimaryAttack <= 0.0f && m_flNextSecondaryAttack <= 0.0f && !m_fInSpecialReload)
					{
						m_flTimeWeaponIdle = -1.0f;
					}

					m_fRunStartAnim = FALSE;
				}
			}

			if (m_flTimeWeaponIdle <= 0.0f && !m_fInSpecialReload)
			{
				if (m_fRunStartAnim)
				{
					SendWeaponAnim(m_Info.m_AnimRun.iSequence);
					m_flTimeWeaponIdle = 60.0f;
				}
				else
				{
					SendWeaponAnim(m_Info.m_AnimIdle.iSequence);
					m_flTimeWeaponIdle = 60.0f;
				}
			}
		}
	}

	m_flNextPrimaryAttack = max(m_flNextPrimaryAttack - gpGlobals->frametime, -1.0f);
	m_flNextSecondaryAttack = max(m_flNextSecondaryAttack - gpGlobals->frametime, -1.0f);
	m_flTimeWeaponIdle = max(m_flTimeWeaponIdle - gpGlobals->frametime, -1.0f);
	m_flNextRunAnim = max(m_flNextRunAnim - gpGlobals->frametime, -1.0f);
}

void CBasePlayerWeapon::SendWeaponAnim(int iAnim)
{
	CBaseEntity *pEntity = NULL;

	m_pPlayer->pev->weaponanim = iAnim;

	if (!ENGINE_CANSKIP(ENT(m_pPlayer->pev)))
	{
		MESSAGE_BEGIN(MSG_ONE_UNRELIABLE, SVC_WEAPONANIM, NULL, ENT(m_pPlayer->pev));
		WRITE_BYTE(iAnim);
		WRITE_BYTE(0);
		MESSAGE_END();
	}

	// For first person observer
	while ((pEntity = UTIL_FindEntityByClassname(pEntity, "player")) != NULL)
	{
		CBasePlayer *pPlayer = (CBasePlayer *)pEntity;

		if (pPlayer->m_iJoiningState != JOINED)
			continue;

		if (pPlayer == m_pPlayer)
			continue;

		if (pPlayer->m_hSpectatorTarget != m_pPlayer)
			continue;

		if (pPlayer->pev->iuser1 != OBS_IN_EYE)
			continue;

		MESSAGE_BEGIN(MSG_ONE_UNRELIABLE, SVC_WEAPONANIM, NULL, pPlayer->edict());
		WRITE_BYTE(iAnim);
		WRITE_BYTE(0);
		MESSAGE_END();
	}
}

void CBasePlayerWeapon::SinglePrimaryAttack(void)
{
}

void CBasePlayerWeapon::PrimaryAttack(void)
{
}

void CBasePlayerWeapon::SingleSecondaryAttack(void)
{
}

void CBasePlayerWeapon::SecondaryAttack(void)
{
}

void CBasePlayerWeapon::Reload(void)
{
}

void CBasePlayerWeapon::WeaponIdle(void)
{
}

void CBasePlayerWeapon::RetireWeapon(void)
{
	Holster();
}

void CBasePlayerWeapon::KickBack(float up_base, float lateral_base, float up_modifier, float lateral_modifier, float up_max, float lateral_max, int direction_change)
{
	float flFront, flSide;

	if (m_iShotsFired == 1)
	{
		flFront = up_base;
		flSide = lateral_base;
	}
	else
	{
		flFront = m_iShotsFired * up_modifier + up_base;
		flSide = m_iShotsFired * lateral_modifier + lateral_base;
	}

	m_pPlayer->pev->punchangle.x -= flFront;

	if (m_pPlayer->pev->punchangle.x < -up_max)
		m_pPlayer->pev->punchangle.x = -up_max;

	if (m_fDirection == 1)
	{
		m_pPlayer->pev->punchangle.y += flSide;

		if (m_pPlayer->pev->punchangle.y > lateral_max)
			m_pPlayer->pev->punchangle.y = lateral_max;
	}
	else
	{
		m_pPlayer->pev->punchangle.y -= flSide;

		if (m_pPlayer->pev->punchangle.y < -lateral_max)
			m_pPlayer->pev->punchangle.y = -lateral_max;
	}

	if (!RANDOM_LONG(0, direction_change))
	{
		m_fDirection = !m_fDirection;
	}
}

LINK_ENTITY_TO_CLASS(weaponbox, CWeaponBox);

void CWeaponBox::Spawn(void)
{
	Precache();
	
	pev->classname = MAKE_STRING("weaponbox");
	pev->movetype = MOVETYPE_TOSS;
	pev->solid = SOLID_TRIGGER;
	
	pev->iuser1 = 101;		// Identity for client
	pev->iuser2 = 0;	// WeaponID

	UTIL_SetSize(pev, g_vecZero, g_vecZero);
	SET_MODEL(edict(), "models/item/item_supplybox.mdl");
	
	SetThink(&CWeaponBox::Kill);
}

void CWeaponBox::Precache(void)
{
	PRECACHE_MODEL("models/item/item_supplybox.mdl");
}

void CWeaponBox::Touch(CBaseEntity *pOther)
{
	if (!FBitSet(pev->flags, FL_ONGROUND))
		return;

	if (!pOther->IsPlayer())
		return;

	if (!pOther->IsAlive())
		return;

	if (!m_pWeapon)
		return;

	CBasePlayer *pPlayer = (CBasePlayer *)pOther;

	if (pPlayer->AddPlayerWeapon(m_pWeapon))
	{
		m_pWeapon->AttachToPlayer(pPlayer);
	}

	SetTouch(NULL);
	m_pWeapon = NULL;
	
	SetThink(&CBaseEntity::SUB_Remove);
	pev->nextthink = gpGlobals->time + 0.1;
}

BOOL CWeaponBox::IsEmpty(void)
{
	return !m_pWeapon;
}

void CWeaponBox::Kill(void)
{
	if (m_pWeapon)
		m_pWeapon->Kill();

	UTIL_Remove(this);
}

void CWeaponBox::PackWeapon(CBasePlayerWeapon *pWeapon)
{
	if (!IsEmpty())
		return;

	if (pWeapon->m_pPlayer)
	{
		pWeapon->m_pPlayer->RemovePlayerWeapon(pWeapon);
	}

	m_pWeapon = pWeapon;
	pev->iuser2 = pWeapon->m_Info.iWeaponIndex;

	pWeapon->pev->movetype = MOVETYPE_NONE;
	pWeapon->pev->solid = SOLID_NOT;
	pWeapon->pev->effects = EF_NODRAW;
	pWeapon->pev->modelindex = 0;
	pWeapon->pev->model = 0;
	pWeapon->pev->owner = edict();
	pWeapon->SetThink(NULL);
	pWeapon->SetTouch(NULL);

	pWeapon->m_pPlayer = NULL;
}