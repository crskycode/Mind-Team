#ifndef UI_SERVERSELECT_H
#define UI_SERVERSELECT_H

#include "CF_Dialog.h"
#include <utlvector.h>

namespace vgui
{
	class CFButton;
	class CFStatic;
	class CFScrollBar;
}

class UIServerSelect : public vgui::CFDialog
{
	DECLARE_CLASS_SIMPLE(UIServerSelect, vgui::CFDialog);

public:
	UIServerSelect(vgui::Panel *parent);

	virtual void Activate(void);

	void LoadServerList(void);

protected:
	// this function to handle button double pressed action signal
	MESSAGE_FUNC_PARAMS(OnCommandFull, "Command", params);
	virtual void OnKeyCodePressed(vgui::KeyCode code);

private:
	MESSAGE_FUNC_INT(OnScrollBarMove, "ScrollBarMoved", value);

	struct ServerEntry
	{
		wchar_t	name[32];
		int		traffic;
	};

	CUtlVector<ServerEntry>	m_ServerItem;

	vgui::CFButton *		m_cButtonExit;
	vgui::CFButton *		m_cButtonEnter;

	vgui::CFScrollBar *		m_cScrollBarList;

	vgui::CFButton *		m_cButtonServer[34];
	vgui::CFStatic *		m_cStaticName[34];
	vgui::CFStatic *		m_cStaticTraffic[34];
};

#endif