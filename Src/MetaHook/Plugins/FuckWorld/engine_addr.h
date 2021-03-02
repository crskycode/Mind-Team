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

#ifndef ENGINE_ADDR_H
#define ENGINE_ADDR_H


//
// gl_model.c
//

extern void *g_addr_Mod_LoadBrushModel;


//
// cmd.c
//

extern void *g_addr_Cmd_AddCommand;


//
// host.c
//

extern void *g_addr_rolling_fps;


//
// sys_getmodes.cpp
//

extern void *g_addr_CVideoMode_Common__DrawStartupGraphic;


//
// keys.c
//

extern void *g_addr_Key_Event;


//
// gl_warp.c
//

extern void *g_addr_R_LoadSkys;


//
// r_studio.c
//

extern void *g_addr_R_StudioSetupSkin;


//
// gl_draw.c
//

extern void *g_addr_GL_LoadTexture;
extern void *g_addr_Draw_MiptexTexture;

//
// gl_rmain.c
//

extern void *g_addr_R_CullBox;


void Engine_Init( void );

#endif
