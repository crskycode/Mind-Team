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
#include "shake.h"
#include "message.h"

extern "C"
{
#include <pm_shared.h>
#include <pm_materials.h>
}

static unsigned int glSeed = 0;

unsigned int seed_table[256] =
{
	28985, 27138, 26457, 9451, 17764, 10909, 28790, 8716, 6361, 4853, 17798, 21977, 19643, 20662, 10834, 20103,
	27067, 28634, 18623, 25849, 8576, 26234, 23887, 18228, 32587, 4836, 3306, 1811, 3035, 24559, 18399, 315,
	26766, 907, 24102, 12370, 9674, 2972, 10472, 16492, 22683, 11529, 27968, 30406, 13213, 2319, 23620, 16823,
	10013, 23772, 21567, 1251, 19579, 20313, 18241, 30130, 8402, 20807, 27354, 7169, 21211, 17293, 5410, 19223,
	10255, 22480, 27388, 9946, 15628, 24389, 17308, 2370, 9530, 31683, 25927, 23567, 11694, 26397, 32602, 15031,
	18255, 17582, 1422, 28835, 23607, 12597, 20602, 10138, 5212, 1252, 10074, 23166, 19823, 31667, 5902, 24630,
	18948, 14330, 14950, 8939, 23540, 21311, 22428, 22391, 3583, 29004, 30498, 18714, 4278, 2437, 22430, 3439,
	28313, 23161, 25396, 13471, 19324, 15287, 2563, 18901, 13103, 16867, 9714, 14322, 15197, 26889, 19372, 26241,
	31925, 14640, 11497, 8941, 10056, 6451, 28656, 10737, 13874, 17356, 8281, 25937, 1661, 4850, 7448, 12744,
	21826, 5477, 10167, 16705, 26897, 8839, 30947, 27978, 27283, 24685, 32298, 3525, 12398, 28726, 9475, 10208,
	617, 13467, 22287, 2376, 6097, 26312, 2974, 9114, 21787, 28010, 4725, 15387, 3274, 10762, 31695, 17320,
	18324, 12441, 16801, 27376, 22464, 7500, 5666, 18144, 15314, 31914, 31627, 6495, 5226, 31203, 2331, 4668,
	12650, 18275, 351, 7268, 31319, 30119, 7600, 2905, 13826, 11343, 13053, 15583, 30055, 31093, 5067, 761,
	9685, 11070, 21369, 27155, 3663, 26542, 20169, 12161, 15411, 30401, 7580, 31784, 8985, 29367, 20989, 14203,
	29694, 21167, 10337, 1706, 28578, 887, 3373, 19477, 14382, 675, 7033, 15111, 26138, 12252, 30996, 21409,
	25678, 18555, 13256, 23316, 22407, 16727, 991, 9236, 5373, 29402, 6117, 15241, 27715, 19291, 19888, 19847
};

unsigned int U_Random(void)
{
	glSeed *= 69069;
	glSeed += seed_table[glSeed & 0xFF];
	return (++glSeed & 0x0FFFFFFF);
}

void U_Srand(unsigned int seed)
{
	glSeed = seed_table[seed & 0xFF];
}

int UTIL_SharedRandomLong(unsigned int seed, int low, int high)
{
	U_Srand((int)seed + low + high);

	unsigned int range = high - low + 1;

	if (!(range - 1))
		return low;

	int rnum = U_Random();
	int offset = rnum % range;
	return (low + offset);
}

float UTIL_SharedRandomFloat(unsigned int seed, float low, float high)
{
	U_Srand((int)seed + *(int *)&low + *(int *)&high);
	U_Random();
	U_Random();

	unsigned range = (int)(high - low);

	if (!range)
		return low;

	int tensixrand = U_Random() & 65535;
	float offset = (float)tensixrand / 65536;
	return (low + offset * range);
}


int g_groupmask = 0;
int g_groupop = 0;

void UTIL_SetGroupTrace(int groupmask, int op)
{
	g_groupmask = groupmask;
	g_groupop = op;

	ENGINE_SETGROUPMASK(g_groupmask, g_groupop);
}

void UTIL_UnsetGroupTrace(void)
{
	g_groupmask = 0;
	g_groupop = 0;

	ENGINE_SETGROUPMASK(0, 0);
}

