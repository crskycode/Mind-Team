#include "UI_Lobby.h"
#include "UI_FlowTop.h"
#include "CF_Base.h"
#include "UI_Globals.h"
#include "UserSystem.h"
#include <keyvalues.h>

UILobby::UILobby(vgui::Panel *parent) : vgui::CFDialog(parent, "Lobby")
{
	// Must be enable
	SetProportional(true);

	// Create top window
	m_pUI_FlowTop = new UIFlowTop(this);

	// Load layout setting
	LoadControlSettings("UI/Scripts/Lobby/Lobby_BaseNew.txt");

	// Load room info layout
	LoadControlSettings("UI/Scripts/Lobby/Lobby_RoomInfo.txt");

	// Load chat layout
	LoadControlSettings("UI/Scripts/Lobby/Lobby_Chat.txt");

	// Load buddy window
	LoadControlSettings("UI/Scripts/Buddy_CF20/FlowTop_Buddy.txt");

	char szOption[32];

	m_cStaticNotice = FIND_CONTROL_STATIC("StaticNotice");

	for (int i = 0; i < 13; i++)
	{
		sprintf(szOption, "ButtonRoomlist%d", i + 1);
		m_cButtonRoomlist[i] = FIND_CONTROL_BUTTON(szOption);
	}
	for (int i = 0; i < 13; i++)
	{
		sprintf(szOption, "StaticRoomNumber%d", i + 1);
		m_cStaticRoomNumberList[i] = FIND_CONTROL_STATIC(szOption);
	}
	for (int i = 0; i < 13; i++)
	{
		sprintf(szOption, "StaticRoomTitle%d", i + 1);
		m_cStaticRoomTitleList[i] = FIND_CONTROL_STATIC(szOption);
	}
	for (int i = 0; i < 13; i++)
	{
		sprintf(szOption, "StaticRoomRule%d", i + 1);
		m_cStaticRoomRuleList[i] = FIND_CONTROL_STATIC(szOption);
	}
	for (int i = 0; i < 13; i++)
	{
		sprintf(szOption, "StaticRoomMap%d", i + 1);
		m_cStaticRoomMapList[i] = FIND_CONTROL_STATIC(szOption);
	}
	for (int i = 0; i < 13; i++)
	{
		sprintf(szOption, "StaticRoomPerson%d", i + 1);
		m_cStaticRoomPersonList[i] = FIND_CONTROL_STATIC(szOption);
	}
	for (int i = 0; i < 13; i++)
	{
		sprintf(szOption, "StaticRoomState%d", i + 1);
		m_cStaticRoomStateList[i] = FIND_CONTROL_STATIC(szOption);
	}
	for (int i = 0; i < 13; i++)
	{
		sprintf(szOption, "ImageWeaponTypeIcon%d", i + 1);
		m_cImageWeaponTypeIconList[i] = FIND_CONTROL_IMAGE(szOption);
	}

	m_cScrollBarRoomList = FIND_CONTROL_SCROLLBAR("ScrollBarRoomList");
	m_cButtonMakeRoom = FIND_CONTROL_BUTTON("ButtonMakeRoom");
	m_cButtonRoomEnter = FIND_CONTROL_BUTTON("ButtonRoomEnter");

	//InfoTab::RoomInfo
	m_cImageRoomInfoBG = FIND_CONTROL_IMAGE("ImageRoomInfoBG");
	m_cImageMapInfo = FIND_CONTROL_IMAGE("ImageMapInfo");
	m_cImageSubRule = FIND_CONTROL_IMAGE("ImageSubRule");
	m_cStaticRoomMap = FIND_CONTROL_STATIC("StaticRoomMap");
	m_cStaticRoomRule = FIND_CONTROL_STATIC("StaticRoomRule");
	m_cStaticRoomGoal = FIND_CONTROL_STATIC("StaticRoomGoal");
	m_cStaticRoomState = FIND_CONTROL_STATIC("StaticRoomState");
	m_cStaticRoomOwner = FIND_CONTROL_STATIC("StaticRoomOwner");

	//InfoTab::MyInfo
	m_cImageMyInfoBG = FIND_CONTROL_IMAGE("ImageMyInfoBG");
	m_cStaticCallName = FIND_CONTROL_STATIC("StaticCallName");
	m_cImageMyCurClass = FIND_CONTROL_IMAGE("ImageMyCurClass");
	m_cImageMyNextClass = FIND_CONTROL_IMAGE("ImageMyNextClass");
	m_cImageEPGauge = FIND_CONTROL_IMAGE("ImageEPGauge");
	m_cStaticEP = FIND_CONTROL_STATIC("StaticEP");
	m_cStaticMyClanName = FIND_CONTROL_STATIC("StaticMyClanName");
	m_cStaticKillDeath = FIND_CONTROL_STATIC("StaticKillDeath");
	m_cStaticWinLose = FIND_CONTROL_STATIC("StaticWinLose");
	m_cStaticManner = FIND_CONTROL_STATIC("StaticManner");
	m_cStaticGP = FIND_CONTROL_STATIC("StaticGP");
	m_cStaticCP = FIND_CONTROL_STATIC("StaticCP");
	m_cButtonIDCard = FIND_CONTROL_BUTTON("ButtonIDCard");

	//InfoTab
	m_cButtonRoomInfo = FIND_CONTROL_RADIOBUTTON("BtnRoomInfo");
	m_cButtonMyInfo = FIND_CONTROL_RADIOBUTTON("BtnMyInfo");

	//InfoTab::RoomInfo
	m_cInfoTab.AddCtrl(0, m_cImageRoomInfoBG);
	m_cInfoTab.AddCtrl(0, m_cImageMapInfo);
	m_cInfoTab.AddCtrl(0, m_cImageSubRule);
	m_cInfoTab.AddCtrl(0, m_cStaticRoomMap);
	m_cInfoTab.AddCtrl(0, m_cStaticRoomRule);
	m_cInfoTab.AddCtrl(0, m_cStaticRoomGoal);
	m_cInfoTab.AddCtrl(0, m_cStaticRoomState);
	m_cInfoTab.AddCtrl(0, m_cStaticRoomOwner);

	//InfoTab::MyInfo
	m_cInfoTab.AddCtrl(1, m_cImageMyInfoBG);
	m_cInfoTab.AddCtrl(1, m_cStaticCallName);
	m_cInfoTab.AddCtrl(1, m_cImageMyCurClass);
	m_cInfoTab.AddCtrl(1, m_cImageMyNextClass);
	m_cInfoTab.AddCtrl(1, m_cImageEPGauge);
	m_cInfoTab.AddCtrl(1, m_cStaticEP);
	m_cInfoTab.AddCtrl(1, m_cStaticMyClanName);
	m_cInfoTab.AddCtrl(1, m_cStaticKillDeath);
	m_cInfoTab.AddCtrl(1, m_cStaticWinLose);
	m_cInfoTab.AddCtrl(1, m_cStaticManner);
	m_cInfoTab.AddCtrl(1, m_cStaticGP);
	m_cInfoTab.AddCtrl(1, m_cStaticCP);
	m_cInfoTab.AddCtrl(1, m_cButtonIDCard);

	for (int i = 0; i < 13; i++)
	{
		m_cButtonRoomlist[i]->SetCommand("Room");
		m_cButtonRoomlist[i]->GetCommand()->SetInt("Index", i);
	}

	m_cButtonMakeRoom->SetCommand("Make");
	m_cButtonRoomEnter->SetCommand("Enter");
}

