#include "cl_dll.h"
#include "hud.h"

void CHudHealth::VidInit(void)
{
	m_pBackgroundArmor = gTexAPI.LoadDTX("Tex/UI/TCModeUI/AC.dtx");
	m_pBackgroundHealth = gTexAPI.LoadDTX("Tex/UI/PlayMain/HP_MAIN.dtx");
	m_pNumber = gTexAPI.LoadDTX("Tex/UI/PlayMain/HP_NUM.dtx");
}

void CHudHealth::Redraw(void)
{
	if (!ClientState.bIsAlive)
		return;

	int iBaseX = 107;
	int iBaseY = ScreenTall - 56;

	gTexAPI.RenderMode(RenderNormal);
	gTexAPI.DrawSetColor(255, 255, 255, 255);

	gTexAPI.DrawSetTexture(m_pBackgroundArmor);
	gTexAPI.DrawTexturedRect(iBaseX, iBaseY, m_pBackgroundArmor->wide, m_pBackgroundArmor->tall);

	DrawNumber(ClientState.iArmor, iBaseX + 44, iBaseY + 5);

	gTexAPI.DrawSetTexture(m_pBackgroundHealth);
	gTexAPI.DrawTexturedRect(iBaseX, iBaseY + 22, m_pBackgroundHealth->wide, m_pBackgroundHealth->tall);

	DrawNumber(ClientState.iHealth, iBaseX + 44, iBaseY + 27);
}

void CHudHealth::DrawNumber(int n ,int x, int y)
{
	int c;
	int dx, dy;
	int i;
	int s;
	int v;

	static int rc[10][4] = 
	{
		{ 109, 0, 10, 11 },
		{   1, 0, 10, 11 },
		{  13, 0, 10, 11 },
		{  25, 0, 10, 11 },
		{  37, 0, 10, 11 },
		{  49, 0, 10, 11 },
		{  61, 0, 10, 11 },
		{  73, 0, 10, 11 },
		{  85, 0, 10, 11 },
		{  97, 0, 10, 11 },
	};

	dx = x;
	dy = y;

	c = 0;
	s = 1;

	gTexAPI.DrawSetTexture(m_pNumber);

	for (i = 0; i < 5; i++)
	{
		v = n / s % 10;

		if (i == 0 || v || n >= s)
		{
			c = v;

			gTexAPI.TextureEnableScissor(rc[c][0], rc[c][1], rc[c][2], rc[c][3]);
			gTexAPI.DrawTexturedRect(dx, dy, rc[c][2], rc[c][3]);

			dx = dx - 12;
		}
		else
		{
			break;
		}

		s = s * 10;
	}
}