#include "cl_dll.h"
#include "hud.h"
#include "WeaponData.h"

void CHudWeaponSlot::VidInit(void)
{
	m_flStartTime = 0;

	memset(&m_rgpPlayerWeapons, 0, sizeof(m_rgpPlayerWeapons));

	m_pWeaponSlot = gTexAPI.LoadDTX("Tex/UI/PlayMain/WeaponSlot.dtx");
}

void CHudWeaponSlot::Redraw(void)
{
	if (!m_flStartTime)
		return;

	// Drift
	const float InTime = 0.3;
	const float HoldTime = 0.3;
	const float OutTime = 0.3;

	// Drift
	const int DriftDist = 20;

	// Fade
	const float FadeTime0 = 1.3;
	const float FadeTime1 = 0.5;

	// Alpha
	const int Alpha0 = 255;
	const int Alpha1 = 150;

	int iBaseX = ScreenWide - 214;
	int iBaseY = ScreenTall / 2 - (ScreenTall - 600) / 10;

	int iOffset = 0;
	int iAlpha0 = 0;
	int iAlpha1 = 0;

	if (m_flStartTime > ClientTime)
	{
		iOffset = 0;
		iAlpha0 = 0;
		iAlpha1 = 0;
	}
	else if (ClientTime > m_flStartTime && ClientTime < m_flStartTime + InTime)
	{
		iOffset = (ClientTime - m_flStartTime) / InTime * DriftDist;
		iAlpha0 = (ClientTime - m_flStartTime) / InTime * Alpha0;
		iAlpha1 = (ClientTime - m_flStartTime) / InTime * Alpha1;
	}
	else if (ClientTime > m_flStartTime + InTime && ClientTime < m_flStartTime + InTime + HoldTime)
	{
		iOffset = DriftDist;
		iAlpha0 = Alpha0;
		iAlpha1 = Alpha1;
	}
	else if (ClientTime > m_flStartTime + InTime + HoldTime && ClientTime < m_flStartTime + InTime + HoldTime + OutTime)
	{
		iOffset = (m_flStartTime + InTime + HoldTime + OutTime - ClientTime) / OutTime * DriftDist;
		iAlpha0 = (m_flStartTime + InTime + HoldTime + OutTime + FadeTime0 - ClientTime) / (OutTime + FadeTime0) * Alpha0;
		iAlpha1 = (m_flStartTime + InTime + HoldTime + OutTime + FadeTime1 - ClientTime) / (OutTime + FadeTime1) * Alpha1;
	}
	else if (ClientTime > m_flStartTime + InTime + HoldTime + OutTime && ClientTime < m_flStartTime + InTime + HoldTime + OutTime + FadeTime0)
	{
		iOffset = 0;
		iAlpha0 = (m_flStartTime + InTime + HoldTime + OutTime + FadeTime0 - ClientTime) / (OutTime + FadeTime0) * Alpha0;

		if (ClientTime < m_flStartTime + InTime + HoldTime + OutTime + FadeTime1)
		{
			iAlpha1 = (m_flStartTime + InTime + HoldTime + OutTime + FadeTime1 - ClientTime) / (OutTime + FadeTime1) * Alpha1;
		}
	}
	else if (ClientTime > m_flStartTime + InTime + HoldTime + OutTime + FadeTime0)
	{
		m_flStartTime = 0;
		iOffset = 0;
		iAlpha0 = 0;
		iAlpha1 = 0;
	}

	gTexAPI.RenderMode(RenderNormal);

	static char szIcon[MAX_QPATH];

	int iDrawX;
	int iDrawY = iBaseY;

	for (int iSlot = 0; iSlot < 4; iSlot++)
	{
		CBasePlayerWeapon *pWeapon = m_rgpPlayerWeapons[iSlot];

		iDrawX = iBaseX;

		int iFix;
		tex_t *pIcon;

		if (iSlot == m_iActiveSlot)
		{
			iDrawX -= iOffset;
			gTexAPI.DrawSetColor(255, 255, 255, iAlpha0);

			if (iSlot != 3)
			{
				gTexAPI.DrawSetTexture(m_pWeaponSlot);
				gTexAPI.DrawTexturedRect(iDrawX, iDrawY, m_pWeaponSlot->wide, m_pWeaponSlot->tall);

				while (pWeapon && pWeapon->m_iId != m_iActiveWeapon)
				{
					pWeapon = pWeapon->m_pNext;
				}

				if (pWeapon)
				{
					GetSelectIcon(pWeapon->m_pData, pIcon, iFix);
					gTexAPI.DrawSetTexture(pIcon);
					gTexAPI.DrawTexturedRect(iDrawX + iFix, iDrawY, pIcon->wide, pIcon->tall);
				}
			}
			else
			{
				//int iSaveX = iDrawX;

				//gTexAPI.DrawSetColor(255, 255, 255, iAlpha1);

				//while (pWeapon)
				//{
				//	if (m_iActiveWeapon != pWeapon->m_iId)
				//	{
				//		GetSelectIcon(pWeapon->m_pData, pIcon, iFixX);

				//		gTexAPI.DrawSetTexture(m_pWeaponSlot);
				//		gTexAPI.DrawTexturedRect(iDrawX, iDrawY, m_pWeaponSlot->wide, m_pWeaponSlot->tall);
				//		gTexAPI.DrawSetTexture(pIcon);
				//		gTexAPI.DrawTexturedRect(iDrawX + iFixX, iDrawY, pIcon->wide, pIcon->tall);
				//	}

				//	iDrawX -= 46;

				//	pWeapon = pWeapon->m_pNext;
				//}

				//iDrawX = iSaveX;

				//gTexAPI.DrawSetColor(255, 255, 255, iAlpha0);

				//pWeapon = m_rgpPlayerWeapons[i];

				//while (pWeapon)
				//{
				//	if (m_iActiveWeapon == pWeapon->m_iId)
				//	{
				//		GetSelectIcon(pWeapon->m_pData, pIcon, iFixX);

				//		gTexAPI.DrawSetTexture(m_pWeaponSlot);
				//		gTexAPI.DrawTexturedRect(iDrawX, iDrawY, m_pWeaponSlot->wide, m_pWeaponSlot->tall);
				//		gTexAPI.DrawSetTexture(pIcon);
				//		gTexAPI.DrawTexturedRect(iDrawX + iFixX, iDrawY, pIcon->wide, pIcon->tall);
				//	}

				//	iDrawX -= 46;

				//	pWeapon = pWeapon->m_pNext;
				//}

				gTexAPI.DrawSetTexture(m_pWeaponSlot);
				gTexAPI.DrawTexturedRect(iDrawX, iDrawY, m_pWeaponSlot->wide, m_pWeaponSlot->tall);

				int iStartX = iDrawX;

				while (pWeapon && pWeapon->m_pNext)
				{
					iStartX -= 46 / 2;
					pWeapon = pWeapon->m_pNext;
				}

				pWeapon = m_rgpPlayerWeapons[iSlot];

				while (pWeapon)
				{
					GetSelectIcon(pWeapon->m_pData, pIcon, iFix);
					gTexAPI.DrawSetTexture(pIcon);

					if (m_iActiveWeapon != pWeapon->m_iId)
						gTexAPI.DrawSetColor(150, 150, 150, iAlpha0);
					else
						gTexAPI.DrawSetColor(255, 255, 255, iAlpha0);

					gTexAPI.DrawTexturedRect(iStartX + iFix, iDrawY, pIcon->wide, pIcon->tall);

					iStartX += 46;

					pWeapon = pWeapon->m_pNext;
				}
			}
		}
		else
		{
			iDrawX += iOffset;
			gTexAPI.DrawSetColor(255, 255, 255, iAlpha1);

			if (iSlot != 3)
			{
				gTexAPI.DrawSetTexture(m_pWeaponSlot);
				gTexAPI.DrawTexturedRect(iDrawX, iDrawY, m_pWeaponSlot->wide, m_pWeaponSlot->tall);

				while (pWeapon && !pWeapon->m_iActive)
				{
					pWeapon = pWeapon->m_pNext;
				}

				if (!pWeapon)
				{
					pWeapon = m_rgpPlayerWeapons[iSlot];
				}

				if (pWeapon)
				{
					GetSelectIcon(pWeapon->m_pData, pIcon, iFix);
					gTexAPI.DrawSetTexture(pIcon);
					gTexAPI.DrawTexturedRect(iDrawX + iFix, iDrawY, pIcon->wide, pIcon->tall);
				}
			}
			else
			{
				//while (pWeapon)
				//{
				//	GetSelectIcon(pWeapon->m_pData, pIcon, iFixX);

				//	gTexAPI.DrawSetTexture(m_pWeaponSlot);
				//	gTexAPI.DrawTexturedRect(iDrawX, iDrawY, m_pWeaponSlot->wide, m_pWeaponSlot->tall);
				//	gTexAPI.DrawSetTexture(pIcon);
				//	gTexAPI.DrawTexturedRect(iDrawX + iFixX, iDrawY, pIcon->wide, pIcon->tall);

				//	iDrawX -= 46;

				//	pWeapon = pWeapon->m_pNext;
				//}

				gTexAPI.DrawSetTexture(m_pWeaponSlot);
				gTexAPI.DrawTexturedRect(iDrawX, iDrawY, m_pWeaponSlot->wide, m_pWeaponSlot->tall);

				int iStartX = iDrawX;

				while (pWeapon && pWeapon->m_pNext)
				{
					iStartX -= 46 / 2;
					pWeapon = pWeapon->m_pNext;
				}

				pWeapon = m_rgpPlayerWeapons[iSlot];

				while (pWeapon)
				{
					GetSelectIcon(pWeapon->m_pData, pIcon, iFix);
					gTexAPI.DrawSetTexture(pIcon);
					gTexAPI.DrawTexturedRect(iStartX + iFix, iDrawY, pIcon->wide, pIcon->tall);

					iStartX += 46;

					pWeapon = pWeapon->m_pNext;
				}
			}
		}

		iDrawY += 59;
	}
}

