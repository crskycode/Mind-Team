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

#ifndef MAPDATA_H
#define MAPDATA_H

struct MapData
{
	int			iMapIndex;
	char		szMapName[64];
	char		szMapFileName[MAX_QPATH];
	char		szLoadingTexFileNameGR[MAX_QPATH];
	char		szLoadingTexFileNameBL[MAX_QPATH];
	int			iMapRuleType;
	int			iMaxPlayers;
	vec3_t		vecDefaultCameraPositionAndRotation[2];	// Server
	vec3_t		vecC4CameraPosAndRotA[2];
	vec3_t		vecC4CameraPosAndRotB[2];
	vec3_t		vecC4PositionA[2];
	vec3_t		vecC4PositionB[2];
	vec3_t		vecOrthoCameraPosAndRot[2];
	char		szMinimapFileNameGR[MAX_QPATH];
	char		szMinimapFileNameBL[MAX_QPATH];
	char		szMinimapFileName[MAX_QPATH];
	int			bUseWeapons[4];
	bool		bUsableBag;
};

MapData *MapData_GetDataForIndex(int index);
MapData *MapData_GetDataForName(const char *pszName);
MapData *MapData_getDataForFile(const char *pszName);

#endif