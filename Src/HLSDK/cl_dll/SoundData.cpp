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
#include "newkeyvalues.h"
#include "SoundData.h"

#include <string>

#if _MSC_VER > 1400
#include <unordered_map>
typedef std::unordered_map<std::string, SoundData *> soundmap_t;
#else
#include <hash_map>
typedef stdext::hash_map<std::string, SoundData *> soundmap_t;
#endif

static soundmap_t g_soundMap;

#define MAX_SOUNDS	1024

static int			gSoundCount = 0;
static SoundData	gSoundData[MAX_SOUNDS];

void SoundData_Init(void)
{
	char	*buffer;
	int		length;

	NewKeyValues	*pkvd;

	buffer = (char *)gEngfuncs.COM_LoadFile("configs/sound.vdf", 5, &length);

	if (!buffer)
	{
		gEngfuncs.Con_Printf("Error: Couldn't to load sound !\n");
		return;
	}

	buffer[length] = '\0';

	pkvd = new NewKeyValues("Sound");

	if (!pkvd->LoadFromBuffer(buffer))
	{
		gEngfuncs.Con_Printf("Error: Couldn't to parse sound !\n");
	}

	gEngfuncs.COM_FreeFile(buffer);

	gSoundCount = 0;
	memset(&gSoundData, 0, sizeof(gSoundData));

	for (NewKeyValues *dat = pkvd; dat; dat = dat->GetNextKey())
	{
		const char *pszName = dat->GetString("Name");

		if (pszName && *pszName)
		{
			SoundData &info = gSoundData[gSoundCount++];

			strcpy(info.szName, dat->GetString("Name"));

			strcpy(info.szSample, dat->GetString("Sample"));

			info.flVolume = dat->GetFloat("Volume");
			info.flAttn = dat->GetFloat("Attenuation");
			info.iPitch = dat->GetInt("Pitch");

			// Insert to hashmap
			g_soundMap.insert(soundmap_t::value_type(info.szName, &info));
		}
	}

	pkvd->deleteThis();
}

SoundData *SoundData_GetData(const char *pszName)
{
	if (!pszName || !*pszName)
		return NULL;

	//for (int i = 0; i < gSoundCount; i++)
	//{
	//	if (!strcmp(gSoundData[i].szName, pszName))
	//		return &gSoundData[i];
	//}

	soundmap_t::iterator it = g_soundMap.find(pszName);

	if (it != g_soundMap.end())
		return it->second;

	return NULL;
}