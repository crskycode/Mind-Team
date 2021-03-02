#include "CF_Edit.h"
#include <KeyValues.h>

using namespace vgui;

// Force build it !
DECLARE_BUILD_FACTORY(CFEdit);

CFEdit::CFEdit(Panel *parent, const char *panelName) : TextEntry(parent, panelName)
{
	_fontName = NULL;
	_textColor.SetColor(255, 255, 255, 255);
}

bool CFEdit::DrawCursor(int x, int y)
{
	// We can't access the private member of TextEntry,
	//  so get them from memory
	
	// TextEntry::_cursorBlink
	bool *blink = reinterpret_cast<bool *>(reinterpret_cast<byte *>(this) + 0x104);

	// TextEntry::_cursorPos
	int *pos = reinterpret_cast<int *>(reinterpret_cast<byte *>(this) + 0xF8);

	// TextEntry::_font
	HFont *font = reinterpret_cast<HFont *>(reinterpret_cast<byte *>(this) + 0x12C);

	// Should draw it ?
	if (!*blink)
	{
		int cx, cy;

		// Get cursor position from text pos
		CursorToPixelSpace(*pos, cx, cy);

		// Setup cursor color
		surface()->DrawSetColor(Color(0, 255, 255, 255));

		// Get cursor tall
		int fontTall = surface()->GetFontTall(*font);

		// Draw cursor
		surface()->DrawFilledRect(cx, cy, cx + 2, cy + fontTall);

		return true;
	}

	return false;
}

void CFEdit::OnMousePressed(MouseCode code)
{
	// Block the edit menu
	if (code == MOUSE_RIGHT)
		return;

	BaseClass::OnMousePressed(code);
}

void CFEdit::ApplySettings(KeyValues *inResourceData)
{
	BaseClass::ApplySettings(inResourceData);

	// Get font name from layout setting
	const char *fontName = inResourceData->GetString("font");

	// We got the font name
	if (fontName && *fontName)
	{
		// Delete the old name
		if (_fontName)
		{
			delete[] _fontName;
		}

		_fontName = new char[strlen(fontName) + 1];

		strcpy(_fontName, fontName);

		// Get scheme interface
		IScheme *pScheme = scheme()->GetIScheme(GetScheme());

		if (pScheme)
		{
			// Find the font
			HFont font = pScheme->GetFont(_fontName, true);

			if (font != INVALID_FONT)
			{
				SetFont(font);
			}
		}
	}
	
	// Get the text color from layout setting
	const char *option = inResourceData->GetString("textColor");

	if (option && *option)
	{
		int clr[3];

		// Parse the color
		sscanf(option, "%d %d %d", &clr[0], &clr[1], &clr[2]);

		// Save the color
		_textColor.SetColor(clr[0], clr[1], clr[2], 255);

		// Set text color
		SetFgColor(_textColor);
	}
}

void CFEdit::ApplySchemeSettings(IScheme *pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);

	// Unset edit border
	SetBorder(NULL);

	// No Bg color
	SetBgColor(Color());

	// Apply text font
	if (_fontName)
	{
		// Find the font
		HFont font = pScheme->GetFont(_fontName);

		if (font != INVALID_FONT)
		{
			SetFont(font);
		}
	}

	// Set text color
	SetFgColor(_textColor);
	SetSelectionBgColor(Color(0, 255, 255, 255));
}