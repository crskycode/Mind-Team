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

#ifndef ACTIVITY_H
#define ACTIVITY_H

enum Activity
{
	ACT_IDLE,
	ACT_WALK_FSIDE,
	ACT_WALK_BSIDE,
	ACT_WALK_LSIDE,
	ACT_WALK_RSIDE,
	ACT_RUN_FSIDE,
	ACT_RUN_BSIDE,
	ACT_RUN_LSIDE,
	ACT_RUN_RSIDE,
	ACT_C_IDLE,
	ACT_C_WALK_FSIDE,
	ACT_C_WALK_BSIDE,
	ACT_C_WALK_LSIDE,
	ACT_C_WALK_RSIDE,
	ACT_JUMP,
	ACT_UP,
	ACT_DOWN,
};

struct activity_map_t
{
	int		type;
	char *	label;
};

extern activity_map_t activity_map[];

#endif