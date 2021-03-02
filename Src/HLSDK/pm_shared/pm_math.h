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

#ifndef PM_MATH_H
#define PM_MATH_H

#define _USE_MATH_DEFINES
#include <math.h>

typedef float vec3_t[3];

// up down
#define	PITCH	0
// left right
#define	YAW		1
// roll
#define	ROLL	2 

#define VectorCopy(a, b) \
	{ \
		(b)[0] = (a)[0]; \
		(b)[1] = (a)[1]; \
		(b)[2] = (a)[2]; \
	}

#define VectorSubtract(a, b, c) \
	{ \
		(c)[0] = (a)[0] - (b)[0]; \
		(c)[1] = (a)[1] - (b)[1]; \
		(c)[2] = (a)[2] - (b)[2]; \
	}

#define VectorAdd(a, b, c) \
	{ \
		(c)[0] = (a)[0] + (b)[0]; \
		(c)[1] = (a)[1] + (b)[1]; \
		(c)[2] = (a)[2] + (b)[2]; \
	}

#define VectorClear(a) \
	{ \
		(a)[0] = 0; \
		(a)[1] = 0; \
		(a)[2] = 0; \
	}

#define CrossProduct(a, b, c) \
	{ \
		(c)[0] = (a)[1] * (b)[2] - (a)[2] * (b)[1]; \
		(c)[1] = (a)[2] * (b)[0] - (a)[0] * (b)[2]; \
		(c)[2] = (a)[0] * (b)[1] - (a)[1] * (b)[0]; \
	}

#define VectorScale(a, b, c) \
	{ \
		(c)[0] = (a)[0] * b; \
		(c)[1] = (a)[1] * b; \
		(c)[2] = (a)[2] * b; \
	}

#define VectorMA(a, s, b, c) \
	{ \
		(c)[0] = (a)[0] + s * (b)[0]; \
		(c)[1] = (a)[1] + s * (b)[1]; \
		(c)[2] = (a)[2] + s * (b)[2]; \
	}

#define Length(a) ( sqrt((a)[0]*(a)[0] + (a)[1]*(a)[1] + (a)[2]*(a)[2]) )
#define DotProduct(a, b) ( (a)[0]*(b)[0] + (a)[1]*(b)[1] + (a)[2]*(b)[2] )
#define VectorCompare(a, b) ( (a)[0]==(b)[0] && (a)[1]==(b)[1] && (a)[2]==(b)[2] )

float VectorNormalize(vec3_t v);
void AngleVectors(const vec3_t angles, vec3_t forward, vec3_t right, vec3_t up);
void AngleVectorsTranspose(const vec3_t angles, vec3_t forward, vec3_t right, vec3_t up);

#endif