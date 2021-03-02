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

#ifndef ANIMATIONSCRIPT_H
#define ANIMATIONSCRIPT_H

struct AnimData
{
	vec3_t	vOrigin;
	vec3_t	vScale;
	float	fAngle;
	float	fAlpha;
	int		iTexture;
	int		bLastFrame;
};

void AnimScript_LoadFile(const char *filename);

int AnimScript_FindScript(const char *script);
int AnimScript_FindLayout(int script, const char *layout);
int AnimScript_GetAnimTime(int index, int layout);
AnimData *AnimScript_GetData(int index, int layout, int time);
AnimData *AnimScript_GetData(int script, int layout, float percen);

#endif