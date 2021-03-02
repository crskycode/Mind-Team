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

#ifndef WEAPONS_H
#define WEAPONS_H

class CBasePlayer;

#define WEAPON_NOCLIP	-1

#define WPNSLOT_PRIMARY		0	// Ö÷ÎäÆ÷
#define WPNSLOT_SECONDARY	1	// ¸±ÎäÆ÷
#define WPNSLOT_KNIFE		2	// ½üÉíÎäÆ÷
#define WPNSLOT_GRENADE		3	// Í¶ÖÀÎäÆ÷

enum MoveState
{
	MOVESTATE_NONE,
	MOVESTATE_WALKING,
	MOVESTATE_RUNNING,
	MOVESTATE_JUMPING,
	MOVESTATE_DUCKING,
};

#include "WeaponData.h"

class CBasePlayerWeapon : public CBaseEntity
{
public:
	virtual void Spawn(void);

	virtual void AttachToPlayer(CBasePlayer *pPlayer);

	virtual BOOL CanDeploy(void);
	virtual void Deploy(void);
	virtual BOOL CanHolster(void);
	virtual void Holster(void);

	virtual BOOL CanDrop(void);

	virtual float GetMaxSpeed(void) { return 240; }

	virtual void GetDropPunchAngle(float &base, float &modifier);

	virtual void ItemPreFrame(void);
	virtual void ItemPostFrame(void);

	virtual void Kill(void);

	virtual BOOL DefaultDeploy(int iszViewModel, int iszWeaponModel, int iAnim, const char *szAnimExt, float flDelay);
	virtual BOOL DefaultReload(int iClipSize, int iAnim, float flDelay);

	virtual void SendWeaponAnim(int iAnim);

	virtual void SinglePrimaryAttack(void);
	virtual void PrimaryAttack(void);
	virtual void SingleSecondaryAttack(void);
	virtual void SecondaryAttack(void);
	virtual void Reload(void);
	virtual void WeaponIdle(void);

	virtual void RetireWeapon(void);

	void KickBack(float up_base, float lateral_base, float up_modifier, float lateral_modifier, float up_max, float lateral_max, int direction_change);

	// Slot link
	BOOL m_bSlotActive;

	float m_flNextPrimaryAttack;
	float m_flNextSecondaryAttack;
	float m_flTimeWeaponIdle;

	BOOL m_fSendPrefireAnim;
	BOOL m_fSendPostfireAnim;

	BOOL m_fRunStart;
	float m_flRunStartTime;
	BOOL m_fRunStartAnim;
	float m_flNextRunAnim;

	int m_iContinuousShooting;

	int m_fInReload;
	int m_fInSpecialReload;

	BOOL m_bDelayFire;
	int m_iShotsFired;
	float m_flDecreaseShotsFired;

	float m_flLastFire;

	float m_flAccuracy;

	BOOL m_fDirection;

	// Sniper
	BOOL m_bResumeZoom;
	int m_iLastZoom;

	// Grenade
	BOOL m_bStartThrow;
	BOOL m_bReleaseThrow;

	WeaponData m_Info;

	int GetId(void) { return m_Info.iWeaponIndex; }
	int GetSlot(void) { return m_Info.iTargetSlot; }

	CBasePlayer *m_pPlayer;
	CBasePlayerWeapon *m_pNext;

	int m_iWeaponClass;

	int m_iClip;
	int m_iAmmo;
};

extern short g_sModelIndexFireball;
extern short g_sModelIndexSmoke;
extern short g_sModelIndexWExplosion;

void DecalGunshot(TraceResult &tr, int iBulletType);

void ClearMultiDamage(void);
void ApplyMultiDamage(entvars_t* pevInflictor, entvars_t* pevAttacker);
void AddMultiDamage(entvars_t *pevInflictor, CBaseEntity *pEntity, float flDamage, int bitsDamageType);

typedef struct
{
	CBaseEntity		*pEntity;
	float			amount;
	int				type;
}
MULTIDAMAGE;

extern MULTIDAMAGE gMultiDamage;

class CWeaponBox : public CBaseEntity
{
public:
	virtual void Spawn(void);
	virtual void Precache(void);
	virtual void Touch(CBaseEntity *pOther);

	BOOL IsEmpty(void);
	void Kill(void);
	void PackWeapon(CBasePlayerWeapon *pWeapon);

