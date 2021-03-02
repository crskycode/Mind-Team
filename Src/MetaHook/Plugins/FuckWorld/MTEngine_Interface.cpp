#include <metahook.h>
#include <IMTEngine.h>

#include "surface.h"
#include "DrawTextures.h"
#include "DrawFonts.h"
#include "engine_addr.h"
#include "IStudioRender.h"

float FPS_Get(void);
void StudioModelSkin_SetCallBack(STUDIOSETUPSKINPROC pfn);

typedef int(*pfn_r_cullbox_t)(float *, float *);

class CMTEngine : public IMTEngine
{
public:
	float GetFPS(void)
	{
		return FPS_Get();
	}

	void GetTexAPI(texture_api_t *funcs)
	{
		memcpy(funcs, &gTexAPI, sizeof(texture_api_t));
	}

	void GetFontAPI(font_api_t *funcs)
	{
		memcpy(funcs, &gFontAPI, sizeof(font_api_t));
	}

	void SetModelSkinCallBack(STUDIOSETUPSKINPROC pfn)
	{
		StudioModelSkin_SetCallBack(pfn);
	}

	void *GetFileSystem(void)
	{
		return g_pInterface->FileSystem;
	}

	int CreateNewTextureID(void)
	{
		return g_pSurface->CreateNewTextureID();
	}

	void GetStudioRenderAPI(studio_render_t *func)
	{
		memcpy(func, &gStudioRender, sizeof(studio_render_t));
	}

	int R_CullBox(float *mins, float *maxs)
	{
		if (g_addr_R_CullBox)
		{
			return ((pfn_r_cullbox_t)g_addr_R_CullBox)(mins, maxs);
		}

		return true;
	}
};

EXPOSE_SINGLE_INTERFACE(CMTEngine, IMTEngine, MTENGINE_VERSION);