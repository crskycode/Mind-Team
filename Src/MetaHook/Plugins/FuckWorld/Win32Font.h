#ifndef WIN32FONT_H
#define WIN32FONT_H

#define WIN32_LEAN_AND_MEAN
#define OEMRESOURCE
#include <windows.h>

#ifdef GetCharABCWidths
#undef GetCharABCWidths
#endif

class CWin32Font
{
public:
	CWin32Font();
	~CWin32Font();

	bool Create(const char *windowsFontName, int tall, int weight, int blur, int scanlines, int flags);
	void GetCharRGBA(int ch, int rgbaX, int rgbaY, int rgbaWide, int rgbaTall, unsigned char *rgba);
	bool IsEqualTo(const char *windowsFontName, int tall, int weight, int blur, int scanlines, int flags);
	bool IsValid();
	void GetCharABCWidths(int ch, int &a, int &b, int &c);
	void SetAsActiveFont(HDC hdc);
	int GetHeight();
	int GetMaxCharWidth();
	int GetFlags();
	const char *GetName() { return m_szName; }
	int GetAscent(void);

private:
	void ApplyOutlineToTexture(int rgbaX, int rgbaY, int rgbaWide, int rgbaTall, int charWide, int charTall, unsigned char *rgba);

	char m_szName[32];
	int m_iTall;
	int m_iWeight;
	int m_iFlags;
	bool m_bAntiAliased;
	int m_fOutlined;

	int m_iHeight;
	int m_iMaxCharWidth;
	int m_iAscent;

	struct abc_t
	{
		char a;
		char b;
		char c;
		char pad;
	};
	enum { ABCWIDTHS_CACHE_SIZE = 256 };
	abc_t m_ABCWidthsCache[ABCWIDTHS_CACHE_SIZE];

	HFONT m_hFont;
	HDC m_hDC;
	HBITMAP m_hDIB;
	int m_rgiBitmapSize[2];
	unsigned char *m_pBuf;
};


#endif