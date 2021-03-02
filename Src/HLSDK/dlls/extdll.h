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

#ifndef EXTDLL_H
#define EXTDLL_H

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
#include <assert.h>
#include <crtdbg.h>

// Use STL
#include <vector>
#include <string>

#define snprintf _snprintf

#define _USE_MATH_DEFINES
#include <math.h>

#define MAX_OSPATH	MAX_PATH
#define MAX_QPATH	64

#define DLLExport __declspec(dllexport)

#pragma warning(disable:4244)
#pragma warning(disable:4305)
#pragma warning(disable:4800)
#pragma warning(disable:4996)

// Vector class
#include "vector.h"

// Defining it as a (bogus) struct helps enforce type-checking
#ifndef NOVECTOR
#define vec3_t Vector
#endif

// Shared engine/DLL constants
#include <const.h>
#include <progdefs.h>
#include <edict.h>

// Shared header describing protocol between engine and DLLs
#include <eiface.h>

#define DLL_GLOBAL

#endif