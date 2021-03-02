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

#ifndef FOV_H
#define FOV_H

class CFOV
{
public:
	CFOV();

	void Set(int fov, float time);
	int Get(void);
	void Update(void);
	bool Finish(void);

	int m_iOldFOV;
	int m_iFOV;

	float m_flStartChange;
	float m_flChangeTime;

	int m_iCurrentFOV;
};

extern CFOV gFOV;

#endif