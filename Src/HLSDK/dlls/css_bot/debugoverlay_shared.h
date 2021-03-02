//========= Copyright ?1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#ifndef DEBUGOVERLAY_SHARED_H
#define DEBUGOVERLAY_SHARED_H
#ifdef _WIN32
#pragma once
#endif

//!#include "engine/ivdebugoverlay.h"
//!#include "mathlib/vector.h"

//=============================================================================
// NDebugOverlay
//=============================================================================
namespace NDebugOverlay
{
	void	Box(const Vector &origin, const Vector &mins, const Vector &maxs, int r, int g, int b, int a, float flDuration);
	void	Line( const Vector &origin, const Vector &target, int r, int g, int b, bool noDepthTest, float flDuration );
	void	Triangle( const Vector &p1, const Vector &p2, const Vector &p3, int r, int g, int b, int a, bool noDepthTest, float duration );
	void	Text( const Vector &origin, const char *text, bool bViewCheck, float flDuration );
	void	ScreenText( float fXpos, float fYpos, const char *text, int r, int g, int b, int a, float flDuration);
	void	Cross3D(const Vector &position, float size, int r, int g, int b, bool noDepthTest, float flDuration );
	void	HorzArrow( const Vector &startPos, const Vector &endPos, float width, int r, int g, int b, int a, bool noDepthTest, float flDuration);
	void	VertArrow( const Vector &startPos, const Vector &endPos, float width, int r, int g, int b, int a, bool noDepthTest, float flDuration);
};

#endif // DEBUGOVERLAY_SHARED_H
