#ifndef UI_FLOWTOP_H
#define UI_FLOWTOP_H

#include "CF_Window.h"

namespace vgui
{
	class CFButton;
}

class UIFlowTop : public vgui::CFWindow
{
	DECLARE_CLASS_SIMPLE(UIFlowTop, vgui::CFWindow);

public:
	UIFlowTop(vgui::Panel *parent);

protected:
	virtual void OnCommand(const char *command);

private:
	vgui::CFButton	*m_cButtonGachaShop;
	vgui::CFButton	*m_cButtonInven;
	vgui::CFButton	*m_cButtonShop;
	vgui::CFButton	*m_cButtonMPointMall;
	vgui::CFButton	*m_cButtonBack;
	vgui::CFButton	*m_cButtonInClan;
	vgui::CFButton	*m_cButtonAutoMatchTry;
	vgui::CFButton	*m_cButtonBuddy;
	vgui::CFButton	*m_cButtonOption;
	vgui::CFButton	*m_cButtonExit;
};

#endif