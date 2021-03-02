#ifndef PLUGINS_H
#define PLUGINS_H

#include "sys.h"

extern IFileSystem *g_pFileSystem;

extern DWORD g_dwEngineBase, g_dwEngineSize;

#define SigNotFound(sig) Sys_Error("Signature \"%s\" is not found !\nFile: %s\nLine: %d", #sig, __FILE__, __LINE__)

inline void *SearchPattern(DWORD dwStartSearch, DWORD dwSearchLen, char *pPattern, DWORD dwPatternLen)
{
	return g_pMetaHookAPI->SearchPattern((void *)dwStartSearch, dwSearchLen, pPattern, dwPatternLen);
}

inline hook_t *InlineHook( void *pOldFuncAddr, void *pNewFuncAddr, void *&pCallBackFuncAddr )
{
	return g_pMetaHookAPI->InlineHook( pOldFuncAddr, pNewFuncAddr, pCallBackFuncAddr );
}

#define CON_PRINTF	( *gEngfuncs.Con_Printf )

void MHP_LogFile(const char *format, ...);

#endif