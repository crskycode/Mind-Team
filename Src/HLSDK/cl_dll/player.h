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

#ifndef PLAYER_H
#define PLAYER_H

#include <progdefs.h>

class CBasePlayerWeapon;

class CBasePlayer
{
public:
	CBasePlayer();

	entvars_t pev;

	BOOL IsAlive(void);
	void Spawn(void);
	void Killed(void);
	void ItemPostFrame(void);

	Vector GetGunPosition(void);

	CBasePlayerWeapon *m_pActiveWeapon;

	int m_iWeaponEntityIndex;	/* 武器实体ID */
	int m_iWeaponEntityClass;	/* 武器实体类型 */
	int m_iWeaponIndex;			/* 武器ID */

	int random_seed;

	int m_afButtonLast;
	int m_afButtonPressed;
	int m_afButtonReleased;

	float m_flNextAttack;
	float m_flNextAttackServer;
};

#endif