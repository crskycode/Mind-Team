#ifndef UI_FIRSTLOGINNOTICE_H
#define UI_FIRSTLOGINNOTICE_H

#include "CF_Dialog.h"

namespace vgui
{
	class CFStatic;
	class CFButton;
	class CFRadioButton;
}

class UIFirstLoginNotice : public vgui::CFDialog
{
	DECLARE_CLASS_SIMPLE(UIFirstLoginNotice, vgui::CFDialog);

public:
	UIFirstLoginNotice(vgui::Panel *parent);

	virtual void Activate(void);

protected:
	MESSAGE_FUNC_PTR(OnRadioButtonCheck, "RadioButtonChecked", panel);
	virtual void OnCommand(const char *command);

private:
	vgui::CFButton			*m_cButtonOk;
	vgui::CFButton			*m_cButtonCancel;
	vgui::CFRadioButton		*m_cRadioCharacter[3];
	vgui::CFStatic			*m_cStaticOwnGP;
	vgui::CFStatic			*m_cStaticNeadGP;
};

#endif