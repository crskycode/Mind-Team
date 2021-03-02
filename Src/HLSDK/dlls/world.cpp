/***
 * 
 * Copyright(c) 1996-2002, Valve LLC. All rights reserved.
 * 
 * This product contains software technology licensed from Id
 * Software, Inc.("Id Technology"). Id Technology(c) 1996 Id Software, Inc.
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
#include "player.h"
#include "weapons.h"
#include "gamerule.h"
//#define OLD_BOT
#if defined OLD_BOT
#include "cs_bot.h"
#endif
#if defined CSS_BOT
#include "nav_int.h"
#include "source.h"
#include "bot.h"
#endif

edict_t *g_pBodyQueueHead;

// CONFIGS SYSTEM
void CharacterData_Init(void);
void WeaponData_Init(void);
void RadioData_Init(void);
void MsgAudio_Init(void);

void W_Precache(void);

#define SF_DECAL_NOTINDEATHMATCH	2048

class CDecal : public CBaseEntity
{
public:
	virtual void Spawn(void);
	virtual void KeyValue(KeyValueData *pkvd);

	void StaticDecal(void);
	void TriggerDecal(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);
};

LINK_ENTITY_TO_CLASS(infodecal, CDecal);


void CDecal::Spawn(void)
{
	if (pev->skin < 0 ||(gpGlobals->deathmatch && FBitSet(pev->spawnflags, SF_DECAL_NOTINDEATHMATCH)))
	{
		REMOVE_ENTITY(ENT(pev));
		return;
	}

	if (FStringNull(pev->targetname))
	{
		SetThink(&CDecal::StaticDecal);
		pev->nextthink = gpGlobals->time;
	}
	else
	{
		SetThink(&CBaseEntity::SUB_DoNothing);
		SetUse(&CDecal::TriggerDecal);
	}
}

void CDecal::TriggerDecal(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
	TraceResult		trace;
	int				entityIndex;

	UTIL_TraceLine(pev->origin - Vector(5,5,5), pev->origin + Vector(5,5,5),  ignore_monsters, ENT(pev), &trace);

	MESSAGE_BEGIN(MSG_BROADCAST, SVC_TEMPENTITY);
		WRITE_BYTE(TE_BSPDECAL);
		WRITE_COORD(pev->origin.x);
		WRITE_COORD(pev->origin.y);
		WRITE_COORD(pev->origin.z);
		WRITE_SHORT((int)pev->skin);
		entityIndex =(short)ENTINDEX(trace.pHit);
		WRITE_SHORT(entityIndex);
		if (entityIndex)
			WRITE_SHORT((int)VARS(trace.pHit)->modelindex);
	MESSAGE_END();

	SetThink(&CBaseEntity::SUB_Remove);
	pev->nextthink = gpGlobals->time + 0.1;
}


void CDecal::StaticDecal(void)
{
	TraceResult		trace;
	int				entityIndex, modelIndex;

	UTIL_TraceLine(pev->origin - Vector(5,5,5), pev->origin + Vector(5,5,5),  ignore_monsters, ENT(pev), &trace);

	entityIndex =(short)ENTINDEX(trace.pHit);

	if (entityIndex)
		modelIndex =(int)VARS(trace.pHit)->modelindex;
	else
		modelIndex = 0;

	g_engfuncs.pfnStaticDecal(pev->origin,(int)pev->skin, entityIndex, modelIndex);

	SUB_Remove();
}

void CDecal::KeyValue(KeyValueData *pkvd)
{
	if (!strcmp(pkvd->szKeyName, "texture"))
	{
		pev->skin = DECAL_INDEX(pkvd->szValue);
		
		// Found
		if (pev->skin >= 0)
			return;
		ALERT(at_console, "Can't find decal %s\n", pkvd->szValue);
	}
	else
		CBaseEntity::KeyValue(pkvd);
}


class CCorpse : public CBaseEntity
{
public:
	virtual int ObjectCaps(void)
	{
		return FCAP_DONT_SAVE;
	}
};

LINK_ENTITY_TO_CLASS(bodyque, CCorpse);

static void InitBodyQue(void)
{
	string_t istrClassname = MAKE_STRING("bodyque");

	g_pBodyQueueHead = CREATE_NAMED_ENTITY(istrClassname);
	entvars_t *pev = VARS(g_pBodyQueueHead);

	// Reserve 3 more slots for dead bodies
	for (int i = 0; i < 3; i++)
	{
		pev->owner = CREATE_NAMED_ENTITY(istrClassname);
		pev = VARS(pev->owner);
	}

	pev->owner = g_pBodyQueueHead;
}

void CopyToBodyQue(entvars_t *pev)
{
	if (pev->effects & EF_NODRAW)
		return;

	entvars_t *pevHead	= VARS(g_pBodyQueueHead);

	pevHead->angles		= pev->angles;
	pevHead->model		= pev->model;
	pevHead->modelindex	= pev->modelindex;
	pevHead->frame		= pev->frame;
	pevHead->colormap	= pev->colormap;
	pevHead->movetype	= MOVETYPE_TOSS;
	pevHead->velocity	= pev->velocity;
	pevHead->flags		= 0;
	pevHead->deadflag	= pev->deadflag;
	pevHead->renderfx	= kRenderFxDeadPlayer;
	pevHead->renderamt	= ENTINDEX(ENT(pev));

	pevHead->effects	= pev->effects | EF_NOINTERP;

	pevHead->sequence = pev->sequence;
	pevHead->animtime = pev->animtime;

	UTIL_SetOrigin(pevHead, pev->origin);
	UTIL_SetSize(pevHead, pev->mins, pev->maxs);
	g_pBodyQueueHead = pevHead->owner;
}

void SaveGlobalState(SAVERESTOREDATA *pSaveData)
{
}


void RestoreGlobalState(SAVERESTOREDATA *pSaveData)
{
}

void ResetGlobalState(void)
{
}

LINK_ENTITY_TO_CLASS(worldspawn, CWorld);

#define SF_WORLD_DARK		0x0001		// Fade from black at startup
#define SF_WORLD_TITLE		0x0002		// Display game title at startup
#define SF_WORLD_FORCETEAM	0x0004		// Force teams

void CWorld::Spawn(void)
{
	Precache();
}

void CWorld::Precache(void)
{
	CVAR_SET_STRING("sv_gravity", "800");	// 67ft/sec
	CVAR_SET_STRING("sv_stepsize", "18");

	CVAR_SET_STRING("room_type", "0");	// clear DSP

	// Set up game rules
	if (g_pGameRule)
	{
		delete g_pGameRule;
	}

	g_pGameRule = InstallGameRules();

#if defined OLD_BOT
	// Install the bot
	InstallBotControl();
#endif

#if defined CSS_BOT
	NAV_Install();
	InstallBotControl();
#endif

	InitBodyQue();

	CharacterData_Init();
	WeaponData_Init();
	RadioData_Init();
	MsgAudio_Init();

	W_Precache();

	PRECACHE_MODEL("Models/Character/Player.mdl");

	PRECACHE_SOUND("common/null.wav");				// clears sound channels

	PRECACHE_MODEL("sprites/smoke.spr");

	PRECACHE_MODEL("gfx/obj.mdl");
	PRECACHE_MODEL("gfx/new_omoh_body_gr.mdl");

//--------------------------------------------------------------
// FOOTSTEPS
//--------------------------------------------------------------

	PRECACHE_SOUND("player/c_run_drop_brokenglass.wav");
	PRECACHE_SOUND("player/c_run_drop_carpet.wav");
	PRECACHE_SOUND("player/c_run_drop_cushion.wav");
	PRECACHE_SOUND("player/c_run_drop_dirt.wav");
	PRECACHE_SOUND("player/c_run_drop_metal.wav");
	PRECACHE_SOUND("player/c_run_drop_snow.wav");
	PRECACHE_SOUND("player/c_run_drop_stone.wav");
	PRECACHE_SOUND("player/c_run_drop_swamp.wav");
	PRECACHE_SOUND("player/c_run_drop_tree.wav");
	PRECACHE_SOUND("player/c_run_drop_water.wav");
	PRECACHE_SOUND("player/c_run_jump_brokenglass.wav");
	PRECACHE_SOUND("player/c_run_jump_carpet.wav");
	PRECACHE_SOUND("player/c_run_jump_cushion.wav");
	PRECACHE_SOUND("player/c_run_jump_dirt.wav");
	PRECACHE_SOUND("player/c_run_jump_metal.wav");
	PRECACHE_SOUND("player/c_run_jump_snow.wav");
	PRECACHE_SOUND("player/c_run_jump_stone.wav");
	PRECACHE_SOUND("player/c_run_jump_swamp.wav");
	PRECACHE_SOUND("player/c_run_jump_tree.wav");
	PRECACHE_SOUND("player/c_run_jump_water.wav");
	PRECACHE_SOUND("player/c_run_left_brokenglass1.wav");
	PRECACHE_SOUND("player/c_run_left_brokenglass2.wav");
	PRECACHE_SOUND("player/c_run_left_brokenglass3.wav");
	PRECACHE_SOUND("player/c_run_left_brokenglass4.wav");
	PRECACHE_SOUND("player/c_run_left_brokenglass5.wav");
	PRECACHE_SOUND("player/c_run_left_brokenglass6.wav");
	PRECACHE_SOUND("player/c_run_left_carpet1.wav");
	PRECACHE_SOUND("player/c_run_left_carpet2.wav");
	PRECACHE_SOUND("player/c_run_left_carpet3.wav");
	PRECACHE_SOUND("player/c_run_left_carpet4.wav");
	PRECACHE_SOUND("player/c_run_left_carpet5.wav");
	PRECACHE_SOUND("player/c_run_left_carpet6.wav");
	PRECACHE_SOUND("player/c_run_left_cushion1.wav");
	PRECACHE_SOUND("player/c_run_left_cushion2.wav");
	PRECACHE_SOUND("player/c_run_left_cushion3.wav");
	PRECACHE_SOUND("player/c_run_left_cushion4.wav");
	PRECACHE_SOUND("player/c_run_left_cushion5.wav");
	PRECACHE_SOUND("player/c_run_left_cushion6.wav");
	PRECACHE_SOUND("player/c_run_left_dirt1.wav");
	PRECACHE_SOUND("player/c_run_left_dirt2.wav");
	PRECACHE_SOUND("player/c_run_left_dirt3.wav");
	PRECACHE_SOUND("player/c_run_left_dirt4.wav");
	PRECACHE_SOUND("player/c_run_left_dirt5.wav");
	PRECACHE_SOUND("player/c_run_left_dirt6.wav");
	PRECACHE_SOUND("player/c_run_left_metal1.wav");
	PRECACHE_SOUND("player/c_run_left_metal2.wav");
	PRECACHE_SOUND("player/c_run_left_metal3.wav");
	PRECACHE_SOUND("player/c_run_left_metal4.wav");
	PRECACHE_SOUND("player/c_run_left_metal5.wav");
	PRECACHE_SOUND("player/c_run_left_metal6.wav");
	PRECACHE_SOUND("player/c_run_left_snow1.wav");
	PRECACHE_SOUND("player/c_run_left_snow2.wav");
	PRECACHE_SOUND("player/c_run_left_snow3.wav");
	PRECACHE_SOUND("player/c_run_left_snow4.wav");
	PRECACHE_SOUND("player/c_run_left_snow5.wav");
	PRECACHE_SOUND("player/c_run_left_snow6.wav");
	PRECACHE_SOUND("player/c_run_left_stone1.wav");
	PRECACHE_SOUND("player/c_run_left_stone2.wav");
	PRECACHE_SOUND("player/c_run_left_stone3.wav");
	PRECACHE_SOUND("player/c_run_left_stone4.wav");
	PRECACHE_SOUND("player/c_run_left_stone5.wav");
	PRECACHE_SOUND("player/c_run_left_stone6.wav");
	PRECACHE_SOUND("player/c_run_left_tree1.wav");
	PRECACHE_SOUND("player/c_run_left_tree2.wav");
	PRECACHE_SOUND("player/c_run_left_tree3.wav");
	PRECACHE_SOUND("player/c_run_left_tree4.wav");
	PRECACHE_SOUND("player/c_run_left_tree5.wav");
	PRECACHE_SOUND("player/c_run_left_tree6.wav");
	PRECACHE_SOUND("player/c_run_left_water1.wav");
	PRECACHE_SOUND("player/c_run_left_water2.wav");
	PRECACHE_SOUND("player/c_run_left_water3.wav");
	PRECACHE_SOUND("player/c_run_left_water4.wav");
	PRECACHE_SOUND("player/c_run_left_water5.wav");
	PRECACHE_SOUND("player/c_run_left_water6.wav");
	PRECACHE_SOUND("player/c_run_right_brokenglass1.wav");
	PRECACHE_SOUND("player/c_run_right_brokenglass2.wav");
	PRECACHE_SOUND("player/c_run_right_brokenglass3.wav");
	PRECACHE_SOUND("player/c_run_right_brokenglass4.wav");
	PRECACHE_SOUND("player/c_run_right_brokenglass5.wav");
	PRECACHE_SOUND("player/c_run_right_brokenglass6.wav");
	PRECACHE_SOUND("player/c_run_right_carpet1.wav");
	PRECACHE_SOUND("player/c_run_right_carpet2.wav");
	PRECACHE_SOUND("player/c_run_right_carpet3.wav");
	PRECACHE_SOUND("player/c_run_right_carpet4.wav");
	PRECACHE_SOUND("player/c_run_right_carpet5.wav");
	PRECACHE_SOUND("player/c_run_right_carpet6.wav");
	PRECACHE_SOUND("player/c_run_right_cushion1.wav");
	PRECACHE_SOUND("player/c_run_right_cushion2.wav");
	PRECACHE_SOUND("player/c_run_right_cushion3.wav");
	PRECACHE_SOUND("player/c_run_right_cushion4.wav");
	PRECACHE_SOUND("player/c_run_right_cushion5.wav");
	PRECACHE_SOUND("player/c_run_right_cushion6.wav");
	PRECACHE_SOUND("player/c_run_right_dirt1.wav");
	PRECACHE_SOUND("player/c_run_right_dirt2.wav");
	PRECACHE_SOUND("player/c_run_right_dirt3.wav");
	PRECACHE_SOUND("player/c_run_right_dirt4.wav");
	PRECACHE_SOUND("player/c_run_right_dirt5.wav");
	PRECACHE_SOUND("player/c_run_right_dirt6.wav");
	PRECACHE_SOUND("player/c_run_right_metal1.wav");
	PRECACHE_SOUND("player/c_run_right_metal2.wav");
	PRECACHE_SOUND("player/c_run_right_metal3.wav");
	PRECACHE_SOUND("player/c_run_right_metal4.wav");
	PRECACHE_SOUND("player/c_run_right_metal5.wav");
	PRECACHE_SOUND("player/c_run_right_metal6.wav");
	PRECACHE_SOUND("player/c_run_right_snow1.wav");
	PRECACHE_SOUND("player/c_run_right_snow2.wav");
	PRECACHE_SOUND("player/c_run_right_snow3.wav");
	PRECACHE_SOUND("player/c_run_right_snow4.wav");
	PRECACHE_SOUND("player/c_run_right_snow5.wav");
	PRECACHE_SOUND("player/c_run_right_snow6.wav");
	PRECACHE_SOUND("player/c_run_right_stone1.wav");
	PRECACHE_SOUND("player/c_run_right_stone2.wav");
	PRECACHE_SOUND("player/c_run_right_stone3.wav");
	PRECACHE_SOUND("player/c_run_right_stone4.wav");
	PRECACHE_SOUND("player/c_run_right_stone5.wav");
	PRECACHE_SOUND("player/c_run_right_stone6.wav");
	PRECACHE_SOUND("player/c_run_right_tree1.wav");
	PRECACHE_SOUND("player/c_run_right_tree2.wav");
	PRECACHE_SOUND("player/c_run_right_tree3.wav");
	PRECACHE_SOUND("player/c_run_right_tree4.wav");
	PRECACHE_SOUND("player/c_run_right_tree5.wav");
	PRECACHE_SOUND("player/c_run_right_tree6.wav");
	PRECACHE_SOUND("player/c_run_right_water1.wav");
	PRECACHE_SOUND("player/c_run_right_water2.wav");
	PRECACHE_SOUND("player/c_run_right_water3.wav");
	PRECACHE_SOUND("player/c_run_right_water4.wav");
	PRECACHE_SOUND("player/c_run_right_water5.wav");
	PRECACHE_SOUND("player/c_run_right_water6.wav");

//--------------------------------------------------------------
// WEAPON
//--------------------------------------------------------------

	PRECACHE_SOUND("weapon/others/g_missbullet.wav");
	PRECACHE_SOUND("weapon/others/g_nobullet_machinegun.wav");
	PRECACHE_SOUND("weapon/others/g_nobullet_pistol.wav");
	PRECACHE_SOUND("weapon/others/g_nobullet_rifle.wav");
	PRECACHE_SOUND("weapon/others/g_nobullet_shotgun.wav");
	PRECACHE_SOUND("weapon/others/g_zoom.wav");

//--------------------------------------------------------------
// COMMON
//--------------------------------------------------------------

	PRECACHE_SOUND("common/ui_notify.wav");

	PRECACHE_MODEL("models/item/item_supplybox.mdl");

	// 0 normal
	LIGHT_STYLE(0, "m");
	
	// 1 FLICKER(first variety)
	LIGHT_STYLE(1, "mmnmmommommnonmmonqnmmo");
	
	// 2 SLOW STRONG PULSE
	LIGHT_STYLE(2, "abcdefghijklmnopqrstuvwxyzyxwvutsrqponmlkjihgfedcba");
	
	// 3 CANDLE(first variety)
	LIGHT_STYLE(3, "mmmmmaaaaammmmmaaaaaabcdefgabcdefg");
	
	// 4 FAST STROBE
	LIGHT_STYLE(4, "mamamamamama");
	
	// 5 GENTLE PULSE 1
	LIGHT_STYLE(5,"jklmnopqrstuvwxyzyxwvutsrqponmlkj");
	
	// 6 FLICKER(second variety)
	LIGHT_STYLE(6, "nmonqnmomnmomomno");
	
	// 7 CANDLE(second variety)
	LIGHT_STYLE(7, "mmmaaaabcdefgmmmmaaaammmaamm");
	
	// 8 CANDLE(third variety)
	LIGHT_STYLE(8, "mmmaaammmaaammmabcdefaaaammmmabcdefmmmaaaa");
	
	// 9 SLOW STROBE(fourth variety)
	LIGHT_STYLE(9, "aaaaaaaazzzzzzzz");
	
	// 10 FLUORESCENT FLICKER
	LIGHT_STYLE(10, "mmamammmmammamamaaamammma");

	// 11 SLOW PULSE NOT FADE TO BLACK
	LIGHT_STYLE(11, "abcdefghijklmnopqrrqponmlkjihgfedcba");
	
	// 12 UNDERWATER LIGHT MUTATION
	// this light only distorts the lightmap - no contribution
	// is made to the brightness of affected surfaces
	LIGHT_STYLE(12, "mmnnmmnnnmmnn");
	
	// styles 32-62 are assigned by the light program for switchable lights

	// 63 testing
	LIGHT_STYLE(63, "a");

	if (pev->speed > 0)
		CVAR_SET_FLOAT("sv_zmax", pev->speed);
	else
		CVAR_SET_FLOAT("sv_zmax", 4096);

	if (pev->spawnflags & SF_WORLD_DARK)
		CVAR_SET_FLOAT("v_dark", 1.0);
	else
		CVAR_SET_FLOAT("v_dark", 0.0);
}

void CWorld::KeyValue(KeyValueData *pkvd)
{
	if (!strcmp(pkvd->szKeyName, "skyname"))
	{
		// Sent over net now.
		CVAR_SET_STRING("sv_skyname", pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (!strcmp(pkvd->szKeyName, "sounds"))
	{
		gpGlobals->cdAudioTrack = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (!strcmp(pkvd->szKeyName, "WaveHeight"))
	{
		// Sent over net now.
		pev->scale = atof(pkvd->szValue) *(1.0/8.0);
		pkvd->fHandled = TRUE;
		CVAR_SET_FLOAT("sv_wateramp", pev->scale);
	}
	else if (!strcmp(pkvd->szKeyName, "MaxRange"))
	{
		pev->speed = atof(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (!strcmp(pkvd->szKeyName, "chaptertitle"))
	{
		pev->netname = ALLOC_STRING(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (!strcmp(pkvd->szKeyName, "startdark"))
	{
		// UNDONE: This is a gross hack!!! The CVAR is NOT sent over the client/sever link
		// but it will work for single player
		int flag = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
		if (flag)
			pev->spawnflags |= SF_WORLD_DARK;
	}
	else if (!strcmp(pkvd->szKeyName, "newunit"))
	{
		// Single player only.  Clear save directory if set
		if (atoi(pkvd->szValue))
			CVAR_SET_FLOAT("sv_newunit", 1);
		pkvd->fHandled = TRUE;
	}
	else if (!strcmp(pkvd->szKeyName, "gametitle"))
	{
		if (atoi(pkvd->szValue))
			pev->spawnflags |= SF_WORLD_TITLE;

		pkvd->fHandled = TRUE;
	}
	else if (!strcmp(pkvd->szKeyName, "mapteams"))
	{
		pev->team = ALLOC_STRING(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (!strcmp(pkvd->szKeyName, "defaultteam"))
	{
		if (atoi(pkvd->szValue))
		{
			pev->spawnflags |= SF_WORLD_FORCETEAM;
		}
		pkvd->fHandled = TRUE;
	}
	else
		CBaseEntity::KeyValue(pkvd);
}