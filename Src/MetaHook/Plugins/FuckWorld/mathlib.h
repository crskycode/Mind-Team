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

#define _USE_MATH_DEFINES
#include <math.h>

// vector
typedef float vec2_t[2];
typedef float vec3_t[3];
typedef float vec4_t[4];

// up down
#define PITCH	0
// left right
#define YAW		1
// roll
#define ROLL	2

void VectorCopy(const float *in, float *out);
void VectorSubtract(const float *in1, const float *in2, float *out);
void VectorAdd(const float *in1, const float *in2, float *out);
void VectorScale(const float *in, float scale, float *out);
void VectorClear(float *v);
void CrossProduct(const float *in1, const float *in2, float *out);
float DotProduct(const float *in1, const float *in2);
bool VectorCompare(const float *in1, const float *in2);
float VectorLength(const float *v);
float VectorNormalize(float *in);
void VectorTransform(const float *in1, const float (*in2)[4], float *out);
void ConcatTransforms(float in1[3][4], float in2[3][4], float out[3][4]);
void AngleQuaternion(float *angles, vec4_t quaternion);
void QuaternionSlerp(vec4_t p, vec4_t q, float t, vec4_t qt);
void QuaternionMatrix(vec4_t quaternion, float (*matrix)[4]);
void MatrixCopy(float in[3][4], float out[3][4]);
void VectorIRotate(const vec3_t in1, const float in2[3][4], vec3_t out);

#endif