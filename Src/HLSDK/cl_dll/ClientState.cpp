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
#include "ClientState.h"
#include <pm_shared.h>
#include "hud.h"
#include "fov.h"
#include "demo.h"

client_state_t ClientState;
view_model_t ViewModel;

static bool g_bClientDLLDataSave = false;

extern "C"
{
	DLLExport BOOL HUD_UpdateClientData(client_data_t *cdata, float flTime);
}

void WriteClientState(CBufferWriter &buf)
{
	buf.WriteUChar(ClientState.iFOV);
	buf.WriteUShort(ClientState.iArmor);
	buf.WriteUShort(ClientState.iHealth);
	buf.WriteUShort(ClientState.iCharacterId);
	buf.WriteUShort(ClientState.iWeaponId);
	buf.WriteUShort(ClientState.iClip);
	buf.WriteUShort(ClientState.iAmmo);
	buf.WriteUChar(ClientState.iTeam);
	buf.WriteUChar(ClientState.bEanbleBag);

	buf.WriteUChar(ViewModel.iTeam);
	buf.WriteUShort(ViewModel.iCharacterId);
	buf.WriteUShort(ViewModel.iWeaponId);
}

void ReadClientState(CBufferReader &buf)
{
	ClientState.iFOV = buf.ReadUChar();
	ClientState.iArmor = buf.ReadUShort();
	ClientState.iHealth = buf.ReadUShort();
	ClientState.iCharacterId = buf.ReadUShort();
	ClientState.iWeaponId = buf.ReadUShort();
	ClientState.iClip = buf.ReadUShort();
	ClientState.iAmmo = buf.ReadUShort();
	ClientState.iTeam = buf.ReadUChar();
	ClientState.bEanbleBag = buf.ReadUChar();

	ViewModel.iTeam = buf.ReadUChar();
	ViewModel.iCharacterId = buf.ReadUShort();
	ViewModel.iWeaponId = buf.ReadUShort();

	gFOV.Set(ClientState.iFOV, 0);
}


//----------------------------------------------------------------
// Purpose : 
//----------------------------------------------------------------
DLLExport BOOL HUD_UpdateClientData(client_data_t *pcldata, float flTime)
{
	// 没有死亡就用自己的本地数据
	if (ClientState.iUser1 == OBS_NONE)
	{
		VectorCopy(pcldata->origin, ClientState.vecOrigin);
		VectorCopy(pcldata->viewangles, ClientState.vecAngles);
	}

	if (Demo_IsRecording())
	{
		if (!g_bClientDLLDataSave)
		{
			static byte data[0x8000];

			CBufferWriter buf(data, sizeof(data));

			buf.BeginWrite();
			WriteClientState(buf);

			Demo_WriteBuffer(DEMOTYPE_CLIENTSTATE, buf.GetDataSize(), data);

			g_bClientDLLDataSave = true;
		}
	}
	else
	{
		g_bClientDLLDataSave = false;
	}

	gHUD.Think();

	pcldata->fov = gFOV.Get();

	return TRUE;
}