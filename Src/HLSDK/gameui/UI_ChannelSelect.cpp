#include "UI_ChannelSelect.h"
#include "UI_FlowTop.h"
#include "CF_Base.h"
#include "UI_Globals.h"
#include "UserSystem.h"
#include "ShopItemManager.h"
#include <keyvalues.h>

UIChannelSelect::UIChannelSelect(vgui::Panel *parent) : vgui::CFDialog(parent, "ChannelSelect")
{
	// Must be enable
	SetProportional(true);

	// Create top window
	m_pUI_FlowTop = new UIFlowTop(this);

	// Load layout setting
	LoadControlSettings("UI/Scripts/ChannelSelect/ChannelSelect.txt");

	// Load chat layout
	LoadControlSettings("UI/Scripts/ChannelSelect/ChannelChat.txt");

	// Load buddy window
	LoadControlSettings("UI/Scripts/Buddy_CF20/FlowTop_Buddy.txt");

	char szOption[32];


	for (int i = 0; i < 13; i++)
	{
		sprintf(szOption, "ButtonChannel%d", i + 1);
		m_cButtonChannel[i] = FIND_CONTROL_BUTTON(szOption);
	}
	for (int i = 0; i < 13; i++)
	{
		sprintf(szOption, "ImgGaugeBack%d", i + 1);
		m_cImageGaugeBack[i] = FIND_CONTROL_IMAGE(szOption);
	}
	for (int i = 0; i < 13; i++)
	{
		sprintf(szOption, "ImageServerGauge%d", i + 1);
		m_cImageServerGauge[i] = FIND_CONTROL_IMAGE(szOption);
	}
	for (int i = 0; i < 13; i++)
	{
		sprintf(szOption, "StaticChannelName%d", i + 1);
		m_cStaticChannelName[i] = FIND_CONTROL_STATIC(szOption);
	}

	m_cStaticNotice = FIND_CONTROL_STATIC("StaticNotice");
	m_cButtonRefresh = FIND_CONTROL_BUTTON("ButtonRefresh");
	m_cButtonChannelEnter = FIND_CONTROL_BUTTON("ButtonChannelEnter");
	m_cStaticNotice = FIND_CONTROL_STATIC("StaticNotice");
	m_cStaticMyCallName = FIND_CONTROL_STATIC("StaticMyCallName");
	m_cStaticMyClanName = FIND_CONTROL_STATIC("StaticMyClanName");
	m_cStaticMyGP = FIND_CONTROL_STATIC("StaticMyGP");
	m_cStaticMyCP = FIND_CONTROL_STATIC("StaticMyCP");
	m_cStaticMyEP = FIND_CONTROL_STATIC("StaticMyEP");
	m_cImageClassGauge = FIND_CONTROL_IMAGE("ImageClassGauge");
	m_cImageMyCurClass = FIND_CONTROL_IMAGE("ImageMyCurClass");
	m_cImageMyNextClass = FIND_CONTROL_IMAGE("ImageMyNextClass");
	m_cButtonIDCard = FIND_CONTROL_BUTTON("ButtonIDCard");
	m_cStaticKillDeath = FIND_CONTROL_STATIC("StaticKillDeath");
	m_cStaticWinLose = FIND_CONTROL_STATIC("StaticWinLose");
	m_cStaticManner = FIND_CONTROL_STATIC("StaticManner");

	for (int i = 0; i < 2; i++)
	{
		sprintf(szOption, "ImageItemBase%d", i + 1);
		m_cImageChannelItem[i] = FIND_CONTROL_IMAGE(szOption);
	}
	for (int i = 0; i < 2; i++)
	{
		sprintf(szOption, "ImageChannelItemTag%d", i + 1);
		m_cImageChannelItemTag[i] = FIND_CONTROL_IMAGE(szOption);
	}
	for (int i = 0; i < 2; i++)
	{
		sprintf(szOption, "StaticItemName%d", i + 1);
		m_cStaticItemName[i] = FIND_CONTROL_STATIC(szOption);
	}
	for (int i = 0; i < 2; i++)
	{
		sprintf(szOption, "StaticItemGP%d", i + 1);
		m_cStaticItemGP[i] = FIND_CONTROL_STATIC(szOption);
	}
	for (int i = 0; i < 2; i++)
	{
		sprintf(szOption, "ButtonItem%d", i + 1);
		m_cButtonItem[i] = FIND_CONTROL_BUTTON(szOption);
	}

	m_cButtonPrev = FIND_CONTROL_BUTTON("ButtonPrev");
	m_cButtonNext = FIND_CONTROL_BUTTON("ButtonNext");

	m_cButtonPrev->SetCommand("PrevItem");
	m_cButtonNext->SetCommand("NextItem");

	for (int i = 0; i < 13; i++)
	{
		m_cButtonChannel[i]->SetCommand("ChannelItem");
		m_cButtonChannel[i]->GetCommand()->SetInt("Item", i);
	}

	m_cButtonChannelEnter->SetCommand("Enter");


	// clear the list, we need reload it
	m_ChannelItem.RemoveAll();

	// lookup the item table
	for (int i = 0; i < g_ShopItemManager.GetCount(); i++)
	{
		CShopItem *pItem = g_ShopItemManager.GetItem(i);

		// channel item is found, append to list
		if (pItem)
		{
			m_ChannelItem.AddToTail(pItem);
		}
	}
}

