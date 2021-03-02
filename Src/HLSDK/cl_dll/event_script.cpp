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

#include <event_api.h>
#include <event_args.h>
#include <r_efx.h>
#include <com_model.h>

#include <r_studioint.h>

#include "ClientState.h"
#include "ev_hldm.h"
#include "com_weapons.h"
#include "weapondata.h"
#include "SoundData.h"

#include "ITextureAPI.h"

#include "AnimationScript.h"
#include "sprite.h"

extern "C"
{
#include <pm_defs.h>
#include <pm_materials.h>
#include <pm_hull.h>
#include <pm_shared.h>
}

extern engine_studio_api_t IEngineStudio;

char CL_TextureHit(pmtrace_t &tr, Vector &vecSrc, Vector &vecEnd)
{
	const char *pTextureName = gEngfuncs.pEventAPI->EV_TraceTexture(tr.ent, vecSrc, vecEnd);

	if (pTextureName)
	{
		if (*pTextureName == '-' || *pTextureName == '+')
		{
			pTextureName += 2;
		}

		if (*pTextureName == '{' || *pTextureName == '!' || *pTextureName == '~' || *pTextureName == ' ')
		{
			pTextureName += 1;
		}

		return PM_FindTextureType(pTextureName);
	}

	return CHAR_TEX_DIRT;
}

Vector CL_GetGunPosition(event_args_t *args)
{
	int index = args->entindex;

	Vector view_ofs;

	if (gEngfuncs.pEventAPI->EV_IsLocal(index - 1))
	{
		gEngfuncs.pEventAPI->EV_LocalPlayerViewheight(view_ofs);
	}
	else
	{
		if (!args->ducking)
		{
			VectorCopy(VEC_IDLE_VIEW, view_ofs);
		}
		else
		{
			VectorCopy(VEC_DUCK_VIEW, view_ofs);
		}
	}

	return Vector(args->origin) + view_ofs;
}

//----------------------------------------------------------------
// Purpose : Play a configured sound.
//----------------------------------------------------------------
void CL_PlaySound(int entindex, float *origin, int channel, const char *pszName, const char *pszPart)
{
	char szSndName[32];

	// Wtf ?
	if (!pszName || !*pszName)
		return;

	// Copy base name.
	strcpy(szSndName, pszName);

	// Append part name e.g "InkNanoWood_3".
	if (pszPart)
	{
		strcat(szSndName, "_");
		strcat(szSndName, pszPart);
	}

	// Find the sound in table.
	SoundData *pData = SoundData_GetData(szSndName);

	if (pData)
	{
		// Play sound.
		gEngfuncs.pEventAPI->EV_PlaySound(entindex, origin, channel, pData->szSample, pData->flVolume, pData->flAttn, 0, pData->iPitch);
	}
	else
	{
		gEngfuncs.Con_Printf("CL_PlaySound: Couldn't found sound %s\n", szSndName);
	}
}

void CL_DecalShoot(pmtrace_t *tr, char *name)
{
	int index = gEngfuncs.pEfxAPI->Draw_DecalIndexFromName(name);
	int decal = gEngfuncs.pEfxAPI->Draw_DecalIndex(index);
	int model = gEngfuncs.pEventAPI->EV_IndexFromTrace(tr);

	gEngfuncs.pEfxAPI->R_DecalShoot(decal, model, 0, tr->endpos, 0);
}

void EV_HLDM_PlayTextureSound( int idx, pmtrace_t *ptr, Vector vecSrc, Vector vecEnd, int iBulletType )
{

}

void CL_FireBullets(int index, Vector &vecSrc, Vector &vecDir, float flDistance)
{
	Vector vecEnd;
	pmtrace_t tr;

	vecEnd = vecSrc + vecDir * flDistance;

	gEngfuncs.pEventAPI->EV_SetUpPlayerPrediction(false, true);

	// Store off the old count
	gEngfuncs.pEventAPI->EV_PushPMStates();

	// Now add in all of the players.
	gEngfuncs.pEventAPI->EV_SetSolidPlayers(index - 1);

	gEngfuncs.pEventAPI->EV_SetTraceHull(2);
	gEngfuncs.pEventAPI->EV_PlayerTrace(vecSrc, vecEnd, PM_STUDIO_BOX, -1, &tr);
	gEngfuncs.pEfxAPI->R_ParticleLine(vecSrc, vecEnd, 255, 255, 255, 10);

	// do damage, paint decals
	if (tr.fraction != 1.0f)
	{
		physent_t *pe = gEngfuncs.pEventAPI->EV_GetPhysent(tr.ent);

		if (pe && pe->solid == SOLID_BSP)
		{
			CL_DecalShoot(&tr, "AAA");
		}
	}

	gEngfuncs.pEventAPI->EV_PopPMStates();
}

