#include <metahook.h>
#include "qgl.h"
#include "surface.h"
#include "plugins.h"

#include "TextureCache.h"
#include "TextureLoad.h"

#include <string>

#if _MSC_VER > 1400
#include <unordered_map>
typedef std::unordered_map<std::string, tex_t *> texturemap_t;
#else
#include <hash_map>
typedef stdext::hash_map<std::string, tex_t *> texturemap_t;
#endif

class CTextureCache : public ITextureCache
{
public:
	~CTextureCache();

	virtual tex_t *FindTexture(const char *name);
	virtual tex_t *Load(const char *name);
	virtual tex_t *LoadTGA(const char *name);
	virtual tex_t *LoadDTX(const char *name);
	virtual tex_t *LoadBMP(const char *name);
	virtual tex_t *LoadPNG(const char *name);

private:
	tex_t *GenTexture(const char *name);
	void FreeAllTexture(void);

	texturemap_t m_textureMap;
}
s_TextureCache;

ITextureCache &TextureCache(void)
{
	return s_TextureCache;
}

CTextureCache::~CTextureCache()
{
	FreeAllTexture();
}

tex_t *CTextureCache::GenTexture(const char *name)
{
	tex_t *item = new tex_t;

	item->id = 0;
	item->wide = 64;
	item->tall = 64;

	m_textureMap.insert(texturemap_t::value_type(name, item));

	return item;
}

tex_t *CTextureCache::FindTexture(const char *name)
{
	texturemap_t::iterator it = m_textureMap.find(name);

	if (it != m_textureMap.end())
		return it->second;

	return NULL;
}

void CTextureCache::FreeAllTexture(void)
{
	for (texturemap_t::iterator it = m_textureMap.begin(); it != m_textureMap.end(); it++)
	{
		delete it->second;
		it->second = NULL;
	}
}

tex_t *CTextureCache::Load(const char *name)
{
	tex_t *pBadTex = FindTexture("gfx/bad.tga");

	if (!pBadTex)
		pBadTex = LoadTGA("gfx/bad.tga");

	if (!name || !*name)
		return pBadTex;

	const char *ext = strrchr(name, '.');

	if (!ext)
		return pBadTex;

	if (!stricmp(ext, ".tga"))
		return LoadTGA(name);
	else if (!stricmp(ext, ".dtx"))
		return LoadDTX(name);
	else if (!stricmp(ext, ".bmp"))
		return LoadBMP(name);
	else if (!stricmp(ext, ".png"))
		return LoadPNG(name);

	return pBadTex;
}

tex_t *CTextureCache::LoadTGA(const char *name)
{
	char path[MAX_PATH];
	static byte buffer[1024 * 1024 * 4];
	int wide, tall;

	strcpy(path, name);
	strlwr(path);

	tex_t *item = FindTexture(path);

	if (item)
		return item;

	item = GenTexture(path);

	if (::LoadTGA(path, buffer, sizeof(buffer), &wide, &tall))
	{
		item->id = g_pSurface->CreateNewTextureID();
		item->wide = wide;
		item->tall = tall;

		qglBindTexture(GL_TEXTURE_2D, item->id);
		qglTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, item->wide, item->tall, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
		qglTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		qglTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		qglTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		qglTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		MHP_LogFile("LoadTGA() Load \"%s\" successfully, TextureID: [%d]\n", path, item->id);
		return item;
	}

	MHP_LogFile("LoadTGA() Load \"%s\" failed\n", path);
	return item;
}

tex_t *CTextureCache::LoadDTX(const char *name)
{
	char path[MAX_PATH];
	static byte buffer[1024 * 1024 * 4];
	int format, wide, tall, size;

	strcpy(path, name);
	strlwr(path);

	tex_t *item = FindTexture(path);

	if (item)
		return item;

	item = GenTexture(path);
	
	item->id = g_pSurface->CreateNewTextureID();

	qglBindTexture(GL_TEXTURE_2D, item->id);

	if (::LoadDTX_Mipmap(path, buffer, sizeof(buffer), &wide, &tall))
	{
		item->wide = wide;
		item->tall = tall;

		qglTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		qglTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		MHP_LogFile("LoadDTX() Load \"%s\" successfully, TextureID: [%d]\n", path, item->id);
		return item;
	}
	else
	{
		qglDeleteTextures(1, (GLuint *)&item->id);
		item->id = 0;
	}

	MHP_LogFile("LoadDTX() Load \"%s\" failed\n", path);
	return item;
}

tex_t *CTextureCache::LoadBMP(const char *name)
{
	char path[MAX_PATH];
	static byte buffer[1024 * 1024 * 4];
	int wide, tall;

	strcpy(path, name);
	strlwr(path);

	tex_t *item = FindTexture(path);

	if (item)
		return item;

	item = GenTexture(path);

	if (::LoadBMP(path, buffer, sizeof(buffer), &wide, &tall))
	{
		item->id = g_pSurface->CreateNewTextureID();
		item->wide = wide;
		item->tall = tall;

		qglBindTexture(GL_TEXTURE_2D, item->id);
		qglTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, item->wide, item->tall, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
		qglTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		qglTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		qglTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		qglTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		MHP_LogFile("LoadBMP() Load \"%s\" successfully, TextureID: [%d]\n", path, item->id);
		return item;
	}

	MHP_LogFile("LoadBMP() Load \"%s\" failed\n", path);
	return item;
}

tex_t *CTextureCache::LoadPNG(const char *name)
{
	char path[MAX_PATH];
	static byte buffer[1024 * 1024 * 4];
	int wide, tall;

	strcpy(path, name);
	strlwr(path);

	tex_t *item = FindTexture(path);

	if (item)
		return item;

	item = GenTexture(path);

	if (::LoadPNG(path, buffer, sizeof(buffer), &wide, &tall))
	{
		item->id = g_pSurface->CreateNewTextureID();
		item->wide = wide;
		item->tall = tall;

		qglBindTexture(GL_TEXTURE_2D, item->id);
		qglTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, item->wide, item->tall, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
		qglTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		qglTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		qglTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		qglTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		MHP_LogFile("LoadPNG() Load \"%s\" successfully, TextureID: [%d]\n", path, item->id);
		return item;
	}

	MHP_LogFile("LoadPNG() Load \"%s\" failed\n", path);
	return item;
}