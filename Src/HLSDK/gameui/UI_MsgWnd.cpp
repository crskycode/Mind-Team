#include "UI_MsgWnd.h"
#include "CF_Base.h"

UIMsgWnd::UIMsgWnd(vgui::Panel *parent) : vgui::CFDialog(parent, "MsgWnd")
{
	SetProportional(true);

	LoadControlSettings("UI/Scripts/MsgWnd.txt");

	m_cStaticMsg = FIND_CONTROL_STATIC("StaticMsg");
}

void UIMsgWnd::Create(int type, const wchar_t *msg)
{
	DoModal();

	m_cStaticMsg->SetText( msg );
}

void UIMsgWnd::Destory(void)
{
	if ( IsVisible() )
	{
		CloseModal();
	}
}

void UIMsgWnd::OnCommand(const char *command)
{
}