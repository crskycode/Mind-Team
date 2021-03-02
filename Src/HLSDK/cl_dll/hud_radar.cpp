#include "cl_dll.h"
#include "hud.h"

#define MAPWIDE	1024
#define MAPTALL	768

void CHudRadar::VidInit(void)
{
	m_pBackground = gTexAPI.LoadDTX("Tex/UI/Radar/RadarBG.dtx");
	m_pFrame = gTexAPI.LoadDTX("Tex/UI/Radar/RadarFrame.dtx");
	m_pMapIcon = NULL;
	m_pRadarIcon = gTexAPI.LoadDTX("Tex/UI/Radar/RadarIcon.dtx");

	m_bDisplay = true;
	m_fFrameYaw = 0;
}

void CHudRadar::DataInit(void)
{
	m_pMapIcon = gTexAPI.LoadTGA("UI/MapIcon/template0000.tga");

	m_OverviewData.bRotated = true;

	m_OverviewData.vecOrigin.x = 0.00f;
	m_OverviewData.vecOrigin.y = 0.00f;

	m_OverviewData.flZoom = 2.67f;
}

#define HUD_BASE_X -44
#define HUD_BASE_Y -42
#define HUD_SIZE_X 256
#define HUD_SIZE_Y 256
#define HUD_CENTER_X ( HUD_BASE_X + ( HUD_SIZE_X / 2 ) )
#define HUD_CENTER_Y ( HUD_BASE_Y + ( HUD_SIZE_Y / 2 ) )

#define MAP_SCALE 0.3f

void CHudRadar::Redraw(void)
{
	if (!m_bDisplay)
		return;

	if (m_OverviewData.bRotated)
		m_flEyeYaw = UTIL_AngleMod( ClientState.vecAngles[1] - 90.0f );
	else
		m_flEyeYaw = UTIL_AngleMod( ClientState.vecAngles[1] );

	Vector vecWorldPos = Vector(ClientState.vecOrigin);

	WorldToOverview(vecWorldPos, m_vecEyePos);

	m_vecEyePos *= MAP_SCALE;

	gTexAPI.RenderMode(RenderNormal);

	if (m_pMapIcon)
	{
		gTexAPI.DrawSetColor(255, 255, 255, 180);
		gTexAPI.DrawSetTexture(m_pMapIcon);
		gTexAPI.DrawEnableRotate(m_vecEyePos.x, m_vecEyePos.y, m_flEyeYaw);
		gTexAPI.DrawEnableCircleScissor(HUD_CENTER_X, HUD_CENTER_Y, 66);
		gTexAPI.DrawTexturedRect(HUD_CENTER_X - m_vecEyePos.x, HUD_CENTER_Y - m_vecEyePos.y, MAPWIDE*MAP_SCALE, MAPTALL*MAP_SCALE);
		gTexAPI.DrawDisableCircleScissor();
	}

	gTexAPI.DrawSetColor(255, 255, 255, 255);
	gTexAPI.DrawSetTexture(m_pBackground);
	gTexAPI.DrawTexturedRect(HUD_BASE_X, HUD_BASE_Y, m_pBackground->wide, m_pBackground->tall);

	float fDelta = UTIL_AngleDistanceAbs(m_flEyeYaw, m_fFrameYaw);
	float fSpeed = fDelta * 0.1;

	m_fFrameYaw = UTIL_ApproachAngle(m_flEyeYaw, m_fFrameYaw, fSpeed);

	gTexAPI.DrawSetTexture(m_pFrame);
	gTexAPI.DrawEnableRotate(HUD_SIZE_X / 2, HUD_SIZE_Y / 2, m_fFrameYaw);
	gTexAPI.DrawTexturedRect(HUD_BASE_X, HUD_BASE_Y, m_pFrame->wide, m_pFrame->tall);

	DrawBombPoint();
	DrawC4();
	DrawPlayer();
	DrawRadio();
}

