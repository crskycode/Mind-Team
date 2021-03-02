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

#ifndef SOUNDDATA_H
#define SOUNDDATA_H

struct SoundData
{
	char	szName[64];
	char	szSample[MAX_QPATH];
	float	flVolume;
	float	flAttn;
	int		iPitch;
};

SoundData *SoundData_GetData(const char *pszName);

#endif