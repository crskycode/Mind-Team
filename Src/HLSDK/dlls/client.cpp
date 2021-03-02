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
#include "weapons.h"
#include "message.h"
#include "spectator.h"
#include "customentity.h"
#include "usercmd.h"
#include "pm_shared.h"
#include "gamerule.h"
#include "delta.h"
//#define OLD_BOT
#if defined OLD_BOT
#include "cs_bot.h"
#endif
#if defined CSS_BOT
#include "nav_int.h"
#include "source.h"
#include "bot_manager.h"
#endif

extern "C"
{
#include <pm_hull.h>
}

extern int gmsgSayText;
extern ULONG g_ulFrameCount;

void LinkUserMessages(void);


BOOL ClientConnect(edict_t *pEntity, const char *pszName, const char *pszAddress, char szRejectReason[128])
{
	return TRUE;
}

void ClientDisconnect(edict_t *pEntity)
{
	CBasePlayer *pPlayer = (CBasePlayer *)CBaseEntity::Instance(pEntity);

	if (!pPlayer)
		return;

	int i = ENTINDEX(pEntity) - 1;

	pPlayer->RemoveAllWeapons();

	pPlayer->pev->classname = iStringNull;
	pPlayer->pev->solid = SOLID_NOT;
	pPlayer->pev->movetype = MOVETYPE_NONE;
	pPlayer->pev->flags = 0;
	pPlayer->pev->effects = 0;
	pPlayer->pev->deadflag = DEAD_DEAD;
	pPlayer->pev->health = 0;
	pPlayer->pev->takedamage = DAMAGE_NO;

	CBasePlayer *pOther = NULL;

	while ((pOther = (CBasePlayer *)UTIL_FindEntityByClassname(pOther, "player")) != NULL)
	{
		if (pOther->m_hSpectatorTarget == pPlayer)
		{
			int iMode = pOther->pev->iuser1;
			pOther->pev->iuser1 = 0;
			pOther->Spectator_SetMode(iMode);
		}
	}

	pPlayer->m_bDisconnected = TRUE;
	pPlayer->m_iJoiningState = WAITINGCLIENT;

	g_pGameRule->PlayerDisconnect(pPlayer);

	ALERT(at_console, "%s disconnect from server\n", STRING(pPlayer->pev->netname));

#if defined OLD_BOT
	if (TheBots)
	{
		TheBots->ClientDisconnect(pPlayer);
	}
#endif
}

void ClientKill(edict_t *pEntity)
{
	CBasePlayer *pPlayer = (CBasePlayer *)GET_PRIVATE(pEntity);

#ifdef _DEBUG
	pPlayer->Killed(pPlayer->pev, pPlayer->pev);
#endif
}

void ClientPutInServer(edict_t *pEntity)
{
	CBasePlayer *pPlayer = (CBasePlayer *)GetClassPtr((CBasePlayer *)&pEntity->v);

	if (!pPlayer)
		return;

	pPlayer->pev->classname = iStringNull;
	pPlayer->pev->solid = SOLID_NOT;
	pPlayer->pev->movetype = MOVETYPE_NONE;
	pPlayer->pev->effects = EF_NODRAW;
	pPlayer->pev->angles = g_vecZero;
	pPlayer->pev->fixangle = TRUE;
	pPlayer->pev->punchangle = g_vecZero;
	pPlayer->pev->velocity = g_vecZero;
	pPlayer->pev->deadflag = DEAD_DEAD;
	pPlayer->pev->health = 0;
	pPlayer->pev->takedamage = DAMAGE_NO;
	pPlayer->pev->iuser1 = OBS_CHASE_FREE;
	pPlayer->pev->iuser2 = 0;
	pPlayer->pev->iuser3 = 0;
	pPlayer->pev->iuser4 = 0;
	pPlayer->pev->fuser1 = 0;
	pPlayer->pev->fuser2 = 0;
	pPlayer->pev->fuser3 = 0;
	pPlayer->pev->fuser4 = 0;

	SET_MODEL(pEntity, "models/character/player.mdl");

	pPlayer->m_fJustConnected = TRUE;
	pPlayer->m_bDisconnected = FALSE;

	pPlayer->m_bIsObserver = FALSE;
	pPlayer->m_iFOV = 90;

	pPlayer->m_iSpectatorLastMode = OBS_CHASE_FREE;

	pPlayer->m_iJoiningState = PUTINSERVER;

	ALERT(at_console, "%s put in the server\n", STRING(pPlayer->pev->netname));
}

