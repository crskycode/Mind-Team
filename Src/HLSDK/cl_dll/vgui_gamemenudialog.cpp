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
#include <tier1/KeyValues.h>
#include <vgui_controls/Frame.h>
#include <vgui_controls/Button.h>
#include <IFontAPI.h>
#include <ITextureAPI.h>
#include "Viewport.h"
#include "input.h"

using namespace vgui;

class CGameMenuButton : public Button
{
	DECLARE_CLASS_SIMPLE(CGameMenuButton, Button);

public:
	CGameMenuButton(Panel *parent, wchar_t *title) : Button(parent, "GameMenuButton", "Options")
	{
		SetPaintBackgroundEnabled(false);
		SetSize(153, 29);

		_titleFont = gFontAPI.CreateFont();
		gFontAPI.AddGlyphSetToFont(_titleFont, "Main", 12, FW_BOLD, 0, 0, 0);

		_titleText = title;
		gFontAPI.GetTextSize(_titleFont, _titleText, _titleWide, _titleTall);

		_upTexture = gTexAPI.LoadPNG("UI/UI_SelectWnd/ButtonUp.png");
		_focusTexture = gTexAPI.LoadPNG("UI/UI_SelectWnd/ButtonFocus.png");
		_downTexture = gTexAPI.LoadPNG("UI/UI_SelectWnd/ButtonDown.png");

		SetArmedSound("Snd2/WebUISnd/BT_H.wav");
		SetDepressedSound("Snd2/WebUISnd/BT_CL.wav");
	}

	virtual void ApplySchemeSettings(IScheme *pScheme)
	{
		BaseClass::ApplySchemeSettings(pScheme);
		SetDefaultBorder(NULL);
		SetDepressedBorder(NULL);
		SetKeyFocusBorder(NULL);
	}

	virtual void Paint(void)
	{
		if ( !ShouldPaint() )
			return;

		gTexAPI.RenderMode(RenderNormal);
		gTexAPI.DrawSetColor(255, 255, 255, 255);

		if (!IsArmed())
		{
			gTexAPI.DrawSetTexture(_upTexture);
			gTexAPI.DrawTexturedRect(0, 0, _upTexture->wide, _upTexture->tall);
		}
		else if (!IsSelected())
		{
			gTexAPI.DrawSetTexture(_focusTexture);
			gTexAPI.DrawTexturedRect(0, 0, _focusTexture->wide, _focusTexture->tall);
		}
		else
		{
			gTexAPI.DrawSetTexture(_downTexture);
			gTexAPI.DrawTexturedRect(0, 0, _downTexture->wide, _downTexture->tall);
		}

		gFontAPI.DrawSetTextFont(_titleFont);
		gFontAPI.DrawSetTextColor(255, 255, 255, 255);
		gFontAPI.DrawSetTextPos(GetWide() / 2 - _titleWide / 2, GetTall() / 2 - _titleTall / 2);
		gFontAPI.DrawPrintText(_titleText);
	}

	int _titleFont;
	int _titleWide, _titleTall;

	wchar_t *_titleText;

	tex_t *_upTexture;
	tex_t *_focusTexture;
	tex_t *_downTexture;
};

class CGameMenuDialog : public Frame
{
	DECLARE_CLASS_SIMPLE(CGameMenuDialog, Frame);

public:
	CGameMenuDialog(Panel *parent) : Frame(parent, "GameMenuDialog")
	{
		int swide, stall;
		g_pVGuiSurface->GetScreenSize(swide, stall);

		SetParent(parent);
		SetDeleteSelfOnClose(false);
		SetTitleBarVisible(false);
		SetSize(153, 197);
		SetPos(swide / 2 - 78, stall / 2 - 50);
		SetSizeable(false);
		SetMoveable(false);

		_titleTexture = gTexAPI.LoadPNG("UI/UI_SelectWnd/Option.png");

		_optionButton = new CGameMenuButton(this, L"设定");
		_optionButton->SetPos(0, 23);
		_optionButton->SetCommand("OnOption");

		_kickButton = new CGameMenuButton(this, L"投票踢人");
		_kickButton->SetPos(0, 52);
		_kickButton->SetCommand("OnKick");

		_reportButton = new CGameMenuButton(this, L"举报");
		_reportButton->SetPos(0, 81);
		_reportButton->SetCommand("OnReport");

		_hideButton = new CGameMenuButton(this, L"屏蔽玩家");
		_hideButton->SetPos(0, 110);
		_hideButton->SetCommand("OnHide");

		_quitButton = new CGameMenuButton(this, L"游戏结束 [Enter]");
		_quitButton->SetPos(0, 139);
		_quitButton->SetCommand("OnQuit");

		_cancelButton = new CGameMenuButton(this, L"取消 [Esc]");
		_cancelButton->SetPos(0, 168);
		_cancelButton->SetCommand("OnCancel");
	}

	virtual void PaintBackground(void)
	{
		gTexAPI.RenderMode(RenderNormal);
		gTexAPI.DrawSetTexture(_titleTexture);
		gTexAPI.DrawSetColor(255, 255, 255, 255);
		gTexAPI.DrawTexturedRect(0, 0, _titleTexture->wide, _titleTexture->tall);
	}

	virtual void OnCommand(const char *command)
	{
		if (!Q_strcmp(command, "OnOption"))
		{
		}
		else if (!Q_strcmp(command, "OnKick"))
		{
		}
		else if (!Q_strcmp(command, "OnReport"))
		{
		}
		else if (!Q_strcmp(command, "OnHide"))
		{
		}
		else if (!Q_strcmp(command, "OnQuit"))
		{
			gEngfuncs.pfnClientCmd("disconnect\n");
		}
		else if (!Q_strcmp(command, "OnCancel"))
		{
			Close();
		}
	}

	virtual void Activate(void)
	{
		BaseClass::Activate();
		IN_DeactivateMouse();
	}

	virtual void OnClose(void)
	{
		BaseClass::OnClose();
		SetVisible(false);
		IN_ActivateMouse();
	}

	tex_t *_titleTexture;

	CGameMenuButton *_optionButton;
	CGameMenuButton *_kickButton;
	CGameMenuButton *_reportButton;
	CGameMenuButton *_hideButton;
	CGameMenuButton *_quitButton;
	CGameMenuButton *_cancelButton;
};

DHANDLE<CGameMenuDialog> g_GameMenuDialog;


void GameMenuDialog_Activate(void)
{
	if (g_GameMenuDialog.Get())
	{
		g_GameMenuDialog->Activate();
	}
}

void GameMenuDialog_Close(void)
{
	if (g_GameMenuDialog.Get())
	{
		g_GameMenuDialog->Close();
	}
}

bool GameMenuDialog_IsVisible(void)
{
	if (g_GameMenuDialog.Get())
	{
		return g_GameMenuDialog->IsVisible();
	}

	return false;
}

void GameMenuDialog_Init(void)
{
	if (!g_GameMenuDialog.Get())
	{
		g_GameMenuDialog = new CGameMenuDialog(g_pViewport);
	}

	gEngfuncs.pfnAddCommand("_gamemenu", GameMenuDialog_Activate);
}