void CL_FireShotgun(int iShots, Vector &vecSrc, Vector &vecDirShooting, Vector &vecRight, Vector &vecUp, float flSpread, float flDistance, int shared_rand)
{
	pmtrace_t tr;

	for (int iShot = 0; iShot < iShots; iShot++)
	{
		float x, y, z;

		do
		{
			x = UTIL_SharedRandomFloat(shared_rand + 0, -0.5, 0.5) + UTIL_SharedRandomFloat(shared_rand + 1, -0.5, 0.5);
			y = UTIL_SharedRandomFloat(shared_rand + 2, -0.5, 0.5) + UTIL_SharedRandomFloat(shared_rand + 3, -0.5, 0.5);
			z = x * x + y * y;

			shared_rand += 4;
		}
		while (z > 1);

		Vector vecDir = vecDirShooting  + x * flSpread * vecRight + y * flSpread * vecUp;
		Vector vecEnd = vecSrc + vecDir * flDistance;

		gEngfuncs.pEventAPI->EV_SetTraceHull( 2 );
		gEngfuncs.pEventAPI->EV_PlayerTrace(vecSrc, vecEnd, PM_STUDIO_BOX, -1, &tr);

		if (tr.fraction != 1)
		{
			physent_t *pe = gEngfuncs.pEventAPI->EV_GetPhysent(tr.ent);

			if (pe && pe->solid == SOLID_BSP)
			{
				switch (gEngfuncs.pfnRandomLong(0, 3))
				{
					case 0:
						gEngfuncs.pEfxAPI->R_DecalShoot(gEngfuncs.pEfxAPI->Draw_DecalIndex(gEngfuncs.pEfxAPI->Draw_DecalIndexFromName("{shot1")), gEngfuncs.pEventAPI->EV_IndexFromTrace(&tr), 0, tr.endpos, 0);
						break;
					case 1:
						gEngfuncs.pEfxAPI->R_DecalShoot(gEngfuncs.pEfxAPI->Draw_DecalIndex(gEngfuncs.pEfxAPI->Draw_DecalIndexFromName("{shot2")), gEngfuncs.pEventAPI->EV_IndexFromTrace(&tr), 0, tr.endpos, 0);
						break;
					case 2:
						gEngfuncs.pEfxAPI->R_DecalShoot(gEngfuncs.pEfxAPI->Draw_DecalIndex(gEngfuncs.pEfxAPI->Draw_DecalIndexFromName("{shot3")), gEngfuncs.pEventAPI->EV_IndexFromTrace(&tr), 0, tr.endpos, 0);
						break;
					case 3:
						gEngfuncs.pEfxAPI->R_DecalShoot(gEngfuncs.pEfxAPI->Draw_DecalIndex(gEngfuncs.pEfxAPI->Draw_DecalIndexFromName("{shot4")), gEngfuncs.pEventAPI->EV_IndexFromTrace(&tr), 0, tr.endpos, 0);
						break;
				}
			}
		}

		// for
	}
}

