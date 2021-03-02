#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "doors.h"

void CPointEntity::Spawn(void)
{
	pev->solid = SOLID_NOT;
}

LINK_ENTITY_TO_CLASS(info_player_deathmatch, CPointEntity);
LINK_ENTITY_TO_CLASS(info_player_start, CPointEntity);

void CBaseEntity::SUB_Remove(void)
{
	if (pev->health > 0)
	{
		ALERT(at_aiconsole, "SUB_Remove called on entity with health > 0\n");
	}

	REMOVE_ENTITY(ENT(pev));
}

void CBaseEntity::SUB_DoNothing(void)
{
}

void CBaseDelay::KeyValue(KeyValueData *pkvd)
{
	if (!strcmp(pkvd->szKeyName, "delay"))
	{
		m_flDelay = atof(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (!strcmp(pkvd->szKeyName, "killtarget"))
	{
		m_iszKillTarget = ALLOC_STRING(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else
		CBaseEntity::KeyValue(pkvd);
}

void CBaseEntity::SUB_UseTargets(CBaseEntity *pActivator, USE_TYPE useType, float value)
{
	if (pev->target)
		FireTargets(STRING(pev->target), pActivator, this, useType, value);
}

void CBaseEntity::SUB_CallUseToggle(void)
{
	this->Use(this, this, USE_TOGGLE, 0);
}

void FireTargets(const char *targetName, CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
	edict_t *pentTarget = NULL;

	if (!targetName)
		return;

	ALERT(at_aiconsole, "Firing: (%s)\n", targetName);

	while (1)
	{
		pentTarget = FIND_ENTITY_BY_TARGETNAME(pentTarget, targetName);

		if (FNullEnt(pentTarget))
		{
			break;
		}

		CBaseEntity *pTarget = CBaseEntity::Instance(pentTarget);

		if (pTarget && !FBitSet(pTarget->pev->flags, FL_KILLME))
		{
			ALERT(at_aiconsole, "Found: %s, firing (%s)\n", STRING(pTarget->pev->classname), targetName);
			pTarget->Use(pActivator, pCaller, useType, value);
		}
	}
}

void CBaseDelay::SUB_UseTargets(CBaseEntity *pActivator, USE_TYPE useType, float value)
{
	if (!pev->target && !m_iszKillTarget)
		return;

	if (m_flDelay != 0)
	{
		CBaseDelay *pTemp = GetClassPtr((CBaseDelay *)NULL);

		pTemp->pev->classname = MAKE_STRING("DelayedUse");

		SetThink(&CBaseDelay::DelayThink);
		pTemp->pev->nextthink = gpGlobals->time + m_flDelay;

		pTemp->pev->button = (int)useType;
		pTemp->m_iszKillTarget = m_iszKillTarget;
		pTemp->m_flDelay = 0;
		pTemp->pev->target = pev->target;

		if (pActivator && pActivator->IsPlayer())
			pTemp->pev->owner = pActivator->edict();
		else
			pTemp->pev->owner = NULL;

		return;
	}

	if (m_iszKillTarget)
	{
		edict_t *pentKillTarget = NULL;

		ALERT(at_aiconsole, "KillTarget: %s\n", STRING(m_iszKillTarget));

		while (1)
		{
			pentKillTarget = FIND_ENTITY_BY_TARGETNAME(pentKillTarget, STRING(m_iszKillTarget));

			if (FNullEnt(pentKillTarget))
			{
				break;
			}

			ALERT(at_aiconsole, "killing %s\n", STRING(pentKillTarget->v.classname));
			REMOVE_ENTITY(pentKillTarget);
		}
	}

	if (pev->target)
	{
		FireTargets(STRING(pev->target), pActivator, this, useType, value);
	}
}

void SetMovedir(entvars_t *pev)
{
	if (pev->angles == Vector(0, -1, 0))
	{
		pev->movedir = Vector(0, 0, 1);
	}
	else if (pev->angles == Vector(0, -2, 0))
	{
		pev->movedir = Vector(0, 0, -1);
	}
	else
	{
		UTIL_MakeVectors(pev->angles);
		pev->movedir = gpGlobals->v_forward;
	}

	pev->angles = g_vecZero;
}

void CBaseDelay::DelayThink(void)
{
	CBaseEntity *pActivator = NULL;

	if (pev->owner != NULL)
	{
		pActivator = CBaseEntity::Instance(pev->owner);
	}

	SUB_UseTargets(pActivator, (USE_TYPE)pev->button, 0);
	REMOVE_ENTITY(ENT(pev));
}

void CBaseToggle::KeyValue(KeyValueData *pkvd)
{
	if (!strcmp(pkvd->szKeyName, "lip"))
	{
		m_flLip = atof(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (!strcmp(pkvd->szKeyName, "wait"))
	{
		m_flWait = atof(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (!strcmp(pkvd->szKeyName, "master"))
	{
		m_sMaster = ALLOC_STRING(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (!strcmp(pkvd->szKeyName, "distance"))
	{
		m_flMoveDistance = atof(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else
		CBaseDelay::KeyValue(pkvd);
}

float CBaseToggle::GetDelay(void)
{
	return m_flWait;
}

void CBaseToggle::LinearMove(Vector vecDest, float flSpeed)
{
	m_vecFinalDest = vecDest;

	if (vecDest == pev->origin)
	{
		LinearMoveDone();
		return;
	}

	Vector vecDestDelta = vecDest - pev->origin;

	float flTravelTime = vecDestDelta.Length() / flSpeed;

	SetThink(&CBaseToggle::LinearMoveDone);
	pev->nextthink = pev->ltime + flTravelTime;

	pev->velocity = vecDestDelta / flTravelTime;
}

void CBaseToggle::LinearMoveDone(void)
{
	UTIL_SetOrigin(pev, m_vecFinalDest);
	pev->velocity = g_vecZero;
	pev->nextthink = -1;

	if (m_pfnCallWhenMoveDone)
		(this->*m_pfnCallWhenMoveDone)();
}

//BOOL CBaseToggle::IsLockedByMaster(void)
//{
//	if (m_sMaster && !UTIL_IsMasterTriggered(m_sMaster, m_hActivator))
//		return TRUE;
//	else
//		return FALSE;
//}

void CBaseToggle::AngularMove(Vector vecDestAngle, float flSpeed)
{
	m_vecFinalAngle = vecDestAngle;

	if (vecDestAngle == pev->angles)
	{
		AngularMoveDone();
		return;
	}

	Vector vecDestDelta = vecDestAngle - pev->angles;

	float flTravelTime = vecDestDelta.Length() / flSpeed;

	SetThink(&CBaseToggle::AngularMoveDone);
	pev->nextthink = pev->ltime + flTravelTime;

	pev->avelocity = vecDestDelta / flTravelTime;
}

void CBaseToggle::AngularMoveDone(void)
{
	pev->angles = m_vecFinalAngle;
	pev->avelocity = g_vecZero;
	pev->nextthink = -1;

	if (m_pfnCallWhenMoveDone)
		(this->*m_pfnCallWhenMoveDone)();
}

float CBaseToggle::AxisValue(int flags, Vector &angles)
{
	if (flags & SF_DOOR_ROTATE_Z)
		return angles.z;
	if (flags & SF_DOOR_ROTATE_X)
		return angles.x;

	return angles.y;
}

void CBaseToggle::AxisDir(entvars_t *pev)
{
	if (pev->spawnflags & SF_DOOR_ROTATE_Z)
		pev->movedir = Vector(0, 0, 1);
	else if (pev->spawnflags & SF_DOOR_ROTATE_X)
		pev->movedir = Vector(1, 0 ,0);
	else
		pev->movedir = Vector(0, 1, 0);
}

float CBaseToggle::AxisDelta(int flags, Vector &angle1, Vector &angle2)
{
	if (flags & SF_DOOR_ROTATE_Z)
		return angle1.z - angle2.z;

	if (flags & SF_DOOR_ROTATE_X)
		return angle1.x - angle2.x;

	return angle1.y = angle2.y;
}