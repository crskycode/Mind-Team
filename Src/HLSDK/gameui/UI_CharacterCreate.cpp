#include "UI_CharacterCreate.h"
#include "CF_Base.h"
#include "EngineInterface.h"
#include "UserSystem.h"

UICharacterCreate::UICharacterCreate(vgui::Panel *parent) : vgui::CFDialog(parent, "CharacterCreate")
{
	SetProportional(true);

	LoadControlSettings("UI/Scripts/CharacterCreate.txt");

	m_cButtonCheck = FIND_CONTROL_BUTTON("ButtonCheck");
	m_cButtonCreate = FIND_CONTROL_BUTTON("ButtonCreate");
	m_cButtonCancel = FIND_CONTROL_BUTTON("ButtonBack");
	m_cStaticResult = FIND_CONTROL_STATIC("StaticCheckResult");
	m_cEditCharName = FIND_CONTROL_EDIT("EditCharName");

	m_cButtonCheck->SetCommand("Check");
	m_cButtonCreate->SetCommand("Create");
	m_cButtonCancel->SetCommand("Cancel");

	OnTextChanged();
}

void UICharacterCreate::Activate(void)
{
	BaseClass::Activate();

	OnTextChanged();

	m_cEditCharName->RequestFocus();
	m_cEditCharName->SetText(L"");
}

void UICharacterCreate::OnTextChanged(void)
{
	m_cStaticResult->SetText(L"");
	m_cButtonCreate->SetEnabled(false);
}

void UICharacterCreate::OnCommand(const char *szCommand)
{
	if (!strcmp(szCommand, "Check"))
	{
		wchar_t szName[12];

		bool bResult = true;

		m_cEditCharName->GetText(szName, sizeof(szName));

		if (!wcslen(szName))
		{
			bResult = false;
		}
		else
		{
			for (int i = 0; i < 12; i++)
			{
				if (szName[i] == L'/' || 
					szName[i] == L'\\' || 
					szName[i] == L':' || 
					szName[i] == L'*' || 
					szName[i] == L'?' || 
					szName[i] == L'\"' ||
					szName[i] == L'<' ||
					szName[i] == L'>' ||
					szName[i] == L'|' ||
					szName[i] == L' ' ||
					szName[i] == L'\t' ||
					szName[i] == L'\r' ||
					szName[i] == L'\n')
				{
					bResult = false;
					break;
				}
			}
		}

		if (bResult)
		{
			m_cStaticResult->SetText(L"您可以使用该昵称。确定要使用该昵称吗?");
			m_cButtonCreate->SetEnabled(true);
		}
		else
		{
			m_cStaticResult->SetText(L"您不可使用该昵称。");
			m_cButtonCreate->SetEnabled(false);
		}
	}
	else if (!strcmp(szCommand, "Create"))
	{
		wchar_t szName[12];

		m_cEditCharName->GetText(szName, sizeof(szName));

		g_User.SetName(szName);

		Close();
	}
	else if (!strcmp(szCommand, "Cancel"))
	{
		Close();
		gEngfuncs.pfnClientCmd("exit\n");
	}
}