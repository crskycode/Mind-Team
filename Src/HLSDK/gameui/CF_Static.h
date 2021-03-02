#ifndef CF_STATIC_H
#define CF_STATIC_H

#include <vgui_controls/Label.h>

namespace vgui
{

class CFStatic : public Label
{
	DECLARE_CLASS_SIMPLE(CFStatic, Label);

public:
	CFStatic(Panel *parent, const char *panelName);

	virtual void SetFontColor(Color color);

	virtual void SetText(const wchar_t *unicodeString, ...);

protected:
	virtual void ApplySettings(KeyValues *inResourceData);
	virtual void ApplySchemeSettings(IScheme *pScheme);

private:
	Color	_color;
};

}

#endif