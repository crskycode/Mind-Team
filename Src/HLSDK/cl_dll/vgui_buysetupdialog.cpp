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
#include <vgui_controls/RadioButton.h>
#include <ITextureAPI.h>
#include "Viewport.h"
#include "input.h"
#include "ClientState.h"
#include "BagData.h"
#include "WeaponData.h"
#include <demo_api.h>

using namespace vgui;

class CBuySetupBagButton : public RadioButton
{
	DECLARE_CLASS_SIMPLE(CBuySetupBagButton, RadioButton);

public:
	CBuySetupBagButton(Panel *parent) : RadioButton(parent, "BuySetupBagButton", NULL)
	{
		SetPaintBackgroundEnabled(false);
		_normalImage = NULL;
		_disableImage = NULL;
	}

	virtual void ApplySchemeSettings(IScheme *pScheme)
	{
		BaseClass::ApplySchemeSettings(pScheme);
		SetDefaultBorder(NULL);
		SetDepressedBorder(NULL);
		SetKeyFocusBorder(NULL);
	}

	virtual void OnCursorEntered(void)
	{
		SetSelected(true);
		PostActionSignal(new KeyValues("BuySetupBagButtonSelected", "number", _number));
	}

	virtual void OnMousePressed(MouseCode code)
	{
		if (code == MOUSE_LEFT)
		{
			PostActionSignal(new KeyValues("BuySetupBagButtonPressed", "number", _number));
		}
	}

	virtual void Paint(void)
	{
		if ( !ShouldPaint() )
			return;

		gTexAPI.RenderMode(RenderNormal);
		gTexAPI.DrawSetColor(255, 255, 255, 255);

		if (IsSelected())
		{
			gTexAPI.DrawSetTexture(_disableImage);
			gTexAPI.DrawTexturedRect(0, 0, _disableImage->wide, _disableImage->tall);
		}
		else
		{
			gTexAPI.DrawSetTexture(_normalImage);
			gTexAPI.DrawTexturedRect(0, 0, _normalImage->wide, _normalImage->tall);
		}
	}

	void SetNumber(int number)
	{
		_number = number;
	}

	void SetImage(const char *pszNormal, const char *pszDisable)
	{
		_normalImage = gTexAPI.LoadDTX(pszNormal);
		_disableImage = gTexAPI.LoadDTX(pszDisable);
	}

	int _number;

	tex_t *_normalImage;
	tex_t *_disableImage;
};

class CBuySetupCancelButton : public Button
{
	DECLARE_CLASS_SIMPLE(CBuySetupCancelButton, Button);

public:
	CBuySetupCancelButton(Panel *parent) : Button(parent, "BuySetupCancelButton", "Cancel")
	{
		SetPaintBackgroundEnabled(false);
		SetCommand("OnCancel");
		_activeTexture = gTexAPI.LoadDTX("Tex/UI/BuySetup/CancelBtn.dtx");
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

		if (IsArmed() && !IsSelected())
		{
			gTexAPI.RenderMode(RenderNormal);
			gTexAPI.DrawSetTexture(_activeTexture);
			gTexAPI.DrawSetColor(255, 255, 255, 255);
			gTexAPI.DrawTexturedRect(0, 0, _activeTexture->wide, _activeTexture->tall);
		}
	}

	tex_t *_activeTexture;
};

class CBuySetupImagePanel : public Panel
{
	DECLARE_CLASS_SIMPLE(CBuySetupImagePanel, Panel);

public:
	CBuySetupImagePanel(Panel *parent) : Panel(parent)
	{
		SetPaintBackgroundEnabled(false);
		SetPaintEnabled(true);
		_image = NULL;
	}