TYPEDESCRIPTION gEntvarsDescription[] = 
{
	DEFINE_ENTITY_FIELD(classname, FIELD_STRING),
	DEFINE_ENTITY_GLOBAL_FIELD(globalname, FIELD_STRING),
	
	DEFINE_ENTITY_FIELD(origin, FIELD_POSITION_VECTOR),
	DEFINE_ENTITY_FIELD(oldorigin, FIELD_POSITION_VECTOR),
	DEFINE_ENTITY_FIELD(velocity, FIELD_VECTOR),
	DEFINE_ENTITY_FIELD(basevelocity, FIELD_VECTOR),
	DEFINE_ENTITY_FIELD(movedir, FIELD_VECTOR),

	DEFINE_ENTITY_FIELD(angles, FIELD_VECTOR),
	DEFINE_ENTITY_FIELD(avelocity, FIELD_VECTOR),
	DEFINE_ENTITY_FIELD(punchangle, FIELD_VECTOR),
	DEFINE_ENTITY_FIELD(v_angle, FIELD_VECTOR),
	DEFINE_ENTITY_FIELD(fixangle, FIELD_FLOAT),
	DEFINE_ENTITY_FIELD(idealpitch, FIELD_FLOAT),
	DEFINE_ENTITY_FIELD(pitch_speed, FIELD_FLOAT),
	DEFINE_ENTITY_FIELD(ideal_yaw, FIELD_FLOAT),
	DEFINE_ENTITY_FIELD(yaw_speed, FIELD_FLOAT),

	DEFINE_ENTITY_FIELD(modelindex, FIELD_INTEGER),
	DEFINE_ENTITY_GLOBAL_FIELD(model, FIELD_MODELNAME),

	DEFINE_ENTITY_FIELD(viewmodel, FIELD_MODELNAME),
	DEFINE_ENTITY_FIELD(weaponmodel, FIELD_MODELNAME),

	DEFINE_ENTITY_FIELD(absmin, FIELD_POSITION_VECTOR),
	DEFINE_ENTITY_FIELD(absmax, FIELD_POSITION_VECTOR),
	DEFINE_ENTITY_GLOBAL_FIELD(mins, FIELD_VECTOR),
	DEFINE_ENTITY_GLOBAL_FIELD(maxs, FIELD_VECTOR),
	DEFINE_ENTITY_GLOBAL_FIELD(size, FIELD_VECTOR),

	DEFINE_ENTITY_FIELD(ltime, FIELD_TIME),
	DEFINE_ENTITY_FIELD(nextthink, FIELD_TIME),

	DEFINE_ENTITY_FIELD(solid, FIELD_INTEGER),
	DEFINE_ENTITY_FIELD(movetype, FIELD_INTEGER),

	DEFINE_ENTITY_FIELD(skin, FIELD_INTEGER),
	DEFINE_ENTITY_FIELD(body, FIELD_INTEGER),
	DEFINE_ENTITY_FIELD(effects, FIELD_INTEGER),

	DEFINE_ENTITY_FIELD(gravity, FIELD_FLOAT),
	DEFINE_ENTITY_FIELD(friction, FIELD_FLOAT),
	DEFINE_ENTITY_FIELD(light_level, FIELD_FLOAT),

	DEFINE_ENTITY_FIELD(frame, FIELD_FLOAT),
	DEFINE_ENTITY_FIELD(scale, FIELD_FLOAT),
	DEFINE_ENTITY_FIELD(sequence, FIELD_INTEGER),
	DEFINE_ENTITY_FIELD(animtime, FIELD_TIME),
	DEFINE_ENTITY_FIELD(framerate, FIELD_FLOAT),
	DEFINE_ENTITY_FIELD(controller, FIELD_INTEGER),
	DEFINE_ENTITY_FIELD(blending, FIELD_INTEGER),

	DEFINE_ENTITY_FIELD(rendermode, FIELD_INTEGER),
	DEFINE_ENTITY_FIELD(renderamt, FIELD_FLOAT),
	DEFINE_ENTITY_FIELD(rendercolor, FIELD_VECTOR),
	DEFINE_ENTITY_FIELD(renderfx, FIELD_INTEGER),

	DEFINE_ENTITY_FIELD(health, FIELD_FLOAT),
	DEFINE_ENTITY_FIELD(frags, FIELD_FLOAT),
	DEFINE_ENTITY_FIELD(weapons, FIELD_INTEGER),
	DEFINE_ENTITY_FIELD(takedamage, FIELD_FLOAT),

	DEFINE_ENTITY_FIELD(deadflag, FIELD_FLOAT),
	DEFINE_ENTITY_FIELD(view_ofs, FIELD_VECTOR),
	DEFINE_ENTITY_FIELD(button, FIELD_INTEGER),
	DEFINE_ENTITY_FIELD(impulse, FIELD_INTEGER),

	DEFINE_ENTITY_FIELD(chain, FIELD_EDICT),
	DEFINE_ENTITY_FIELD(dmg_inflictor, FIELD_EDICT),
	DEFINE_ENTITY_FIELD(enemy, FIELD_EDICT),
	DEFINE_ENTITY_FIELD(aiment, FIELD_EDICT),
	DEFINE_ENTITY_FIELD(owner, FIELD_EDICT),
	DEFINE_ENTITY_FIELD(groundentity, FIELD_EDICT),

	DEFINE_ENTITY_FIELD(spawnflags, FIELD_INTEGER),
	DEFINE_ENTITY_FIELD(flags, FIELD_FLOAT),

	DEFINE_ENTITY_FIELD(colormap, FIELD_INTEGER),
	DEFINE_ENTITY_FIELD(team, FIELD_INTEGER),

	DEFINE_ENTITY_FIELD(max_health, FIELD_FLOAT),
	DEFINE_ENTITY_FIELD(teleport_time, FIELD_TIME),
	DEFINE_ENTITY_FIELD(armortype, FIELD_FLOAT),
	DEFINE_ENTITY_FIELD(armorvalue, FIELD_FLOAT),
	DEFINE_ENTITY_FIELD(waterlevel, FIELD_INTEGER),
	DEFINE_ENTITY_FIELD(watertype, FIELD_INTEGER),

	// Having these fields be local to the individual levels makes it easier to test those levels individually.
	DEFINE_ENTITY_GLOBAL_FIELD(target, FIELD_STRING),
	DEFINE_ENTITY_GLOBAL_FIELD(targetname, FIELD_STRING),
	DEFINE_ENTITY_FIELD(netname, FIELD_STRING),
	DEFINE_ENTITY_FIELD(message, FIELD_STRING),

	DEFINE_ENTITY_FIELD(dmg_take, FIELD_FLOAT),
	DEFINE_ENTITY_FIELD(dmg_save, FIELD_FLOAT),
	DEFINE_ENTITY_FIELD(dmg, FIELD_FLOAT),
	DEFINE_ENTITY_FIELD(dmgtime, FIELD_TIME),

	DEFINE_ENTITY_FIELD(noise, FIELD_SOUNDNAME),
	DEFINE_ENTITY_FIELD(noise1, FIELD_SOUNDNAME),
	DEFINE_ENTITY_FIELD(noise2, FIELD_SOUNDNAME),
	DEFINE_ENTITY_FIELD(noise3, FIELD_SOUNDNAME),
	DEFINE_ENTITY_FIELD(speed, FIELD_FLOAT),
	DEFINE_ENTITY_FIELD(air_finished, FIELD_TIME),
	DEFINE_ENTITY_FIELD(pain_finished, FIELD_TIME),
	DEFINE_ENTITY_FIELD(radsuit_finished, FIELD_TIME),
};

