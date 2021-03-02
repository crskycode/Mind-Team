/***
 * 
 * Copyright (c) 1996-2002, Valve LLC. All rights reserved.
 * 
 * This product contains software technology licensed from Id
 * Software, Inc. ("Id Technology"). Id Technology (c) 1996 Id Software, Inc.
 * All Rights Reserved.
 * 
 * Use, distribution, and modification of this source code and/or resulting
 * object code is restricted to non-commercial enhancements to products from
 * Valve LLC. All other use, distribution, or modification is prohibited
 * without written permission from Valve LLC.
 * 
***/

#include "extdll.h"

#define MAX_ARGS	80
#define ARGSIZE	256

static int	cmd_argc;
static char	cmd_argv[ MAX_ARGS ][ ARGSIZE ];
static char cmd_args[ ARGSIZE ];

void Cmd_Init( const char *text )
{
	char	*p;

	strcpy( cmd_args, text );

	cmd_argc = 0;

	while ( cmd_argc < MAX_ARGS )
	{
		while ( *text && isspace( *text ) )
		{
			text++;
		}

		if ( !*text || *text == '\n' )
			break;

		if ( *text == '"' )
		{
			text++;
			p = ( char * )&cmd_argv[ cmd_argc++ ];

			while ( *text && *text != '"' )
			{
				*p++ = *text++;
			}

			*p = '\0';

			if ( *text == '"' )
				text++;
		}
		else
		{
			p = ( char * )&cmd_argv[ cmd_argc++ ];

			while ( *text && !isspace( *text ) )
			{
				*p++ = *text++;
			}

			*p = '\0';
		}
	}
}

int Cmd_Argc( void )
{
	return cmd_argc;
}

const char *Cmd_Argv( int i )
{
	if ( i < cmd_argc )
		return cmd_argv[ i ];

	return "";
}

const char *Cmd_Args( void )
{
	return cmd_args;
}