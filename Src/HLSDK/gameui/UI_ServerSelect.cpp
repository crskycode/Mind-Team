#include "UI_ServerSelect.h"
#include "CF_Base.h"
#include <KeyValues.h>
#include "filesystem.h"
#include "EngineInterface.h"
#include "UserSystem.h"
#include "UI_Globals.h"

/******
 *
 * dynamic_cast用于带有虚函数的class指针转换.
 *
 * 它可以保证class指针转换的安全, 如果你尝试将一个CFStatic的指针转换成CFImage指针, 那麽dynamic_cast将会返回null.
 *
 * dynamic_cast还允许子级指针转换为父级指针, 假如CFDialog基于EditPanel, 那么CFDialog指针到EditPanel指针的转换将会成功.
 *
 * dynamic_cast通过校检虚函数表来验证转换的安全性, 所以没有虚函数表的class指针是无法使用dynamic_cast来转换的.
 *
 * 它是在程序运行时进行校检的, 而不是在编译时.
 *
 * 使用dynamic_cast转换控件指针可以保证不会获取到错误的控件类型而导致程序崩溃.
 *
 * 当你需要转换控件指针时, 请务必使用dynamic_cast来操作.
 *
******/

UIServerSelect::UIServerSelect(vgui::Panel *parent) : vgui::CFDialog(parent, "ServerSelect")
{
	SetProportional(true);

	LoadControlSettings("UI/Scripts/ServerSelect.txt");

	char szOption[32];

	for (int i = 0; i < 34; i++)
	{
		sprintf(szOption, "ButtonServer%d", i + 1);
		m_cButtonServer[i] = FIND_CONTROL_BUTTON(szOption);
	}
	for (int i = 0; i < 34; i++)
	{
		sprintf(szOption, "StaticServerName%d", i + 1);
		m_cStaticName[i] = FIND_CONTROL_STATIC(szOption);
	}
	for (int i = 0; i < 34; i++)
	{
		sprintf(szOption, "StaticServerTraffic%d", i + 1);
		m_cStaticTraffic[i] = FIND_CONTROL_STATIC(szOption);
	}

	m_cButtonExit = FIND_CONTROL_BUTTON("ButtonExit");
	m_cButtonEnter = FIND_CONTROL_BUTTON("ButtonEnter");
	m_cScrollBarList = FIND_CONTROL_SCROLLBAR("ScrollBarServer");

	for (int i = 0; i < 34; i++)
	{
		m_cButtonServer[i]->SetCommand("ServerItem");
		m_cButtonServer[i]->GetCommand()->SetInt("Item", -1);
	}

	m_cButtonExit->SetCommand("Exit");
	m_cButtonEnter->SetCommand("Enter");

	LoadServerList();
}

void UIServerSelect::LoadServerList(void)
{
	KeyValues *pServerListData = new KeyValues("ServerList");;

	if (pServerListData == NULL)
	{
		Sys_Error("Out of memory.");
	}

	if (pServerListData->LoadFromFile(g_pFullFileSystem, "ServerList.vdf") != true)
	{
		Sys_Error("Failed to load ServerListData");
	}

	m_ServerItem.RemoveAll();

	for (KeyValues *pDat = pServerListData->GetFirstSubKey(); pDat; pDat = pDat->GetNextKey())
	{
		int i = m_ServerItem.AddToTail();
		wcscpy(m_ServerItem[i].name, pDat->GetWString("Name"));
		m_ServerItem[i].traffic = pDat->GetInt("Traffic");
	}

	pServerListData->deleteThis();

	int iCount = m_ServerItem.Count();

	if (iCount > 34)
	{
		int iStep = ceil((iCount - 34) / 2.0);

		m_cScrollBarList->SetEnabled(true);
		m_cScrollBarList->SetValue(0);
		m_cScrollBarList->SetRange(0, iStep);
	}
	else
	{
		m_cScrollBarList->SetEnabled(false);
		m_cScrollBarList->SetValue(0);
		m_cScrollBarList->SetRange(0, 1);
	}

	OnScrollBarMove( 0 );
}

void UIServerSelect::OnScrollBarMove(int iValue)
{
	int nIndex = iValue * 2;	// 2 rows
	int iCount = m_ServerItem.Count();

	for (int i = 0; i < 34; i++)
	{
		if (nIndex < iCount)
		{
			m_cButtonServer[i]->GetCommand()->SetInt("Item", nIndex);
			m_cStaticName[i]->SetText(m_ServerItem[nIndex].name);

			switch (m_ServerItem[nIndex].traffic)
			{
				case 0:
				{
					m_cStaticTraffic[i]->SetText(L"正常");
					m_cStaticTraffic[i]->SetFontColor(Color(0x36, 0x95, 0x3C, 0xFF));
					break;
				}
				case 1:
				{
					m_cStaticTraffic[i]->SetText(L"拥挤");
					m_cStaticTraffic[i]->SetFontColor(Color(0xE4, 0x7D, 0x1A, 0xFF));
					break;
				}
				case 2:
				{
					m_cStaticTraffic[i]->SetText(L"爆满");
					m_cStaticTraffic[i]->SetFontColor(Color(0xCC, 0x36, 0x36, 0xFF));
					break;
				}
			}

			nIndex++;
		}
		else
		{
			m_cButtonServer[i]->GetCommand()->SetInt("Item", -1);
			m_cStaticName[i]->SetText(L"");
			m_cStaticTraffic[i]->SetText(L"");
		}
	}
}

void UIServerSelect::Activate(void)
{
	BaseClass::Activate();

	if (g_User.IsFirstLogin())
	{
		gUI.GetCharacterCreate()->Activate();
	}
}

void UIServerSelect::OnCommandFull(KeyValues *params)
{
	const char *szCommand = params->GetString("command");

	if (!strcmp(szCommand, "Exit"))
	{
		gUI.GetGameEnd()->Activate(this);
		return;
	}

	if (!strcmp(szCommand, "Enter"))
	{
		//gUI.GetResult()->Activate();
		//Close();
		gEngfuncs.pfnClientCmd(
			"disconnect\nwait\nwait\nsv_lan 1\nsetmaster disable\nmaxplayers 16\nsv_password \"\"\nhostname \"Mind-Team Server\"\nprogress_enable\nmap template\n");
		return;
	}

	if (!strcmp(szCommand, "ServerItem"))
	{
		if (params->GetInt("IsDouble") == 1)
		{
			int index = params->GetInt("Item");

			if (index == -1)
				return;

			wcscpy(gUI.m_szServer, m_ServerItem[index].name);
			gUI.GetChannelSelect()->Activate();
			Close();
		}
		return;
	}
}

void UIServerSelect::OnKeyCodePressed(vgui::KeyCode code)
{
	if (code == vgui::KEY_ESCAPE)
	{
		gUI.GetGameEnd()->Activate(this);
		return;
	}

	BaseClass::OnKeyCodePressed(code);
}