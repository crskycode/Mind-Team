#include "UI_Room.h"
#include "UI_FlowTop.h"
#include "CF_Base.h"
#include "EngineInterface.h"
#include "UI_Globals.h"
#include "UserSystem.h"
#include <keyvalues.h>

UIRoom::UIRoom(vgui::Panel *parent) : vgui::CFDialog(parent, "Room")
{
	// Must be enable
	SetProportional(true);

	// Create top window
	m_pUI_FlowTop = new UIFlowTop(this);

	// Load layout setting
	LoadControlSettings("UI/Scripts/Room/Room_BaseNew.txt");

	// Load room info layout
	LoadControlSettings("UI/Scripts/Room/Room_Infonormal.txt");

	LoadControlSettings("UI/Scripts/Room/Room_Bag.txt");
	LoadControlSettings("UI/Scripts/Room/Room_Myinfo.txt");

	// Load host layout
	LoadControlSettings("UI/Scripts/Room/Room_Hostmenu.txt");

	// Load nanip layout
	LoadControlSettings("UI/Scripts/Room/Room_Nanipmenu.txt");

	// Load chat layout
	LoadControlSettings("UI/Scripts/Room/Room_Chat.txt");

	// Load buddy window
	LoadControlSettings("UI/Scripts/Buddy_CF20/FlowTop_Buddy.txt");

	// Get controls

	m_cStaticNotice = FIND_CONTROL_STATIC("StaticNotice");

	//InfoTab::BagInfo
	m_cImageBagBG[0] = FIND_CONTROL_IMAGE("ImageBagBG");
	m_cImageBagBG[1] = FIND_CONTROL_IMAGE("ImageBagBG1");
	m_cImageCashWP[0] = FIND_CONTROL_IMAGE("ImageCashWP1");
	m_cImageCashWP[1] = FIND_CONTROL_IMAGE("ImageCashWP2");
	m_cImageSlotRed[0] = FIND_CONTROL_IMAGE("ImageMainSlotRed");
	m_cImageSlotRed[1] = FIND_CONTROL_IMAGE("ImageSubSlotRed");
	m_cImageWP[0] = FIND_CONTROL_IMAGE("ImageWP1");
	m_cImageWP[1] = FIND_CONTROL_IMAGE("ImageWP2");
	m_cImageWP[2] = FIND_CONTROL_IMAGE("ImageWP3");
	m_cImageWP[3] = FIND_CONTROL_IMAGE("ImageHE1");
	m_cImageWP[4] = FIND_CONTROL_IMAGE("ImageHE2");
	m_cImageWP[5] = FIND_CONTROL_IMAGE("ImageHE3");
	m_cRoomRadioBag[0] = FIND_CONTROL_RADIOBUTTON("RoomRadioBag1");
	m_cRoomRadioBag[1] = FIND_CONTROL_RADIOBUTTON("RoomRadioBag2");
	m_cRoomRadioBag[2] = FIND_CONTROL_RADIOBUTTON("RoomRadioBag3");
	m_cImageWPGuageBG[0] = FIND_CONTROL_IMAGE("ImageWPGuageBG1");
	m_cImageWPGuageBG[1] = FIND_CONTROL_IMAGE("ImageWPGuageBG2");
	m_cImageWPGuage[0] = FIND_CONTROL_IMAGE("ImageWPGuage1");
	m_cImageWPGuage[1] = FIND_CONTROL_IMAGE("ImageWPGuage2");
	m_cButtonRepair[0] = FIND_CONTROL_BUTTON("ButtonRepair1");
	m_cButtonRepair[1] = FIND_CONTROL_BUTTON("ButtonRepair2");
	m_cStaticWPName[0] = FIND_CONTROL_STATIC("StaticWPName1");
	m_cStaticWPName[1] = FIND_CONTROL_STATIC("StaticWPName2");
	m_cStaticWPName[2] = FIND_CONTROL_STATIC("StaticWPName3");
	m_cStaticExpireWP[0] = FIND_CONTROL_STATIC("StaticExpireWP1");
	m_cStaticExpireWP[1] = FIND_CONTROL_STATIC("StaticExpireWP2");
	m_cStaticExpireWP[2] = FIND_CONTROL_STATIC("StaticExpireWP3");
	m_cImageHEDisable[0] = FIND_CONTROL_IMAGE("ImageHE2Disable");
	m_cImageHEDisable[1] = FIND_CONTROL_IMAGE("ImageHE3Disable");

	//InfoTab::MyInfo
	m_cImageMyInfoBG = FIND_CONTROL_IMAGE("ImageMyInfoBG");
	m_cStaticCallName = FIND_CONTROL_STATIC("StaticCallName");
	m_cImageMyCurClass = FIND_CONTROL_IMAGE("ImageMyCurClass");
	m_cImageMyNextClass = FIND_CONTROL_IMAGE("ImageMyNextClass");
	m_cImageEPGauge = FIND_CONTROL_IMAGE("ImageEPGauge");
	m_cStaticEP = FIND_CONTROL_STATIC("StaticEP");
	m_cStaticMyClanName = FIND_CONTROL_STATIC("StaticMyClanName");
	m_cStaticKilldeath = FIND_CONTROL_STATIC("StaticKilldeath");
	m_cStaticWinLose = FIND_CONTROL_STATIC("StaticWinLose");
	m_cStaticManner = FIND_CONTROL_STATIC("StaticManner");
	m_cStaticGP = FIND_CONTROL_STATIC("StaticGP");
	m_cStaticCP = FIND_CONTROL_STATIC("StaticCP");
	m_cButtonIDCard = FIND_CONTROL_BUTTON("ButtonIDCard");

	m_cButtonBag = FIND_CONTROL_RADIOBUTTON("ButtonBag");
	m_cButtonMyInfo = FIND_CONTROL_RADIOBUTTON("ButtonMyInfo");

	m_cButtonCLeave = FIND_CONTROL_BUTTON("ButtonCLeave");
	m_cButtonHostinvite = FIND_CONTROL_BUTTON("ButtonHostinvite");
	m_cButtonStart = FIND_CONTROL_BUTTON("ButtonStart");
	m_cButtonNanipstart = FIND_CONTROL_BUTTON("ButtonNanipstart");

	//InfoTab::BagInfo
	m_cInfoTab.AddCtrl(0, m_cImageBagBG[0]);
	m_cInfoTab.AddCtrl(0, m_cImageCashWP[0]);
	m_cInfoTab.AddCtrl(0, m_cImageCashWP[1]);
	m_cInfoTab.AddCtrl(0, m_cImageSlotRed[0]);
	m_cInfoTab.AddCtrl(0, m_cImageSlotRed[1]);
	m_cInfoTab.AddCtrl(0, m_cImageWP[0]);
	m_cInfoTab.AddCtrl(0, m_cImageWP[1]);
	m_cInfoTab.AddCtrl(0, m_cRoomRadioBag[0]);
	m_cInfoTab.AddCtrl(0, m_cRoomRadioBag[1]);
	m_cInfoTab.AddCtrl(0, m_cRoomRadioBag[2]);
	m_cInfoTab.AddCtrl(0, m_cImageWPGuageBG[0]);
	m_cInfoTab.AddCtrl(0, m_cImageWPGuageBG[1]);
	m_cInfoTab.AddCtrl(0, m_cImageWPGuage[0]);
	m_cInfoTab.AddCtrl(0, m_cImageWPGuage[1]);
	m_cInfoTab.AddCtrl(0, m_cButtonRepair[0]);
	m_cInfoTab.AddCtrl(0, m_cButtonRepair[1]);
	m_cInfoTab.AddCtrl(0, m_cStaticWPName[0]);
	m_cInfoTab.AddCtrl(0, m_cStaticWPName[1]);
	m_cInfoTab.AddCtrl(0, m_cStaticExpireWP[0]);
	m_cInfoTab.AddCtrl(0, m_cStaticExpireWP[1]);

	//InfoTab::MyInfo
	m_cInfoTab.AddCtrl(1, m_cImageMyInfoBG);
	m_cInfoTab.AddCtrl(1, m_cStaticCallName);
	m_cInfoTab.AddCtrl(1, m_cImageMyCurClass);
	m_cInfoTab.AddCtrl(1, m_cImageMyNextClass);
	m_cInfoTab.AddCtrl(1, m_cImageEPGauge);
	m_cInfoTab.AddCtrl(1, m_cStaticEP);
	m_cInfoTab.AddCtrl(1, m_cStaticMyClanName);
	m_cInfoTab.AddCtrl(1, m_cStaticKilldeath);
	m_cInfoTab.AddCtrl(1, m_cStaticWinLose);
	m_cInfoTab.AddCtrl(1, m_cStaticManner);
	m_cInfoTab.AddCtrl(1, m_cStaticGP);
	m_cInfoTab.AddCtrl(1, m_cStaticCP);
	m_cInfoTab.AddCtrl(1, m_cButtonIDCard);

	m_cButtonStart->SetCommand( "Start" );
}

