
#include "cl_dll.h"
#include "tempentity.h"
#include "view.h"

typedef struct
{
	int				distance;
	tempentity_t	*entity;
}
transObjRef;

#define MAX_TRANSOBJS	1024

static int			numTransObjs;
static transObjRef	transObjects[ MAX_TRANSOBJS ];


//-----------------------------------------------------------------------------
// Purpose: Add entity to drawlist, it will sort by distance from the camera
//-----------------------------------------------------------------------------
void TE_AddEntity( tempentity_t *pEnt )
{
	int		i;
	float	dist;
	vec3_t	v;

	if ( numTransObjs >= MAX_TRANSOBJS )
		Sys_Error( "TE_AddEntity: Too many objects" );

	// calculate distance
	VectorSubtract( pEnt->origin, r_origin, v );
	dist = DotProduct( v, v );

	for ( i = numTransObjs; i > 0; i-- )
	{
		if ( transObjects[i - 1].distance >= dist )
			break;

		transObjects[i].distance = transObjects[i - 1].distance;
		transObjects[i].entity = transObjects[i - 1].entity;
	}

	transObjects[i].distance = dist;
	transObjects[i].entity = pEnt;

	numTransObjs++;
}

//-----------------------------------------------------------------------------
// Purpose: Draw sorted list
//-----------------------------------------------------------------------------
void TE_DrawEntitiesOnList( void )
{
	int		i;

	// draw all the objects
	for ( i = 0; i < numTransObjs; i++ )
	{
		transObjRef *obj = &transObjects[i];

		if ( obj->entity->draw )
		{
			obj->entity->draw( obj->entity );
		}
	}

	// list clear
	numTransObjs = 0;
}


#define MAX_ENTITIES	1024
static tempentity_t entityPool[ MAX_ENTITIES ];


//-----------------------------------------------------------------------------
// Purpose: Create a new temp entity, return the pointer. Must be call 
//            TE_DestroyEntity() to destroy it.
//-----------------------------------------------------------------------------
tempentity_t *TE_AllocateEntity( void )
{
	int		i;

	// find a free position in the pool
	for ( i = 0; i < MAX_ENTITIES; i++ )
	{
		tempentity_t *ent = &entityPool[i];

		if ( !ent->valid )
		{
			// _DEBUG
			memset( ent, 0, sizeof( tempentity_t ) );

			// setup this entity
			ent->valid = 1;
			ent->start = gEngfuncs.GetClientTime();

			// return the pointer
			return ent;
		}
	}

	// the pool is full
	Sys_Error( "TE_AllocateEntity: Too many entities" );

	return NULL;
}

//-----------------------------------------------------------------------------
// Purpose: Destroy an entity, the pointer must be valid
//-----------------------------------------------------------------------------
void TE_DestroyEntity( tempentity_t *pEntity )
{
	//pEntity->valid = 0;
	memset( pEntity, 0, sizeof( tempentity_t ) );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void TE_DestroyAllEntity( void )
{
	memset( entityPool, 0, sizeof( entityPool ) );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void TE_DrawEntitiesOnPool( void )
{
	int		i;
	float	time;

	time = gEngfuncs.GetClientTime();

	// fill the drawlist
	for ( i = 0; i < MAX_ENTITIES; i++ )
	{
		tempentity_t *ent = &entityPool[i];

		if ( ent->valid )
		{
			// update the clock
			ent->time = time;
			// add to drawlist
			TE_AddEntity( ent );
		}
	}

	// draw all the entities in the list
	TE_DrawEntitiesOnList();
}

tent_api_t gTEntAPI = 
{
	TE_AllocateEntity,
	TE_DestroyEntity,
	TE_DestroyAllEntity,
};