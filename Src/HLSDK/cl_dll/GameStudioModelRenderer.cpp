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
#include "cl_util.h"
#include <com_model.h>
#include <studio.h>
#include "studio_util.h"
#include <mathlib.h>

#include "qgl.h"

#include <r_studioint.h>

#include "GameState.h"
#include "ClientState.h"
#include "CharacterData.h"
#include "WeaponData.h"

#include "fov.h"
#include "MuzzleFlash.h"

#include "GameStudioModelRenderer.h"

// Global engine <-> studio model rendering code interface.
extern engine_studio_api_t IEngineStudio;

// The renderer object, created on the stack.
CGameStudioModelRenderer g_StudioRenderer;

#if defined ( TEST_VVIP_EYE )
void PART_Shoot(const float *origin, const float *velocity);
void PART_SetupTransform(const float **matrix);
void PART_Update(void);
#endif

#define gaitanimtime gaityaw

void CGameStudioModelRenderer::StudioSetupBones( void )
{
	int					i;
	double				f;

	mstudiobone_t		*pbones;
	mstudioseqdesc_t	*pseqdesc;
	mstudioanim_t		*panim;

	static vec3_t		pos[MAXSTUDIOBONES];
	static vec4_t		q[MAXSTUDIOBONES];
	float				bonematrix[3][4];

	static vec3_t		pos2[MAXSTUDIOBONES];
	static vec4_t		q2[MAXSTUDIOBONES];
	static vec3_t		pos3[MAXSTUDIOBONES];
	static vec4_t		q3[MAXSTUDIOBONES];
	static vec3_t		pos4[MAXSTUDIOBONES];
	static vec4_t		q4[MAXSTUDIOBONES];

	bool				isviewentity = false;

	if ( m_pCurrentEntity->curstate.sequence >= m_pStudioHeader->numseq )
		m_pCurrentEntity->curstate.sequence = 0;

	pseqdesc = ( mstudioseqdesc_t * )( ( byte * )m_pStudioHeader + m_pStudioHeader->seqindex ) + m_pCurrentEntity->curstate.sequence;

	f = StudioEstimateFrame( pseqdesc );

	panim = StudioGetAnim( m_pRenderModel, pseqdesc );
	StudioCalcRotations( pos, q, pseqdesc, panim, f );

	if ( m_fDoInterp &&
	     m_pCurrentEntity->latched.sequencetime &&
	     ( m_pCurrentEntity->latched.sequencetime + 0.2 > m_clTime ) &&
	     ( m_pCurrentEntity->latched.prevsequence < m_pStudioHeader->numseq ) )
	{
		static float	pos1b[MAXSTUDIOBONES][3];
		static vec4_t	q1b[MAXSTUDIOBONES];
		float			s;

		pseqdesc = ( mstudioseqdesc_t * )( ( byte * )m_pStudioHeader + m_pStudioHeader->seqindex ) + m_pCurrentEntity->latched.prevsequence;
		panim = StudioGetAnim( m_pRenderModel, pseqdesc );

		StudioCalcRotations( pos1b, q1b, pseqdesc, panim, m_pCurrentEntity->latched.prevframe );

		s = 1.0 - ( m_clTime - m_pCurrentEntity->latched.sequencetime ) / 0.2;
		StudioSlerpBones( q, pos, q1b, pos1b, s );
	}
	else
		m_pCurrentEntity->latched.prevframe = f;

	pbones = ( mstudiobone_t * )( ( byte * )m_pStudioHeader + m_pStudioHeader->boneindex );

	if ( m_pPlayerInfo && m_pPlayerInfo->gaitsequence > 0 )
	{
		if ( m_pPlayerInfo->gaitsequence >= m_pStudioHeader->numseq )
			m_pPlayerInfo->gaitsequence = 0;

		pseqdesc = ( mstudioseqdesc_t * )( ( byte * )m_pStudioHeader + m_pStudioHeader->seqindex ) + m_pPlayerInfo->gaitsequence;

		panim = StudioGetAnim( m_pRenderModel, pseqdesc );
		StudioCalcRotations( pos2, q2, pseqdesc, panim, m_pPlayerInfo->gaitframe );		// !!!

		for ( i = 0; i < m_pStudioHeader->numbones; i++ )
		{
			if ( !strcmp( pbones[i].name, "Scene Root" ) || 
				 !strcmp( pbones[i].name, "M-bone" ) || 
				 !strcmp( pbones[i].name, "M-bone Pelvis" ) || 
				 !strcmp( pbones[i].name, "M-bone L Thigh" ) || 
				 !strcmp( pbones[i].name, "M-bone L Calf" ) || 
				 !strcmp( pbones[i].name, "M-bone L Foot" ) || 
				 !strcmp( pbones[i].name, "M-bone L Toe0" ) || 
				 !strcmp( pbones[i].name, "M-bone R Thigh" ) || 
				 !strcmp( pbones[i].name, "M-bone R Calf" ) || 
				 !strcmp( pbones[i].name, "M-bone R Foot" ) || 
				 !strcmp( pbones[i].name, "M-bone R Toe0" ) )
			{
				memcpy( pos[i], pos2[i], sizeof( pos[i] ) );
				memcpy( q[i], q2[i], sizeof( q[i] ) );
			}
		}
	}

	if ( m_pCurrentEntity == IEngineStudio.GetViewEntity() )
		isviewentity = true;

	for ( i = 0; i < m_pStudioHeader->numbones; i++ )
	{
		QuaternionMatrix( q[i], bonematrix );

		// | X Y Z
		// | 1 0 0 X |
		// | 0 1 0 Y |
		// | 0 0 1 Z |

		bonematrix[0][3] = pos[i][0];
		bonematrix[1][3] = pos[i][1];
		bonematrix[2][3] = pos[i][2];

		if ( pbones[i].parent == -1 )
		{
			if ( isviewentity && m_pCvarRightHand->value )
			{
				bonematrix[1][0] = -bonematrix[1][0];
				bonematrix[1][1] = -bonematrix[1][1];
				bonematrix[1][2] = -bonematrix[1][2];
				bonematrix[1][3] = -bonematrix[1][3];
			}

			if ( IEngineStudio.IsHardware() )
			{
				ConcatTransforms( ( *m_protationmatrix ), bonematrix, ( *m_pbonetransform )[i] );
				MatrixCopy( ( *m_pbonetransform )[i], ( *m_plighttransform )[i] );
			}
			else
			{
				ConcatTransforms( ( *m_paliastransform ), bonematrix, ( *m_pbonetransform )[i] );
				ConcatTransforms( ( *m_protationmatrix ), bonematrix, ( *m_plighttransform )[i] );
			}

			StudioFxTransform( m_pCurrentEntity, ( *m_pbonetransform )[i] );
		}
		else
		{
			ConcatTransforms( ( *m_pbonetransform )[pbones[i].parent], bonematrix, ( *m_pbonetransform )[i] );
			ConcatTransforms( ( *m_plighttransform )[pbones[i].parent], bonematrix, ( *m_plighttransform )[i] );
		}
	}
}