void CHudWeaponSlot::GetSelectIcon(void *data, tex_t *&icon, int &fix)
{
	static char szIcon[MAX_QPATH];

	sprintf(szIcon, "Tex/UI/WeaponIcon/Weapon_Select_%s.dtx", ((WeaponData *)data)->szBigIconName);
	icon = gTexAPI.LoadDTX(szIcon);
	fix = 0;

	if (!icon->id)
	{
		sprintf(szIcon, "Tex/UI/AmmoIcon/%s_Line.dtx", ((WeaponData *)data)->szBigIconName);
		icon = gTexAPI.LoadDTX(szIcon);
		fix = -55;
	}
}

void CHudWeaponSlot::AddPlayerWeapon(int id)
{
	WeaponData *pData = WeaponData_GetData(id);

	if ( !pData )
		return;

	CBasePlayerWeapon *pInsert = m_rgpPlayerWeapons[pData->iTargetSlot];

	while (pInsert)
	{
		if (pInsert->m_iId == id)
			return;

		pInsert = pInsert->m_pNext;
	}

	CBasePlayerWeapon *pWeapon = new CBasePlayerWeapon;

	pWeapon->m_iId = pData->iWeaponIndex;
	pWeapon->m_iSlot = pData->iTargetSlot;
	pWeapon->m_pData = pData;

	pWeapon->m_pNext = m_rgpPlayerWeapons[pWeapon->m_iSlot];
	m_rgpPlayerWeapons[pWeapon->m_iSlot] = pWeapon;
}

