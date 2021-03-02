#include <metahook.h>
#include <cvardef.h>
#include <com_model.h>

#include "DrawTextures.h"
#include "util_vector.h"
#include "sys.h"

static cl_entity_t *cl_entities = NULL;
static cvar_t *r_decals = NULL;

#define	SURF_PLANEBACK		2
#define	SURF_DRAWSKY		4
#define SURF_DRAWSPRITE		8
#define SURF_DRAWTURB		0x10
#define SURF_DRAWTILED		0x20
#define SURF_DRAWBACKGROUND	0x40
#define SURF_UNDERWATER		0x80
#define SURF_DONTWARP		0x100

#define FDECAL_PERMANENT	0x01
#define FDECAL_REFERENCE	0x02
#define FDECAL_CUSTOM		0x04
#define FDECAL_HFLIP		0x08
#define FDECAL_VFLIP		0x10
#define FDECAL_CLIPTEST		0x20
#define FDECAL_NOCLIP		0x40

#define MAX_DECALS	300

static decal_t		gDecalPool[ MAX_DECALS ];
static int			gDecalCount;
static vec3_t		gDecalPos;
static model_t *	gDecalModel;
static tex_t *		gDecalTexture;
static int			gDecalIndex;
static int			gDecalSize;
static int			gDecalFlags;
static int			gDecalEntity;


#define DECAL_DISTANCE	4


typedef struct decalcache_s
{
	int	decalIndex;
	vec3_t	decalVert[4];
}
decalcache_t;


#define DECAL_CACHEENTRY	256
decalcache_t	gDecalCache[ DECAL_CACHEENTRY ];


void R_DecalInit( void )
{
	int i;

	memset( gDecalPool, 0, sizeof( gDecalPool ) );
	gDecalCount = 0;

	for( i = 0; i < DECAL_CACHEENTRY; i++ )
		gDecalCache[i].decalIndex = -1;

	r_decals = gEngfuncs.pfnGetCvarPointer("r_decals");
}

int R_DecalIndex( decal_t *pdecal )
{
	return ( pdecal - gDecalPool );
}

int R_DecalCacheIndex( int index )
{
	return index & (DECAL_CACHEENTRY-1);
}

decalcache_t *R_DecalCacheSlot( int decalIndex )
{
	int				cacheIndex;

	cacheIndex = R_DecalCacheIndex( decalIndex );	// Find the cache slot

	return gDecalCache + cacheIndex;
}

// Release the cache entry for this decal
void R_DecalCacheClear( decal_t *pdecal )
{
	int				index;
	decalcache_t	*pCache;

	index = R_DecalIndex( pdecal );
	pCache = R_DecalCacheSlot( index );		// Find the cache slot

	if ( pCache->decalIndex == index )		// If this is the decal that's cached here, clear it.
		pCache->decalIndex = -1;
}

// Unlink pdecal from any surface it's attached to
void R_DecalUnlink( decal_t *pdecal )
{
	decal_t *tmp;

	R_DecalCacheClear( pdecal );
	if ( pdecal->psurface )
	{
		if ( pdecal->psurface->pdecals == pdecal )
		{
			pdecal->psurface->pdecals = pdecal->pnext;
		}
		else 
		{
			tmp = pdecal->psurface->pdecals;
			if ( !tmp )
				Sys_Error("Bad decal list");
			while ( tmp->pnext ) 
			{
				if ( tmp->pnext == pdecal ) 
				{
					tmp->pnext = pdecal->pnext;
					break;
				}
				tmp = tmp->pnext;
			}
		}
	}

	pdecal->psurface = NULL;
}


// Just reuse next decal in list
// A decal that spans multiple surfaces will use multiple decal_t pool entries, as each surface needs
// it's own.
decal_t *R_DecalAlloc( void )
{
	decal_t	*pdecal;
	int limit = MAX_DECALS;

	if ( r_decals->value < limit )
		limit = r_decals->value;
	
	if ( !limit )
		return NULL;

	int count = 0;

	do 
	{
		gDecalCount++;
		if ( gDecalCount >= limit )
			gDecalCount = 0;
		pdecal = gDecalPool + gDecalCount;
		count++;
	} while ( ( pdecal->flags & FDECAL_PERMANENT ) && count < limit );

	
	// If decal is already linked to a surface, unlink it.
	R_DecalUnlink( pdecal );

	return pdecal;	
}

