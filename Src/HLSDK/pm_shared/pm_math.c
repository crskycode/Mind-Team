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

#include "pm_math.h"

void AngleVectors( const vec3_t angles, vec3_t forward, vec3_t right, vec3_t up )
{
	float	angle;
	float	sr, sp, sy, cr, cp, cy;

	angle = angles[YAW] * ( M_PI * 2 / 360 );
	sy = sin( angle );
	cy = cos( angle );
	angle = angles[PITCH] * ( M_PI * 2 / 360 );
	sp = sin( angle );
	cp = cos( angle );
	angle = angles[ROLL] * ( M_PI * 2 / 360 );
	sr = sin( angle );
	cr = cos( angle );

	if ( forward )
	{
		forward[0] = cp * cy;
		forward[1] = cp * sy;
		forward[2] = -sp;
	}

	if ( right )
	{
		right[0] = ( -1 * sr * sp * cy + -1 * cr * -sy );
		right[1] = ( -1 * sr * sp * sy + -1 * cr * cy );
		right[2] = -1 * sr * cp;
	}

	if ( up )
	{
		up[0] = ( cr * sp * cy + -sr * -sy );
		up[1] = ( cr * sp * sy + -sr * cy );
		up[2] = cr * cp;
	}
}

void AngleVectorsTranspose( const vec3_t angles, vec3_t forward, vec3_t right, vec3_t up )
{
	float angle;
	float sr, sp, sy, cr, cp, cy;

	angle = angles[YAW] * ( M_PI * 2 / 360 );
	sy = sin( angle );
	cy = cos( angle );
	angle = angles[PITCH] * ( M_PI * 2 / 360 );
	sp = sin( angle );
	cp = cos( angle );
	angle = angles[ROLL] * ( M_PI * 2 / 360 );
	sr = sin( angle );
	cr = cos( angle );

	if ( forward )
	{
		forward[0] = cp * cy;
		forward[1] = sr * sp * cy + cr * -sy;
		forward[2] = cr * sp * cy + -sr * -sy;
	}

	if ( right )
	{
		right[0] = cp * sy;
		right[1] = sr * sp * sy + cr * cy;
		right[2] = cr * sp * sy + -sr * cy;
	}

	if ( up )
	{
		up[0] = -sp;
		up[1] = sr * cp;
		up[2] = cr * cp;
	}
}

float VectorNormalize( vec3_t v )
{
	float len, rat;

	len = v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
	len = sqrt( len );

	if ( len )
	{
		rat = 1 / len;

		v[0] *= rat;
		v[1] *= rat;
		v[2] *= rat;
	}

	return len;
}