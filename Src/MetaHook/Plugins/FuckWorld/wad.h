
#ifndef WAD_H
#define WAD_H

typedef struct cachewad_s
{
	char	*name;
	void	*cache;
	int		cacheCount;
	int		cacheMax;
	void	*lumps;
	int		lumpCount;
	int		cacheExtra;

} cachewad_t;

typedef struct miptex_s
{
	char	name[16];
	int		width, height;

} miptex_t;

typedef struct texture_s
{
	char	name[16];
	int		width, height;
	int		gl_texturenum;
	void	*texturechain;
	int		anim_total;
	int		anim_min, anim_max;
	void	*anim_next;
	void	*alternate_anims;
	int		offsets[4];
	void	*pPal;

} texture_t;

typedef struct wadtex_s
{
	char	name[64];
	int		width, height;

} wadtex_t;

void WAD_FindTexture(char *pSource, wadtex_t *pOut);

#endif