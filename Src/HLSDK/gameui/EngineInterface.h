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

#ifndef ENGINEINTERFACE_H
#define ENGINEINTERFACE_H

//#define WIN32_LEAN_AND_MEAN
//#define NOWINRES
//#define NOSERVICE
//#define NOMCX
//#define NOIME
//#include <windows.h>
#include "sys.h"

#ifdef MessageBox
#undef MessageBox
#endif

#ifdef SendMessage
#undef SendMessage
#endif
#ifdef PostMessage
#undef PostMessage
#endif

// Misc C-runtime library headers
#include <stdio.h>
#include <stdlib.h>

#define _USE_MATH_DEFINES
#include <math.h>

#define MAX_QPATH	64

#define DLLExport __declspec(dllexport)

#pragma warning(disable:4244)
#pragma warning(disable:4305)
#pragma warning(disable:4800)

#include <mathlib.h>

// Shared engine/DLL constants
#include <const.h>
#include <cdll_int.h>

#include <keydefs.h>

extern cl_enginefunc_t gEngfuncs;

extern cl_enginefunc_t *engine;

class IGameUIFuncs;
extern IGameUIFuncs *gameuifuncs;

#define PRINT ( *gEngfuncs.Con_Printf )

#endif