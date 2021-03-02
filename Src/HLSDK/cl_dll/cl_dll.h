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

#ifndef CL_DLL_H
#define CL_DLL_H

#define WIN32_LEAN_AND_MEAN
#define NOWINRES
#define NOSERVICE
#define NOMCX
#define NOIME
#include <windows.h>
#include "sys.h"

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

// Vector class
#ifndef NOVECTOR
#include "util_vector.h"
#endif

// Shared engine/DLL constants
#include <const.h>
#include <cdll_int.h>
#include <triangleapi.h>

extern cl_enginefunc_t gEngfuncs;

#endif