void CHudRadar::DrawPlayer(void)
{
	int iLocalPlayer = gEngfuncs.GetLocalPlayer()->index;

	gTexAPI.RenderMode(RenderNormal);
	gTexAPI.DrawSetColor(255, 255, 255, 255);
	gTexAPI.DrawSetTexture(m_pRadarIcon);

	for (int i = 0; i < 32; i++)
	{
		cl_entity_t *pEntity = gEngfuncs.GetEntityByIndex(i + 1);

		if (!pEntity)
		{
			continue;
		}

		if (!pEntity->model)
		{
			continue;
		}

		if (ClientState.iUser1 != OBS_NONE)
		{
			if (pEntity->index == ClientState.iUser2)
				continue;
		}
		else
		{
			if (pEntity->index == iLocalPlayer)
				continue;
		}

		Vector vecWorldPos;
		Vector vecRadarPos;
		bool bOutSide;
		float flArrowAngle;

		vecWorldPos = Vector(pEntity->origin);

		WorldToRadar(vecWorldPos, vecRadarPos, &bOutSide, &flArrowAngle);

		if (!bOutSide)
		{
			Vector vecEye = Vector(ClientState.vecOrigin);	// world space

			gTexAPI.DrawDisableRotate();

			gTexAPI.TextureEnableScissor(7, 39, 4, 4);
			gTexAPI.DrawTexturedRect(vecRadarPos.x - 2, vecRadarPos.y - 2, 4, 4);

			// up
			if (vecEye.z - vecWorldPos.z < -64.0f)
			{
				gTexAPI.TextureEnableScissor(6, 46, 6, 1);
				gTexAPI.DrawTexturedRect(vecRadarPos.x - 3, vecRadarPos.y - 3, 6, 1);
			}
			// down
			else if (vecEye.z - vecWorldPos.z > 64.0f)
			{
				gTexAPI.TextureEnableScissor(6, 46, 6, 1);
				gTexAPI.DrawTexturedRect(vecRadarPos.x - 3, vecRadarPos.y + 2, 6, 1);
			}
		}
		else
		{
			gTexAPI.TextureEnableScissor(33, 39, 4, 7);
			gTexAPI.DrawEnableRotate(2, 3, flArrowAngle);
			gTexAPI.DrawTexturedRect(vecRadarPos.x - 2, vecRadarPos.y - 3, 4, 7);
		}
	}
}

void CHudRadar::DrawRadio(void)
{
}

void CHudRadar::DrawBombPoint(void)
{
	Vector vecPointA, vecPointB;

	vecPointA = Vector(100, 100, -219);
	vecPointB = Vector(-100, -100, -219);

	gTexAPI.RenderMode(RenderNormal);
	gTexAPI.DrawSetColor(255, 255, 255, 255);
	gTexAPI.DrawSetTexture(m_pRadarIcon);

	Vector vecRadarPos;

	Vector vecEye = Vector(ClientState.vecOrigin);	// world space

	// Draw point A

	WorldToRadar(vecPointA, vecRadarPos, NULL, NULL);

	gTexAPI.TextureEnableScissor(6, 6, 14, 14);
	gTexAPI.DrawTexturedRect(vecRadarPos.x - 7, vecRadarPos.y - 7, 14, 14);

	// up
	if (vecEye.z - vecPointA.z < -64.0f)
	{
		gTexAPI.TextureEnableScissor(5, 32, 14, 2);
		gTexAPI.DrawTexturedRect(vecRadarPos.x - 7, vecRadarPos.y - 8, 14, 2);
	}
	// down
	else if (vecEye.z - vecPointA.z > 64.0f)
	{
		gTexAPI.TextureEnableScissor(5, 27, 14, 2);
		gTexAPI.DrawTexturedRect(vecRadarPos.x - 7, vecRadarPos.y + 6, 14, 2);
	}

	// Draw point B

	WorldToRadar(vecPointB, vecRadarPos, NULL, NULL);

	gTexAPI.TextureEnableScissor(24, 6, 14, 14);
	gTexAPI.DrawTexturedRect(vecRadarPos.x - 7, vecRadarPos.y - 7, 14, 14);

	// up
	if (vecEye.z - vecPointB.z < -64.0f)
	{
		gTexAPI.TextureEnableScissor(5, 32, 14, 2);
		gTexAPI.DrawTexturedRect(vecRadarPos.x - 7, vecRadarPos.y - 8, 14, 2);
	}
	// down
	else if (vecEye.z - vecPointB.z > 64.0f)
	{
		gTexAPI.TextureEnableScissor(5, 27, 14, 2);
		gTexAPI.DrawTexturedRect(vecRadarPos.x - 7, vecRadarPos.y + 6, 14, 2);
	}
}

