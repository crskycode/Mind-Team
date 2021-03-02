#include <metahook.h>
#include "plugins.h"

void (*g_pfnR_StudioSetupSkin)(void *ptexturehdr, int index) = NULL;

#define R_STUDIOSETUPSKIN_SIG "\xA0\x2A\x2A\x2A\x2A\x81\xEC\x2A\x2A\x2A\x2A\xA8\x02\x53\x56\x57\x0F\x85\x2A\x2A\x2A\x2A\x8B\x9C\x24\x20\x01\x00\x00\x8B\x84\x24\x1C\x01\x00\x00"


typedef int (*STUDIOSETUPSKINPROC)(void *entity, void *ptexturehdr, int index);

STUDIOSETUPSKINPROC g_pfnStudioSetupSkinCallback = NULL;



void R_StudioSetupSkin(void *ptexturehdr, int index)
{
	void *currententity = (void  *)*(DWORD *)0x2C2023C;

	if (g_pfnStudioSetupSkinCallback)
	{
		if (g_pfnStudioSetupSkinCallback(currententity, ptexturehdr, index))
			return;
	}

	g_pfnR_StudioSetupSkin(ptexturehdr, index);
}

void StudioModelSkin_InstallHook(void)
{
	*(void **)&g_pfnR_StudioSetupSkin = SearchPattern(g_dwEngineBase, g_dwEngineSize, R_STUDIOSETUPSKIN_SIG, sizeof(R_STUDIOSETUPSKIN_SIG) - 1);

	if (g_pfnR_StudioSetupSkin)
		InlineHook(g_pfnR_StudioSetupSkin, R_StudioSetupSkin, (void *&)g_pfnR_StudioSetupSkin);
	else
		SigNotFound(R_STUDIOSETUPSKIN_SIG);
}

void StudioModelSkin_SetCallBack(STUDIOSETUPSKINPROC pfn)
{
	g_pfnStudioSetupSkinCallback = pfn;
}