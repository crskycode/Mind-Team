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
#include <com_model.h>
#include "plugins.h"
#include "engine_addr.h"
#include "keyvalues_helper.h"

static NewKeyValues *bsptextures = NULL;


void ( *g_pfn_Mod_LoadBrushModel )( model_t *mod, void *buffer );


void Mod_LoadBrushModel( model_t *mod, void *buffer )
{
	char			filename[64];
	char			*ext;
	NewKeyValues	*root;

	strcpy( filename, mod->name );
	ext = strrchr( filename, '.' );
	strcpy(ext, ".vdf");

	root = LoadKeyValuesFromFile( filename );

	if ( root )
	{
		bsptextures = root->FindKey( "Texture" );
	}

	g_pfn_Mod_LoadBrushModel( mod, buffer );

	if ( root )
	{
		root->deleteThis();
		root = NULL;
		bsptextures = NULL;
	}
}

void BspTexture_Find( char *source, char *texturename )
{
	texturename[0] = '\0';

	if ( bsptextures )
	{
		strncpy( texturename, bsptextures->GetString( source ), 63 );
	}
}

void BspTexture_Init( void )
{
	if ( g_addr_Mod_LoadBrushModel )
	{
		g_pMetaHookAPI->InlineHook( g_addr_Mod_LoadBrushModel, Mod_LoadBrushModel, (void *&)g_pfn_Mod_LoadBrushModel );
	}
	else
	{
		CON_PRINTF( "Failed to initialize BSP textures\n" );
	}
}
