#ifndef ITEXTUREAPI_H
#define ITEXTUREAPI_H

typedef struct tex_s
{
	int id;
	int wide;
	int tall;
}
tex_t, *HTEXTURE, *PTEXTURE;

enum TexRenderMode
{
	RenderNormal,
	RenderAdditive,
};

typedef struct texture_api_s
{
	int				( *CreateNewTextureID )				( void );
	tex_t *			( *FindTexture )					( const char *name );
	tex_t *			( *Load )							( const char *name );
	tex_t *			( *LoadTGA )						( const char *name );
	tex_t *			( *LoadDTX )						( const char *name );
	tex_t *			( *LoadBMP )						( const char *name );
	tex_t *			( *LoadPNG )						( const char *name );
	void			( *RenderMode )						( int mode );
	void			( *DrawSetTexture )					( tex_t *texture );
	void			( *DrawSetColor )					( int r, int g, int b, int a );
	void			( *DrawTexturedRect )				( int x, int y, int wide, int tall );
	void			( *DrawTexturedRectEx )				( int x0, int y0, int x1, int y1 );
	void			( *DrawTexturedRect3D )				( const float *v0, const float *v1, const float *v2, const float *v3 );
	void			( *TextureEnableScissor )			( int x, int y, int wide, int tall );
	void			( *TextureDisableScissor )			( void );
	void			( *DrawEnableRotate )				( int x, int y, float angle );
	void			( *DrawDisableRotate )				( void );
	void			( *DrawEnableCircleScissor )		( int x, int y, int radius );
	void			( *DrawDisableCircleScissor )		( void );
	void			( *HorizontalFlip )					( void );
	void			( *VerticalFlip )					( void );
}
texture_api_t;

extern texture_api_t gTexAPI;

#endif