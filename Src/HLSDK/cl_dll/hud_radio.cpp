/***
 * 
 * Copyright (c) 1996-2002, Valve LLC. All rights reserved.
 * 
 * This product contains software technology licensed from Id
 * Software, Inc. ("Id Technology"). Id Technology (c) 1996 Id Software, Inc.
 * All Rights Reserved.
 * 
 * Use, distribution, and modification of this source code and/or resulting
 * object code is restricted to non-commercial enhancements to products from
 * Valve LLC. All other use, distribution, or modification is prohibited
 * without written permission from Valve LLC.
 * 
***/

#include "cl_dll.h"
#include "hud.h"

static void _Radio1(void)
{
	gHUD.m_Radio.Show(1);
}

static void _Radio2(void)
{
	gHUD.m_Radio.Show(2);
}

static void _Radio3(void)
{
	gHUD.m_Radio.Show(3);
}

void CHudRadio::Init(void)
{
	gEngfuncs.pfnAddCommand("radio1", _Radio1);
	gEngfuncs.pfnAddCommand("radio2", _Radio2);
	gEngfuncs.pfnAddCommand("radio3", _Radio3);
}

void CHudRadio::VidInit(void)
{
	m_iGroup = 0;
}

void CHudRadio::Redraw(void)
{
	if ( !m_iGroup )
		return;

	int iBaseX = ScreenWide - 100;
	int iBaseY = ScreenTall / 2 - 20;

	gFontAPI.DrawSetTextFont(gHUD.m_iFontOutline);
	gFontAPI.DrawSetTextColor(220, 200, 150, 255);

	switch (m_iGroup)
	{
		case 1:
		{
			gFontAPI.DrawSetTextPos(iBaseX, iBaseY);
			gFontAPI.DrawPrintText(L"答复 / 报告");

			gFontAPI.DrawSetTextPos(iBaseX, iBaseY + 20);
			gFontAPI.DrawPrintText(L"1. 肯定");

			gFontAPI.DrawSetTextPos(iBaseX, iBaseY + 40);
			gFontAPI.DrawPrintText(L"2. 否定");

			gFontAPI.DrawSetTextPos(iBaseX, iBaseY + 60);
			gFontAPI.DrawPrintText(L"3. 有敌情");

			gFontAPI.DrawSetTextPos(iBaseX, iBaseY + 80);
			gFontAPI.DrawPrintText(L"4. 发现C4");

			gFontAPI.DrawSetTextPos(iBaseX, iBaseY + 100);
			gFontAPI.DrawPrintText(L"5. 地区无异常");

			gFontAPI.DrawSetTextPos(iBaseX, iBaseY + 120);
			gFontAPI.DrawPrintText(L"6. 取消");

			break;
		}
		case 2:
		{
			gFontAPI.DrawSetTextPos(iBaseX, iBaseY);
			gFontAPI.DrawPrintText(L"鼓励 / 邀请");

			gFontAPI.DrawSetTextPos(iBaseX, iBaseY + 20);
			gFontAPI.DrawPrintText(L"1. 漂亮");

			gFontAPI.DrawSetTextPos(iBaseX, iBaseY + 40);
			gFontAPI.DrawPrintText(L"2. 坚持到底");

			gFontAPI.DrawSetTextPos(iBaseX, iBaseY + 60);
			gFontAPI.DrawPrintText(L"3. 跟我来");

			gFontAPI.DrawSetTextPos(iBaseX, iBaseY + 80);
			gFontAPI.DrawPrintText(L"4. 呼叫支援");

			gFontAPI.DrawSetTextPos(iBaseX, iBaseY + 100);
			gFontAPI.DrawPrintText(L"5. 优先等待");

			gFontAPI.DrawSetTextPos(iBaseX, iBaseY + 120);
			gFontAPI.DrawPrintText(L"6. 取消");

			break;
		}
		case 3:
		{
			gFontAPI.DrawSetTextPos(iBaseX, iBaseY);
			gFontAPI.DrawPrintText(L"共通");

			gFontAPI.DrawSetTextPos(iBaseX, iBaseY + 20);
			gFontAPI.DrawPrintText(L"1. A 点集合");

			gFontAPI.DrawSetTextPos(iBaseX, iBaseY + 40);
			gFontAPI.DrawPrintText(L"2. B 点集合");

			gFontAPI.DrawSetTextPos(iBaseX, iBaseY + 60);
			gFontAPI.DrawPrintText(L"3. 守卫");

			gFontAPI.DrawSetTextPos(iBaseX, iBaseY + 80);
			gFontAPI.DrawPrintText(L"4. 全体集合");

			gFontAPI.DrawSetTextPos(iBaseX, iBaseY + 100);
			gFontAPI.DrawPrintText(L"5. 突破");

			gFontAPI.DrawSetTextPos(iBaseX, iBaseY + 120);
			gFontAPI.DrawPrintText(L"6. 取消");

			break;
		}
	}
}

void CHudRadio::Show(int group)
{
	if (!ClientState.bIsAlive)
		return;

	if (group != m_iGroup)
		m_iGroup = group;
	else
		m_iGroup = 0;
}

void CHudRadio::Hide(void)
{
	m_iGroup = 0;
}

bool CHudRadio::IsVisable(void)
{
	return m_iGroup != 0;
}

void CHudRadio::Select(int num)
{
	if (!ClientState.bIsAlive)
		return;

	char command[32];

	switch (m_iGroup)
	{
		case 1:
		{
			sprintf(command, "radio1 %d\n", num);
			gEngfuncs.pfnServerCmd(command);
			break;
		}
		case 2:
		{
			sprintf(command, "radio2 %d\n", num);
			gEngfuncs.pfnServerCmd(command);
			break;
		}
		case 3:
		{
			sprintf(command, "radio3 %d\n", num);
			gEngfuncs.pfnServerCmd(command);
			break;
		}
	}

	m_iGroup = 0;
}