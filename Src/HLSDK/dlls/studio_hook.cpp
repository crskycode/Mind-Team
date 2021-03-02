
#define NOVECTOR
#include "extdll.h"
#include "enginecallback.h"

#include "engine_hook.h"

#include "com_model.h"
#define FMODEL_TRACE_HITBOX	512

#include <r_studioint.h>	// Mod_Extradata

extern "C" {
#include <pm_defs.h>
}

#include <metastudio.h>


#if defined ( ENGINE_VER_3266 )

#define SV_HULLFORSTUDIOMODEL_SIG "\x83\xEC\x20\x53\x55\x8B\x6C\x24\x30\x56\x57\x8B\x7C\x24\x3C\x8D\x44\x24\x18\x33\xDB\xD9\x07\xD8\x65\x00\x50\x68"
#define PM_HULLFORSTUDIOMODEL_SIG "\x83\xEC\x0C\x8B\x0D\x2A\x2A\x2A\x2A\x8B\x81\xBC\x00\x00\x00\x8D\x04\x40\xC1\xE0\x02\xD9\x80\x7C\xF3\xEC\x01\xD8\xA0\x4C\xF3\xEC\x01"

#endif


void GL_VectorSlerp(vec3_t v1, vec3_t v2, float t, vec3_t dest)
{
	int i;

	for (i = 0; i < 3; i++)
	{
		dest[i] = v1[i] + t * (v2[i] - v1[i]);
	}
}

void GL_QuaternionSlerp(vec4_t q1, vec4_t q2, float t, vec4_t out)
{
	float	rot1q[4];
	float	omega, cosom, oosinom;
	float	scalerot0, scalerot1;

	cosom = q1[0] * q2[0] + q1[1] * q2[1] + q1[2] * q2[2] + q1[3] * q2[3];

	if (cosom < 0.0f)
	{
		cosom = -cosom;
		rot1q[0] = -q2[0];
		rot1q[1] = -q2[1];
		rot1q[2] = -q2[2];
		rot1q[3] = -q2[3];
	}
	else
	{
		rot1q[0] = q2[0];
		rot1q[1] = q2[1];
		rot1q[2] = q2[2];
		rot1q[3] = q2[3];
	}

	if ((1.0f - cosom) > 0.0001f)
	{
		omega = acos(cosom);
		oosinom = 1.0f / sin(omega);
		scalerot0 = sin((1.0f - t) * omega) * oosinom;
		scalerot1 = sin(t * omega) * oosinom;
	}
	else
	{
		scalerot0 = 1.0f - t;
		scalerot1 = t;
	}

	out[0] = (scalerot0 * q1[0] + scalerot1 * rot1q[0]);
	out[1] = (scalerot0 * q1[1] + scalerot1 * rot1q[1]);
	out[2] = (scalerot0 * q1[2] + scalerot1 * rot1q[2]);
	out[3] = (scalerot0 * q1[3] + scalerot1 * rot1q[3]);
}

void GL_AngleMatrix(vec3_t angles, mat4_t matrix)
{
	float angle;
	float sr, sp, sy, cr, cp, cy;

	angle = angles[1] * (M_PI * 2 / 360);	// YAW
	sy = sin(angle);
	cy = cos(angle);
	angle = angles[0] * (M_PI * 2 / 360);	// PITCH
	sp = sin(angle);
	cp = cos(angle);
	angle = angles[2] * (M_PI * 2 / 360);	// ROLL
	sr = sin(angle);
	cr = cos(angle);

	matrix[0][0] = cp * cy;
	matrix[0][1] = cp * sy;
	matrix[0][2] = -sp;

	matrix[1][0] = sr * sp*cy + cr * -sy;
	matrix[1][1] = sr * sp*sy + cr * cy;
	matrix[1][2] = sr * cp;

	matrix[2][0] = (cr*sp*cy + -sr * -sy);
	matrix[2][1] = (cr*sp*sy + -sr * cy);
	matrix[2][2] = cr * cp;

	matrix[3][0] = matrix[3][1] = matrix[3][2] = matrix[0][3] = matrix[1][3] = matrix[2][3] = 0.0;
	matrix[3][3] = 1.0;
}

