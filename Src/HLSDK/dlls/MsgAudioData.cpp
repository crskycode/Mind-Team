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

#include <string>
#if _MSC_VER > 1400
#include <unordered_map>
#else
#include <hash_map>
#endif

struct MessageSound
{
	int iNumFile;
	char szFileName[5][MAX_QPATH];
};

struct MessageTeam
{
	MessageSound m_English;
	MessageSound m_Woman;
	MessageSound m_Chinese;
	MessageSound m_ChineseWoman;
};

struct MessageSentence
{
	char szName[64];
	MessageTeam m_Team[2];
};

struct MessageData
{
	int iNumSentence;
	MessageSentence m_Sentence[100];
};


static MessageData gMessageData;

#if _MSC_VER > 1400
typedef std::unordered_map<std::string, MessageSentence *> messagemap_t;
#else
typedef stdext::hash_map<std::string, MessageSentence *> messagemap_t;
#endif

static messagemap_t g_messageMap;


void MsgAudio_Init(void)
{
	char	*bufer;
	int		length;

	NewKeyValues	*pkvd;

	bufer = (char *)g_engfuncs.pfnLoadFileForMe("Configs/MsgAudio.vdf", &length);

	if (!bufer)
	{
		ALERT(at_error, "Couldn't to load message !\n");
		return;
	}

	bufer[length] = '\0';

	pkvd = new NewKeyValues("MessageAudio");

	if (!pkvd->LoadFromBuffer(bufer))
	{
		ALERT(at_error, "Couldn't to parse message !\n");
	}

	g_engfuncs.pfnFreeFile(bufer);

	memset(&gMessageData, 0, sizeof(gMessageData));

	static char szTeamToken[2][3] = { "GR", "BL" };

	for (NewKeyValues *pSent = pkvd->GetFirstSubKey(); pSent; pSent = pSent->GetNextKey())
	{
		MessageSentence &sent = gMessageData.m_Sentence[gMessageData.iNumSentence++];

		strcpy(sent.szName, pSent->GetName());

		for (int iTeam = 0; iTeam < 2; iTeam++)
		{
			NewKeyValues *pTeam = pSent->FindKey( szTeamToken[iTeam] );

			if ( !pTeam )
				continue;

			MessageTeam &team = sent.m_Team[iTeam];

			NewKeyValues *pEnglish = pTeam->FindKey("english");

			if (pEnglish)
			{
				for (NewKeyValues *pSound = pEnglish->GetFirstValue(); pSound; pSound = pSound->GetNextKey())
				{
					strcpy(team.m_English.szFileName[team.m_English.iNumFile++], pSound->GetString());
				}
			}

			NewKeyValues *pWoman = pTeam->FindKey("woman");

			if (pWoman)
			{
				for (NewKeyValues *pSound = pWoman->GetFirstValue(); pSound; pSound = pSound->GetNextKey())
				{
					strcpy(team.m_Woman.szFileName[team.m_Woman.iNumFile++], pSound->GetString());
				}
			}

			NewKeyValues *pChinese = pTeam->FindKey("chinese");

			if (pChinese)
			{
				for (NewKeyValues *pSound = pChinese->GetFirstValue(); pSound; pSound = pSound->GetNextKey())
				{
					strcpy(team.m_Chinese.szFileName[team.m_Chinese.iNumFile++], pSound->GetString());
				}
			}

			NewKeyValues *pChineseWoman = pTeam->FindKey("chineseWoman");

			if (pChineseWoman)
			{
				for (NewKeyValues *pSound = pChineseWoman->GetFirstValue(); pSound; pSound = pSound->GetNextKey())
				{
					strcpy(team.m_ChineseWoman.szFileName[team.m_ChineseWoman.iNumFile++], pSound->GetString());
				}
			}
		}

		// Insert to hashmap
		g_messageMap.insert(messagemap_t::value_type(sent.szName, &sent));
	}

	pkvd->deleteThis();
}

void MsgAudio_GetFile(int iTeam, bool bIsWoman, bool bIsChinese, const char *pSentence, const char *&pSoundFile, int &iRand)
{
	pSoundFile = NULL;

	if (iTeam < 1 || iTeam > 2)
		return;

	if (!pSentence || !*pSentence)
		return;

	messagemap_t::iterator it = g_messageMap.find(pSentence);

	if (it == g_messageMap.end())
		return;

	MessageSentence *sent = it->second;

	MessageSound *pSound;

	if (!bIsWoman)
	{
		if (!bIsChinese)
			pSound = &sent->m_Team[iTeam - 1].m_English;
		else
			pSound = &sent->m_Team[iTeam - 1].m_Chinese;
	}
	else
	{
		if (!bIsChinese)
			pSound = &sent->m_Team[iTeam - 1].m_Woman;
		else
			pSound = &sent->m_Team[iTeam - 1].m_ChineseWoman;
	}

	if (!pSound->iNumFile)
		return;

	iRand = g_engfuncs.pfnRandomLong(0, pSound->iNumFile - 1);

	pSoundFile = pSound->szFileName[ iRand ];
}