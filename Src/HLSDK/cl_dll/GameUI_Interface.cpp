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
#include <IGameUI.h>

static IGameUI *gameui = NULL;

void GameUI_Initailize(void)
{
	if (!gameui)
	{
		HINTERFACEMODULE hGameUI = (HINTERFACEMODULE)GetModuleHandleA("GameUI.dll");

		if (hGameUI)
		{
			CreateInterfaceFn pfnFactory = Sys_GetFactory(hGameUI);

			if (pfnFactory)
			{
				gameui = (IGameUI *)pfnFactory(GAMEUI_INTERFACE_VERSION, NULL);
			}
		}
	}
}

bool GameUI_IsGameUIActive(void)
{
	GameUI_Initailize();

	if (gameui)
	{
		return gameui->IsGameUIActive();
	}

	return false;
}