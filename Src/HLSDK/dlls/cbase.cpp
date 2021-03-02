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
#include "client.h"
#include "game.h"


// From util.cpp
void EntvarsKeyvalue(entvars_t *pev, KeyValueData *pkvd);

extern "C"
{
#include <pm_shared.h>
}

// From bmodel.cpp
Vector VecBModelOrigin(entvars_t *pevBModel);

// From globals.cpp
extern Vector g_vecAttackDir;


/*static */DLL_FUNCTIONS gFunctionTable = 
{
	GameDLLInit,
	DispatchSpawn,
	DispatchThink,
	DispatchUse,
	DispatchTouch,
	DispatchBlocked,
	DispatchKeyValue,
	DispatchSave,
	DispatchRestore,
	DispatchSetAbsBox,
	SaveWriteFields,
	SaveReadFields,
	SaveGlobalState,
	RestoreGlobalState,
	ResetGlobalState,
	ClientConnect,
	ClientDisconnect,
	ClientKill,
	ClientPutInServer,
	ClientCommand,
	ClientUserInfoChanged,
	ServerActivate,
	ServerDeactivate,
	PlayerPreThink,
	PlayerPostThink,
	StartFrame,
	ParmsNewLevel,
	ParmsChangeLevel,
	GetGameDescription,
	PlayerCustomization,
	SpectatorConnect,
	SpectatorDisconnect,
	SpectatorThink,
	Sys_Error,
	PM_Move,
	PM_Init,
	PM_FindTextureType,
	SetupVisibility,
	UpdateClientData,
	AddToFullPack,
	CreateBaseline,
	RegisterEncoders,
	GetWeaponData,
	CmdStart,
	CmdEnd,
	ConnectionlessPacket,
	GetHullBounds,
	CreateInstancedBaselines,
	InconsistentFile,
	AllowLagCompensation,
};

static NEW_DLL_FUNCTIONS gNewDLLFunctionTable = 
{
	OnFreeEntPrivateData,
	GameShutdown,
	ShouldCollide,
};

extern "C"
{

/*BOOL DLLExport GetEntityAPI(DLL_FUNCTIONS *pFunctionTable, int interfaceVersion)
{
	if (!pFunctionTable || interfaceVersion != INTERFACE_VERSION)
		return FALSE;

	memcpy(pFunctionTable, &gFunctionTable, sizeof(DLL_FUNCTIONS));

	return TRUE;
}*/

BOOL DLLExport GetEntityAPI2(DLL_FUNCTIONS *pFunctionTable, int *interfaceVersion)
{
	if (!pFunctionTable || *interfaceVersion != INTERFACE_VERSION)
	{
		*interfaceVersion = INTERFACE_VERSION;
		return FALSE;
	}

	memcpy(pFunctionTable, &gFunctionTable, sizeof(DLL_FUNCTIONS));

	return TRUE;
}

DLLExport BOOL GetNewDLLFunctions(NEW_DLL_FUNCTIONS *pFunctionTable, int *interfaceVersion)
{
	if (!pFunctionTable || *interfaceVersion != NEW_DLL_FUNCTIONS_VERSION)
	{
		*interfaceVersion = NEW_DLL_FUNCTIONS_VERSION;
		return FALSE;
	}

	memcpy(pFunctionTable, &gNewDLLFunctionTable, sizeof(NEW_DLL_FUNCTIONS));

	return TRUE;
}

}

int DispatchSpawn(edict_t *pent)
{
	CBaseEntity *pEntity = CBaseEntity::Instance(pent);

	if (pEntity)
	{
		pEntity->pev->absmin = pEntity->pev->origin - Vector(1.0, 1.0, 1.0);
		pEntity->pev->absmax = pEntity->pev->origin + Vector(1.0, 1.0, 1.0);

		pEntity->Spawn();

		// Try to get the pointer again, in case the spawn function deleted the entity.
		// UNDONE: Spawn() should really return a code to ask that the entity be deleted, but
		// that would touch too much code for me to do that right now.
		pEntity = CBaseEntity::Instance(pent);

		if (pEntity)
		{
			if (pEntity->pev->flags & FL_KILLME)
			{
				return -1;
			}
		}
	}

	return 0;
}

void DispatchThink(edict_t *pent)
{
	CBaseEntity *pEntity = CBaseEntity::Instance(pent);

	if (pEntity)
	{
		pEntity->Think();
	}
}

void DispatchUse(edict_t *pentUsed, edict_t *pentOther)
{
	CBaseEntity *pEntity = CBaseEntity::Instance(pentUsed);
	CBaseEntity *pOther = CBaseEntity::Instance(pentOther);

	if (pEntity && pOther && !((pEntity->pev->flags | pOther->pev->flags) & FL_KILLME))
	{
		pEntity->Use(pOther, pOther, USE_TOGGLE, 0);
	}
}

void DispatchTouch(edict_t *pentTouched, edict_t *pentOther)
{
	CBaseEntity *pEntity = CBaseEntity::Instance(pentTouched);
	CBaseEntity *pOther = CBaseEntity::Instance(pentOther);

	if (pEntity && pOther && !((pEntity->pev->flags | pOther->pev->flags) & FL_KILLME))
	{
		pEntity->Touch(pOther);
	}
}

