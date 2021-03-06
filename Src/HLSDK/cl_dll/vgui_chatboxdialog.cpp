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
#include <vgui_controls/CheckButton.h>
#include <IFontAPI.h>
#include <ITextureAPI.h>
#include "Viewport.h"
#include "input.h"

using namespace vgui;

class CChatBoxImagePanel : public Panel
{
	DECLARE_CLASS_SIMPLE(CChatBoxImagePanel, Panel);

public:
	CChatBoxImagePanel(Panel *parent) : Panel(parent)
	{
		SetPaintBackgroundEnabled(false);
		SetPaintEnabled(true);
		_image = NULL;
	}

	void SetImage(const char *image)
	{
		if (image)
			_image = gTexAPI.Load(image);
		else
			_image = NULL;
	}

	virtual void Paint(void)
	{
		if (!_image)
			return;

		gTexAPI.RenderMode(RenderNormal);
		gTexAPI.DrawSetColor(255, 255, 255, 255);
		gTexAPI.DrawSetTexture(_image);
		gTexAPI.DrawTexturedRect(0, 0, _image->wide, _image->tall);
	}

	tex_t *_image;
};

class CChatBoxLabelPanel : public Panel
{
	DECLARE_CLASS_SIMPLE(CChatBoxLabelPanel, Panel);

public:
	CChatBoxLabelPanel(Panel *parent) : Panel(parent, "ChatBoxLabelPanel")
	{
		SetPaintBackgroundEnabled(false);
		SetPaintEnabled(true);

		_font = gFontAPI.CreateFont();
		_text = 0;
	}

	void SetFont(const char *name, int tall, int weight, int flags)
	{
		gFontAPI.AddGlyphSetToFont(_font, name, tall, weight, 0, 0, flags);
	}

	void SetText(const wchar_t *text)
	{
		if (_text)
		{
			delete[] _text;
			_text = NULL;
		}

		int length = wcslen(text) + 1;

		_text = new wchar_t[length];

		wcscpy(_text, text);
	}

	void SetColor(int r, int g, int b, int a)
	{
		_color.SetColor(r, g, b, a);
	}

	virtual void Paint(void)
	{
		if ( !_text )
			return;

		gFontAPI.DrawSetTextFont(_font);
		gFontAPI.DrawSetTextColor(_color[0], _color[1], _color[2], _color[3]);
		gFontAPI.DrawSetTextPos(0, 0);
		gFontAPI.DrawPrintText(_text);
	}

	int _font;
	Color _color;
	wchar_t *_text;
};

class CChatBoxCheckButton : public CheckButton
{
	DECLARE_CLASS_SIMPLE(CChatBoxCheckButton, CheckButton);

public:
	CChatBoxCheckButton(Panel *parent) : CheckButton(parent, "ChatBoxCheckButton", "Button")
	{
		SetPaintBackgroundEnabled(false);
		SetPaintEnabled(true);

		_upImage = gTexAPI.LoadPNG("UI/UI_GNB/OldNewCheckUp.png");
		_downImage = gTexAPI.LoadPNG("UI/UI_GNB/OldNewCheckDown.png");

		_font = gFontAPI.CreateFont();
		_text = NULL;

		gFontAPI.AddGlyphSetToFont(_font, "Main", 12, FW_NORMAL, 0, 0, FONTFLAG_NONE);

		_offsetX = 17;
		_offsetY = 2;
	}

	void SetText(const wchar_t *text)
	{
		if (_text)
		{
			delete[] _text;
			_text = NULL;
		}

		int length = wcslen(text) + 1;

		_text = new wchar_t[length];

		wcscpy(_text, text);
	}

	virtual void Paint(void)
	{
		if ( !ShouldPaint() )
			return;

		gTexAPI.RenderMode(RenderNormal);
		gTexAPI.DrawSetColor(255, 255, 255, 255);

		if ( !IsSelected() )
			gTexAPI.DrawSetTexture(_upImage);
		else
			gTexAPI.DrawSetTexture(_downImage);

		gTexAPI.DrawTexturedRect(0, 0, _upImage->wide, _upImage->tall);

		if ( !_text )
			return;

		gFontAPI.DrawSetTextFont(_font);
		gFontAPI.DrawSetTextColor(255, 255, 255, 255);
		gFontAPI.DrawSetTextPos(_offsetX, _offsetY);
		gFontAPI.DrawPrintText(_text);
	}

