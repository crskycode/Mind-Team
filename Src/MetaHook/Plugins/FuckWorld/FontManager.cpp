#include "FontManager.h"

static CFontManager s_FontManager;

CFontManager &FontManager(void)
{
	return s_FontManager;
}

CFontManager::CFontManager()
{
	m_FontAmalgams.EnsureCapacity(100);
	m_FontAmalgams.AddToTail();
	m_Win32Fonts.EnsureCapacity(100);
}

CFontManager::~CFontManager()
{
	ClearAllFonts();
}

void CFontManager::ClearAllFonts()
{
	for (int i = 0; i < m_Win32Fonts.Count(); i++)
	{
		delete m_Win32Fonts[i];
	}
	m_Win32Fonts.RemoveAll();
}

int CFontManager::CreateFont()
{
	int i = m_FontAmalgams.AddToTail();
	return i;
}

bool CFontManager::AddGlyphSetToFont(int font, const char *windowsFontName, int tall, int weight, int blur, int scanlines, int flags, int lowRange, int highRange)
{
	int i;

	CWin32Font *winFont = NULL;
	for (int i = 0; i < m_Win32Fonts.Count(); i++)
	{
		if (m_Win32Fonts[i]->IsEqualTo(windowsFontName, tall, weight, blur, scanlines, flags))
		{
			winFont = m_Win32Fonts[i];
			break;
		}
	}

	if (!winFont)
	{
		i = m_Win32Fonts.AddToTail();

		m_Win32Fonts[i] = new CWin32Font();
		if (m_Win32Fonts[i]->Create(windowsFontName, tall, weight, blur, scanlines, flags))
		{
			winFont = m_Win32Fonts[i];
		}
		else
		{
			delete m_Win32Fonts[i];
			m_Win32Fonts.Remove(i);
			return false;
		}
	}

	m_FontAmalgams[font].AddFont(winFont, lowRange, highRange);
	return true;
}

int CFontManager::GetFontByName(const char *name)
{
	for (int i = 1; i < m_FontAmalgams.Count(); i++)
	{
		if (!stricmp(name, m_FontAmalgams[i].Name()))
		{
			return i;
		}
	}
	return 0;
}

CWin32Font *CFontManager::GetFontForChar(int font, wchar_t wch)
{
	return m_FontAmalgams[font].GetFontForChar(wch);
}

void CFontManager::GetCharABCwide(int font, int ch, int &a, int &b, int &c)
{
	CWin32Font *winFont = m_FontAmalgams[font].GetFontForChar(ch);
	if (winFont)
	{
		winFont->GetCharABCWidths(ch, a, b, c);
	}
	else
	{
		a = c = 0;
		b = m_FontAmalgams[font].GetFontMaxWidth();
	}
}

int CFontManager::GetFontTall(int font)
{
	return m_FontAmalgams[font].GetFontHeight();
}

int CFontManager::GetFontAscent(int font, wchar_t wch)
{
	CWin32Font *winFont = m_FontAmalgams[font].GetFontForChar(wch);

	if (winFont)
		return winFont->GetAscent();
	else
		return 0;
}

int CFontManager::GetFontFlags(int font)
{
	return m_FontAmalgams[font].GetFlags(0);
}

int CFontManager::GetCharacterWidth(int font, int ch)
{
	int a, b, c;
	GetCharABCwide(font, ch, a, b, c);
	return (a + b + c);
}

void CFontManager::GetTextSize(int font, const wchar_t *text, int &wide, int &tall)
{
	wide = 0;
	tall = 0;

	if (!text)
		return;

	tall = GetFontTall(font);

	int xx = 0;
	for (int i = 0;; i++)
	{
		wchar_t ch = text[i];
		if (ch == 0)
		{
			break;
		}
		else if (ch == '\n')
		{
			tall += GetFontTall(font);
			xx = 0;
		}
		else if (ch == '\t')
		{
		}
		else
		{
			xx += GetCharacterWidth(font, ch);
			if (xx > wide)
			{
				wide = xx;
			}
		}
	}
}