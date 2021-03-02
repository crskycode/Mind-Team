//========= Copyright ?1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $Revision: $
// $NoKeywords: $
//=============================================================================//

#ifndef ISPATIALPARTITION_H
#define ISPATIALPARTITION_H

//-----------------------------------------------------------------------------
// forward declarations
//-----------------------------------------------------------------------------

//class Vector;
struct Ray_t;


//-----------------------------------------------------------------------------
// These are the various partition lists. Note some are server only, some
// are client only
//-----------------------------------------------------------------------------

enum
{
	PARTITION_ENGINE_SOLID_EDICTS		= (1 << 0),		// every edict_t that isn't SOLID_TRIGGER or SOLID_NOT (and static props)
};

//-----------------------------------------------------------------------------
// Clients that want to know about all elements within a particular
// volume must inherit from this
//-----------------------------------------------------------------------------

enum IterationRetval_t
{
	ITERATION_CONTINUE = 0,
	ITERATION_STOP,
};


// A combination of the PARTITION_ flags above.
typedef int SpatialPartitionListMask_t;	


//-----------------------------------------------------------------------------
// Any search in the CSpatialPartition must use this to filter out entities it doesn't want.
// You're forced to use listMasks because it can filter by listMasks really fast. Any other
// filtering can be done by EnumElement.
//-----------------------------------------------------------------------------

class IPartitionEnumerator
{
public:
	virtual IterationRetval_t EnumElement( CBaseEntity *pHandleEntity ) = 0;
};


//-----------------------------------------------------------------------------
// This is the spatial partition manager, groups objects into buckets
//-----------------------------------------------------------------------------

class ISpatialPartition
{
public:
	// Gets all entities in a particular volume...
	// if coarseTest == true, it'll return all elements that are in
	// spatial partitions that intersect the box
	// if coarseTest == false, it'll return only elements that truly intersect
	static void EnumerateElementsAlongRay(
		SpatialPartitionListMask_t listMask, 
		const Ray_t& ray, 
		bool coarseTest, 
		IPartitionEnumerator* pIterator );
};

#endif
