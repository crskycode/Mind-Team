#ifndef FONTMANAGER_H
#define FONTMANAGER_H

#include "FontAmalgam.h"

#ifdef CreateFont
#undef CreateFont
#endif

#ifndef FONTFLAG
#define FONTFLAG

enum FontFlag
{
	FONTFLAG_NONE,
	FONTFLAG_ITALIC = 0x001,
	FONTFLAG_UNDERLINE = 0x002,
	FONTFLAG_STRIKEOUT = 0x004,
	FONTFLAG_SYMBOL = 0x008,
	FONTFLAG_ANTIALIAS = 0x010,
	FONTFLAG_OUTLINE = 0x200,
};

#endif

class CFontManager
{
public:
	CFontManager();
	~CFontManager();

	void ClearAllFonts();
	int CreateFont();
	bool AddGlyphSetToFont(int font, const char *windowsFontName, int tall, int weight, int blur, int scanlines, int flags, int lowRange, int highRange);
	int GetFontByName(const char *name);
	void GetCharABCwide(int font, int ch, int &a, int &b, int &c);
	int GetFontTall(int font);
	int GetFontAscent(int font, wchar_t wch);
	int GetCharacterWidth(int font, int ch);
	void GetTextSize(int font, const wchar_t *text, int &wide, int &tall);
	CWin32Font *GetFontForChar(int font, wchar_t wch);
	int GetFontFlags(int font);

private:
	CUtlVector<CFontAmalgam> m_FontAmalgams;
	CUtlVector<CWin32Font *> m_Win32Fonts;
};

CFontManager &FontManager();

#endif