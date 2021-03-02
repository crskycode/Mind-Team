#include <metahook.h>
#include "FontManager.h"
#include "FontTextureCache.h"
#include "DrawFonts.h"
#include "qgl.h"
#include "Shader.h"
#include "surface.h"

int CreateFont(void)
{
	return FontManager().CreateFont();
}

bool AddGlyphSetToFont(int font, const char *windowsFontName, int tall, int weight, int blur, int scanlines, int flags)
{
	return FontManager().AddGlyphSetToFont(font, windowsFontName, tall, weight, blur, scanlines, flags, 0x0, 0xFFFF);
}

bool AddCustomFontFile(const char *fontFileName)
{
	return g_pSurface->AddCustomFontFile(fontFileName);
}

int GetFontTall(int font)
{
	return FontManager().GetFontTall(font);
}

void GetCharABCwide(int font, int ch, int &a, int &b, int &c)
{
	FontManager().GetCharABCwide(font, ch, a, b, c);
}

int GetCharacterWidth(int font, int ch)
{
	return FontManager().GetCharacterWidth(font, ch);
}

void GetTextSize(int font, const wchar_t *text, int &wide, int &tall)
{
	FontManager().GetTextSize(font, text, wide, tall);
}

int GetFontAscent(int font, wchar_t wch)
{
	return FontManager().GetFontAscent(font, wch);
}

static int g_hCurrentFont = 0;
static int g_iCurrentTexture = -1;
static int g_drawTextColor[4] = { 255, 255, 255, 255 };
static int g_drawTextPos[2] = { 0, 0 };

void DrawSetTextFont(int font)
{
	g_hCurrentFont = font;
}

void DrawSetTextColor(int r, int g, int b, int a)
{
	g_drawTextColor[0] = r;
	g_drawTextColor[1] = g;
	g_drawTextColor[2] = b;
	g_drawTextColor[3] = a;
}

void DrawSetTextPos(int x, int y)
{
	g_drawTextPos[0] = x;
	g_drawTextPos[1] = y;
}

void DrawGetTextPos(int &x,int &y)
{
	x = g_drawTextPos[0];
	y = g_drawTextPos[1];
}

void DrawPrintChar(int x, int y, int wide, int tall, float s0, float t0, float s1, float t1)
{
	qglBegin(GL_QUADS);
		qglTexCoord2f(s0, t0);
		qglVertex3f(x, y, 0);
		qglTexCoord2f(s0, t1);
		qglVertex3f(x, y + tall, 0);
		qglTexCoord2f(s1, t1);
		qglVertex3f(x + wide, y + tall, 0);
		qglTexCoord2f(s1, t0);
		qglVertex3f(x + wide, y, 0);
	qglEnd();
}

void DrawUnicodeChar(wchar_t wch, int abcA, int abcB, int abcC)
{
	if (!g_hCurrentFont)
		return;

	int x, y;
	DrawGetTextPos(x, y);

	int tall = GetFontTall(g_hCurrentFont);

	//int abcA, abcB, abcC;
	//GetCharABCwide(g_hCurrentFont, wch, abcA, abcB, abcC);

	x += abcA;
	int wide = abcB;

	if (FontManager().GetFontFlags(g_hCurrentFont) & FONTFLAG_OUTLINE)
	{
		wide += 2;
		tall += 2;
	}

	int newTexID = 0;
	float *texCoords;
	if (!FontTextureCache().GetTextureForChar(g_hCurrentFont, FONT_DRAW_DEFAULT, wch, &newTexID, &texCoords))
	{
		return;
	}

	if (g_iCurrentTexture != newTexID)
	{
		qglBindTexture(GL_TEXTURE_2D, newTexID);
		g_iCurrentTexture = newTexID;
	}

	qglEnable(GL_TEXTURE_2D);

	qglEnable(GL_BLEND);
	qglBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	qglColor4ub(g_drawTextColor[0], g_drawTextColor[1], g_drawTextColor[2], g_drawTextColor[3]);

	DrawPrintChar(x, y, wide, tall, texCoords[0], texCoords[1], texCoords[2], texCoords[3]);

	x += abcB + abcC;

	DrawSetTextPos(x, y);
}

static ABC g_abcWidthCache[0xFFFF];

void DrawPrintText(const wchar_t *text)
{
	if (!text || !*text)
		return;

	wchar_t *ptext;

	g_iCurrentTexture = -1;

	int x, y;
	DrawGetTextPos(x, y);

	int tall = GetFontTall(g_hCurrentFont);

	ptext = (wchar_t *)text;

	while (*ptext)
	{
		ABC &abc = g_abcWidthCache[*ptext];

		if (*ptext == '\n')
			continue;

		if (*ptext == '\t')
			continue;

		GetCharABCwide(g_hCurrentFont, *ptext++, (int &)abc.abcA, (int &)abc.abcB, (int &)abc.abcC);
	}

	ptext = (wchar_t *)text;

	while (*ptext)
	{
		ABC &abc = g_abcWidthCache[*ptext];

		if (*ptext == '\n')
		{
			y += tall;
			DrawSetTextPos(x, y);
			continue;
		}

		if (*ptext == '\t')
			continue;

		DrawUnicodeChar(*ptext++, abc.abcA, abc.abcB, abc.abcC);
	}
}

void DrawCenterText(const wchar_t *text)
{
	if (!text || !*text)
		return;

	wchar_t *ptext;

	g_iCurrentTexture = -1;

	int x, y;
	DrawGetTextPos(x, y);

	int tall = GetFontTall(g_hCurrentFont);

	int  textWidth = 0;

	ptext = (wchar_t *)text;

	while (*ptext)
	{
		ABC &abc = g_abcWidthCache[*ptext];

		if (*ptext == '\n')
			continue;

		if (*ptext == '\t')
			continue;

		GetCharABCwide(g_hCurrentFont, *ptext++, (int &)abc.abcA, (int &)abc.abcB, (int &)abc.abcC);

		textWidth += abc.abcA + abc.abcB + abc.abcC;
	}

	x -= textWidth / 2;
	DrawSetTextPos(x, y);

	ptext = (wchar_t *)text;

	while (*ptext)
	{
		ABC &abc = g_abcWidthCache[*ptext];

		if (*ptext == '\n')
		{
			y += tall;
			DrawSetTextPos(x, y);
			continue;
		}

		if (*ptext == '\t')
			continue;

		DrawUnicodeChar(*ptext++, abc.abcA, abc.abcB, abc.abcC);
	}
}

font_api_t gFontAPI = 
{
	CreateFont,
	AddGlyphSetToFont,
	AddCustomFontFile,
	GetFontTall,
	GetCharABCwide,
	GetCharacterWidth,
	GetTextSize,
	GetFontAscent,
	DrawSetTextFont,
	DrawSetTextColor,
	DrawSetTextPos,
	DrawGetTextPos,
	DrawPrintText,
	DrawCenterText,
};