void UIChannelSelect::Activate(void)
{
	BaseClass::Activate();

	m_cStaticNotice->SetText(L"%s - 请选择频道.", gUI.m_szServer);

	int iCurClassEP = g_User.GetLevelEP(g_User.GetCurLevel());
	int iNextClassEP = g_User.GetLevelEP(g_User.GetNextLevel());
	int iCurrentEP = g_User.GetEP();

	int iGaugeWide = (iCurrentEP - iCurClassEP) / (float)(iNextClassEP - iCurClassEP) * PROPORT( 100 );

	m_cStaticMyCallName->SetText(g_User.GetName());
	m_cImageMyCurClass->SetImage("UI/Class/Class_%d.png", g_User.GetCurLevel());
	m_cImageMyNextClass->SetImage("UI/Class/Class_%d.png", g_User.GetNextLevel());
	m_cImageClassGauge->SetSize(iGaugeWide, 12);
	m_cStaticMyEP->SetText(L"%d/%d", iCurrentEP, iNextClassEP);
	m_cStaticMyClanName->SetText(g_User.GetClan());
	m_cStaticKillDeath->SetText(L"%d/%d", g_User.GetKill(), g_User.GetDeath());
	m_cStaticWinLose->SetText(L"%d/%d", g_User.GetWin(), g_User.GetLose());
	m_cStaticManner->SetText(L"...");
	m_cStaticMyGP->SetText(L"%d GP", g_User.GetGP());
	m_cStaticMyCP->SetText(L"%d CF点", g_User.GetCP());

	RefHotItem(m_iHotlItemStep = 0);

	if (g_User.IsFirstLogin())
	{
		gUI.GetFirstLoginNotice()->Activate();
	}
}

void UIChannelSelect::OnCommandFull(KeyValues *params)
{
	const char *szCommand = params->GetString("command");

	if (!strcmp(szCommand, "GachaShop"))
	{
		return;
	}

	if (!strcmp(szCommand, "Inven"))
	{
		gUI.GetInven()->m_pOwnerPanel = this;
		gUI.GetInven()->Activate();
		Close();
		return;
	}

	if (!strcmp(szCommand, "Shop"))
	{
		return;
	}

	if (!strcmp(szCommand, "MPointMall"))
	{
		return;
	}

	if (!strcmp(szCommand, "Back"))
	{
		gUI.GetServerSelect()->Activate();
		Close();
		return;
	}

	if (!strcmp(szCommand, "InClan"))
	{
		return;
	}

	if (!strcmp(szCommand, "AutoMatchTry"))
	{
		return;
	}

	if (!strcmp(szCommand, "Buddy"))
	{
		return;
	}

	if (!strcmp(szCommand, "Option"))
	{
		gUI.GetOption()->DoModal();
		return;
	}

	if (!strcmp(szCommand, "Exit"))
	{
		gUI.GetGameEnd()->Activate(this);
		return;
	}

	if (!strcmp(szCommand, "ServerList"))
	{
		gUI.GetServerSelect()->Activate();
		Close();
		return;
	}

	if (!strcmp(szCommand, "PrevItem"))
	{
		if (m_ChannelItem.Count() > 0)
		{
			m_iHotlItemStep--;

			if (m_iHotlItemStep < 0)
			{
				m_iHotlItemStep = m_ChannelItem.Count() - 1;
			}

			RefHotItem(m_iHotlItemStep);
		}

		return;
	}

	if (!strcmp(szCommand, "NextItem"))
	{
		if (m_ChannelItem.Count() > 0)
		{
			m_iHotlItemStep++;

			if (m_iHotlItemStep >= m_ChannelItem.Count())
			{
				m_iHotlItemStep = 0;
			}

			RefHotItem(m_iHotlItemStep);
		}

		return;
	}

	if (!strcmp(szCommand, "ChannelItem"))
	{
		if (params->GetInt("IsDouble") == true)
		{
			int index = params->GetInt("Item");

			if (index == -1)
				return;

			swprintf(gUI.m_szChannel, L"频道 %d", index + 1);
			gUI.GetLobby()->Activate();
			Close();
		}
		return;
	}

	if (!strcmp(szCommand, "Enter"))
	{
		return;
	}
}

void UIChannelSelect::OnKeyCodePressed(vgui::KeyCode code)
{
	if (code == vgui::KEY_ESCAPE)
	{
		gUI.GetServerSelect()->Activate();
		Close();
		return;
	}

	BaseClass::OnKeyCodePressed(code);
}

void UIChannelSelect::RefHotItem(int iStep)
{
	int nIndex = iStep;
	int iCount = m_ChannelItem.Count();

	if (iCount > 0)
	{
		for (int i = 0; i < 2; i++)
		{
			if (nIndex >= iCount)
			{
				nIndex = (nIndex - iCount) % iCount;
			}

			m_cImageChannelItem[i]->SetImage("UI/ItemIcon/ItemIcon_%03d.PNG", (int)m_ChannelItem[nIndex]->m_nItemIndex);
			m_cImageChannelItemTag[i]->SetImage(NULL);
			m_cStaticItemName[i]->SetText(m_ChannelItem[nIndex]->m_szName);
			m_cStaticItemGP[i]->SetText(L"%d GP", (int)m_ChannelItem[nIndex]->m_iPrice);

			nIndex++;
		}
	}
	else
	{
		for (int i = 0; i < 2; i++)
		{
			m_cImageChannelItem[i]->SetImage(NULL);
			m_cImageChannelItemTag[i]->SetImage(NULL);
			m_cStaticItemName[i]->SetText(L"");
			m_cStaticItemGP[i]->SetText(L"");
		}
	}
}