void CHudRadar::DrawC4(void)
{
	Vector vecC4Point = Vector(150, 150, -219);

	gTexAPI.RenderMode(RenderNormal);
	gTexAPI.DrawSetColor(255, 255, 255, 255);
	gTexAPI.DrawSetTexture(m_pRadarIcon);

	Vector vecRadarPos;

	Vector vecEye = Vector(ClientState.vecOrigin);	// world space

	WorldToRadar(vecC4Point, vecRadarPos, NULL, NULL);

	gTexAPI.TextureEnableScissor(45, 6, 13, 15);
	gTexAPI.DrawTexturedRect(vecRadarPos.x - 6, vecRadarPos.y - 15, 13, 15);

	// up
	if (vecEye.z - vecC4Point.z < -64.0f)
	{
		gTexAPI.TextureEnableScissor(45, 27, 13, 2);
		gTexAPI.DrawTexturedRect(vecRadarPos.x - 6, vecRadarPos.y - 17, 13, 2);
	}
	// down
	else if (vecEye.z - vecC4Point.z > 64.0f)
	{
		gTexAPI.TextureEnableScissor(45, 22, 13, 2);
		gTexAPI.DrawTexturedRect(vecRadarPos.x - 6, vecRadarPos.y - 1, 13, 2);
	}
}

void CHudRadar::WorldToOverview(Vector &vecWorldPos, Vector &vecOverviewPos)
{
	if (m_OverviewData.bRotated)
	{
		vecOverviewPos.x = ((float)MAPWIDE / 2.0f) + ((vecWorldPos.x - m_OverviewData.vecOrigin.x) / 8.0f * m_OverviewData.flZoom);
		vecOverviewPos.y = ((float)MAPTALL / 2.0f) - ((vecWorldPos.y - m_OverviewData.vecOrigin.y) / 8.0f * m_OverviewData.flZoom);
	}
	else
	{
		vecOverviewPos.x = ((float)MAPWIDE / 2.0f) - ((vecWorldPos.y - m_OverviewData.vecOrigin.y) / 8.0f * m_OverviewData.flZoom);
		vecOverviewPos.y = ((float)MAPTALL / 2.0f) - ((vecWorldPos.x - m_OverviewData.vecOrigin.x) / 8.0f * m_OverviewData.flZoom);
	}
}

void CHudRadar::WorldToRadar(Vector &vecWorldPos, Vector &vecRadarPos, bool *bOutSide, float *flAngle)
{
	// vecEyePos	- Overview sapce (scaled)

	Vector vecOverviewPos;

	WorldToOverview(vecWorldPos, vecOverviewPos);

	vecOverviewPos *= MAP_SCALE;

	// 
	Vector vecTemp = m_vecEyePos - vecOverviewPos;

	Vector vecCenter(HUD_CENTER_X, HUD_CENTER_Y, 0.0f);

	if (vecTemp.Length2D() > 63.0f)
	{
		Vector vecTemp2 = vecCenter - vecTemp;

		// Apply yaw
		UTIL_Rotate2D(vecTemp2, vecCenter, m_flEyeYaw);

		Vector vecDir = (vecTemp2 - vecCenter).Normalize();	// center to point
		Vector vecRadius = vecDir * 63.0f;
		Vector vecSub = (vecTemp2 - vecCenter) - vecRadius;
		vecRadarPos = vecTemp2 - vecSub;	// move to center

		// Get angle hud space
		Vector vecArrowAngles;
		VectorAngles(vecDir, vecArrowAngles);

		if (bOutSide)
			*bOutSide = true;

		if (flAngle)
			*flAngle = vecArrowAngles.y;
	}
	else
	{
		vecRadarPos = vecCenter - vecTemp;

		// Apply yaw
		UTIL_Rotate2D(vecRadarPos, vecCenter, m_flEyeYaw);

		if (bOutSide)
			*bOutSide = false;

		if (flAngle)
			*flAngle = 0.0f;
	}
}

bool CHudRadar::IsVisible(void)
{
	return m_bDisplay;
}

void CHudRadar::Show(void)
{
	m_bDisplay = true;
}

void CHudRadar::Hide(void)
{
	m_bDisplay = false;
}