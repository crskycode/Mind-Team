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

#ifndef CL_UTIL_H
#define CL_UTIL_H

// Macros to hook function calls into the HUD object
#define DECLARE_MESSAGE(x, y) int __MsgFunc_##y(const char *pszName, int iSize, void *pbuf) { return gHUD.##x.MsgFunc_##y(pszName, iSize, pbuf); }
#define HOOK_MESSAGE(name) gEngfuncs.pfnHookUserMsg(#name, __MsgFunc_##name);

#define DECLARE_COMMAND(x, y) void __CmdFunc_##y(void) { gHUD.##x.UserCmd_##y(); }
#define HOOK_COMMAND(x, y) gEngfuncs.pfnAddCommand(x, __CmdFunc_##y);

// CVAR
#define CVAR_CREATE				( *gEngfuncs.pfnRegisterVariable )
#define CVAR_GET				( *gEngfuncs.pfnGetCvarPointer )
#define CVAR_GET_FLOAT			( *gEngfuncs.pfnGetCvarFloat )
#define CVAR_GET_STRING			( *gEngfuncs.pfnGetCvarString )

#define FillRGBA				( *gEngfuncs.pfnFillRGBA )

#define GetScreenInfo		( *gEngfuncs.pfnGetScreenInfo )
#define ServerCmd			( *gEngfuncs.pfnServerCmd )
#define ClientCmd			( *gEngfuncs.pfnClientCmd )
#define SetCrosshair		( *gEngfuncs.pfnSetCrosshair )
#define AngleVectors		( *gEngfuncs. pfnAngleVectors )

#define GetPlayerInfo		( *gEngfuncs.pfnGetPlayerInfo )

#ifdef PlaySound
#undef PlaySound
#endif

inline void HUD_PlaySound(char *szSound, float vol)
{
	//gEngfuncs.pfnPlaySoundByName(szSound, vol);
	static char cmd[256];
	_snprintf(cmd, sizeof(cmd), "spk \"%s\"\n", szSound);
	gEngfuncs.pfnClientCmd(cmd);
}

extern "C"
{
extern vec3_t vec3_origin;
}

#endif