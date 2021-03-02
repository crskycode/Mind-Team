#include <metahook.h>
#include "plugins.h"
#include "wad.h"
#include "keyvalues_helper.h"
#include "DrawTextures.h"

#define DECALS_FILE	"decals.vdf"

static NewKeyValues *g_WadTexture = NULL;


void (*g_pfnDraw_MiptexTexture)(cachewad_t *wad, byte *data) = NULL;

#define DRAW_MIPTEXTEXTURE_SIG "\x83\xEC\x28\x53\x55\x56\x8B\x74\x24\x38\x57\x83\x7E\x18\x20\x74\x2A\x8B\x06\x50\x68\x2A\x2A\x2A\x2A\xE8"


void Draw_MiptexTexture(cachewad_t *wad, byte *data)
{
	miptex_t mip = *(miptex_t *)(data + wad->cacheExtra);
	texture_t *tex = (texture_t *)data;

	memset(tex, 0, sizeof(texture_t));

	memcpy(tex->name, mip.name, sizeof(tex->name));
	tex->width = 16;
	tex->height = 16;
	tex->gl_texturenum = 0;

	wadtex_t wadtex;
	WAD_FindTexture(tex->name, &wadtex);

	if (wadtex.name[0])
	{
		tex_t *t = gTexAPI.Load(wadtex.name);

		tex->width = max(4, wadtex.width);
		tex->height = max(4, wadtex.height);
		tex->gl_texturenum = t->id;
	}
}

void WAD_Init(void)
{
	g_WadTexture = LoadKeyValuesFromFile(DECALS_FILE);

	if (!g_WadTexture)
	{
		gEngfuncs.Con_Printf("Couldn't load %s\n", DECALS_FILE);
	}
}

void WAD_FindTexture(char *pSource, wadtex_t *pOut)
{
	memset(pOut, 0, sizeof(wadtex_t));

	if (!g_WadTexture)
	{
		WAD_Init();
	}

	if (g_WadTexture)
	{
		NewKeyValues *pKey = g_WadTexture->FindKey(pSource);

		if (pKey)
		{
			strcpy(pOut->name, pKey->GetString("Texture", ""));
			pOut->width = pKey->GetInt("Width", 16);
			pOut->height = pKey->GetInt("Height", 16);
		}
		else
		{
			gEngfuncs.Con_Printf("Couldn't found %s in %s\n", pSource, DECALS_FILE);
		}
	}
}

void WAD_InstallHook(void)
{
	*(void **)&g_pfnDraw_MiptexTexture = SearchPattern(g_dwEngineBase, g_dwEngineSize, DRAW_MIPTEXTEXTURE_SIG, sizeof(DRAW_MIPTEXTEXTURE_SIG) - 1);
	
	if (g_pfnDraw_MiptexTexture)
	{
		InlineHook(g_pfnDraw_MiptexTexture, Draw_MiptexTexture, (void *&)g_pfnDraw_MiptexTexture);
	}
	else
	{
		SigNotFound(DRAW_MIPTEXTEXTURE_SIG);
	}
}