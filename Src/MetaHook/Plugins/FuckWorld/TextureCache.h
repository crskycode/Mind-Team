#ifndef TEXTURECACHE_H
#define TEXTURECACHE_H

typedef struct tex_s
{
	int id;
	int wide;
	int tall;
}
tex_t;

class ITextureCache
{
public:
	virtual tex_t *FindTexture(const char *name) = 0;
	virtual tex_t *Load(const char *name) = 0;
	virtual tex_t *LoadTGA(const char *name) = 0;
	virtual tex_t *LoadDTX(const char *name) = 0;
	virtual tex_t *LoadBMP(const char *name) = 0;
	virtual tex_t *LoadPNG(const char *name) = 0;
};

ITextureCache &TextureCache(void);

#endif