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

#ifndef UTIL_H
#define UTIL_H

#include "enginecallback.h"

extern globalvars_t *gpGlobals;


// Use this instead of ALLOC_STRING on constant strings
#define STRING(offset) (const char *)(gpGlobals->pStringBase + (int)offset)
#define MAKE_STRING(str) ((int)str - (int)STRING(0))


inline edict_t *FIND_ENTITY_BY_CLASSNAME(edict_t *entStart, const char *pszName)
{
	return FIND_ENTITY_BY_STRING(entStart, "classname", pszName);
}
inline edict_t *FIND_ENTITY_BY_TARGETNAME(edict_t *entStart, const char *pszName)
{
	return FIND_ENTITY_BY_STRING(entStart, "targetname", pszName);
}
inline edict_t *FIND_ENTITY_BY_TARGET(edict_t *entStart, const char *pszName)
{
	return FIND_ENTITY_BY_STRING(entStart, "target", pszName);
}


// Keeps clutter down a bit, when using a float as a bit-vector
#define SetBits(flBitVector, bits) ((flBitVector) = (int)(flBitVector) | (bits))
#define ClearBits(flBitVector, bits) ((flBitVector) = (int)(flBitVector) & ~(bits))
#define FBitSet(flBitVector, bit) ((int)(flBitVector) & (bit))

// More explicit than "int"
typedef int EOFFSET;


// This is the glue that hooks .MAP entity class names to our CPP classes
// The _declspec forces them to be exported by name so we can do a lookup with GetProcAddress()
// The function is used to intialize / allocate the object for the entity
#ifdef _WIN32

#define LINK_ENTITY_TO_CLASS(entityName, className) \
	\
	extern "C" void DLLExport entityName(entvars_t *pev); \
	\
	void entityName(entvars_t *pev) \
	{ \
		GetClassPtr((className *)pev); \
	}

#endif

inline edict_t *ENT(const entvars_t *pev)
{
	return pev->pContainingEntity;
}
inline edict_t *ENT(EOFFSET eoffset)
{
	return g_engfuncs.pfnPEntityOfEntOffset(eoffset);
}

inline EOFFSET OFFSET(const edict_t *pent)
{
	return g_engfuncs.pfnEntOffsetOfPEntity(pent);
}
inline EOFFSET OFFSET(entvars_t *pev)
{ 
	return OFFSET(ENT(pev));
}

inline entvars_t *VARS(edict_t *pent)
{
	if (!pent)
		return NULL;

	return &pent->v;
}
inline entvars_t* VARS(EOFFSET eoffset)
{
	return VARS(ENT(eoffset));
}

inline int ENTINDEX(edict_t *pEdict)
{
	return g_engfuncs.pfnIndexOfEdict(pEdict);
}
inline int ENTINDEX(entvars_t *pev)
{
	return g_engfuncs.pfnIndexOfEdict(ENT(pev));
}

inline edict_t* INDEXENT(int iEdictNum)
{
	return g_engfuncs.pfnPEntityOfEntIndex(iEdictNum);
}

#define eoNullEntity 0
inline BOOL FNullEnt(EOFFSET eoffset)
{
	return eoffset == 0;
}
inline BOOL FNullEnt(const edict_t* pent)
{
	return pent == NULL || FNullEnt(OFFSET(pent));
}
inline BOOL FNullEnt(entvars_t* pev)
{
	return pev == NULL || FNullEnt(OFFSET(pev));
}

#define iStringNull 0
inline BOOL FStringNull(int iString)
{
	return iString == iStringNull;
}


enum TOGGLE_STATE
{
	TS_AT_TOP,
	TS_AT_BOTTOM,
	TS_GOING_UP,
	TS_GOING_DOWN
};

inline BOOL FStrEq(const char*sz1, const char*sz2)
{
	return !strcmp(sz1, sz2);
}

inline BOOL FClassnameIs(edict_t* pent, const char* szClassname)
{
	return FStrEq(STRING(pent->v.classname), szClassname);
}
inline BOOL FClassnameIs(entvars_t* pev, const char* szClassname)
{
	return FStrEq(STRING(pev->classname), szClassname);
}

int UTIL_SharedRandomLong(unsigned int seed, int low, int high);
float UTIL_SharedRandomFloat(unsigned int seed, float low, float high);

void UTIL_SetGroupTrace(int groupmask, int op);
void UTIL_UnsetGroupTrace(void);

class CBaseEntity;
class CBasePlayer;