void GL_QuaternionMatrix(vec4_t q, mat4_t m)
{
	float s, xs, ys, zs, wx, wy, wz, xx, xy, xz, yy, yz, zz;

	s = 2.0 / (q[0] * q[0] + q[1] * q[1] + q[2] * q[2] + q[3] * q[3]);

	xs = q[0] * s;
	ys = q[1] * s;
	zs = q[2] * s;

	wx = q[3] * xs;
	wy = q[3] * ys;
	wz = q[3] * zs;

	xx = q[0] * xs;
	xy = q[0] * ys;
	xz = q[0] * zs;

	yy = q[1] * ys;
	yz = q[1] * zs;

	zz = q[2] * zs;

	m[0][0] = 1.0 - (yy + zz);
	m[1][0] = xy - wz;
	m[2][0] = xz + wy;

	m[0][1] = xy + wz;
	m[1][1] = 1.0 - (xx + zz);
	m[2][1] = yz - wx;

	m[0][2] = xz - wy;
	m[1][2] = yz + wx;
	m[2][2] = 1.0 - (xx + yy);

	m[3][0] = m[3][1] = m[3][2] = m[0][3] = m[1][3] = m[2][3] = 0.0;
	m[3][3] = 1.0;
}

void GL_ConcatTransforms(mat4_t m1, mat4_t m2, mat4_t out)
{
	out[0][0] = m1[0][0] * m2[0][0] + m1[1][0] * m2[0][1] + m1[2][0] * m2[0][2] + m1[3][0] * m2[0][3];
	out[0][1] = m1[0][1] * m2[0][0] + m1[1][1] * m2[0][1] + m1[2][1] * m2[0][2] + m1[3][1] * m2[0][3];
	out[0][2] = m1[0][2] * m2[0][0] + m1[1][2] * m2[0][1] + m1[2][2] * m2[0][2] + m1[3][2] * m2[0][3];
	out[0][3] = m1[0][3] * m2[0][0] + m1[1][3] * m2[0][1] + m1[2][3] * m2[0][2] + m1[3][3] * m2[0][3];

	out[1][0] = m1[0][0] * m2[1][0] + m1[1][0] * m2[1][1] + m1[2][0] * m2[1][2] + m1[3][0] * m2[1][3];
	out[1][1] = m1[0][1] * m2[1][0] + m1[1][1] * m2[1][1] + m1[2][1] * m2[1][2] + m1[3][1] * m2[1][3];
	out[1][2] = m1[0][2] * m2[1][0] + m1[1][2] * m2[1][1] + m1[2][2] * m2[1][2] + m1[3][2] * m2[1][3];
	out[1][3] = m1[0][3] * m2[1][0] + m1[1][3] * m2[1][1] + m1[2][3] * m2[1][2] + m1[3][3] * m2[1][3];

	out[2][0] = m1[0][0] * m2[2][0] + m1[1][0] * m2[2][1] + m1[2][0] * m2[2][2] + m1[3][0] * m2[2][3];
	out[2][1] = m1[0][1] * m2[2][0] + m1[1][1] * m2[2][1] + m1[2][1] * m2[2][2] + m1[3][1] * m2[2][3];
	out[2][2] = m1[0][2] * m2[2][0] + m1[1][2] * m2[2][1] + m1[2][2] * m2[2][2] + m1[3][2] * m2[2][3];
	out[2][3] = m1[0][3] * m2[2][0] + m1[1][3] * m2[2][1] + m1[2][3] * m2[2][2] + m1[3][3] * m2[2][3];

	out[3][0] = m1[0][0] * m2[3][0] + m1[1][0] * m2[3][1] + m1[2][0] * m2[3][2] + m1[3][0] * m2[3][3];
	out[3][1] = m1[0][1] * m2[3][0] + m1[1][1] * m2[3][1] + m1[2][1] * m2[3][2] + m1[3][1] * m2[3][3];
	out[3][2] = m1[0][2] * m2[3][0] + m1[1][2] * m2[3][1] + m1[2][2] * m2[3][2] + m1[3][2] * m2[3][3];
	out[3][3] = m1[0][3] * m2[3][0] + m1[1][3] * m2[3][1] + m1[2][3] * m2[3][2] + m1[3][3] * m2[3][3];
}

mat4_t	bonetransform[META_STUDIO_BONES];