void UILobby::Activate(void)
{
	BaseClass::Activate();

	m_cStaticNotice->SetText(L"%s - %s - ·¿¼ä.", gUI.m_szServer, gUI.m_szChannel);

	m_cImageMapInfo->SetImage("");
	m_cImageSubRule->SetImage("");
	m_cStaticRoomMap->SetText(L"");
	m_cStaticRoomRule->SetText(L"");
	m_cStaticRoomGoal->SetText(L"");
	m_cStaticRoomState->SetText(L"");
	m_cStaticRoomOwner->SetText(L"");

	int iCurClassEP = g_User.GetLevelEP(g_User.GetCurLevel());
	int iNextClassEP = g_User.GetLevelEP(g_User.GetNextLevel());
	int iCurrentEP = g_User.GetEP();

	int iGaugeWide = (iCurrentEP - iCurClassEP) / (float)(iNextClassEP - iCurClassEP) * PROPORT(100);

	m_cStaticCallName->SetText(g_User.GetName());
	m_cImageMyCurClass->SetImage("ui/class/class_%d.png", g_User.GetCurLevel());
	m_cImageMyNextClass->SetImage("ui/class/class_%d.png", g_User.GetNextLevel());
	m_cImageEPGauge->SetSize(iGaugeWide, 12);
	m_cStaticEP->SetText(L"%d/%d", iCurrentEP, iNextClassEP);
	m_cStaticMyClanName->SetText(g_User.GetClan());
	m_cStaticKillDeath->SetText(L"%d/%d", g_User.GetKill(), g_User.GetDeath());
	m_cStaticWinLose->SetText(L"%d/%d", g_User.GetWin(), g_User.GetLose());
	m_cStaticManner->SetText(L"...");
	m_cStaticGP->SetText(L"%d GP", g_User.GetGP());
	m_cStaticCP->SetText(L"%d CFµã", g_User.GetCP());

	// default show tab ROOMINFO
	m_cButtonRoomInfo->SetSelected(true);
}

void UILobby::OnCommandFull(KeyValues *params)
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
		gUI.GetChannelSelect()->Activate();
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

	if (!strcmp(szCommand, "Room"))
	{
		if (params->GetInt("IsDouble") == 1)
		{
			gUI.GetRoom()->Activate();
			Close();
		}
		return;
	}

	if (!strcmp(szCommand, "Enter"))
	{
		gUI.GetRoom()->Activate();
		Close();
		return;
	}
}

void UILobby::OnRadioButtonCheck(vgui::Panel *button, int tabposition)
{
	if (button == m_cButtonRoomInfo)
	{
		m_cInfoTab.Switch(0);
		return;
	}
	if (button == m_cButtonMyInfo)
	{
		m_cInfoTab.Switch(1);
		return;
	}
}

void UILobby::OnKeyCodePressed(vgui::KeyCode code)
{
	if (code == vgui::KEY_ESCAPE)
	{
		gUI.GetChannelSelect()->Activate();
		Close();
		return;
	}

	BaseClass::OnKeyCodePressed(code);
}