//----------------------------------------------------------------
// Purpose : Knife attack action
//----------------------------------------------------------------
void EV_KnifeAttack(struct event_args_s *args)
{
	// iparam1 - WeaponIndex
	// iparam2 - ComboType

	// Find this weapon
	WeaponData *pWeaponData = WeaponData_GetData(args->iparam1);

	if (pWeaponData)
	{
		Vector vForward, vSrc, vEnd;
		float flRange;
		pmtrace_t tr;

		// Get the knife attack range.
		switch (args->iparam2)
		{
		case 0:
			flRange = pWeaponData->m_KnifeCombo1Attack.flAttackRange;
			break;
		case 1:
			flRange = pWeaponData->m_KnifeCombo2Attack.flAttackRange;
			break;
		case 2:
			flRange = pWeaponData->m_KnifeBigshotAttack.flAttackRange;
			break;
		}

		// Get player's forward vector.
		gEngfuncs.pfnAngleVectors(args->angles, vForward, NULL, NULL);

		// Get eye postion.
		vSrc = CL_GetGunPosition(args);

		// Move to forward.
		vEnd = vSrc + vForward * flRange;

		// Set hull 2 that the trace line can hit a studio model.
		gEngfuncs.pEventAPI->EV_SetTraceHull(2);
		// Trace line.
		gEngfuncs.pEventAPI->EV_PlayerTrace(vSrc, vEnd, PM_STUDIO_BOX, -1, &tr);

		// If the line hit some object.
		if (tr.fraction != 1.0f)
		{
			physent_t *pe = gEngfuncs.pEventAPI->EV_GetPhysent(tr.ent);

			// Hit world model.
			if (pe && pe->solid == SOLID_BSP)
			{
				switch (args->iparam2)
				{
				case 0:
					CL_DecalShoot(&tr, "{mark00");
					break;
				case 1:
					CL_DecalShoot(&tr, "{mark01");
					break;
				case 2:
					CL_DecalShoot(&tr, "{mark02");
					break;
				}

				// Is attack BigShot ?
				bool bBigShot = (args->iparam2 == 2);

				// Get texture name of the hit point.
				char cTextureType = CL_TextureHit(tr, vSrc, vEnd);

				// Play knife hit sound.
				switch (cTextureType)
				{
				case CHAR_TEX_BROKENGLASS:
					CL_PlaySound(args->entindex, tr.endpos, CHAN_STATIC, pWeaponData->szKnifeBrokenGlassSoundName, bBigShot ? "2" : "1");
					break;
				case CHAR_TEX_CARPET:
				case CHAR_TEX_CUSHION:
				case CHAR_TEX_DIRT:
					CL_PlaySound(args->entindex, tr.endpos, CHAN_STATIC, pWeaponData->szKnifeCarpetSoundName, bBigShot ? "2" : "1");
					break;
				case CHAR_TEX_METAL:
					CL_PlaySound(args->entindex, tr.endpos, CHAN_STATIC, pWeaponData->szKnifeMetalSoundName, bBigShot ? "2" : "1");
					break;
				case CHAR_TEX_SNOW:
					CL_PlaySound(args->entindex, tr.endpos, CHAN_STATIC, pWeaponData->szKnifeSnowSoundName, bBigShot ? "2" : "1");
					break;
				case CHAR_TEX_STONE:
					CL_PlaySound(args->entindex, tr.endpos, CHAN_STATIC, pWeaponData->szKnifeStoneSoundName, bBigShot ? "2" : "1");
					break;
				case CHAR_TEX_TREE:
					CL_PlaySound(args->entindex, tr.endpos, CHAN_STATIC, pWeaponData->szKnifeWoodSoundName, bBigShot ? "2" : "1");
					break;
				}
			}
		}
	}
	else
	{
		// Weapon is not found, so...
		gEngfuncs.Con_Printf("EV_KnifeAttack: Got a invalid weapon index %d\n", args->iparam1);
	}
}

void EV_RifleFire(struct event_args_s *args)
{
	// fparam1 - Bullet direction (x)
	// fparam2 - Bullet direction (y)
	// iparam1 - Weapon index

	// Find this weapon
	WeaponData *pWeaponData = WeaponData_GetData(args->iparam1);

	if (pWeaponData)
	{
		Vector vForward, vSrc, vEnd;
		float flRange;
		pmtrace_t tr;

		// Get player's forward vector.
		gEngfuncs.pfnAngleVectors(args->angles, vForward, NULL, NULL);

		// Get eye postion.
		vSrc = CL_GetGunPosition(args);

		// Move to forward.
		vEnd = vSrc + vForward * 4096.0;

		// Set hull 2 that the trace line can hit a studio model.
		gEngfuncs.pEventAPI->EV_SetTraceHull(2);
		// Trace line.
		gEngfuncs.pEventAPI->EV_PlayerTrace(vSrc, vEnd, PM_STUDIO_BOX, -1, &tr);

		// If the line hit some object.
		if (tr.fraction != 1.0f)
		{
			//gEngfuncs.pEfxAPI->R_StreakSplash( tr.endpos, -vForward, 209, 30, 1, 10, 100 );
		}
	}
}