void UIRoom::Activate(void)
{
	BaseClass::Activate();

	m_cStaticNotice->SetText(L"%s - %s - 27. 枪法与战术的完美组合!", gUI.m_szServer, gUI.m_szChannel);

	//
	m_bHasBombPackage = g_User.FindInvenItem(ITEMBOMBPACKAGEINDEX) != NULL;

	m_cButtonBag->SetSelected( true );

	m_cButtonCLeave->SetVisible( true );
	m_cButtonHostinvite->SetVisible( true );
	m_cButtonStart->SetVisible( true );
	m_cButtonNanipstart->SetVisible( false );
}

void UIRoom::OnCommandFull(KeyValues *params)
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
		gUI.GetLobby()->Activate();
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

	if (!strcmp(szCommand, "Start"))
	{
		gUI.GetMsgWnd()->Create( 1, L"正在连接服务器" );

		gEngfuncs.pfnClientCmd(
			"disconnect\nwait\nwait\nsv_lan 1\nsetmaster disable\nmaxplayers 16\nsv_password \"\"\nhostname \"Mind-Team Server\"\nprogress_enable\nmap cf_transportship_ren\n");

//		gUI.m_pUI_MsgWnd->Close();

		return;
	}
}

void UIRoom::OnRadioButtonCheck(vgui::Panel *panel, int tabposition)
{
	if (panel == m_cButtonBag)
	{
		m_cInfoTab.Switch(0);
		m_cRoomRadioBag[0]->SetSelected(true);
		return;
	}

	if (panel == m_cButtonMyInfo)
	{
		m_cInfoTab.Switch(1);
		return;
	}

	if (panel == m_cRoomRadioBag[0])
	{
		RefBagInfo(1);
		return;
	}

	if (panel == m_cRoomRadioBag[1])
	{
		RefBagInfo(2);
		return;
	}

	if (panel == m_cRoomRadioBag[2])
	{
		RefBagInfo(3);
		return;
	}
}