void CHudWeaponSlot::RemovePlayerWeapon(int id)
{
	WeaponData *pData = WeaponData_GetData(id);

	if ( !pData )
		return;

	CBasePlayerWeapon **pIter = &m_rgpPlayerWeapons[pData->iTargetSlot];
	CBasePlayerWeapon *pTemp;

	if (id == (*pIter)->m_iId)
	{
		pTemp = *pIter;
		*pIter = (*pIter)->m_pNext;
		delete pTemp;
	}
	else
	{
		while (*pIter && (*pIter)->m_pNext->m_iId != id)
		{
			pIter = &(*pIter)->m_pNext;
		}

		if (*pIter)
		{
			pTemp = (*pIter)->m_pNext;
			(*pIter)->m_pNext = (*pIter)->m_pNext->m_pNext;
			delete pTemp;
		}
	}
}

void CHudWeaponSlot::MsgFunc_WeapSlot(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);

	if (READ_BYTE())
		AddPlayerWeapon(READ_SHORT());
	else
		RemovePlayerWeapon(READ_SHORT());
}

void CHudWeaponSlot::MsgFunc_WeapSlotA(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);

	WeaponData *pData = WeaponData_GetData( READ_SHORT() );

	if ( !pData )
		return;

	CBasePlayerWeapon *pWeapon = m_rgpPlayerWeapons[pData->iTargetSlot];

	while (pWeapon)
	{
		if (pData->iWeaponIndex == pWeapon->m_iId)
			pWeapon->m_iActive = 1;
		else
			pWeapon->m_iActive = 0;

		pWeapon = pWeapon->m_pNext;
	}

	m_iActiveWeapon = pData->iWeaponIndex;
	m_iActiveSlot = pData->iTargetSlot;
	m_flStartTime = ClientTime;
}