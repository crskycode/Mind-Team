#ifndef IMETAHOOKPLUGIN_H
#define IMETAHOOKPLUGIN_H

#include <interface.h>

typedef int (*STUDIOSETUPSKINPROC)(void *entity, void *ptexturehdr, int index);

class IMTEngine : public IBaseInterface
{
public:
	// Return current engine FPS value.
	virtual float GetFPS(void) = 0;

	// TexAPI can load your textures and draw them in game scene.
	virtual void GetTexAPI(struct texture_api_s *funcs) = 0;

	// FontAPI can use custom font to draw text and measure text rectangle.
	virtual void GetFontAPI(struct font_api_s *funcs) = 0;

	// Set model skin callback, so we can modify model's skin (hands etc).
	virtual void SetModelSkinCallBack(STUDIOSETUPSKINPROC pfn) = 0;

	// Return engine filesystem interface.
	virtual void *GetFileSystem(void) = 0;

	// Return a new texture ID.
	virtual int CreateNewTextureID(void) = 0;

	// Get StudioRender factory.
	virtual void GetStudioRenderAPI(struct studio_render_s *func) = 0;

	// Returns true if the box is completely outside the frustom
	virtual int R_CullBox(float *mins, float *maxs) = 0;
};

#define MTENGINE_VERSION "MTEngine001"

#endif