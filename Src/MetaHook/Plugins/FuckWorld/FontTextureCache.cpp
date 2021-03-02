#include "FontTextureCache.h"
#include "Win32Font.h"
#include "FontManager.h"
#include "qgl.h"
#include "surface.h"

static CFontTextureCache s_FontTextureCache;

CFontTextureCache &FontTextureCache(void)
{
	return s_FontTextureCache;
}

template <class T>
inline T clamp(T const &val, T const &minVal, T const &maxVal)
{
	if (val < minVal)
		return minVal;
	else if (val > maxVal)
		return maxVal;
	else
		return val;
}

int CFontTextureCache::s_pFontPageSize[FONT_PAGE_SIZE_COUNT] =
{
	16,
	32,
	64,
	128,
};

CFontTextureCache::CFontTextureCache() : m_CharCache(0, 256, CacheEntryLessFunc)
{
	CacheEntry_t listHead = { 0, 0 };
	m_LRUListHeadIndex = m_CharCache.Insert(listHead);

	m_CharCache[m_LRUListHeadIndex].nextEntry = m_LRUListHeadIndex;
	m_CharCache[m_LRUListHeadIndex].prevEntry = m_LRUListHeadIndex;

	for (int i = 0; i < FONT_PAGE_SIZE_COUNT; ++i)
	{
		m_pCurrPage[i] = -1;
	}
}

CFontTextureCache::~CFontTextureCache()
{

}

bool CFontTextureCache::CacheEntryLessFunc(CacheEntry_t const &lhs, CacheEntry_t const &rhs)
{
	if (lhs.font < rhs.font)
		return true;
	else if (lhs.font > rhs.font)
		return false;

	return (lhs.wch < rhs.wch);
}

bool CFontTextureCache::GetTextureForChar(int font, int type, wchar_t wch, int *textureID, float **texCoords)
{
	static CacheEntry_t cacheitem;

	if (type == FONT_DRAW_DEFAULT)
	{
		type = FONT_DRAW_NONADDITIVE;
	}

	int typePage = (int)type - 1;
	typePage = clamp(typePage, 0, (int)FONT_DRAW_TYPE_COUNT - 1);


	cacheitem.font = font;
	cacheitem.wch = wch;

	*texCoords = cacheitem.texCoords;

	HCacheEntry cacheHandle = m_CharCache.Find(cacheitem);
	if (m_CharCache.IsValidIndex(cacheHandle))
	{
		int page = m_CharCache[cacheHandle].page;
		*textureID = m_PageList[page].textureID[typePage];
		*texCoords = m_CharCache[cacheHandle].texCoords;
		return true;
	}

	CWin32Font *winFont = FontManager().GetFontForChar(font, wch);
	if (!winFont)
		return false;

	int fontTall = winFont->GetHeight();
	int a, b, c;
	winFont->GetCharABCWidths(wch, a, b, c);
	int fontWide = b;

	if (winFont->GetFlags() & FONTFLAG_OUTLINE)
	{
		// top bottom left right (1px)
		fontWide += 2;
		fontTall += 2;
	}

	int page, drawX, drawY, twide, ttall;
	if (!AllocatePageForChar(fontWide, fontTall, page, drawX, drawY, twide, ttall))
		return false;

	int nByteCount = s_pFontPageSize[FONT_PAGE_SIZE_COUNT - 1] * s_pFontPageSize[FONT_PAGE_SIZE_COUNT - 1] * 4;
	unsigned char* rgba = (unsigned char*)_alloca(nByteCount);
	memset(rgba, 0, nByteCount);
	winFont->GetCharRGBA(wch, 0, 0, fontWide, fontTall, rgba);

	qglBindTexture(GL_TEXTURE_2D, m_PageList[page].textureID[typePage]);
	qglTexSubImage2D(GL_TEXTURE_2D, 0, drawX, drawY, fontWide, fontTall, GL_RGBA, GL_UNSIGNED_BYTE, rgba);

	cacheitem.page = page;

	double adjust = 0.0f;

	cacheitem.texCoords[0] = (float)((double)drawX / ((double)twide + adjust));
	cacheitem.texCoords[1] = (float)((double)drawY / ((double)ttall + adjust));
	cacheitem.texCoords[2] = (float)((double)(drawX + fontWide) / (double)twide);
	cacheitem.texCoords[3] = (float)((double)(drawY + fontTall) / (double)ttall);
	m_CharCache.Insert(cacheitem);

	*textureID = m_PageList[page].textureID[typePage];
	return true;
}

int CFontTextureCache::ComputePageType(int charTall) const
{
	for (int i = 0; i < FONT_PAGE_SIZE_COUNT; ++i)
	{
		if (charTall < s_pFontPageSize[i])
			return i;
	}

	return -1;
}

bool CFontTextureCache::AllocatePageForChar(int charWide, int charTall, int &pageIndex, int &drawX, int &drawY, int &twide, int &ttall)
{
	int nPageType = ComputePageType(charTall);
	pageIndex = m_pCurrPage[nPageType];

	int nNextX = 0;
	bool bNeedsNewPage = true;
	if (pageIndex > -1)
	{
		Page_t &page = m_PageList[pageIndex];

		nNextX = page.nextX + charWide;

		if (nNextX > page.wide)
		{
			page.nextX = 0;
			nNextX = charWide;
			page.nextY += page.fontHeight + 1;
		}

		bNeedsNewPage = ((page.nextY + page.fontHeight + 1) > page.tall);
	}

	if (bNeedsNewPage)
	{
		pageIndex = m_PageList.AddToTail();
		Page_t &newPage = m_PageList[pageIndex];
		m_pCurrPage[nPageType] = pageIndex;

		for (int i = 0; i < FONT_DRAW_TYPE_COUNT; ++i)
		{
			newPage.textureID[i] = g_pSurface->CreateNewTextureID();
		}

		newPage.fontHeight = s_pFontPageSize[nPageType];
		newPage.wide = 256;
		newPage.tall = 256;
		newPage.nextX = 0;
		newPage.nextY = 0;

		nNextX = charWide;

		unsigned char rgba[256 * 256 * 4];
		memset(rgba, 0, sizeof(rgba));
		int typePageNonAdditive = (int)(FONT_DRAW_NONADDITIVE) - 1;

		qglBindTexture(GL_TEXTURE_2D, newPage.textureID[typePageNonAdditive]);
		qglTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, newPage.wide, newPage.tall, 0, GL_RGBA, GL_UNSIGNED_BYTE, rgba);
		qglTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		qglTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}

	Page_t &page = m_PageList[pageIndex];
	drawX = page.nextX;
	drawY = page.nextY;
	twide = page.wide;
	ttall = page.tall;

	page.nextX = nNextX/* + 1*/;

	return true;
}