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
#include "util.h"
#include "newkeyvalues.h"
#include "CharacterData.h"

#define MAX_CHARACTERS	256

static int				gCharacterCount = 0;
static CharacterData	gCharacterData[MAX_CHARACTERS];

void CharacterData_Init(void)
{
	char	*buffer;
	int		length;

	NewKeyValues	*pkvd;

	buffer = (char *)g_engfuncs.pfnLoadFileForMe("Configs/Character.vdf", &length);

	if (!buffer)
	{
		ALERT(at_error, "Couldn't to load character !\n");
		return;
	}

	pkvd = new NewKeyValues("Character");

	if (!pkvd->LoadFromBuffer(buffer))
	{
		ALERT(at_error, "Couldn't to parse character !\n");
	}

	g_engfuncs.pfnFreeFile(buffer);

	gCharacterCount = 0;
	memset(&gCharacterData, 0, sizeof(gCharacterData));

	for (NewKeyValues *dat = pkvd; dat; dat = dat->GetNextKey())
	{
		int iCharacterIndex = dat->GetInt("CharacterIndex");

		if (iCharacterIndex > 0)
		{
			CharacterData &info = gCharacterData[gCharacterCount++];

			info.iCharacterIndex = dat->GetInt("CharacterIndex");

			strcpy(info.szCharacterName, dat->GetString("CharacterName"));

			info.bIsWoman = dat->GetInt("IsWoman");
			info.bIsChinese = dat->GetInt("IsChinese");

			strcpy(info.szHUDEffectPath, dat->GetString("HUDEffectPath"));
			strcpy(info.szHUDLinePath, dat->GetString("HUDLinePath"));
			strcpy(info.szGRModelFile, dat->GetString("GRModelFile"));
			strcpy(info.szBLModelFile, dat->GetString("BLModelFile"));
			strcpy(info.szGRTextureFile, dat->GetString("GRTextureFile"));
			strcpy(info.szBLTextureFile, dat->GetString("BLTextureFile"));
			strcpy(info.szGRPVArmTextureFile, dat->GetString("GRPVArmTextureFile"));
			strcpy(info.szBLPVArmTextureFile, dat->GetString("BLPVArmTextureFile"));
			strcpy(info.szGRPVHandTextureFile, dat->GetString("GRPVHandTextureFile"));
			strcpy(info.szBLPVHandTextureFile, dat->GetString("BLPVHandTextureFile"));

			if (info.szGRModelFile[0])
				info.iszGRModelFile = ALLOC_STRING(info.szGRModelFile);

			if (info.szBLModelFile[0])
				info.iszBLModelFile = ALLOC_STRING(info.szBLModelFile);

			if (info.iszGRModelFile)
				PRECACHE_MODEL(STRING(info.iszGRModelFile));

			if (info.iszBLModelFile)
				PRECACHE_MODEL(STRING(info.iszBLModelFile));

		}
	}

	pkvd->deleteThis();
}

CharacterData *CharacterData_GetData(int index)
{
	if (!index)
		return NULL;

	for (int i = 0; i < gCharacterCount; i++)
	{
		if (gCharacterData[i].iCharacterIndex == index)
			return &gCharacterData[i];
	}
	
	return NULL;
}