#pragma once

#ifndef ENGINE_HOOK_H
#define ENGINE_HOOK_H

typedef unsigned long DWORD;

/*
==========
EngineHook_Initialize
Get the engine module base address and image size for SearchPattern.
==========
*/
void EngineHook_Initialize( void *lpAddress );

/*
==========
EngineHook_Shutdown
Uninstall all the hooks, this function must be call before
this module has been unloaded.
==========
*/
void EngineHook_Shutdown( void );

/*
==========
EngineHook_InlineHook
Install hook for a function, return the original address for callback.
The new function must be valid before the hook has been uninstall.
==========
*/
void EngineHook_InlineHook( void *pOldFuncAddr, void *pNewFuncAddr, void **pCallBackFuncAddr );

/*
==========
EngineHook_SearchPattern
Search memory by an pattern in engine module, return address
if memory is founded, or NULL the pattern not any match.
==========
*/
void *EngineHook_SearchPattern( char *pPattern, DWORD dwPatternLen );

#endif