#define ENTVARS_COUNT (sizeof(gEntvarsDescription)/sizeof(gEntvarsDescription[0]))

static int gSizes[FIELD_TYPECOUNT] = 
{
	sizeof(float),		// FIELD_FLOAT
	sizeof(int),		// FIELD_STRING
	sizeof(int),		// FIELD_ENTITY
	sizeof(int),		// FIELD_CLASSPTR
	sizeof(int),		// FIELD_EHANDLE
	sizeof(int),		// FIELD_entvars_t
	sizeof(int),		// FIELD_EDICT
	sizeof(float)*3,	// FIELD_VECTOR
	sizeof(float)*3,	// FIELD_POSITION_VECTOR
	sizeof(int *),		// FIELD_POINTER
	sizeof(int),		// FIELD_INTEGER
	sizeof(int *),		// FIELD_FUNCTION
	sizeof(int),		// FIELD_BOOLEAN
	sizeof(short),		// FIELD_SHORT
	sizeof(char),		// FIELD_CHARACTER
	sizeof(float),		// FIELD_TIME
	sizeof(int),		// FIELD_MODELNAME
	sizeof(int),		// FIELD_SOUNDNAME
};

void EntvarsKeyvalue(entvars_t *pev, KeyValueData *pkvd)
{
	TYPEDESCRIPTION *pField;

	for (int i = 0; i < ENTVARS_COUNT; i++)
	{
		pField = &gEntvarsDescription[i];

		if (!stricmp(pField->fieldName, pkvd->szKeyName))
		{
			switch(pField->fieldType)
			{
			case FIELD_MODELNAME:
			case FIELD_SOUNDNAME:
			case FIELD_STRING:
				(*(int *)((byte *)pev + pField->fieldOffset)) = ALLOC_STRING(pkvd->szValue);
				break;
			case FIELD_TIME:
			case FIELD_FLOAT:
				(*(float *)((byte *)pev + pField->fieldOffset)) = atof(pkvd->szValue);
				break;
			case FIELD_INTEGER:
				(*(int *)((byte *)pev + pField->fieldOffset)) = atoi(pkvd->szValue);
				break;
			case FIELD_POSITION_VECTOR:
			case FIELD_VECTOR:
				UTIL_StringToVector((float *)((byte *)pev + pField->fieldOffset), pkvd->szValue);
				break;
			default:
			case FIELD_EVARS:
			case FIELD_CLASSPTR:
			case FIELD_EDICT:
			case FIELD_ENTITY:
			case FIELD_POINTER:
				ALERT(at_error, "Bad field in entity!!\n");
				break;
			}

			pkvd->fHandled = TRUE;
			return;
		}
	}
}

