
#ifndef SOURCESDK_H
#define SOURCESDK_H

#ifndef MAX_PLAYERS
#define MAX_PLAYERS 32
#endif

//------------------------------------------------------------------------------------------------------------------
// DBG.H

#define Assert assert
#define ASSERT assert

#define AssertMsg( _exp, _msg )

#define COMPILE_TIME_ASSERT( pred )

void Warning( const char *pMsg, ... );
void Error( const char *pMsg, ... );
void Msg( const char *pMsg, ... );
void DevMsg( const char *pMsg, ... );


//------------------------------------------------------------------------------------------------------------------
// BASETYPES.H

#define clamp(val, min, max) (((val) > (max)) ? (max) : (((val) < (min)) ? (min) : (val)))


//------------------------------------------------------------------------------------------------------------------
// PLATFORM.H

#define RESTRICT __restrict

// decls for aligning data
#define ALIGN16 __declspec(align(16))

inline double Plat_FloatTime( void )
{
	return g_engfuncs.pfnTime();
}

// In win32 try to use the intrinsic fabs so the optimizer can do it's thing inline in the code
#pragma intrinsic( fabs )
// Also, alias float make positive to use fabs, too
// NOTE:  Is there a perf issue with double<->float conversion?
inline float FloatMakePositive( float f )
{
	return (float)fabs( f );
}

template <class T>
inline void CopyConstruct( T* pMemory, T const& src )
{
	::new( pMemory ) T(src);
}

template <class T>
inline void Destruct( T* pMemory )
{
	pMemory->~T();

#ifdef _DEBUG
	memset( pMemory, 0xDD, sizeof(T) );
#endif
}


//------------------------------------------------------------------------------------------------------------------
// MEMALLOC.H

#define MEM_ALLOC_CREDIT_CLASS()	((void)0)


//------------------------------------------------------------------------------------------------------------------
// VPROF.H

#define VPROF_BUDGET( a, b )

#define VPROF( x )


//------------------------------------------------------------------------------------------------------------------
// MATHLIB.H

#include <float.h>

template <class T>
inline void swap( T& x, T& y )
{
	T temp = x;
	x = y;
	y = temp;
}

inline float _InvRSquared(const float* v)
{
	float r2 = DotProduct(v, v);
	return r2 < 1.f ? 1.f : 1/r2;
}

void VectorVectors( const Vector &forward, Vector &right, Vector &up );

// BUGBUG: Why do we need both of these?
inline float AngleDiff( float destAngle, float srcAngle )
{
	float delta;

	delta = fmodf(destAngle - srcAngle, 360.0f);
	if ( destAngle > srcAngle )
	{
		if ( delta >= 180 )
			delta -= 360;
	}
	else
	{
		if ( delta <= -180 )
			delta += 360;
	}
	return delta;
}

inline float AngleNormalize( float angle )
{
	angle = fmodf(angle, 360.0f);
	if (angle > 180) 
	{
		angle -= 360;
	}
	if (angle < -180)
	{
		angle += 360;
	}
	return angle;
}

// ensure that 0 <= angle <= 360
inline float AngleNormalizePositive( float angle )
{
	angle = fmodf( angle, 360.0f );

	if (angle < 0.0f)
	{
		angle += 360.0f;
	}

	return angle;
}

inline bool AnglesAreEqual( float a, float b, float tolerance = 0.0f )
{
	return (fabs( AngleDiff( a, b ) ) < tolerance);
}

inline void AngleVectors( const Vector &angles, Vector *forward = NULL, Vector *right = NULL, Vector *up = NULL )
{
	ANGLE_VECTORS( angles, (float *)forward, (float *)right, (float *)up );
}

inline void VectorAngles( const Vector &forward, Vector &angles )
{
	VEC_TO_ANGLES( angles, angles );
}


//------------------------------------------------------------------------------------------------------------------
// RANDOM.H

#define RandomFloat	RANDOM_FLOAT
#define RandomInt RANDOM_LONG


//------------------------------------------------------------------------------------------------------------------
// COMMONMACROS.H

#define SETBITS SetBits
#define CLEARBITS ClearBits


//------------------------------------------------------------------------------------------------------------------
// STRTOOLS.H

#define Q_strncat( a, b, c, d ) strcat( a, b )


//------------------------------------------------------------------------------------------------------------------
// SOURCE HEADERS

#include "utlmemory.h"
#include "utllinkedlist.h"
#include "utlvector.h"
#include "convar.h"
#include "util_shared.h"
#include "filesystem_simple.h"
#include "debugoverlay_shared.h"
#include "cmodel.h"


//------------------------------------------------------------------------------------------------------------------
// VECTOR.H

// ** Note: this generates a temporary, don't hold reference!
#define VecToString(v)	(static_cast<const char *>(CFmtStr("(%f, %f, %f)", (v).x, (v).y, (v).z)))

inline float InvRSquared( const Vector &v )
{
	return _InvRSquared(&v.x);
}


//------------------------------------------------------------------------------------------------------------------
// VECTOR2D.H

inline float DotProduct2D(const Vector2D& a, const Vector2D& b) 
{ 
	return( a.x*b.x + a.y*b.y ); 
}

// Vector equality with tolerance
inline bool VectorsAreEqual( const Vector& src1, const Vector& src2, float tolerance )
{
	if (FloatMakePositive(src1.x - src2.x) > tolerance)
		return false;
	if (FloatMakePositive(src1.y - src2.y) > tolerance)
		return false;
	return (FloatMakePositive(src1.z - src2.z) <= tolerance);
}

//------------------------------------------------------------------------------------------------------------------
// UTIL.H

// Returns true if the command was issued by the listenserver host, or by the dedicated server, via rcon or the server console.
// This is valid during ConCommand execution.
bool UTIL_IsCommandIssuedByServerAdmin( void );

// get the local player on a listen server
CBasePlayer *UTIL_GetListenServerHost( void );

// Execute server command
void UTIL_ServerCommand( const char *pszCommand, ... );


//------------------------------------------------------------------------------------------------------------------
// UTIL_SHARED.H

void UTIL_TraceHull( const Vector &vecAbsStart, const Vector &vecAbsEnd, const Vector &hullMin, const Vector &hullMax, IGNORE_MONSTERS igmon, edict_t *pentIgnore, TraceResult *ptr );


//------------------------------------------------------------------------------------------------------------------
// OBSTACLE_PUSHAWAY.H

bool IsBreakableEntity( CBaseEntity *pEnt );


//------------------------------------------------------------------------------------------------------------------
// PLAYER.H

template < typename Functor >
bool ForEachPlayer( Functor &func )
{
        for( int i=1; i<=gpGlobals->maxClients; ++i )
        {
                CBasePlayer *player = static_cast<CBasePlayer *>( UTIL_PlayerByIndex( i ) );

                if (player == NULL)
                        continue;

                if (FNullEnt( player->edict() ))
                        continue;

                if (!player->IsPlayer())
                        continue;

                if (func( player ) == false)
                        return false;
        }

        return true;
}


//------------------------------------------------------------------------------------------------------------------
// SHAREDDEFS.H

#define CSTRIKE_DLL

#define TEAM_UNASSIGNED 0
#define TEAM_TERRORIST 1
#define TEAM_CT 2
#define TEAM_MAXCOUNT 3

extern const Vector vec3_origin;


#endif