void ClientCommand(edict_t *pEntity)
{
	CBasePlayer *pPlayer = (CBasePlayer *)GET_PRIVATE(pEntity);

	if (!pPlayer)
		return;

	const char *pcmd = CMD_ARGV(0);

	if (!strcmp(pcmd, "join"))
	{
		if (pPlayer->m_iJoiningState == JOINED)
			return;

		pPlayer->m_iJoinToTeam		= atoi(CMD_ARGV(1));
		pPlayer->m_iJoinToCharacter	= atoi(CMD_ARGV(2));
		pPlayer->m_iJoinToObserver	= atoi(CMD_ARGV(3));

		pPlayer->m_iJoiningState = GETINTOGAME;
		return;
	}
	else if (!strcmp(pcmd, "drop"))
	{
		pPlayer->DropPlayerWeapon();
		return;
	}
	else if (!strcmp(pcmd, "lastinv"))
	{
		pPlayer->SelectLastWeapon();
		return;
	}
	else if (!strcmp(pcmd, "invprev"))
	{
		pPlayer->SelectNextWeapon(TRUE);
		return;
	}
	else if (!strcmp(pcmd, "invnext"))
	{
		pPlayer->SelectNextWeapon(FALSE);
		return;
	}
	else if (!strcmp(pcmd, "slot1"))
	{
		pPlayer->SelectSlot(0);
		return;
	}
	else if (!strcmp(pcmd, "slot2"))
	{
		pPlayer->SelectSlot(1);
		return;
	}
	else if (!strcmp(pcmd, "slot3"))
	{
		pPlayer->SelectSlot(2);
		return;
	}
	else if (!strcmp(pcmd, "slot4"))
	{
		pPlayer->SelectSlot(3);
		return;
	}
	else if (!strcmp(pcmd, "slot5"))
	{
		pPlayer->SelectSlot(4);
		return;
	}
	else if (!strcmp(pcmd, "radio1"))
	{
		int num = atoi(CMD_ARGV(1));
		pPlayer->Radio(1, num);
		return;
	}
	else if (!strcmp(pcmd, "radio2"))
	{
		int num = atoi(CMD_ARGV(1));
		pPlayer->Radio(2, num);
		return;
	}
	else if (!strcmp(pcmd, "radio3"))
	{
		int num = atoi(CMD_ARGV(1));
		pPlayer->Radio(3, num);
		return;
	}
	else if (!strcmp(pcmd, "bagreset"))
	{
		if (pPlayer->m_bEnableBag)
		{
			pPlayer->RemoveAllWeapons();
		}

		return;
	}
	else if (!strcmp(pcmd, "bagbuy"))
	{
		if (!pPlayer->m_bEnableBag)
			return;

		int iPrimary = atoi(CMD_ARGV(1));
		int iSecondary = atoi(CMD_ARGV(2));
		int iKnife = atoi(CMD_ARGV(3));
		int iEquipment1 = atoi(CMD_ARGV(4));
		int iEquipment2 = atoi(CMD_ARGV(5));
		int iEquipment3 = atoi(CMD_ARGV(6));

		if (iPrimary)
			pPlayer->GiveWeapon(iPrimary);

		if (iSecondary)
			pPlayer->GiveWeapon(iSecondary);

		if (iKnife)
			pPlayer->GiveWeapon(iKnife);

		if (iEquipment1)
			pPlayer->GiveWeapon(iEquipment1);

		if (iEquipment2)
			pPlayer->GiveWeapon(iEquipment2);

		if (iEquipment3)
			pPlayer->GiveWeapon(iEquipment3);

		pPlayer->RetireWeapon();
		return;
	}
	else if (!strcmp(pcmd, "_spawn"))
	{
		pPlayer->Spawn();
		return;
	}

	ClientPrint(pEntity, HUD_PRINTCONSOLE, "Unknown command: %s\n", pcmd);
}

void ClientUserInfoChanged(edict_t *pEntity, char *infobuffer)
{
}

void ServerDeactivate(void)
{
	if (g_pGameRule)
		g_pGameRule->ServerDeactivate();

#if defined OLD_BOT
	if (TheBots)
	{
		TheBots->ServerDeactivate();
	}
#endif
#if defined CSS_BOT
	if (TheBots)
	{
		TheBots->ServerDeactivate();
	}
#endif
}

void R_DrawRectangle( vec3_t tl, vec3_t bl, vec3_t tr, vec3_t br, int color[3], int life )
{
	//PM_ParticleLine( tl, bl, pcolor, life, 0 );
	//PM_ParticleLine( bl, br, pcolor, life, 0 );
	//PM_ParticleLine( br, tr, pcolor, life, 0 );
	//PM_ParticleLine( tr, tl, pcolor, life, 0 );
	UTIL_DrawBeamPoints( tl, bl, life, color[0], color[1], color[2] );
	UTIL_DrawBeamPoints( bl, br, life, color[0], color[1], color[2] );
	UTIL_DrawBeamPoints( br, tr, life, color[0], color[1], color[2] );
	UTIL_DrawBeamPoints( tr, tl, life, color[0], color[1], color[2] );
}

static int PM_boxpnt[6][4] =
{
	{ 0, 4, 6, 2 },		// +X
	{ 0, 1, 5, 4 },		// +Y
	{ 0, 2, 3, 1 },		// +Z
	{ 7, 5, 1, 3 },		// -X
	{ 7, 3, 2, 6 },		// -Y
	{ 7, 6, 4, 5 },		// -Z
};

void R_DrawBBox( vec3_t mins, vec3_t maxs, vec3_t origin, int color[3], int life )
{
	int		j;

	vec3_t	tmp;
	vec3_t	p[8];
	float	gap = 0;

	for ( j = 0; j < 8; j++ )
	{
		tmp[0] = ( j & 1 ) ? mins[0] - gap : maxs[0] + gap;
		tmp[1] = ( j & 2 ) ? mins[1] - gap : maxs[1] + gap;
		tmp[2] = ( j & 4 ) ? mins[2] - gap : maxs[2] + gap;

		VectorAdd( tmp, origin, tmp );
		VectorCopy( tmp, p[j] );
	}

	for ( j = 0; j < 6; j++ )
	{
		R_DrawRectangle(
		    p[PM_boxpnt[j][1]],
		    p[PM_boxpnt[j][0]],
		    p[PM_boxpnt[j][2]],
		    p[PM_boxpnt[j][3]],
			color,
		    life );
	}
}

class CModzz : public CBaseAnimating
{
public:
	void Spawn(void)
	{
		pev->classname = MAKE_STRING("modzz01");
		//pev->origin = Vector(0, 0, 30);
		pev->gravity = 1.0f;
		pev->friction = 1.0f;
		pev->solid = SOLID_BBOX;
		pev->movetype = MOVETYPE_TOSS;
		//SET_MODEL(edict(), "gfx/obj.mdl");
		//UTIL_SetSize(pev, Vector(-23, -23, -66), Vector(23, 23, 66));
		//UTIL_SetOrigin(pev, pev->origin);
		//DROP_TO_FLOOR(edict());

		pev->nextthink = gpGlobals->time + 0.2f;

		//m_color[0] = RANDOM_LONG(50, 255);
		//m_color[1] = RANDOM_LONG(50, 255);
		//m_color[2] = RANDOM_LONG(50, 255);
	}
	bool Collide(CBaseEntity *pOther)
	{
		//const char *me = STRING(pev->targetname);
		//if ( FStrEq( STRING(pOther->pev->targetname), "trg2" ) )
		//	return false;
		return true;
	}

	void Think(void)
	{
		R_DrawBBox( pev->mins, pev->maxs, pev->origin, m_color, 2 );

		//pev->angles[2] = 40;

		//StudioFrameAdvance();

		pev->nextthink = gpGlobals->time + 0.2f;
	}

	int m_color[3];

