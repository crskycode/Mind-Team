#include "UI_FirstLoginNotice.h"
#include "CF_Base.h"

UIFirstLoginNotice::UIFirstLoginNotice(vgui::Panel *parent) : vgui::CFDialog(parent, "FirstLoginNotice")
{
	SetProportional(true);

	LoadControlSettings("UI/Scripts/FirstLoginNotice.txt");

	m_cButtonOk = FIND_CONTROL_BUTTON("ButtonOk");
	m_cButtonCancel = FIND_CONTROL_BUTTON("ButtonCancel");
	m_cRadioCharacter[0] = FIND_CONTROL_RADIOBUTTON("RadioCharacter1");
	m_cRadioCharacter[1] = FIND_CONTROL_RADIOBUTTON("RadioCharacter2");
	m_cRadioCharacter[2] = FIND_CONTROL_RADIOBUTTON("RadioCharacter3");
	m_cStaticOwnGP = FIND_CONTROL_STATIC("StaticOwnGP");
	m_cStaticNeadGP = FIND_CONTROL_STATIC("StaticNeadGP");

	m_cButtonOk->SetCommand("OK");
	m_cButtonCancel->SetCommand("Cancel");
}

void UIFirstLoginNotice::Activate(void)
{
	BaseClass::Activate();

	m_cRadioCharacter[0]->SetSelected(false);
	m_cRadioCharacter[1]->SetSelected(false);
	m_cRadioCharacter[2]->SetSelected(false);

	m_cStaticNeadGP->SetText(L"0 GP");
}

void UIFirstLoginNotice::OnRadioButtonCheck(vgui::Panel *panel)
{
	if (panel == m_cRadioCharacter[0])
	{
		m_cStaticNeadGP->SetText(L"5000 GP");
		return;
	}

	if (panel == m_cRadioCharacter[1])
	{
		m_cStaticNeadGP->SetText(L"5000 GP");
		return;
	}

	if (panel == m_cRadioCharacter[2])
	{
		m_cStaticNeadGP->SetText(L"5000 GP");
		return;
	}
}

void UIFirstLoginNotice::OnCommand(const char *command)
{
	if (!strcmp(command, "OK"))
	{
		CloseModal();
		return;
	}

	if (!strcmp(command, "Cancel"))
	{
		CloseModal();
		return;
	}
}