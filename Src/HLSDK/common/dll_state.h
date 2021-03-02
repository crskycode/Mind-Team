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

#ifndef DLL_STATE_H
#define DLL_STATE_H

//DLL State Flags
#define DLL_INACTIVE		0	// no dll
#define DLL_ACTIVE			1	// dll is running
#define DLL_PAUSED			2	// dll is paused
#define DLL_CLOSE			3	// closing down dll
#define DLL_TRANS			4	// Level Transition

// DLL Pause reasons
#define DLL_NORMAL			0	// User hit Esc or something.
#define DLL_QUIT			4	// Quit now
#define DLL_RESTART			6	// Switch to launcher for linux, does a quit but returns 1

// DLL Substate info ( not relevant )
#define ENG_NORMAL			(1<<0)

#endif