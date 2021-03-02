#include "cl_dll.h"
#include "hud.h"
#include "weapondata.h"

void CHudAmmo::VidInit(void)
{
	m_pBackground = gTexAPI.LoadDTX("Tex/UI/PlayMain/Ammo_BG.dtx");
	m_pNumber = gTexAPI.LoadDTX("Tex/UI/PlayMain/HP_Num.dtx");

	memset(m_szCacheName, 0, sizeof(m_szCacheName));
	m_iWeaponId = 0;
}

void CHudAmmo::Redraw(void)
{
	if (!ClientState.bIsAlive)
		return;

	if (!ClientState.iWeaponId)
		return;

	int iBaseX = ScreenWide - 249;
	int iBaseY = ScreenTall - 34;

	WeaponData *pData = WeaponData_GetData(ClientState.iWeaponId);

	if (pData)
	{
		static char szIconBG[64], szIconEF[64], szIconLI[64];

		sprintf(szIconBG, "Tex/UI/AmmoIcon/%s_BG.dtx", pData->szBigIconName);
		sprintf(szIconEF, "Tex/UI/AmmoIcon/%s_Effect.dtx", pData->szBigIconName);
		sprintf(szIconLI, "Tex/UI/AmmoIcon/%s_Line.dtx", pData->szBigIconName);

		tex_t *pIconBG = gTexAPI.LoadDTX(szIconBG);
		tex_t *pIconEF = gTexAPI.LoadDTX(szIconEF);
		tex_t *pIconLI = gTexAPI.LoadDTX(szIconLI);

		gTexAPI.RenderMode(RenderNormal);
		gTexAPI.DrawSetTexture(pIconBG);
		gTexAPI.DrawSetColor(255, 255, 255, 180);
		gTexAPI.DrawTexturedRect(iBaseX - 8, iBaseY - 33, pIconBG->wide, pIconBG->tall);

		//gTexAPI.RenderMode(RenderNormal);
		//gTexAPI.DrawSetTexture(pIconEF);
		//gTexAPI.DrawSetColor(255, 255, 255, 255);
		//gTexAPI.DrawTexturedRect(iBaseX - 8, iBaseY - 33, pIconEF->wide, pIconEF->tall);

		gTexAPI.RenderMode(RenderAdditive);
		gTexAPI.DrawSetTexture(pIconLI);
		gTexAPI.DrawSetColor(255, 255, 255, 255);
		gTexAPI.DrawTexturedRect(iBaseX - 8, iBaseY - 33, pIconLI->wide, pIconLI->tall);

		if (ClientState.iWeaponId != m_iWeaponId)
		{
			wcscpy(m_szCacheName, UTF8ToUnicode(pData->szWeaponName));
			m_iWeaponId = ClientState.iWeaponId;
		}

		gFontAPI.DrawSetTextFont(gHUD.m_iFontOutline);
		gFontAPI.DrawSetTextColor(153, 193, 193, 255);
		gFontAPI.DrawSetTextPos(iBaseX + 150, iBaseY - 38);
		gFontAPI.DrawPrintText(m_szCacheName);
	}

	if (ClientState.iClip != -1)
	{
		gTexAPI.RenderMode(RenderNormal);
		gTexAPI.DrawSetTexture(m_pBackground);
		gTexAPI.DrawSetColor(255, 255, 255, 255);
		gTexAPI.DrawTexturedRect(iBaseX, iBaseY, m_pBackground->wide, m_pBackground->tall);

		DrawNumber(ClientState.iClip, iBaseX + 65, iBaseY + 5, 255, 255, 255);
		DrawNumber(ClientState.iAmmo, iBaseX + 110, iBaseY + 5, 255, 255, 255);
	}
}

void CHudAmmo::DrawNumber(int n, int x, int y, int r, int g, int b)
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
	gTexAPI.DrawSetColor(r, g, b, 255);

	for (i = 0; i < 5; i++)
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