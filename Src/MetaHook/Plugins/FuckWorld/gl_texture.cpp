/****
 * 
 * Copyright (c) 2017, Crsky
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 * 
 * * Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
****/

#include <metahook.h>
#include "engine_addr.h"
#include "plugins.h"
#include "surface.h"
#include "qgl.h"
#include "bsptexture.h"
#include "DrawTextures.h"


enum GL_TEXTURETYPE
{
	GLT_SYSTEM,
	GLT_DECAL,			// decals.wad tempdecal.wad
	GLT_HUDSPRITE,		// *.spr
	GLT_STUDIO,			// *.mdl
	GLT_WORLD,			// *.bsp halflife.wad
	GLT_SPRITE,			// *.spr
};

#define TEX_TYPE_NONE	0
#define TEX_TYPE_ALPHA	1
#define TEX_TYPE_LUM	2
#define TEX_TYPE_ALPHA_GRADIENT 3
#define TEX_TYPE_RGBA	4


int ( *g_pfn_GL_LoadTexture )( char *identifier, GL_TEXTURETYPE textureType, int width, int height, byte *data, qboolean mipmap, int iType, byte *pPal );


int GL_LoadTexture( char *identifier, GL_TEXTURETYPE textureType, int width, int height, byte *data, qboolean mipmap, int iType, byte *pPal )
{
	if ( textureType == GLT_WORLD )
	{
		char texturename[64];

		BspTexture_Find( identifier, texturename );

		if ( texturename[0] )
		{
			return gTexAPI.Load( texturename )->id;
		}
	}

	if ( textureType == GLT_STUDIO )
	{
		int				i;
		static byte		buffer[1024 * 1024 * 4];

		for ( i = 0; i < width * height; i++ )
		{
			byte *pixel = &buffer[i * 4];

			pixel[0] = pPal[ (data[i] * 3) + 0 ];
			pixel[1] = pPal[ (data[i] * 3) + 1 ];
			pixel[2] = pPal[ (data[i] * 3) + 2 ];
			pixel[3] = 255;

			if ( iType == TEX_TYPE_ALPHA )
			{
				if ( data[i] == 255 )
				{
					pixel[3] = 0;
				}
			}
		}

		int texnum = g_pSurface->CreateNewTextureID();

		qglBindTexture( GL_TEXTURE_2D, texnum );
		qglTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer );
		qglTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		qglTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

		return texnum;
	}

	return g_pfn_GL_LoadTexture( identifier, textureType, width, height, data, mipmap, iType, pPal );
}

void GLTexture_Init( void )
{
	if ( g_addr_GL_LoadTexture )
	{
		g_pMetaHookAPI->InlineHook( g_addr_GL_LoadTexture, GL_LoadTexture, (void *&)g_pfn_GL_LoadTexture );
	}
	else
	{
		CON_PRINTF( "Failed to initialize GL texture\n" );
	}
}
