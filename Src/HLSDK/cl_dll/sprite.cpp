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
#include "qgl.h"
#include "AnimationScript.h"
#include "ITextureAPI.h"

typedef struct
{
	bool	active;
	vec3_t	origin;
	float	start;
	float	life;
	tex_t	*texture;
	int		rendermode;
	float	scale;
	int		script;
	int		layout;
}
cl_sprite_t;

#define MAXVISEDICTS	1024

static cl_sprite_t	visiblEdicts[ MAXVISEDICTS ];


//------------------------------------------------------------
// Purpose : 
//------------------------------------------------------------
void SPR_Create( vec3_t origin, tex_t *texture, int rendermode, int script, int layout, float scale, float life )
{
	int		i, j;

	for ( i = 0; i < MAXVISEDICTS; i++ )
	{
		if ( !visiblEdicts[ i ].active )
			break;
	}

	if ( i >= MAXVISEDICTS )
	{
		float	currenttime, rem, temp;

		currenttime = gEngfuncs.GetClientTime();
		j = -1;
		rem = 9999.0;

		for ( i = 0; i < MAXVISEDICTS; i++ )
		{
			if ( visiblEdicts[ i ].active )
			{
				temp = visiblEdicts[ i ].life - ( currenttime - visiblEdicts[ i ].start );

				if ( temp < rem )
				{
					j = i;
					rem = temp;
				}
			}
		}

		if ( j == -1 )
			return;

		i = j;
	}

	VectorCopy( origin, visiblEdicts[ i ].origin );

	visiblEdicts[ i ].active = true;
	visiblEdicts[ i ].start = gEngfuncs.GetClientTime();
	visiblEdicts[ i ].life = life;
	visiblEdicts[ i ].texture = texture;
	visiblEdicts[ i ].rendermode = rendermode;
	visiblEdicts[ i ].scale = scale;
	visiblEdicts[ i ].script = script;
	visiblEdicts[ i ].layout = layout;
}


typedef struct
{
	cl_sprite_t		*pEnt;
	float			distance;
}
transObjRef;

#define MAXTRANSOBJS	1024

static int			numTransObjs = 0;
static transObjRef	transObjects[ MAXTRANSOBJS ];

// The camera position
extern vec3_t g_CameraOrigin;
extern vec3_t g_CameraAngles;


//------------------------------------------------------------
// Purpose : Add sprite to the drawing list, 
//  it will be sorted (far to near)
//------------------------------------------------------------
void SPR_AddToList( cl_sprite_t *pEnt )
{
	int		i;
	float	dist;
	vec3_t	v;

	if ( numTransObjs >= MAXTRANSOBJS )
		return;

	VectorSubtract( g_CameraOrigin, pEnt->origin, v );
	dist = DotProduct( v, v );

	for ( i = numTransObjs; i > 0; i-- )
	{
		if ( transObjects[ i - 1 ].distance >= dist )
			break;

		transObjects[ i ].pEnt = transObjects[ i - 1 ].pEnt;
		transObjects[ i ].distance = transObjects[ i - 1 ].distance;
	}

	transObjects[ i ].pEnt = pEnt;
	transObjects[ i ].distance = dist;
	numTransObjs++;
}

//------------------------------------------------------------
// Purpose : Draw sprite in list, and remove dead
//------------------------------------------------------------
void SPR_DrawOnList( void )
{
	int		i;
	float	currenttime;

	currenttime = gEngfuncs.GetClientTime();

	for ( i = 0; i < MAXVISEDICTS; i++ )
	{
		if ( visiblEdicts[ i ].active )
		{
			if ( currenttime - visiblEdicts[ i ].start < visiblEdicts[ i ].life )
				SPR_AddToList( &visiblEdicts[ i ] );
			else
				visiblEdicts[ i ].active = false;
		}
	}

	for ( i = 0; i < numTransObjs; i++ )
	{
		cl_sprite_t		*e;
		AnimData		*panim;
		vec3_t			v_angles, forward, right, up, origin, v_origin, lt, rt, rb, lb;
		float			scale[2], angle, alpha;

		e = transObjects[ i ].pEnt;

		VectorCopy( g_CameraAngles, v_angles );

		if ( e->script != -1 && e->layout != -1 )
		{
			panim = AnimScript_GetData( e->script, e->layout, ( currenttime - e->start ) / e->life );

			if ( panim )
			{
				origin[0] = panim->vOrigin[0];
				origin[1] = panim->vOrigin[1];
				origin[2] = panim->vOrigin[2];
				scale[0] = panim->vScale[0];
				scale[1] = panim->vScale[1];
				angle = panim->fAngle;
				alpha = panim->fAlpha;
			}
		}
		else
		{
			origin[0] = 0;
			origin[1] = 0;
			origin[2] = 0;
			scale[0] = e->scale;
			scale[1] = e->scale;
			angle = 0;
			alpha = 1;
		}

		v_angles[ROLL] += angle;

		gEngfuncs.pfnAngleVectors( v_angles, forward, right, up );

		VectorCopy( e->origin, v_origin );

		VectorMA( v_origin, origin[0], forward, v_origin );
		VectorMA( v_origin, origin[1], right, v_origin );
		VectorMA( v_origin, origin[2], up, v_origin );

		VectorMA( v_origin, -e->texture->wide * scale[0], right, lt );
		VectorMA( lt, e->texture->tall * scale[1], up, lt );
		VectorMA( v_origin, e->texture->wide * scale[0], right, rt );
		VectorMA( rt, e->texture->tall * scale[1], up, rt );
		VectorMA( v_origin, e->texture->wide * scale[0], right, rb );
		VectorMA( rb, -e->texture->tall * scale[1], up, rb );
		VectorMA( v_origin, -e->texture->wide * scale[0], right, lb );
		VectorMA( lb, -e->texture->tall * scale[1], up, lb );

		qglDepthMask( GL_FALSE );

		gTexAPI.RenderMode( e->rendermode );
		gTexAPI.DrawSetColor( 255, 255, 255, 255 * alpha );
		gTexAPI.DrawSetTexture( e->texture );
		gTexAPI.DrawTexturedRect3D( rt, rb, lb, lt );

		qglDepthMask( GL_TRUE );
	}

	numTransObjs = 0;
}