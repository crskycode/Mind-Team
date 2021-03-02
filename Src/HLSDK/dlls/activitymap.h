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

#ifndef ACTIVITYMAP_H
#define ACTIVITYMAP_H

#include "activity.h"

activity_map_t activity_map[] = 
{
	{ ACT_IDLE, "M-idle" },
	{ ACT_WALK_FSIDE, "M-walk-FSide" },
	{ ACT_WALK_BSIDE, "M-walk-BSide" },
	{ ACT_WALK_LSIDE, "M-walk-LSide" },
	{ ACT_WALK_RSIDE, "M-walk-RSide" },
	{ ACT_RUN_FSIDE, "M-run-FSide" },
	{ ACT_RUN_BSIDE, "M-run-BSide" },
	{ ACT_RUN_LSIDE, "M-run-LSide" },
	{ ACT_RUN_RSIDE, "M-run-RSide" },
	{ ACT_C_IDLE, "M-c-idle" },
	{ ACT_C_WALK_FSIDE, "M-c-walk-FSide" },
	{ ACT_C_WALK_BSIDE, "M-c-walk-BSide" },
	{ ACT_C_WALK_LSIDE, "M-c-walk-LSide" },
	{ ACT_C_WALK_RSIDE, "M-c-walk-RSide" },
	{ ACT_JUMP, "M-jump" },
	{ ACT_UP, "M-up" },
	{ ACT_DOWN, "M-down" },

	{ NULL, NULL },
};

#endif