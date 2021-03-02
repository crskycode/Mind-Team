#include <metahook.h>
#include "plugins.h"
#include "keyvalues_helper.h"
#include "DrawTextures.h"

void (*g_pfnR_LoadSkys)(void) = NULL;

#define R_LOADSKYS_SIG "\x83\xEC\x6C\xA1\x2A\x2A\x2A\x2A\x56\x85\xC0\xC7\x44\x24\x0C\x00\x00\x00\x00\x75\x2A\x33\xF6\x8D\x4C\x24\x04\x8D\x86\x2A\x2A\x2A\x2A"

int (*gSkyTexNumber)[6] = (int (*)[6])0x280E0A0;

void R_LoadSkys(void)
{
	const char *level;
	char filename[ 64 ];
	char *ext;

	for (int i = 0; i < 6; i++)
	{
		(*gSkyTexNumber)[i] = 0;
	}

	level = gEngfuncs.pfnGetLevelName();

	if (!*level)
		return;

	strcpy(filename, level);

	ext = strrchr(filename, '.');
	strcpy(ext, ".vdf");
	
	NewKeyValues *dat = LoadKeyValuesFromFile(filename);

	if (!dat)
	{
		gEngfuncs.Con_Printf("Couldn't load %s\n", filename);
		return;
	}

	NewKeyValues *skys = dat->FindKey("Sky");

	if (skys)
	{
		const char *texname;

		texname = skys->GetString("rt");
		if (*texname)
		{
			(*gSkyTexNumber)[0] = gTexAPI.Load(texname)->id;
		}

		texname = skys->GetString("bk");
		if (*texname)
		{
			(*gSkyTexNumber)[1] = gTexAPI.Load(texname)->id;
		}

		texname = skys->GetString("lf");
		if (*texname)
		{
			(*gSkyTexNumber)[2] = gTexAPI.Load(texname)->id;
		}

		texname = skys->GetString("ft");
		if (*texname)
		{
			(*gSkyTexNumber)[3] = gTexAPI.Load(texname)->id;
		}

		texname = skys->GetString("up");
		if (*texname)
		{
			(*gSkyTexNumber)[4] = gTexAPI.Load(texname)->id;
		}

		texname = skys->GetString("dn");
		if (*texname)
		{
			(*gSkyTexNumber)[5] = gTexAPI.Load(texname)->id;
		}
	}
	else
	{
		gEngfuncs.Con_Printf("Couldn't find 'Sky' in %s\n", filename);
	}

	dat->deleteThis();
}

void Sky_InstallHook(void)
{
	*(void **)&g_pfnR_LoadSkys = SearchPattern(g_dwEngineBase, g_dwEngineSize, R_LOADSKYS_SIG, sizeof(R_LOADSKYS_SIG) - 1);

	if (g_pfnR_LoadSkys)
	{
		InlineHook(g_pfnR_LoadSkys, R_LoadSkys, (void *&)g_pfnR_LoadSkys);
	}
	else
	{
		SigNotFound(R_LOADSKYS_SIG);
	}
}