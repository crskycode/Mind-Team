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

#ifndef STUDIO_UTIL_H
#define STUDIO_UTIL_H

void ConcatTransforms(float in1[3][4], float in2[3][4], float out[3][4]);
void MatrixCopy(float in[3][4], float out[3][4]);
void QuaternionMatrix(vec4_t quaternion, float (*matrix)[4]);
void QuaternionSlerp(vec4_t p, vec4_t q, float t, vec4_t qt);
void AngleQuaternion(float *angles, vec4_t quaternion);
void VectorIRotate(const vec3_t in1, const float in2[3][4], vec3_t out);

#endif