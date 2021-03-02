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

#ifndef COM_MODEL_H
#define COM_MODEL_H

#define STUDIO_RENDER	1
#define STUDIO_EVENTS	2

#define MAX_CLIENTS		32
#define	MAX_EDICTS		900

#define MAX_MODEL_NAME		64
#define MAX_MAP_HULLS		4
#define	MIPLEVELS			4
#define	NUM_AMBIENTS		4		// automatic ambient sounds
#define	MAXLIGHTMAPS		4
#define	PLANE_ANYZ			5

#define ALIAS_Z_CLIP_PLANE	5

// flags in finalvert_t.flags
#define ALIAS_LEFT_CLIP			0x0001
#define ALIAS_TOP_CLIP			0x0002
#define ALIAS_RIGHT_CLIP		0x0004
#define ALIAS_BOTTOM_CLIP		0x0008
#define ALIAS_Z_CLIP			0x0010
#define ALIAS_ONSEAM			0x0020
#define ALIAS_XY_CLIP_MASK		0x000F

struct surfcache_s;

#define	ZISCALE	( (float)0x8000 )

#define CACHE_SIZE	32		// used to align key data structures

typedef enum modtype_e
{
	mod_brush,
	mod_sprite,
	mod_alias,
	mod_studio,

} modtype_t;

#include "synctype.h"

typedef struct dmodel_s
{
	vec3_t			mins, maxs;
	vec3_t			origin;
	int				headnode[MAX_MAP_HULLS];
	int				visleafs;
	int				firstface, numfaces;

} dmodel_t;

typedef struct mplane_s
{
	// Surface normal
	vec3_t			normal;
	// Closest appoach to origin
	float			dist;
	// For texture axis selection and fast side tests
	byte			type;
	// signx + signy<<1 + signz<<1
	byte			signbits;
	byte			pad[2];

} mplane_t;

typedef struct mvertex_s
{
	vec3_t			position;

} mvertex_t;

typedef struct medge_s
{
	unsigned short	v[2];
	unsigned int	cachededgeoffset;

} medge_t;

typedef struct texture_s
{
	char			name[16];
	unsigned int	width, height;

	// Total tenths in sequence ( 0 = no)
	int				anim_total;
	// Time for this frame min <=time< max
	int				anim_min, anim_max;

	// In the animation sequence
	texture_s *		anim_next;
	// bmodels in frame 1 use these
	texture_s *		alternate_anims;

	// Four mip maps stored
	unsigned int	offsets[MIPLEVELS];
	unsigned int	paloffset;

} texture_t;

typedef struct mtexinfo_s
{
	// [s/t] unit vectors in world space
	// [i][3] is the s/t offset relative to the origin
	// s or t = dot(3Dpoint,vecs[i])+vecs[i][3]
	float			vecs[2][4];

	// ?? mipmap limits for very small surfaces
	float			mipadjust;
	texture_t *		texture;

	// sky or slime, no lightmap or 256 subdivision
	int				flags;

} mtexinfo_t;

typedef struct mnode_s
{
	// 0, to differentiate from leafs
	int				contents;
	// Node needs to be traversed if current
	int				visframe;

	// For bounding box culling
	short			minmaxs[6];

	mnode_s *		parent;

	// Node specific
	mplane_t *		plane;
	mnode_s *		children[2];

	unsigned short	firstsurface;
	unsigned short	numsurfaces;

} mnode_t;

typedef struct msurface_s msurface_t;

typedef struct decal_s
{
	// linked list for each surface
	decal_s *		pnext;

	// Surface id for persistence / unlinking
	msurface_t *	psurface;

	// Offsets into surface texture (in texture coordinates, so we don't need floats)
	short			dx;
	short			dy;

	// Decal texture
	short			texture;

	// Pixel scale
	byte			scale;

	// Decal flags
	byte			flags;

	// Entity this is attached to
	short			entityIndex;

} decal_t;

