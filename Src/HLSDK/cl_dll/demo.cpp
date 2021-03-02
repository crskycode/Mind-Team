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
#include "hud.h"
#include "demo.h"
#include <demo_api.h>
#include "buffer.h"

extern "C"
{
	DLLExport void Demo_ReadBuffer(int size, unsigned char *buffer);
}

void ReadClientState(CBufferReader &buf);

//----------------------------------------------------------------
// Purpose : 
//----------------------------------------------------------------
bool Demo_IsRecording(void)
{
	return gEngfuncs.pDemoAPI->IsRecording();
}

//----------------------------------------------------------------
// Purpose : 
//----------------------------------------------------------------
bool Demo_IsPlayingBack(void)
{
	return gEngfuncs.pDemoAPI->IsPlayingback();
}

//----------------------------------------------------------------
// Purpose : 
//----------------------------------------------------------------
void Demo_WriteBuffer(int type, int size, unsigned char *buffer)
{
	static byte data[0x8000];

	CBufferWriter buf(data, sizeof(data));

	buf.BeginWrite();
	buf.WriteUChar(type);
	buf.WriteData(buffer, size);

	gEngfuncs.pDemoAPI->WriteBuffer(buf.GetDataSize(), data);
}

//----------------------------------------------------------------
// Purpose : Engine wants us to parse some data from the demo stream
//----------------------------------------------------------------
DLLExport void Demo_ReadBuffer(int size, unsigned char *buffer)
{
	CBufferReader buf(buffer, size);

	buf.BeginRead();

	int type = buf.ReadUChar();

	switch (type)
	{
	case DEMOTYPE_CLIENTSTATE:
		{
			ReadClientState(buf);
			break;
		}
	case DEMOTYPE_HUD:
		{
			gHUD.Restore(buf);
			break;
		}
	}
}