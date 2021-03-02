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

#ifndef WEAPONDATA_H
#define WEAPONDATA_H

enum WeaponClass
{
	WeaponClass_Rifle = 1,
	WeaponClass_Sniper,
	WeaponClass_Shotgun,
	WeaponClass_Pistol,
	WeaponClass_Knife,
	WeaponClass_HEGrenade,
	WeaponClass_SmokeGrenade,
	WeaponClass_Flashbang,
};

struct WeaponData
{
	struct AnimInfo
	{
		int		iSequence;
		float	flTime;
	};

	struct ViewInfo
	{
		vec3_t	vecPosition;
		vec3_t	vecRotate;
		float	flFOV;
		float	flAspect;
	};

	struct KnifeAttackInfo
	{
		float	flAttackIntervalTime;
		float	flDamageDelay;
		float	flDamageValue;
		float	flAttackRange;
		float	flAttackAngle;
	};

	struct FireBulletsInfo
	{
		float	flDistance;
		int		iPenetration;
		int		iPenetrationPower;
		int		iPenetrationDistance;
		int		iDamage;
		float	flRangeModifier;
		int		iShots;
	};

	struct AccuracyInfo
	{
		float	flSpreadBase;
		float	flSpreadModifier;
		float	flAccuracyBase;
		float	flAccuracyModifier;
		float	flAccuracyMin;
		float	flAccuracyMax;
	};

	struct DropPunchAngleInfo
	{
		float	flBase;
		float	flModifier;
	};

	struct KickBackInfo
	{
		float	flUpBase;
		float	flLateralBase;
		float	flUpModifier;
		float	flLateralModifier;
		float	flUpMax;
		float	flLateralMax;
		int		iDirectionChange;
	};

	struct GrenadeThrowInfo
	{
		float	flVelocity;
		float	flAngle;
		float	flGravity;
		float	flFriction;
		float	flAirTime;
	};

	int			iWeaponIndex;
	char		szWeaponClass[32];
	int			iWeaponClass;
	char		szWeaponName[64];
	char		szModelFileName[MAX_QPATH];
	char		szSkinFileName[MAX_QPATH];
	char		szSkinFileName2[MAX_QPATH];
	char		szPViewModelFileName[MAX_QPATH];
	char		szPViewSkinFileName[MAX_QPATH];
	char		szPViewSkinFileName2[MAX_QPATH];
	char		szGViewModelFileName[MAX_QPATH];
	char		szGViewSkinFileName[MAX_QPATH];
	char		szGViewSkinFileName2[MAX_QPATH];
	char		szGViewAnimName[32];
	char		szKnifeMetalSoundName[32];
	char		szKnifeStoneSoundName[32];
	char		szKnifeWoodSoundName[32];
	char		szKnifeBrokenGlassSoundName[32];
	char		szKnifeCarpetSoundName[32];
	char		szKnifeSnowSoundName[32];
	int			iTargetSlot;
	float		flMaxMoveSpeed;
	float		flMaxMoveSpeedInZoom;
	char		szBigIconName[MAX_QPATH];
	char		szSmallIconName[MAX_QPATH];
	char		szSniperCrosshairFileName[MAX_QPATH];
	char		szShotSoundName[64];
	float		flChangeWeaponAnimRatio;
	float		flReloadAnimRatio;
	int			iAmmoPerMagazine;
	int			iMaxAmmo;
	float		flShotIntervalTime;

	KnifeAttackInfo		m_KnifeCombo1Attack;
	KnifeAttackInfo		m_KnifeCombo2Attack;
	KnifeAttackInfo		m_KnifeBigshotAttack;

	FireBulletsInfo		m_FireBullets;

	AccuracyInfo	m_AccuracyDefault;
	AccuracyInfo	m_AccuracyJumping;
	AccuracyInfo	m_AccuracyDucking;
	AccuracyInfo	m_AccuracyRunning;

	DropPunchAngleInfo	m_DropPunchAngle;

	KickBackInfo	m_KickBackDefault;
	KickBackInfo	m_KickBackJumping;
	KickBackInfo	m_KickBackDucking;
	KickBackInfo	m_KickBackRunning;

	GrenadeThrowInfo	m_GrenadeThrow;

	ViewInfo	m_ViewInfo;

	AnimInfo	m_AnimIdle;
	AnimInfo	m_AnimSelect;
	AnimInfo	m_AnimReload;
	AnimInfo	m_AnimStartReload;
	AnimInfo	m_AnimEndReload;
	AnimInfo	m_AnimPrefire;
	AnimInfo	m_AnimPostfire;
	AnimInfo	m_AnimFire;
	AnimInfo	m_AnimRun;
	AnimInfo	m_AnimCombo1;
	AnimInfo	m_AnimCombo2;
	AnimInfo	m_AnimBigshot;
	AnimInfo	m_AnimStartThrow;
	AnimInfo	m_AnimThrow;
};

WeaponData *WeaponData_GetData(int index);

#endif