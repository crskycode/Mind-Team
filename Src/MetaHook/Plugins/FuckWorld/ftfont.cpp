
#include <metahook.h>
#include "plugins.h"

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_BITMAP_H
#include FT_OUTLINE_H

#include <map>
#include <vector>

#include "qgl.h"
#include "surface.h"

#define PAGE_SIZE	512


class CFTFontManager
{
public:
	CFTFontManager();
	~CFTFontManager();

	void initialize(void);
	void release(void);
	int createFont(const char *filename, int face, int tall, bool bold, bool italic, bool antialias);
	bool getCharInfo(int font, int code, int *wide, int *tall, int *horiBearingX, int *horiBearingY, int *horiAdvance, GLuint *texture, GLdouble coords[]);
	int getFontTall(int font);

private:
	struct glyphMetrics
	{
		int width;
		int height;
		int horiBearingX;
		int horiBearingY;
		int horiAdvance;
	};

	class CFont
	{
	public:
		CFont();
		~CFont();

		bool create(FT_Library library, const char *filename, FT_Long face, int tall, int bold, int italic, bool antialias);
		void release(void);
		bool getCharInfo(int code, glyphMetrics *metrics, GLuint *texture, GLdouble coords[]);
		int getFontTall(void);

	private:
		bool loadChar(int code, glyphMetrics *metrics);

		class CChar
		{
		public:
			CChar();

			void setInfo(glyphMetrics *metrics);
			void getInfo(glyphMetrics *metrics, GLuint *texture, GLdouble coords[]);

		public:
			int m_code;
			GLuint m_texture;
			GLdouble m_coords[4];

		private:
			glyphMetrics m_metrics;
		};

		class CPage
		{
		public:
			CPage();
			~CPage();

			bool appendChar(int wide, int tall, GLubyte *rgba, GLdouble coords[]);
			GLuint getTexture(void);

		private:
			GLuint m_texture;
			int m_wide;
			int m_tall;
			int m_posx;
			int m_posy;
			int m_maxCharTall;
		};

		typedef std::map<int, CChar *> TCharMap;
		typedef std::vector<CPage *> TPageVec;

		FT_Byte *m_fontFile;
		FT_Library m_library;
		FT_Face m_face;
		int m_tall;
		bool m_bold;
		bool m_italic;
		bool m_antialias;
		int m_rgbaSize;
		GLubyte *m_rgba;
		TCharMap m_chars;
		TPageVec m_pages;
	};

	typedef std::vector<CFont *> TFontVec;

	FT_Library m_library;
	TFontVec m_fonts;
};


CFTFontManager::CFont::CChar::CChar()
{
	m_code = 0;
	m_texture = 0;
	memset(m_coords, 0, sizeof(m_coords));
	memset(&m_metrics, 0, sizeof(glyphMetrics));
}

void CFTFontManager::CFont::CChar::setInfo(glyphMetrics *metrics)
{
	memcpy(&m_metrics, metrics, sizeof(glyphMetrics));
}

void CFTFontManager::CFont::CChar::getInfo(glyphMetrics *metrics, GLuint *texture, GLdouble coords[])
{
	memcpy(metrics, &m_metrics, sizeof(glyphMetrics));
	*texture = m_texture;
	memcpy(coords, m_coords, sizeof(m_coords));
}