	tex_t *_upImage;
	tex_t *_downImage;

	int _font;
	wchar_t *_text;

	int _offsetX;
	int _offsetY;
};

class CChatBoxDialog : public Frame
{
	DECLARE_CLASS_SIMPLE(CChatBoxDialog, Frame);

public:
	CChatBoxDialog(Panel *parent) : Frame(parent, "ChatBoxDialog")
	{
		int swide, stall;
		surface()->GetScreenSize(swide, stall);

		SetPos(13, stall - 277);
		SetSize(512, 217);
		SetMoveable(false);
		SetSizeable(false);

		SetTitleBarVisible(false);
		SetPaintBackgroundEnabled(false);

		_chatBoxBG_0 = new CChatBoxImagePanel(this);
		_chatBoxBG_0->SetPos(0, 0);
		_chatBoxBG_0->SetSize(349, 153);
		_chatBoxBG_0->SetImage("UI/UI_GNB/ChatBG_InGame.png");

		_chatBoxBG_1 = new CChatBoxImagePanel(this);
		_chatBoxBG_1->SetPos(2, 153);
		_chatBoxBG_1->SetSize(512, 64);
		_chatBoxBG_1->SetImage("Tex/UI/PlayMain/ChatBox.dtx");

		_optionTitle = new CChatBoxLabelPanel(this);
		_optionTitle->SetPos(11, 5);
		_optionTitle->SetSize(80, 12);
		_optionTitle->SetFont("Main", 12, FW_NORMAL, FONTFLAG_NONE);
		_optionTitle->SetColor(255, 205, 99, 255);
		_optionTitle->SetText(L"查看聊天选项");

		_commonCheckButton = new CChatBoxCheckButton(this);
		_commonCheckButton->SetPos(111, 3);
		_commonCheckButton->SetSize(50, 16);
		_commonCheckButton->SetText(L"普通");
		_commonCheckButton->SetSelected(true);

		_teamCheckButton = new CChatBoxCheckButton(this);
		_teamCheckButton->SetPos(173, 3);
		_teamCheckButton->SetSize(50, 16);
		_teamCheckButton->SetText(L"团队");
		_teamCheckButton->SetSelected(true);

		_clanCheckButton = new CChatBoxCheckButton(this);
		_clanCheckButton->SetPos(235, 3);
		_clanCheckButton->SetSize(50, 16);
		_clanCheckButton->SetText(L"战队");
		_clanCheckButton->SetSelected(true);
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

	CChatBoxImagePanel *_chatBoxBG_0;
	CChatBoxImagePanel *_chatBoxBG_1;

	CChatBoxLabelPanel *_optionTitle;

	CChatBoxCheckButton *_commonCheckButton;
	CChatBoxCheckButton *_teamCheckButton;
	CChatBoxCheckButton *_clanCheckButton;
};

DHANDLE<CChatBoxDialog> g_ChatBoxDialog;


void ChatBoxDialog_Activate(void)
{
	if (g_ChatBoxDialog.Get())
	{
		g_ChatBoxDialog->Activate();
	}
}

void ChatBoxDialog_Close(void)
{
	if (g_ChatBoxDialog.Get())
	{
		g_ChatBoxDialog->Close();
	}
}

bool ChatBoxDialog_IsVisible(void)
{
	if (g_ChatBoxDialog.Get())
	{
		return g_ChatBoxDialog->IsVisible();
	}

	return false;
}

void ChatBoxDialog_Init(void)
{
	if (!g_ChatBoxDialog.Get())
	{
		g_ChatBoxDialog = new CChatBoxDialog(g_pViewport);
	}

	gEngfuncs.pfnAddCommand("_chatbox", ChatBoxDialog_Activate);
}