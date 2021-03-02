//========= Copyright ?1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: Common collision utility methods
//
// $Header: $
// $NoKeywords: $
//=============================================================================//

//!#if !defined(_STATIC_LINKED) || defined(_SHARED_LIB)

#include "extdll.h"
#include "util.h"
#include "source.h"
#include "collisionutils.h"
#include <float.h>

#define UNINIT		-99999.0


//-----------------------------------------------------------------------------
// Compute the offset in t along the ray that we'll use for the collision
//-----------------------------------------------------------------------------
static float ComputeBoxOffset( const Ray_t& ray )
{
	if (ray.m_IsRay)
		return 1e-3f;

	// Find the projection of the box diagonal along the ray...
	float offset = FloatMakePositive(ray.m_Extents[0] * ray.m_Delta[0]) +
					FloatMakePositive(ray.m_Extents[1] * ray.m_Delta[1]) +
					FloatMakePositive(ray.m_Extents[2] * ray.m_Delta[2]);

	// We need to divide twice: Once to normalize the computation above
	// so we get something in units of extents, and the second to normalize
	// that with respect to the entire raycast.
	offset *= InvRSquared( ray.m_Delta );

	// 1e-3 is an epsilon
	return offset + 1e-3;
}

//-----------------------------------------------------------------------------
// Intersects a swept box against a triangle
//-----------------------------------------------------------------------------
float IntersectRayWithTriangle( const Ray_t& ray, 
		const Vector& v1, const Vector& v2, const Vector& v3, bool oneSided )
{
	// This is cute: Use barycentric coordinates to represent the triangle
	// Vo(1-u-v) + V1u + V2v and intersect that with a line Po + Dt
	// This gives us 3 equations + 3 unknowns, which we can solve with
	// Cramer's rule...
	//		E1x u + E2x v - Dx t = Pox - Vox
	// There's a couple of other optimizations, Cramer's rule involves
	// computing the determinant of a matrix which has been constructed
	// by three vectors. It turns out that 
	// det | A B C | = -( A x C ) dot B or -(C x B) dot A
	// which we'll use below..

	Vector edge1, edge2, org;
	VectorSubtract( v2, v1, edge1 );
	VectorSubtract( v3, v1, edge2 );

	// Cull out one-sided stuff
	if (oneSided)
	{
		Vector normal;
		CrossProduct( edge1, edge2, normal );
		if (DotProduct( normal, ray.m_Delta ) >= 0.0f)
			return -1.0f;
	}

	// FIXME: This is inaccurate, but fast for boxes
	// We want to do a fast separating axis implementation here
	// with a swept triangle along the reverse direction of the ray.

	// Compute some intermediary terms
	Vector dirCrossEdge2, orgCrossEdge1;
	CrossProduct( ray.m_Delta, edge2, dirCrossEdge2 );

	// Compute the denominator of Cramer's rule:
	//		| -Dx E1x E2x |
	// det	| -Dy E1y E2y | = (D x E2) dot E1
	//		| -Dz E1z E2z |
	float denom = DotProduct( dirCrossEdge2, edge1 );
	if( FloatMakePositive( denom ) < 1e-6 )
		return -1.0f;
	denom = 1.0f / denom;

	// Compute u. It's gotta lie in the range of 0 to 1.
	//				   | -Dx orgx E2x |
	// u = denom * det | -Dy orgy E2y | = (D x E2) dot org
	//				   | -Dz orgz E2z |
	VectorSubtract( ray.m_Start, v1, org );
	float u = DotProduct( dirCrossEdge2, org ) * denom;
	if ((u < 0.0f) || (u > 1.0f))
		return -1.0f;

	// Compute t and v the same way...
	// In barycentric coords, u + v < 1
	CrossProduct( org, edge1, orgCrossEdge1 );
	float v = DotProduct( orgCrossEdge1, ray.m_Delta ) * denom;
	if ((v < 0.0f) || (v + u > 1.0f))
		return -1.0f;

	// Compute the distance along the ray direction that we need to fudge 
	// when using swept boxes
	float boxt = ComputeBoxOffset( ray );
	float t = DotProduct( orgCrossEdge1, edge2 ) * denom;
	if ((t < -boxt) || (t > 1.0f + boxt))
		return -1.0f;

	return clamp( t, 0, 1 );
}

float IntersectRayWithAAPlane( const Vector& vecStart, const Vector& vecEnd, int nAxis, float flSign, float flDist )
{
	float denom	= flSign * (vecEnd[nAxis] - vecStart[nAxis]);
	if (denom == 0.0f)
		return 0.0f;

	denom = 1.0f / denom;
	return (flDist - flSign * vecStart[nAxis]) * denom;
}
