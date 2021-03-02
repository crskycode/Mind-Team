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

#ifndef MATHLIB_H
#define MATHLIB_H

// vector
typedef float vec2_t[2];
typedef float vec3_t[3];
typedef float vec4_t[4];

// matrix
typedef float mat4_t[4][4];

// up down
#define PITCH	0
// left right
#define YAW		1
// roll
#define ROLL	2

inline void VectorCopy(const float *in, float *out)
{
	out[0] = in[0];
	out[1] = in[1];
	out[2] = in[2];
}

inline void VectorCopy4(const float *in, float *out)
{
	out[0] = in[0];
	out[1] = in[1];
	out[2] = in[2];
	out[3] = in[3];
}


inline void VectorSubtract(const float *in1, const float *in2, float *out)
{
	out[0] = in1[0] - in2[0];
	out[1] = in1[1] - in2[1];
	out[2] = in1[2] - in2[2];
}

inline void VectorAdd(const float *in1, const float *in2, float *out)
{
	out[0] = in1[0] + in2[0];
	out[1] = in1[1] + in2[1];
	out[2] = in1[2] + in2[2];
}

inline void VectorScale(const float *in, float scale, float *out)
{
	out[0] = in[0] * scale;
	out[1] = in[1] * scale;
	out[2] = in[2] * scale;
}

inline void VectorClear(float *in)
{
	in[0] = 0;
	in[1] = 0;
	in[2] = 0;
}

inline void CrossProduct(const float *in1, const float *in2, float *out)
{
	out[0] = in1[1]*in2[2] - in1[2]*in2[1];
	out[1] = in1[2]*in2[0] - in1[0]*in2[2];
	out[2] = in1[0]*in2[1] - in1[1]*in2[0];
}

inline float DotProduct(const float *in1, const float *in2)
{
	return in1[0]*in2[0] + in1[1]*in2[1] + in1[2]*in2[2];
}

inline float VectorLength(const float *in)
{
	return sqrt(in[0]*in[0] + in[1]*in[1] + in[2]*in[2]);
}

inline bool VectorCompare(const float *in1, const float *in2)
{
	return in1[0]==in2[0] && in1[1]==in2[1] && in1[2]==in2[2];
}

inline float Length(const float *v)
{
	return sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
}

inline void VectorMA(const float *veca, float scale, const float *vecb, float *vecc)
{
	vecc[0] = veca[0] + scale*vecb[0];
	vecc[1] = veca[1] + scale*vecb[1];
	vecc[2] = veca[2] + scale*vecb[2];
}

inline void VectorInverse(float *v)
{
	v[0] = -v[0];
	v[1] = -v[1];
	v[2] = -v[2];
}

inline float Distance(const float *v1, const float *v2)
{
	vec3_t d;
	VectorSubtract(v2, v1, d);
	return Length(d);
}

float VectorNormalize(float *v);
void AngleMatrix(const float *angles, float (*matrix)[4]);
void VectorTransform(const float *in1, const float (*in2)[4], float *out);
void NormalizeAngles(float *angles);
void VectorAngles(const float *forward, float *angles);

inline float Radian(float a)
{
	return a * (M_PI / 180.0);
}
inline float Degree(float a)
{
	return a * (180.0 / M_PI);
}

#endif