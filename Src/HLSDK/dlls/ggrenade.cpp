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
#include "weapons.h"

void CGrenade::Spawn(void)
{
}

int CGrenade::GetWeaponID(void)
{
	return m_iWeaponIndex;
}

LINK_ENTITY_TO_CLASS(grenade_hegrenade, CWorldHEGrenade);

void CWorldHEGrenade::Spawn(void)
{
	pev->classname = MAKE_STRING("grenade_hegrenade");
	pev->solid = SOLID_BBOX;
	pev->movetype = MOVETYPE_BOUNCE;

	Precache();
}

void CWorldHEGrenade::Precache(void)
{
	m_iExplosionEvent = PRECACHE_EVENT(1, "events/grenade.sc");
}

void CWorldHEGrenade::BounceTouch(CBaseEntity *pOther)
{
	if (pev->flags & FL_ONGROUND)
	{
		//EmitSound
	}

	pev->velocity *= pev->friction - 0.1;
}

void CWorldHEGrenade::Explosion(void)
{
	/*MESSAGE_BEGIN(MSG_PAS, SVC_TEMPENTITY);
	WRITE_BYTE(TE_EXPLOSION);
	WRITE_COORD(pev->origin.x);
	WRITE_COORD(pev->origin.y);
	WRITE_COORD(pev->origin.z);
	WRITE_SHORT(g_sModelIndexWExplosion);
	WRITE_BYTE(50);
	WRITE_BYTE(30);
	WRITE_BYTE(0);
	MESSAGE_END();*/

	PLAYBACK_EVENT_FULL(FEV_RELIABLE, edict(), m_iExplosionEvent, 0.0, g_vecZero, g_vecZero, 0.0, 0.0, m_iWeaponIndex, 0, FALSE, FALSE);

	SetThink(&CBaseEntity::SUB_Remove);
	pev->nextthink = gpGlobals->time + 0.1;
}

void CWorldHEGrenade::Create(entvars_t *pevOwner, string_t iszModel, Vector &vecStart, Vector &vecVelocity, float flGravity, float flFriction, float flDelay, int iWeaponIndex)
{
	CWorldHEGrenade *pGrenade = GetClassPtr<CWorldHEGrenade>(NULL);

	pGrenade->Spawn();

	SET_ORIGIN(pGrenade->edict(), vecStart);

	pGrenade->pev->angles = pevOwner->angles;
	pGrenade->pev->velocity = vecVelocity;
	pGrenade->pev->avelocity = Vector(20, 20, 0);
	pGrenade->pev->owner = ENT(pevOwner);
	pGrenade->pev->gravity = flGravity;
	pGrenade->pev->friction = flFriction;

	pGrenade->m_iWeaponIndex = iWeaponIndex;

	SET_MODEL(pGrenade->edict(), STRING(iszModel));

	pGrenade->SetTouch(&CWorldHEGrenade::BounceTouch);
	pGrenade->SetThink(&CWorldHEGrenade::Explosion);

	pGrenade->pev->nextthink = gpGlobals->time + flDelay;

	MESSAGE_BEGIN(MSG_BROADCAST, SVC_TEMPENTITY);
	WRITE_BYTE(TE_BEAMFOLLOW);
	WRITE_SHORT(pGrenade->entindex());
	WRITE_SHORT(g_sModelIndexSmoke);
	WRITE_BYTE(5);
	WRITE_BYTE(1);
	WRITE_BYTE(200);
	WRITE_BYTE(200);
	WRITE_BYTE(200);
	WRITE_BYTE(200);
	MESSAGE_END();
}

LINK_ENTITY_TO_CLASS(grenade_flashbang, CWorldFlashbang);

void CWorldFlashbang::Spawn(void)
{
	pev->classname = MAKE_STRING("grenade_flashbang");
	pev->solid = SOLID_BBOX;
	pev->movetype = MOVETYPE_BOUNCE;
}

void CWorldFlashbang::BounceTouch(CBaseEntity *pOther)
{
	if (pev->flags & FL_ONGROUND)
	{
		//EmitSound
	}

	pev->velocity *= pev->friction - 0.1;
}

void CWorldFlashbang::Explosion(void)
{
	UTIL_RadiusFlash(pev->origin, pev, 400.0, 550.0, 3.0);
	SetThink(&CBaseEntity::SUB_Remove);
	pev->nextthink = gpGlobals->time + 0.1;
}

void CWorldFlashbang::Create(entvars_t *pevOwner, string_t iszModel, Vector &vecStart, Vector &vecVelocity, float flGravity, float flFriction, float flDelay, int iWeaponIndex)
{
	CWorldFlashbang *pGrenade = GetClassPtr<CWorldFlashbang>(NULL);

	pGrenade->Spawn();

	SET_ORIGIN(pGrenade->edict(), vecStart);

	pGrenade->pev->angles = pevOwner->angles;
	pGrenade->pev->velocity = vecVelocity;
	pGrenade->pev->avelocity = Vector(20, 20, 0);
	pGrenade->pev->owner = ENT(pevOwner);
	pGrenade->pev->gravity = flGravity;
	pGrenade->pev->friction = flFriction;

	pGrenade->m_iWeaponIndex = iWeaponIndex;

	SET_MODEL(pGrenade->edict(), STRING(iszModel));

	pGrenade->SetTouch(&CWorldFlashbang::BounceTouch);
	pGrenade->SetThink(&CWorldFlashbang::Explosion);

	pGrenade->pev->nextthink = gpGlobals->time + flDelay;

	MESSAGE_BEGIN(MSG_BROADCAST, SVC_TEMPENTITY);
	WRITE_BYTE(TE_BEAMFOLLOW);
	WRITE_SHORT(pGrenade->entindex());
	WRITE_SHORT(g_sModelIndexSmoke);
	WRITE_BYTE(5);
	WRITE_BYTE(1);
	WRITE_BYTE(200);
	WRITE_BYTE(200);
	WRITE_BYTE(200);
	WRITE_BYTE(200);
	MESSAGE_END();
}