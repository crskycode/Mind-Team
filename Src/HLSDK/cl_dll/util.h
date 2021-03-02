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

#ifndef UTIL_H
#define UTIL_H

float UTIL_AngleMod(float angle);
float UTIL_AngleDistance(float next, float cur);
float UTIL_AngleDistanceAbs(float next, float cur);
float UTIL_ApproachAngle(float target, float value, float speed);
void UTIL_Rotate2D(vec2_t point, vec2_t origin, float angle);
bool UTIL_IsPlayerBot(int playerNum);
void UTIL_ANSIToUnicode(wchar_t *buffer, const char *source);
void UTIL_UTF8ToUnicode(wchar_t *buffer, const char *source);

#endif