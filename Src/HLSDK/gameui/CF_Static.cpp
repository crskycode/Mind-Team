#include "CF_Static.h"
#include <KeyValues.h>

using namespace vgui;

// Force build it !
DECLARE_BUILD_FACTORY(CFStatic);

CFStatic::CFStatic(Panel *parent, const char *panelName) : Label(parent, panelName, L"Static")
{
	// Ignore mouse input
	DisableMouseInputForThisPanel(true);

	// Setup font color
	_color.SetColor(255, 255, 255, 255);
}

void CFStatic::SetFontColor(Color color)
{
	_color = color;
	SetFgColor(_color);
}

void CFStatic::SetText(const wchar_t *unicodeString, ...)
{
	va_list arg;
	static wchar_t string[1024];

	va_start(arg, unicodeString);
	vswprintf(string, sizeof(string) / sizeof(wchar_t), unicodeString, arg);
	va_end(arg);

	BaseClass::SetText(string);
}

void CFStatic::ApplySettings(KeyValues *inResourceData)
{
	BaseClass::ApplySettings(inResourceData);

	const char *option = inResourceData->GetString("textColor");

	if (option && *option)
	{
		int clr[3];

		// Parse the option
		sscanf(option, "%d %d %d", &clr[0], &clr[1], &clr[2]);

		// Copy color
		_color.SetColor(clr[0], clr[1], clr[2], 255);

		// Setup text color
		SetFgColor(_color);
	}
}

void CFStatic::ApplySchemeSettings(IScheme *pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);

	// Override the color
	SetFgColor(_color);
}