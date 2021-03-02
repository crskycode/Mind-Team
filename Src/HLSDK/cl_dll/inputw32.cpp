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

#include "cl_dll.h"
#include "view.h"
#include <usercmd.h>
#include <keydefs.h>

#include "dxinput.h"

// !!!
#include "GameUI_Interface.h"
#include "vgui_gamemenudialog.h"
#include "vgui_buysetupdialog.h"
#include "vgui_chatboxdialog.h"

#define MOUSE_BUTTON_COUNT 5

extern "C" 
{
	DLLExport void IN_ActivateMouse( void );
	DLLExport void IN_DeactivateMouse( void );
	DLLExport void IN_MouseEvent( int mstate );
	DLLExport void IN_Accumulate( void );
	DLLExport void IN_ClearStates( void );
}

cvar_t	*cl_sensity;
cvar_t	*cl_sensity_zoom;

static int	mouseoldbuttonstate;
static int	mouseactive;
static int	directinputactive;

DLLExport void IN_ActivateMouse( void )
{
	if ( GameUI_IsGameUIActive() )
		return;

	if ( GameMenuDialog_IsVisible() )
		return;

	if ( ChatBoxDialog_IsVisible() )
		return;

	if ( BuySetupDialog_IsVisible() )
		return;

	DI_Acquire();

	mouseactive = 1;
}

DLLExport void IN_DeactivateMouse( void )
{
	DI_Unacquire();

	mouseactive = 0;
}

DLLExport void IN_MouseEvent( int mstate )
{
	for ( int i = 0 ; i < MOUSE_BUTTON_COUNT ; ++i )
	{
		if ( ( mstate & (1 << i) ) && !( mouseoldbuttonstate & (1 << i) ) )
		{
			gEngfuncs.Key_Event( K_MOUSE1 + i, 1 );
		}

		if ( !( mstate & (1 << i) ) && ( mouseoldbuttonstate & (1 << i) ) )
		{
			gEngfuncs.Key_Event( K_MOUSE1 + i, 0 );
		}
	}

	mouseoldbuttonstate = mstate;
}

float anglemod( float a )
{
	return ( 360.0 / 65536 ) * ( ( int )( a * ( 65536 / 360.0 ) ) & 65535 );
}

void IN_MouseMove( void )
{
	vec3_t	viewangles;

	gEngfuncs.GetViewAngles( viewangles );

	V_StopPitchDrift();

	mouse_state_t	ms;
	float	mx, my;

	DI_GetState( &ms );

	// Sensitivity
	mx = ms.lx * cl_sensity->value;
	my = ms.ly * cl_sensity->value;

	// add mouse X/Y movement to cmd
	viewangles[YAW] -= mx * 0.022;
	viewangles[PITCH] += my * 0.022;

	// set limit
	viewangles[PITCH] = min( viewangles[PITCH], 89 );
	viewangles[PITCH] = max( viewangles[PITCH], -89 );

	// FixUp
	viewangles[YAW] = anglemod( viewangles[YAW] );

	gEngfuncs.SetViewAngles( viewangles );
}

void IN_MouseKey( void )
{
	mouse_state_t	ms;
	int				mstate, i;

	DI_GetState( &ms );

	mstate = 0;

	for ( i = 0; i < MOUSE_BUTTON_COUNT; i++ )
	{
		if ( ms.buttons[i] & 0x80 )
		{
			mstate |= ( 1 << i );
		}
	}

	IN_MouseEvent( mstate );
}

DLLExport void IN_Accumulate( void )
{
}

DLLExport void IN_ClearStates( void )
{
	if ( mouseactive )
	{
		mouseoldbuttonstate = 0;
	}
}

void IN_Move( float frametime, usercmd_t *cmd )
{
	if ( mouseactive )
	{
		if ( directinputactive )
		{
			// acviate direct input
			DI_Poll();
			DI_Acquire();

			// process mouse movement
			IN_MouseMove();
			// process mouse keypress
			IN_MouseKey();
		}
	}
}

// Reset camera angles for debug
void CAM_Reset( void )
{
	vec3_t	angles;

	VectorClear( angles );
	gEngfuncs.SetViewAngles( angles );
}

void IN_Init( void )
{
	mouseoldbuttonstate = 0;
	mouseactive = 0;
	directinputactive = 0;

	cl_sensity = gEngfuncs.pfnRegisterVariable( "cl_sensity", "3", FCVAR_ARCHIVE );
	cl_sensity_zoom = gEngfuncs.pfnRegisterVariable( "cl_sensity_zoom", "3", FCVAR_ARCHIVE );

	gEngfuncs.pfnAddCommand( "cam_reset", CAM_Reset );

	if ( DI_Init() )
	{
		directinputactive = 1;
	}
	else
	{
		Sys_Error( "Failed to create direct input device" );
	}
}

void IN_Shutdown( void )
{
	IN_DeactivateMouse();

	DI_Shutdown();
}