void DispatchBlocked(edict_t *pentBlocked, edict_t *pentOther)
{
	CBaseEntity *pEntity = CBaseEntity::Instance(pentBlocked);
	CBaseEntity *pOther = CBaseEntity::Instance(pentOther);

	if (pEntity && pOther && !((pEntity->pev->flags | pOther->pev->flags) & FL_KILLME))
	{
		pEntity->Blocked(pOther);
	}
}

void DispatchKeyValue(edict_t *pentKeyvalue, KeyValueData *pkvd)
{
	EntvarsKeyvalue(VARS(pentKeyvalue), pkvd);

	if (pkvd->fHandled || !pkvd->szClassName)
		return;

	CBaseEntity *pEntity = CBaseEntity::Instance(pentKeyvalue);

	if (!pEntity)
		return;

	pEntity->KeyValue(pkvd);
}

void DispatchSave(edict_t *pent, SAVERESTOREDATA *pSaveData)
{
}

int DispatchRestore(edict_t *pent, SAVERESTOREDATA *pSaveData, int globalEntity)
{
	return 0;
}

void DispatchSetAbsBox(edict_t *pent)
{
	CBaseEntity *pEntity = CBaseEntity::Instance(pent);

	if (pEntity)
	{
		pEntity->SetAbsBox();
	}
}

void OnFreeEntPrivateData(edict_t *pEnt)
{
	CBaseEntity *pEntity = (CBaseEntity *)pEnt->pvPrivateData;

	pEntity->~CBaseEntity();
}

int ShouldCollide(edict_t *pentTouched, edict_t *pentOther)
{
	CBaseEntity *pEntity = CBaseEntity::Instance(pentOther);
	CBaseEntity *pOther = CBaseEntity::Instance(pentTouched);

	if (pEntity && pOther && !((pEntity->pev->flags | pOther->pev->flags) & FL_KILLME))
	{
		return pEntity->Collide(pOther);
	}

	return true;
}

void SaveWriteFields(SAVERESTOREDATA *pSaveData, const char *pname, void *pBaseData, TYPEDESCRIPTION *pFields, int fieldCount)
{
}

void SaveReadFields(SAVERESTOREDATA *pSaveData, const char *pname, void *pBaseData, TYPEDESCRIPTION *pFields, int fieldCount)
{
}

void CBaseEntity::Spawn(void)
{
}

void CBaseEntity::Think(void)
{
	if (m_pfnThink)
	{
		(this->*m_pfnThink)();
	}
}

void CBaseEntity::Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
	if (m_pfnUse)
	{
		(this->*m_pfnUse)(pActivator, pCaller, useType, value);
	}
}

void CBaseEntity::Touch(CBaseEntity *pOther)
{
	if (m_pfnTouch)
	{
		(this->*m_pfnTouch)(pOther);
	}
}

void CBaseEntity::Blocked(CBaseEntity *pOther)
{
	if (m_pfnBlocked)
	{
		(this->*m_pfnBlocked)(pOther);
	}
}

void CBaseEntity::KeyValue(KeyValueData *pkvd)
{
	pkvd->fHandled = FALSE;
}

void CBaseEntity::SetAbsBox(void)
{
	if ((pev->solid == SOLID_BSP) && (pev->angles.x || pev->angles.y || pev->angles.z))
	{
		float max = 0.0, v;

		for (int i = 0; i < 3; ++i)
		{
			v = fabs(((float *)pev->mins)[i]);
			if (v > max)
				max = v;
			v = fabs(((float *)pev->maxs)[i]);
			if (v > max)
				max = v;
		}

		for (int i = 0; i < 3; ++i)
		{
			((float *)pev->absmin)[i] = ((float *)pev->origin)[i] - max;
			((float *)pev->absmax)[i] = ((float *)pev->origin)[i] + max;
		}
	}
	else
	{
		pev->absmin = pev->origin + pev->mins;
		pev->absmax = pev->origin + pev->maxs;
	}

	pev->absmin.x -= 1.0;
	pev->absmin.y -= 1.0;
	pev->absmin.z -= 1.0;
	pev->absmax.x += 1.0;
	pev->absmax.y += 1.0;
	pev->absmax.z += 1.0;
}

bool CBaseEntity::Collide(CBaseEntity *pOther)
{
	return true;
}

void CBaseEntity::Precache(void)
{
}

int CBaseEntity::ObjectCaps(void)
{
	return 0;
}

void CBaseEntity::TakeHealth(float flHealth)
{
	if (!pev->takedamage)
		return;

	if (pev->health >= pev->max_health)
		return;

	pev->health += flHealth;

	if (pev->health > pev->max_health)
		pev->health = pev->max_health;

	return;
}

