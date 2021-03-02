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

#ifndef CHARACTERDATA_H
#define CHARACTERDATA_H

struct CharacterData
{
	int		iCharacterIndex;
	char	szCharacterName[64];
	bool	bIsWoman;
	bool	bIsChinese;
	char	szHUDEffectPath[MAX_QPATH];
	char	szHUDLinePath[MAX_QPATH];
	char	szGRModelFile[MAX_QPATH];
	char	szBLModelFile[MAX_QPATH];
	char	szGRTextureFile[MAX_QPATH];
	char	szBLTextureFile[MAX_QPATH];
	char	szGRPVArmTextureFile[MAX_QPATH];
	char	szBLPVArmTextureFile[MAX_QPATH];
	char	szGRPVHandTextureFile[MAX_QPATH];
	char	szBLPVHandTextureFile[MAX_QPATH];

	string_t	iszGRModelFile;
	string_t	iszBLModelFile;
};

CharacterData *CharacterData_GetData(int index);

#endif