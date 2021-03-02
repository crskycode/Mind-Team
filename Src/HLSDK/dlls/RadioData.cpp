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
#include <newkeyvalues.h>


struct RadioSound
{
	int iNumFile;
	char szFileName[5][MAX_QPATH];
};

struct RadioTeam
{
	RadioSound m_English;
	RadioSound m_Woman;
	RadioSound m_Chinese;
	RadioSound m_ChineseWoman;
};

struct RadioSentence
{
	RadioTeam m_Team[2];
};

struct RadioGroup
{
	RadioSentence m_Sentence[5];
};

struct RadioData
{
	RadioGroup m_Group[3];
};


static RadioData gRadioData;


void RadioData_Init(void)
{
	char	*buffer;
	int		length;

	NewKeyValues	*pkvd;

	buffer = (char *)g_engfuncs.pfnLoadFileForMe("Configs/Radio.vdf", &length);

	if (!buffer)
	{
		ALERT(at_error, "Couldn't to load radio !\n");
		return;
	}

	buffer[length] = '\0';

	pkvd = new NewKeyValues("Radio");

	if (!pkvd->LoadFromBuffer(buffer))
	{
		ALERT(at_error, "Couldn't to parse radio !\n");
	}

	g_engfuncs.pfnFreeFile(buffer);

	memset(&gRadioData, 0, sizeof(gRadioData));

	static char szGrouToken[3][2] = { "Z", "X", "C" };
	static char szSentToken[5][2] = { "1", "2", "3", "4", "5" };
	static char szTeamToken[2][3] = { "GR", "BL" };

	for (int iGroup = 0; iGroup < 3; iGroup++)
	{
		NewKeyValues *pGroup = pkvd->FindKey( szGrouToken[iGroup] );

		if (!pGroup)
			continue;

		RadioGroup &group = gRadioData.m_Group[iGroup];

		for (int iSente = 0; iSente < 5; iSente++)
		{
			NewKeyValues *pSente = pGroup->FindKey( szSentToken[iSente] );

			if (!pSente)
				continue;

			RadioSentence &sente = group.m_Sentence[iSente];

			for (int iTeam = 0; iTeam < 2; iTeam++)
			{
				NewKeyValues *pTeam = pSente->FindKey( szTeamToken[iTeam] );

				if (!pTeam)
					continue;

				RadioTeam &team = sente.m_Team[iTeam];

				NewKeyValues *pEnglish = pTeam->FindKey("english");

				if (pEnglish)
				{
					for (NewKeyValues *pSound = pEnglish->GetFirstValue(); pSound; pSound = pSound->GetNextKey())
					{
						char *file = team.m_English.szFileName[team.m_English.iNumFile++];
						strcpy(file, pSound->GetString());
					}
				}

				NewKeyValues *pWoman = pTeam->FindKey("woman");

				if (pWoman)
				{
					for (NewKeyValues *pSound = pWoman->GetFirstValue(); pSound; pSound = pSound->GetNextKey())
					{
						char *file = team.m_Woman.szFileName[team.m_Woman.iNumFile++];
						strcpy(file, pSound->GetString());
					}
				}

				NewKeyValues *pChinese = pTeam->FindKey("chinese");

				if (pChinese)
				{
					for (NewKeyValues *pSound = pChinese->GetFirstValue(); pSound; pSound = pSound->GetNextKey())
					{
						char *file = team.m_Chinese.szFileName[team.m_Chinese.iNumFile++];
						strcpy(file, pSound->GetString());
					}
				}

				NewKeyValues *pchineseWoman = pTeam->FindKey("chineseWoman");

				if (pchineseWoman)
				{
					for (NewKeyValues *pSound = pchineseWoman->GetFirstValue(); pSound; pSound = pSound->GetNextKey())
					{
						char *file = team.m_ChineseWoman.szFileName[team.m_ChineseWoman.iNumFile++];
						strcpy(file, pSound->GetString());
					}
				}
			}
		}
	}

	pkvd->deleteThis();
}

void RadioData_GetFile(int iTeam, bool bWoman, bool bChinese, int iGroup, int iSentence, const char *&pSoundFile, int &iRand)
{
	pSoundFile = NULL;
	iRand = 0;

	if (iTeam < 1 || iTeam > 2)
		return;

	if (iGroup < 1 || iGroup > 3)
		return;

	if (iSentence < 1 || iSentence > 5)
		return;

	RadioSound *pSound;

	if (!bWoman)
	{
		if (!bChinese)
			pSound = &gRadioData.m_Group[iGroup - 1].m_Sentence[iSentence - 1].m_Team[iTeam - 1].m_English;
		else
			pSound = &gRadioData.m_Group[iGroup - 1].m_Sentence[iSentence - 1].m_Team[iTeam - 1].m_Chinese;
	}
	else
	{
		if (!bChinese)
			pSound = &gRadioData.m_Group[iGroup - 1].m_Sentence[iSentence - 1].m_Team[iTeam - 1].m_Woman;
		else
			pSound = &gRadioData.m_Group[iGroup - 1].m_Sentence[iSentence - 1].m_Team[iTeam - 1].m_ChineseWoman;
	}

	if (!pSound->iNumFile)
		return;

	iRand = g_engfuncs.pfnRandomLong(0, pSound->iNumFile - 1);

	pSoundFile = pSound->szFileName[ iRand ];
}