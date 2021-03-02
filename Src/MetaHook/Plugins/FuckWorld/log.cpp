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
#include "plugins.h"

#define LOG_FILE	".\\MetaHook.log"

VOID ( WINAPI *g_pfn_OutputDebugStringA )( LPCSTR lpOutputString );


VOID WINAPI NewOutputDebugStringA( LPCSTR lpOutputString )
{
	FILE *file = fopen( LOG_FILE, "at" );

	if ( file )
	{
		fprintf( file, "%s", lpOutputString );
		fclose( file );
	}

	g_pfn_OutputDebugStringA( lpOutputString );
}

void Log_Init( void )
{
	FILE *file = fopen( LOG_FILE, "w" );

	if ( file )
	{
		fclose( file );
	}

	g_pMetaHookAPI->InlineHook( &OutputDebugStringA, &NewOutputDebugStringA, (void *&)g_pfn_OutputDebugStringA );
}