void EV_PistolFire(struct event_args_s *args)
{
}

void EV_SniperFire(struct event_args_s *args)
{
}

void EV_ShotgunFire(struct event_args_s *args)
{
}

void EV_Grenade(struct event_args_s *args)
{
	Vector vecOrigin = args->origin;

	static int iScript = -1;

	static int iLayoutSmok = -1;
	static float flLayoutSmokTime = 0;
	static int iLayoutFire = -1;
	static float flLayoutFireTime = 0;
	static int iLayoutExp0 = -1;
	static float flLayoutExp0Time = 0;
	static int iLayoutExp1 = -1;
	static float flLayoutExp1Time = 0;
	static int iLayoutWH = -1;
	static float flLayoutWHTime = 0;

	if (iScript == -1)
	{
		AnimScript_LoadFile("Script/Grenade.txt");

		iScript = AnimScript_FindScript("Grenade");

		if (iScript == -1)
		{
			Sys_Error("EV_Grenade: Script \"Grenade\" not found");
			return;
		}

		iLayoutSmok = AnimScript_FindLayout(iScript, "Smoke");
		iLayoutFire = AnimScript_FindLayout(iScript, "Fire");
		iLayoutExp0 = AnimScript_FindLayout(iScript, "Exp0");
		iLayoutExp1 = AnimScript_FindLayout(iScript, "Exp1");
		iLayoutWH	= AnimScript_FindLayout(iScript, "WH");

		if (iLayoutSmok == -1 || iLayoutFire == -1 || iLayoutExp0 == -1 || iLayoutExp1 == -1)
		{
			Sys_Error("EV_Grenade: Layout not found");
			return;
		}

		flLayoutSmokTime = AnimScript_GetAnimTime(iScript, iLayoutSmok) / 1000.0;
		flLayoutFireTime = AnimScript_GetAnimTime(iScript, iLayoutFire) / 1000.0;
		flLayoutExp0Time = AnimScript_GetAnimTime(iScript, iLayoutExp0) / 1000.0;
		flLayoutExp1Time = AnimScript_GetAnimTime(iScript, iLayoutExp1) / 1000.0;
		flLayoutWHTime	 = AnimScript_GetAnimTime(iScript, iLayoutWH) / 1000.0;
	}

	static tex_t *pSmok = NULL;
	static tex_t *pFire = NULL;
	static tex_t *pExp0 = NULL;
	static tex_t *pExp1 = NULL;
	static tex_t *pWH	= NULL;

	if (pSmok == NULL)
	{
		pSmok = gTexAPI.LoadDTX("FX/GRANADE/SMOKE3.DTX");
		pFire = gTexAPI.LoadDTX("FX/EXP1/EXP05.DTX");
		pExp0 = gTexAPI.LoadTGA("FX/FX_DTX/GS0033.TGA");
		pExp1 = gTexAPI.LoadTGA("FX/FX_DTX/GS0035.TGA");
		pWH	  = gTexAPI.LoadTGA("gfx/6060.tga");
	}

	SPR_Create(vecOrigin, pSmok, RenderNormal, iScript, iLayoutSmok, 1.0, flLayoutSmokTime);
	SPR_Create(vecOrigin, pFire, RenderNormal, iScript, iLayoutFire, 1.0, flLayoutFireTime);
	SPR_Create(vecOrigin, pExp0, RenderAdditive, iScript, iLayoutExp0, 1.0, flLayoutExp0Time);
	SPR_Create(vecOrigin, pExp1, RenderAdditive, iScript, iLayoutExp1, 1.0, flLayoutExp1Time);
	SPR_Create(vecOrigin, pWH, RenderNormal, iScript, iLayoutWH, 1.0, flLayoutWHTime);

	gEngfuncs.pEventAPI->EV_PlaySound(args->entindex, args->origin, CHAN_STATIC, "weapon/hegrenade/t_boom_he.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM);
}