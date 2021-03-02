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

#define DIRECTINPUT_VERSION	0x0800

#include <windows.h>
#include <dinput.h>
#include "dxinput.h"


LPDIRECTINPUT8	g_pDI = NULL;
LPDIRECTINPUTDEVICE8	g_pMouse = NULL;


bool DI_Init( void )
{
	HWND hWnd;

	if ( FAILED( DirectInput8Create( GetModuleHandle( NULL ), DIRECTINPUT_VERSION, IID_IDirectInput8, ( VOID ** )&g_pDI, NULL ) ) )
	{
		return false;
	}

	if ( FAILED( g_pDI->CreateDevice( GUID_SysMouse, &g_pMouse, NULL ) ) )
	{
		DI_Shutdown();
		return false;
	}

	if ( FAILED( g_pMouse->SetDataFormat( &c_dfDIMouse2 ) ) )
	{
		DI_Shutdown();
		return false;
	}

	if ( hWnd = GetActiveWindow() )
	{
		if ( FAILED( g_pMouse->SetCooperativeLevel( hWnd, DISCL_EXCLUSIVE | DISCL_FOREGROUND ) ) )
		{
			DI_Shutdown();
			return false;
		}
	}

	return true;
}

void DI_Shutdown( void )
{
	if ( g_pMouse )
	{
		g_pMouse->Unacquire();
		g_pMouse->Release();
		g_pMouse = NULL;
	}

	if ( g_pDI )
	{
		g_pDI->Release();
		g_pDI = NULL;
	}
}

void DI_Acquire( void )
{
	if ( g_pMouse )
	{
		g_pMouse->Acquire();
	}
}

void DI_Unacquire( void )
{
	if ( g_pMouse )
	{
		g_pMouse->Unacquire();
	}
}

void DI_Poll( void )
{
	if ( g_pMouse )
	{
		g_pMouse->Poll();
	}
}

bool DI_GetState( mouse_state_t *state )
{
	DIMOUSESTATE2	dims;
	int				i;

	ZeroMemory( state, sizeof( mouse_state_t ) );

	if ( g_pMouse == NULL )
	{
		return false;
	}

	if ( FAILED( g_pMouse->GetDeviceState( sizeof( DIMOUSESTATE2 ), &dims ) ) )
	{
		return false;
	}

	state->lx = dims.lX;
	state->ly = dims.lY;
	
	for ( i = 0; i < 8; i++ )
	{
		state->buttons[i] = dims.rgbButtons[i];
	}

	return true;
}