typedef struct mleaf_s
{
	// Will be a negative contents number
	int				contents;
	// Node needs to be traversed if current
	int				visframe;

	// For bounding box culling
	short			minmaxs[6];

	mnode_t *		parent;

	// Leaf specific
	byte *			compressed_vis;
	struct efrag_s *	efrags;

	msurface_t **	firstmarksurface;
	int				nummarksurfaces;

	// BSP sequence number for leaf's contents
	int				key;
	byte			ambient_sound_level[NUM_AMBIENTS];

} mleaf_t;

typedef struct msurface_s
{
	// Should be drawn when node is crossed
	int				visframe;

	// Last frame the surface was checked by an animated light
	int				dlightframe;
	// Dynamically generated. Indicates if the surface illumination is modified by an animated light
	int				dlightbits;

	// Pointer to shared plane
	mplane_t *		plane;
	// see SURF_ #defines
	int				flags;

	// Look up in model->surfedges[], negative numbers
	int				firstedge;
	// are backwards edges
	int				numedges;

	// Surface generation data
	surfcache_s *	cachespots[MIPLEVELS];

	// Smallest s/t position on the surface
	short			texturemins[2];
	// ?? s/t texture size, 1..256 for all non-sky surfaces
	short			extents[2];

	mtexinfo_t *	texinfo;

	// Index into d_lightstylevalue[] for animated lights
	// No one surface can be effected by more than 4 animated lights
	byte			styles[MAXLIGHTMAPS];
	color24 *		samples;

	decal_t *		pdecals;

} msurface_t;

typedef struct dclipnode_s
{
	int				planenum;
	short			children[2];

} dclipnode_t;

typedef struct hull_s
{
	dclipnode_t *	clipnodes;
	mplane_t *		planes;
	int				firstclipnode;
	int				lastclipnode;
	vec3_t			clip_mins;
	vec3_t			clip_maxs;

} hull_t;

#include "cache_user.h"

typedef struct model_s
{
	char			name[MAX_MODEL_NAME];
	qboolean		needload;

	modtype_t		type;
	int				numframes;
	synctype_t		synctype;

	int				flags;

	vec3_t			mins, maxs;
	float			radius;

	int				firstmodelsurface, nummodelsurfaces;

	int				numsubmodels;
	dmodel_t *		submodels;

	int				numplanes;
	mplane_t *		planes;

	int				numleafs;
	mleaf_t *		leafs;

	int				numvertexes;
	mvertex_t *		vertexes;
	int				numedges;
	medge_t *		edges;
	int				numnodes;
	mnode_t *		nodes;

	int				numtexinfo;
	mtexinfo_t *	texinfo;

	int				numsurfaces;
	msurface_t *	surfaces;

	int				numsurfedges;
	int *			surfedges;

	int				numclipnodes;
	dclipnode_t *	clipnodes;

	int				nummarksurfaces;
	msurface_t **	marksurfaces;

	hull_t			hulls[MAX_MAP_HULLS];

	int				numtextures;
	texture_t **	textures;

	byte *			visdata;

	color24 *		lightdata;

	char *			entities;

	cache_user_t	cache;

} model_t;

typedef struct alight_s
{
	int				ambientlight;
	int				shadelight;
	vec3_t			color;
	float *			plightvec;

} alight_t;

typedef struct auxvert_s
{
	float		fv[3];

} auxvert_t;

#include "custom.h"

#define MAX_INFO_STRING			256
#define MAX_SCOREBOARDNAME		32

typedef struct player_info_s
{
	// User id on server
	int				userid;

	// User info string
	char			userinfo[MAX_INFO_STRING];

	// Name
	char			name[MAX_SCOREBOARDNAME];

	// Spectator or not, unused
	int				spectator;

	int				ping;
	int				packet_loss;

	// Skin information
	char			model[MAX_QPATH];
	int				topcolor;
	int				bottomcolor;

	// Last frame rendered
	int				renderframe;

	// Gait frame estimation
	int				gaitsequence;
	float			gaitframe;
	float			gaityaw;
	vec3_t			prevgaitorigin;

	customization_t		customdata;

} player_info_t;

#endif