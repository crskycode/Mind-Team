#ifndef UI_CHARACTERCREATE_H
#define UI_CHARACTERCREATE_H

#include "CF_Dialog.h"

namespace vgui
{
	class CFButton;
	class CFStatic;
	class CFEdit;
}

class UICharacterCreate : public vgui::CFDialog
{
	DECLARE_CLASS_SIMPLE(UICharacterCreate, vgui::CFDialog);

public:
	UICharacterCreate(vgui::Panel *parent);

	virtual void Activate(void);

protected:
	MESSAGE_FUNC(OnTextChanged, "TextChanged");

	virtual void OnCommand(const char *command);

private:
	vgui::CFButton	*m_cButtonCheck;
	vgui::CFButton	*m_cButtonCreate;
	vgui::CFButton	*m_cButtonCancel;
	vgui::CFStatic	*m_cStaticResult;
	vgui::CFEdit	*m_cEditCharName;
};

#endif