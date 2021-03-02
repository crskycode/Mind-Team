
#include <metahook.h>
#include "engfuncs.h"

// helper macro
#define SearchEngineFunction( name, type, sig1, sig2 )	\
	if ( g_dwEngineBuildnum == 3266 ) \
		g_pfn_##name = (type)g_pMetaHookAPI->SearchPattern( g_dwEngineBase, g_dwEngineSize, sig1, sizeof(sig1) - 1 ); \
	else \
		g_pfn_##name = (type)g_pMetaHookAPI->SearchPattern( g_dwEngineBase, g_dwEngineSize, sig2, sizeof(sig2) - 1 ); \

#define ValidateEngineFunction( name ) \
	if ( !g_pfn_##name ) \
		Sys_Error( "Engine function '%s' not found", #name );

#define HookEngineFunction( name ) \
	g_pMetaHookAPI->InlineHook( g_pfn_##name, name, (void *&)g_pfn_##name )

#define SearchEngineVariable( sig ) \
	g_pMetaHookAPI->SearchPattern( g_dwEngineBase, g_dwEngineSize, sig, sizeof(sig) - 1 )

#define ValidateEngineVariable( name ) \
	if ( !name ) \
		Sys_Error( "Engine variable '%s' not found", #name );


// function binary code pattern
#define FN_S_STARTUP_SIG			"\xA1\x2A\x2A\x2A\x2A\x85\xC0\x74\x2A\xA1\x2A\x2A\x2A\x2A\x85\xC0\x75\x2A\xE8\x2A\x2A\x2A\x2A\x85\xC0\xC7\x05"
#define FN_S_INIT_SIG				"\x83\xEC\x08\x68\x2A\x2A\x2A\x2A\xE8\x2A\x2A\x2A\x2A\xE8\x2A\x2A\x2A\x2A\x68\x2A\x2A\x2A\x2A\xE8\x2A\x2A\x2A\x2A\x83\xC4\x08\x85\xC0"
#define FN_S_SHUTDOWN_SIG			"\xE8\x2A\x2A\x2A\x2A\xA1\x2A\x2A\x2A\x2A\x33\xC9\x3B\xC1\x74\x2A\xA1\x2A\x2A\x2A\x2A\x3B\xC1\x74\x2A\x89\x08\xA1"
#define FN_S_FINDNAME_SIG			"\x53\x55\x8B\x6C\x24\x0C\x33\xDB\x56\x57\x85\xED\x75\x2A\x68\x2A\x2A\x2A\x2A\xE8\x2A\x2A\x2A\x2A\x83\xC4\x04\x55\xE8"
#define FN_S_STARTDYNAMICSOUND_SIG	"\x83\xEC\x48\xA1\x2A\x2A\x2A\x2A\x53\x55\x56\x85\xC0\x57\xC7\x44\x24\x10\x00\x00\x00\x00\x0F\x84\x2A\x2A\x2A\x2A\x8B\x7C\x24\x64"
#define FN_S_STARTSTATICSOUND_SIG	"\x83\xEC\x44\x53\x55\x8B\x6C\x24\x58\x56\x85\xED\x57\xC7\x44\x24\x10\x00\x00\x00\x00\x0F\x84\x2A\x2A\x2A\x2A\x80\x7D\x00\x2A\x75"
#define FN_S_STOPSOUND_SIG			"\xA1\x2A\x2A\x2A\x2A\x57\xBF\x2A\x2A\x2A\x2A\x3B\xC7\x7E\x2A\x53\x8B\x5C\x24\x10\x55\x8B\x6C\x24\x10\x56\xBE\x2A\x2A\x2A\x2A\x39\x6E\xFC"
#define FN_S_STOPALLSOUNDS_SIG		"\xA1\x2A\x2A\x2A\x2A\x85\xC0\x74\x2A\x56\xC7\x05\x2A\x2A\x2A\x2A\x2A\x2A\x2A\x2A\xBE\x2A\x2A\x2A\x2A\x83\x3E\x00\x74\x2A\x56\xE8"
#define FN_S_UPDATE_SIG				"\xA1\x2A\x2A\x2A\x2A\x85\xC0\x0F\x84\x2A\x2A\x2A\x2A\xA1\x2A\x2A\x2A\x2A\x85\xC0\x0F\x8F\x2A\x2A\x2A\x2A\x53\x56\x8B\x74\x24\x0C"
#define FN_S_LOADSOUND_SIG			"\x81\xEC\x2A\x2A\x2A\x2A\x53\x8B\x9C\x24\x50\x05\x00\x00\x55\x56\x8A\x03\x57\x3C\x2A\x75\x2A\x8B\x84\x24\x60\x05\x00\x00\x50\x53"
#define FN_S_FREECHANNEL_SIG		"\x56\x8B\x74\x24\x08\x8B\x46\x1C\x83\xF8\x05\x75\x2A\x8B\xCE\xB8\x2A\x2A\x2A\x2A\x81\xE9\x2A\x2A\x2A\x2A\xF7\xE1\xC1\xEA\x04\x52"
#define FN_VOX_LOADSOUND_SIG		"\x81\xEC\x2A\x2A\x2A\x2A\x56\x8B\xB4\x24\xEC\x06\x00\x00\x85\xF6\x57\x75\x2A\x5F\x33\xC0\x5E\x81\xC4\x2A\x2A\x2A\x2A\xC3\x68"
#define FN_CACHE_CHECK_SIG			"\x57\x8B\x7C\x24\x08\x8B\x07\x85\xC0\x75\x2A\x5F\xC3\x56\x8D\x70\xA8\x56\xE8\x2A\x2A\x2A\x2A\x56\xE8\x2A\x2A\x2A\x2A\x8B\x07\x83\xC4\x08"
#define FN_CACHE_ALLOC_SIG			"\x53\x8B\x5C\x24\x08\x56\x57\x83\x3B\x00\x74\x2A\x68\x2A\x2A\x2A\x2A\xE8\x2A\x2A\x2A\x2A\x83\xC4\x04\x8B\x74\x24\x14\x85\xF6\x7F"
#define FN_CACHE_FREE_SIG			"\x56\x8B\x74\x24\x08\x83\x3E\x00\x75\x2A\x68\x2A\x2A\x2A\x2A\xE8\x2A\x2A\x2A\x2A\x83\xC4\x04\x8B\x06\xC7\x06\x2A\x2A\x2A\x2A\x83\xE8\x58"

#define FN_S_STARTUP_NEW_SIG			"\xA1\x2A\x2A\x2A\x2A\x85\xC0\x74\x2A\xA1\x2A\x2A\x2A\x2A\x85\xC0\x75\x2A\xE8\x2A\x2A\x2A\x2A\x85\xC0\x75\x2A\x68\x2A\x2A\x2A\x2A"
#define FN_S_INIT_NEW_SIG				"\x68\x2A\x2A\x2A\x2A\xE8\x2A\x2A\x2A\x2A\xE8\x2A\x2A\x2A\x2A\x68\x2A\x2A\x2A\x2A\xE8\x2A\x2A\x2A\x2A\x83\xC4\x08\x85\xC0\x0F\x85"
#define FN_S_SHUTDOWN_NEW_SIG			"\xE8\x2A\x2A\x2A\x2A\xA1\x2A\x2A\x2A\x2A\x33\xC9\x3B\xC1\x74\x2A\xA1\x2A\x2A\x2A\x2A\x3B\xC1\x74\x2A\x89\x08\xA1"
#define FN_S_FINDNAME_NEW_SIG			"\x55\x8B\xEC\x53\x56\x8B\x75\x08\x33\xDB\x85\xF6\x57\x75\x2A\x68\x2A\x2A\x2A\x2A\xE8\x2A\x2A\x2A\x2A\x83\xC4\x04\x56\xE8"
#define FN_SND_SPATIALIZE_NEW_SIG		"\x55\x8B\xEC\x83\xEC\x1C\x8B\x0D\x2A\x2A\x2A\x2A\x56\x8B\x75\x08\x8B\x46\x18\x3B\xC1\x75\x2A\x8B\x46\x30\x56\x89\x46\x04\x89\x46\x08"
#define FN_S_STARTDYNAMICSOUND_NEW_SIG	"\x55\x8B\xEC\x83\xEC\x48\xA1\x2A\x2A\x2A\x2A\x53\x56\x57\x85\xC0\xC7\x45\xFC\x00\x00\x00\x00\x0F\x84\x2A\x2A\x2A\x2A\x8B\x75\x10"
#define FN_S_STARTSTATICSOUND_NEW_SIG	"\x55\x8B\xEC\x83\xEC\x44\x53\x56\x57\x8B\x7D\x10\x85\xFF\xC7\x45\xFC\x00\x00\x00\x00\x0F\x84\x2A\x2A\x2A\x2A\x80\x3F\x2A\x75"
#define FN_S_STOPSOUND_NEW_SIG			"\x55\x8B\xEC\xA1\x2A\x2A\x2A\x2A\x57\xBF\x2A\x2A\x2A\x2A\x3B\xC7\x7E\x2A\x53\x8B\x5D\x0C\x56\xBE\x2A\x2A\x2A\x2A\x8B\x45\x08\x8B\x4E\xFC"
#define FN_S_STOPALLSOUNDS_NEW_SIG		"\x55\x8B\xEC\xA1\x2A\x2A\x2A\x2A\x85\xC0\x74\x2A\x56\xC7\x05\x2A\x2A\x2A\x2A\x2A\x2A\x2A\x2A\xBE\x2A\x2A\x2A\x2A\x83\x3E\x00\x74"
#define FN_S_UPDATE_NEW_SIG				"\x55\x8B\xEC\xA1\x2A\x2A\x2A\x2A\x85\xC0\x0F\x84\x2A\x2A\x2A\x2A\xA1\x2A\x2A\x2A\x2A\x85\xC0\x0F\x8F\x2A\x2A\x2A\x2A\x8B\x45\x08"
#define FN_S_LOADSOUND_NEW_SIG			"\x55\x8B\xEC\x81\xEC\x2A\x2A\x2A\x2A\x53\x56\x8B\x75\x08\x57\x8A\x06\x3C\x2A\x75\x2A\x8B\x45\x0C\x50\x56\xE8\x2A\x2A\x2A\x2A\x83\xC4\x08"
#define FN_S_FREECHANNEL_NEW_SIG		"\x55\x8B\xEC\x56\x8B\x75\x08\x8B\x46\x1C\x83\xF8\x05\x75\x2A\x8B\xC6\x2D\x2A\x2A\x2A\x2A\xC1\xE8\x06\x50\xE8\x2A\x2A\x2A\x2A\x8B\x0E"
#define FN_VOX_SETCHANVOL_NEW_SIG		"\x55\x8B\xEC\x51\x56\x8B\x75\x08\x8B\x46\x34\x85\xC0\x7C\x2A\x8B\x56\x38\xC1\xE0\x05\x03\xC2\x8D\x04\xC0\x8B\x04\x85"
#define FN_VOX_LOADSOUND_NEW_SIG		"\x55\x8B\xEC\x81\xEC\x2A\x2A\x2A\x2A\x53\x56\x8B\x75\x0C\x57\x85\xF6\x75\x2A\x5F\x5E\x33\xC0\x5B\x8B\xE5\x5D\xC3\x68"
#define FN_CACHE_CHECK_NEW_SIG			"\x55\x8B\xEC\x57\x8B\x7D\x08\x8B\x07\x85\xC0\x75\x2A\x5F\x5D\xC3\x56\x8D\x70\xA8\x56\xE8\x2A\x2A\x2A\x2A\x56\xE8"
#define FN_CACHE_ALLOC_NEW_SIG			"\x55\x8B\xEC\x53\x8B\x5D\x08\x56\x57\x83\x3B\x00\x74\x2A\x68\x2A\x2A\x2A\x2A\xE8\x2A\x2A\x2A\x2A\x83\xC4\x04\x8B\x75\x0C\x85\xF6"
#define FN_CACHE_FREE_NEW_SIG			"\x55\x8B\xEC\x56\x8B\x75\x08\x83\x3E\x00\x75\x2A\x68\x2A\x2A\x2A\x2A\xE8\x2A\x2A\x2A\x2A\x83\xC4\x04\x8B\x06\xC7\x06"


// function callback
PFN_S_STARTUP_T				g_pfn_S_Startup;
PFN_S_INIT_T				g_pfn_S_Init;
PFN_S_SHUTDOWN_T			g_pfn_S_Shutdown;
PFN_S_FINDNAME_T			g_pfn_S_FindName;
PFN_S_STARTDYNAMICSOUND_T	g_pfn_S_StartDynamicSound;
PFN_S_STARTSTATICSOUND_T	g_pfn_S_StartStaticSound;
PFN_S_STOPSOUND_T			g_pfn_S_StopSound;
PFN_S_STOPALLSOUNDS_T		g_pfn_S_StopAllSounds;
PFN_S_UPDATE_T				g_pfn_S_Update;
PFN_S_LOADSOUND_T			g_pfn_S_LoadSound;
PFN_S_FREECHANNEL_T			g_pfn_S_FreeChannel;
PFN_VOX_LOADSOUND_T			g_pfn_VOX_LoadSound;
PFN_CACHE_CHECK_T			g_pfn_Cache_Check;
PFN_CACHE_ALLOC_T			g_pfn_Cache_Alloc;
PFN_CACHE_FREE_T			g_pfn_Cache_Free;


// engine variable pointer
int*	g_pfn_cl_viewentity = NULL;
int*	g_pfn_cl_parsecount = NULL;
void*	g_pfn_rgrgvoxword = NULL;


void EngFuncs_Init(void)
{
	//
	// find engine functions
	//

	SearchEngineFunction(S_Startup, PFN_S_STARTUP_T, FN_S_STARTUP_SIG, FN_S_STARTUP_NEW_SIG);
	SearchEngineFunction(S_Init, PFN_S_INIT_T, FN_S_INIT_SIG, FN_S_INIT_NEW_SIG);
	SearchEngineFunction(S_Shutdown, PFN_S_SHUTDOWN_T, FN_S_SHUTDOWN_SIG, FN_S_SHUTDOWN_NEW_SIG);
	SearchEngineFunction(S_FindName, PFN_S_FINDNAME_T, FN_S_FINDNAME_SIG, FN_S_FINDNAME_NEW_SIG);
	SearchEngineFunction(S_StartDynamicSound, PFN_S_STARTDYNAMICSOUND_T, FN_S_STARTDYNAMICSOUND_SIG, FN_S_STARTDYNAMICSOUND_NEW_SIG);
	SearchEngineFunction(S_StartStaticSound, PFN_S_STARTSTATICSOUND_T, FN_S_STARTSTATICSOUND_SIG, FN_S_STARTSTATICSOUND_NEW_SIG);
	SearchEngineFunction(S_StopSound, PFN_S_STOPSOUND_T, FN_S_STOPSOUND_SIG, FN_S_STOPSOUND_NEW_SIG);
	SearchEngineFunction(S_StopAllSounds, PFN_S_STOPALLSOUNDS_T, FN_S_STOPALLSOUNDS_SIG, FN_S_STOPALLSOUNDS_NEW_SIG);
	SearchEngineFunction(S_Update, PFN_S_UPDATE_T, FN_S_UPDATE_SIG, FN_S_UPDATE_NEW_SIG);
	SearchEngineFunction(S_LoadSound, PFN_S_LOADSOUND_T, FN_S_LOADSOUND_SIG, FN_S_LOADSOUND_NEW_SIG);
	SearchEngineFunction(S_FreeChannel, PFN_S_FREECHANNEL_T, FN_S_FREECHANNEL_SIG, FN_S_FREECHANNEL_NEW_SIG);
	SearchEngineFunction(VOX_LoadSound, PFN_VOX_LOADSOUND_T, FN_VOX_LOADSOUND_SIG, FN_VOX_LOADSOUND_NEW_SIG);
	SearchEngineFunction(Cache_Check, PFN_CACHE_CHECK_T, FN_CACHE_CHECK_SIG, FN_CACHE_CHECK_NEW_SIG);
	SearchEngineFunction(Cache_Alloc, PFN_CACHE_ALLOC_T, FN_CACHE_ALLOC_SIG, FN_CACHE_ALLOC_NEW_SIG);
	SearchEngineFunction(Cache_Free, PFN_CACHE_FREE_T, FN_CACHE_FREE_SIG, FN_CACHE_FREE_NEW_SIG);

	ValidateEngineFunction(S_Startup);
	ValidateEngineFunction(S_Init);
	ValidateEngineFunction(S_Shutdown);
	ValidateEngineFunction(S_FindName);
	ValidateEngineFunction(S_StartDynamicSound);
	ValidateEngineFunction(S_StartStaticSound);
	ValidateEngineFunction(S_StopSound);
	ValidateEngineFunction(S_StopAllSounds);
	ValidateEngineFunction(S_Update);
	ValidateEngineFunction(S_LoadSound);
	ValidateEngineFunction(S_FreeChannel);
	ValidateEngineFunction(VOX_LoadSound);
	ValidateEngineFunction(Cache_Check);
	ValidateEngineFunction(Cache_Alloc);
	ValidateEngineFunction(Cache_Free);

	//
	// find engine variables
	//

	if ( g_dwEngineBuildnum != 3266 )
	{
		DWORD pfnSpatialize = (DWORD)SearchEngineVariable(FN_SND_SPATIALIZE_NEW_SIG);
		DWORD pfnSetChanVol = (DWORD)SearchEngineVariable(FN_VOX_SETCHANVOL_NEW_SIG);

		ValidateEngineVariable(pfnSpatialize);
		ValidateEngineVariable(pfnSetChanVol);

		if (pfnSpatialize)
		{
			g_pfn_cl_viewentity = (int *)(*(DWORD *)(pfnSpatialize + 0x08));
			g_pfn_cl_parsecount = (int *)(*(DWORD *)(pfnSpatialize + 0x6E));
		}
		if (pfnSetChanVol)
		{
			g_pfn_rgrgvoxword = (void*)(*(DWORD *)(pfnSetChanVol + 0x1D));
		}
	}
	else
	{
		g_pfn_cl_viewentity = (int *)0x02F599CC;
		g_pfn_cl_parsecount = (int *)0x02DE1004;
		g_pfn_rgrgvoxword = (void*)0x027731C0;
	}

	ValidateEngineVariable(g_pfn_cl_viewentity);
	ValidateEngineVariable(g_pfn_cl_parsecount);
	ValidateEngineVariable(g_pfn_rgrgvoxword);


	//
	// HOOK FUNCTIONS
	//

	HookEngineFunction(S_Startup);
	HookEngineFunction(S_Init);
	HookEngineFunction(S_Shutdown);
	HookEngineFunction(S_FindName);
	HookEngineFunction(S_StartDynamicSound);
	HookEngineFunction(S_StartStaticSound);
	HookEngineFunction(S_StopSound);
	HookEngineFunction(S_StopAllSounds);
	HookEngineFunction(S_Update);
	HookEngineFunction(S_LoadSound);
	HookEngineFunction(S_FreeChannel);	// because VOX_LoadSound call it
}

void Sys_Error(char *error, ...)
{
	va_list argptr;
	static char string[256];

	va_start(argptr, error);
	vsnprintf(string, sizeof(string), error, argptr);
	va_end(argptr);

	HWND hwnd = GetActiveWindow();
	MessageBox(hwnd, string, "Fatal Error", MB_ICONERROR);

	exit(1);
}