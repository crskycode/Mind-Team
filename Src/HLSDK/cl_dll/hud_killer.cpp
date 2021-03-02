/***
 * 
 * Copyright (c) 1996-2002, Valve LLC. All rights reserved.
 * 
 * This product contains software technology licensed from Id
 * Software, Inc. ("Id Technology"). Id Technology (c) 1996 Id Software, Inc.
 * All Rights Reserved.
 * 
 * Use, distribution, and modification of this source code and/or resulting
 * object code is restricted to non-commercial enhancements to products from
 * Valve LLC. All other use, distribution, or modification is prohibited
 * without written permission from Valve LLC.
 * 
***/

#include "cl_dll.h"
#include "hud.h"

extern "C"
{
#include "pm_hull.h"
}

void CHudKillerIcon::VidInit( void )
{
	m_pMark50 = gTexAPI.LoadDTX( "tex/UI/ProjectionID/KillerMark_50.dtx" );
	m_pMark70 = gTexAPI.LoadDTX( "tex/UI/ProjectionID/KillerMark_70.dtx" );
	m_pMark100 = gTexAPI.LoadDTX( "tex/UI/ProjectionID/KillerMark_100.dtx" );

	m_iKillerID = 0;
	m_flStartTime = 0;
}

void CHudKillerIcon::Redraw( void )
{
	if ( !m_iKillerID )
		return;

	if ( !m_flStartTime )
		return;

	Vector world, screen;

	cl_entity_t *pl = gEngfuncs.GetEntityByIndex( m_iKillerID );

	if ( !pl )
		return;

	world = pl->curstate.origin;

	// put point on head
	world.z += VEC_IDLE_VIEW[2] + 10;

	if ( !gHUD.Project( world, screen ) )
		return;

	float percen;

	if ( gHUD.CalcFadePercen( m_flStartTime, 0.0, 1.0, 1.0, percen ) == 2 )
	{
		m_flStartTime = 0;
		return;
	}

	int iSrcX = screen.x;
	int iSrcY = screen.y;

	// out of left/right
	if ( iSrcX < -100 || iSrcX > ScreenWide + 50 )
		return;

	// out of top/bottom
	if ( iSrcY < -100 || iSrcY > ScreenTall + 50 )
		return;

	// clamp left/right
	iSrcX = max( 0, iSrcX );
	iSrcX = min( ScreenWide, iSrcX );

	// clamp top/bottom
	iSrcY = max( 0, iSrcY );
	iSrcY = min( ScreenTall, iSrcY );

	// calc distance
	float dist = ( world - Vector( ClientState.vecOrigin ) ).Length();

	// choose a mark icon
	tex_t *pMark = NULL;

	if ( dist < 100 )
		pMark = m_pMark100;
	else if ( dist < 500 )
		pMark = m_pMark70;
	else
		pMark = m_pMark50;

	gTexAPI.RenderMode( RenderNormal );
	gTexAPI.DrawSetColor( 255, 255, 255, 255 * percen );
	gTexAPI.DrawSetTexture( pMark );
	gTexAPI.DrawTexturedRect( iSrcX - pMark->wide / 2, iSrcY - pMark->tall / 2, pMark->wide, pMark->tall );
}

void CHudKillerIcon::MsgFunc_KillerIcon( const char *pszName, int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );

	m_iKillerID = READ_BYTE();

	// got a bad packet ?
	if ( m_iKillerID < 1 || m_iKillerID > 32 )
		return;

	m_flStartTime = ClientTime;
}