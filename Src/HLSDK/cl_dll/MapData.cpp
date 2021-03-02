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
#include "cmdparse.h"
#include "MapData.h"

#define MAX_MAPS	256

static int		gMapCount = 0;
static MapData	gMapData[MAX_MAPS];

void MapData_Init(void)
{
	char	*buffer;
	int		length;

	NewKeyValues	*pkvd;

	buffer = (char *)gEngfuncs.COM_LoadFile("configs/map.vdf", 5, &length);

	if (!buffer)
	{
		gEngfuncs.Con_Printf("Error: Couldn't to load map !\n");
		return;
	}

	buffer[length] = '\0';

	pkvd = new NewKeyValues("Map");

	if (!pkvd->LoadFromBuffer(buffer))
	{
		gEngfuncs.Con_Printf("Error: Couldn't to parse sound !\n");
	}

	gEngfuncs.COM_FreeFile(buffer);

	gMapCount = 0;
	memset(&gMapData, 0, sizeof(gMapData));

	for (NewKeyValues *dat = pkvd; dat; dat = dat->GetNextKey())
	{
		const char *pszMapName = dat->GetString("MapName");

		if (pszMapName && *pszMapName)
		{
			MapData &info = gMapData[gMapCount++];

			strcpy(info.szMapName, dat->GetString("MapName"));
			strcpy(info.szMapFileName, dat->GetString("MapFileName"));

			strcpy(info.szLoadingTexFileNameGR, dat->GetString("LoadingTexFileNameGR"));
			strcpy(info.szLoadingTexFileNameBL, dat->GetString("LoadingTexFileNameBL"));

			info.iMapRuleType = dat->GetInt("MapRuleType");
			info.iMaxPlayers = dat->GetInt("MaxPlayers");

			Cmd_Init(dat->GetString("DefaultCameraPositionAndRotation"));

			info.vecDefaultCameraPositionAndRotation[0][0] = atof(Cmd_Argv(0));
			info.vecDefaultCameraPositionAndRotation[0][1] = atof(Cmd_Argv(1));
			info.vecDefaultCameraPositionAndRotation[0][2] = atof(Cmd_Argv(2));
			info.vecDefaultCameraPositionAndRotation[1][0] = atof(Cmd_Argv(3));
			info.vecDefaultCameraPositionAndRotation[1][1] = atof(Cmd_Argv(4));
			info.vecDefaultCameraPositionAndRotation[1][2] = atof(Cmd_Argv(5));

			Cmd_Init(dat->GetString("C4CameraPosAndRotA"));

			info.vecC4CameraPosAndRotA[0][0] = atof(Cmd_Argv(0));
			info.vecC4CameraPosAndRotA[0][1] = atof(Cmd_Argv(1));
			info.vecC4CameraPosAndRotA[0][2] = atof(Cmd_Argv(2));
			info.vecC4CameraPosAndRotA[1][0] = atof(Cmd_Argv(3));
			info.vecC4CameraPosAndRotA[1][1] = atof(Cmd_Argv(4));
			info.vecC4CameraPosAndRotA[1][2] = atof(Cmd_Argv(5));

			Cmd_Init(dat->GetString("C4CameraPosAndRotB"));

			info.vecC4CameraPosAndRotB[0][0] = atof(Cmd_Argv(0));
			info.vecC4CameraPosAndRotB[0][1] = atof(Cmd_Argv(1));
			info.vecC4CameraPosAndRotB[0][2] = atof(Cmd_Argv(2));
			info.vecC4CameraPosAndRotB[1][0] = atof(Cmd_Argv(3));
			info.vecC4CameraPosAndRotB[1][1] = atof(Cmd_Argv(4));
			info.vecC4CameraPosAndRotB[1][2] = atof(Cmd_Argv(5));

			Cmd_Init(dat->GetString("C4PositionA"));

			info.vecC4PositionA[0][0] = atof(Cmd_Argv(0));
			info.vecC4PositionA[0][1] = atof(Cmd_Argv(1));
			info.vecC4PositionA[0][2] = atof(Cmd_Argv(2));
			info.vecC4PositionA[1][0] = atof(Cmd_Argv(3));
			info.vecC4PositionA[1][1] = atof(Cmd_Argv(4));
			info.vecC4PositionA[1][2] = atof(Cmd_Argv(5));

			Cmd_Init(dat->GetString("C4PositionB"));

			info.vecC4PositionB[0][0] = atof(Cmd_Argv(0));
			info.vecC4PositionB[0][1] = atof(Cmd_Argv(1));
			info.vecC4PositionB[0][2] = atof(Cmd_Argv(2));
			info.vecC4PositionB[1][0] = atof(Cmd_Argv(3));
			info.vecC4PositionB[1][1] = atof(Cmd_Argv(4));
			info.vecC4PositionB[1][2] = atof(Cmd_Argv(5));

			Cmd_Init(dat->GetString("OrthoCameraPosAndRot"));

			info.vecOrthoCameraPosAndRot[0][0] = atof(Cmd_Argv(0));
			info.vecOrthoCameraPosAndRot[0][1] = atof(Cmd_Argv(1));
			info.vecOrthoCameraPosAndRot[0][2] = atof(Cmd_Argv(2));
			info.vecOrthoCameraPosAndRot[1][0] = atof(Cmd_Argv(3));
			info.vecOrthoCameraPosAndRot[1][1] = atof(Cmd_Argv(4));
			info.vecOrthoCameraPosAndRot[1][2] = atof(Cmd_Argv(5));

			strcpy(info.szMinimapFileNameGR, dat->GetString("MinimapFileNameGR"));
			strcpy(info.szMinimapFileNameBL, dat->GetString("MinimapFileNameBL"));

			strcpy(info.szMinimapFileName, dat->GetString("MinimapFileName"));

			Cmd_Init(dat->GetString("UseWeapons"));

			info.bUseWeapons[0] = atoi(Cmd_Argv(0));
			info.bUseWeapons[1] = atoi(Cmd_Argv(1));
			info.bUseWeapons[2] = atoi(Cmd_Argv(2));
			info.bUseWeapons[3] = atoi(Cmd_Argv(3));

			info.bUsableBag = dat->GetInt("UsableBag");
		}
	}

	pkvd->deleteThis();
}

MapData *MapData_GetDataForIndex(int index)
{
	if (!index)
		return NULL;

	for (int i = 0; i < gMapCount; i++)
	{
		if (gMapData[i].iMapIndex == index)
			return &gMapData[i];
	}

	return NULL;
}

MapData *MapData_GetDataForName(const char *pszName)
{
	if (!pszName || !*pszName)
		return NULL;

	for (int i = 0; i < gMapCount; i++)
	{
		if (!strcmp(gMapData[i].szMapName, pszName))
			return &gMapData[i];
	}

	return NULL;
}

MapData *MapData_getDataForFile(const char *pszName)
{
	if (!pszName || !*pszName)
		return NULL;

	for (int i = 0; i < gMapCount; i++)
	{
		if (!stricmp(gMapData[i].szMapFileName, pszName))
			return &gMapData[i];
	}

	return NULL;
}