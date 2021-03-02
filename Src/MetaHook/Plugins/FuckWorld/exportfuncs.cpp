#include <metahook.h>
#include <ref_params.h>
#include "plugins.h"
#include "quakedef.h"
#include "mathlib.h"
#include "qgl.h"
#include "surface.h"
#include "DrawTextures.h"
#include "cachedbg.h"
#include "ftfont.h"

cl_enginefunc_t gEngfuncs;

void DebugLine_Redraw(void);

int Initialize(struct cl_enginefuncs_s *pEnginefuncs, int iVersion)
{
	memcpy(&gEngfuncs, pEnginefuncs, sizeof(gEngfuncs));

	if (!gExportfuncs.Initialize(pEnginefuncs, iVersion))
		return 0;

	QGL_Init();
	Surface_InstallHook();

	DrawTexture_Init();
	CacheDbg_Init();
	FTF_Init();

	qglEnable( GL_LINE_SMOOTH );
	qglHint( GL_LINE_SMOOTH_HINT, GL_NICEST );

	return 1;
}

#include "BinkTextureInterface.h"

static IBinkTextureObject *bink = nullptr;

void HUD_Init(void)
{
	bink = createBinkTextureObject();

	if (bink->open("sfscope.bik") == false) {
		deleteBinkTextureObject(bink);
		bink = nullptr;
	}
	bink->setPause(false);
	bink->setLoop(true);

	return gExportfuncs.HUD_Init();
}

int HUD_Redraw(float time, int intermission)
{
	if (bink) {
		bink->updateFrame();
		bink->updateTexture();
		GLuint texId;
		GLfloat coords[4];
		bink->getTexture(&texId, coords);

		RECT rc;
		rc.left = 0;
		rc.right = rc.left + 640;
		rc.top = 0;
		rc.bottom = rc.top + 480;

		qglEnable(GL_BLEND);
		qglBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		qglColor4f(0, 0.6, 0.8, 1);
		qglBindTexture(GL_TEXTURE_2D, texId);
		qglBegin(GL_QUADS);
		qglTexCoord2f(coords[0], coords[2]);
		qglVertex2f(rc.left, rc.top);
		qglTexCoord2f(coords[1], coords[2]);
		qglVertex2f(rc.right, rc.top);
		qglTexCoord2f(coords[1], coords[3]);
		qglVertex2f(rc.right, rc.bottom);
		qglTexCoord2f(coords[0], coords[3]);
		qglVertex2f(rc.left, rc.bottom);
		qglEnd();
		/*tex_t t;
		t.id = texId;
		t.wide = 640;
		t.tall = 360;
		gTexAPI.RenderMode(RenderNormal);
		gTexAPI.DrawSetTexture(&t);
		gTexAPI.DrawSetColor(255, 255, 255, 255);
		gTexAPI.DrawTexturedRect(0, 0, t.wide, t.tall);*/
	}

	return gExportfuncs.HUD_Redraw(time, intermission);
}

void V_CalcRefdef(struct ref_params_s *pparams)
{
	return gExportfuncs.V_CalcRefdef(pparams);
}

void HUD_DrawTransparentTriangles(void)
{
	DebugLine_Redraw();
	return gExportfuncs.HUD_DrawTransparentTriangles();
}

void HUD_Shutdown(void)
{
	gExportfuncs.HUD_Shutdown();
	FTF_Shutdown();
}

void HUD_Frame(double time)
{
	return gExportfuncs.HUD_Frame(time);
}

int HUD_GetStudioModelInterface(int version, struct r_studio_interface_s **ppinterface, struct engine_studio_api_s *pstudio)
{
	return gExportfuncs.HUD_GetStudioModelInterface(version, ppinterface, pstudio);
}