void MetaStudio_GetKeyFrame(metastudiohdr_t *pmetahdr, metastudioanim_t *panim, int index, vec3_t *pos, vec4_t *quat)
{
	int							i;
	metastudioanimvalue_t *		pv;

	pv = (metastudioanimvalue_t *)((byte *)pmetahdr + panim->valueindex);

	pv += index * panim->numbones;

	for (i = 0; i < panim->numbones; i++)
	{
		VectorCopy(pv[i].pos, pos[i]);
		VectorCopy4(pv[i].quat, quat[i]);
	}
}

void MetaStudio_CalcRotations(metastudiohdr_t *pmetahdr, metastudioanim_t *panim, int frame, vec3_t *pos, vec4_t *quat)
{
	int				i, j;
	int *			time;
	int				start, end;
	int				curr, next;
	int				t1, t2;
	float			t;

	static vec3_t	pos1[META_STUDIO_BONES];
	static vec4_t	quat1[META_STUDIO_BONES];
	static vec3_t	pos2[META_STUDIO_BONES];
	static vec4_t	quat2[META_STUDIO_BONES];

	time = (int *)((byte *)pmetahdr + panim->timeindex);

	start = time[0];
	end = time[panim->numkeyframes - 1];

	if (frame < start)
		frame = start;
	else if (frame > end)
		frame = end;

	for (i = 0; i < panim->numkeyframes - 1; i++)
	{
		curr = i;
		next = i + 1;

		if (frame == time[curr])
		{
			MetaStudio_GetKeyFrame(pmetahdr, panim, curr, pos, quat);
			return;
		}
		else if (frame == time[next])
		{
			MetaStudio_GetKeyFrame(pmetahdr, panim, next, pos, quat);
			return;
		}
		else if (time[curr] < frame && frame < time[next])
		{
			MetaStudio_GetKeyFrame(pmetahdr, panim, curr, pos1, quat1);
			MetaStudio_GetKeyFrame(pmetahdr, panim, next, pos2, quat2);

			t1 = time[curr];
			t2 = time[next];

			t = (float)(frame - t1) / (float)(t2 - t1);

			for (j = 0; j < panim->numbones; j++)
			{
				GL_VectorSlerp(pos1[j], pos2[j], t, pos[j]);
				GL_QuaternionSlerp(quat1[j], quat2[j], t, quat[j]);
			}

			return;
		}
	}
}

void MetaStudio_SetupBones(metastudiohdr_t *pmetahdr, vec3_t origin, vec3_t angles, int sequence, float frame)
{
	int						i;
	metastudioseqdesc_t *	pseqdesc;
	metastudioanim_t *		panim;
	float					f;
	metastudiobone_t *		pbone;
	mat4_t					rotatematrix;

	static vec3_t	pos[META_STUDIO_BONES];
	static vec4_t	quat[META_STUDIO_BONES];

	pseqdesc = (metastudioseqdesc_t *)((byte *)pmetahdr + pmetahdr->seqindex) + sequence;

	panim = (metastudioanim_t *)((byte *)pmetahdr + pseqdesc->animindex);

	if (frame < 0)
		frame = 0;
	else if (frame > 256)
		frame = 256;

	f = frame / 256.0 * pseqdesc->duration;

	MetaStudio_CalcRotations(pmetahdr, panim, f, pos, quat);

	pbone = (metastudiobone_t *)((byte *)pmetahdr + pmetahdr->boneindex);

	GL_AngleMatrix(angles, rotatematrix);
	VectorCopy(origin, rotatematrix[3]);

	for (i = 0; i < pmetahdr->numbones; i++)
	{
		mat4_t	m;
		int		parent;

		GL_QuaternionMatrix(quat[i], m);
		VectorCopy(pos[i], m[3]);

		parent = pbone[i].parent;

		if (parent == -1)
		{
			GL_ConcatTransforms(rotatematrix, m, bonetransform[i]);
		}
		else
		{
			GL_ConcatTransforms(bonetransform[parent], m, bonetransform[i]);
		}
	}
}

#define MAX_STUDIO_HULLS	128

int				cache_hull_hitgroup[MAX_STUDIO_HULLS];
hull_t			cache_hull[MAX_STUDIO_HULLS];
mplane_t		cache_planes[MAX_STUDIO_HULLS * 6];
int				nCurrentHull;
int				nCurrentPlane;

