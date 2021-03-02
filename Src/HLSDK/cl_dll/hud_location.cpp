#include "cl_dll.h"
#include "hud.h"

void CHudLocation::Init(void)
{
	wcscpy(m_szLocation, L"保卫者基地");
}

void CHudLocation::Redraw(void)
{
	int iBaseX = 80;
	int iBaseY = 178;

	//int iTextWide, iTextTall;

	//gFontAPI.GetTextSize(gHUD.m_iFontOutline, m_szLocation, iTextWide, iTextTall);

	gFontAPI.DrawSetTextFont(gHUD.m_iFontOutline);
	gFontAPI.DrawSetTextColor(255, 255, 255, 255);
	gFontAPI.DrawSetTextPos(iBaseX /*- iTextWide / 2*/, iBaseY /*- iTextTall / 2*/);
	gFontAPI.DrawCenterText(m_szLocation);
}