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
#include "fov.h"

extern "C" 
{
	DLLExport void CAM_Think(void);
	DLLExport BOOL CL_IsThirdPerson(void);
	DLLExport void CL_CameraOffset(float *ofs);
}

DLLExport void CAM_Think(void)
{
	gFOV.Update();
}

void CAM_Init(void)
{
}

DLLExport BOOL CL_IsThirdPerson(void)
{
	return FALSE;
}

DLLExport void CL_CameraOffset(float *ofs)
{
}