void UIRoom::OnCursorMoved(int x, int y)
{
	RefBagInfoExtra();
	BaseClass::OnCursorMoved(x, y);
}

void UIRoom::OnKeyCodePressed(vgui::KeyCode code)
{
	if (code == vgui::KEY_ESCAPE)
	{
		gUI.GetLobby()->Activate();
		Close();
		return;
	}

	BaseClass::OnKeyCodePressed(code);
}

void UIRoom::RefBagInfo(int iBagNumber)
{
	for (int iSlot = 0; iSlot <= BAG_SLOT_SECOND; iSlot++)
	{
		CInvenItem *pInvenItem = g_User.GetBagItem(iBagNumber, iSlot);

		if (pInvenItem)
		{
			CShopItem *pShopItem = pInvenItem->m_pShopItem;

			if (iSlot <= BAG_SLOT_SECOND)
			{
				if (pShopItem->m_bCashItem)
				{
					m_cImageCashWP[iSlot]->SetImage("UI/UI_Character_Uniform/UniBox1.PNG");
					m_cImageCashWP[iSlot]->SetVisible(true);
				}
				else
				{
					//m_cImageCashWP[iSlot]->SetImage("UI/UI_Character_Setting/Box1.PNG");
					m_cImageCashWP[iSlot]->SetVisible(false);
				}
			}

			if (iSlot <= BAG_SLOT_SECOND)
			{
				m_cImageSlotRed[iSlot]->SetVisible(false);
			}

			if (iSlot <= BAG_SLOT_KNIFE)
			{
				m_cImageWP[iSlot]->SetImage("UI/ItemIcon/ItemIcon_%03d.PNG", (int)pShopItem->m_nItemIndex);
				m_cImageWP[iSlot]->SetVisible(true);
			}
			else
			{
				m_cImageWP[iSlot]->SetImage("UI/ItemIcon/ItemIcon_%03ds.PNG", (int)pShopItem->m_nItemIndex);
				m_cImageWP[iSlot]->SetVisible(true);
			}

			if (iSlot <= BAG_SLOT_SECOND)
			{
				if (pShopItem->m_bRepairItem)
				{
					int iWide = pInvenItem->m_iDurability / 100.0 * PROPORT( 88 );

					m_cImageWPGuageBG[iSlot]->SetVisible(true);
					m_cImageWPGuage[iSlot]->SetWide(iWide);
					m_cImageWPGuage[iSlot]->SetVisible(true);
					m_cButtonRepair[iSlot]->SetVisible(true);
				}
				else
				{
					m_cImageWPGuageBG[iSlot]->SetVisible(false);
					m_cImageWPGuage[iSlot]->SetVisible(false);
					m_cButtonRepair[iSlot]->SetVisible(false);
				}
			}

			if (iSlot <= BAG_SLOT_KNIFE)
			{
				m_cStaticWPName[iSlot]->SetText(pShopItem->m_szName);
				m_cStaticWPName[iSlot]->SetVisible(true);
			}

			if (iSlot <= BAG_SLOT_KNIFE)
			{
				if (pInvenItem->m_iPeriodTime > 0)
				{
					m_cStaticExpireWP[iSlot]->SetVisible(true);
				}
				else
				{
					m_cStaticExpireWP[iSlot]->SetVisible(false);
				}
			}
		}
		else
		{
			if (iSlot <= BAG_SLOT_SECOND)
			{
				m_cImageCashWP[iSlot]->SetVisible(false);
			}

			if (iSlot <= BAG_SLOT_SECOND)
			{
				m_cImageSlotRed[iSlot]->SetVisible(false);
			}

			m_cImageWP[iSlot]->SetVisible(false);

			if (iSlot <= BAG_SLOT_SECOND)
			{
				m_cImageWPGuageBG[iSlot]->SetVisible(false);
				m_cImageWPGuage[iSlot]->SetVisible(false);
				m_cButtonRepair[iSlot]->SetVisible(false);
			}

			if (iSlot <= BAG_SLOT_KNIFE)
			{
				m_cStaticWPName[iSlot]->SetVisible(false);
				m_cStaticExpireWP[iSlot]->SetVisible(false);
			}
		}
	}

	m_iCurrentBag = iBagNumber;
	// Draw extra info panel
	RefBagInfoExtra();
}