BOOL FClassnameIs(CBaseEntity* pent, const char* szClassname);

Vector UTIL_VecToAngles(Vector &vec);

int UTIL_EntitiesInBox(CBaseEntity **pList, int listMax, Vector &mins, Vector &maxs, int flagMask);

CBaseEntity *UTIL_FindEntityInSphere(CBaseEntity *pStartEntity, Vector &vecCenter, float flRadius);
CBaseEntity *UTIL_FindEntityByString(CBaseEntity *pStartEntity, const char *szKeyword, const char *szValue);
CBaseEntity *UTIL_FindEntityByClassname(CBaseEntity *pStartEntity, const char *szName);
CBaseEntity *UTIL_FindEntityByTargetname(CBaseEntity *pStartEntity, const char *szName);

int UTIL_EntityCountByClassname(const char *szName);
CBasePlayer *UTIL_PlayerByIndex(int playerIndex);

void UTIL_MakeVectors(Vector &vecAngles);
void UTIL_MakeAimVectors(Vector &vecAngles);

void UTIL_EmitAmbientSound(edict_t *entity, Vector &vecOrigin, const char *samp, float vol, float attenuation, int fFlags, int pitch);

void UTIL_ScreenShake(Vector &center, float amplitude, float frequency, float duration, float radius);
void UTIL_ScreenShakeAll(Vector &center, float amplitude, float frequency, float duration);
void UTIL_ScreenFadeAll(Vector &color, float fadeTime, float holdTime, int alpha, int flags);
void UTIL_ScreenFade(CBaseEntity *pEntity, Vector &color, float fadeTime, float fadeHold, int alpha, int flags);

inline void UTIL_MakeVectorsPrivate(Vector vecAngles, float *p_vForward, float *p_vRight, float *p_vUp)
{
	(*g_engfuncs.pfnAngleVectors)(vecAngles, p_vForward, p_vRight, p_vUp);
}

#define HUD_PRINTNOTIFY		1
#define HUD_PRINTCONSOLE	2
#define HUD_PRINTTALK		3
#define HUD_PRINTCENTER		4

void ClientPrintAll(int dest, const char *format, ...);
void ClientPrint(edict_t *client, int dest, const char *format, ...);
void ClientPrint(CBaseEntity *client, int dest, const char *format, ...);


enum IGNORE_MONSTERS
{
	ignore_monsters = 1,
	dont_ignore_monsters = 0,
	missile = 2,
};
enum IGNORE_GLASS
{
	ignore_glass = 1,
	dont_ignore_glass = 0,
};
void UTIL_TraceLine(const Vector &vecStart, const Vector &vecEnd, IGNORE_MONSTERS igmon, edict_t *pentIgnore, TraceResult *ptr);
void UTIL_TraceLine(const Vector &vecStart, const Vector &vecEnd, IGNORE_MONSTERS igmon, IGNORE_GLASS ignoreGlass, edict_t *pentIgnore, TraceResult *ptr);

enum
{
	point_hull = 0,
	human_hull = 1,
	large_hull = 2,
	head_hull = 3,
};
void UTIL_TraceHull(const Vector &vecStart, const Vector &vecEnd, IGNORE_MONSTERS igmon, int hullNumber, edict_t *pentIgnore, TraceResult *ptr);

void UTIL_TraceModel(const Vector &vecStart, const Vector &vecEnd, int hullNumber, edict_t *pentModel, TraceResult *ptr);

void UTIL_SetSize(entvars_t* pev, Vector &vecMin, Vector &vecMax);
void UTIL_SetOrigin(entvars_t* pev, Vector &vecOrigin);

float UTIL_VecToYaw(const Vector &vec);

float UTIL_Approach(float target, float value, float speed);

int UTIL_IsMasterTriggered(string_t sMaster, CBaseEntity *pActivator);
int UTIL_PointContents(Vector &vecOrigin);
void UTIL_DecalShoot(Vector &vecOrigin, int decalNumber);
void UTIL_Sparks(Vector &vecOrigin);
void UTIL_Ricochet(Vector &vecOrigin, float flScale);
void UTIL_StringToVector(float *pVector, const char *pString);
Vector UTIL_ClampVectorToBox(Vector &input, Vector &clampSize);

void UTIL_Remove(CBaseEntity *pEntity);
BOOL UTIL_IsValidEntity(edict_t *pent);

void UTIL_PrecacheOther(const char *szClassname);

float UTIL_WeaponTimeBase(void);

