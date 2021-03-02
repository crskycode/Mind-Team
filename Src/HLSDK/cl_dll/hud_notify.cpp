#include "cl_dll.h"
#include "cl_util.h"
#include "hud.h"

void CHudNotifyBox::VidInit(void)
{
	m_iBack = gTexAPI.LoadDTX("Tex/UI/PlayMain/MsgBox.dtx");

	memset(m_szMsg, 0, sizeof(m_szMsg));
	m_flDisplay = 0;
}

void CHudNotifyBox::Redraw(void)
{
	if (!m_flDisplay)
		return;

	if (ClientTime > m_flDisplay)
		m_flDisplay = 0;

	int iBaseX = ScreenWide - 280;
	int iBaseY = 3;

	gTexAPI.RenderMode(RenderNormal);
	gTexAPI.DrawSetTexture(m_iBack);
	gTexAPI.DrawSetColor(255, 255, 255, 255);
	gTexAPI.DrawTexturedRect(iBaseX, iBaseY, m_iBack->wide, m_iBack->tall);

	gFontAPI.DrawSetTextFont(gHUD.m_iFont);
	gFontAPI.DrawSetTextColor(255, 255, 255, 255);
	gFontAPI.DrawSetTextPos(iBaseX + 36, iBaseY + 13);
	gFontAPI.DrawPrintText(m_szMsg);
}

void CHudNotifyBox::MsgFunc_Notify(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);

	wcsncpy(m_szMsg, ANSIToUnicode(READ_STRING()), 63);
	m_flDisplay = ClientTime + 3;

	HUD_PlaySound("common/ui_notify.wav", 1);
}