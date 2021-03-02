#include "extdll.h"
#include "util.h"
#include "cbase.h"

Vector CBaseEntity::EyePosition(void) const
{
	return pev->origin + pev->view_ofs;
}

bool CBaseEntity::FVisible(CBaseEntity *pEntity) const
{
	TraceResult tr;
	Vector vecLookerOrigin;
	Vector vecTargetOrigin;
	
	if (FBitSet(pEntity->pev->flags, FL_NOTARGET))
		return FALSE;

	if ((pev->waterlevel != 3 && pEntity->pev->waterlevel == 3) 
		|| (pev->waterlevel == 3 && pEntity->pev->waterlevel == 0))
		return FALSE;

	vecLookerOrigin = pev->origin + pev->view_ofs;
	vecTargetOrigin = pEntity->EyePosition();

	UTIL_TraceLine(vecLookerOrigin, vecTargetOrigin, ignore_monsters, ignore_glass, ENT(pev), &tr);
	
	if (tr.flFraction != 1.0)
		return FALSE;
	else
		return TRUE;
}

bool CBaseEntity::FVisible(const Vector &vecOrigin) const
{
	TraceResult tr;
	Vector vecLookerOrigin;

	vecLookerOrigin = EyePosition();

	UTIL_TraceLine(vecLookerOrigin, vecOrigin, ignore_monsters, ignore_glass, ENT(pev), &tr);

	if (tr.flFraction != 1.0)
		return FALSE;
	else
		return TRUE;
}

void CBaseEntity::SUB_StartFadeOut(void)
{
	if (pev->rendermode == kRenderNormal)
	{
		pev->renderamt = 255;
		pev->rendermode = kRenderTransTexture;
	}

	pev->solid = SOLID_NOT;
	pev->avelocity = g_vecZero;

	SetThink(&CBaseEntity::SUB_FadeOut);
	pev->nextthink = gpGlobals->time + 0.1;
}

void CBaseEntity::SUB_FadeOut(void)
{
	if (pev->renderamt > 7)
	{
		pev->renderamt -= 7;
		pev->nextthink = gpGlobals->time + 0.1;
	}
	else
	{
		pev->renderamt = 0;

		SetThink(&CBaseEntity::SUB_Remove);	// ???
		pev->nextthink = gpGlobals->time + 0.2;
	}
}