BOOL FClassnameIs(CBaseEntity* pent, const char* szClassname)
{
	return FStrEq(STRING(pent->pev->classname), szClassname);
}

Vector UTIL_VecToAngles(Vector &vec)
{
	float out[3];
	VEC_TO_ANGLES(vec, out);
	return Vector(out);
}

int UTIL_EntitiesInBox(CBaseEntity **pList, int listMax, Vector &mins, Vector &maxs, int flagMask)
{
	edict_t *pEdict = g_engfuncs.pfnPEntityOfEntIndex(1);
	CBaseEntity *pEntity;

	int count = 0;

	if (!pEdict)
		return count;

	for (int i = 1; i < gpGlobals->maxEntities; ++i, ++pEdict)
	{
		if (pEdict->free)
			continue;

		if (flagMask && !(pEdict->v.flags & flagMask))
			continue;

		if (mins.x > pEdict->v.absmax.x || 
			mins.y > pEdict->v.absmax.y || 
			mins.z > pEdict->v.absmax.z || 
			maxs.x < pEdict->v.absmin.x || 
			maxs.y < pEdict->v.absmin.y || 
			maxs.z < pEdict->v.absmin.z)
		{
			continue;
		}

		pEntity = CBaseEntity::Instance(pEdict);

		if (!pEntity)
			continue;

		pList[count] = pEntity;

		if (count >= listMax)
			return count;
	}

	return count;
}

CBaseEntity *UTIL_FindEntityInSphere(CBaseEntity *pStartEntity, Vector &vecCenter, float flRadius)
{
	edict_t *pentEntity;

	if (pStartEntity)
		pentEntity = pStartEntity->edict();
	else
		pentEntity = NULL;

	pentEntity = FIND_ENTITY_IN_SPHERE(pentEntity, vecCenter, flRadius);

	if (!FNullEnt(pentEntity))
		return CBaseEntity::Instance(pentEntity);

	return NULL;
}

CBaseEntity *UTIL_FindEntityByString(CBaseEntity *pStartEntity, const char *szKeyword, const char *szValue)
{
	edict_t *pentEntity;

	if (pStartEntity)
		pentEntity = pStartEntity->edict();
	else
		pentEntity = NULL;

	pentEntity = FIND_ENTITY_BY_STRING(pentEntity, szKeyword, szValue);

	if (!FNullEnt(pentEntity))
		return CBaseEntity::Instance(pentEntity);

	return NULL;
}

CBaseEntity *UTIL_FindEntityByClassname(CBaseEntity *pStartEntity, const char *szName)
{
	return UTIL_FindEntityByString(pStartEntity, "classname", szName);
}

CBaseEntity *UTIL_FindEntityByTargetname(CBaseEntity *pStartEntity, const char *szName)
{
	return UTIL_FindEntityByString(pStartEntity, "targetname", szName);
}

int UTIL_EntityCountByClassname(const char *szName)
{
	int i = 0;
	CBaseEntity *pEntity = NULL;

	while ((pEntity = UTIL_FindEntityByClassname(pEntity, szName)) != NULL)
	{
		i++;
	}

	return i;
}