typedef struct
{
	metastudiohdr_t *	pModel;
	float				frame;
	int					sequence;
	vec3_t				origin;
	vec3_t				angles;
	vec3_t				size;

	int					nStartHull;
	int					nStartPlane;
	int					numhulls;

} r_studiocache_t;

#define MAX_STUDIO_CACHES 32
r_studiocache_t rgStudioCache[MAX_STUDIO_CACHES];
int r_cachecurrent;

bool studio_hull_cache_initialized = false;


int				studio_hull_hitgroup[MAX_STUDIO_HULLS];
hull_t			studio_hull[MAX_STUDIO_HULLS];
dclipnode_t		studio_clipnodes[6];
mplane_t		studio_planes[MAX_STUDIO_HULLS * 6];

bool studio_hull_initialized = false;


void R_InitStudioCache(void)
{
	memset(rgStudioCache, 0, sizeof(rgStudioCache));
	r_cachecurrent = 0;

	memset(cache_hull_hitgroup, 0, sizeof(cache_hull_hitgroup));
	memset(cache_hull, 0, sizeof(cache_hull));
	memset(cache_planes, 0, sizeof(cache_planes));
	nCurrentHull = 0;
	nCurrentPlane = 0;
}

r_studiocache_t *R_CheckStudioCache(metastudiohdr_t *pModel, float frame, int sequence, vec3_t angles, vec3_t origin, vec3_t size)
{
	int				i;
	r_studiocache_t	*pCached;

	if (!studio_hull_cache_initialized)
	{
		R_InitStudioCache();

		studio_hull_cache_initialized = true;

		return NULL;
	}

	for (i = 0; i < MAX_STUDIO_CACHES; i++)
	{
		pCached = &rgStudioCache[(r_cachecurrent - i) & (MAX_STUDIO_CACHES - 1)];

		if (pCached->pModel != pModel)
			continue;

		if (pCached->frame != frame)
			continue;

		if (pCached->sequence != sequence)
			continue;

		if (!VectorCompare(pCached->origin, origin))
			continue;

		if (!VectorCompare(pCached->angles, angles))
			continue;

		if (!VectorCompare(pCached->size, size))
			continue;

		return pCached;
	}

	return NULL;
}

void R_AddToStudioCache(metastudiohdr_t *pmetahdr, float frame, int sequence, vec3_t angles, vec3_t origin, vec3_t size, hull_t *pHulls, int numhulls)
{
	r_studiocache_t	*p;

	if (numhulls + nCurrentHull >= MAX_STUDIO_HULLS)
	{
		R_InitStudioCache();
		return;
	}

	r_cachecurrent++;
	p = &rgStudioCache[r_cachecurrent & (MAX_STUDIO_CACHES - 1)];

	p->pModel = pmetahdr;
	p->frame = frame;
	p->sequence = sequence;

	VectorCopy(angles, p->angles);
	VectorCopy(origin, p->origin);
	VectorCopy(size, p->size);

	p->nStartPlane = nCurrentPlane;
	p->nStartHull = nCurrentHull;

	memcpy(&cache_hull[nCurrentHull], pHulls, numhulls * sizeof(hull_t));
	memcpy(&cache_planes[nCurrentPlane], studio_planes, numhulls * sizeof(mplane_t) * 6);
	memcpy(&cache_hull_hitgroup[nCurrentHull], studio_hull_hitgroup, numhulls * sizeof(int));

	p->numhulls = numhulls;

	nCurrentHull += numhulls;
	nCurrentPlane += numhulls * 6;
}

void SV_InitStudioHull(void)
{
	int		i;
	int		side;

	if (studio_hull_initialized)
		return;

	memset(studio_hull_hitgroup, 0, sizeof(studio_hull_hitgroup));
	memset(studio_hull, 0, sizeof(studio_hull));
	memset(studio_clipnodes, 0, sizeof(studio_clipnodes));
	memset(studio_planes, 0, sizeof(studio_planes));

	studio_hull_initialized = true;

	for (i = 0; i<6; i++)
	{
		studio_clipnodes[i].planenum = i;

		side = i & 1;

		studio_clipnodes[i].children[side] = CONTENTS_EMPTY;
		if (i != 5)
			studio_clipnodes[i].children[side ^ 1] = i + 1;
		else
			studio_clipnodes[i].children[side ^ 1] = CONTENTS_SOLID;
	}

	for (i = 0; i < MAX_STUDIO_HULLS; i++)
	{
		studio_hull[i].clipnodes = studio_clipnodes;
		studio_hull[i].planes = &studio_planes[i * 6];
		studio_hull[i].firstclipnode = 0;
		studio_hull[i].lastclipnode = 5;
	}
}