	CBasePlayerWeapon *m_pWeapon;
};

class CGrenade : public CBaseEntity
{
public:
	virtual void Spawn(void);
	virtual int GetWeaponID(void);

	int m_iWeaponIndex;
};

class CRifle : public CBasePlayerWeapon
{
public:
	void Spawn(void);
	void Precache(void);
	void Deploy(void);
	void Reload(void);
	void PrimaryAttack(void);
	void WeaponIdle(void);

	float GetMaxSpeed(void);

	void RifleFire(float flSpread, float flCycleTime, MoveState iMoveState, bool bFirstShoot);

	int m_iRifleFire;
};

class CSniper : public CBasePlayerWeapon
{
public:
	void Spawn(void);
	void Precache(void);
	void Deploy(void);
	void Holster(void);
	void Reload(void);
	void PrimaryAttack(void);
	void SingleSecondaryAttack(void);
	void WeaponIdle(void);

	float GetMaxSpeed(void);

	void SniperFire(float flSpread, float flCycleTime);

	int m_iSniperFire;
};

class CShotgun : public CBasePlayerWeapon
{
public:
	void Spawn(void);
	void Precache(void);
	void Deploy(void);
	void Reload(void);
	void PrimaryAttack(void);
	void WeaponIdle(void);

	float GetMaxSpeed(void);

	void ShotgunFire(float flSpread, float flCycleTime);

	int m_iShotgunFire;
};

class CPistol : public CBasePlayerWeapon
{
public:
	void Spawn(void);
	void Precache(void);
	void Deploy(void);
	void Reload(void);
	void SinglePrimaryAttack(void);
	void WeaponIdle(void);

	float GetMaxSpeed(void);

	void PistolFire(float flSpread, float flCycleTime);

	int m_iPistolFire;
};

class CKnife : public CBasePlayerWeapon
{
public:
	void Spawn(void);
	void Precache(void);
	void Deploy(void);
	void Holster(void);
	BOOL CanDrop(void);
	void PrimaryAttack(void);
	void SingleSecondaryAttack(void);
	void WeaponIdle(void);

	float GetMaxSpeed(void);

	void Combo1Attack(void);
	void Combo2Attack(void);
	void BigshotAttack(void);

	int m_iComboType;
	float m_flLastPrimaryAttackTime;

	int m_iKnifeAttack;
};

class CHEGrenade : public CBasePlayerWeapon
{
public:
	void Spawn(void);
	void Precache(void);
	void Deploy(void);
	BOOL CanHolster(void);
	void Holster(void);
	BOOL CanDrop(void);
	void PrimaryAttack(void);
	void WeaponIdle(void);

	float GetMaxSpeed(void);

	BOOL m_bCanHolster;
};

class CSmokeGrenade : public CBasePlayerWeapon
{
public:
	void Spawn(void);
	void Precache(void);
	void Deploy(void);
	BOOL CanHolster(void);
	void Holster(void);
	BOOL CanDrop(void);
	void PrimaryAttack(void);
	void WeaponIdle(void);

	float GetMaxSpeed(void);

	BOOL m_bCanHolster;
};

class CFlashbang : public CBasePlayerWeapon
{
public:
	void Spawn(void);
	void Precache(void);
	void Deploy(void);
	BOOL CanHolster(void);
	void Holster(void);
	BOOL CanDrop(void);
	void PrimaryAttack(void);
	void WeaponIdle(void);

	float GetMaxSpeed(void);

	BOOL m_bCanHolster;
};

class CWorldHEGrenade : public CGrenade
{
public:
	void Spawn(void);
	void Precache(void);
	void BounceTouch(CBaseEntity *pOther);
	void Explosion(void);

	static void Create(entvars_t *pevOwner, string_t iszModel, Vector &vecStart, Vector &vecVelocity, float flGravity, float flFriction, float flDelay, int iWeaponIndex);

	int m_iExplosionEvent;
};

class CWorldFlashbang : public CGrenade
{
public:
	void Spawn(void);
	void BounceTouch(CBaseEntity *pOther);
	void Explosion(void);

	static void Create(entvars_t *pevOwner, string_t iszModel, Vector &vecStart, Vector &vecVelocity, float flGravity, float flFriction, float flDelay, int iWeaponIndex);
};

#endif