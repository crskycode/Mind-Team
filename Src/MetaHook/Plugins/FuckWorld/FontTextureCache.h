#ifndef FONTTEXTURECACHE_H
#define FONTTEXTURECACHE_H

#include "FontManager.h"
#include "UtlRBTree.h"

enum FontDrawType
{
	FONT_DRAW_DEFAULT = 0,
	FONT_DRAW_NONADDITIVE,
	FONT_DRAW_ADDITIVE,
	FONT_DRAW_TYPE_COUNT = 2,
};

class CFontTextureCache
{
public:
	CFontTextureCache();
	~CFontTextureCache();

	bool GetTextureForChar(int font, int type, wchar_t wch, int *textureID, float **texCoords);

private:
	enum
	{
		FONT_PAGE_SIZE_16,
		FONT_PAGE_SIZE_32,
		FONT_PAGE_SIZE_64,
		FONT_PAGE_SIZE_128,

		FONT_PAGE_SIZE_COUNT,
	};

	typedef unsigned short HCacheEntry;
	struct CacheEntry_t
	{
		int font;
		wchar_t wch;
		unsigned char page;
		float texCoords[4];

		HCacheEntry nextEntry;
		HCacheEntry prevEntry;
	};

	struct Page_t
	{
		short textureID[FONT_DRAW_TYPE_COUNT];
		short fontHeight;
		short wide, tall;
		short nextX, nextY;
	};

	bool AllocatePageForChar(int charWide, int charTall, int &pageIndex, int &drawX, int &drawY, int &twide, int &ttall);
	int ComputePageType(int charTall) const;
	static bool CacheEntryLessFunc(const CacheEntry_t &lhs, const CacheEntry_t &rhs);

	typedef CUtlVector<Page_t> FontPageList_t;

	CUtlRBTree<CacheEntry_t, HCacheEntry> m_CharCache;
	FontPageList_t m_PageList;
	int m_pCurrPage[FONT_PAGE_SIZE_COUNT];
	HCacheEntry m_LRUListHeadIndex;

	static int s_pFontPageSize[FONT_PAGE_SIZE_COUNT];
};

CFontTextureCache &FontTextureCache(void);

#endif