void SV_SetStudioHullPlane(mplane_t *pplane, int bone, int k, float dist)
{
	pplane->type = 5;

	pplane->normal[0] = bonetransform[bone][k][0];
	pplane->normal[1] = bonetransform[bone][k][1];
	pplane->normal[2] = bonetransform[bone][k][2];

	pplane->dist = (pplane->normal[0] * bonetransform[bone][3][0]) + (pplane->normal[1] * bonetransform[bone][3][1]) + (pplane->normal[2] * bonetransform[bone][3][2]) + dist;
}

hull_t *R_StudioHull(metastudiohdr_t *pmetahdr, float frame, int sequence, vec3_t origin, vec3_t angles, vec3_t size, int *pNumHulls)
{
	int						i, j;
	metastudiohitbox_t *	pbbox;
	r_studiocache_t *		pCached;
	
	SV_InitStudioHull();

	/**
	 * We may have calculated a same frame for this model. check if it's in the cache,
	 * avoid duplicate calculation.
	**/
	pCached = R_CheckStudioCache(pmetahdr, frame, sequence, angles, origin, size);

	if (pCached)
	{
		memcpy(studio_planes, &cache_planes[pCached->nStartPlane], pCached->numhulls * sizeof(mplane_t) * 6);
		memcpy(studio_hull, &cache_hull[pCached->nStartHull], pCached->numhulls * sizeof(hull_t));
		memcpy(studio_hull_hitgroup, &cache_hull_hitgroup[pCached->nStartHull], pCached->numhulls * sizeof(int));

		*pNumHulls = pCached->numhulls;

		return studio_hull;
	}

	MetaStudio_SetupBones(pmetahdr, origin, angles, sequence, frame);
	
	pbbox = (metastudiohitbox_t *)((byte *)pmetahdr + pmetahdr->hitboxindex);

	for (i = 0, j = 0; i < pmetahdr->numhitboxes; i++, j += 6)
	{
		studio_hull_hitgroup[i] = pbbox[i].group;

		SV_SetStudioHullPlane(&studio_planes[j + 0], pbbox[i].bone, 0, pbbox[i].bmax[0]);
		SV_SetStudioHullPlane(&studio_planes[j + 1], pbbox[i].bone, 0, pbbox[i].bmin[0]);
		SV_SetStudioHullPlane(&studio_planes[j + 2], pbbox[i].bone, 1, pbbox[i].bmax[1]);
		SV_SetStudioHullPlane(&studio_planes[j + 3], pbbox[i].bone, 1, pbbox[i].bmin[1]);
		SV_SetStudioHullPlane(&studio_planes[j + 4], pbbox[i].bone, 2, pbbox[i].bmax[2]);
		SV_SetStudioHullPlane(&studio_planes[j + 5], pbbox[i].bone, 2, pbbox[i].bmin[2]);

		studio_planes[j + 0].dist += abs(studio_planes[j + 0].normal[0] * size[0])
								   + abs(studio_planes[j + 0].normal[1] * size[1])
								   + abs(studio_planes[j + 0].normal[2] * size[2]);

		studio_planes[j + 1].dist -= abs(studio_planes[j + 1].normal[0] * size[0])
								   + abs(studio_planes[j + 1].normal[1] * size[1])
								   + abs(studio_planes[j + 1].normal[2] * size[2]);

		studio_planes[j + 2].dist += abs(studio_planes[j + 2].normal[0] * size[0])
								   + abs(studio_planes[j + 2].normal[1] * size[1])
								   + abs(studio_planes[j + 2].normal[2] * size[2]);

		studio_planes[j + 3].dist -= abs(studio_planes[j + 3].normal[0] * size[0])
								   + abs(studio_planes[j + 3].normal[1] * size[1])
								   + abs(studio_planes[j + 3].normal[2] * size[2]);

		studio_planes[j + 4].dist += abs(studio_planes[j + 4].normal[0] * size[0])
								   + abs(studio_planes[j + 4].normal[1] * size[1])
								   + abs(studio_planes[j + 4].normal[2] * size[2]);

		studio_planes[j + 5].dist -= abs(studio_planes[j + 5].normal[0] * size[0])
								   + abs(studio_planes[j + 5].normal[1] * size[1])
								   + abs(studio_planes[j + 5].normal[2] * size[2]);
	}

	/**
	 * We have calculated a frame for this model. Add to the cache.
	**/
	R_AddToStudioCache(pmetahdr, frame, sequence, angles, origin, size, studio_hull, pmetahdr->numhitboxes);

	*pNumHulls = pmetahdr->numhitboxes;

	return studio_hull;
}

