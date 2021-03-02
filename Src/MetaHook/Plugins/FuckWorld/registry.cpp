#include <metahook.h>
#include "plugins.h"

LONG (APIENTRY *g_pfnRegQueryValueExA)(HKEY hKey, LPCSTR lpValueName, LPDWORD lpReserved, LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData) = NULL;

LONG APIENTRY NewRegQueryValueExA(HKEY hKey, LPCSTR lpValueName, LPDWORD lpReserved, LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData)
{
	LPSTR lpString = (LPSTR)lpData;

	if (!strcmp(lpValueName, "Language"))
	{
		strcpy(lpString, "english");

		if (lpcbData)
		{
			*lpcbData = strlen(lpString) + 1;
		}

		return ERROR_SUCCESS;
	}

	return g_pfnRegQueryValueExA(hKey, lpValueName, lpReserved, lpType, lpData, lpcbData);
}

void Registry_InstallHook(void)
{
	InlineHook(RegQueryValueExA, NewRegQueryValueExA, (void *&)g_pfnRegQueryValueExA);
}