	void Touch(CBaseEntity *pOther)
	{
	}
};
////
////void Test(void)
////{
////	CModzz *ent1 = GetClassPtr((CModzz *)NULL);
////	ent1->pev->origin = Vector( 0, 0, 30 );
////	ent1->pev->targetname = MAKE_STRING("trg1");
////	ent1->m_color[0] = 100;
////	ent1->m_color[1] = 255;
////	ent1->m_color[2] = 255;
////	ent1->Spawn();
////
////	CModzz *ent2 = GetClassPtr((CModzz *)NULL);
////	ent2->pev->origin = Vector( 100, 0, 30 );
////	ent2->pev->targetname = MAKE_STRING("trg2");
////	ent2->m_color[0] = 255;
////	ent2->m_color[1] = 50;
////	ent2->m_color[2] = 50;
////	ent2->Spawn();
////
////	CBasePlayer *pl = UTIL_PlayerByIndex(1);
////	if (pl)
////	{
////		//pl->pev->groupinfo = (1<<8);
////
////		//ent1->pev->groupinfo = (1<<7);
////
////		//g_engfuncs.pfnSetGroupMask( 0, GROUP_OP_AND );
////	}
////}
////
////void Test2(void)
////{
////	CBaseEntity *ent = UTIL_FindEntityByTargetname(NULL, "trg1");
////
////	//g_engfuncs.pfnWalkMove( ent->edict(), 0, 20, 1 );
////
////	ent->pev->velocity = Vector(30, 0, 80);
////
////	//UTIL_SetOrigin(ent->pev, ent->pev->origin);
////
////	//Con_Printf("%f %f %f, %f %f %f\n", ent->pev->absmin.x,
////	//								   ent->pev->absmin.y,
////	//								   ent->pev->absmin.z,
////	//								   ent->pev->absmax.x,
////	//								   ent->pev->absmax.y,
////	//								   ent->pev->absmax.z);
////}

#include <metastudio.h>

//void Test3(void)
//{
//	CModzz *pEntity = GetClassPtr<CModzz>(NULL);
//
//	pEntity->pev->solid = SOLID_BBOX;
//	pEntity->pev->movetype = MOVETYPE_TOSS;
//
//	pEntity->pev->angles = Vector(0, 0, 0);
//
//	SET_MODEL(pEntity->edict(), "gfx/new_omoh_body_gr.mdl");
//
//	metastudiohdr_t *phdr = (metastudiohdr_t *)GET_MODEL_PTR(pEntity->edict());
//
//	vec3_t bbmin, bbmax;
//	VectorCopy(phdr->bbmin, bbmin);
//	VectorCopy(phdr->bbmax, bbmax);
//	bbmin[0] = -16;
//	bbmin[1] = -16;
//	bbmax[0] = 16;
//	bbmax[1] = 16;
//	SET_SIZE(pEntity->edict(), bbmin, bbmax);
//
//	SET_ORIGIN(pEntity->edict(), Vector(0, 0, 30));
//
//	pEntity->m_color[0] = 200;
//	pEntity->m_color[1] = 255;
//	pEntity->m_color[2] = 10;
//	pEntity->Spawn();
//}
//
//void Test4(void)
//{
//	edict_t *pEdict = g_engfuncs.pfnPEntityOfEntIndex(1);
//
//	if (pEdict == NULL)
//		return;
//
//	vec3_t forward;
//
//	g_engfuncs.pfnAngleVectors(pEdict->v.v_angle, forward, NULL, NULL);
//
//	vec3_t startpos, endpos;
//
//	startpos = pEdict->v.origin + pEdict->v.view_ofs;
//	endpos = startpos + (forward * 500.0f);
//
//	TraceResult tr;
//
//	g_engfuncs.pfnTraceLine(startpos, endpos, 0, pEdict, &tr);
//
//	UTIL_DrawBeamPoints(startpos, tr.vecEndPos, 300, 255, 0, 0);
//}

//void Test5(void)
//{
//	edict_t *player = INDEXENT(1);
//
//	float vecOrigin[3];
//	float vecForward[3];
//	float vecRight[3];
//	float vecUp[3];
//
//	VectorCopy(player->v.origin, vecOrigin);
//	VectorAdd(vecOrigin, player->v.view_ofs, vecOrigin);
//
//	g_engfuncs.pfnMakeVectors(player->v.v_angle);
//
//	VectorCopy(gpGlobals->v_forward, vecForward);
//	VectorCopy(gpGlobals->v_right, vecRight);
//	VectorCopy(gpGlobals->v_up, vecUp);
//
//	for (int i = 0; i < 10; i++)
//	{
//		float vecSpread[2];
//		float vecRightOffset[3];
//		float vecUpOffset[3];
//		float vecDir[3];
//		float vecEndPos[3];
//
//		vecSpread[0] = RANDOM_FLOAT(-0.07, 0.07);
//		vecSpread[1] = RANDOM_FLOAT(-0.07, 0.07);
//
//		VectorScale(vecRight, vecSpread[0], vecRightOffset);
//		VectorScale(vecUp, vecSpread[1], vecUpOffset);
//
//		VectorAdd(vecForward, vecRightOffset, vecDir);
//		VectorAdd(vecDir, vecUpOffset, vecDir);
//
//		VectorNormalize(vecDir);
//
//		VectorScale(vecDir, 4096.0, vecDir);
//		VectorAdd(vecOrigin, vecDir, vecEndPos);
//
//		TraceResult tr;
//		g_engfuncs.pfnTraceLine(vecOrigin, vecEndPos, 0, player, &tr);
//
//		UTIL_DrawBeamPoints(vecOrigin, tr.vecEndPos, 500.0, 200, 255, 100);
//	}
//}

void VectorSlerp2( float *a, float *b, float t, float *out )
{
	float angle;
	float v[3];
	float len;
	float c;
	float s;
	float tmp[3];
	float m[3][3];

	if ( t < 0 )
		t = 0;
	else if ( t > 1 )
		t = 1;

	angle = acos(a[0] * b[0] + a[1] * b[1] + a[2] * b[2]) * t;

	v[0] = a[1] * b[2] - b[1] * a[2];
	v[1] = a[2] * b[0] - b[2] * a[0];
	v[2] = a[0] * b[1] - b[0] * a[1];

	len = sqrt( v[0] * v[0] + v[1] * v[1] + v[2] * v[2] );

	if ( len > 0 )
	{
		float s = 1.0 / len;
		v[0] *= s;
		v[1] *= s;
		v[2] *= s;
	}

	c = cos( angle );
	s = sin( angle );

	tmp[0] = (1 - c) * v[0];
	tmp[1] = (1 - c) * v[1];
	tmp[2] = (1 - c) * v[2];

	m[0][0] = c + tmp[0] * v[0];
	m[0][1] = tmp[0] * v[1] + s * v[2];
	m[0][2] = tmp[0] * v[2] - s * v[1];
	m[1][0] = tmp[1] * v[0] - s * v[2];
	m[1][1] = c + tmp[1] * v[1];
	m[1][2] = tmp[1] * v[2] + s * v[0];
	m[2][0] = tmp[2] * v[0] + s * v[1];
	m[2][1] = tmp[2] * v[1] - s * v[0];
	m[2][2] = c + tmp[2] * v[2];

	out[0] = m[0][0] * a[0] + m[1][0] * a[1] + m[2][0] * a[2];
	out[1] = m[0][1] * a[0] + m[1][1] * a[1] + m[2][1] * a[2];
	out[2] = m[0][2] * a[0] + m[1][2] * a[1] + m[2][2] * a[2];
}

