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
#include "MetaHook_Interface.h"
#include "sys.h"
#include "qgl.h"
#include "hud.h"

#include "ClientState.h"
#include "GameState.h"

extern "C"
{
#include <pm_shared.h>
}

cl_enginefunc_t gEngfuncs;

IMTEngine *gpMTEngine;

IFileSystem *g_pFileSystem;

font_api_t gFontAPI;
texture_api_t gTexAPI;


extern "C" 
{
	DLLExport BOOL Initialize(cl_enginefunc_t *pEnginefuncs, int iVersion);
	DLLExport BOOL HUD_VidInit(void);
	DLLExport void HUD_Init(void);
	DLLExport void HUD_Shutdown(void);
	DLLExport BOOL HUD_Redraw(float flTime, int intermission);
	DLLExport void HUD_Reset(void);
	DLLExport void HUD_PlayerMove(struct playermove_s *ppmove, int server);
	DLLExport void HUD_PlayerMoveInit(struct playermove_s *ppmove);
	DLLExport char HUD_PlayerMoveTexture(char *name);
	DLLExport BOOL HUD_ConnectionlessPacket(const struct netadr_s *net_from, const char *args, char *response_buffer, int *response_buffer_size);
	DLLExport BOOL HUD_GetHullBounds(int hullnumber, float *mins, float *maxs);
	DLLExport void HUD_Frame(double time);
	DLLExport void HUD_VoiceStatus(int entindex, qboolean bTalking);
	DLLExport void HUD_DirectorMessage(int iSize, void *pbuf);
}

// INPUT SYSTEM
void InitInput(void);
void ShutdownInput(void);

// MESSAGE SYSTEM
void MSG_HookUserMsg(void);
void EV_HookEvents(void);

// INTERFACE
void StudioModelSkin_Init(void);
void ScreenShot_Init(void);

// CONFIGS SYSTEM
void CharacterData_Init(void);
void WeaponData_Init(void);
void SoundData_Init(void);
void MapData_Init(void);
void RadioData_Init(void);
void MsgAudio_Init(void);
void BagData_Init(void);

// SCRIPT SYSTEM
void AnimScript_Init(void);

void _test(void)
{
}

BOOL Initialize(cl_enginefunc_t *pEnginefuncs, int iVersion)
{
	if (iVersion != CLDLL_INTERFACE_VERSION)
		return FALSE;
	
	memcpy(&gEngfuncs, pEnginefuncs, sizeof(cl_enginefunc_t));

	gEngfuncs.pfnAddCommand("cltest", _test);
	
	HINTERFACEMODULE hMT = (HINTERFACEMODULE)GetModuleHandleA("MT.dll");

	if (hMT)
	{
		CreateInterfaceFn pfnFactory = Sys_GetFactory(hMT);

		if (pfnFactory)
		{
			gpMTEngine = (IMTEngine *)pfnFactory(MTENGINE_VERSION, NULL);
		}
	}
	else
	{
		Sys_Error("Couldn't initialize MT.dll");
	}

	QGL_Init();

	MSG_HookUserMsg();
	EV_HookEvents();

	g_pFileSystem = (IFileSystem *)gpMTEngine->GetFileSystem();

	gpMTEngine->GetTexAPI(&gTexAPI);
	gpMTEngine->GetFontAPI(&gFontAPI);

	StudioModelSkin_Init();
	ScreenShot_Init();

	CharacterData_Init();
	WeaponData_Init();
	SoundData_Init();
	MapData_Init();
	RadioData_Init();
	MsgAudio_Init();
	BagData_Init();

	AnimScript_Init();

	return TRUE;
}

BOOL HUD_VidInit(void)
{
	gHUD.VidInit();
	return TRUE;
}

void HUD_Init(void)
{
	memset( &ClientState, 0, sizeof(ClientState) );
	memset( &ViewModel, 0, sizeof(ViewModel) );
	memset( &GameState, 0, sizeof(GameState) );

	InitInput();
	gHUD.Init();
}

void HUD_Shutdown(void)
{
	ShutdownInput();
	gHUD.Shutdown();
}

BOOL HUD_Redraw(float time, int intermission)
{
	gHUD.Redraw();
	return TRUE;
}

void HUD_Reset(void)
{
	gHUD.VidInit();
}

void HUD_PlayerMove(struct playermove_s *ppmove, int server)
{
	PM_Move(ppmove, server);
}

void HUD_PlayerMoveInit(struct playermove_s *ppmove)
{
	PM_Init(ppmove);
}

char HUD_PlayerMoveTexture(char *name)
{
	return PM_FindTextureType(name);
}

BOOL HUD_ConnectionlessPacket(const struct netadr_s *net_from, const char *args, char *response_buffer, int *response_buffer_size)
{
	// Parse stuff from args.
	int max_buffer_size = *response_buffer_size;
	
	// Zero it out since we aren't going to respond.
	// If we wanted to response, we'd write data into response_buffer
	*response_buffer_size = 0;
	
	// Since we don't listen for anything here, just respond that it's a bogus message.
	// If we didn't reject the message, we'd return 1 for success instead.
	return 0;
}

extern "C"
{
#include "pm_hull.h"
}

BOOL HUD_GetHullBounds(int hullnumber, float *mins, float *maxs)
{
	switch (hullnumber)
	{
		case 0:
		{
			VectorCopy(VEC_IDLE_HULL_MIN, mins);
			VectorCopy(VEC_IDLE_HULL_MAX, maxs);
			
			return TRUE;
		}
		case 1:
		{
			VectorCopy(VEC_DUCK_HULL_MIN, mins);
			VectorCopy(VEC_DUCK_HULL_MAX, maxs);
			
			return TRUE;
		}
		case 2:
		{
			VectorCopy(VEC_POIN_HULL_MIN, mins);
			VectorCopy(VEC_POIN_HULL_MAX, maxs);
			
			return TRUE;
		}
	}

	return FALSE;
}

void HUD_Frame(double time)
{
	gHUD.Frame();
}

void HUD_VoiceStatus(int entindex, qboolean bTalking)
{
}

void HUD_DirectorMessage(int iSize, void *pbuf)
{
}