void CBaseEntity::TakeDamage(entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType)
{
	if (!pev->takedamage)
		return;

	g_vecAttackDir = (pevInflictor->origin - VecBModelOrigin(pev)).Normalize();

	if (!FNullEnt(pevInflictor) && (pev->movetype == MOVETYPE_WALK || pev->movetype == MOVETYPE_STEP) && pevAttacker->solid != SOLID_TRIGGER)
	{
		Vector vecDir = (pev->origin - (pevInflictor->absmin + pevInflictor->absmax) * 0.5).Normalize();

		float flForce = flDamage * ((32.0 * 32.0 * 72.0) / (pev->size.x * pev->size.y * pev->size.z)) * 5.0;

		if (flForce > 1000.0)
			flForce = 1000.0;

		pev->velocity = pev->velocity + vecDir * flForce;
	}

	pev->health -= flDamage;

	if (pev->health <= 0.0)
	{
		Killed(pevInflictor, pevAttacker);
		return;
	}
}

void CBaseEntity::Killed(entvars_t *pevInflictor, entvars_t *pevAttacker)
{
	pev->takedamage = DAMAGE_NO;
	pev->deadflag = DEAD_DEAD;

	UTIL_Remove(this);
}

void CBaseEntity::TraceAttack(entvars_t *pevAttacker, float flDamage, Vector &vecDir, TraceResult *ptr, int bitsDamageType)
{
}

float CBaseEntity::GetDelay(void)
{
	return 0;
}

BOOL CBaseEntity::IsTriggered(CBaseEntity *pActivator)
{
	return TRUE;
}

BOOL CBaseEntity::HasTarget(string_t targetname)
{
	return !strcmp(STRING(targetname), STRING(pev->targetname));
}

CBaseEntity *CBaseEntity::GetNextTarget(void)
{
	if (!pev->target)
		return NULL;

	edict_t *pTarget = FIND_ENTITY_BY_TARGETNAME(NULL, STRING(pev->target));

	if (FNullEnt(pTarget))
		return NULL;

	return Instance(pTarget);
}

BOOL CBaseEntity::ShouldToggle(USE_TYPE useType, BOOL currentState)
{
	if (useType != USE_TOGGLE && useType != USE_SET)
	{
		if ((currentState && useType == USE_ON) || (!currentState && useType == USE_OFF))
			return FALSE;
	}

	return TRUE;
}

bool CBaseEntity::IsAlive(void) const
{
	return (pev->deadflag == DEAD_NO) && (pev->health > 0);
}

bool CBaseEntity::IsBSPModel(void) const
{
	return (pev->solid == SOLID_BSP) || (pev->movetype == MOVETYPE_PUSHSTEP);
}

bool CBaseEntity::IsInWorld(void) const
{
	if (pev->origin.x >= 4096.0f) return FALSE;
	if (pev->origin.y >= 4096.0f) return FALSE;
	if (pev->origin.z >= 4096.0f) return FALSE;
	if (pev->origin.x <= -4096.0f) return FALSE;
	if (pev->origin.y <= -4096.0f) return FALSE;
	if (pev->origin.z <= -4096.0f) return FALSE;

	if (pev->velocity.x >= 2000.0f) return FALSE;
	if (pev->velocity.y >= 2000.0f) return FALSE;
	if (pev->velocity.z >= 2000.0f) return FALSE;
	if (pev->velocity.x <= -2000.0f) return FALSE;
	if (pev->velocity.y <= -2000.0f) return FALSE;
	if (pev->velocity.z <= -2000.0f) return FALSE;

	return TRUE;
}

bool CBaseEntity::IsPlayer(void) const
{
	return false;
}

bool CBaseEntity::IsNetClient(void) const
{
	return false;
}

Vector CBaseEntity::Center(void) const
{
	if (pev->solid == SOLID_BSP)
		return (pev->absmax + pev->absmin) / 2;

	return pev->origin;
}

void CBaseEntity::WorldSpaceAABB(Vector *pWorldMins, Vector *pWorldMaxs) const
{
	*pWorldMins = pev->absmin;
	*pWorldMaxs = pev->absmax;
}

edict_t *CBaseEntity::edict(void) const
{
	return ENT(pev);
}

int CBaseEntity::entindex(void) const
{
	return ENTINDEX(ENT(pev));
}

CBaseEntity *CBaseEntity::Create(const char *szName, Vector &vecOrigin, Vector &vecAngles, edict_t *pentOwner)
{
	edict_t *pent;
	CBaseEntity *pEntity;

	pent = CREATE_NAMED_ENTITY(MAKE_STRING(szName));

	if (FNullEnt(pent))
	{
		ALERT(at_console, "NULL Ent in Create!\n");
		return NULL;
	}

	pEntity = Instance(pent);
	pEntity->pev->owner = pentOwner;
	pEntity->pev->origin = vecOrigin;
	pEntity->pev->angles = vecAngles;
	DispatchSpawn(pEntity->edict());

	return pEntity;
}

CBaseEntity *CBaseEntity::Instance(const edict_t *pent)
{
	if (pent)
		return (CBaseEntity *)pent->pvPrivateData;

	return NULL;
}

CBaseEntity *CBaseEntity::Instance(const entvars_t *pev)
{
	return (CBaseEntity *)pev->pContainingEntity->pvPrivateData;
}