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

#ifndef DEMO_H
#define DEMO_H

// Types of demo messages we can write/parse
enum
{
	DEMOTYPE_CLIENTSTATE,
	DEMOTYPE_HUD,
};

bool Demo_IsRecording(void);
bool Demo_IsPlayingBack(void);
void Demo_WriteBuffer(int type, int size, unsigned char *buffer);

#endif