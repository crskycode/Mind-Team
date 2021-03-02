
#include <windows.h>
#include <psapi.h>
#include "detours/detours.h"


DWORD g_dwEngineBase = NULL;
DWORD g_dwEngineSize = NULL;


typedef struct tagENGINEHOOK
{
	void *pOldFuncAddr;
	void *pNewFuncAddr;

	tagENGINEHOOK *pNext;

} ENGINEHOOK;


ENGINEHOOK *g_pEngineHookList = NULL;


void EngineHook_Initialize(void *lpAddress)
{
	HMODULE hEngine;
	MODULEINFO modinfo;

	if (lpAddress == NULL)
		return;

	if (GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT | GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (LPCSTR)lpAddress, &hEngine) == FALSE)
		return;

	if (GetModuleInformation(GetCurrentProcess(), hEngine, &modinfo, sizeof(modinfo)) == FALSE)
		return;

	g_dwEngineBase = (DWORD)modinfo.lpBaseOfDll;
	g_dwEngineSize = (DWORD)modinfo.SizeOfImage;
}

void EngineHook_Shutdown(void)
{
	ENGINEHOOK *lpHook;

	lpHook = g_pEngineHookList;

	while (lpHook)
	{
		ENGINEHOOK *pNext = lpHook->pNext;

		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourDetach(&lpHook->pOldFuncAddr, lpHook->pNewFuncAddr);
		DetourTransactionCommit();

		delete lpHook;

		lpHook = pNext;
	}
}

void EngineHook_InlineHook(void *pOldFuncAddr, void *pNewFuncAddr, void **pCallBackFuncAddr)
{
	ENGINEHOOK *lpHook;

	lpHook = new ENGINEHOOK;
	lpHook->pOldFuncAddr = pOldFuncAddr;
	lpHook->pNewFuncAddr = pNewFuncAddr;

	lpHook->pNext = g_pEngineHookList;
	g_pEngineHookList = lpHook;

	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourAttach(&lpHook->pOldFuncAddr, pNewFuncAddr);
	DetourTransactionCommit();

	*pCallBackFuncAddr = lpHook->pOldFuncAddr;
}

void* EngineHook_SearchPattern(char *pPattern, DWORD dwPatternLen)
{
	if (g_dwEngineBase == NULL || g_dwEngineSize == NULL)
		return NULL;

	DWORD dwStartAddr = (DWORD)g_dwEngineBase;
	DWORD dwEndAddr = dwStartAddr + g_dwEngineSize - dwPatternLen;

	while (dwStartAddr < dwEndAddr)
	{
		bool found = true;

		for (DWORD i = 0; i < dwPatternLen; i++)
		{
			char code = *(char *)(dwStartAddr + i);

			if (pPattern[i] != 0x2A && pPattern[i] != code)
			{
				found = false;
				break;
			}
		}

		if (found)
			return (void *)dwStartAddr;

		dwStartAddr++;
	}

	return NULL;
}