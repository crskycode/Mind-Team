#include "UI_GameEnd.h"
#include "CF_Base.h"
#include "EngineInterface.h"
#include "UserSystem.h"
#include <keyvalues.h>

UIGameEnd::UIGameEnd(vgui::Panel *parent) : vgui::CFDialog(parent, "GameEnd")
{
	SetProportional(true);

	LoadControlSettings("UI/Scripts/GameEnd.txt");

	m_cButtonGameEnd = FIND_CONTROL_BUTTON("ButtonGameEnd");
	m_cButtonMoveServer = FIND_CONTROL_BUTTON("ButtonMoveServer");
	m_cButtonCancel = FIND_CONTROL_BUTTON("ButtonCancel");
	m_cStaticNameComment = FIND_CONTROL_STATIC("StaticNameComment");
	m_cStaticMoreGameComment = FIND_CONTROL_STATIC("StaticMoreGameComment");
	m_cStaticMoreRound = FIND_CONTROL_STATIC("StaticMoreRound");
	m_cStaticNextClass = FIND_CONTROL_STATIC("StaticNextClass");
	m_cStaticNoEP = FIND_CONTROL_STATIC("StaticNoEP");
	m_cStaticTodayEP = FIND_CONTROL_STATIC("StaticTodayEP");
	m_cStaticTotalEP = FIND_CONTROL_STATIC("StaticTotalEP");
	m_cStaticTodayGP = FIND_CONTROL_STATIC("StaticTodayGP");
	m_cStaticTotalGP = FIND_CONTROL_STATIC("StaticTotalGP");
	m_cStaticWinLose = FIND_CONTROL_STATIC("StaticWinLose");
	m_cStaticKill = FIND_CONTROL_STATIC("StaticKill");
	m_cStaticDeath = FIND_CONTROL_STATIC("StaticDeath");
	m_cStaticKillDeath = FIND_CONTROL_STATIC("StaticKillDeath");
	m_cStaticHeadShot = FIND_CONTROL_STATIC("StaticHeadShot");

	m_cButtonGameEnd->SetCommand("GameEnd");
	m_cButtonMoveServer->SetCommand("ServerList");
	m_cButtonCancel->SetCommand("Cancel");

	m_pOwnerPanel = NULL;
}

void UIGameEnd::Activate(vgui::Panel *pOwnerPanel)
{
	BaseClass::Activate();

	m_pOwnerPanel = pOwnerPanel;

	m_cStaticNameComment->SetText( L"'%s'的 今日战绩", g_User.GetName() );
}

void UIGameEnd::OnCommand(const char *szCommand)
{
	if (!strcmp(szCommand, "GameEnd"))
	{
		gEngfuncs.pfnClientCmd("exit\n");
	}
	else if (!strcmp(szCommand, "ServerList"))
	{
		if (m_pOwnerPanel)
		{
			PostMessage(m_pOwnerPanel, new KeyValues("Command", "command", szCommand));
		}
		Close();
	}
	else if (!strcmp(szCommand, "Cancel"))
	{
		Close();
	}
}

void UIGameEnd::OnKeyCodePressed(vgui::KeyCode code)
{
	if (code == vgui::KEY_ESCAPE)
	{
		Close();
		return;
	}

	BaseClass::OnKeyCodePressed(code);
}