CBasePlayer *UTIL_PlayerByIndex(int playerIndex)
{
	if (playerIndex > 0 && playerIndex <= gpGlobals->maxClients)
	{
		edict_t *pEntity = INDEXENT(playerIndex);

		if (pEntity && !pEntity->free)
		{
			CBasePlayer *pPlayer = (CBasePlayer *)CBaseEntity::Instance(pEntity);

			if (pPlayer && !pPlayer->m_bDisconnected)
				return pPlayer;
		}
	}

	return NULL;
}

void UTIL_MakeVectors(Vector &vecAngles)
{
	MAKE_VECTORS(vecAngles);
}

void UTIL_MakeAimVectors(Vector &vecAngles)
{
	Vector in = vecAngles;
	in.x = -in.x;
	MAKE_VECTORS(in);
}

void UTIL_EmitAmbientSound(edict_t *entity, Vector &vecOrigin, const char *samp, float vol, float attenuationm, int fFlags, int pitch)
{
	EMIT_AMBIENT_SOUND(entity, vecOrigin, samp, vol, attenuationm, fFlags, pitch);
}

unsigned short FixedUnsigned16(float value, float scale)
{
	int out;

	out = value * scale;

	if (out < 0)
		out = 0;

	if (out > 0xFFFF)
		out = 0xFFFF;

	return (unsigned short)out;
}

void UTIL_ScreenShake(Vector &center, float amplitude, float frequency, float duration, float radius)
{
	float localAmplitude;
	ScreenShake shake;

	shake.duration = FixedUnsigned16(duration, 1 << 12);
	shake.frequency = FixedUnsigned16(frequency, 1 << 8);

	for (int i = 1; i <= gpGlobals->maxClients; ++i)
	{
		CBaseEntity *pPlayer = UTIL_PlayerByIndex(i);

		if (!pPlayer || !(pPlayer->pev->flags & FL_ONGROUND))
			continue;

		localAmplitude = 0;

		if (radius <= 0)
			localAmplitude = amplitude;
		else
		{
			Vector delta = center - pPlayer->pev->origin;
			float distance = delta.Length();

			if (distance < radius)
				localAmplitude = amplitude;
		}

		if (localAmplitude)
		{
			shake.amplitude = FixedUnsigned16(localAmplitude, 1 << 12);

			MESSAGE_BEGIN(MSG_ONE, gmsgShake, NULL, pPlayer->edict());
			WRITE_SHORT(shake.amplitude);
			WRITE_SHORT(shake.duration);
			WRITE_SHORT(shake.frequency);
			MESSAGE_END();
		}
	}
}

void UTIL_ScreenShakeAll(Vector &center, float amplitude, float frequency, float duration)
{
	UTIL_ScreenShake(center, amplitude, frequency, duration, 0);
}

void UTIL_ScreenFadeBuild(ScreenFade &fade, Vector &color, float fadeTime, float fadeHold, int alpha, int flags)
{
	fade.duration = FixedUnsigned16(fadeTime, 1 << 12);
	fade.holdTime = FixedUnsigned16(fadeHold, 1 << 12);
	fade.r = color.x;
	fade.g = color.y;
	fade.b = color.z;
	fade.a = alpha;
	fade.fadeFlags = flags;
}

void UTIL_ScreenFadeWrite(ScreenFade &fade, CBaseEntity *pEntity)
{
	if (!pEntity || !pEntity->IsNetClient())
		return;

	MESSAGE_BEGIN(MSG_ONE, gmsgFade, NULL, pEntity->edict());
	WRITE_SHORT(fade.duration);
	WRITE_SHORT(fade.holdTime);
	WRITE_SHORT(fade.fadeFlags);
	WRITE_BYTE(fade.r);
	WRITE_BYTE(fade.g);
	WRITE_BYTE(fade.b);
	WRITE_BYTE(fade.a);
	MESSAGE_END();
}

void UTIL_ScreenFadeAll(Vector &color, float fadeTime, float fadeHold, int alpha, int flags)
{
	ScreenFade fade;

	UTIL_ScreenFadeBuild(fade, color, fadeTime, fadeHold, alpha, flags);

	for (int i = 1; i <= gpGlobals->maxClients; ++i)
	{
		CBaseEntity *pPlayer = UTIL_PlayerByIndex(i);

		if (pPlayer)
		{
			UTIL_ScreenFadeWrite(fade, pPlayer);
		}
	}
}

