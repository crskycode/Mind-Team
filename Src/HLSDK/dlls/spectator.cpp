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
#include "message.h"
#include <in_buttons.h>
#include <pm_shared.h>

CBaseEntity *CBasePlayer::Spectator_IsValidTarget(int iTarget)
{
	if (iTarget < 1 || iTarget > gpGlobals->maxClients)
		return NULL;

	CBasePlayer *pTarget = UTIL_PlayerByIndex(iTarget);

	if (!pTarget || pTarget->m_bDisconnected || pTarget->IsObserver() || pTarget == this)
		return NULL;

	return pTarget;
}

void CBasePlayer::Spectator_FindNextPlayer(bool bReverse)
{
	int iStartId;
	int iCurrentId;
	int iDir;

	iStartId = m_hSpectatorTarget ? m_hSpectatorTarget->entindex() : this->GetId();
	iCurrentId = iStartId;

	m_hSpectatorTarget.Set(NULL);

	iDir = bReverse ? -1 : 1;

	do
	{
		iCurrentId += iDir;

		if (iCurrentId > gpGlobals->maxClients)
			iCurrentId = 1;
		else if (iCurrentId < 1)
			iCurrentId = gpGlobals->maxClients;

		m_hSpectatorTarget = (CBasePlayer *)Spectator_IsValidTarget(iCurrentId);

		if (m_hSpectatorTarget)
			break;
	}
	while (iCurrentId != iStartId);

	if (m_hSpectatorTarget)
	{
		UTIL_SetOrigin(pev, m_hSpectatorTarget->pev->origin);

		if (pev->iuser1 != OBS_ROAMING)
		{
			pev->iuser2 = m_hSpectatorTarget->entindex();
		}
	}
}

void CBasePlayer::Spectator_SetMode(int iMode)
{
	if (iMode == pev->iuser1)
		return;

	if (iMode < OBS_CHASE_LOCKED || iMode > OBS_MAP_CHASE)
		iMode = OBS_IN_EYE;

	if (m_hSpectatorTarget)
	{
		CBasePlayer *pl = (CBasePlayer *)CBaseEntity::Instance(m_hSpectatorTarget.Get());

		if (pl->m_bDisconnected || pl->IsObserver())
			m_hSpectatorTarget = NULL;
	}

	pev->iuser1 = iMode;

	if (iMode != OBS_ROAMING)
	{
		if (!m_hSpectatorTarget)
		{
			Spectator_FindNextPlayer(false);

			if (!m_hSpectatorTarget)
			{
				pev->iuser1 = OBS_ROAMING;
			}
		}
	}

	if (pev->iuser1 != OBS_ROAMING)
	{
		if (m_hSpectatorTarget)
		{
			pev->iuser2 = m_hSpectatorTarget->entindex();
		}
	}
	else
	{
		pev->iuser2 = 0;
	}

	if (m_hSpectatorTarget)
	{
		UTIL_SetOrigin(pev, m_hSpectatorTarget->pev->origin);
	}

	m_iSpectatorLastMode = iMode;
}

void CBasePlayer::Spectator_HandleButtons(void)
{
	if (gpGlobals->time < m_fSpectatorNextInput)
		return;

	if (m_afButtonPressed & IN_JUMP)
	{
		switch (pev->iuser1)
		{
			case OBS_NONE:
			case OBS_CHASE_LOCKED:
			case OBS_CHASE_FREE:
			case OBS_ROAMING:
			case OBS_MAP_FREE:
			case OBS_MAP_CHASE:
			{
				Spectator_SetMode(OBS_IN_EYE);
				break;
			}
			case OBS_IN_EYE:
			{
				Spectator_SetMode(OBS_CHASE_FREE);
				break;
			}
		}

		m_fSpectatorNextInput = gpGlobals->time + 0.2;
	}

	if (m_afButtonPressed & IN_ATTACK)
	{
		Spectator_FindNextPlayer(false);
		m_fSpectatorNextInput = gpGlobals->time + 0.2;
	}

	if (m_afButtonPressed & IN_ATTACK2)
	{
		Spectator_FindNextPlayer(true);
		m_fSpectatorNextInput = gpGlobals->time + 0.2;
	}
}

