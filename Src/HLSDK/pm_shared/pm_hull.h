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

#ifndef PLAYER_HULL_H
#define PLAYER_HULL_H

#define __VEC_POIN_HULL_MIN__	  0,   0,   0
#define __VEC_POIN_HULL_MAX__	  0,   0,   0
#define __VEC_IDLE_HULL_MIN__	-16, -16, -36
#define __VEC_IDLE_HULL_MAX__	 16,  16,  36
#define __VEC_DUCK_HULL_MIN__	-16, -16, -18
#define __VEC_DUCK_HULL_MAX__	 16,  16,  32

#define __VEC_IDLE_VIEW__		  0,   0,  24
#define __VEC_DUCK_VIEW__		  0,   0,  15


#ifdef vec3_t

#define VEC_POIN_HULL_MIN Vector( __VEC_POIN_HULL_MIN__ )
#define VEC_POIN_HULL_MAX Vector( __VEC_POIN_HULL_MAX__ )
#define VEC_IDLE_HULL_MIN Vector( __VEC_IDLE_HULL_MIN__ )
#define VEC_IDLE_HULL_MAX Vector( __VEC_IDLE_HULL_MAX__ )
#define VEC_DUCK_HULL_MIN Vector( __VEC_DUCK_HULL_MIN__ )
#define VEC_DUCK_HULL_MAX Vector( __VEC_DUCK_HULL_MAX__ )

#define VEC_IDLE_VIEW Vector( __VEC_IDLE_VIEW__ )
#define VEC_DUCK_VIEW Vector( __VEC_DUCK_VIEW__ )

#else

extern vec3_t VEC_POIN_HULL_MIN;
extern vec3_t VEC_POIN_HULL_MAX;
extern vec3_t VEC_IDLE_HULL_MIN;
extern vec3_t VEC_IDLE_HULL_MAX;
extern vec3_t VEC_DUCK_HULL_MIN;
extern vec3_t VEC_DUCK_HULL_MAX;

extern vec3_t VEC_IDLE_VIEW;
extern vec3_t VEC_DUCK_VIEW;

#endif

#endif