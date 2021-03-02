
#include "extdll.h"
#include "util.h"
#include "cbase.h"

#include "source.h"
#include "ispatialpartition.h"


void ISpatialPartition::EnumerateElementsAlongRay(
	SpatialPartitionListMask_t listMask, const Ray_t& ray, bool coarseTest, IPartitionEnumerator* pIterator )
{
	TraceResult result;
	UTIL_TraceHull( ray.m_Start, ray.m_Start + ray.m_Delta, ignore_monsters, 1, NULL, &result );

	CBaseEntity *pEntity;

	if ( !FNullEnt( result.pHit ) )
	{
		pEntity = CBaseEntity::Instance( result.pHit );

		if ( pEntity )
		{
			pIterator->EnumElement( pEntity );
		}
	}
}

