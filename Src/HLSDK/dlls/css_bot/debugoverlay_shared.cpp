//========= Copyright ?1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: Utility functions for using debug overlays to visualize information
//			in the world.  Uses the IVDebugOverlay interface.
//
//=============================================================================//

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "source.h"
#include "debugoverlay_shared.h"
#include "Color.h"
//!#include "mathlib/mathlib.h"

// memdbgon must be the last include file in a .cpp file!!!
//!#include "tier0/memdbgon.h"

#define	MAX_OVERLAY_DIST_SQR	90000000

extern void (*pfnDebugLine)(float *start, float *end, int r, int g, int b, float life);
void _Line(const Vector &vecSrc, const Vector &vecDest, float flDuration, byte r, byte g, byte b)
{
	if (!pfnDebugLine)
		return;
	pfnDebugLine(vecSrc, vecDest, r, g, b, flDuration);
}

void _DrawRect(const Vector &tl, const Vector &bl, const Vector &tr, const Vector &br, Color &color, float flDuration)
{
	_Line(tl, bl, flDuration, color[0], color[1], color[2]);
	_Line(bl, br, flDuration, color[0], color[1], color[2]);
	_Line(br, tr, flDuration, color[0], color[1], color[2]);
	_Line(tr, tl, flDuration, color[0], color[1], color[2]);
}

static int _boxpnt[6][4] =
{
	{ 0, 4, 6, 2 },		// +X
	{ 0, 1, 5, 4 },		// +Y
	{ 0, 2, 3, 1 },		// +Z
	{ 7, 5, 1, 3 },		// -X
	{ 7, 3, 2, 6 },		// -Y
	{ 7, 6, 4, 5 },		// -Z
};

void _DrawBBox(const Vector &mins, const Vector &maxs, const Vector &origin, Color &color, float flDuration)
{
	int		j;

	vec3_t	tmp;
	vec3_t	p[8];
	float	gap = 0;

	for ( j = 0; j < 8; j++ )
	{
		tmp[0] = ( j & 1 ) ? mins[0] - gap : maxs[0] + gap;
		tmp[1] = ( j & 2 ) ? mins[1] - gap : maxs[1] + gap;
		tmp[2] = ( j & 4 ) ? mins[2] - gap : maxs[2] + gap;

		VectorAdd( tmp, origin, tmp );
		VectorCopy( tmp, p[j] );
	}

	for ( j = 0; j < 6; j++ )
	{
		_DrawRect(
		    p[_boxpnt[j][1]],
		    p[_boxpnt[j][0]],
		    p[_boxpnt[j][2]],
		    p[_boxpnt[j][3]],
			color,
		    flDuration );
	}
}

//-----------------------------------------------------------------------------
// Purpose: Draw a box with no orientation
//-----------------------------------------------------------------------------
void NDebugOverlay::Box(const Vector &origin, const Vector &mins, const Vector &maxs, int r, int g, int b, int a, float flDuration)
{
	Color color(r, g, b, a);
	_DrawBBox(mins, maxs, origin, color, flDuration);
}

//-----------------------------------------------------------------------------
// Purpose: Draws a line from one position to another
//-----------------------------------------------------------------------------
void NDebugOverlay::Line( const Vector &origin, const Vector &target, int r, int g, int b, bool noDepthTest, float duration )
{
	_Line(origin, target, duration, r, g, b);
}

//-----------------------------------------------------------------------------
// Purpose: Draw triangle
//-----------------------------------------------------------------------------
void NDebugOverlay::Triangle( const Vector &p1, const Vector &p2, const Vector &p3, int r, int g, int b, int a, bool noDepthTest, float duration )
{
	_Line(p1, p2, duration, r, g, b);
	_Line(p2, p3, duration, r, g, b);
	_Line(p3, p1, duration, r, g, b);
}

//-----------------------------------------------------------------------------
// Purpose: Draw debug text at a position
//-----------------------------------------------------------------------------
void NDebugOverlay::Text( const Vector &origin, const char *text, bool bViewCheck, float duration )
{
	Msg( "Overlay: %s\n", text );
}

//-----------------------------------------------------------------------------
// Purpose: Add debug overlay text with screen position
//-----------------------------------------------------------------------------
void NDebugOverlay::ScreenText( float flXpos, float flYpos, const char *text, int r, int g, int b, int a, float duration )
{
	Msg( "Overlay: %s", text );
}

