#ifndef CF_EDIT_H
#define CF_EDIT_H

#include <vgui_controls/TextEntry.h>

namespace vgui
{

class CFEdit : public TextEntry
{
	DECLARE_CLASS_SIMPLE(CFEdit, TextEntry);

public:
	CFEdit(Panel *parent, const char *panelName);

protected:
	virtual bool DrawCursor(int x, int y);
	virtual void OnMousePressed(MouseCode code);
	virtual void ApplySettings(KeyValues *inResourceData);
	virtual void ApplySchemeSettings(IScheme *pScheme);

private:
	char *		_fontName;
	Color		_textColor;
};

}

#endif