static vec3_t vec3_origin = { 0, 0, 0 };

extern globalvars_t *gpGlobals;

extern server_studio_api_t IEngineStudio;

extern "C" {
	extern playermove_t *pmove;
}

#if defined ( ENGINE_VER_3266 )

vec3_t *player_mins = (vec3_t *)0x01ECF34C;
vec3_t *player_maxs = (vec3_t *)0x01ECF37C;

#endif

void *g_pfn_SV_HullForStudioModel;
void *g_pfn_PM_HullForStudioModel;


hull_t *SV_HullForStudioModel(edict_t *pEdict, vec3_t mins, vec3_t maxs, vec3_t offset, int *pNumHulls)
{
	bool				useComplexHull;
	vec3_t				size;
	float				factor;
	metastudiohdr_t *	pmetahdr;

	VectorSubtract(maxs, mins, size);

	useComplexHull = false;
	factor = 0.5;

	if (VectorCompare(vec3_origin, size) && !(gpGlobals->trace_flags & FTRACE_SIMPLEBOX))
	{
		useComplexHull = true;

		if (pEdict->v.flags & FL_CLIENT)
		{
			size[0] = size[1] = size[2] = 1;
		}
	}

	VectorScale(size, factor, size);

	VectorClear(offset);

	pmetahdr = (metastudiohdr_t *)GET_MODEL_PTR(pEdict);

	if (!(pmetahdr->flags & META_STUDIO))
	{
		*pNumHulls = 0;
		return NULL;
	}

	return R_StudioHull(pmetahdr, pEdict->v.frame, pEdict->v.sequence, pEdict->v.origin, pEdict->v.angles, size, pNumHulls);
}

hull_t *PM_HullForStudioModel(model_t *pModel, vec3_t offset, float frame, int sequence, vec3_t angles, vec3_t origin, byte *pcontroller, byte *pblending, int *pNumHulls)
{
	vec3_t				size;
	metastudiohdr_t *	pmetahdr;

	pmetahdr = (metastudiohdr_t *)IEngineStudio.Mod_Extradata(pModel);

	if (pmetahdr == NULL)
	{
		*pNumHulls = 0;
		return studio_hull;
	}

	VectorSubtract(player_maxs[pmove->usehull], player_mins[pmove->usehull], size);
	VectorScale(size, 0.5, size);

	VectorClear(offset);

	return R_StudioHull(pmetahdr, frame, sequence, origin, angles, size, pNumHulls);
}

void StudioHook_Install(void)
{
	g_pfn_SV_HullForStudioModel = EngineHook_SearchPattern(SV_HULLFORSTUDIOMODEL_SIG, sizeof(SV_HULLFORSTUDIOMODEL_SIG) - 1);
	g_pfn_PM_HullForStudioModel = EngineHook_SearchPattern(PM_HULLFORSTUDIOMODEL_SIG, sizeof(PM_HULLFORSTUDIOMODEL_SIG) - 1);

	if (g_pfn_SV_HullForStudioModel)
	{
		EngineHook_InlineHook(g_pfn_SV_HullForStudioModel, SV_HullForStudioModel, &g_pfn_SV_HullForStudioModel);
		g_engfuncs.pfnServerPrint("SV_HullForStudioModel: Install Hook\n");
	}
	if (g_pfn_PM_HullForStudioModel)
	{
		EngineHook_InlineHook(g_pfn_PM_HullForStudioModel, PM_HullForStudioModel, &g_pfn_PM_HullForStudioModel);
		g_engfuncs.pfnServerPrint("PM_HullForStudioModel: Install Hook\n");
	}
}