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
#include "newkeyvalues.h"
#include "cmdparse.h"
#include "WeaponData.h"

#define MAX_WEAPONS	1024

static int			gWeaponCount = 0;
static WeaponData	gWeaponData[MAX_WEAPONS];

int ParseWeaponClass(const char *string)
{
	if (!strcmp(string, "weapon_rifle"))
		return WeaponClass_Rifle;
	if (!strcmp(string, "weapon_sniper"))
		return WeaponClass_Sniper;
	if (!strcmp(string, "weapon_shotgun"))
		return WeaponClass_Shotgun;
	if (!strcmp(string, "weapon_pistol"))
		return WeaponClass_Pistol;
	if (!strcmp(string, "weapon_knife"))
		return WeaponClass_Knife;
	if (!strcmp(string, "weapon_hegrenade"))
		return WeaponClass_HEGrenade;
	if (!strcmp(string, "weapon_smokegrenade"))
		return WeaponClass_SmokeGrenade;
	if (!strcmp(string, "weapon_flashbang"))
		return WeaponClass_Flashbang;

	return 0;
}

void WeaponData_Init(void)
{
	char	*buffer;
	int		length;

	NewKeyValues	*pkvd;

	buffer = (char *)gEngfuncs.COM_LoadFile("configs/weapon.vdf", 5, &length);

	if (!buffer)
	{
		gEngfuncs.Con_Printf("Error: Couldn't to load weapon !\n");
		return;
	}

	buffer[length] = '\0';

	pkvd = new NewKeyValues("Weapon");

	if (!pkvd->LoadFromBuffer(buffer))
	{
		gEngfuncs.Con_Printf("Error: Couldn't to parse weapon !\n");
	}

	gEngfuncs.COM_FreeFile(buffer);

	gWeaponCount = 0;
	memset(&gWeaponData, 0, sizeof(gWeaponData));

	for (NewKeyValues *dat = pkvd; dat; dat = dat->GetNextKey())
	{
		int iWeaponClass = ParseWeaponClass(dat->GetString("WeaponClass"));

		if (iWeaponClass)
		{
			if (dat->GetInt("WeaponIndex") < 1)
				continue;

			WeaponData &info = gWeaponData[gWeaponCount++];

			info.iWeaponIndex = dat->GetInt("WeaponIndex");

			strcpy(info.szWeaponClass, dat->GetString("WeaponClass"));
			info.iWeaponClass = iWeaponClass;

			strcpy(info.szWeaponName, dat->GetString("WeaponName"));

			strcpy(info.szModelFileName, dat->GetString("ModelFileName"));
			strcpy(info.szSkinFileName, dat->GetString("SkinFileName"));
			strcpy(info.szSkinFileName2, dat->GetString("SkinFileName2"));

			strcpy(info.szPViewModelFileName, dat->GetString("PViewModelFileName"));
			strcpy(info.szPViewSkinFileName, dat->GetString("PViewSkinFileName"));
			strcpy(info.szPViewSkinFileName2, dat->GetString("PViewSkinFileName2"));

			strcpy(info.szGViewModelFileName, dat->GetString("GViewModelFileName"));
			strcpy(info.szGViewSkinFileName, dat->GetString("GViewSkinFileName"));
			strcpy(info.szGViewSkinFileName2, dat->GetString("GViewSkinFileName2"));

			strcpy(info.szGViewAnimName, dat->GetString("GViewAnimName"));

			info.iTargetSlot = dat->GetInt("TargetSlot");

			info.flMaxMoveSpeed = dat->GetFloat("MaxMoveSpeed");

			strcpy(info.szBigIconName, dat->GetString("SmallIconName"));
			strcpy(info.szSmallIconName, dat->GetString("SmallIconName"));

			if (info.iWeaponClass == WeaponClass_Rifle)
			{
		// SOUND
				strcpy(info.szShotSoundName, dat->GetString("ShotSoundName"));

		// ANIMATING
				info.flChangeWeaponAnimRatio	= dat->GetFloat("ChangeWeaponAnimRatio");
				info.flReloadAnimRatio			= dat->GetFloat("ReloadAnimRatio");

		// ANIMATION SEQUENCE
				Cmd_Init(dat->GetString("PViewModelAnimSequence"));
				info.m_AnimIdle.iSequence		= atoi(Cmd_Argv(0));
				info.m_AnimSelect.iSequence		= atoi(Cmd_Argv(1));
				info.m_AnimReload.iSequence		= atoi(Cmd_Argv(2));
				info.m_AnimPrefire.iSequence	= atoi(Cmd_Argv(3));
				info.m_AnimPostfire.iSequence	= atoi(Cmd_Argv(4));
				info.m_AnimFire.iSequence		= atoi(Cmd_Argv(5));
				info.m_AnimRun.iSequence		= atoi(Cmd_Argv(6));

		// ANIMATION TIME
				Cmd_Init(dat->GetString("PViewModelAnimTime"));
				info.m_AnimIdle.flTime		= atof(Cmd_Argv(0));
				info.m_AnimSelect.flTime	= atof(Cmd_Argv(1));
				info.m_AnimReload.flTime	= atof(Cmd_Argv(2));
				info.m_AnimPrefire.flTime	= atof(Cmd_Argv(3));
				info.m_AnimPostfire.flTime	= atof(Cmd_Argv(4));
				info.m_AnimFire.flTime		= atof(Cmd_Argv(5));
				info.m_AnimRun.flTime		= atof(Cmd_Argv(6));

		// AMMO
				info.iMaxAmmo			= dat->GetInt("MaxAmmo");
				info.iAmmoPerMagazine	= dat->GetInt("AmmoPerMagazine");

		// FIRE
				info.flShotIntervalTime	= dat->GetFloat("ShotIntervalTime");

		// FIREBULLETS
				Cmd_Init(dat->GetString("FireBullets"));
				info.m_FireBullets.iPenetration			= atoi(Cmd_Argv(0));
				info.m_FireBullets.iPenetrationPower	= atoi(Cmd_Argv(1));
				info.m_FireBullets.iPenetrationDistance	= atoi(Cmd_Argv(2));
				info.m_FireBullets.iDamage				= atoi(Cmd_Argv(3));
				info.m_FireBullets.flRangeModifier		= atof(Cmd_Argv(4));

		// ACCURACY
				Cmd_Init(dat->GetString("AccuracyDefault"));
				info.m_AccuracyDefault.flSpreadBase			= atof(Cmd_Argv(0));
				info.m_AccuracyDefault.flSpreadModifier		= atof(Cmd_Argv(1));
				info.m_AccuracyDefault.flAccuracyBase		= atof(Cmd_Argv(2));
				info.m_AccuracyDefault.flAccuracyModifier	= atof(Cmd_Argv(3));
				info.m_AccuracyDefault.flAccuracyMax		= atof(Cmd_Argv(4));

				Cmd_Init(dat->GetString("AccuracyJump"));
				info.m_AccuracyJumping.flSpreadBase			= atof(Cmd_Argv(0));
				info.m_AccuracyJumping.flSpreadModifier		= atof(Cmd_Argv(1));
				info.m_AccuracyJumping.flAccuracyBase			= atof(Cmd_Argv(2));
				info.m_AccuracyJumping.flAccuracyModifier		= atof(Cmd_Argv(3));
				info.m_AccuracyJumping.flAccuracyMax			= atof(Cmd_Argv(4));

				Cmd_Init(dat->GetString("AccuracyDucking"));
				info.m_AccuracyDucking.flSpreadBase			= atof(Cmd_Argv(0));
				info.m_AccuracyDucking.flSpreadModifier		= atof(Cmd_Argv(1));
				info.m_AccuracyDucking.flAccuracyBase		= atof(Cmd_Argv(2));
				info.m_AccuracyDucking.flAccuracyModifier	= atof(Cmd_Argv(3));
				info.m_AccuracyDucking.flAccuracyMax		= atof(Cmd_Argv(4));

				Cmd_Init(dat->GetString("AccuracyMove"));
				info.m_AccuracyRunning.flSpreadBase			= atof(Cmd_Argv(0));
				info.m_AccuracyRunning.flSpreadModifier		= atof(Cmd_Argv(1));
				info.m_AccuracyRunning.flAccuracyBase			= atof(Cmd_Argv(2));
				info.m_AccuracyRunning.flAccuracyModifier		= atof(Cmd_Argv(3));
				info.m_AccuracyRunning.flAccuracyMax			= atof(Cmd_Argv(4));

		// PUNCHANGLE
				Cmd_Init(dat->GetString("DropPunchAngle"));
				info.m_DropPunchAngle.flBase		= atof(Cmd_Argv(0));
				info.m_DropPunchAngle.flModifier	= atof(Cmd_Argv(1));

		// KICKBACK
				Cmd_Init(dat->GetString("KickBackDefault"));
				info.m_KickBackDefault.flUpBase				= atof(Cmd_Argv(0));
				info.m_KickBackDefault.flLateralBase		= atof(Cmd_Argv(1));
				info.m_KickBackDefault.flUpModifier			= atof(Cmd_Argv(2));
				info.m_KickBackDefault.flLateralModifier	= atof(Cmd_Argv(3));
				info.m_KickBackDefault.flUpMax				= atof(Cmd_Argv(4));
				info.m_KickBackDefault.flLateralMax			= atof(Cmd_Argv(5));
				info.m_KickBackDefault.iDirectionChange		= atoi(Cmd_Argv(6));

				Cmd_Init(dat->GetString("KickBackJump"));
				info.m_KickBackJumping.flUpBase				= atof(Cmd_Argv(0));
				info.m_KickBackJumping.flLateralBase			= atof(Cmd_Argv(1));
				info.m_KickBackJumping.flUpModifier			= atof(Cmd_Argv(2));
				info.m_KickBackJumping.flLateralModifier		= atof(Cmd_Argv(3));
				info.m_KickBackJumping.flUpMax					= atof(Cmd_Argv(4));
				info.m_KickBackJumping.flLateralMax			= atof(Cmd_Argv(5));
				info.m_KickBackJumping.iDirectionChange		= atoi(Cmd_Argv(6));

				Cmd_Init(dat->GetString("KickBackDucking"));
				info.m_KickBackDucking.flUpBase				= atof(Cmd_Argv(0));
				info.m_KickBackDucking.flLateralBase		= atof(Cmd_Argv(1));
				info.m_KickBackDucking.flUpModifier			= atof(Cmd_Argv(2));
				info.m_KickBackDucking.flLateralModifier	= atof(Cmd_Argv(3));
				info.m_KickBackDucking.flUpMax				= atof(Cmd_Argv(4));
				info.m_KickBackDucking.flLateralMax			= atof(Cmd_Argv(5));
				info.m_KickBackDucking.iDirectionChange		= atoi(Cmd_Argv(6));

				Cmd_Init(dat->GetString("KickBackMove"));
				info.m_KickBackRunning.flUpBase				= atof(Cmd_Argv(0));
				info.m_KickBackRunning.flLateralBase			= atof(Cmd_Argv(1));
				info.m_KickBackRunning.flUpModifier			= atof(Cmd_Argv(2));
				info.m_KickBackRunning.flLateralModifier		= atof(Cmd_Argv(3));
				info.m_KickBackRunning.flUpMax					= atof(Cmd_Argv(4));
				info.m_KickBackRunning.flLateralMax			= atof(Cmd_Argv(5));
				info.m_KickBackRunning.iDirectionChange		= atoi(Cmd_Argv(6));

			}
			else if (info.iWeaponClass == WeaponClass_Sniper)
			{
		// FOR SNIPER
				info.flMaxMoveSpeedInZoom = dat->GetFloat("MaxMoveSpeedInZoom");

		// EXTRA TEXTURE
				strcpy(info.szSniperCrosshairFileName, dat->GetString("SniperCrosshairFileName"));

		// SOUND
				strcpy(info.szShotSoundName, dat->GetString("ShotSoundName"));

		// ANIMATING
				info.flChangeWeaponAnimRatio	= dat->GetFloat("ChangeWeaponAnimRatio");
				info.flReloadAnimRatio			= dat->GetFloat("ReloadAnimRatio");

		// ANIMATION SEQUENCE
				Cmd_Init(dat->GetString("PViewModelAnimSequence"));
				info.m_AnimIdle.iSequence	= atoi(Cmd_Argv(0));
				info.m_AnimSelect.iSequence	= atoi(Cmd_Argv(1));
				info.m_AnimReload.iSequence	= atoi(Cmd_Argv(2));
				info.m_AnimFire.iSequence	= atoi(Cmd_Argv(3));
				info.m_AnimRun.iSequence	= atoi(Cmd_Argv(4));

		// ANIMATION TIME
				Cmd_Init(dat->GetString("PViewModelAnimTime"));
				info.m_AnimIdle.flTime		= atof(Cmd_Argv(0));
				info.m_AnimSelect.flTime	= atof(Cmd_Argv(1));
				info.m_AnimReload.flTime	= atof(Cmd_Argv(2));
				info.m_AnimFire.flTime		= atof(Cmd_Argv(3));
				info.m_AnimRun.flTime		= atof(Cmd_Argv(4));

		// AMMO
				info.iMaxAmmo			= dat->GetInt("MaxAmmo");
				info.iAmmoPerMagazine	= dat->GetInt("AmmoPerMagazine");

		// FIRE
				info.flShotIntervalTime	= dat->GetFloat("ShotIntervalTime");

		// FIREBULLETS
				Cmd_Init(dat->GetString("FireBullets"));
				info.m_FireBullets.iPenetration			= atoi(Cmd_Argv(0));
				info.m_FireBullets.iPenetrationPower	= atoi(Cmd_Argv(1));
				info.m_FireBullets.iPenetrationDistance	= atoi(Cmd_Argv(2));
				info.m_FireBullets.iDamage				= atoi(Cmd_Argv(3));
				info.m_FireBullets.flRangeModifier		= atof(Cmd_Argv(4));

		// ACCURACY
				Cmd_Init(dat->GetString("AccuracyDefault"));
				info.m_AccuracyDefault.flSpreadBase	= atof(Cmd_Argv(0));

				Cmd_Init(dat->GetString("AccuracyJump"));
				info.m_AccuracyJumping.flSpreadBase	= atof(Cmd_Argv(0));

				Cmd_Init(dat->GetString("AccuracyDucking"));
				info.m_AccuracyDucking.flSpreadBase	= atof(Cmd_Argv(0));

				Cmd_Init(dat->GetString("AccuracyMove"));
				info.m_AccuracyRunning.flSpreadBase	= atof(Cmd_Argv(0));

		// PUNCHANGLE
				Cmd_Init(dat->GetString("DropPunchAngle"));
				info.m_DropPunchAngle.flBase		= atof(Cmd_Argv(0));
				info.m_DropPunchAngle.flModifier	= atof(Cmd_Argv(1));

		// KICKBACK
				Cmd_Init(dat->GetString("KickBackDefault"));
				info.m_KickBackDefault.flUpBase	= atof(Cmd_Argv(0));

				Cmd_Init(dat->GetString("KickBackJump"));
				info.m_KickBackJumping.flUpBase	= atof(Cmd_Argv(0));

				Cmd_Init(dat->GetString("KickBackDucking"));
				info.m_KickBackDucking.flUpBase	= atof(Cmd_Argv(0));

				Cmd_Init(dat->GetString("KickBackMove"));
				info.m_KickBackRunning.flUpBase	= atof(Cmd_Argv(0));

			}
			else if (info.iWeaponClass == WeaponClass_Shotgun)
			{
		// SOUND
				strcpy(info.szShotSoundName, dat->GetString("ShotSoundName"));

		// ANIMATION SEQUENCE
				Cmd_Init(dat->GetString("PViewModelAnimSequence"));
				info.m_AnimIdle.iSequence			= atoi(Cmd_Argv(0));
				info.m_AnimSelect.iSequence			= atoi(Cmd_Argv(1));
				info.m_AnimStartReload.iSequence	= atoi(Cmd_Argv(2));
				info.m_AnimReload.iSequence			= atoi(Cmd_Argv(3));
				info.m_AnimEndReload.iSequence		= atoi(Cmd_Argv(4));
				info.m_AnimFire.iSequence			= atoi(Cmd_Argv(5));
				info.m_AnimRun.iSequence			= atoi(Cmd_Argv(6));

		// ANIMATION TIME
				Cmd_Init(dat->GetString("PViewModelAnimTime"));
				info.m_AnimIdle.flTime			= atof(Cmd_Argv(0));
				info.m_AnimSelect.flTime		= atof(Cmd_Argv(1));
				info.m_AnimStartReload.flTime	= atof(Cmd_Argv(2));
				info.m_AnimReload.flTime		= atof(Cmd_Argv(3));
				info.m_AnimEndReload.flTime		= atof(Cmd_Argv(4));
				info.m_AnimFire.flTime			= atof(Cmd_Argv(5));
				info.m_AnimRun.flTime			= atof(Cmd_Argv(6));

		// AMMO
				info.iMaxAmmo			= dat->GetInt("MaxAmmo");
				info.iAmmoPerMagazine	= dat->GetInt("AmmoPerMagazine");

		// FIRE
				info.flShotIntervalTime	= dat->GetFloat("ShotIntervalTime");

		// FIREBULLETS
				Cmd_Init(dat->GetString("FireBullets"));
				info.m_FireBullets.iShots			= atoi(Cmd_Argv(0));
				info.m_FireBullets.flDistance		= atof(Cmd_Argv(1));
				info.m_FireBullets.iDamage			= atoi(Cmd_Argv(2));
				info.m_FireBullets.flRangeModifier	= atof(Cmd_Argv(3));

		// ACCURACY
				Cmd_Init(dat->GetString("AccuracyDefault"));
				info.m_AccuracyDefault.flSpreadBase	= atof(Cmd_Argv(0));

				Cmd_Init(dat->GetString("AccuracyJump"));
				info.m_AccuracyJumping.flSpreadBase	= atof(Cmd_Argv(0));

				Cmd_Init(dat->GetString("AccuracyDucking"));
				info.m_AccuracyDucking.flSpreadBase	= atof(Cmd_Argv(0));

				Cmd_Init(dat->GetString("AccuracyMove"));
				info.m_AccuracyRunning.flSpreadBase	= atof(Cmd_Argv(0));

		// PUNCHANGLE
				Cmd_Init(dat->GetString("DropPunchAngle"));
				info.m_DropPunchAngle.flBase		= atof(Cmd_Argv(0));
				info.m_DropPunchAngle.flModifier	= atof(Cmd_Argv(1));

		// KICKBACK
				Cmd_Init(dat->GetString("KickBackDefault"));
				info.m_KickBackDefault.flUpBase	= atof(Cmd_Argv(0));

				Cmd_Init(dat->GetString("KickBackJump"));
				info.m_KickBackJumping.flUpBase	= atof(Cmd_Argv(0));

				Cmd_Init(dat->GetString("KickBackDucking"));
				info.m_KickBackDucking.flUpBase	= atof(Cmd_Argv(0));

				Cmd_Init(dat->GetString("KickBackMove"));
				info.m_KickBackRunning.flUpBase	= atof(Cmd_Argv(0));

			}
			else if (info.iWeaponClass == WeaponClass_Pistol)
			{
		// SOUND
				strcpy(info.szShotSoundName, dat->GetString("ShotSoundName"));

		// ANIMATING
				info.flChangeWeaponAnimRatio	= dat->GetFloat("ChangeWeaponAnimRatio");
				info.flReloadAnimRatio			= dat->GetFloat("ReloadAnimRatio");

		// ANIMATION SEQUENCE
				Cmd_Init(dat->GetString("PViewModelAnimSequence"));
				info.m_AnimIdle.iSequence	= atoi(Cmd_Argv(0));
				info.m_AnimSelect.iSequence	= atoi(Cmd_Argv(1));
				info.m_AnimReload.iSequence	= atoi(Cmd_Argv(2));
				info.m_AnimFire.iSequence	= atoi(Cmd_Argv(3));
				info.m_AnimRun.iSequence	= atoi(Cmd_Argv(4));

		// ANIMATION TIME
				Cmd_Init(dat->GetString("PViewModelAnimTime"));
				info.m_AnimIdle.flTime		= atof(Cmd_Argv(0));
				info.m_AnimSelect.flTime	= atof(Cmd_Argv(1));
				info.m_AnimReload.flTime	= atof(Cmd_Argv(2));
				info.m_AnimFire.flTime		= atof(Cmd_Argv(3));
				info.m_AnimRun.flTime		= atof(Cmd_Argv(4));

		// AMMO
				info.iMaxAmmo			= dat->GetInt("MaxAmmo");
				info.iAmmoPerMagazine	= dat->GetInt("AmmoPerMagazine");

		// FIRE
				info.flShotIntervalTime	= dat->GetFloat("ShotIntervalTime");

		// FIREBULLETS
				Cmd_Init(dat->GetString("FireBullets"));
				info.m_FireBullets.iPenetration			= atoi(Cmd_Argv(0));
				info.m_FireBullets.iPenetrationPower	= atoi(Cmd_Argv(1));
				info.m_FireBullets.iPenetrationDistance	= atoi(Cmd_Argv(2));
				info.m_FireBullets.iDamage				= atoi(Cmd_Argv(3));
				info.m_FireBullets.flRangeModifier		= atof(Cmd_Argv(4));

		// ACCURACY
				Cmd_Init(dat->GetString("AccuracyDefault"));
				info.m_AccuracyDefault.flSpreadBase			= atof(Cmd_Argv(0));
				info.m_AccuracyDefault.flAccuracyBase		= atof(Cmd_Argv(1));
				info.m_AccuracyDefault.flAccuracyModifier	= atof(Cmd_Argv(2));
				info.m_AccuracyDefault.flAccuracyMin		= atof(Cmd_Argv(3));
				info.m_AccuracyDefault.flAccuracyMax		= atof(Cmd_Argv(4));

				Cmd_Init(dat->GetString("AccuracyJump"));
				info.m_AccuracyJumping.flSpreadBase			= atof(Cmd_Argv(0));
				info.m_AccuracyJumping.flAccuracyBase			= atof(Cmd_Argv(1));
				info.m_AccuracyJumping.flAccuracyModifier		= atof(Cmd_Argv(2));
				info.m_AccuracyJumping.flAccuracyMin			= atof(Cmd_Argv(3));
				info.m_AccuracyJumping.flAccuracyMax			= atof(Cmd_Argv(4));

				Cmd_Init(dat->GetString("AccuracyDucking"));
				info.m_AccuracyDucking.flSpreadBase			= atof(Cmd_Argv(0));
				info.m_AccuracyDucking.flAccuracyBase		= atof(Cmd_Argv(1));
				info.m_AccuracyDucking.flAccuracyModifier	= atof(Cmd_Argv(2));
				info.m_AccuracyDucking.flAccuracyMin		= atof(Cmd_Argv(3));
				info.m_AccuracyDucking.flAccuracyMax		= atof(Cmd_Argv(4));

				Cmd_Init(dat->GetString("AccuracyMove"));
				info.m_AccuracyRunning.flSpreadBase			= atof(Cmd_Argv(0));
				info.m_AccuracyRunning.flAccuracyBase			= atof(Cmd_Argv(1));
				info.m_AccuracyRunning.flAccuracyModifier		= atof(Cmd_Argv(2));
				info.m_AccuracyRunning.flAccuracyMin			= atof(Cmd_Argv(3));
				info.m_AccuracyRunning.flAccuracyMax			= atof(Cmd_Argv(4));

		// PUNCHANGLE
				Cmd_Init(dat->GetString("DropPunchAngle"));
				info.m_DropPunchAngle.flBase		= atof(Cmd_Argv(0));
				info.m_DropPunchAngle.flModifier	= atof(Cmd_Argv(1));

		// KICKBACK
				Cmd_Init(dat->GetString("KickBackDefault"));
				info.m_KickBackDefault.flUpBase	= atof(Cmd_Argv(0));

				Cmd_Init(dat->GetString("KickBackJump"));
				info.m_KickBackJumping.flUpBase	= atof(Cmd_Argv(0));

				Cmd_Init(dat->GetString("KickBackDucking"));
				info.m_KickBackDucking.flUpBase	= atof(Cmd_Argv(0));

				Cmd_Init(dat->GetString("KickBackMove"));
				info.m_KickBackRunning.flUpBase	= atof(Cmd_Argv(0));

			}
			else if (info.iWeaponClass == WeaponClass_Knife)
			{
		// ANIMATION SEQUENCE
				Cmd_Init(dat->GetString("PViewModelAnimSequence"));
				info.m_AnimIdle.iSequence		= atoi(Cmd_Argv(0));
				info.m_AnimSelect.iSequence		= atoi(Cmd_Argv(1));
				info.m_AnimCombo1.iSequence		= atoi(Cmd_Argv(2));
				info.m_AnimCombo2.iSequence		= atoi(Cmd_Argv(3));
				info.m_AnimBigshot.iSequence	= atoi(Cmd_Argv(4));
				info.m_AnimRun.iSequence		= atoi(Cmd_Argv(5));

		// ANIMATION TIME
				Cmd_Init(dat->GetString("PViewModelAnimTime"));
				info.m_AnimIdle.flTime		= atof(Cmd_Argv(0));
				info.m_AnimSelect.flTime	= atof(Cmd_Argv(1));
				info.m_AnimCombo1.flTime	= atof(Cmd_Argv(2));
				info.m_AnimCombo2.flTime	= atof(Cmd_Argv(3));
				info.m_AnimBigshot.flTime	= atof(Cmd_Argv(4));
				info.m_AnimRun.flTime		= atof(Cmd_Argv(5));

		// HIT SOUND
				strcpy(info.szKnifeMetalSoundName, dat->GetString("KnifeMetalSoundName"));
				strcpy(info.szKnifeStoneSoundName, dat->GetString("KnifeStoneSoundName"));
				strcpy(info.szKnifeWoodSoundName, dat->GetString("KnifeWoodSoundName"));
				strcpy(info.szKnifeBrokenGlassSoundName, dat->GetString("KnifeBrokenGlassSoundName"));
				strcpy(info.szKnifeCarpetSoundName, dat->GetString("KnifeCarpetSoundName"));
				strcpy(info.szKnifeSnowSoundName, dat->GetString("KnifeSnowSoundName"));

		// KNIFE ATTACK
				Cmd_Init(dat->GetString("Combo1Attack"));
				info.m_KnifeCombo1Attack.flAttackIntervalTime	= atof(Cmd_Argv(0));
				info.m_KnifeCombo1Attack.flDamageDelay			= atof(Cmd_Argv(1));
				info.m_KnifeCombo1Attack.flDamageValue			= atof(Cmd_Argv(2));
				info.m_KnifeCombo1Attack.flAttackRange			= atof(Cmd_Argv(3));
				info.m_KnifeCombo1Attack.flAttackAngle			= atof(Cmd_Argv(4));

				Cmd_Init(dat->GetString("Combo2Attack"));
				info.m_KnifeCombo2Attack.flAttackIntervalTime	= atof(Cmd_Argv(0));
				info.m_KnifeCombo2Attack.flDamageDelay			= atof(Cmd_Argv(1));
				info.m_KnifeCombo2Attack.flDamageValue			= atof(Cmd_Argv(2));
				info.m_KnifeCombo2Attack.flAttackRange			= atof(Cmd_Argv(3));
				info.m_KnifeCombo2Attack.flAttackAngle			= atof(Cmd_Argv(4));

				Cmd_Init(dat->GetString("BigshotAttack"));
				info.m_KnifeBigshotAttack.flAttackIntervalTime	= atof(Cmd_Argv(0));
				info.m_KnifeBigshotAttack.flDamageDelay			= atof(Cmd_Argv(1));
				info.m_KnifeBigshotAttack.flDamageValue			= atof(Cmd_Argv(2));
				info.m_KnifeBigshotAttack.flAttackRange			= atof(Cmd_Argv(3));
				info.m_KnifeBigshotAttack.flAttackAngle			= atof(Cmd_Argv(4));
			}
			else if (info.iWeaponClass == WeaponClass_HEGrenade)
			{
				Cmd_Init(dat->GetString("PViewModelAnimSequence"));
				info.m_AnimIdle.iSequence		= atoi(Cmd_Argv(0));
				info.m_AnimSelect.iSequence		= atoi(Cmd_Argv(1));
				info.m_AnimStartThrow.iSequence	= atoi(Cmd_Argv(2));
				info.m_AnimThrow.iSequence		= atoi(Cmd_Argv(3));
				info.m_AnimRun.iSequence		= atoi(Cmd_Argv(4));

				Cmd_Init(dat->GetString("PViewModelAnimTime"));
				info.m_AnimIdle.flTime			= atof(Cmd_Argv(0));
				info.m_AnimSelect.flTime		= atof(Cmd_Argv(1));
				info.m_AnimStartThrow.flTime	= atof(Cmd_Argv(2));
				info.m_AnimThrow.flTime			= atof(Cmd_Argv(3));
				info.m_AnimRun.flTime			= atof(Cmd_Argv(4));

				info.iAmmoPerMagazine	= dat->GetInt("AmmoPerMagazine");

				Cmd_Init(dat->GetString("ThrowVelAngleGravityAirResTime"));
				info.m_GrenadeThrow.flVelocity	= atof(Cmd_Argv(0));
				info.m_GrenadeThrow.flAngle		= atof(Cmd_Argv(1));
				info.m_GrenadeThrow.flGravity	= atof(Cmd_Argv(2));
				info.m_GrenadeThrow.flFriction	= atof(Cmd_Argv(3));
				info.m_GrenadeThrow.flAirTime	= atof(Cmd_Argv(4));
			}
			else if (info.iWeaponClass == WeaponClass_SmokeGrenade)
			{
				Cmd_Init(dat->GetString("PViewModelAnimSequence"));
				info.m_AnimIdle.iSequence		= atoi(Cmd_Argv(0));
				info.m_AnimSelect.iSequence		= atoi(Cmd_Argv(1));
				info.m_AnimStartThrow.iSequence	= atoi(Cmd_Argv(2));
				info.m_AnimThrow.iSequence		= atoi(Cmd_Argv(3));
				info.m_AnimRun.iSequence		= atoi(Cmd_Argv(4));

				Cmd_Init(dat->GetString("PViewModelAnimTime"));
				info.m_AnimIdle.flTime			= atof(Cmd_Argv(0));
				info.m_AnimSelect.flTime		= atof(Cmd_Argv(1));
				info.m_AnimStartThrow.flTime	= atof(Cmd_Argv(2));
				info.m_AnimThrow.flTime			= atof(Cmd_Argv(3));
				info.m_AnimRun.flTime			= atof(Cmd_Argv(4));

				info.iAmmoPerMagazine	= dat->GetInt("AmmoPerMagazine");

				Cmd_Init(dat->GetString("ThrowVelAngleGravityAirResTime"));
				info.m_GrenadeThrow.flVelocity	= atof(Cmd_Argv(0));
				info.m_GrenadeThrow.flAngle		= atof(Cmd_Argv(1));
				info.m_GrenadeThrow.flGravity	= atof(Cmd_Argv(2));
				info.m_GrenadeThrow.flFriction	= atof(Cmd_Argv(3));
				info.m_GrenadeThrow.flAirTime	= atof(Cmd_Argv(4));
			}
			else if (info.iWeaponClass == WeaponClass_Flashbang)
			{
				Cmd_Init(dat->GetString("PViewModelAnimSequence"));
				info.m_AnimIdle.iSequence		= atoi(Cmd_Argv(0));
				info.m_AnimSelect.iSequence		= atoi(Cmd_Argv(1));
				info.m_AnimStartThrow.iSequence	= atoi(Cmd_Argv(2));
				info.m_AnimThrow.iSequence		= atoi(Cmd_Argv(3));
				info.m_AnimRun.iSequence		= atoi(Cmd_Argv(4));

				Cmd_Init(dat->GetString("PViewModelAnimTime"));
				info.m_AnimIdle.flTime			= atof(Cmd_Argv(0));
				info.m_AnimSelect.flTime		= atof(Cmd_Argv(1));
				info.m_AnimStartThrow.flTime	= atof(Cmd_Argv(2));
				info.m_AnimThrow.flTime			= atof(Cmd_Argv(3));
				info.m_AnimRun.flTime			= atof(Cmd_Argv(4));

				info.iAmmoPerMagazine	= dat->GetInt("AmmoPerMagazine");

				Cmd_Init(dat->GetString("ThrowVelAngleGravityAirResTime"));
				info.m_GrenadeThrow.flVelocity	= atof(Cmd_Argv(0));
				info.m_GrenadeThrow.flAngle		= atof(Cmd_Argv(1));
				info.m_GrenadeThrow.flGravity	= atof(Cmd_Argv(2));
				info.m_GrenadeThrow.flFriction	= atof(Cmd_Argv(3));
				info.m_GrenadeThrow.flAirTime	= atof(Cmd_Argv(4));
			}

	// PLAYER VIEW MODEL
			Cmd_Init(dat->GetString("PVPosAndRotAndFovDefault"));
			info.m_ViewInfo.vecPosition[0]	= atof(Cmd_Argv(0));
			info.m_ViewInfo.vecPosition[1]	= atof(Cmd_Argv(1));
			info.m_ViewInfo.vecPosition[2]	= atof(Cmd_Argv(2));
			info.m_ViewInfo.vecRotate[0]	= atof(Cmd_Argv(3));
			info.m_ViewInfo.vecRotate[1]	= atof(Cmd_Argv(4));
			info.m_ViewInfo.vecRotate[2]	= atof(Cmd_Argv(5));
			info.m_ViewInfo.flFOV			= atof(Cmd_Argv(6));
			info.m_ViewInfo.flAspect		= atof(Cmd_Argv(7));
		}
	}

	pkvd->deleteThis();
}

WeaponData *WeaponData_GetData(int index)
{
	if (!index)
		return NULL;

	for (int i = 0; i < gWeaponCount; i++)
	{
		if (gWeaponData[i].iWeaponIndex == index)
			return &gWeaponData[i];
	}

	return NULL;
}