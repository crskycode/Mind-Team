#include "cl_dll.h"
#include "hud.h"

void CHudGameTip::VidInit(void)
{
	m_flHoldTime = 0;
}

void CHudGameTip::Redraw(void)
{
	if (!m_flHoldTime)
		return;

	int iBaseX = ScreenWide / 2;
	int iBaseY = 190;

	int iAlpha;
	float flTime = ClientTime - m_flStartTime;

	if (flTime < m_flHoldTime)
	{
		iAlpha = 255;
	}
	else
	{
		iAlpha = (1 - (flTime - m_flHoldTime) / 1.0) * 255;

		if (flTime - m_flHoldTime > 1)
		{
			m_flHoldTime = 0;
			return;
		}
	}

	gFontAPI.DrawSetTextFont(gHUD.m_iFontOutline);
	gFontAPI.DrawSetTextColor(128, 255, 128, iAlpha);

	if (m_szText1)
	{
		gFontAPI.DrawSetTextPos(iBaseX - m_iWide1 / 2, iBaseY);
		gFontAPI.DrawPrintText(m_szText1);
	}

	if (m_szText2)
	{
		gFontAPI.DrawSetTextPos(iBaseX - m_iWide2 / 2, iBaseY + 20);
		gFontAPI.DrawPrintText(m_szText2);
	}
}

void CHudGameTip::MsgFunc_GameTip(const char *pszName, int iSize, void *pbuf)
{
	if (m_szText1)
	{
		delete[] m_szText1;
		m_szText1 = NULL;
	}

	if (m_szText2)
	{
		delete[] m_szText2;
		m_szText2 = NULL;
	}

	BEGIN_READ(pbuf, iSize);

	int twide, ttall;

	wchar_t *wsz1 = ANSIToUnicode(READ_STRING());
	
	if (*wsz1)
	{
		m_szText1 = new wchar_t[wcslen(wsz1) + 1];
		wcscpy(m_szText1, wsz1);
		gFontAPI.GetTextSize(gHUD.m_iFontOutline, m_szText1, twide, ttall);
		m_iWide1 = twide;
	}

	wchar_t *wsz2 = ANSIToUnicode(READ_STRING());

	if (*wsz2)
	{
		m_szText2 = new wchar_t[wcslen(wsz2) + 1];
		wcscpy(m_szText2, wsz2);
		gFontAPI.GetTextSize(gHUD.m_iFontOutline, m_szText2, twide, ttall);
		m_iWide2 = twide;
	}

	m_flStartTime = ClientTime;
	m_flHoldTime = 3;
}