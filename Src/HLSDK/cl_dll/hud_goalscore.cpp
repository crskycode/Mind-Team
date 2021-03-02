#include "cl_dll.h"
#include "hud.h"

void CHudGoalScore::VidInit(void)
{
	m_pBackgroundBL = gTexAPI.LoadDTX("Tex/UI/PlayMain/GoalScore_BL.dtx");
	m_pBackgroundGR = gTexAPI.LoadDTX("Tex/UI/PlayMain/GoalScore_GR.dtx");
	m_pWhiteNumber = gTexAPI.LoadDTX("Tex/UI/PlayMain/MiddleNum_White.dtx");
	m_pGrayNumber = gTexAPI.LoadDTX("Tex/UI/PlayMain/MiddleNum_Gray.dtx");
	m_pSmallNumber = gTexAPI.LoadDTX("Tex/UI/PlayMain/SmallNum_Gray.dtx");

	m_flNextFlash = 0;
	m_fFlash = FALSE;
}

void CHudGoalScore::Redraw(void)
{
	if (GameState.iGameMode == GAMEMODE_TD)
	{
		int iBaseX = ScreenWide / 2 - m_pBackgroundBL->wide / 2;
		int iBaseY = 3;

		// Shift to 0~31
		int nIndex = LocalIndex - 1;

		switch (GameState.m_PlayerTeam[nIndex])
		{
			case TEAM_BL:
			{
				gTexAPI.RenderMode(RenderNormal);
				gTexAPI.DrawSetTexture(m_pBackgroundBL);
				gTexAPI.DrawSetColor(255, 255, 255, 255);
				gTexAPI.DrawTexturedRect(iBaseX, iBaseY, m_pBackgroundBL->wide, m_pBackgroundBL->tall);

				DrawTimer(iBaseX, iBaseY);

				if (GameState.iSubRuleType == 0)	//SUBRULETYPE_KILL
					DrawCenterNumber(GameState.iRoundKill, iBaseX + 119, iBaseY + 5, m_pWhiteNumber, 108, 135, 137);

				DrawCenterNumber(GameState.iScoreBL, iBaseX + 75, iBaseY + 5, m_pWhiteNumber, 247, 247, 247);
				DrawCenterNumber(GameState.iScoreGR, iBaseX + 165, iBaseY + 5, m_pWhiteNumber, 174, 174, 174);

				break;
			}

			case TEAM_GR:
			{
				gTexAPI.RenderMode(RenderNormal);
				gTexAPI.DrawSetTexture(m_pBackgroundGR);
				gTexAPI.DrawSetColor(255, 255, 255, 255);
				gTexAPI.DrawTexturedRect(iBaseX, iBaseY, m_pBackgroundGR->wide, m_pBackgroundGR->tall);

				DrawTimer(iBaseX, iBaseY);

				if (GameState.iSubRuleType == 0)	//SUBRULETYPE_KILL
					DrawCenterNumber(GameState.iRoundKill, iBaseX + 119, iBaseY + 5, m_pWhiteNumber, 108, 135, 137);

				DrawCenterNumber(GameState.iScoreGR, iBaseX + 75, iBaseY + 5, m_pWhiteNumber, 247, 247, 247);
				DrawCenterNumber(GameState.iScoreBL, iBaseX + 165, iBaseY + 5, m_pWhiteNumber, 174, 174, 174);

				break;
			}
		}
	}
}

void CHudGoalScore::DrawCenterNumber(int n, int x, int y, tex_t *tex, int r, int g, int b)
{
	int c;
	int dx, dy;
	int i;
	int s;
	int v;

	static int rc[10][4] = 
	{
		{ 127, 0, 13, 13 },
		{   1, 0, 13, 13 },
		{  15, 0, 13, 13 },
		{  29, 0, 13, 13 },
		{  43, 0, 13, 13 },
		{  57, 0, 13, 13 },
		{  71, 0, 13, 13 },
		{  85, 0, 13, 13 },
		{  99, 0, 13, 13 },
		{ 113, 0, 13, 13 },
	};

	dx = x;
	dy = y;

	// <--

	if (n > 9)
		dx += 6;
	if (n > 99)
		dx += 5;

	c = 0;
	s = 1;

	gTexAPI.RenderMode(RenderNormal);
	gTexAPI.DrawSetTexture(tex);
	gTexAPI.DrawSetColor(r, g, b, 255);

	for (i = 0; i < 3; i++)
	{
		v = n / s % 10;

		if (i == 0 || v || n >= s)
		{
			c = v;

			gTexAPI.TextureEnableScissor(rc[c][0], rc[c][1], rc[c][2], rc[c][3]);
			gTexAPI.DrawTexturedRect(dx, dy, rc[c][2], rc[c][3]);

			dx = dx - 11;
		}
		else
		{
			break;
		}

		s = s * 10;
	}
}

void CHudGoalScore::DrawSmallNumber(int n, int x, int y, int r, int g, int b)
{
	static int rc[10][4] = 
	{
		{ 81, 0, 9, 10 },
		{  0, 0, 9, 10 },
		{  9, 0, 9, 10 },
		{ 18, 0, 9, 10 },
		{ 27, 0, 9, 10 },
		{ 36, 0, 9, 10 },
		{ 45, 0, 9, 10 },
		{ 54, 0, 9, 10 },
		{ 63, 0, 9, 10 },
		{ 72, 0, 9, 10 },
	};

	gTexAPI.RenderMode(RenderNormal);
	gTexAPI.DrawSetTexture(m_pSmallNumber);
	gTexAPI.DrawSetColor(r, g, b, 255);
	gTexAPI.TextureEnableScissor(rc[n][0], rc[n][1], rc[n][2], rc[n][3]);
	gTexAPI.DrawTexturedRect(x, y, rc[n][2], rc[n][3]);
}

void CHudGoalScore::DrawTimer(int x, int y)
{
	int m, s, m1, m2, s1, s2;

	int iRemaining = GameState.iRoundTime - ClientTime;

	if (iRemaining < 0)
		iRemaining = 0;

	if (iRemaining <= 60)
	{
		if (iRemaining == 0)
		{
			m_fFlash = TRUE;
		}
		else if (ClientTime > m_flNextFlash)
		{
			m_fFlash = !m_fFlash;
			m_flNextFlash = ClientTime + 0.02;
		}
	}
	else
	{
		m_fFlash = FALSE;
	}

	m = iRemaining / 60 % 60;
	s = iRemaining % 60;

	m1 = m / 10 % 10;
	m2 = m % 10;
	s1 = s / 10 % 10;
	s2 = s % 10;

	DrawSmallNumber(m1, x + 104, y + 25, 255, (m_fFlash ? 0 : 255), (m_fFlash ? 0 : 255));
	DrawSmallNumber(m2, x + 113, y + 25, 255, (m_fFlash ? 0 : 255), (m_fFlash ? 0 : 255));
	DrawSmallNumber(s1, x + 132, y + 25, 255, (m_fFlash ? 0 : 255), (m_fFlash ? 0 : 255));
	DrawSmallNumber(s2, x + 141, y + 25, 255, (m_fFlash ? 0 : 255), (m_fFlash ? 0 : 255));
}