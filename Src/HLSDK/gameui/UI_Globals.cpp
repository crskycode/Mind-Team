#include "UI_Globals.h"

// Static Instance
CCFUI gUI;

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CCFUI::Initialize(vgui::Panel *parent)
{
	m_pUI_BasePanel = parent;
	m_szServer[0] = L'\0';
	m_szChannel[0] = L'\0';
	m_szRoom[0] = L'\0';

	SAFE_INITIALIZE_UI(ServerSelect);
	SAFE_INITIALIZE_UI(ChannelSelect);
	SAFE_INITIALIZE_UI(Lobby);
	SAFE_INITIALIZE_UI(Room);
	SAFE_INITIALIZE_UI(Inven);
	SAFE_INITIALIZE_UI(Result);
	SAFE_INITIALIZE_UI(Loading);
	SAFE_INITIALIZE_UI(CharacterCreate);
	SAFE_INITIALIZE_UI(FirstLoginNotice);
	SAFE_INITIALIZE_UI(GameEnd);
	SAFE_INITIALIZE_UI(MsgWnd);
	SAFE_INITIALIZE_UI(Option);
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CCFUI::Shutdown(void)
{
	SAFE_DELETE_UI(ServerSelect);
	SAFE_DELETE_UI(ChannelSelect);
	SAFE_DELETE_UI(Lobby);
	SAFE_DELETE_UI(Room);
	SAFE_DELETE_UI(Inven);
	SAFE_DELETE_UI(Result);
	SAFE_DELETE_UI(Loading);
	SAFE_DELETE_UI(CharacterCreate);
	SAFE_DELETE_UI(FirstLoginNotice);
	SAFE_DELETE_UI(GameEnd);
	SAFE_DELETE_UI(MsgWnd);
	SAFE_DELETE_UI(Option);
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
	SAFE_RETURN_UI(ServerSelect);
	SAFE_RETURN_UI(ChannelSelect);
	SAFE_RETURN_UI(Lobby);
	SAFE_RETURN_UI(Room);
	SAFE_RETURN_UI(Inven);
	SAFE_RETURN_UI(Result);
	SAFE_RETURN_UI(Loading);
	SAFE_RETURN_UI(CharacterCreate);
	SAFE_RETURN_UI(FirstLoginNotice);
	SAFE_RETURN_UI(GameEnd);
	SAFE_RETURN_UI(MsgWnd);
	SAFE_RETURN_UI(Option);

//-----------------------------------------------------------------------------
// Purpose: Start CFGUI System
//-----------------------------------------------------------------------------
void CCFUI::Start(void)
{
	GetServerSelect()->Activate();
}