CFTFontManager::CFont::CPage::CPage()
{
	m_wide = PAGE_SIZE;
	m_tall = PAGE_SIZE;
	m_posx = 0;
	m_posy = 0;

	m_maxCharTall = 0;

	m_texture = g_pSurface->CreateNewTextureID();
	qglBindTexture(GL_TEXTURE_2D, m_texture);
	qglTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_wide, m_tall, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	qglTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	qglTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

CFTFontManager::CFont::CPage::~CPage()
{
	qglDeleteTextures(1, &m_texture);
	m_texture = 0;
}

bool CFTFontManager::CFont::CPage::appendChar(int wide, int tall, GLubyte *rgba, GLdouble coords[])
{
	if (m_posy + tall > m_tall)
	{
		return false;
	}

	if (m_posx + wide > m_wide)
	{
		int newLineY = m_posy + m_maxCharTall;

		if (newLineY + tall > m_tall)
		{
			return false;
		}

		m_posx = 0;
		m_posy = newLineY;
		m_maxCharTall = 0;
	}

	if (tall > m_maxCharTall)
	{
		m_maxCharTall = tall;
	}

	qglBindTexture(GL_TEXTURE_2D, m_texture);
	qglTexSubImage2D(GL_TEXTURE_2D, 0, m_posx, m_posy, wide, tall, GL_RGBA, GL_UNSIGNED_BYTE, rgba);

	coords[0] = m_posx / (GLdouble)m_wide;
	coords[1] = m_posy / (GLdouble)m_tall;
	coords[2] = (m_posx + wide) / (GLdouble)m_wide;
	coords[3] = (m_posy + tall) / (GLdouble)m_tall;

	m_posx += wide;

	return true;
}

GLuint CFTFontManager::CFont::CPage::getTexture(void)
{
	return m_texture;
}

CFTFontManager::CFont::CFont()
{
	m_fontFile = NULL;
	m_library = NULL;
	m_face = NULL;
	m_tall = 0;
	m_bold = false;
	m_italic = false;
	m_antialias = false;
	m_rgbaSize = 0;
	m_rgba = NULL;
}

CFTFontManager::CFont::~CFont()
{
	release();
}

bool CFTFontManager::CFont::create(FT_Library library, const char *filename, FT_Long face, int tall, int bold, int italic, bool antialias)
{
	FT_Error err;

	if (tall > PAGE_SIZE)
	{
//		gEngfuncs.Con_Printf("CFTFontManager::CFont::create() : font size bigger than the page\n");
		return false;
	}

	FileHandle_t hFile = g_pFileSystem->Open(filename, "rb");

	if (hFile == FILESYSTEM_INVALID_HANDLE)
	{
//		gEngfuncs.Con_Printf("CFTFontManager::CFont::create() : failed to open file\n");
		return false;
	}

	FT_UInt fileSize = g_pFileSystem->Size(hFile);

	if (fileSize == 0)
	{
		g_pFileSystem->Close(hFile);
		return false;
	}

	m_fontFile = new FT_Byte[fileSize];

	if (!m_fontFile)
	{
		g_pFileSystem->Close(hFile);
		return false;
	}

	if (g_pFileSystem->Read(m_fontFile, fileSize, hFile) != fileSize)
	{
		delete[] m_fontFile;
		g_pFileSystem->Close(hFile);
		return false;
	}

	g_pFileSystem->Close(hFile);

	if ((err = FT_New_Memory_Face(library, m_fontFile, fileSize, face, &m_face)) != FT_Err_Ok)
	{
//		gEngfuncs.Con_Printf("FT_New_Face() : Error %d\n", err);
		return false;
	}

	if ((err = FT_Set_Pixel_Sizes(m_face, 0, tall)) != FT_Err_Ok)
	{
//		gEngfuncs.Con_Printf("FT_Set_Pixel_Sizes() : Error %d\n", err);
		return false;
	}

	m_rgbaSize = (tall * 2) * tall * 4;

	m_rgba = new GLubyte[m_rgbaSize];

	if (!m_rgba)
	{
//		gEngfuncs.Con_Printf("CFTFontManager::CFont::create() : failed to allocate RGBA buffer\n");
		return false;
	}

	if (italic)
	{
		FT_Matrix m;
		m.xx = 0x10000l;
		m.xy = 0.5f * 0x10000l;
		m.yx = 0l;
		m.yy = 0x10000l;
		FT_Set_Transform(m_face, &m, NULL);
	}

	m_library = library;
	m_tall = tall;
	m_bold = bold;
	m_italic = italic;
	m_antialias = antialias;

	return true;
}

void CFTFontManager::CFont::release(void)
{
	FT_Error err;

	if (m_face)
	{
		if ((err = FT_Done_Face(m_face)) != FT_Err_Ok)
		{
//			gEngfuncs.Con_Printf("FT_Done_Face() : Error %d\n", err);
		}
		m_face = NULL;
	}

	if (m_fontFile)
	{
		delete[] m_fontFile;
		m_fontFile = NULL;
	}

	if (m_rgba)
	{
		delete[] m_rgba;
		m_rgba = NULL;
	}

	for (TCharMap::iterator it = m_chars.begin(); it != m_chars.end(); it++)
	{
		delete it->second;
		it->second = NULL;
	}

	m_chars.clear();

	for (int i = 0; i < m_pages.size(); i++)
	{
		delete m_pages[i];
		m_pages[i] = NULL;
	}

	m_pages.clear();
}

bool CFTFontManager::CFont::getCharInfo(int code, glyphMetrics *metrics, GLuint *texture, GLdouble coords[])
{
	TCharMap::iterator it = m_chars.find(code);

	if (it != m_chars.end())
	{
		it->second->getInfo(metrics, texture, coords);
		return true;
	}

	glyphMetrics gm;

	if (!loadChar(code, &gm))
	{
//		gEngfuncs.Con_Printf("CFTFontManager::CFont::getCharInfo() : load unicode character %d failed\n", code);
		return false;
	}

	CChar *ch = new CChar();
	ch->m_code = code;
	ch->setInfo(&gm);

	for (size_t i = 0; i < m_pages.size(); i++)
	{
		CPage *page = m_pages[i];

		if (page->appendChar(gm.width, gm.height, m_rgba, ch->m_coords))
		{
			ch->m_texture = page->getTexture();
			ch->getInfo(metrics, texture, coords);
			m_chars.insert(TCharMap::value_type(code, ch));
			return true;
		}
	}

	CPage *page = new CPage();

	if (page->appendChar(gm.width, gm.height, m_rgba, ch->m_coords))
	{
		ch->m_texture = page->getTexture();
		ch->getInfo(metrics, texture, coords);
		m_chars.insert(TCharMap::value_type(code, ch));
		m_pages.push_back(page);
		return true;
	}

	delete ch;
	delete page;

//	gEngfuncs.Con_Printf("CFTFontManager::CFont::getCharInfo() : cache character %d failed\n", code);
	return false;
}

int CFTFontManager::CFont::getFontTall(void)
{
	return m_tall;
}

static void ConvertMONOToRGBA(FT_Bitmap *source, GLubyte *rgba)
{
	GLubyte *s = source->buffer;
	GLubyte *t = rgba;

	for (GLuint y = source->rows; y > 0; y--)
	{
		GLubyte *ss = s;
		GLubyte *tt = t;

		for (GLuint x = source->width >> 3; x > 0; x--)
		{
			GLuint val = *ss;

			for (GLuint i = 8; i > 0; i--)
			{
				tt[0] = tt[1] = tt[2] = tt[3] = ( val & (1<<(i-1)) ) ? 0xFF : 0x00;
				tt += 4;
			}

			ss += 1;
		}

		GLuint rem = source->width & 7;

		if (rem > 0)
		{
			GLuint val = *ss;

			for (GLuint x = rem; x > 0; x--)
			{
				tt[0] = tt[1] = tt[2] = tt[3] = ( val & 0x80 ) ? 0xFF : 0x00;
				tt += 4;
				val <<= 1;
			}
		}

		s += source->pitch;
		t += source->width * 4;	//pitch
	}
}

static void ConvertGRAYToRGBA(FT_Bitmap *source, GLubyte *rgba)
{
	for (GLuint y = 0; y < source->rows; y++)
	{
		for (GLuint x = 0; x < source->width; x++)
		{
			GLubyte *s = &source->buffer[(y * source->pitch) + x];
			GLubyte *t = &rgba[((y * source->pitch) + x) * 4];
			t[0] = t[1] = t[2] = 0xFF;
			t[3] = *s;
		}
	}
}

bool CFTFontManager::CFont::loadChar(int code, glyphMetrics *metrics)
{
	FT_Error err;

	FT_UInt glyph_index = FT_Get_Char_Index(m_face, (FT_ULong)code);

	if (glyph_index > 0)
	{
		if ((err = FT_Load_Glyph(m_face, glyph_index, FT_LOAD_DEFAULT)) == FT_Err_Ok)
		{
			FT_GlyphSlot glyph = m_face->glyph;

			FT_Render_Mode render_mode = m_antialias ? FT_RENDER_MODE_NORMAL : FT_RENDER_MODE_MONO;

			if (m_antialias && m_bold)
			{
				if ((err = FT_Outline_EmboldenXY(&glyph->outline, 60, 60)) != FT_Err_Ok)
				{
//					gEngfuncs.Con_Printf("FT_Outline_EmboldenXY() : Error %d\n", err);
				}
			}

			if ((err = FT_Render_Glyph(glyph, render_mode)) == FT_Err_Ok)
			{
				FT_Bitmap *bitmap = &glyph->bitmap;

				switch (bitmap->pixel_mode)
				{
				case FT_PIXEL_MODE_MONO:
					{
						if (!m_antialias && m_bold)
						{
							if ((err = FT_Bitmap_Embolden(m_library, bitmap, 60, 0)) != FT_Err_Ok)
							{
							}
						}
						ConvertMONOToRGBA(bitmap, m_rgba);
						break;
					}
				case FT_PIXEL_MODE_GRAY:
					{
						ConvertGRAYToRGBA(bitmap, m_rgba);
						break;
					}
				default:
					{
						memset(m_rgba, 0xFF, m_rgbaSize);
						break;
					}
				}

				metrics->width = bitmap->width;
				metrics->height = bitmap->rows;
				metrics->horiBearingX = glyph->bitmap_left;
				metrics->horiBearingY = glyph->bitmap_top;
				metrics->horiAdvance = glyph->advance.x >> 6;

				return true;
			}
			else
			{
//				gEngfuncs.Con_Printf("FT_Render_Glyph() : Error %d\n", err);
			}
		}
		else
		{
//			gEngfuncs.Con_Printf("FT_Load_Glyph() : Error %d\n", err);
		}
	}
	else
	{
//		gEngfuncs.Con_Printf("CFTFontManager::CFont::loadChar() : unicode character %d glyph not found in font file\n", code);
	}

	memset(metrics, 0, sizeof(glyphMetrics));

	return false;
}

CFTFontManager::CFTFontManager()
{
	m_library = NULL;
}

CFTFontManager::~CFTFontManager()
{
	release();
}

void CFTFontManager::initialize(void)
{
	FT_Error err;

	if ((err = FT_Init_FreeType(&m_library)) != FT_Err_Ok)
	{
		Sys_Error("FT_Init_FreeType() : Error %d", err);
	}
}

void CFTFontManager::release(void)
{
	FT_Error err;

	for (size_t i = 0; i < m_fonts.size(); i++)
	{
		delete m_fonts[i];
		m_fonts[i] = NULL;
	}

	m_fonts.clear();

	if (m_library)
	{
		if ((err = FT_Done_FreeType(m_library)) != FT_Err_Ok)
		{
			Sys_Error("FT_Done_FreeType() : Error %d", err);
		}
		m_library = NULL;
	}
}

int CFTFontManager::createFont(const char *filename, int face, int tall, bool bold, bool italic, bool antialias)
{
	CFont *font = new CFont();

	if (!font->create(m_library, filename, face, tall, bold, italic, antialias))
	{
		delete font;
		return 0;
	}

	m_fonts.push_back(font);

	return m_fonts.size();
}

#define CONVERT_FONT_INDEX(x) ( ((x) < 1 || (x) > (int)m_fonts.size()) ? -1 : (x) - 1 )

bool CFTFontManager::getCharInfo(int font, int code, int *wide, int *tall, int *horiBearingX, int *horiBearingY, int *horiAdvance, GLuint *texture, GLdouble coords[])
{
	int i = CONVERT_FONT_INDEX(font);

	if (i == -1)
	{
//		gEngfuncs.Con_Printf("CFTFontManager::getCharInfo() : invalid font index %d\n", i);
		return false;
	}

	CFont *cf = m_fonts[i];

	glyphMetrics gm;

	if (!cf->getCharInfo(code, &gm, texture, coords))
	{
		return false;
	}

	*wide = gm.width;
	*tall = gm.height;
	*horiBearingX = gm.horiBearingX;
	*horiBearingY = gm.horiBearingY;
	*horiAdvance = gm.horiAdvance;

	return true;
}

int CFTFontManager::getFontTall(int font)
{
	int i = CONVERT_FONT_INDEX(font);

	if (i == -1)
	{
//		gEngfuncs.Con_Printf("CFTFontManager::getCharInfo() : invalid font index %d\n", i);
		return 0;
	}

	CFont *cf = m_fonts[i];

	return cf->getFontTall();
}


// instance
static CFTFontManager g_TFontManager;


void FTF_Init(void)
{
	g_TFontManager.initialize();
}

void FTF_Shutdown(void)
{
	g_TFontManager.release();
}

int FTF_CreateFont(const char *filename, int tall, bool bold, bool italic, bool antialias)
{
	return g_TFontManager.createFont(filename, 0, tall, bold, italic, antialias);
}

bool FTF_GetCharInfo(int font, int code, int *wide, int *tall, int *horiBearingX, int *horiBearingY, int *horiAdvance, GLuint *texture, GLdouble coords[])
{
	return g_TFontManager.getCharInfo(font, code, wide, tall, horiBearingX, horiBearingY, horiAdvance, texture, coords);
}

int FTF_GetFontTall(int font)
{
	return g_TFontManager.getFontTall(font);
}

void FTF_DrawString(int x, int y, int font, wchar_t *string)
{
	if (!font)
		return;

	int tall = g_TFontManager.getFontTall(font);

	int dx = x;
	int dy = y + tall;

	GLuint sglt = 0;

	while (*string)
	{
		if (*string == L'\n')
		{
			string++;
			dx = x;
			dy += tall + 1;	//row spacing
			continue;
		}

		int cw, ct, bx, by, av;
		GLuint glt;
		GLdouble crd[4];

		if (!g_TFontManager.getCharInfo(font, *string, &cw, &ct, &bx, &by, &av, &glt, crd))
		{
			string++;
			continue;
		}

		if (glt != sglt)
		{
			qglBindTexture(GL_TEXTURE_2D, glt);
			sglt = glt;
		}

		int px = dx + bx;
		int py = dy - by;

		qglBegin(GL_QUADS);
			qglTexCoord2d(crd[0], crd[1]);
			qglVertex3f(px, py, 0.0f);
			qglTexCoord2d(crd[2], crd[1]);
			qglVertex3f(px + cw, py, 0.0f);
			qglTexCoord2d(crd[2], crd[3]);
			qglVertex3f(px + cw, py + ct, 0.0f);
			qglTexCoord2d(crd[0], crd[3]);
			qglVertex3f(px, py + ct, 0.0f);
		qglEnd();

		dx += av;

		string++;
	}
}

void FTF_GetStringSize(int font, wchar_t *string, int *wide, int *tall)
{
	if (!font)
		return;

	int ft = g_TFontManager.getFontTall(font);

	int dx = 0;
	int dy = 0;
	int ow = 0;
	int ot = 0;

	while (*string)
	{
		if (*string == L'\n')
		{
			string++;
			dx = 0;
			dy += ft + 1;	//row spacing
			continue;
		}

		int cw, ct, bx, by, av;
		GLuint glt;
		GLdouble crd[4];

		if (!g_TFontManager.getCharInfo(font, *string, &cw, &ct, &bx, &by, &av, &glt, crd))
		{
			string++;
			continue;
		}

		dx += av;

		if (dx > ow)
		{
			ow = dx;
		}

		if (dy + ft > ot)
		{
			ot = dy + ft;
		}

		string++;
	}

	*wide = ow;
	*tall = ot;
}