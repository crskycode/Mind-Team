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

#include "cl_dll.h"
#include "player.h"
#include "weapons.h"

//void CKnife::Spawn(void)
//{
//	Precache();
//	CBasePlayerWeapon::Spawn();
//}
//
//void CKnife::Precache(void)
//{
//	m_usKnife = PRECACHE_EVENT(1, "events/knife.sc");
//}
//
//void CKnife::Deploy(void)
//{
//	DefaultDeploy(m_Info.animSelect.iSequence, m_Info.animSelect.flTime);
//}
//
//void CKnife::PrimaryAttack(void)
//{
//}
//
//void CKnife::SecondaryAttack(void)
//{
//}
//
//void CKnife::WeaponIdle(void)
//{
//}