void R_DecalRemoveAll( int textureIndex )
{
	int		i;
	decal_t	*pDecal;

	for( i = 0; i < MAX_DECALS; i++ )
	{
		pDecal = &gDecalPool[i];

		if ( pDecal->texture == textureIndex )
		{
			R_DecalUnlink( pDecal );
			memset( pDecal, 0, sizeof( decal_t ) );
		}
	}
}

void R_DecalRemoveNonPermanent( int textureIndex )
{
	int		i;
	decal_t	*pDecal;

	for( i = 0; i < MAX_DECALS; i++ )
	{
		pDecal = &gDecalPool[i];

		if ( pDecal->texture == textureIndex && (pDecal->flags & FDECAL_PERMANENT) )
		{
			R_DecalUnlink( pDecal );
			memset( pDecal, 0, sizeof( decal_t ) );
		}
	}
}

void R_DecalRemoveWithFlag( int flags )
{
	int		i;
	decal_t	*pDecal;

	for( i = 0; i < MAX_DECALS; i++ )
	{
		pDecal = &gDecalPool[i];

		if ( pDecal->flags & flags )
		{
			R_DecalUnlink( pDecal );
			memset( pDecal, 0, sizeof( decal_t ) );
		}
	}
}

void R_DecalCreate( msurface_t *psurface, int textureIndex, float scale, float x, float y )
{
	decal_t			*pdecal, *pold;
	int				count;

	pdecal = R_DecalAlloc();
	
	pdecal->flags = gDecalFlags;
	pdecal->dx = x;
	pdecal->dy = y;
	pdecal->texture = textureIndex;
	pdecal->pnext = NULL;

	if ( psurface->pdecals ) 
	{
		pold = psurface->pdecals;
		while ( pold->pnext ) 
			pold = pold->pnext;
		pold->pnext = pdecal;
	}
	else
	{
		psurface->pdecals = pdecal;
	}

	// Tag surface
	pdecal->psurface = psurface;

	// Set scaling
	pdecal->scale = scale;
	pdecal->entityIndex = gDecalEntity;
}

