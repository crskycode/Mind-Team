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
#include "engine_hook.h"

//#define OLD_BOT
#if defined OLD_BOT
#include "bot.h"
#endif
#if defined CSS_BOT
#include "convar.h"
#endif


void (*pfnDebugLine)(float *start, float *end, int r, int g, int b, float life) = NULL;


// Register your console variables here
// This gets called one time when the game is initialied
void GameDLLInit(void)
{
#if defined OLD_BOT
	Bot_RegisterCvars();
#endif

#if defined CSS_BOT
	ConVar_Register();
#endif

	HMODULE hModule = GetModuleHandle("MT.dll");
	if (hModule)
	{
		*(FARPROC *)&pfnDebugLine = GetProcAddress(hModule, "DebugLine");
	}
}

void GameShutdown(void)
{
	EngineHook_Shutdown();
}