void UTIL_ScreenFade(CBaseEntity *pEntity, Vector &color, float fadeTime, float fadeHold, int alpha, int flags)
{
	ScreenFade fade;

	UTIL_ScreenFadeBuild(fade, color, fadeTime, fadeHold, alpha, flags);
	UTIL_ScreenFadeWrite(fade, pEntity);
}

void ClientPrintAll(int dest, const char *format, ...)
{
	static char message[1024];
	
	va_list ap;
	va_start(ap, format);
	vsnprintf(message, sizeof(message) - 1, format, ap);
	va_end(ap);

	MESSAGE_BEGIN(MSG_BROADCAST, gmsgTextMsg);
	WRITE_BYTE(dest);
	WRITE_STRING(message);
	MESSAGE_END();
}

void ClientPrint(edict_t *client, int dest, const char *format, ...)
{
	static char message[1024];
	
	va_list ap;
	va_start(ap, format);
	vsnprintf(message, sizeof(message) - 1, format, ap);
	va_end(ap);

	MESSAGE_BEGIN(MSG_ONE, gmsgTextMsg, NULL, client);
	WRITE_BYTE(dest);
	WRITE_STRING(message);
	MESSAGE_END();
}

void ClientPrint(CBaseEntity *client, int dest, const char *format, ...)
{
	if (!client)
		return;

	static char message[1024];
	
	va_list ap;
	va_start(ap, format);
	vsnprintf(message, sizeof(message) - 1, format, ap);
	va_end(ap);

	MESSAGE_BEGIN(MSG_ONE, gmsgTextMsg, NULL, client->edict());
	WRITE_BYTE(dest);
	WRITE_STRING(message);
	MESSAGE_END();
}

void UTIL_TraceLine(const Vector &vecStart, const Vector &vecEnd, IGNORE_MONSTERS igmon, IGNORE_GLASS ignoreGlass, edict_t *pentIgnore, TraceResult *ptr)
{
	TRACE_LINE(vecStart, vecEnd, (igmon == ignore_monsters ? TRUE : FALSE) | (ignoreGlass ? 0x100 : 0), pentIgnore, ptr);
}

void UTIL_TraceLine(const Vector &vecStart, const Vector &vecEnd, IGNORE_MONSTERS igmon, edict_t *pentIgnore, TraceResult *ptr)
{
	TRACE_LINE(vecStart, vecEnd, (igmon == ignore_monsters ? TRUE : FALSE), pentIgnore, ptr);
}

void UTIL_TraceHull(const Vector &vecStart, const Vector &vecEnd, IGNORE_MONSTERS igmon, int hullNumber, edict_t *pentIgnore, TraceResult *ptr)
{
	TRACE_HULL(vecStart, vecEnd, (igmon == ignore_monsters ? TRUE : FALSE), hullNumber, pentIgnore, ptr);
}

void UTIL_TraceModel(const Vector &vecStart, const Vector &vecEnd, int hullNumber, edict_t *pentModel, TraceResult *ptr)
{
	g_engfuncs.pfnTraceModel(vecStart, vecEnd, hullNumber, pentModel, ptr);
}

void UTIL_SetSize(entvars_t *pev, Vector &mins, Vector &maxs)
{
	SET_SIZE(ENT(pev), mins, maxs);
}

void UTIL_SetOrigin(entvars_t *pev, Vector &vecOrigin)
{
	SET_ORIGIN(ENT(pev), vecOrigin);
}

float UTIL_VecToYaw(const Vector &vec)
{
	return VEC_TO_YAW(vec);
}

float UTIL_Approach(float target, float value, float speed)
{
	float delta = target - value;

	if (delta > speed)
		value += speed;
	else if (delta < -speed)
		value -= speed;
	else
		value = target;

	return value;
}

int UTIL_IsMasterTriggered(string_t sMaster, CBaseEntity *pActivator)
{
	if (sMaster)
	{
		edict_t *pentTarget = FIND_ENTITY_BY_TARGETNAME(NULL, STRING(sMaster));

		if (!FNullEnt(pentTarget))
		{
			CBaseEntity *pMaster = CBaseEntity::Instance(pentTarget);

			if (pMaster && (pMaster->ObjectCaps() & FCAP_MASTER))
				return pMaster->IsTriggered(pActivator);
		}

		ALERT(at_console, "Master was null or not a master!\n");
	}

	return 1;
}

int UTIL_PointContents(Vector &vecOrigin)
{
	return POINT_CONTENTS(vecOrigin);
}

