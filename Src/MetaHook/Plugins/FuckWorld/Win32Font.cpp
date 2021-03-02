#include "Win32Font.h"
#include "FontManager.h"
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <malloc.h>

CWin32Font::CWin32Font()
{
	m_szName[0] = 0;
	m_iTall = 0;
	m_iWeight = 0;
	m_iFlags = 0;
	m_iMaxCharWidth = 0;
	m_hFont = NULL;
	m_hDC = NULL;
	m_hDIB = NULL;
	m_bAntiAliased = false;
}

CWin32Font::~CWin32Font()
{
	if (m_hFont)
		::DeleteObject(m_hFont);

	if (m_hDC)
		::DeleteDC(m_hDC);

	if (m_hDIB)
		::DeleteObject(m_hDIB);
}

extern bool g_bFontFound = false;

int CALLBACK FontEnumProc(const LOGFONT *lpelfe,
	const TEXTMETRIC *lpntme,
	DWORD FontType,
	LPARAM lParam
	)
{
	g_bFontFound = true;
	return 0;
}

bool CWin32Font::Create(const char *windowsFontName, int tall, int weight, int blur, int scanlines, int flags)
{
	strncpy(m_szName, windowsFontName, sizeof(m_szName)-1);
	m_szName[sizeof(m_szName)-1] = 0;
	m_iTall = tall;
	m_iWeight = weight;
	m_iFlags = flags;
	m_bAntiAliased = flags & FONTFLAG_ANTIALIAS;
	m_fOutlined = (flags & FONTFLAG_OUTLINE) ? TRUE : FALSE;

	int charset = (flags & FONTFLAG_SYMBOL) ? SYMBOL_CHARSET : ANSI_CHARSET;

	m_hDC = ::CreateCompatibleDC(NULL);

	LOGFONT logfont;
	logfont.lfCharSet = DEFAULT_CHARSET;
	logfont.lfPitchAndFamily = 0;
	strcpy(logfont.lfFaceName, windowsFontName);
	g_bFontFound = false;
	::EnumFontFamiliesExA(m_hDC, &logfont, &FontEnumProc, 0, 0);
	if (!g_bFontFound)
	{
		m_szName[0] = 0;
		return false;
	}

	m_hFont = ::CreateFontA(tall, 0, 0, 0,
		m_iWeight,
		flags & FONTFLAG_ITALIC,
		flags & FONTFLAG_UNDERLINE,
		flags & FONTFLAG_STRIKEOUT,
		charset,
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		m_bAntiAliased ? ANTIALIASED_QUALITY : NONANTIALIASED_QUALITY,
		DEFAULT_PITCH | FF_DONTCARE,
		m_szName);

	if (!m_hFont)
	{
		m_szName[0] = 0;
		return false;
	}

	::SelectObject(m_hDC, m_hFont);
	::SetTextAlign(m_hDC, TA_LEFT | TA_TOP | TA_UPDATECP);

	::TEXTMETRIC tm;
	memset(&tm, 0, sizeof(tm));

	if (!GetTextMetricsA(m_hDC, &tm))
	{
		m_szName[0] = 0;
		return false;
	}

	m_iHeight = tm.tmHeight;
	m_iMaxCharWidth = tm.tmMaxCharWidth;
	m_iAscent = tm.tmAscent;

	m_rgiBitmapSize[0] = tm.tmMaxCharWidth;
	m_rgiBitmapSize[1] = tm.tmHeight;

	::BITMAPINFOHEADER header;
	memset(&header, 0, sizeof(header));
	header.biSize = sizeof(header);
	header.biWidth = m_rgiBitmapSize[0];
	header.biHeight = -m_rgiBitmapSize[1];
	header.biPlanes = 1;
	header.biBitCount = 32;
	header.biCompression = BI_RGB;

	m_hDIB = ::CreateDIBSection(m_hDC, (BITMAPINFO*)&header, DIB_RGB_COLORS, (void**)(&m_pBuf), NULL, 0);
	::SelectObject(m_hDC, m_hDIB);

	memset(m_ABCWidthsCache, 0, sizeof(m_ABCWidthsCache));
	ABC abc[ABCWIDTHS_CACHE_SIZE];
	if (::GetCharABCWidthsW(m_hDC, 0, ABCWIDTHS_CACHE_SIZE - 1, &abc[0]) || ::GetCharABCWidthsA(m_hDC, 0, ABCWIDTHS_CACHE_SIZE - 1, &abc[0]))
	{
		for (int i = 0; i < ABCWIDTHS_CACHE_SIZE; i++)
		{
			m_ABCWidthsCache[i].a = abc[i].abcA;
			m_ABCWidthsCache[i].b = abc[i].abcB;
			m_ABCWidthsCache[i].c = abc[i].abcC;
		}
	}
	else
	{
		for (int i = 0; i < ABCWIDTHS_CACHE_SIZE; i++)
		{
			m_ABCWidthsCache[i].b = (char)tm.tmAveCharWidth;
		}
	}

	return true;
}

