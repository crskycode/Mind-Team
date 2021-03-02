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

CFOV gFOV;

CFOV::CFOV()
{
	m_iOldFOV = 0;
	m_iFOV = 0;
	m_flStartChange = 0;
	m_flChangeTime = 0;
	m_iCurrentFOV = 0;
}

void CFOV::Set(int fov, float time)
{
	m_iOldFOV = m_iFOV;
	m_iFOV = fov;
	m_flStartChange = gEngfuncs.GetClientTime();
	m_flChangeTime = time;
}

int CFOV::Get(void)
{
	return m_iCurrentFOV;
}

void CFOV::Update(void)
{
	float flTime = gEngfuncs.GetClientTime();

	if (flTime < m_flStartChange)
	{
		m_iCurrentFOV = m_iFOV;
	}
	else if (flTime > m_flStartChange && flTime < m_flStartChange + m_flChangeTime)
	{
		m_iCurrentFOV = m_iOldFOV + (flTime - m_flStartChange) / m_flChangeTime * (m_iFOV - m_iOldFOV);
	}
	else if (flTime > m_flStartChange + m_flChangeTime)
	{
		m_iCurrentFOV = m_iFOV;
	}
}

bool CFOV::Finish(void)
{
	float flTime = gEngfuncs.GetClientTime();

	if (flTime > m_flStartChange + m_flChangeTime)
		return true;

	return false;
}