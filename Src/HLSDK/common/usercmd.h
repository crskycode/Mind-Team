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

#ifndef USERCMD_H
#define USERCMD_H

typedef struct usercmd_s
{
	short			lerp_msec;
	byte			msec;
	vec3_t			viewangles;
	float			forwardmove;
	float			sidemove;
	float			upmove;
	byte			lightlevel;
	unsigned short	buttons;
	byte			impulse;
	byte			weaponselect;
	int				impact_index;
	vec3_t			impact_position;
	
} usercmd_t;

#endif