void Test6( void )
{
	edict_t *pl = INDEXENT( 1 );

	float vecOrigin[3];
	float vecForward[3];
	float vecRight[3];
	float vecUp[3];

	VectorCopy( pl->v.origin, vecOrigin );
	//VectorAdd( vecOrigin, pl->v.view_ofs, vecOrigin );

	VectorCopy( gpGlobals->v_forward, vecForward );
	VectorCopy( gpGlobals->v_right, vecRight );
	VectorCopy( gpGlobals->v_up, vecUp );

	g_engfuncs.pfnMakeVectors( pl->v.v_angle );	//ÍùÇ°ÒÆ25
	VectorScale( vecForward, 25, vecForward );
	VectorAdd( vecOrigin, vecForward, vecOrigin );

	float vecDir[3];
	float vecEnd[3];

	// forward
	VectorScale( gpGlobals->v_forward, 1000.0, vecDir );
	VectorAdd( vecOrigin, vecDir, vecEnd );
	UTIL_DrawBeamPoints( vecOrigin, vecEnd, 200, 0, 255, 0 );

	// right
	VectorScale( gpGlobals->v_right, 1000.0, vecDir );
	VectorAdd( vecOrigin, vecDir, vecEnd );
	UTIL_DrawBeamPoints( vecOrigin, vecEnd, 200, 255, 0, 0 );

	// center
	/*VectorAdd( gpGlobals->v_forward, gpGlobals->v_right, vecDir );
	VectorNormalize( vecDir );
	VectorScale( vecDir, 1000.0, vecDir );
	VectorAdd( vecOrigin, vecDir, vecEnd );
	UTIL_DrawBeamPoints( vecOrigin, vecEnd, 200, 255, 255, 0 );*/

	float t = 0.1;

	for ( int i = 0; i < 9; i++ )
	{
		VectorSlerp2( gpGlobals->v_forward, gpGlobals->v_right, t, vecDir );
		t += 0.1;

		VectorScale( vecDir, 1000.0, vecDir );
		VectorAdd( vecOrigin, vecDir, vecEnd );
		UTIL_DrawBeamPoints( vecOrigin, vecEnd, 200, 255, 255, 255 );
	}

	/*VectorSlerp2( gpGlobals->v_forward, gpGlobals->v_right, 0.5, vecDir );
	VectorNormalize( vecDir );
	VectorScale( vecDir, 1000.0, vecDir );
	VectorAdd( vecOrigin, vecDir, vecEnd );
	UTIL_DrawBeamPoints( vecOrigin, vecEnd, 200, 255, 255, 255 );*/
}

void ServerActivate(edict_t *pEdictList, int edictCount, int clientMax)
{
	////ADD_SERVER_COMMAND( "ttt", Test );
	////ADD_SERVER_COMMAND( "ttt2", Test2 );
	//ADD_SERVER_COMMAND("ttt3", Test3);
	//ADD_SERVER_COMMAND("ttt4", Test4);
	//ADD_SERVER_COMMAND("ttt5", Test5);
	ADD_SERVER_COMMAND( "ttt6", Test6 );
	//for (int i = 0; i < edictCount; ++i)
	//{
	//	if (pEdictList[i].free)
	//		continue;

	//	if (i < clientMax || !pEdictList[i].pvPrivateData)
	//		continue;

	//	CBaseEntity *pClass = CBaseEntity::Instance(&pEdictList[i]);

	//	if (pClass && !FBitSet(pClass->pev->flags, FL_DORMANT))
	//	{
	//		pClass->Activate();
	//	}
	//	else
	//		ALERT(at_console, "Can't instance %s\n", STRING(pEdictList[i].v.classname));
	//}

	LinkUserMessages();

	if (g_pGameRule)
		g_pGameRule->ServerActivate();

#if defined OLD_BOT
	if (TheBots)
	{
		TheBots->ServerActivate();
	}
#endif
	
#if defined CSS_BOT
	NAV_Load();
	
	if (TheBots)
	{
		TheBots->ServerActivate();
	}
#endif
}

void PlayerPreThink(edict_t *pEntity)
{
	CBasePlayer *pPlayer = (CBasePlayer *)GET_PRIVATE(pEntity);

	if (pPlayer)
		pPlayer->PreThink();
}

void PlayerPostThink(edict_t *pEntity)
{
	CBasePlayer *pPlayer = (CBasePlayer *)GET_PRIVATE(pEntity);

	if (pPlayer)
		pPlayer->PostThink();
}

void ParmsNewLevel(void)
{
}

void ParmsChangeLevel(void)
{
}

void StartFrame(void)
{
	if (g_pGameRule)
		g_pGameRule->Think();

#if defined OLD_BOT
	if (TheBots)
	{
		TheBots->StartFrame();
	}
#endif

#if defined CSS_BOT
	NAV_Update();

	if (TheBots)
	{
		TheBots->StartFrame();
	}
#endif

	g_ulFrameCount++;
}

const char *GetGameDescription(void)
{
	return "Mind-Team";
}

void Sys_Error(const char *error_string)
{
	//__asm int 3;
}

void PlayerCustomization(edict_t *pEntity, struct customization_s *pCust)
{
}

void SpectatorConnect(edict_t *pEntity)
{
	CBaseObserver *pObserver = (CBaseObserver *)CBaseEntity::Instance(pEntity);

	if (pObserver)
		pObserver->ObserverConnect();
}

void SpectatorDisconnect(edict_t *pEntity)
{
	CBaseObserver *pObserver = (CBaseObserver *)CBaseEntity::Instance(pEntity);

	if (pObserver)
		pObserver->ObserverDisconnect();
}

