#include "cl_dll.h"
#include "hud.h"

void CHudGauge::VidInit(void)
{
	m_flStartTime = 0;
	m_flHoldTime = 0;

	m_pGaugeBoxR = gTexAPI.LoadDTX("tex/UI/Gauge/GaugeBox_R.dtx");
	m_pGaugeC = gTexAPI.LoadDTX("tex/UI/Gauge/Gauge_C.dtx");
	m_pGaugeEffectC = gTexAPI.LoadDTX("tex/UI/Gauge/GaugeEffect_C.dtx");
}

void CHudGauge::Redraw(void)
{
	if (!m_flHoldTime)
		return;

	int iBaseX = ScreenWide / 2 - 160;
	int iBaseY = ScreenTall - 90;

	float percen = (ClientTime - m_flStartTime) / m_flHoldTime;

	if (percen > 1)
	{
		m_flHoldTime = 0;
		return;
	}

	gTexAPI.RenderMode(RenderNormal);
	gTexAPI.DrawSetColor(255, 255, 255, 255);

	gTexAPI.DrawSetTexture(m_pGaugeBoxR);
	gTexAPI.DrawTexturedRect(iBaseX, iBaseY, m_pGaugeBoxR->wide, m_pGaugeBoxR->tall);

	int iSize = percen * 300;

	gTexAPI.DrawSetTexture(m_pGaugeC);
	gTexAPI.DrawTexturedRect(iBaseX + 9, iBaseY + 5, iSize, m_pGaugeC->tall);

	gTexAPI.DrawSetTexture(m_pGaugeEffectC);
	gTexAPI.DrawTexturedRect(iBaseX + 9 + iSize - 28, iBaseY + 1, m_pGaugeEffectC->wide, m_pGaugeEffectC->tall);

	gFontAPI.DrawSetTextFont(gHUD.m_iFont);
	gFontAPI.DrawSetTextColor(255, 255, 255, 255);
	gFontAPI.DrawSetTextPos(iBaseX + 122, iBaseY + 6);
	gFontAPI.DrawPrintText(L"ÎÞµÐÊ±¼ä >>");
}

void CHudGauge::MsgFunc_Gauge(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);

	int type = READ_BYTE();
	float time = READ_BYTE();

	m_flStartTime = ClientTime;
	m_flHoldTime = time;
}