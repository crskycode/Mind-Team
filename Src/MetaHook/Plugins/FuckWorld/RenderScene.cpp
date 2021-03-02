#include <metahook.h>
#include "plugins.h"


void (*g_pfnR_RenderView)(void) = NULL;

#define R_RENDERVIEW_SIG "\xD9\x05\x2A\x2A\x2A\x2A\xD8\x1D\x2A\x2A\x2A\x2A\x83\xEC\x14\xDF\xE0\xF6\xC4\x44\x0F\x8A\x2A\x2A\x2A\x2A\xA1\x2A\x2A\x2A\x2A\x85\xC0"


typedef int (*RENDERSCENEPROC)(void);

RENDERSCENEPROC g_pfnRenderScenePreCallBack = NULL;
RENDERSCENEPROC g_pfnRenderScenePostCallBack = NULL;


void R_RenderView(void)
{
	if (g_pfnRenderScenePreCallBack)
	{
		if (g_pfnRenderScenePreCallBack())
			return;
	}

	g_pfnR_RenderView();

	if (g_pfnRenderScenePostCallBack)
	{
		g_pfnRenderScenePostCallBack();
	}
}

void RenderScene_InstallHook(void)
{
	*(void **)&g_pfnR_RenderView = SearchPattern(g_dwEngineBase, g_dwEngineSize, R_RENDERVIEW_SIG, sizeof(R_RENDERVIEW_SIG) - 1);

	if (g_pfnR_RenderView)
		InlineHook(g_pfnR_RenderView, R_RenderView, (void *&)g_pfnR_RenderView);
	else
		SigNotFound(R_RENDERVIEW_SIG);
}

void RenderScene_SetCallBack(RENDERSCENEPROC pre, RENDERSCENEPROC post)
{
	g_pfnRenderScenePreCallBack = pre;
	g_pfnRenderScenePostCallBack = post;
}