void SpectatorThink(edict_t *pEntity)
{
	CBaseObserver *pObserver = (CBaseObserver *)CBaseEntity::Instance(pEntity);

	if (pObserver)
		pObserver->ObserverThink();
}

void SetupVisibility(edict_t *pViewEntity, edict_t *pClient, byte **pvs, byte **pas)
{
	edict_t *pView = pClient;

	if (pViewEntity)
		pView = pViewEntity;

	if (pClient->v.flags & FL_PROXY)
	{
		*pvs = NULL;
		*pas = NULL;

		return;
	}

	Vector org = pView->v.origin + pView->v.view_ofs;

	if (pView->v.flags & FL_DUCKING)
	{
		org = org + (VEC_IDLE_HULL_MIN - VEC_DUCK_HULL_MIN);
	}

	*pvs = ENGINE_SET_PVS((float *)&org);
	*pas = ENGINE_SET_PAS((float *)&org);
}


#include "entity_state.h"


BOOL AddToFullPack(struct entity_state_s *state, int e, edict_t *ent, edict_t *host, int hostflags, int player, unsigned char *pSet)
{
	CBasePlayer *pPlayer = NULL;

	if (player)
	{
		pPlayer = (CBasePlayer *)CBaseEntity::Instance(ent);
	}

	if (FBitSet(ent->v.effects, EF_NODRAW) && ent != host)
		return FALSE;

	if (!ent->v.modelindex || !STRING(ent->v.model))
		return FALSE;

	if (FBitSet(ent->v.flags, FL_SPECTATOR) && ent != host)
		return FALSE;

	if (ent != host)
	{
		if (!ENGINE_CHECK_VISIBILITY(ent, pSet))
			return FALSE;
	}

	if (ent->v.flags & FL_SKIPLOCALHOST)
	{
		if (FBitSet(hostflags, 1) && ent->v.owner == host)
			return FALSE;
	}

	//if (host->v.groupinfo)
	//{
	//	UTIL_SetGroupTrace(host->v.groupinfo, GROUP_OP_AND);

	//	if (ent->v.groupinfo)
	//	{
	//		if (g_groupop == GROUP_OP_AND)
	//		{
	//			if (!(ent->v.groupinfo & host->v.groupinfo))
	//				return FALSE;
	//		}
	//		else if (g_groupop == GROUP_OP_NAND)
	//		{
	//			if (ent->v.groupinfo & host->v.groupinfo)
	//				return FALSE;
	//		}
	//	}

	//	UTIL_UnsetGroupTrace();
	//}
	//int solid = ent->v.solid;
	//if (ent->v.groupinfo != 0 && host->v.groupinfo != 0)
	//{
	//	if ( (ent->v.groupinfo & host->v.groupinfo) == 0 )
	//	{
	//		solid = SOLID_NOT;
	//	}
	//}

	memset(state, 0, sizeof(entity_state_t));

	state->number = e;
	state->entityType = ENTITY_NORMAL;

	if (ent->v.flags & FL_CUSTOMENTITY)
	{
		state->entityType = ENTITY_BEAM;
	}

	state->animtime = (int)(1000.0 * ent->v.animtime) / 1000.0;

	memcpy(state->origin, ent->v.origin, sizeof(vec3_t));
	memcpy(state->angles, ent->v.angles, sizeof(vec3_t));
	memcpy(state->mins, ent->v.mins, sizeof(vec3_t));
	memcpy(state->maxs, ent->v.maxs, sizeof(vec3_t));
	memcpy(state->startpos, ent->v.startpos, sizeof(vec3_t));
	memcpy(state->endpos, ent->v.endpos, sizeof(vec3_t));

	state->impacttime = ent->v.impacttime;
	state->starttime = ent->v.starttime;
	state->modelindex = ent->v.modelindex;
	state->frame = ent->v.frame;
	state->skin = ent->v.skin;
	state->effects = ent->v.effects;

	if (!player && ent->v.animtime && !ent->v.velocity[0] && !ent->v.velocity[1] && !ent->v.velocity[2])
		state->eflags |= EFLAG_SLERP;

	state->scale = ent->v.scale;
	state->solid = ent->v.solid;
	state->colormap = ent->v.colormap;
	state->movetype = ent->v.movetype;
	state->sequence = ent->v.sequence;
	state->framerate = ent->v.framerate;
	state->body = ent->v.body;

	for (int i = 0; i < 4; ++i)
		state->controller[i] = ent->v.controller[i];

	for (int i = 0; i < 2; ++i)
		state->blending[i] = ent->v.blending[i];

	state->rendermode = ent->v.rendermode;
	state->renderamt = ent->v.renderamt;
	state->renderfx = ent->v.renderfx;
	state->rendercolor.r = ent->v.rendercolor.x;
	state->rendercolor.g = ent->v.rendercolor.y;
	state->rendercolor.b = ent->v.rendercolor.z;
	state->aiment = 0;

	if (ent->v.aiment)
		state->aiment = ENTINDEX(ent->v.aiment);

	state->owner = 0;

	if (ent->v.owner)
	{
		int owner = ENTINDEX(ent->v.owner);

		if (owner >= 1 && owner <= gpGlobals->maxClients)
			state->owner = owner;
	}

	if (!player)
		state->playerclass = ent->v.playerclass;

	if (player)
	{
		//
		VectorCopy(ent->v.v_angle, state->angles);

		memcpy(state->basevelocity, ent->v.basevelocity, sizeof(vec3_t));

		state->weaponmodel = ent->v.weaponmodel;
		state->gaitsequence = ent->v.gaitsequence;
		state->spectator = FBitSet(ent->v.flags, FL_SPECTATOR) ? TRUE : FALSE;
		state->friction = ent->v.friction;
		state->gravity = ent->v.gravity;
		state->usehull = FBitSet(ent->v.flags, FL_DUCKING) ? TRUE : FALSE;
		state->health = ent->v.health;

		// WeaponID for PModel on client
		state->skin = pPlayer->m_pActiveWeapon ? pPlayer->m_pActiveWeapon->m_Info.iWeaponIndex : 0;
	}

	if (FClassnameIs(&ent->v, "weaponbox"))
	{
		state->iuser1 = ent->v.iuser1;
		state->iuser2 = ent->v.iuser2;
	}

	return TRUE;
}

