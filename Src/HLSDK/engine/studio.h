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

#ifndef STUDIO_H
#define STUDIO_H

#define MAXSTUDIOTRIANGLES		20000
#define MAXSTUDIOVERTS			2048
#define MAXSTUDIOSEQUENCES		256
#define MAXSTUDIOSKINS			100
#define MAXSTUDIOSRCBONES		512
#define MAXSTUDIOBONES			128
#define MAXSTUDIOMODELS			32
#define MAXSTUDIOBODYPARTS		32
#define MAXSTUDIOGROUPS			16
#define MAXSTUDIOANIMATIONS		512
#define MAXSTUDIOMESHES			256
#define MAXSTUDIOEVENTS			1024
#define MAXSTUDIOPIVOTS			256
#define MAXSTUDIOCONTROLLERS	8

typedef struct
{
	int			id;
	int			version;

	char		name[64];
	int			length;

	vec3_t		eyeposition;
	vec3_t		min;
	vec3_t		max;

	vec3_t		bbmin;
	vec3_t		bbmax;

	int			flags;

	int			numbones;
	int			boneindex;

	int			numbonecontrollers;
	int			bonecontrollerindex;

	int			numhitboxes;
	int			hitboxindex;

	int			numseq;
	int			seqindex;

	int			numseqgroups;
	int			seqgroupindex;

	int			numtextures;
	int			textureindex;
	int			texturedataindex;

	int			numskinref;
	int			numskinfamilies;
	int			skinindex;

	int			numbodyparts;
	int			bodypartindex;

	int			numattachments;
	int			attachmentindex;

	int			soundtable;
	int			soundindex;
	int			soundgroups;
	int			soundgroupindex;

	int			numtransitions;
	int			transitionindex;

} studiohdr_t;

typedef struct
{
	int			id;
	int			version;

	char		name[64];
	int			length;

} studioseqhdr_t;

typedef struct
{
	char		name[32];
	int			parent;
	int			flags;
	int			bonecontroller[6];
	float		value[6];
	float		scale[6];

} mstudiobone_t;

typedef struct
{
	int			bone;
	int			type;
	float		start;
	float		end;
	int			rest;
	int			index;

} mstudiobonecontroller_t;

typedef struct
{
	int			bone;
	int			group;
	vec3_t		bbmin;
	vec3_t		bbmax;

} mstudiobbox_t;

#include "cache_user.h"

typedef struct
{
	char			label[32];
	char			name[64];
	cache_user_t	cache;
	int				data;

} mstudioseqgroup_t;

typedef struct
{
	char		label[32];

	float		fps;
	int			flags;

	int			activity;
	int			actweight;

	int			numevents;
	int			eventindex;

	int			numframes;

	int			numpivots;
	int			pivotindex;

	int			motiontype;
	int			motionbone;
	vec3_t		linearmovement;
	int			automoveposindex;
	int			automoveangleindex;

	vec3_t		bbmin;
	vec3_t		bbmax;

	int			numblends;
	int			animindex;

	int			blendtype[2];
	float		blendstart[2];
	float		blendend[2];
	int			blendparent;

	int			seqgroup;

	int			entrynode;
	int			exitnode;
	int			nodeflags;
	
	int			nextseq;

} mstudioseqdesc_t;

#include "studio_event.h"

typedef struct
{
	vec3_t		org;
	int			start;
	int			end;

} mstudiopivot_t;

typedef struct
{
	char		name[32];
	int			type;
	int			bone;
	vec3_t		org;
	vec3_t		vectors[3];

} mstudioattachment_t;

typedef struct
{
	unsigned short	offset[6];

} mstudioanim_t;

typedef union
{
	struct {
		byte	valid;
		byte	total;

	} num;

	short	value;

} mstudioanimvalue_t;

typedef struct
{
	char		name[64];
	int			nummodels;
	int			base;
	int			modelindex;

} mstudiobodyparts_t;

typedef struct
{
	char		name[64];
	int			flags;
	int			width;
	int			height;
	int			index;

} mstudiotexture_t;

typedef struct
{
	char		name[64];

	int			type;

	float		boundingradius;

	int			nummesh;
	int			meshindex;

	int			numverts;
	int			vertinfoindex;
	int			vertindex;
	int			numnorms;
	int			norminfoindex;
	int			normindex;

	int			numgroups;
	int			groupindex;

} mstudiomodel_t;

typedef struct
{
	int			numtris;
	int			triindex;
	int			skinref;
	int			numnorms;
	int			normindex;

} mstudiomesh_t;

#define STUDIO_NF_FLATSHADE		0x0001
#define STUDIO_NF_CHROME		0x0002
#define STUDIO_NF_FULLBRIGHT	0x0004

#define STUDIO_X		0x0001
#define STUDIO_Y		0x0002
#define STUDIO_Z		0x0004
#define STUDIO_XR		0x0008
#define STUDIO_YR		0x0010
#define STUDIO_ZR		0x0020
#define STUDIO_LX		0x0040
#define STUDIO_LY		0x0080
#define STUDIO_LZ		0x0100
#define STUDIO_AX		0x0200
#define STUDIO_AY		0x0400
#define STUDIO_AZ		0x0800
#define STUDIO_AXR		0x1000
#define STUDIO_AYR		0x2000
#define STUDIO_AZR		0x4000
#define STUDIO_TYPES	0x7FFF
#define STUDIO_RLOOP	0x8000

#define STUDIO_LOOPING	0x0001

#define STUDIO_HAS_NORMALS	0x0001
#define STUDIO_HAS_VERTICES	0x0002
#define STUDIO_HAS_BBOX		0x0004
#define STUDIO_HAS_CHROME	0x0008

#endif