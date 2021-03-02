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
#include "parsemsg.h"
#include "CharacterData.h"
#include "MsgAudioData.h"
#include "MetaHook_Interface.h"

void MessageAudio(int iSender, int iTeam, int iCharacter, const char *pSentence, int iRand)
{
	CharacterData *pCharacter = CharacterData_GetData(iCharacter);

	if ( !pCharacter )
		return;

	const char *pSound;

	MsgAudio_GetFile(iTeam, pCharacter->bIsWoman, pCharacter->bIsChinese, pSentence, pSound, iRand);

	if ( !pSound )
		return;

	int iLocal = gEngfuncs.GetLocalPlayer()->index;

	if (iSender == iLocal)
	{
		static char szCurrent[MAX_QPATH];

		strcpy(szCurrent, pSound);

		char *pExt = strrchr(szCurrent, '.');

		if (pExt)
		{
			strcpy(pExt, "_C.wav");

			if (g_pFileSystem->FileExists(szCurrent))
				pSound = szCurrent;
		}
	}

	char command[100];

	sprintf(command, "spk %s\n", pSound);

	gEngfuncs.pfnClientCmd(command);
}

BOOL MsgFunc_MsgAudio(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);

	int iSender = READ_BYTE();
	int iTeam = READ_BYTE();
	int iCharacter = READ_SHORT();
	char *pSentence = READ_STRING();
	int iRand = READ_BYTE();

	MessageAudio(iSender, iTeam, iCharacter, pSentence, iRand);

	return TRUE;
}