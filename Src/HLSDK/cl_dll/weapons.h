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

#ifndef WEAPON_H
#define WEAPON_H

#define PRECACHE_EVENT	( *gEngfuncs.pfnPrecacheEvent )
#define PLAYBACK_EVENT_FULL	( *gEngfuncs.pfnPlaybackEvent )

#ifndef ClientTime
#define ClientTime	( gEngfuncs.GetClientTime() )
#endif

class CBasePlayer;
class NewKeyValues;

enum WeaponEntityClass
{
	WeaponEntityClass_RIFLE,		// 步枪
	WeaponEntityClass_PISTOL,		// 手枪
	WeaponEntityClass_KNIFE,		// 刀子
	WeaponEntityClass_GRENADE,		// 手雷
};

#include "weapondata.h"

class CBasePlayerWeapon
{
public:
	virtual void Spawn(void);
	virtual void Deploy(void);
	virtual void Holster(void);
	virtual void ItemPostFrame(void);
	virtual BOOL DefaultDeploy(int iAnim, float flDelay);
	virtual BOOL DefaultReload(int iClipSize, int iAnim, float flDelay);
	virtual void SendWeaponAnim(int iAnim);
	virtual void PrimaryAttack(void);
	virtual void SecondaryAttack(void);
	virtual void Reload(void);
	virtual void WeaponIdle(void);

	float m_flNextPrimaryAttack;
	float m_flNextSecondaryAttack;
	float m_flTimeWeaponIdle;

	int m_fInReload;
	int m_fInSpecialReload;

	WeaponData m_Info;

	CBasePlayer *m_pPlayer;

	int m_iEntityType;

	int m_iClip;
	int m_iAmmo;
};

class CKnife : public CBasePlayerWeapon
{
public:
	void Spawn(void);
	void Precache(void);
	void Deploy(void);
	void PrimaryAttack(void);
	void SecondaryAttack(void);
	void WeaponIdle(void);

	unsigned short m_usKnife;
};

class CRifle : public CBasePlayerWeapon
{
public:
	void Spawn(void);
	void Precache(void);
	void Deploy(void);
	void Reload(void);
	void PrimaryAttack(void);
	void SecondaryAttack(void);
	void WeaponIdle(void);

	unsigned short m_usRifle;
};

#endif