void CWin32Font::GetCharRGBA(int ch, int rgbaX, int rgbaY, int rgbaWide, int rgbaTall, unsigned char *rgba)
{
	int abcA, abcB, abcC;
	GetCharABCWidths(ch, abcA, abcB, abcC);

	::SelectObject(m_hDC, m_hFont);

	int bufferWide = m_rgiBitmapSize[0];
	int bufferTall = m_rgiBitmapSize[1];

	int wide = abcB;
	int tall = m_iHeight;
	GLYPHMETRICS glyphMetrics;
	MAT2 mat2 = { { 0, 1 }, { 0, 0 }, { 0, 0 }, { 0, 1 } };
	int bytesNeeded = 0;

	if (m_bAntiAliased)
	{
		::SelectObject(m_hDC, m_hFont);
		bytesNeeded = ::GetGlyphOutlineW(m_hDC, ch, GGO_GRAY8_BITMAP, &glyphMetrics, 0, NULL, &mat2);
	}

	if (bytesNeeded > 0)
	{
		unsigned char *lpbuf = (unsigned char *)_alloca(bytesNeeded);
		::GetGlyphOutlineW(m_hDC, ch, GGO_GRAY8_BITMAP, &glyphMetrics, bytesNeeded, lpbuf, &mat2);

		wide = glyphMetrics.gmBlackBoxX;
		while (wide % 4 != 0)
		{
			wide++;
		}

		int pushDown = m_iAscent - glyphMetrics.gmptGlyphOrigin.y;
		int xstart = 0;

		if ((int)glyphMetrics.gmBlackBoxX >= abcB + 2)
		{
			xstart = (glyphMetrics.gmBlackBoxX - abcB) / 2;
		}

		for (unsigned int j = 0; j < glyphMetrics.gmBlackBoxY; j++)
		{
			for (unsigned int i = xstart; i < glyphMetrics.gmBlackBoxX; i++)
			{
				int x = rgbaX + i - xstart;
				int y = rgbaY + j + pushDown;
				if ((x < rgbaWide) && (y < rgbaTall))
				{
					unsigned char grayscale = lpbuf[(j*wide + i)];

					float r, g, b, a;
					if (grayscale)
					{
						r = g = b = 1.0f;
						a = (grayscale + 0) / 64.0f;
						if (a > 1.0f) a = 1.0f;
					}
					else
					{
						r = g = b = a = 0.0f;
					}

					if (ch == '\t')
					{
						r = g = b = 0;
					}

					unsigned char *dst = &rgba[(y*rgbaWide + x) * 4];
					dst[0] = (unsigned char)(r * 255.0f);
					dst[1] = (unsigned char)(g * 255.0f);
					dst[2] = (unsigned char)(b * 255.0f);
					dst[3] = (unsigned char)(a * 255.0f);
				}
			}
		}
	}
	else
	{
		memset(m_pBuf, 0, bufferWide * bufferTall * 4);

		::SetBkColor(m_hDC, RGB(0, 0, 0));
		::SetTextColor(m_hDC, RGB(255, 255, 255));
		::SetBkMode(m_hDC, OPAQUE);
		::MoveToEx(m_hDC, -abcA, 0, NULL);

		wchar_t wch = (wchar_t)ch;
		::ExtTextOutW(m_hDC, 0, 0, 0, NULL, &wch, 1, NULL);
		::SetBkMode(m_hDC, TRANSPARENT);

		int srcX, srcY, dstX, dstY;

		dstY = m_fOutlined ? 1 : 0;

		for (srcY = 0; srcY < bufferTall; srcY++, dstY++)
		{
			dstX = m_fOutlined ? 1 : 0;

			for (srcX = 0; srcX < wide; srcX++, dstX++)
			{
				unsigned char *src = &m_pBuf[(srcY * bufferWide + srcX) * 4];
				unsigned char *dst = &rgba[(dstY * rgbaWide + dstX) * 4];

				dst[0] = src[0];
				dst[1] = src[1];
				dst[2] = src[2];

				dst[3] = (src[0] || src[1] || src[2]) ? 255 : 0;
			}
		}
	}

	if (m_fOutlined)
	{
		ApplyOutlineToTexture(rgbaX, rgbaY, rgbaWide, rgbaTall, wide + 2, tall + 2, rgba);
	}
}

