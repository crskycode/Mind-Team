#include "UI_FlowTop.h"
#include "CF_Base.h"
#include <keyvalues.h>

UIFlowTop::UIFlowTop(vgui::Panel *parent) : vgui::CFWindow(NULL, "FlowTop")
{
	// Make sure the parent window is NULL before the load control setting,
	//  so the child controls are not affect by the parent window

	// Setup coordinate space
	SetProportionalBase(1024, 768);

	// Load the layout setting file
	LoadControlSettings("UI/Scripts/WindowGroups/Window_FlowTop.txt");

	// Setup the parent window
	SetParent(parent);

	// Get button controls
	m_cButtonGachaShop = FIND_CONTROL_BUTTON("ButtonGachaShop");
	m_cButtonInven = FIND_CONTROL_BUTTON("ButtonInven");
	m_cButtonShop = FIND_CONTROL_BUTTON("ButtonShop");
	m_cButtonMPointMall = FIND_CONTROL_BUTTON("ButtonMPointMall");
	m_cButtonBack = FIND_CONTROL_BUTTON("ButtonBack");
	m_cButtonInClan = FIND_CONTROL_BUTTON("ButtonInClan");
	m_cButtonAutoMatchTry = FIND_CONTROL_BUTTON("ButtonAutoMatchTry");
	m_cButtonBuddy = FIND_CONTROL_BUTTON("ButtonBuddy");
	m_cButtonOption = FIND_CONTROL_BUTTON("ButtonOption");
	m_cButtonExit = FIND_CONTROL_BUTTON("ButtonExit");

	// Setup button command
	m_cButtonGachaShop->SetCommand("GachaShop");
	m_cButtonInven->SetCommand("Inven");
	m_cButtonShop->SetCommand("Shop");
	m_cButtonMPointMall->SetCommand("MPointMall");
	m_cButtonBack->SetCommand("Back");
	m_cButtonInClan->SetCommand("InClan");
	m_cButtonAutoMatchTry->SetCommand("AutoMatchTry");
	m_cButtonBuddy->SetCommand("Buddy");
	m_cButtonOption->SetCommand("Option");
	m_cButtonExit->SetCommand("Exit");
}

void UIFlowTop::OnCommand(const char *command)
{
	// Forward to parent window handle button command
	PostActionSignal(new KeyValues("Command", "command", command));
}