	void SetImage(const char *image)
	{
		if (image)
			_image = gTexAPI.LoadDTX(image);
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

class CBuySetupDialog : public Frame
{
	DECLARE_CLASS_SIMPLE(CBuySetupDialog, Frame);

public:
	CBuySetupDialog(Panel *parent) : Frame(parent, "BuySetupDialog")
	{
		int swide, stall;
		surface()->GetScreenSize(swide, stall);

		SetParent(parent);
		SetDeleteSelfOnClose(false);
		SetTitleBarVisible(false);
		SetSize(512, 512);
		SetPos(swide / 2 - 257, stall / 2 - 216);
		SetSizeable(false);
		SetMoveable(false);

		m_iBGTextureType = 0;
		m_iBagNumber = 1;

		_bgTexture = gTexAPI.LoadDTX("Tex/UI/BuySetup/SetWeaponUIBG.dtx");
		_bgTextureA = gTexAPI.LoadDTX("Tex/UI/BuySetup/SetWeaponUIBG_A.dtx");

		_bagButton1 = new CBuySetupBagButton(this);
		_bagButton1->SetPos(14, 79);
		_bagButton1->SetSize(62, 38);
		_bagButton1->SetNumber(1);
		_bagButton1->SetImage("Tex/UI/BuySetup/NormalBtn_1.dtx", "Tex/UI/BuySetup/DisableBtn_1.dtx");

		_bagButton2 = new CBuySetupBagButton(this);
		_bagButton2->SetPos(76, 79);
		_bagButton2->SetSize(62, 38);
		_bagButton2->SetNumber(2);
		_bagButton2->SetImage("Tex/UI/BuySetup/NormalBtn_2.dtx", "Tex/UI/BuySetup/DisableBtn_2.dtx");

		_bagButton3 = new CBuySetupBagButton(this);
		_bagButton3->SetPos(138, 79);
		_bagButton3->SetSize(62, 38);
		_bagButton3->SetNumber(3);
		_bagButton3->SetImage("Tex/UI/BuySetup/NormalBtn_3.dtx", "Tex/UI/BuySetup/DisableBtn_3.dtx");

		_cancelButton = new CBuySetupCancelButton(this);
		_cancelButton->SetPos(392, 381);
		_cancelButton->SetSize(120, 42);

		_primaryWeapon = new CBuySetupImagePanel(this);
		_primaryWeapon->SetPos(17, 137);
		_primaryWeapon->SetSize(256, 128);

		_secondaryWeapon = new CBuySetupImagePanel(this);
		_secondaryWeapon->SetPos(304, 137);
		_secondaryWeapon->SetSize(256, 128);

		_knifeWeapon = new CBuySetupImagePanel(this);
		_knifeWeapon->SetPos(-35, 268);
		_knifeWeapon->SetSize(256, 128);

		_equipmentWeapon1 = new CBuySetupImagePanel(this);
		_equipmentWeapon1->SetPos(98, 268);
		_equipmentWeapon1->SetSize(256, 128);

		_equipmentWeapon2 = new CBuySetupImagePanel(this);
		_equipmentWeapon2->SetPos(205, 268);
		_equipmentWeapon2->SetSize(256, 128);

		_equipmentWeapon3 = new CBuySetupImagePanel(this);
		_equipmentWeapon3->SetPos(312, 268);
		_equipmentWeapon3->SetSize(256, 128);


		// INITAILIZE

		SetBagNumber(1);
		_bagButton1->SetSelected(true);
	}

	void SetBagNumber(int number)
	{
		if (number < 1 || number > 3)
			return;

		m_iBagNumber = number;

		char szIconPath[MAX_QPATH];

		WeaponData *pWeaponData;

		_primaryWeapon->SetImage(NULL);
		_secondaryWeapon->SetImage(NULL);
		_knifeWeapon->SetImage(NULL);
		_equipmentWeapon1->SetImage(NULL);
		_equipmentWeapon2->SetImage(NULL);
		_equipmentWeapon3->SetImage(NULL);

		pWeaponData = WeaponData_GetData(BagData_GetWeapon(number, 1, 1));

		if (pWeaponData)
		{
			sprintf(szIconPath, "Tex/UI/WeaponIcon/BuyWeapon_Info_%s.dtx", pWeaponData->szBigIconName);
			_primaryWeapon->SetImage(szIconPath);
			m_iBGTextureType = 0;
		}

		pWeaponData = WeaponData_GetData(BagData_GetWeapon(number, 2, 1));

		if (pWeaponData)
		{
			sprintf(szIconPath, "Tex/UI/WeaponIcon/BuyWeapon_Info_%s.dtx", pWeaponData->szBigIconName);
			_secondaryWeapon->SetImage(szIconPath);
			m_iBGTextureType = 0;
		}

		pWeaponData = WeaponData_GetData(BagData_GetWeapon(number, 3, 1));

		if (pWeaponData)
		{
			sprintf(szIconPath, "Tex/UI/WeaponIcon/BuyWeapon_Info_%s.dtx", pWeaponData->szBigIconName);
			_knifeWeapon->SetImage(szIconPath);
			m_iBGTextureType = 0;
		}

		pWeaponData = WeaponData_GetData(BagData_GetWeapon(number, 4, 1));

		if (pWeaponData)
		{
			sprintf(szIconPath, "Tex/UI/WeaponIcon/BuyWeapon_Info_%s.dtx", pWeaponData->szBigIconName);
			_equipmentWeapon1->SetImage(szIconPath);
			m_iBGTextureType = 0;
		}

		pWeaponData = WeaponData_GetData(BagData_GetWeapon(number, 4, 2));

		if (pWeaponData)
		{
			sprintf(szIconPath, "Tex/UI/WeaponIcon/BuyWeapon_Info_%s.dtx", pWeaponData->szBigIconName);
			_equipmentWeapon2->SetImage(szIconPath);
			m_iBGTextureType = 1;
		}

		pWeaponData = WeaponData_GetData(BagData_GetWeapon(number, 4, 3));

		if (pWeaponData)
		{
			sprintf(szIconPath, "Tex/UI/WeaponIcon/BuyWeapon_Info_%s.dtx", pWeaponData->szBigIconName);
			_equipmentWeapon3->SetImage(szIconPath);
			m_iBGTextureType = 1;
		}
	}

	void Server_BagBuy(int number)
	{
		if (number < 1 || number > 3)
			return;

		int iPrimary = BagData_GetWeapon(number, 1, 1);
		int iSecondary = BagData_GetWeapon(number, 2, 1);
		int iKnife = BagData_GetWeapon(number, 3, 1);
		int iEquipment1 = BagData_GetWeapon(number, 4, 1);
		int iEquipment2 = BagData_GetWeapon(number, 4, 2);
		int iEquipment3 = BagData_GetWeapon(number, 4, 3);

		char command[64];
		sprintf(command, "bagbuy %d %d %d %d %d %d\n", iPrimary, iSecondary, iKnife, iEquipment1, iEquipment2, iEquipment3);

		gEngfuncs.pfnServerCmd("bagreset\n");
		gEngfuncs.pfnServerCmd(command);
	}

	void Server_BagRebuy(void)
	{
		if (m_iBagNumber < 1 || m_iBagNumber > 3)
			return;

		Server_BagBuy(m_iBagNumber);
	}

	virtual void PaintBackground(void)
	{
		gTexAPI.RenderMode(RenderNormal);
		gTexAPI.DrawSetColor(255, 255, 255, 255);

		if (m_iBGTextureType == 0)
			gTexAPI.DrawSetTexture(_bgTexture);
		else
			gTexAPI.DrawSetTexture(_bgTextureA);

		gTexAPI.DrawTexturedRect(0, 0, _bgTexture->wide, _bgTexture->tall);
	}

	MESSAGE_FUNC_INT(OnBuyButtonSelect, "BuySetupBagButtonSelected", number)
	{
		SetBagNumber(number);
	}

	MESSAGE_FUNC_INT(OnBuyButtonPressed, "BuySetupBagButtonPressed", number)
	{
		Server_BagBuy(number);
		Close();
	}

	virtual void OnKeyCodePressed(KeyCode code)
	{
		switch (code)
		{
			case KEY_1:
			{
				_bagButton1->OnCursorEntered();
				SetBagNumber(1);
				Server_BagBuy(1);
				Close();
				break;
			}
			case KEY_2:
			{
				_bagButton2->OnCursorEntered();
				SetBagNumber(2);
				Server_BagBuy(2);
				Close();
				break;
			}
			case KEY_3:
			{
				_bagButton3->OnCursorEntered();
				SetBagNumber(3);
				Server_BagBuy(3);
				Close();
				break;
			}

			case KEY_B:
			{
				Close();
				break;
			}
		}
	}

	virtual void OnCommand(const char *command)
	{
		if (!Q_strcmp(command, "OnCancel"))
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

	int m_iBGTextureType;
	int m_iBagNumber;

	tex_t *_bgTexture;
	tex_t *_bgTextureA;

	CBuySetupBagButton *_bagButton1;
	CBuySetupBagButton *_bagButton2;
	CBuySetupBagButton *_bagButton3;

	CBuySetupCancelButton *_cancelButton;

	CBuySetupImagePanel *_primaryWeapon;
	CBuySetupImagePanel *_secondaryWeapon;
	CBuySetupImagePanel *_knifeWeapon;
	CBuySetupImagePanel *_equipmentWeapon1;
	CBuySetupImagePanel *_equipmentWeapon2;
	CBuySetupImagePanel *_equipmentWeapon3;
};

DHANDLE<CBuySetupDialog> g_BuySetupDialog;


void BuySetupDialog_Active(void)
{
	if (gEngfuncs.pDemoAPI->IsPlayingback())
		return;

	if (!ClientState.bEanbleBag)
		return;

	if (g_BuySetupDialog.Get())
	{
		g_BuySetupDialog->Activate();
	}
}

void BuySetupDialog_Close(void)
{
	if (g_BuySetupDialog.Get())
	{
		g_BuySetupDialog->Close();
	}
}

bool BuySetupDialog_IsVisible(void)
{
	if (g_BuySetupDialog.Get())
	{
		return g_BuySetupDialog->IsVisible();
	}

	return false;
}

void BuySetupDialog_Rebuy(void)
{
	if (g_BuySetupDialog.Get())
	{
		g_BuySetupDialog->Server_BagRebuy();
	}
}

void BuySetupDialog_Init(void)
{
	if (!g_BuySetupDialog.Get())
	{
		g_BuySetupDialog = new CBuySetupDialog(g_pViewport);
	}

	gEngfuncs.pfnAddCommand("buy", BuySetupDialog_Active);
}

BOOL MsgFunc_BagRebuy(const char *pszName, int iSize, void *pbuf)
{
	BuySetupDialog_Rebuy();
	return TRUE;
}