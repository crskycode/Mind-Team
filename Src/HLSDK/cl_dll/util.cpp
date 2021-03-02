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

#include "cl_dll.h"
#include "encode.h"

float UTIL_AngleMod(float angle)
{
	angle = fmod(angle, 360.0f);
	
	if (angle < 0)
	{
		angle += 360;
	}

	return angle;
}

float UTIL_AngleDistance(float next, float cur)
{
	float delta = next - cur;

	if (delta < -180)
		delta += 360;
	else if (delta > 180)
		delta -= 360;

	return delta;
}

float UTIL_AngleDistanceAbs(float next, float cur)
{
	float delta = next - cur;

	if (delta < -180)
		delta += 360;
	else if (delta > 180)
		delta -= 360;

	return fabs(delta);
}

float UTIL_ApproachAngle(float target, float value, float speed)
{
	target = UTIL_AngleMod(target);
	value = UTIL_AngleMod(value);
	
	float delta = target - value;
	
	if (speed < 0)
		speed = -speed;

	if (delta < -180)
		delta += 360;
	else if (delta > 180)
		delta -= 360;
	
	if (delta > speed)
		value += speed;
	else if (delta < -speed)
		value -= speed;
	else
		value = target;

	return value;
}

void UTIL_Rotate2D(vec2_t point, vec2_t origin, float angle)
{
	vec2_t xy;
	float rad = angle * (M_PI / 180.0);

	xy[0] = (point[0] - origin[0])*cos(rad) - (point[1] - origin[1])*sin(rad) + origin[0];
	xy[1] = (point[0] - origin[0])*sin(rad) + (point[1] - origin[1])*cos(rad) + origin[1];

	// copy value
	point[0] = xy[0];
	point[1] = xy[1];
}

bool UTIL_IsPlayerBot(int playerNum)
{
	const char *info = gEngfuncs.PlayerInfo_ValueForKey(playerNum, "*bot");

	if (info && *info == '1')
		return true;

	return false;
}

void UTIL_ANSIToUnicode(wchar_t *buffer, const char *source)
{
	wcscpy(buffer, ANSIToUnicode(source));
}

void UTIL_UTF8ToUnicode(wchar_t *buffer, const char *source)
{
	wcscpy(buffer, UTF8ToUnicode(source));
}