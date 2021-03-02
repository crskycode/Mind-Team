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

#ifndef PM_MOVEVARS_H
#define PM_MOVEVARS_H

typedef struct movevars_s
{
	float		gravity;			// Gravity for map
	float		stopspeed;			// Deceleration when not moving
	float		maxspeed;			// Max allowed speed
	float		spectatormaxspeed;
	float		accelerate;			// Acceleration factor
	float		airaccelerate;		// Same for when in open air
	float		wateraccelerate;	// Same for when in water
	float		friction;
	float		edgefriction;		// Extra friction near dropofs 
	float		waterfriction;		// Less in water
	float		entgravity;			// 1.0
	float		bounce;				// Wall bounce value. 1.0
	float		stepsize;			// sv_stepsize;
	float		maxvelocity;		// maximum server velocity.
	float		zmax;				// Max z-buffer range (for GL)
	float		waveHeight;			// Water wave height (for GL)
	qboolean	footsteps;			// Play footstep sounds
	char		skyName[32];		// Name of the sky map
	float		rollangle;
	float		rollspeed;
	float		skycolor_r;			// Sky color
	float		skycolor_g;			//
	float		skycolor_b;			//
	float		skyvec_x;			// Sky vector
	float		skyvec_y;			//
	float		skyvec_z;			//

} movevars_t;

#endif