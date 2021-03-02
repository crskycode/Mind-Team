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


//
// gl_model.c
//

void *g_addr_Mod_LoadBrushModel = NULL;


//
// cmd.c
//

void *g_addr_Cmd_AddCommand = NULL;


//
// host.c
//

void *g_addr_rolling_fps = NULL;


//
// sys_getmodes.cpp
//

void *g_addr_CVideoMode_Common__DrawStartupGraphic = NULL;


//
// keys.c
//

void *g_addr_Key_Event = NULL;


//
// gl_warp.c
//

void *g_addr_R_LoadSkys = NULL;


//
// r_studio.c
//

void *g_addr_R_StudioSetupSkin = NULL;

//
// gl_draw.c
//

void *g_addr_GL_LoadTexture = NULL;
void *g_addr_Draw_MiptexTexture = NULL;

//
// gl_rmain.c
//

void *g_addr_R_CullBox = NULL;


//=============================================================================
// Purpose : 
//=============================================================================

void Engine_Init( void )
{

#define PTR( x ) ( (void *)( x ) )

	switch ( g_pMetaHookAPI->GetEngineBuildnum() )
	{
	case 3266:
		{
			g_addr_Mod_LoadBrushModel = PTR( 0x01D42830 );

			g_addr_Cmd_AddCommand = PTR( 0x01D2B290 );

			g_addr_rolling_fps = PTR( 0x023CDD50 );

			g_addr_CVideoMode_Common__DrawStartupGraphic = PTR( 0x01DBCA00 );

			g_addr_Key_Event = PTR( gEngfuncs.Key_Event );

			g_addr_R_LoadSkys = PTR( 0x01D4F8D2 );

			g_addr_R_StudioSetupSkin = PTR( 0x01D8E250 );

			g_addr_GL_LoadTexture = PTR( 0x01D3FB30 );

			g_addr_Draw_MiptexTexture = PTR( 0x01D3FDA0 );

			g_addr_R_CullBox = PTR( 0x01D446F0 );
		}
	default:
		{
		}
	}

#undef ADDR

}