void R_DecalNode( mnode_t *node, float flScale )
{
	mplane_t	*splitplane;
	float		dist;

	if (!node )
		return;
	if ( node->contents < 0 )
		return;

	splitplane = node->plane;
	dist = DotProduct(gDecalPos, splitplane->normal) - splitplane->dist;

	// This is arbitrarily set to 10 right now.  In an ideal world we'd have the 
	// exact surface but we don't so, this tells me which planes are "sort of 
	// close" to the gunshot -- the gunshot is actually 4 units in front of the 
	// wall (see dlls\weapons.cpp). We also need to check to see if the decal 
	// actually intersects the texture space of the surface, as this method tags
	// parallel surfaces in the same node always.
	// JAY: This still tags faces that aren't correct at edges because we don't 
	// have a surface normal

	if (dist > gDecalSize)
	{
		R_DecalNode (node->children[0], flScale);
	}
	else if (dist < -gDecalSize)
	{
		R_DecalNode (node->children[1], flScale);
	}
	else 
	{
		if ( dist < DECAL_DISTANCE && dist > -DECAL_DISTANCE )
		{
			int			w, h;
			float		s, t, scale;
			msurface_t	*surf;
			int			i;
			mtexinfo_t	*tex;
			vec3_t		cross, normal;
			float		face;

			surf = gDecalModel->surfaces + node->firstsurface;

			// iterate over all surfaces in the node
			for ( i=0; i<node->numsurfaces ; ++i, ++surf)
			{
				if ( (surf->flags & (SURF_DRAWTILED | SURF_DRAWTURB)) || ( (surf->flags & SURF_DONTWARP) && (gDecalFlags & FDECAL_CUSTOM) ) )
					continue;

				tex = surf->texinfo;

				if ( flScale == -1 )
					scale = Length( tex->vecs[0] );
				else
					scale = flScale;

				if ( scale == 0 )
					continue;

				s = DotProduct( gDecalPos, tex->vecs[0] ) + tex->vecs[0][3] - surf->texturemins[0];
				t = DotProduct( gDecalPos, tex->vecs[1] ) + tex->vecs[1][3] - surf->texturemins[1];

				w = gDecalTexture->wide * scale;
				h = gDecalTexture->tall * scale;

				// move s,t to upper left corner
				s -= ( w * 0.5 );
				t -= ( h * 0.5 );

				if ( s <= -w || t <= -h || s > (surf->extents[0]+w) || t > (surf->extents[1]+h) )
				{
					continue; // nope
				}

				scale = 1.0 / scale;
				s = ( s + surf->texturemins[0] ) / (float)tex->texture->width;
				t = ( t + surf->texturemins[1] ) / (float)tex->texture->height;

				if ( gDecalFlags & FDECAL_CUSTOM )
				{
					if ( surf->flags & SURF_PLANEBACK )
						VectorScale( surf->plane->normal, -1, normal );
					else
						VectorCopy( surf->plane->normal, normal );

					CrossProduct( surf->texinfo->vecs[0], normal, cross );
					face = DotProduct( cross, surf->texinfo->vecs[1] );

					if ( face < 0 )
						gDecalFlags |= FDECAL_HFLIP;
					else
						gDecalFlags &= ~FDECAL_HFLIP;

					CrossProduct( surf->texinfo->vecs[1], normal, cross );
					face = DotProduct( cross, surf->texinfo->vecs[0] );

					if ( gDecalFlags & FDECAL_HFLIP )
						face = -face;

					if ( face > 0 )
						gDecalFlags |= FDECAL_VFLIP;
					else
						gDecalFlags &= ~FDECAL_VFLIP;
				}

				// stamp it
				R_DecalCreate( surf, gDecalIndex, scale, s, t );
			}
		}

		R_DecalNode (node->children[0], flScale);
		R_DecalNode (node->children[1], flScale);
	}
}

void R_DecalShoot( tex_t *ptexture, int index, int entity, int modelIndex, const float *position, int flags, float flScale )
{
	mnode_t		*pnodes;
	cl_entity_t	*pent;

	cl_entities = ( cl_entity_t * )*( DWORD * )0x1EF9B04;

	VectorCopy( position, gDecalPos );	// Pass position in global

	pent = &cl_entities[entity];
	if ( pent )
	{
		gDecalModel = pent->model;
	}
	else
		gDecalModel = NULL;

	if ( !gDecalModel || gDecalModel->type != mod_brush || !ptexture )
	{
		gEngfuncs.Con_DPrintf( "Decals must hit mod_brush!\n" );
		return;
	}

	pnodes = gDecalModel->nodes;

	if ( entity )
	{
		hull_t	*phull;
		vec3_t	temp;

		if ( gDecalModel->firstmodelsurface )
		{
			phull = &gDecalModel->hulls[0];

			VectorSubtract( position, phull->clip_mins, temp );
			VectorSubtract( temp, pent->origin, gDecalPos );

			pnodes = pnodes + phull->firstclipnode;
		}

		if ( pent->angles[0] || pent->angles[1] || pent->angles[2] )
		{
			vec3_t	forward, right, up;

			gEngfuncs.pfnAngleVectors( pent->angles, forward, right, up );

			VectorCopy (gDecalPos, temp);
			gDecalPos[0] = DotProduct (temp, forward);
			gDecalPos[1] = -DotProduct (temp, right);
			gDecalPos[2] = DotProduct (temp, up);
		}
	}

	// More state used by R_DecalNode()
	gDecalEntity = entity;
	gDecalTexture = ptexture;
	gDecalIndex = ptexture->id;

	// Don't optimize custom decals
	if ( !(flags & FDECAL_CUSTOM) )
		flags |= FDECAL_CLIPTEST;

	gDecalFlags = flags;
	gDecalSize = ptexture->wide >> 1;
	if ( ( int )( ptexture->tall >> 1 ) > gDecalSize )
		gDecalSize = ptexture->tall >> 1;

	R_DecalNode( pnodes, flScale );
}