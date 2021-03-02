#ifndef CF_DIALOG_H
#define CF_DIALOG_H

#include <vgui_controls/Frame.h>

namespace vgui
{

class CFDialog : public Frame
{
	DECLARE_CLASS_SIMPLE(CFDialog, Frame);

public:
	CFDialog(Panel *parent, const char *panelName);

protected:
	virtual void MoveToCenterOfScreen(void);
	virtual void PaintBackground(void);
	virtual void Paint(void);
	virtual void PaintBorder(void);
	virtual void ApplySchemeSettings(IScheme *pScheme);
};

}

#endif