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

#ifndef DXINPUT_H
#define DXINPUT_H

typedef struct mouse_state_s
{
	int		lx, ly;
	int		buttons[8];

} mouse_state_t;

bool DI_Init( void );
void DI_Shutdown( void );
void DI_Acquire( void );
void DI_Unacquire( void );
void DI_Poll( void );
bool DI_GetState( mouse_state_t *state );

#endif