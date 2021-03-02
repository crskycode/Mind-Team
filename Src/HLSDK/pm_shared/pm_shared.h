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

#ifndef PM_SHARED_H
#define PM_SHARED_H

void PM_Init( struct playermove_s *ppmove );
void PM_Move( struct playermove_s *ppmove, int server );
char PM_FindTextureType( const char *name );

// Spectator Movement modes (stored in pev->iuser1, so the physics code can get at them)
#define OBS_NONE				0
#define OBS_CHASE_LOCKED		1
#define OBS_CHASE_FREE			2	// 自由追逐视角
#define OBS_ROAMING				3	// 自由查看
#define OBS_IN_EYE				4	// 第一人称视角
#define OBS_MAP_FREE			5
#define OBS_MAP_CHASE			6

#endif