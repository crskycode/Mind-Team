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
#include <ITextureAPI.h>
#include "qgl.h"

#if 0

extern vec3_t g_CameraAngles;

struct MuzzleFlash
{
	MuzzleFlash *	pNext;

	tex_t *		pTexture;
	float		flScale;

	Vector		vecOrigin;

	float		flStartTime;
	float		flHoldTime;
};

static MuzzleFlash *	g_pMuzzleFlash;


void MZF_Allocate(const float *origin, struct tex_s *texture, float scale, float life)
{
	MuzzleFlash *mzf = new MuzzleFlash;

	mzf->pTexture = texture;
	mzf->flScale = scale;
	mzf->vecOrigin = origin;
	mzf->flStartTime = gEngfuncs.GetClientTime();
	mzf->flHoldTime = life;

	mzf->pNext = g_pMuzzleFlash;
	g_pMuzzleFlash = mzf;
}

void MZF_Free(MuzzleFlash *mzf)
{
	if (mzf == g_pMuzzleFlash)
	{
		g_pMuzzleFlash = g_pMuzzleFlash->pNext;
	}
	else
	{
		MuzzleFlash *iter = g_pMuzzleFlash;

		while (iter && iter->pNext != mzf)
		{
			iter = iter->pNext;
		}

		if (iter)
		{
			MuzzleFlash *save = iter->pNext;
			iter->pNext = iter->pNext->pNext;
			delete save;
		}
	}
}

void MZF_DrawOnList(void)
{
	MuzzleFlash *mzf = g_pMuzzleFlash;

	float time = gEngfuncs.GetClientTime();

	while (mzf)
	{
		if (time > mzf->flStartTime + mzf->flHoldTime)
		{
			MuzzleFlash *save = mzf->pNext;
			MZF_Free(mzf);
			mzf = save;

			continue;
		}

		Vector forward, right, up;
		gEngfuncs.pfnAngleVectors(g_CameraAngles, forward, right, up);

		Vector p0, p1, p2, p3;

		VectorMA(mzf->vecOrigin, -mzf->pTexture->wide * mzf->flScale, right, p0);
		VectorMA(p0, mzf->pTexture->tall * mzf->flScale, up, p0);
		VectorMA(mzf->vecOrigin, mzf->pTexture->wide * mzf->flScale, right, p1);
		VectorMA(p1, mzf->pTexture->tall * mzf->flScale, up, p1);
		VectorMA(mzf->vecOrigin, mzf->pTexture->wide * mzf->flScale, right, p2);
		VectorMA(p2, -mzf->pTexture->tall * mzf->flScale, up, p2);
		VectorMA(mzf->vecOrigin, -mzf->pTexture->wide * mzf->flScale, right, p3);
		VectorMA(p3, -mzf->pTexture->tall * mzf->flScale, up, p3);

		qglDepthMask(GL_FALSE);
		qglPushAttrib(GL_VIEWPORT_BIT);
		qglDepthRange(0, 0.4);

		gTexAPI.RenderMode(RenderAdditive);
		gTexAPI.DrawSetColor(255, 255, 255, 255);
		gTexAPI.DrawSetTexture(mzf->pTexture);
		gTexAPI.DrawTexturedRect3D(p0, p1, p2, p3);

		qglDepthMask(GL_TRUE);
		qglPopAttrib();

		mzf = mzf->pNext;
	}
}

#endif