static bool g_lastState = true;

void UIRoom::RefBagInfoExtra(void)
{
	// What the fuck ?
	if (m_iCurrentBag < 1 || m_iCurrentBag > BAG_MAX_NUM)
		return;

	int x, y;

	vgui::input()->GetCursorPosition(x, y);
	ScreenToLocal(x, y);

	// Now in MyBag tab ?
	bool bInBagTab = ( m_cInfoTab.GetIndex() == 0 );
	// Is cursor inside the rectangle ?
	bool bInBagRect = ( (x >= PROPORT(16) && x <= PROPORT(246)) && (y >= PROPORT(400) && y <= PROPORT(560)) );
	bool bDrawIt = bInBagTab && bInBagRect;

	if (bDrawIt == g_lastState)
	{
		// Don't darw multiple !
		return;
	}

	g_lastState = bDrawIt;

	if (bDrawIt)
	{
		m_cImageBagBG[1]->SetVisible(true);

		for (int iSlot = BAG_SLOT_KNIFE; iSlot < BAG_MAX_SLOT; iSlot++)
		{
			CInvenItem *pItem = g_User.GetBagItem(m_iCurrentBag, iSlot);

			if (pItem)
			{
				if (iSlot <= BAG_SLOT_KNIFE)
				{
					m_cImageWP[iSlot]->SetImage("UI/ItemIcon/ItemIcon_%03d.PNG", (int)pItem->m_pShopItem->m_nItemIndex);
					m_cImageWP[iSlot]->SetVisible(true);
				}
				else
				{
					m_cImageWP[iSlot]->SetImage("UI/ItemIcon/ItemIcon_%03ds.PNG", (int)pItem->m_pShopItem->m_nItemIndex);
					m_cImageWP[iSlot]->SetVisible(true);
				}

				if (iSlot <= BAG_SLOT_KNIFE)
				{
					m_cStaticWPName[iSlot]->SetText(pItem->m_pShopItem->m_szName);
					m_cStaticWPName[iSlot]->SetVisible(true);
				}

				if (iSlot <= BAG_SLOT_KNIFE)
				{
					if (pItem->m_iPeriodTime > 0)
					{
						m_cStaticExpireWP[iSlot]->SetVisible(true);
					}
					else
					{
						m_cStaticExpireWP[iSlot]->SetVisible(false);
					}
				}
			}
		}

		if (m_bHasBombPackage)
		{
			m_cImageHEDisable[0]->SetVisible(false);
			m_cImageHEDisable[1]->SetVisible(false);
		}
		else
		{
			m_cImageHEDisable[0]->SetVisible(true);
			m_cImageHEDisable[1]->SetVisible(true);
		}
	}
	else
	{
		m_cImageBagBG[1]->SetVisible(false);

		for (int iSlot = BAG_SLOT_KNIFE; iSlot < BAG_MAX_SLOT; iSlot++)
		{
			m_cImageWP[iSlot]->SetVisible(false);

			if (iSlot <= BAG_SLOT_KNIFE)
			{
				m_cStaticWPName[iSlot]->SetVisible(false);
				m_cStaticExpireWP[iSlot]->SetVisible(false);
			}
		}

		m_cImageHEDisable[0]->SetVisible(false);
		m_cImageHEDisable[1]->SetVisible(false);
	}
}