void UTIL_StripToken(const char *pKey, char *pDest);

char *UTIL_VarArgs(char *format, ...);

void UTIL_DrawBeamPoints(Vector vecSrc, Vector vecDest, int iLifetime, byte bRed, byte bGreen, byte bBlue);

void UTIL_RadiusFlash(Vector &vecSrc, entvars_t *pevInflictor, float flMinRadius, float flMaxRadius, float flFlashTime);

CBasePlayer *UTIL_GetLocalPlayer(void);

// cmdparse.cpp
void Cmd_Init(const char *text);
int Cmd_Argc(void);
const char *Cmd_Argv(int i);

char UTIL_TextureHit(TraceResult *ptr, Vector vecSrc, Vector vecEnd);

// subs.cpp
void SetMovedir(entvars_t* pev);
// bmodel.cpp
Vector VecBModelOrigin( entvars_t* pevBModel );
// globals.cpp
extern Vector g_vecZero;

void Con_Printf(const char *msg, ...);

#define AMBIENT_SOUND_STATIC			0	// medium radius attenuation
#define AMBIENT_SOUND_EVERYWHERE		1
#define AMBIENT_SOUND_SMALLRADIUS		2
#define AMBIENT_SOUND_MEDIUMRADIUS		4
#define AMBIENT_SOUND_LARGERADIUS		8
#define AMBIENT_SOUND_START_SILENT		16
#define AMBIENT_SOUND_NOT_LOOPING		32

#define SPEAKER_START_SILENT			1	// wait for trigger 'on' to start announcements

#define SND_SPAWNING		(1<<8)		// duplicated in protocol.h we're spawing, used in some cases for ambients 
#define SND_STOP			(1<<5)		// duplicated in protocol.h stop sound
#define SND_CHANGE_VOL		(1<<6)		// duplicated in protocol.h change sound vol
#define SND_CHANGE_PITCH	(1<<7)		// duplicated in protocol.h change sound pitch

#define	LFO_SQUARE			1
#define LFO_TRIANGLE		2
#define LFO_RANDOM			3

// func_rotating
#define SF_BRUSH_ROTATE_Y_AXIS		0
#define SF_BRUSH_ROTATE_INSTANT		1
#define SF_BRUSH_ROTATE_BACKWARDS	2
#define SF_BRUSH_ROTATE_Z_AXIS		4
#define SF_BRUSH_ROTATE_X_AXIS		8
#define SF_PENDULUM_AUTO_RETURN		16
#define	SF_PENDULUM_PASSABLE		32

#define SF_BRUSH_ROTATE_SMALLRADIUS	128
#define SF_BRUSH_ROTATE_MEDIUMRADIUS 256
#define SF_BRUSH_ROTATE_LARGERADIUS 512

#define PUSH_BLOCK_ONLY_X	1
#define PUSH_BLOCK_ONLY_Y	2

#define SVC_TEMPENTITY		23
#define SVC_INTERMISSION	30
#define SVC_CDTRACK			32
#define SVC_WEAPONANIM		35
#define SVC_ROOMTYPE		37
#define	SVC_DIRECTOR		51


#define SF_LIGHT_START_OFF		1

#define SPAWNFLAG_NOMESSAGE	1
#define SPAWNFLAG_NOTOUCH	1
#define SPAWNFLAG_DROIDONLY	4

#define SPAWNFLAG_USEONLY	1		// can't be touched, must be used (buttons)

#define SF_TRIG_PUSH_ONCE		1

// NOTE: use EMIT_SOUND_DYN to set the pitch of a sound. Pitch of 100
// is no pitch shift.  Pitch > 100 up to 255 is a higher pitch, pitch < 100
// down to 1 is a lower pitch.   150 to 70 is the realistic range.
// EMIT_SOUND_DYN with pitch != 100 should be used sparingly, as it's not quite as
// fast as EMIT_SOUND (the pitchshift mixer is not native coded).

inline void EMIT_SOUND(edict_t *entity, int channel, const char *sample, float volume, float attenuation)
{
	EMIT_SOUND_DYN(entity, channel, sample, volume, attenuation, 0, PITCH_NORM);
}

inline void STOP_SOUND(edict_t *entity, int channel, const char *sample)
{
	EMIT_SOUND_DYN(entity, channel, sample, 0, 0, SND_STOP, PITCH_NORM);
}

#define GROUP_OP_AND	0
#define GROUP_OP_NAND	1

extern int g_groupmask;
extern int g_groupop;

#endif