void CBasePlayer::Spectator_CheckTarget(void)
{
	if (pev->iuser1 == OBS_ROAMING)
		return;

	if (!m_hSpectatorTarget)
	{
		Spectator_FindNextPlayer(false);
	}

	if (m_hSpectatorTarget)
	{
		CBasePlayer *pl = (CBasePlayer *)CBaseEntity::Instance(m_hSpectatorTarget.Get());

		if (pl->m_bDisconnected || pl->pev->deadflag == DEAD_DEAD)
		{
			Spectator_FindNextPlayer(false);
		}
	}
	else
	{
		Spectator_SetMode(OBS_ROAMING);
	}
}

void CBasePlayer::Spectator_CheckProperties(void)
{
	if (m_hSpectatorTarget)
	{
		CBasePlayer *pl = (CBasePlayer *)CBaseEntity::Instance(m_hSpectatorTarget.Get());

		if (m_iSpectatorTarget != pl->entindex())
		{
			m_iSpectatorTarget = pl->entindex();

			MESSAGE_BEGIN(MSG_ONE, gmsgSpecTarget, NULL, ENT(pev));
			WRITE_BYTE(m_iSpectatorTarget);
			MESSAGE_END();	
		}

		if (m_iSpectatorHealth != pl->pev->health)
		{
			m_iSpectatorHealth = pl->pev->health;

			MESSAGE_BEGIN(MSG_ONE, gmsgSpecHealth, NULL, ENT(pev));
			WRITE_SHORT(m_iSpectatorHealth);
			MESSAGE_END();
		}

		if (m_iSpectatorFOV != pl->m_iFOV)
		{
			m_iSpectatorFOV = pl->m_iFOV;

			MESSAGE_BEGIN(MSG_ONE, gmsgSetFOV, NULL, ENT(pev));
			WRITE_BYTE(m_iSpectatorFOV);
			WRITE_SHORT(pl->m_fFOVChangeTime * 1000);
			MESSAGE_END();
		}

		if (m_iSpectatorWeapon != pl->GetActiveWeaponId())
		{
			m_iSpectatorWeapon = pl->GetActiveWeaponId();

			MESSAGE_BEGIN(MSG_ONE, gmsgSpecWeapon, NULL, ENT(pev));
			WRITE_SHORT(m_iSpectatorWeapon);
			MESSAGE_END();
		}
	}
	else
	{
		if (m_iSpectatorTarget)
		{
			m_iSpectatorTarget = 0;

			MESSAGE_BEGIN(MSG_ONE, gmsgSpecTarget, NULL, ENT(pev));
			WRITE_BYTE(m_iSpectatorTarget);
			MESSAGE_END();	
		}

		if (m_iSpectatorHealth)
		{
			m_iSpectatorHealth = 0;

			MESSAGE_BEGIN(MSG_ONE, gmsgSpecHealth, NULL, ENT(pev));
			WRITE_SHORT(m_iSpectatorHealth);
			MESSAGE_END();
		}

		if (m_iSpectatorFOV != 90)
		{
			m_iSpectatorFOV = 90;

			MESSAGE_BEGIN(MSG_ONE, gmsgSetFOV, NULL, ENT(pev));
			WRITE_BYTE(m_iSpectatorFOV);
			WRITE_SHORT(0);
			MESSAGE_END();
		}

		if (m_iSpectatorWeapon)
		{
			m_iSpectatorWeapon = 0;

			MESSAGE_BEGIN(MSG_ONE, gmsgSpecWeapon, NULL, ENT(pev));
			WRITE_SHORT(m_iSpectatorWeapon);
			MESSAGE_END();
		}
	}
}