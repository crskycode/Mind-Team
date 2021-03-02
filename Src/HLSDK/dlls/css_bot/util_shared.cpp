//========= Copyright ?1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "source.h"

const Vector vec3_origin;

static char string_buf[1024];


CBasePlayer *UTIL_GetListenServerHost(void)
{
	if ( IS_DEDICATED_SERVER() )
	{
		return NULL;
	}

	return UTIL_PlayerByIndex( 1 );
}

bool UTIL_IsCommandIssuedByServerAdmin(void)
{
	if ( IS_DEDICATED_SERVER() )
	{
		return NULL;
	}

	return true;
}

void UTIL_ServerCommand( const char *pszCommand, ... )
{
	va_list args;
	va_start( args, pszCommand );
	vsnprintf( string_buf, sizeof( string_buf ), pszCommand, args );
	va_end( args );
	SERVER_COMMAND( string_buf );
}

void Warning( const char *pMsg, ... )
{
	va_list args;
	va_start( args, pMsg );
	vsnprintf( string_buf, sizeof( string_buf ), pMsg, args );
	va_end( args );
	Con_Printf( "WARNING: %s", string_buf );
}

void Error( const char *pMsg, ... )
{
	va_list args;
	va_start( args, pMsg );
	vsnprintf( string_buf, sizeof( string_buf ), pMsg, args );
	va_end( args );
	Con_Printf( "ERROR: %s", string_buf );
}

void Msg( const char *pMsg, ... )
{
	va_list args;
	va_start( args, pMsg );
	vsnprintf( string_buf, sizeof( string_buf ), pMsg, args );
	va_end( args );
	Con_Printf( "MSG: %s", string_buf );
}

void DevMsg( const char *pMsg, ... )
{
	va_list args;
	va_start( args, pMsg );
	vsnprintf( string_buf, sizeof( string_buf ), pMsg, args );
	va_end( args );
	Con_Printf( "DEBUG: %s", string_buf );
}

void UTIL_TraceHull(const Vector &vecAbsStart, const Vector &vecAbsEnd,const Vector &hullMin,
					const Vector &hullMax, IGNORE_MONSTERS igmon, edict_t *pentIgnore, TraceResult *ptr)
{
	TRACE_HULL( vecAbsStart, vecAbsEnd, igmon, 1, pentIgnore, ptr );
}

void VectorVectors( const Vector &forward, Vector &right, Vector &up )
{
	Vector tmp;

	if (forward[0] == 0 && forward[1] == 0)
	{
		// pitch 90 degrees up/down from identity
		right[0] = 0;	
		right[1] = -1; 
		right[2] = 0;
		up[0] = -forward[2]; 
		up[1] = 0; 
		up[2] = 0;
	}
	else
	{
		tmp[0] = 0; tmp[1] = 0; tmp[2] = 1.0;
		CrossProduct( forward, tmp, right );
		VectorNormalize( right );
		CrossProduct( right, forward, up );
		VectorNormalize( up );
	}
}