void CWin32Font::ApplyOutlineToTexture(int rgbaX, int rgbaY, int rgbaWide, int rgbaTall, int charWide, int charTall, unsigned char *rgba)
{
	int x, y;
	int shadowX, shadowY;

	for (y = 0; y < charTall; y++)
	{
		for (x = 0; x < charWide; x++)
		{
			unsigned char *src = &rgba[(x + rgbaX + ((rgbaY + y) * rgbaWide)) * 4];

			if (src[3] == 0)
			{
				shadowX = shadowY = 0;

				for (shadowY = -1; shadowY <= 1; shadowY++)
				{
					if (shadowX == 0 && shadowY == 0)
						continue;

					int testX = shadowX + x;
					int testY = shadowY + y;

					if (testX < 0 || testX >= charWide || testY < 0 || testY >= charTall)
						continue;

					unsigned char *test = &rgba[(testX + rgbaX + ((rgbaY + testY) * rgbaWide)) * 4];

					if (test[0] != 0 && test[1] != 0 && test[2] != 0 && test[3] != 0)
					{
						src[0] = 0;
						src[1] = 0;
						src[2] = 0;
						src[3] = 255;
					}
				}

				shadowX = shadowY = 0;

				for (shadowX = -1; shadowX <= 1; shadowX++)
				{
					if (shadowX == 0 && shadowY == 0)
						continue;

					int testX = shadowX + x;
					int testY = shadowY + y;

					if (testX < 0 || testX >= charWide || testY < 0 || testY >= charTall)
						continue;

					unsigned char *test = &rgba[(testX + rgbaX + ((rgbaY + testY) * rgbaWide)) * 4];

					if (test[0] != 0 && test[1] != 0 && test[2] != 0 && test[3] != 0)
					{
						src[0] = 0;
						src[1] = 0;
						src[2] = 0;
						src[3] = 255;
					}
				}
			}
		}
	}
}

bool CWin32Font::IsEqualTo(const char *windowsFontName, int tall, int weight, int blur, int scanlines, int flags)
{
	if (!stricmp(windowsFontName, m_szName)
		&& m_iTall == tall
		&& m_iWeight == weight
		&& m_iFlags == flags)
		return true;

	return false;
}

bool CWin32Font::IsValid()
{
	if (m_szName[0])
		return true;

	return false;
}

void CWin32Font::SetAsActiveFont(HDC hdc)
{
	::SelectObject(hdc, m_hFont);
}

void CWin32Font::GetCharABCWidths(int ch, int &a, int &b, int &c)
{
	if (ch < ABCWIDTHS_CACHE_SIZE)
	{
		a = m_ABCWidthsCache[ch].a;
		b = m_ABCWidthsCache[ch].b;
		c = m_ABCWidthsCache[ch].c;
	}
	else
	{
		ABC abc;
		if (::GetCharABCWidthsW(m_hDC, ch, ch, &abc))
		{
			a = abc.abcA;
			b = abc.abcB;
			c = abc.abcC;
		}
		else
		{
			a = c = 0;
			b = m_iMaxCharWidth;
		}
	}
}

int CWin32Font::GetHeight()
{
	return m_iHeight;
}

int CWin32Font::GetMaxCharWidth()
{
	return m_iMaxCharWidth;
}

int CWin32Font::GetFlags()
{
	return m_iFlags;
}

int CWin32Font::GetAscent(void)
{
	return m_iAscent;
}