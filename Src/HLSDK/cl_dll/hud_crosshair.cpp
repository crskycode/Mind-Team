#include "cl_dll.h"
#include "hud.h"
#include "WeaponData.h"

void CHudCrosshair::Redraw(void)
{
	if (!IsFirstPerson)
		return;

	WeaponData *pWeapon = WeaponData_GetData(ClientState.iWeaponId);

	if (!pWeapon)
		return;

	if (pWeapon->iWeaponClass == WeaponClass_Sniper)
		return;

	int iDistance = 8;
	int iDeltaDistance = 3;

	if (ClientState.iFlags & FL_ONGROUND)
	{
		if (ClientState.iFlags & FL_DUCKING)
		{
			iDistance *= 0.5;
		}
		else
		{
			float flLimitSpeed = 140;

			if (VectorLength(ClientState.vecVelocity) > flLimitSpeed)
			{
				iDistance *= 1.5;
			}
		}
	}
	else
	{
		iDistance *= 2.2;
	}

	//if (g_iShotsFired > m_iAmmoLastCheck)
	//{
	//	m_flCrosshairDistance += iDeltaDistance;
	//
	//	if (m_flCrosshairDistance > 15)
	//	{
	//		m_flCrosshairDistance = 15;
	//	}
	//}
	//else
	{
		m_flCrosshairDistance -= (0.013 * m_flCrosshairDistance) + 0.1;
	}

	if (iDistance > m_flCrosshairDistance)
	{
		m_flCrosshairDistance = iDistance;
	}

	int iBaseX = ScreenWide / 2;
	int iBaseY = ScreenTall / 2;

	int size = 5;
	int dist = m_flCrosshairDistance;

	gEngfuncs.pfnFillRGBA(iBaseX, iBaseY, 1, 1, 0, 255, 0, 255);

	gEngfuncs.pfnFillRGBA(iBaseX, iBaseY - dist - size, 1, size, 0, 255, 0, 255);	// top
	gEngfuncs.pfnFillRGBA(iBaseX, iBaseY + dist + 1, 1, size, 0, 255, 0, 255);	// bottom
	gEngfuncs.pfnFillRGBA(iBaseX - dist - size, iBaseY, size, 1, 0, 255, 0, 255);	// left
	gEngfuncs.pfnFillRGBA(iBaseX + dist + 1, iBaseY, size, 1, 0, 255, 0, 255);	// right
}