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

#include <string.h>
#include <stdlib.h>
#include "interface.h"

InterfaceReg *InterfaceReg::s_pInterfaceRegs = NULL;


InterfaceReg::InterfaceReg(InstantiateInterfaceFn fn, const char *pName) : m_pName(pName)
{
	m_CreateFn = fn;
	m_pNext = s_pInterfaceRegs;
	s_pInterfaceRegs = this;
}

DLLExport IBaseInterface *CreateInterface(const char *pName, int *pReturnCode)
{
	InterfaceReg *pCur;

	for(pCur = InterfaceReg::s_pInterfaceRegs; pCur; pCur = pCur->m_pNext)
	{
		if(!strcmp(pCur->m_pName, pName))
		{
			if (pReturnCode)
			{
				*pReturnCode = IFACE_OK;
			}

			return pCur->m_CreateFn();
		}
	}

	if (pReturnCode)
	{
		*pReturnCode = IFACE_FAILED;
	}

	return NULL;
}

#define WIN32_LEAN_AND_MEAN
#define NOWINRES
#define NOSERVICE
#define NOMCX
#define NOIME
#include "windows.h"

HINTERFACEMODULE Sys_LoadModule(const char *pModuleName)
{
	return (HINTERFACEMODULE)LoadLibrary(pModuleName);
}

void Sys_FreeModule(HINTERFACEMODULE hModule)
{
	if(!hModule)
		return;

	FreeLibrary((HMODULE)hModule);
}

CreateInterfaceFn Sys_GetFactoryThis(void)
{
	return CreateInterface;
}

CreateInterfaceFn Sys_GetFactory(HINTERFACEMODULE hModule)
{
	if(!hModule)
		return NULL;

	return (CreateInterfaceFn)GetProcAddress((HMODULE)hModule, "CreateInterface");
}