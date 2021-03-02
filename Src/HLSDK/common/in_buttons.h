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

#ifndef IN_BUTTONS_H
#define IN_BUTTONS_H

#define IN_ATTACK		(1<<0)
#define IN_JUMP			(1<<1)
#define IN_DUCK			(1<<2)
#define IN_FORWARD		(1<<3)
#define IN_BACK			(1<<4)
#define IN_USE			(1<<5)
#define IN_SPEED		(1<<6)
//7
//8
#define IN_MOVELEFT		(1<<9)
#define IN_MOVERIGHT	(1<<10)
#define IN_ATTACK2		(1<<11)
//12
#define IN_RELOAD		(1<<13)
//14
#define IN_SCORE		(1<<15)		// Used by engine.dll and client.dll for when scoreboard is held down

#endif