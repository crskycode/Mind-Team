//========= Copyright ?1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: Common collision utility methods
//
// $Header: $
// $NoKeywords: $
//=============================================================================//

#ifndef COLLISIONUTILS_H
#define COLLISIONUTILS_H

#ifdef _WIN32
#pragma once
#endif

//-----------------------------------------------------------------------------
// forward declarations
//-----------------------------------------------------------------------------
struct Ray_t;
class Vector2D;


//-----------------------------------------------------------------------------
//
// IntersectRayWithTriangle
//
// Intersects a ray with a triangle, returns distance t along ray.
// t will be less than zero if no intersection occurred
// oneSided will cull collisions which approach the triangle from the back
// side, assuming the vertices are specified in counter-clockwise order
// The vertices need not be specified in that order if oneSided is not used
//
//-----------------------------------------------------------------------------
float IntersectRayWithTriangle( const Ray_t& ray, 
		                        const Vector& v1, const Vector& v2, const Vector& v3, 
								bool oneSided );

// This version intersects a ray with an axis-aligned plane
float IntersectRayWithAAPlane( const Vector& vecStart, const Vector& vecEnd, int nAxis, float flSign, float flDist );

#endif // COLLISIONUTILS_H