//-----------------------------------------------------------------------------
// Purpose: Draw a colored 3D cross of the given size at the given position
//-----------------------------------------------------------------------------
void NDebugOverlay::Cross3D(const Vector &position, float size, int r, int g, int b, bool noDepthTest, float flDuration )
{
	Line( position + Vector(size,0,0), position - Vector(size,0,0), r, g, b, noDepthTest, flDuration );
	Line( position + Vector(0,size,0), position - Vector(0,size,0), r, g, b, noDepthTest, flDuration );
	Line( position + Vector(0,0,size), position - Vector(0,0,size), r, g, b, noDepthTest, flDuration );
}

//--------------------------------------------------------------------------------
// Purpose : Draw a horizontal arrow pointing in the specified direction
//--------------------------------------------------------------------------------
void NDebugOverlay::HorzArrow( const Vector &startPos, const Vector &endPos, float width, int r, int g, int b, int a, bool noDepthTest, float flDuration)
{
	Vector	lineDir		= (endPos - startPos);
	VectorNormalize( lineDir );
	Vector  upVec		= Vector( 0, 0, 1 );
	Vector	sideDir;
	float   radius		= width / 2.0;

	CrossProduct(lineDir, upVec, sideDir);

	Vector p1 =	startPos - sideDir * radius;
	Vector p2 = endPos - lineDir * width - sideDir * radius;
	Vector p3 = endPos - lineDir * width - sideDir * width;
	Vector p4 = endPos;
	Vector p5 = endPos - lineDir * width + sideDir * width;
	Vector p6 = endPos - lineDir * width + sideDir * radius;
	Vector p7 =	startPos + sideDir * radius;

	// Outline the arrow
	Line(p1, p2, r,g,b,noDepthTest,flDuration);
	Line(p2, p3, r,g,b,noDepthTest,flDuration);
	Line(p3, p4, r,g,b,noDepthTest,flDuration);
	Line(p4, p5, r,g,b,noDepthTest,flDuration);
	Line(p5, p6, r,g,b,noDepthTest,flDuration);
	Line(p6, p7, r,g,b,noDepthTest,flDuration);

	if ( a > 0 )
	{
		// Fill us in with triangles
		Triangle( p5, p4, p3, r, g, b, a, noDepthTest, flDuration ); // Tip
		Triangle( p1, p7, p6, r, g, b, a, noDepthTest, flDuration ); // Shaft
		Triangle( p6, p2, p1, r, g, b, a, noDepthTest, flDuration );

		// And backfaces
		Triangle( p3, p4, p5, r, g, b, a, noDepthTest, flDuration ); // Tip
		Triangle( p6, p7, p1, r, g, b, a, noDepthTest, flDuration ); // Shaft
		Triangle( p1, p2, p6, r, g, b, a, noDepthTest, flDuration );
	}
}


//--------------------------------------------------------------------------------
// Purpose : Draw a vertical arrow at a position
//--------------------------------------------------------------------------------
void NDebugOverlay::VertArrow( const Vector &startPos, const Vector &endPos, float width, int r, int g, int b, int a, bool noDepthTest, float flDuration)
{
	Vector	lineDir		= (endPos - startPos);
	VectorNormalize( lineDir );
	Vector  upVec;
	Vector	sideDir;
	float   radius		= width / 2.0;

	VectorVectors( lineDir, sideDir, upVec );

	Vector p1 =	startPos - upVec * radius;
	Vector p2 = endPos - lineDir * width - upVec * radius;
	Vector p3 = endPos - lineDir * width - upVec * width;
	Vector p4 = endPos;
	Vector p5 = endPos - lineDir * width + upVec * width;
	Vector p6 = endPos - lineDir * width + upVec * radius;
	Vector p7 =	startPos + upVec * radius;

	// Outline the arrow
	Line(p1, p2, r,g,b,noDepthTest,flDuration);
	Line(p2, p3, r,g,b,noDepthTest,flDuration);
	Line(p3, p4, r,g,b,noDepthTest,flDuration);
	Line(p4, p5, r,g,b,noDepthTest,flDuration);
	Line(p5, p6, r,g,b,noDepthTest,flDuration);
	Line(p6, p7, r,g,b,noDepthTest,flDuration);

	if ( a > 0 )
	{
		// Fill us in with triangles
		Triangle( p5, p4, p3, r, g, b, a, noDepthTest, flDuration ); // Tip
		Triangle( p1, p7, p6, r, g, b, a, noDepthTest, flDuration ); // Shaft
		Triangle( p6, p2, p1, r, g, b, a, noDepthTest, flDuration );

		// And backfaces
		Triangle( p3, p4, p5, r, g, b, a, noDepthTest, flDuration ); // Tip
		Triangle( p6, p7, p1, r, g, b, a, noDepthTest, flDuration ); // Shaft
		Triangle( p1, p2, p6, r, g, b, a, noDepthTest, flDuration );
	}
}