void CreateBaseline(BOOL player, int eindex, struct entity_state_s *baseline, struct edict_s *entity, int playermodelindex, vec3_t player_mins, vec3_t player_maxs)
{
	baseline->origin = entity->v.origin;
	baseline->angles = entity->v.angles;
	baseline->frame = entity->v.frame;
	baseline->skin = (short)entity->v.skin;
	baseline->rendermode = (byte)entity->v.rendermode;
	baseline->renderamt = (byte)entity->v.renderamt;
	baseline->rendercolor.r = (byte)entity->v.rendercolor.x;
	baseline->rendercolor.g = (byte)entity->v.rendercolor.y;
	baseline->rendercolor.b = (byte)entity->v.rendercolor.z;
	baseline->renderfx = (byte)entity->v.renderfx;

	if (player)
	{
		baseline->mins = player_mins;
		baseline->maxs = player_maxs;
		baseline->colormap = eindex;
		baseline->modelindex = playermodelindex;
		baseline->friction = 1.0f;
		baseline->movetype = MOVETYPE_WALK;
		baseline->scale = entity->v.scale;
		baseline->solid = SOLID_SLIDEBOX;
		baseline->framerate = 1.0f;
		baseline->gravity = 1.0f;
	}
	else
	{
		baseline->mins = entity->v.mins;
		baseline->maxs = entity->v.maxs;
		baseline->colormap = 0;
		baseline->modelindex = entity->v.modelindex;
		baseline->movetype = entity->v.movetype;
		baseline->scale = entity->v.scale;
		baseline->solid = entity->v.solid;
		baseline->framerate = entity->v.framerate;
		baseline->gravity = entity->v.gravity;
	}
}


struct entity_field_alias_t
{
	char name[32];
	int field;
};

#define FIELD_ORIGIN0	0
#define FIELD_ORIGIN1	1
#define FIELD_ORIGIN2	2
#define FIELD_ANGLES0	3
#define FIELD_ANGLES1	4
#define FIELD_ANGLES2	5
#define FIELD_IUSER1	6
#define FIELD_IUSER2	7

static entity_field_alias_t entity_field_alias[] = 
{
	{ "origin[0]", 0 },
	{ "origin[1]", 0 },
	{ "origin[2]", 0 },
	{ "angles[0]", 0 },
	{ "angles[1]", 0 },
	{ "angles[2]", 0 },
	{ "iuser1", 0 },
	{ "iuser2", 0 },
};


void Entity_FieldInit(struct delta_s *pFields)
{
	entity_field_alias[FIELD_ORIGIN0].field = DELTA_FINDFIELD(pFields, entity_field_alias[FIELD_ORIGIN0].name);
	entity_field_alias[FIELD_ORIGIN1].field = DELTA_FINDFIELD(pFields, entity_field_alias[FIELD_ORIGIN1].name);
	entity_field_alias[FIELD_ORIGIN2].field = DELTA_FINDFIELD(pFields, entity_field_alias[FIELD_ORIGIN2].name);
	entity_field_alias[FIELD_ANGLES0].field = DELTA_FINDFIELD(pFields, entity_field_alias[FIELD_ANGLES0].name);
	entity_field_alias[FIELD_ANGLES1].field = DELTA_FINDFIELD(pFields, entity_field_alias[FIELD_ANGLES1].name);
	entity_field_alias[FIELD_ANGLES2].field = DELTA_FINDFIELD(pFields, entity_field_alias[FIELD_ANGLES2].name);
	entity_field_alias[FIELD_IUSER1].field = DELTA_FINDFIELD(pFields, entity_field_alias[FIELD_IUSER1].name);
	entity_field_alias[FIELD_IUSER2].field = DELTA_FINDFIELD(pFields, entity_field_alias[FIELD_IUSER2].name);
}

void Entity_Encode(struct delta_s *pFields, const byte *from, const byte *to)
{
	static bool initialized = false;

	if (!initialized)
	{
		Entity_FieldInit(pFields);
		initialized = true;
	}

	entity_state_t *f = (entity_state_t *)from;
	entity_state_t *t = (entity_state_t *)to;

	bool localplayer = (t->number - 1) == ENGINE_CURRENT_PLAYER();

	if (localplayer)
	{
		DELTA_UNSETBYINDEX(pFields, entity_field_alias[FIELD_ORIGIN0].field);
		DELTA_UNSETBYINDEX(pFields, entity_field_alias[FIELD_ORIGIN1].field);
		DELTA_UNSETBYINDEX(pFields, entity_field_alias[FIELD_ORIGIN2].field);
	}

	if (t->impacttime != 0 && t->starttime != 0)
	{
		DELTA_UNSETBYINDEX(pFields, entity_field_alias[FIELD_ORIGIN0].field);
		DELTA_UNSETBYINDEX(pFields, entity_field_alias[FIELD_ORIGIN1].field);
		DELTA_UNSETBYINDEX(pFields, entity_field_alias[FIELD_ORIGIN2].field);
		DELTA_UNSETBYINDEX(pFields, entity_field_alias[FIELD_ANGLES0].field);
		DELTA_UNSETBYINDEX(pFields, entity_field_alias[FIELD_ANGLES1].field);
		DELTA_UNSETBYINDEX(pFields, entity_field_alias[FIELD_ANGLES2].field);
	}

	if (t->movetype == MOVETYPE_FOLLOW && t->aiment != 0)
	{
		DELTA_UNSETBYINDEX(pFields, entity_field_alias[FIELD_ORIGIN0].field);
		DELTA_UNSETBYINDEX(pFields, entity_field_alias[FIELD_ORIGIN1].field);
		DELTA_UNSETBYINDEX(pFields, entity_field_alias[FIELD_ORIGIN2].field);
	}
	else if (t->aiment != f->aiment)
	{
		DELTA_SETBYINDEX(pFields, entity_field_alias[FIELD_ORIGIN0].field);
		DELTA_SETBYINDEX(pFields, entity_field_alias[FIELD_ORIGIN1].field);
		DELTA_SETBYINDEX(pFields, entity_field_alias[FIELD_ORIGIN2].field);
	}
}


static entity_field_alias_t player_field_alias[] = 
{
	{ "origin[0]", 0 },
	{ "origin[1]", 0 },
	{ "origin[2]", 0 },
};