void UTIL_DecalShoot(Vector &vecOrigin, int decalNumber)
{
	MESSAGE_BEGIN(MSG_BROADCAST, SVC_TEMPENTITY);
	WRITE_BYTE(TE_WORLDDECAL);
	WRITE_COORD(vecOrigin.x);
	WRITE_COORD(vecOrigin.y);
	WRITE_COORD(vecOrigin.z);
	WRITE_BYTE(decalNumber);
	MESSAGE_END();
}

void UTIL_Sparks(Vector &vecOrigin)
{
	MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, vecOrigin);
	WRITE_BYTE(TE_SPARKS);
	WRITE_COORD(vecOrigin.x);
	WRITE_COORD(vecOrigin.y);
	WRITE_COORD(vecOrigin.z);
	MESSAGE_END();
}

void UTIL_Ricochet(Vector &vecOrigin, float flScale)
{
	MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, vecOrigin);
	WRITE_BYTE(TE_ARMOR_RICOCHET);
	WRITE_COORD(vecOrigin.x);
	WRITE_COORD(vecOrigin.y);
	WRITE_COORD(vecOrigin.z);
	WRITE_BYTE((int)(flScale * 10));
	MESSAGE_END();
}

void UTIL_StringToVector(float *out, const char *in)
{
	char *pstr, *pfront, tmp[128];
	int i;

	strcpy(tmp, in);
	pstr = pfront = tmp;

	for (i = 0; i < 3; ++i)
	{
		out[i] = atof(pfront);

		while (*pstr && *pstr != ' ')
			pstr++;

		if (!*pstr)
			break;

		pstr++;
		pfront = pstr;
	}

	if (i < 2)
	{
		for (i = i + 1; i < 3; ++i)
		{
			out[i] = 0;
		}
	}
}

Vector UTIL_ClampVectorToBox(Vector &input, Vector &clampSize)
{
	Vector sourceVector = input;

	if (sourceVector.x > clampSize.x)
		sourceVector.x -= clampSize.x;
	else if (sourceVector.x < -clampSize.x)
		sourceVector.x += clampSize.x;
	else
		sourceVector.x = 0;

	if (sourceVector.y > clampSize.y)
		sourceVector.y -= clampSize.y;
	else if (sourceVector.y < -clampSize.y)
		sourceVector.y += clampSize.y;
	else
		sourceVector.y = 0;
	
	if (sourceVector.z > clampSize.z)
		sourceVector.z -= clampSize.z;
	else if (sourceVector.z < -clampSize.z)
		sourceVector.z += clampSize.z;
	else
		sourceVector.z = 0;

	return sourceVector.Normalize();
}

void UTIL_Remove(CBaseEntity *pEntity)
{
	if (!pEntity)
		return;

	pEntity->pev->flags |= FL_KILLME;
	pEntity->pev->targetname = 0;
}

BOOL UTIL_IsValidEntity(edict_t *pent)
{
	if (!pent || pent->free || (pent->v.flags & FL_KILLME))
		return FALSE;

	return TRUE;
}

void UTIL_PrecacheOther(const char *szClassname)
{
	edict_t	*pent;

	pent = CREATE_NAMED_ENTITY(MAKE_STRING(szClassname));

	if (FNullEnt(pent))
	{
		ALERT (at_console, "NULL Ent in UTIL_PrecacheOther\n");
		return;
	}
	
	CBaseEntity *pEntity = CBaseEntity::Instance(VARS(pent));

	if (pEntity)
		pEntity->Precache();

	REMOVE_ENTITY(pent);
}

float UTIL_WeaponTimeBase(void)
{
	return 0;
}

void UTIL_StripToken(const char *pKey, char *pDest)
{
	int i = 0;

	while (pKey[i] && pKey[i] != '#')
	{
		pDest[i] = pKey[i];
		i++;
	}

	pDest[i] = 0;
}

char *UTIL_VarArgs(char *format, ...)
{
	va_list argptr;
	static char string[1024];

	va_start(argptr, format);
	vsprintf(string, format, argptr);
	va_end(argptr);

	return string;
}