void qgluPerspective( GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar )
{
	GLdouble xmin, xmax, ymin, ymax;

	ymax = zNear * tan( fovy * M_PI / 360.0 );
	ymin = -ymax;

	xmin = ymin * aspect;
	xmax = ymax * aspect;

	qglFrustum( xmin, xmax, ymin, ymax, zNear, zFar );
}

int CGameStudioModelRenderer::StudioDrawModel( int flags )
{
	alight_t		lighting;
	vec3_t			dir;
	SCREENINFO		screen;
	bool			gunmodel;
	float			fov;
	float			aspect;

	m_pCurrentEntity = IEngineStudio.GetCurrentEntity();

	IEngineStudio.GetTimes( &m_nFrameCount, &m_clTime, &m_clOldTime );
	IEngineStudio.GetViewInfo( m_vRenderOrigin, m_vUp, m_vRight, m_vNormal );
	IEngineStudio.GetAliasScale( &m_fSoftwareXScale, &m_fSoftwareYScale );

	if ( m_pCurrentEntity == IEngineStudio.GetViewEntity() )
		gunmodel = true;
	else
		gunmodel = false;

	if ( gunmodel )
	{
		fov = 45.0;
		aspect = 0.0;

		if ( ViewModel.iCharacterId )
		{
			CharacterData *pCharacter = CharacterData_GetData( ViewModel.iCharacterId );

			if ( pCharacter )
			{
				switch ( ViewModel.iTeam )
				{
					case TEAM_GR:
					{
						if ( pCharacter->bIsWoman )
							m_pCurrentEntity->curstate.body = 3;
						else
							m_pCurrentEntity->curstate.body = 1;

						break;
					}
					case TEAM_BL:
					{
						if ( pCharacter->bIsWoman )
							m_pCurrentEntity->curstate.body = 2;
						else
							m_pCurrentEntity->curstate.body = 0;

						break;
					}
				}
			}
		}

		if ( ViewModel.iWeaponId )
		{
			WeaponData *pWeapon = WeaponData_GetData( ViewModel.iWeaponId );

			if ( pWeapon )
			{
				if ( pWeapon->iWeaponClass == WeaponClass_Sniper && ClientState.iFOV != 90 )
					return 1;

				if ( 
					pWeapon->iWeaponClass == WeaponClass_Rifle || 
					pWeapon->iWeaponClass == WeaponClass_Pistol || 
					pWeapon->iWeaponClass == WeaponClass_Sniper )
				{
					if ( m_pCurrentEntity->curstate.sequence == pWeapon->m_AnimSelect.iSequence )
						m_pCurrentEntity->curstate.framerate = pWeapon->flChangeWeaponAnimRatio;

					if ( m_pCurrentEntity->curstate.sequence == pWeapon->m_AnimReload.iSequence )
						m_pCurrentEntity->curstate.framerate = pWeapon->flReloadAnimRatio;
				}
				else
				{
					m_pCurrentEntity->curstate.framerate = 1;
				}

				fov = pWeapon->m_ViewInfo.flFOV;
				aspect = pWeapon->m_ViewInfo.flAspect;
			}
		}

		if ( aspect <= 0 )
		{
			screen.iSize = sizeof( screen );
			gEngfuncs.pfnGetScreenInfo( &screen );

			aspect = ( float )screen.iWidth / ( float )screen.iHeight;
		}

		qglMatrixMode( GL_PROJECTION );
		qglPushMatrix();
		qglLoadIdentity();
		qgluPerspective( fov, aspect, 4.0, 8192.0 );
		qglMatrixMode( GL_MODELVIEW );
	}

	if ( m_pCurrentEntity->curstate.renderfx == kRenderFxDeadPlayer )
		return 1;

	m_pRenderModel = m_pCurrentEntity->model;
	m_pStudioHeader = ( studiohdr_t * )IEngineStudio.Mod_Extradata( m_pRenderModel );
	IEngineStudio.StudioSetHeader( m_pStudioHeader );
	IEngineStudio.SetRenderModel( m_pRenderModel );

	StudioSetUpTransform( 0 );

	if ( flags & STUDIO_RENDER )
	{
		if ( !IEngineStudio.StudioCheckBBox() )
			return 0;

		( *m_pModelsDrawn )++;
		( *m_pStudioModelCount )++;

		if ( m_pStudioHeader->numbodyparts == 0 )
			return 1;
	}

	if ( m_pCurrentEntity->curstate.movetype == MOVETYPE_FOLLOW )
		StudioMergeBones( m_pRenderModel );
	else
		StudioSetupBones();

	StudioSaveBones();

	if ( flags & STUDIO_EVENTS )
	{
		StudioCalcAttachments();
		IEngineStudio.StudioClientEvents();

		if ( m_pCurrentEntity->index > 0 )
		{
			cl_entity_t *ent = gEngfuncs.GetEntityByIndex( m_pCurrentEntity->index );
			memcpy( ent->attachment, m_pCurrentEntity->attachment, sizeof( vec3_t ) * 4 );
		}
	}

	if ( flags & STUDIO_RENDER )
	{
		lighting.plightvec = dir;

		IEngineStudio.StudioDynamicLight( m_pCurrentEntity, &lighting );
		IEngineStudio.StudioEntityLight( &lighting );

		IEngineStudio.StudioSetupLighting( &lighting );

		m_nTopColor = m_pCurrentEntity->curstate.colormap & 0xFF;
		m_nBottomColor = ( m_pCurrentEntity->curstate.colormap & 0xFF00 ) >> 8;

		IEngineStudio.StudioSetRemapColors( m_nTopColor, m_nBottomColor );

		StudioRenderModel();
	}

	if ( gunmodel )
	{
		qglMatrixMode( GL_PROJECTION );
		qglPopMatrix();
		qglMatrixMode( GL_MODELVIEW );
	}

	return 1;
}

