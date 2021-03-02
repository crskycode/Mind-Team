#include <metahook.h>

#define _USE_MATH_DEFINES
#include <math.h>

#include "surface.h"
#include "qgl.h"
#include "Shader.h"

#include "TextureCache.h"
#include "DrawTextures.h"

int GL_GenTexture(void)
{
	return g_pSurface->CreateNewTextureID();
}

tex_t *FindTexture(const char *name)
{
	return TextureCache().FindTexture(name);
}

tex_t *Load(const char *name)
{
	return TextureCache().Load(name);
}

tex_t *LoadTGA(const char *name)
{
	return TextureCache().LoadTGA(name);
}

tex_t *LoadDTX(const char *name)
{
	return TextureCache().LoadDTX(name);
}

tex_t *LoadBMP(const char *name)
{
	return TextureCache().LoadBMP(name);
}

tex_t *LoadPNG(const char *name)
{
	return TextureCache().LoadPNG(name);
}

static tex_t *	g_drawTexture = NULL;
static float	g_drawCoords[4] = { 0, 0, 1, 1 };
static float	g_drawRotate[3] = { 0, 0, 0 };
static int		g_drawRoundRect[3] = { 0, 0, 0 };

struct ShaderDrawRoundRect_t
{
	int texture;
	int center;
	int radius;
};

int						g_shaderDrawRoundRect;
ShaderDrawRoundRect_t	g_shaderDrawRoundRectUniform;

void DrawTexture_Init(void)
{
	g_shaderDrawRoundRect = CreateShader("shaders/hud_drawroundrect.vsh", "shaders/hud_drawroundrect.fsh");

	g_shaderDrawRoundRectUniform.texture = qglGetUniformLocationARB(g_shaderDrawRoundRect, "texture");
	g_shaderDrawRoundRectUniform.center = qglGetUniformLocationARB(g_shaderDrawRoundRect, "center");
	g_shaderDrawRoundRectUniform.radius = qglGetUniformLocationARB(g_shaderDrawRoundRect, "radius");
}

void RenderMode(int mode)
{
	switch (mode)
	{
		case RenderNormal:
		{
			qglEnable(GL_TEXTURE_2D);
			qglEnable(GL_BLEND);
			qglBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			break;
		}
		case RenderAdditive:
		{
			qglEnable(GL_TEXTURE_2D);
			qglEnable(GL_BLEND);
			qglBlendFunc(GL_SRC_ALPHA, GL_ONE);
			break;
		}
	}
}

void TextureEnableScissor(int x, int y, int wide, int tall)
{
	g_drawCoords[0] = (float)x / (float)g_drawTexture->wide;
	g_drawCoords[1] = (float)y / (float)g_drawTexture->tall;
	g_drawCoords[2] = (float)(x + wide) / (float)g_drawTexture->wide;
	g_drawCoords[3] = (float)(y + tall) / (float)g_drawTexture->tall;
}

void TextureDisableScissor(void)
{
	g_drawCoords[0] = g_drawCoords[1] = 0;
	g_drawCoords[2] = g_drawCoords[3] = 1;
}

void DrawEnableRotate(int x, int y, float angle)
{
	g_drawRotate[0] = x;
	g_drawRotate[1] = y;
	g_drawRotate[2] = angle * (M_PI / 180.0);
}

void DrawDisableRotate(void)
{
	g_drawRotate[0] = g_drawRotate[1] = g_drawRotate[2] = 0;
}

void DrawEnableRoundRectScissor(int x, int y, int radius)
{
	g_drawRoundRect[0] = x;
	g_drawRoundRect[1] = y;
	g_drawRoundRect[2] = radius;
}

void DrawDisableRoundRectScissor(void)
{
	g_drawRoundRect[0] = g_drawRoundRect[1] = g_drawRoundRect[2] = 0;
}

void DrawSetTexture(tex_t *texture)
{
	g_drawTexture = texture;
	qglBindTexture(GL_TEXTURE_2D, g_drawTexture->id);

	TextureDisableScissor();
	DrawDisableRotate();
	DrawDisableRoundRectScissor();
}

void DrawSetColor(int r, int g, int b, int a)
{
	qglColor4ub(r, g, b, a);
}

