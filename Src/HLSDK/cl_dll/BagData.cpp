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
#include <newkeyvalues.h>
#include "cmdparse.h"

struct BagWeapon
{
	int iWeapon[3];
};

struct Bag
{
	BagWeapon m_Weapon[4];
};

struct BagData
{
	Bag m_Bag[3];
};


static BagData gBagData;


void BagData_Init(void)
{
	char	*buffer;
	int		length;

	NewKeyValues	*pkvd;

	buffer = (char *)gEngfuncs.COM_LoadFile("BagConfig.vdf", 5, &length);

	if (!buffer)
	{
		gEngfuncs.Con_Printf("Error: Couldn't to load bag !\n");
		return;
	}

	buffer[length] = '\0';

	pkvd = new NewKeyValues("Bag");

	if (!pkvd->LoadFromBuffer(buffer))
	{
		gEngfuncs.Con_Printf("Error: Couldn't to parse bag !\n");
	}

	gEngfuncs.COM_FreeFile(buffer);

	memset(&gBagData, 0, sizeof(gBagData));

	static char szBagToken[3][2] = { "1", "2", "3" };
	static char szSloToken[4][2] = { "0", "1", "2", "3" };

	for (int iBag = 0; iBag < 3; iBag++)
	{
		NewKeyValues *pBag = pkvd->FindKey( szBagToken[iBag] );

		if (!pBag)
			continue;

		Bag &bag = gBagData.m_Bag[iBag];

		for (int iSlot = 0; iSlot < 4; iSlot++)
		{
			char *text = (char *)pBag->GetString( szSloToken[iSlot] );

			if (!*text)
				continue;

			BagWeapon &weapon = bag.m_Weapon[iSlot];

			Cmd_Init(text);

			weapon.iWeapon[0] = atoi(Cmd_Argv(0));
			weapon.iWeapon[1] = atoi(Cmd_Argv(1));
			weapon.iWeapon[2] = atoi(Cmd_Argv(2));
		}
	}

	pkvd->deleteThis();
}

int BagData_GetWeapon(int iBag, int iSlot, int iNum)
{
	if (iBag < 1 || iBag > 3)
		return 0;

	if (iSlot < 1 || iSlot > 4)
		return 0;

	if (iNum < 1 || iNum > 3)
		return 0;

	return gBagData.m_Bag[iBag - 1].m_Weapon[iSlot -1].iWeapon[iNum - 1];
}