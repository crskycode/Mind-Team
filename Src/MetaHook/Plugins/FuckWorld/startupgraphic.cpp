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
#include <GdiPlus.h>
#include "engine_addr.h"
#include "plugins.h"
#include "TextureLoad.h"


void ( __fastcall *g_pfn_CVideoMode_Common__DrawStartupGraphic )( void *pthis, int edx, HWND hwnd );


void DrawLoadingText( HWND window, RECT *rect )
{
	HDC hdc = GetDC( window );
	FillRect( hdc, rect, (HBRUSH)GetStockObject( DKGRAY_BRUSH ) );

	HFONT fnt;
		fnt = CreateFont(
		-15								// H
		, 0								// W
		, 0								// Escapement
		, 0								// Orient
		, FW_HEAVY	 					// Wt.  (BOLD)
		, TRUE							// Ital.
		, 0								// Underl.
		, 0								// SO
		, ANSI_CHARSET					// Charset
		, OUT_TT_PRECIS					// Out prec.
		, CLIP_DEFAULT_PRECIS			// Clip prec.
		, PROOF_QUALITY					// Qual.
		, VARIABLE_PITCH | FF_DONTCARE	// Pitch and Fam.
		, "Arial" );

	COLORREF clr = RGB( 200, 200, 200 );

	SetTextColor( hdc, clr );
	SetBkMode( hdc, TRANSPARENT );

	HFONT old = (HFONT)SelectObject( hdc, fnt );
	DrawText( hdc, "Loading..." , -1, rect, 
		DT_CENTER | DT_VCENTER | DT_NOPREFIX | DT_SINGLELINE );

	SelectObject( hdc, old );
	DeleteObject( fnt );
	ReleaseDC( window, hdc );
}

#define BUFFER_SIZE ( 1024 * 1024 * 4 )

void DrawLoadingImage( HWND window, RECT *rect )
{
	byte	*bits0, *bits1;
	int		wide, tall;
	int		x, y;

	bits0 = (byte *)malloc( BUFFER_SIZE );
	bits1 = (byte *)malloc( BUFFER_SIZE );

	if ( !bits0 || !bits1 )
	{
		if ( bits0 )
			free( bits0 );

		if ( bits1 )
			free( bits1 );

		DrawLoadingText( window, rect );
		return;
	}

	if ( !LoadDTX32( "tex/UI/Loading/Title1.DTX", bits0, BUFFER_SIZE, &wide, &tall ) )
	{
		if ( bits0 )
			free( bits0 );

		if ( bits1 )
			free( bits1 );

		DrawLoadingText( window, rect );
		return;
	}

	for ( y = 0; y < tall; y++ )
	{
		for ( x = 0; x < wide; x++ )
		{
			byte *src = &bits0[ ( ( ( tall - y - 1 ) * wide ) + x ) * 4 ];
			byte *dst = &bits1[ ( ( y * wide ) + x ) * 4 ];

			dst[0] = src[2];	// B
			dst[1] = src[1];	// G
			dst[2] = src[0];	// R
			dst[3] = src[3];	// A
		}
	}

	BITMAPINFOHEADER hdr;
	hdr.biSize = sizeof( BITMAPINFOHEADER );
	hdr.biWidth = wide;
	hdr.biHeight = tall;
	hdr.biPlanes = 1;
	hdr.biBitCount = 32;
	hdr.biCompression = BI_RGB;
	hdr.biSizeImage = 0;
	hdr.biXPelsPerMeter = 0;
	hdr.biYPelsPerMeter = 0;
	hdr.biClrUsed = 0;
	hdr.biClrImportant = 0;

	Gdiplus::Bitmap image( (BITMAPINFO *)&hdr, bits1 );

	Gdiplus::Graphics graphics( window );

	graphics.DrawImage( &image, Gdiplus::RectF( 0, 0, rect->right, rect->bottom ), 0, 0, 1024, 768, Gdiplus::UnitPixel );

	Gdiplus::Font font( L"SimSun", 9, Gdiplus::FontStyleBold, Gdiplus::UnitPoint );

	// Outline color
	Gdiplus::SolidBrush brush0( Gdiplus::Color( 0x00, 0x00, 0x00 ) );
	// String color
	Gdiplus::SolidBrush brush1( Gdiplus::Color( 0xFF, 0xFF, 0xFF ) );

	PWCHAR str = L"Initializing Render Mode ...";

	x = 580.0 * ( rect->right / 800.0 );
	y = 540.0 * ( rect->bottom / 600.0 );

	graphics.DrawString( str, -1, &font, Gdiplus::PointF( x - 1, y ), NULL, &brush0 );
	graphics.DrawString( str, -1, &font, Gdiplus::PointF( x + 1, y ), NULL, &brush0 );
	graphics.DrawString( str, -1, &font, Gdiplus::PointF( x, y + 1 ), NULL, &brush0 );
	graphics.DrawString( str, -1, &font, Gdiplus::PointF( x, y - 1 ), NULL, &brush0 );

	graphics.DrawString( str, -1, &font, Gdiplus::PointF( x + 0, y ), NULL, &brush1 );

	if ( bits0 )
		free( bits0 );

	if ( bits1 )
		free( bits1 );
}

// CVideoMode_Common::DrawStartupGraphic( HWND window )
void __fastcall CVideoMode_Common__DrawStartupGraphic( void *pthis, int edx, HWND window )
{
	RECT	rc;

	GetClientRect( window, &rc );

	Gdiplus::GdiplusStartupInput gdipStartupInput;
	ULONG gdipToken;

	if ( Gdiplus::GdiplusStartup( &gdipToken, &gdipStartupInput, NULL ) == Gdiplus::Status::Ok )
	{
		DrawLoadingImage( window, &rc );
		Gdiplus::GdiplusShutdown( gdipToken );
	}
	else
	{
		DrawLoadingText( window, &rc );
	}
}

void StartupGraphic_Init( void )
{
	if ( g_addr_CVideoMode_Common__DrawStartupGraphic )
	{
		g_pMetaHookAPI->InlineHook( g_addr_CVideoMode_Common__DrawStartupGraphic, CVideoMode_Common__DrawStartupGraphic, (void *&)g_pfn_CVideoMode_Common__DrawStartupGraphic );
	}
}