char UTIL_TextureHit(TraceResult *ptr, Vector vecSrc, Vector vecEnd)
{
	char chTextureType;
	float rgfl1[3], rgfl2[3];
	const char *pTextureName;
	char szbuffer[64];
	CBaseEntity *pEntity = CBaseEntity::Instance(ptr->pHit);

	vecSrc.CopyToArray(rgfl1);
	vecEnd.CopyToArray(rgfl2);

	if (pEntity)
		pTextureName = TRACE_TEXTURE(ENT(pEntity->pev), rgfl1, rgfl2);
	else
		pTextureName = TRACE_TEXTURE(ENT(0), rgfl1, rgfl2);

	if (pTextureName)
	{
		if (*pTextureName == '-' || *pTextureName == '+')
			pTextureName += 2;

		if (*pTextureName == '{' || *pTextureName == '!' || *pTextureName == '~' || *pTextureName == ' ')
			pTextureName++;

		strcpy(szbuffer, pTextureName);
		szbuffer[CBTEXTURENAMEMAX - 1] = 0;
		chTextureType = PM_FindTextureType(szbuffer);
	}
	else
		chTextureType = 0;

	return chTextureType;
}

extern void (*pfnDebugLine)(float *start, float *end, int r, int g, int b, float life);

void UTIL_DrawBeamPoints(Vector vecSrc, Vector vecDest, int iLifetime, byte bRed, byte bGreen, byte bBlue)
{
	static int s_iBeamSprite = 0;

/*	if (!s_iBeamSprite)
		s_iBeamSprite = PRECACHE_MODEL("sprites/smoke.spr");

	MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, vecSrc);
	WRITE_BYTE(TE_BEAMPOINTS);
	WRITE_COORD(vecSrc.x);
	WRITE_COORD(vecSrc.y);
	WRITE_COORD(vecSrc.z);
	WRITE_COORD(vecDest.x);
	WRITE_COORD(vecDest.y);
	WRITE_COORD(vecDest.z);
	WRITE_SHORT(s_iBeamSprite);
	WRITE_BYTE(0);
	WRITE_BYTE(0);
	WRITE_BYTE(iLifetime);
	WRITE_BYTE(10);
	WRITE_BYTE(0);
	WRITE_BYTE(bRed);
	WRITE_BYTE(bGreen);
	WRITE_BYTE(bBlue);
	WRITE_BYTE(255);
	WRITE_BYTE(0);
	MESSAGE_END();*/

	if (pfnDebugLine)
	{
		pfnDebugLine( vecSrc, vecDest, bRed, bGreen, bBlue, (float)iLifetime / 10);
	}
}

void UTIL_RadiusFlash(Vector &vecSrc, entvars_t *pevInflictor, float flMinRadius, float flMaxRadius, float flFlashTime)
{
	CBaseEntity *pEntity = NULL;

	while ((pEntity = UTIL_FindEntityInSphere(pEntity, vecSrc, flMaxRadius)) != NULL)
	{
		if (!pEntity->IsPlayer())
			continue;

		CBasePlayer *pVictim = dynamic_cast<CBasePlayer*>(pEntity);

		//if (pevInflictor->waterlevel != 3 && pVictim->pev->waterlevel == 3)
		//	continue;

		//if (pevInflictor->waterlevel == 3 && pVictim->pev->waterlevel == 0)
		//	continue;

		Vector vecLookerOrigin = pVictim->GetGunPosition();
		Vector vecTargetOrigin = vecSrc;

		TraceResult tr;
		UTIL_TraceLine(vecLookerOrigin, vecTargetOrigin, ignore_monsters, ignore_glass, pVictim->edict(), &tr);

		if (tr.flFraction != 1.0 && VARS(tr.pHit) != pevInflictor)
			continue;

		float flDistance = (vecTargetOrigin - vecLookerOrigin).Length();

		if (flDistance > flMaxRadius)
			continue;

		float flFadeOutTime = flFlashTime - 1.0;

		if (flDistance > flMinRadius)
		{
			float percen = (flMaxRadius - flDistance) / (flMaxRadius - flMinRadius);

			flFlashTime = flFlashTime * percen;
			flFadeOutTime = flFadeOutTime * percen;

			if (flFlashTime < 0.2)
				continue;
		}

		MESSAGE_BEGIN(MSG_ONE, gmsgFlashbang, NULL, pVictim->edict());
		WRITE_BYTE(flFlashTime * 10);
		WRITE_BYTE(flFadeOutTime * 10);
		MESSAGE_END();
	}
}

void Con_Printf(const char *msg, ...)
{
	static char string[1024];
	va_list arg;

	va_start(arg, msg);
	_vsnprintf(string, sizeof(string), msg, arg);
	va_end(arg);

	g_engfuncs.pfnServerPrint(string);
}