int CGameStudioModelRenderer::StudioDrawPlayer( int flags, entity_state_t *pplayer )
{
	alight_t	lighting;
	vec3_t		dir;
	vec3_t		angles;

	m_pCurrentEntity = IEngineStudio.GetCurrentEntity();

	IEngineStudio.GetTimes( &m_nFrameCount, &m_clTime, &m_clOldTime );
	IEngineStudio.GetViewInfo( m_vRenderOrigin, m_vUp, m_vRight, m_vNormal );
	IEngineStudio.GetAliasScale( &m_fSoftwareXScale, &m_fSoftwareYScale );

	m_nPlayerIndex = pplayer->number - 1;

	if ( m_nPlayerIndex < 0 || m_nPlayerIndex >= gEngfuncs.GetMaxClients() )
		return 0;

	m_pRenderModel = m_pCurrentEntity->model;

	if ( m_pRenderModel == NULL )
		return 0;

	m_pStudioHeader = ( studiohdr_t * )IEngineStudio.Mod_Extradata( m_pRenderModel );
	IEngineStudio.StudioSetHeader( m_pStudioHeader );
	IEngineStudio.SetRenderModel( m_pRenderModel );

	if ( pplayer->gaitsequence > 0 )
	{
		m_pPlayerInfo = IEngineStudio.PlayerInfo( m_nPlayerIndex );

		VectorCopy( m_pCurrentEntity->curstate.angles, angles );

		// Make sure for the palyer
		m_pCurrentEntity->curstate.angles[PITCH] = 0.0;
		m_pCurrentEntity->angles[PITCH] = 0.0;

		StudioProcessGait( pplayer );

		StudioSetUpTransform( 0 );

		VectorCopy( angles, m_pCurrentEntity->curstate.angles );
		VectorCopy( angles, m_pCurrentEntity->angles );
	}
	else
	{
		StudioSetUpTransform( 0 );
	}

	if ( flags & STUDIO_RENDER )
	{
		if ( !IEngineStudio.StudioCheckBBox() )
			return 0;

		( *m_pModelsDrawn )++;
		( *m_pStudioModelCount )++;

		if ( m_pStudioHeader->numbodyparts == 0 )
			return 1;
	}

	m_pPlayerInfo = IEngineStudio.PlayerInfo( m_nPlayerIndex );

	// FIXME 10/1
	m_pPlayerInfo->gaitsequence = m_pCurrentEntity->curstate.gaitsequence;

	StudioSetupBones();
	StudioSaveBones();

	m_pPlayerInfo->renderframe = m_nFrameCount;
	m_pPlayerInfo = NULL;

	if ( flags & STUDIO_EVENTS )
	{
		StudioCalcAttachments();
		IEngineStudio.StudioClientEvents();

		if ( m_pCurrentEntity->index > 0 )
		{
			cl_entity_t *ent = gEngfuncs.GetEntityByIndex( m_pCurrentEntity->index );
			memcpy( ent->attachment, m_pCurrentEntity->attachment, sizeof( vec3_t ) * 4 );
		}
	}

	if ( flags & STUDIO_RENDER )
	{
		lighting.plightvec = dir;

		IEngineStudio.StudioDynamicLight( m_pCurrentEntity, &lighting );
		IEngineStudio.StudioEntityLight( &lighting );
		IEngineStudio.StudioSetupLighting( &lighting );

		m_pPlayerInfo = IEngineStudio.PlayerInfo( m_nPlayerIndex );

		m_nTopColor = m_pPlayerInfo->topcolor;
		m_nBottomColor = m_pPlayerInfo->bottomcolor;

		if ( m_nTopColor < 0 )
			m_nTopColor = 0;

		if ( m_nTopColor > 360 )
			m_nTopColor = 360;

		if ( m_nBottomColor < 0 )
			m_nBottomColor = 0;

		if ( m_nBottomColor > 360 )
			m_nBottomColor = 360;

		IEngineStudio.StudioSetRemapColors( m_nTopColor, m_nBottomColor );

		StudioRenderModel();
		m_pPlayerInfo = NULL;

		if ( pplayer->weaponmodel )
		{
			cl_entity_t saveent = *m_pCurrentEntity;

			model_t *pweaponmodel = IEngineStudio.GetModelByIndex( pplayer->weaponmodel );

			m_pStudioHeader = ( studiohdr_t * )IEngineStudio.Mod_Extradata( pweaponmodel );
			IEngineStudio.StudioSetHeader( m_pStudioHeader );

			StudioMergeBones( pweaponmodel );//实际上是SetupBones的过程

			IEngineStudio.StudioSetupLighting ( &lighting );

			StudioRenderModel();

			StudioCalcAttachments();

			*m_pCurrentEntity = saveent;
		}
	}

	return 1;
}