void Player_FieldInit(struct delta_s *pFields)
{
	player_field_alias[FIELD_ORIGIN0].field = DELTA_FINDFIELD(pFields, player_field_alias[FIELD_ORIGIN0].name);
	player_field_alias[FIELD_ORIGIN1].field = DELTA_FINDFIELD(pFields, player_field_alias[FIELD_ORIGIN1].name);
	player_field_alias[FIELD_ORIGIN2].field = DELTA_FINDFIELD(pFields, player_field_alias[FIELD_ORIGIN2].name);
}

void Player_Encode(struct delta_s *pFields, const unsigned char *from, const unsigned char *to)
{
	static bool initialized = false;

	if (!initialized)
	{
		Player_FieldInit(pFields);
		initialized = true;
	}

	entity_state_t *f = (entity_state_t *)from;
	entity_state_t *t = (entity_state_t *)to;

	bool localplayer = (t->number - 1) == ENGINE_CURRENT_PLAYER();

	if (localplayer)
	{
		DELTA_UNSETBYINDEX(pFields, entity_field_alias[FIELD_ORIGIN0].field);
		DELTA_UNSETBYINDEX(pFields, entity_field_alias[FIELD_ORIGIN1].field);
		DELTA_UNSETBYINDEX(pFields, entity_field_alias[FIELD_ORIGIN2].field);
	}

	if (t->movetype == MOVETYPE_FOLLOW && t->aiment != 0)
	{
		DELTA_UNSETBYINDEX(pFields, entity_field_alias[FIELD_ORIGIN0].field);
		DELTA_UNSETBYINDEX(pFields, entity_field_alias[FIELD_ORIGIN1].field);
		DELTA_UNSETBYINDEX(pFields, entity_field_alias[FIELD_ORIGIN2].field);
	}
	else if (t->aiment != f->aiment)
	{
		DELTA_SETBYINDEX(pFields, entity_field_alias[FIELD_ORIGIN0].field);
		DELTA_SETBYINDEX(pFields, entity_field_alias[FIELD_ORIGIN1].field);
		DELTA_SETBYINDEX(pFields, entity_field_alias[FIELD_ORIGIN2].field);
	}
}


#define CUSTOMFIELD_ORIGIN0		0
#define CUSTOMFIELD_ORIGIN1		1
#define CUSTOMFIELD_ORIGIN2		2
#define CUSTOMFIELD_ANGLES0		3
#define CUSTOMFIELD_ANGLES1		4
#define CUSTOMFIELD_ANGLES2		5
#define CUSTOMFIELD_SKIN		6
#define CUSTOMFIELD_SEQUENCE	7
#define CUSTOMFIELD_ANIMTIME	8

entity_field_alias_t custom_entity_field_alias[] = 
{
	{ "origin[0]", 0 },
	{ "origin[1]", 0 },
	{ "origin[2]", 0 },
	{ "angles[0]", 0 },
	{ "angles[1]", 0 },
	{ "angles[2]", 0 },
	{ "skin", 0 },
	{ "sequence", 0 },
	{ "animtime", 0 },
};


void Custom_Entity_FieldInit(struct delta_s *pFields)
{
	custom_entity_field_alias[CUSTOMFIELD_ORIGIN0].field = DELTA_FINDFIELD(pFields, custom_entity_field_alias[CUSTOMFIELD_ORIGIN0].name);
	custom_entity_field_alias[CUSTOMFIELD_ORIGIN1].field = DELTA_FINDFIELD(pFields, custom_entity_field_alias[CUSTOMFIELD_ORIGIN1].name);
	custom_entity_field_alias[CUSTOMFIELD_ORIGIN2].field = DELTA_FINDFIELD(pFields, custom_entity_field_alias[CUSTOMFIELD_ORIGIN2].name);
	custom_entity_field_alias[CUSTOMFIELD_ANGLES0].field = DELTA_FINDFIELD(pFields, custom_entity_field_alias[CUSTOMFIELD_ANGLES0].name);
	custom_entity_field_alias[CUSTOMFIELD_ANGLES1].field = DELTA_FINDFIELD(pFields, custom_entity_field_alias[CUSTOMFIELD_ANGLES1].name);
	custom_entity_field_alias[CUSTOMFIELD_ANGLES2].field = DELTA_FINDFIELD(pFields, custom_entity_field_alias[CUSTOMFIELD_ANGLES2].name);
	custom_entity_field_alias[CUSTOMFIELD_SKIN].field = DELTA_FINDFIELD(pFields, custom_entity_field_alias[CUSTOMFIELD_SKIN].name);
	custom_entity_field_alias[CUSTOMFIELD_SEQUENCE].field = DELTA_FINDFIELD(pFields, custom_entity_field_alias[CUSTOMFIELD_SEQUENCE].name);
	custom_entity_field_alias[CUSTOMFIELD_ANIMTIME].field = DELTA_FINDFIELD(pFields, custom_entity_field_alias[CUSTOMFIELD_ANIMTIME].name);
}

void Custom_Encode(struct delta_s *pFields, const unsigned char *from, const unsigned char *to)
{
	static bool initialized = false;

	if (!initialized)
	{
		Custom_Entity_FieldInit(pFields);
		initialized = true;
	}

	entity_state_t *f = (entity_state_t *)from;
	entity_state_t *t = (entity_state_t *)to;

	int beamType = t->rendermode & 0xF;

	if (beamType != BEAM_POINTS && beamType != BEAM_ENTPOINT)
	{
		DELTA_UNSETBYINDEX(pFields, custom_entity_field_alias[CUSTOMFIELD_ORIGIN0].field);
		DELTA_UNSETBYINDEX(pFields, custom_entity_field_alias[CUSTOMFIELD_ORIGIN1].field);
		DELTA_UNSETBYINDEX(pFields, custom_entity_field_alias[CUSTOMFIELD_ORIGIN2].field);
	}

	if (beamType != BEAM_POINTS)
	{
		DELTA_UNSETBYINDEX(pFields, custom_entity_field_alias[CUSTOMFIELD_ANGLES0].field);
		DELTA_UNSETBYINDEX(pFields, custom_entity_field_alias[CUSTOMFIELD_ANGLES1].field);
		DELTA_UNSETBYINDEX(pFields, custom_entity_field_alias[CUSTOMFIELD_ANGLES2].field);
	}

	if (beamType != BEAM_ENTS && beamType != BEAM_ENTPOINT)
	{
		DELTA_UNSETBYINDEX(pFields, custom_entity_field_alias[CUSTOMFIELD_SKIN].field);
		DELTA_UNSETBYINDEX(pFields, custom_entity_field_alias[CUSTOMFIELD_SEQUENCE].field);
	}

	if ((int)f->animtime == (int)t->animtime)
	{
		DELTA_UNSETBYINDEX(pFields, custom_entity_field_alias[CUSTOMFIELD_ANIMTIME].field);
	}
}

