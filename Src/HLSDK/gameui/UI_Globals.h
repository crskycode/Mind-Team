#ifndef UI_GLOBALS_H
#define UI_GLOBALS_H

#include "UI_ServerSelect.h"
#include "UI_ChannelSelect.h"
#include "UI_Lobby.h"
#include "UI_Room.h"
#include "UI_Inven.h"
#include "UI_Result.h"
#include "UI_Loading.h"

#include "UI_CharacterCreate.h"
#include "UI_FirstLoginNotice.h"
#include "UI_GameEnd.h"
#include "UI_MsgWnd.h"
#include "UI_Option.h"


//-----------------------------------------------------------------------------
// Purpose: Declare UI handle
//-----------------------------------------------------------------------------
#define SAFE_DECLARE_UI(name) \
	public: \
		UI##name *Get##name(void); \
	private: \
		UI##name *m_pUI_##name;

//-----------------------------------------------------------------------------
// Purpose: Initialize UI
//-----------------------------------------------------------------------------
#define SAFE_INITIALIZE_UI(name) \
	{ \
		m_pUI_##name = NULL; \
	}

//-----------------------------------------------------------------------------
// Purpose: Returns UI
//-----------------------------------------------------------------------------
#define SAFE_RETURN_UI(name) \
	UI##name *CCFUI::Get##name(void) \
	{ \
		if (!m_pUI_##name) \
			m_pUI_##name = new UI##name(m_pUI_BasePanel); \
			\
		return m_pUI_##name; \
	}

//-----------------------------------------------------------------------------
// Purpose: Release UI
//-----------------------------------------------------------------------------
#define SAFE_DELETE_UI(name) \
	{ \
		if (m_pUI_##name) \
			m_pUI_##name->DeletePanel(); \
	}


//-----------------------------------------------------------------------------
// Purpose: CFGUI System
//-----------------------------------------------------------------------------
class CCFUI
{
public:
	void Initialize(vgui::Panel *parent);
	void Shutdown(void);
	void Start(void);

	SAFE_DECLARE_UI(ServerSelect);
	SAFE_DECLARE_UI(ChannelSelect);
	SAFE_DECLARE_UI(Lobby);
	SAFE_DECLARE_UI(Room);
	SAFE_DECLARE_UI(Inven);
	SAFE_DECLARE_UI(Result);
	SAFE_DECLARE_UI(Loading);
	SAFE_DECLARE_UI(CharacterCreate);
	SAFE_DECLARE_UI(FirstLoginNotice);
	SAFE_DECLARE_UI(GameEnd);
	SAFE_DECLARE_UI(MsgWnd);
	SAFE_DECLARE_UI(Option);

private:
	// GameUI base panel
	vgui::Panel			*m_pUI_BasePanel;

public:
	// current server name
	wchar_t				m_szServer[32];
	wchar_t				m_szChannel[32];
	wchar_t				m_szRoom[32];
};

extern CCFUI gUI;

#endif