void DrawTexturedRect(int x, int y, int wide, int tall)
{
	int v0[2], v1[2], v2[2], v3[2];

	v0[0] = x;			v0[1] = y;
	v1[0] = x;			v1[1] = y + tall;
	v2[0] = x + wide;	v2[1] = y + tall;
	v3[0] = x + wide;	v3[1] = y;

	if (g_drawRotate[2] != 0)
	{
		float rx, ry, ca, sa, tx, ty;

		rx = g_drawRotate[0] + x;
		ry = g_drawRotate[1] + y;

		ca = cos(g_drawRotate[2]);
		sa = sin(g_drawRotate[2]);

		tx = v0[0];
		ty = v0[1];
		v0[0] = (tx - rx) * ca - (ty - ry) * sa + rx;
		v0[1] = (tx - rx) * sa + (ty - ry) * ca + ry;
		tx = v1[0];
		ty = v1[1];
		v1[0] = (tx - rx) * ca - (ty - ry) * sa + rx;
		v1[1] = (tx - rx) * sa + (ty - ry) * ca + ry;
		tx = v2[0];
		ty = v2[1];
		v2[0] = (tx - rx) * ca - (ty - ry) * sa + rx;
		v2[1] = (tx - rx) * sa + (ty - ry) * ca + ry;
		tx = v3[0];
		ty = v3[1];
		v3[0] = (tx - rx) * ca - (ty - ry) * sa + rx;
		v3[1] = (tx - rx) * sa + (ty - ry) * ca + ry;
	}

	if (g_drawRoundRect[2] != 0)
	{
		float cx, cy, ra;

		cx = g_drawRoundRect[0];
		cy = g_drawRoundRect[1];
		ra = g_drawRoundRect[2];

		qglUseProgramObjectARB(g_shaderDrawRoundRect);
		qglUniform1iARB(g_shaderDrawRoundRectUniform.texture, 0);
		qglUniform2fARB(g_shaderDrawRoundRectUniform.center, cx, cy);
		qglUniform1fARB(g_shaderDrawRoundRectUniform.radius, ra);
	}

	qglBegin(GL_QUADS);
		qglTexCoord2f(g_drawCoords[0], g_drawCoords[1]);
		qglVertex3f(v0[0], v0[1], 0);
		qglTexCoord2f(g_drawCoords[0], g_drawCoords[3]);
		qglVertex3f(v1[0], v1[1], 0);
		qglTexCoord2f(g_drawCoords[2], g_drawCoords[3]);
		qglVertex3f(v2[0], v2[1], 0);
		qglTexCoord2f(g_drawCoords[2], g_drawCoords[1]);
		qglVertex3f(v3[0], v3[1], 0);
	qglEnd();

	if (g_drawRoundRect[2] != 0)
	{
		qglUseProgramObjectARB(0);
	}
}

void DrawTexturedRectEx(int x0, int y0, int x1, int y1)
{
	if (g_drawRoundRect[2] != 0)
	{
		float cx, cy, ra;

		cx = g_drawRoundRect[0];
		cy = g_drawRoundRect[1];
		ra = g_drawRoundRect[2];

		qglUseProgramObjectARB(g_shaderDrawRoundRect);
		qglUniform1iARB(g_shaderDrawRoundRectUniform.texture, 0);
		qglUniform2fARB(g_shaderDrawRoundRectUniform.center, cx, cy);
		qglUniform1fARB(g_shaderDrawRoundRectUniform.radius, ra);
	}

	qglBegin(GL_QUADS);
		qglTexCoord2f(g_drawCoords[0], g_drawCoords[1]);
		qglVertex3f(x0, y0, 0);
		qglTexCoord2f(g_drawCoords[0], g_drawCoords[3]);
		qglVertex3f(x0, y1, 0);
		qglTexCoord2f(g_drawCoords[2], g_drawCoords[3]);
		qglVertex3f(x1, y1, 0);
		qglTexCoord2f(g_drawCoords[2], g_drawCoords[1]);
		qglVertex3f(x1, y0, 0);
	qglEnd();

	if (g_drawRoundRect[2] != 0)
	{
		qglUseProgramObjectARB(0);
	}
}

void DrawTexturedRect3D(const float *v0, const float *v1, const float *v2, const float *v3)
{
	qglBegin(GL_QUADS);
		qglTexCoord2f(g_drawCoords[0], g_drawCoords[1]);
		qglVertex3f(v0[0], v0[1], v0[2]);
		qglTexCoord2f(g_drawCoords[0], g_drawCoords[3]);
		qglVertex3f(v1[0], v1[1], v1[2]);
		qglTexCoord2f(g_drawCoords[2], g_drawCoords[3]);
		qglVertex3f(v2[0], v2[1], v2[2]);
		qglTexCoord2f(g_drawCoords[2], g_drawCoords[1]);
		qglVertex3f(v3[0], v3[1], v3[2]);
	qglEnd();
}

template <typename T>
void _swap(T &a, T &b)
{
	T t;
	t = a;
	a = b;
	b = t;
}

void HorizontalFlip(void)
{
	_swap(g_drawCoords[0], g_drawCoords[2]);
}

void VerticalFlip(void)
{
	_swap(g_drawCoords[1], g_drawCoords[3]);
}

texture_api_t gTexAPI = 
{
	GL_GenTexture,
	FindTexture,
	Load,
	LoadTGA,
	LoadDTX,
	LoadBMP,
	LoadPNG,
	RenderMode,
	DrawSetTexture,
	DrawSetColor,
	DrawTexturedRect,
	DrawTexturedRectEx,
	DrawTexturedRect3D,
	TextureEnableScissor,
	TextureDisableScissor,
	DrawEnableRotate,
	DrawDisableRotate,
	DrawEnableRoundRectScissor,
	DrawDisableRoundRectScissor,
	HorizontalFlip,
	VerticalFlip,
};