int gait_sequence[33] = { -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, -1 };

void CGameStudioModelRenderer::StudioProcessGait( entity_state_t *pplayer )
{
	mstudioseqdesc_t	*pseqdesc;
	double				dfdt, f;

	m_pPlayerInfo->gaitsequence = pplayer->gaitsequence;
	m_pPlayerInfo->gaitframe = 0;

	if ( m_pPlayerInfo->gaitsequence != gait_sequence[pplayer->number] )
	{
		gait_sequence[pplayer->number] = m_pPlayerInfo->gaitsequence;
		m_pPlayerInfo->gaitanimtime = m_clTime;
	}

	if ( m_pPlayerInfo->gaitsequence < 0 )
		m_pPlayerInfo->gaitsequence = 0;

	if ( m_pPlayerInfo->gaitsequence >= m_pStudioHeader->numseq )
		m_pPlayerInfo->gaitsequence = 0;

	pseqdesc = ( mstudioseqdesc_t * )( ( byte * )m_pStudioHeader + m_pStudioHeader->seqindex ) + m_pPlayerInfo->gaitsequence;

	if ( m_fDoInterp )
	{
		if ( m_clTime < m_pPlayerInfo->gaitanimtime )
			dfdt = 0;
		else
			dfdt = ( m_clTime - m_pPlayerInfo->gaitanimtime ) * 1 * pseqdesc->fps;
	}
	else
		dfdt = 0;

	if ( pseqdesc->numframes <= 1 )
		f = 0;
	else
		f = ( m_pPlayerInfo->gaitframe * ( pseqdesc->numframes - 1 ) ) / 256.0;

	f += dfdt;

	if ( pseqdesc->flags & STUDIO_LOOPING )
	{
		if ( pseqdesc->numframes > 1 )
			f -= ( int )( f / ( pseqdesc->numframes - 1 ) ) * ( pseqdesc->numframes - 1 );

		if ( f < 0 )
			f += ( pseqdesc->numframes - 1 );
	}
	else
	{
		if ( f >= pseqdesc->numframes - 1.001 )
			f = pseqdesc->numframes - 1.001;

		if ( f < 0.0 )
			f = 0.0;
	}

	m_pPlayerInfo->gaitframe = f;
}

int R_StudioDrawModel(int flags)
{
	return g_StudioRenderer.StudioDrawModel(flags);
}

int R_StudioDrawPlayer(int flags, entity_state_t *pplayer)
{
	return g_StudioRenderer.StudioDrawPlayer(flags, pplayer);
}

void R_StudioInit(void)
{
	g_StudioRenderer.Init();
}

// The simple drawing interface we'll pass back to the engine.
r_studio_interface_t studio = 
{
	STUDIO_INTERFACE_VERSION,
	R_StudioDrawModel,
	R_StudioDrawPlayer,
};

void R_MetaStudioInit(void);

extern "C"
{

BOOL DLLExport HUD_GetStudioModelInterface(int version, struct r_studio_interface_s **ppinterface, struct engine_studio_api_s *pstudio)
{
	if (version != STUDIO_INTERFACE_VERSION)
		return FALSE;
	
	// Point the engine to our callbacks.
	*ppinterface = &studio;
	
	// Copy in engine helper functions.
	memcpy(&IEngineStudio, pstudio, sizeof(IEngineStudio));
	
	// Initialize local variables, etc.
	R_StudioInit();
	R_MetaStudioInit();
	
	return TRUE;
}

}