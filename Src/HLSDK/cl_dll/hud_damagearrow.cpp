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

#include <triangleapi.h>

void CHudDamageArrow::VidInit( void )
{
	m_pArrow = gTexAPI.LoadDTX( "tex/UI/PlayMain/DamageArrow.dtx" );

	m_bIsFall = false;
	m_flStartTime = 0;
}

void CHudDamageArrow::Redraw( void )
{
	if ( !m_flStartTime )
		return;

	float percen;

	if ( gHUD.CalcFadePercen( m_flStartTime, 0.0, 0.2, 1.0, percen ) == 2 )
	{
		m_flStartTime = 0;
		return;
	}

	int iBaseX = ScreenWide / 2;
	int iBaseY = ScreenTall / 2;

	Vector world, screen, angle;

	if ( !m_bIsFall )
	{
		world = m_vecOrigin;

		bool bClip = gEngfuncs.pTriAPI->WorldToScreen( world, screen );

		int iDestX = XPROJECT(screen.x);
		int iDestY = YPROJECT(screen.y);

		iDestX = max( 0, iDestX );
		iDestX = min( ScreenWide, iDestX );

		iDestY = max( 0, iDestY );
		iDestY = min( ScreenTall, iDestY );

		Vector cnete, point;

		cnete.x = iBaseX;
		cnete.y = iBaseY;
		point.x = iDestX;
		point.y = iDestY;
		
		Vector dir = ( point - cnete ).Normalize();

		if ( bClip )
		{
			dir = -dir;
		}

		VectorAngles( dir, angle );

		angle.y += 90.0;
	}
	else
	{
		angle.y = 180;
	}

	int iCentX = m_pArrow->wide / 2;
	int iCentY = m_pArrow->tall / 2;

	gTexAPI.RenderMode( RenderAdditive );
	gTexAPI.DrawSetColor( 255, 255, 255, 255 * percen );
	gTexAPI.DrawSetTexture( m_pArrow );
	gTexAPI.DrawEnableRotate( iCentX, iCentY, angle.y );
	gTexAPI.DrawTexturedRect( iBaseX - iCentX, iBaseY - iCentY, m_pArrow->wide, m_pArrow->tall );
}

void CHudDamageArrow::MsgFunc_DmgArrow( const char *pszName, int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );

	m_bIsFall = READ_BYTE();

	m_vecOrigin.x = READ_COORD();
	m_vecOrigin.y = READ_COORD();
	m_vecOrigin.z = READ_COORD();

	m_flStartTime = ClientTime;
}