void RegisterEncoders(void)
{
	DELTA_ADDENCODER("Entity_Encode", Entity_Encode);
	DELTA_ADDENCODER("Custom_Encode", Custom_Encode);
	DELTA_ADDENCODER("Player_Encode", Player_Encode);
}

BOOL GetWeaponData(edict_t *player, weapon_data_t *info)
{
	return FALSE;
}

//----------------------------------------------------------------
// Purpose : Data sent to current client only, engine sets cd to 0 before calling.
//----------------------------------------------------------------
void UpdateClientData(const edict_t *ent, int sendweapons, clientdata_t *cd)
{
	CBasePlayer *pPlayer = (CBasePlayer *)CBasePlayer::Instance(ent);

	if (pPlayer->m_hSpectatorTarget && pPlayer->pev->iuser1 == OBS_IN_EYE)
	{
		CBasePlayer *pTarget = (CBasePlayer *)CBaseEntity::Instance(pPlayer->m_hSpectatorTarget.Get());

		cd->origin = pTarget->pev->origin;
		cd->velocity = pTarget->pev->velocity;
		cd->viewmodel = pTarget->pev->viewmodel;
		cd->punchangle = pTarget->pev->punchangle;
		cd->flags = pTarget->pev->flags;
		cd->waterlevel = pTarget->pev->waterlevel;
		cd->watertype = pTarget->pev->watertype;
		cd->view_ofs = pTarget->pev->view_ofs;
		cd->health = 1;
		cd->bInDuck = pTarget->pev->bInDuck;
		cd->flTimeStepSound = pTarget->pev->flTimeStepSound;
		cd->flDuckTime = pTarget->pev->flDuckTime;
		cd->flSwimTime = pTarget->pev->flSwimTime;
		cd->waterjumptime = pTarget->pev->teleport_time;
		cd->maxspeed = pTarget->pev->maxspeed;
		cd->fov = pTarget->pev->fov;
		cd->pushmsec = pTarget->pev->pushmsec;
		cd->deadflag = pTarget->pev->deadflag;
		cd->iuser1 = pPlayer->pev->iuser1;
		cd->iuser2 = pPlayer->pev->iuser2;
		cd->iuser3 = pPlayer->pev->iuser3;
		cd->vuser1[0] = pTarget->m_iTeam;
		cd->vuser1[1] = pTarget->GetCharacterId();
		cd->vuser1[2] = pTarget->GetActiveWeaponId();
	}
	else
	{
		cd->origin = pPlayer->pev->origin;
		cd->velocity = pPlayer->pev->velocity;
		cd->viewmodel = pPlayer->pev->viewmodel;
		cd->punchangle = pPlayer->pev->punchangle;
		cd->flags = pPlayer->pev->flags;
		cd->waterlevel = pPlayer->pev->waterlevel;
		cd->watertype = pPlayer->pev->watertype;
		cd->view_ofs = pPlayer->pev->view_ofs;
		cd->health = 1;
		cd->bInDuck = pPlayer->pev->bInDuck;
		cd->flTimeStepSound = pPlayer->pev->flTimeStepSound;
		cd->flDuckTime = pPlayer->pev->flDuckTime;
		cd->flSwimTime = pPlayer->pev->flSwimTime;
		cd->waterjumptime = pPlayer->pev->teleport_time;
		cd->maxspeed = pPlayer->pev->maxspeed;
		cd->fov = pPlayer->pev->fov;
		cd->pushmsec = pPlayer->pev->pushmsec;
		cd->deadflag = pPlayer->pev->deadflag;
		cd->iuser1 = pPlayer->pev->iuser1;
		cd->iuser2 = pPlayer->pev->iuser2;
		cd->iuser3 = pPlayer->pev->iuser3;
		cd->vuser1[0] = pPlayer->m_iTeam;
		cd->vuser1[1] = pPlayer->GetCharacterId();;
		cd->vuser1[2] = pPlayer->GetActiveWeaponId();
	}
}

void CmdStart(const edict_t *player, const usercmd_t *cmd, unsigned int random_seed)
{
	CBasePlayer *pPlayer = (CBasePlayer *)GET_PRIVATE((edict_t *)player);

	if (!pPlayer)
		return;

	if (pPlayer->pev->groupinfo != 0)
	{
		UTIL_SetGroupTrace(pPlayer->pev->groupinfo, GROUP_OP_AND);
	}

	pPlayer->random_seed = random_seed;
}

void CmdEnd(const edict_t *player)
{
	CBasePlayer *pPlayer = (CBasePlayer *)GET_PRIVATE((edict_t *)player);
	
	if (!pPlayer)
		return;

	if (pPlayer->pev->groupinfo)
	{
		UTIL_UnsetGroupTrace();
	}
}

BOOL ConnectionlessPacket(const struct netadr_s *net_from, const char *args, char *response_buffer, int *response_buffer_size)
{
	*response_buffer_size = 0;
	return FALSE;
}

BOOL GetHullBounds(int hullnumber, float *mins, float *maxs)
{
	switch (hullnumber)
	{
		case 0:
		{
			// Normal player
			VectorCopy(VEC_IDLE_HULL_MIN, mins);
			VectorCopy(VEC_IDLE_HULL_MAX, maxs);
			return TRUE;
		}
		case 1:
		{
			// Crouched player
			VectorCopy(VEC_DUCK_HULL_MIN, mins);
			VectorCopy(VEC_DUCK_HULL_MAX, maxs);
			return TRUE;
		}
		case 2:
		{
			// Point based hull
			VectorCopy(VEC_POIN_HULL_MIN, mins);
			VectorCopy(VEC_POIN_HULL_MAX, maxs);
			return TRUE;
		}
	}
	
	return FALSE;
}

void CreateInstancedBaselines(void)
{
}

BOOL InconsistentFile(const edict_t *player, const char *filename, char *disconnect_message)
{
	return FALSE;
}

BOOL AllowLagCompensation(void)
{
	return TRUE;
}