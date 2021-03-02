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

// WARNING !!!  don't use Vector in SV_StudioSetupBones params !
#define NOVECTOR

#include "extdll.h"
#include "enginecallback.h"
#include "animation.h"
#include "studio.h"
#include "studio_util.h"
#include "com_model.h"
#include "r_studioint.h"
#include <metastudio.h>

#define ACTIVITY_NOT_AVAILABLE	-1

int LookupActivity( void *pmodel, entvars_t *pev, int activity )
{
	studiohdr_t *pstudiohdr;

	pstudiohdr = ( studiohdr_t * )pmodel;

	if ( ! pstudiohdr )
		return 0;

	mstudioseqdesc_t *pseqdesc;

	pseqdesc = ( mstudioseqdesc_t * )( ( byte * )pstudiohdr + pstudiohdr->seqindex );

	int weighttotal = 0;
	int seq = ACTIVITY_NOT_AVAILABLE;

	for ( int i = 0; i < pstudiohdr->numseq; i++ )
	{
		if ( pseqdesc[i].activity == activity )
		{
			weighttotal += pseqdesc[i].actweight;

			if ( !weighttotal || RANDOM_LONG( 0, weighttotal - 1 ) < pseqdesc[i].actweight )
				seq = i;
		}
	}

	return seq;
}

int LookupSequence( void *pmodel, const char *label )
{
	studiohdr_t *pstudiohdr;

	pstudiohdr = ( studiohdr_t * )pmodel;

	if ( !pstudiohdr )
		return -1;

	mstudioseqdesc_t *pseqdesc;

	pseqdesc = ( mstudioseqdesc_t * )( ( byte * )pstudiohdr + pstudiohdr->seqindex );

	for ( int i = 0; i < pstudiohdr->numseq; i++ )
	{
		if ( stricmp( pseqdesc[i].label, label ) == 0 )
			return i;
	}

	return -1;
}

void GetSequenceInfo( void *pmodel, entvars_t *pev, float *pflFrameRate, float *pflGroundSpeed )
{
	studiohdr_t *pstudiohdr;

	pstudiohdr = ( studiohdr_t * )pmodel;

	if ( ! pstudiohdr )
		return;

	mstudioseqdesc_t *pseqdesc;

	if ( pev->sequence >= pstudiohdr->numseq )
	{
		*pflFrameRate = 0.0;
		*pflGroundSpeed = 0.0;
		return;
	}

	pseqdesc = ( mstudioseqdesc_t * )( ( byte * )pstudiohdr + pstudiohdr->seqindex ) + ( int )pev->sequence;

	if ( pseqdesc->numframes > 1 )
	{
		*pflFrameRate = 256 * pseqdesc->fps / ( pseqdesc->numframes - 1 );
		*pflGroundSpeed = sqrt( pseqdesc->linearmovement[0] * pseqdesc->linearmovement[0] + pseqdesc->linearmovement[1] * pseqdesc->linearmovement[1] +
		                        pseqdesc->linearmovement[2] * pseqdesc->linearmovement[2] );
		*pflGroundSpeed = *pflGroundSpeed * pseqdesc->fps / ( pseqdesc->numframes - 1 );
	}
	else
	{
		*pflFrameRate = 256.0;
		*pflGroundSpeed = 0.0;
	}
}

int GetSequenceFlags( void *pmodel, entvars_t *pev )
{
	studiohdr_t *pstudiohdr;

	pstudiohdr = ( studiohdr_t * )pmodel;

	if ( !pstudiohdr || pev->sequence >= pstudiohdr->numseq )
		return 0;

	mstudioseqdesc_t	*pseqdesc;

	pseqdesc = ( mstudioseqdesc_t * )( ( byte * )pstudiohdr + pstudiohdr->seqindex ) + ( int )pev->sequence;

	return pseqdesc->flags;
}

float SetController( void *pmodel, entvars_t *pev, int iController, float flValue )
{
	studiohdr_t *pstudiohdr;

	pstudiohdr = ( studiohdr_t * )pmodel;

	if ( ! pstudiohdr )
		return flValue;

	mstudiobonecontroller_t	*pbonecontroller = ( mstudiobonecontroller_t * )( ( byte * )pstudiohdr + pstudiohdr->bonecontrollerindex );

	int i;

	for ( i = 0; i < pstudiohdr->numbonecontrollers; i++, pbonecontroller++ )
	{

		if ( pbonecontroller->index == iController )
			break;
	}

	if ( i >= pstudiohdr->numbonecontrollers )
		return flValue;

	if ( pbonecontroller->type & ( STUDIO_XR | STUDIO_YR | STUDIO_ZR ) )
	{
		if ( pbonecontroller->end < pbonecontroller->start )
			flValue = -flValue;

		if ( pbonecontroller->start + 359.0 >= pbonecontroller->end )
		{
			if ( flValue > ( ( pbonecontroller->start + pbonecontroller->end ) / 2.0 ) + 180 )
				flValue = flValue - 360;

			if ( flValue < ( ( pbonecontroller->start + pbonecontroller->end ) / 2.0 ) - 180 )
				flValue = flValue + 360;
		}
		else
		{
			if ( flValue > 360 )
				flValue = flValue - ( int )( flValue / 360.0 ) * 360.0;
			else if ( flValue < 0 )
				flValue = flValue + ( int )( ( flValue / -360.0 ) + 1 ) * 360.0;
		}
	}

	int setting = 255 * ( flValue - pbonecontroller->start ) / ( pbonecontroller->end - pbonecontroller->start );

	if ( setting < 0 ) setting = 0;

	if ( setting > 255 ) setting = 255;

	pev->controller[iController] = setting;

	return setting * ( 1.0 / 255.0 ) * ( pbonecontroller->end - pbonecontroller->start ) + pbonecontroller->start;
}

float SetBlending( void *pmodel, entvars_t *pev, int iBlender, float flValue )
{
	studiohdr_t *pstudiohdr;

	pstudiohdr = ( studiohdr_t * )pmodel;

	if ( ! pstudiohdr )
		return flValue;

	mstudioseqdesc_t *pseqdesc;

	pseqdesc = ( mstudioseqdesc_t * )( ( byte * )pstudiohdr + pstudiohdr->seqindex ) + ( int )pev->sequence;

	if ( pseqdesc->blendtype[iBlender] == 0 )
		return flValue;

	if ( pseqdesc->blendtype[iBlender] & ( STUDIO_XR | STUDIO_YR | STUDIO_ZR ) )
	{
		if ( pseqdesc->blendend[iBlender] < pseqdesc->blendstart[iBlender] )
			flValue = -flValue;

		if ( pseqdesc->blendstart[iBlender] + 359.0 >= pseqdesc->blendend[iBlender] )
		{
			if ( flValue > ( ( pseqdesc->blendstart[iBlender] + pseqdesc->blendend[iBlender] ) / 2.0 ) + 180 )
				flValue = flValue - 360;

			if ( flValue < ( ( pseqdesc->blendstart[iBlender] + pseqdesc->blendend[iBlender] ) / 2.0 ) - 180 )
				flValue = flValue + 360;
		}
	}

	int setting = 255 * ( flValue - pseqdesc->blendstart[iBlender] ) / ( pseqdesc->blendend[iBlender] - pseqdesc->blendstart[iBlender] );

	if ( setting < 0 ) setting = 0;

	if ( setting > 255 ) setting = 255;

	pev->blending[iBlender] = setting;

	return setting * ( 1.0 / 255.0 ) * ( pseqdesc->blendend[iBlender] - pseqdesc->blendstart[iBlender] ) + pseqdesc->blendstart[iBlender];
}

void SetBodygroup( void *pmodel, entvars_t *pev, int iGroup, int iValue )
{
	studiohdr_t *pstudiohdr;

	pstudiohdr = ( studiohdr_t * )pmodel;

	if ( ! pstudiohdr )
		return;

	if ( iGroup > pstudiohdr->numbodyparts )
		return;

	mstudiobodyparts_t *pbodypart = ( mstudiobodyparts_t * )( ( byte * )pstudiohdr + pstudiohdr->bodypartindex ) + iGroup;

	if ( iValue >= pbodypart->nummodels )
		return;

	int iCurrent = ( pev->body / pbodypart->base ) % pbodypart->nummodels;

	pev->body = ( pev->body - ( iCurrent * pbodypart->base ) + ( iValue * pbodypart->base ) );
}

int GetBodygroup( void *pmodel, entvars_t *pev, int iGroup )
{
	studiohdr_t *pstudiohdr;

	pstudiohdr = ( studiohdr_t * )pmodel;

	if ( ! pstudiohdr )
		return 0;

	if ( iGroup > pstudiohdr->numbodyparts )
		return 0;

	mstudiobodyparts_t *pbodypart = ( mstudiobodyparts_t * )( ( byte * )pstudiohdr + pstudiohdr->bodypartindex ) + iGroup;

	if ( pbodypart->nummodels <= 1 )
		return 0;

	int iCurrent = ( pev->body / pbodypart->base ) % pbodypart->nummodels;

	return iCurrent;
}

server_studio_api_t IEngineStudio;

studiohdr_t *pstudiohdr;

float ( *rotationmatrix )[3][4];
float ( *bonetransform )[MAXSTUDIOBONES][3][4];

int GetPlayerGaitsequence( const edict_t *pEdict );

mstudioanim_t *StudioGetAnim( model_t *psubmodel, mstudioseqdesc_t *pseqdesc )
{
	mstudioseqgroup_t	*pseqgroup;
	cache_user_t		*paSequences;

	pseqgroup = ( mstudioseqgroup_t * )( ( byte * )pstudiohdr + pstudiohdr->seqgroupindex ) + pseqdesc->seqgroup;

	if ( pseqdesc->seqgroup == 0 )
		return ( mstudioanim_t * )( ( byte * )pstudiohdr + pseqdesc->animindex );

	paSequences = ( cache_user_t * )psubmodel->submodels;

	if ( paSequences == NULL )
	{
		paSequences = ( cache_user_t * )IEngineStudio.Mem_Calloc( 16, sizeof( cache_user_t ) );
		psubmodel->submodels = ( dmodel_t * )paSequences;
	}

	if ( !IEngineStudio.Cache_Check( ( struct cache_user_s * ) & ( paSequences[pseqdesc->seqgroup] ) ) )
	{
		ALERT( at_console, "loading %s\n", pseqgroup->name );
		IEngineStudio.LoadCacheFile( pseqgroup->name, ( struct cache_user_s * )&paSequences[pseqdesc->seqgroup] );
	}

	return ( mstudioanim_t * )( ( byte * )paSequences[pseqdesc->seqgroup].data + pseqdesc->animindex );
}

void StudioCalcBoneAdj( float dadt, float *adj, const byte *pcontroller1, const byte *pcontroller2, byte mouthopen )
{
	int						i, j;
	float					value;
	mstudiobonecontroller_t	*pbonecontroller;

	pbonecontroller = ( mstudiobonecontroller_t * )( ( byte * )pstudiohdr + pstudiohdr->bonecontrollerindex );

	for ( j = 0; j < pstudiohdr->numbonecontrollers; j++ )
	{
		i = pbonecontroller[j].index;

		if ( i <= 3 )
		{
			if ( pbonecontroller[j].type & STUDIO_RLOOP )
			{
				if ( abs( pcontroller1[i] - pcontroller2[i] ) > 128 )
				{
					int a, b;
					a = ( pcontroller1[j] + 128 ) % 256;
					b = ( pcontroller2[j] + 128 ) % 256;
					value = ( ( a * dadt ) + ( b * ( 1 - dadt ) ) - 128 ) * ( 360.0 / 256.0 ) + pbonecontroller[j].start;
				}
				else
					value = ( ( pcontroller1[i] * dadt + ( pcontroller2[i] ) * ( 1.0 - dadt ) ) ) * ( 360.0 / 256.0 ) + pbonecontroller[j].start;
			}
			else
			{
				value = ( pcontroller1[i] * dadt + pcontroller2[i] * ( 1.0 - dadt ) ) / 255.0;

				if ( value < 0 ) value = 0;

				if ( value > 1.0 ) value = 1.0;

				value = ( 1.0 - value ) * pbonecontroller[j].start + value * pbonecontroller[j].end;
			}
		}
		else
		{
			value = mouthopen / 64.0;

			if ( value > 1.0 ) value = 1.0;

			value = ( 1.0 - value ) * pbonecontroller[j].start + value * pbonecontroller[j].end;
		}

		switch ( pbonecontroller[j].type & STUDIO_TYPES )
		{
			case STUDIO_XR:
			case STUDIO_YR:
			case STUDIO_ZR:
				adj[j] = value * ( M_PI / 180.0 );
				break;

			case STUDIO_X:
			case STUDIO_Y:
			case STUDIO_Z:
				adj[j] = value;
				break;
		}
	}
}

void StudioCalcBoneQuaterion( int frame, float s, mstudiobone_t *pbone, mstudioanim_t *panim, float *adj, float *q )
{
	int					j, k;
	vec4_t				q1, q2;
	vec3_t				angle1, angle2;
	mstudioanimvalue_t	*panimvalue;

	for ( j = 0; j < 3; j++ )
	{
		if ( panim->offset[j + 3] == 0 )
			angle2[j] = angle1[j] = pbone->value[j + 3];
		else
		{
			panimvalue = ( mstudioanimvalue_t * )( ( byte * )panim + panim->offset[j + 3] );

			k = frame;

			if ( panimvalue->num.total < panimvalue->num.valid )
				k = 0;

			while ( panimvalue->num.total <= k )
			{
				k -= panimvalue->num.total;
				panimvalue += panimvalue->num.valid + 1;

				if ( panimvalue->num.total < panimvalue->num.valid )
					k = 0;
			}

			if ( panimvalue->num.valid > k )
			{
				angle1[j] = panimvalue[k + 1].value;

				if ( panimvalue->num.valid > k + 1 )
					angle2[j] = panimvalue[k + 2].value;
				else
				{
					if ( panimvalue->num.total > k + 1 )
						angle2[j] = angle1[j];
					else
						angle2[j] = panimvalue[panimvalue->num.valid + 2].value;
				}
			}
			else
			{
				angle1[j] = panimvalue[panimvalue->num.valid].value;

				if ( panimvalue->num.total > k + 1 )
					angle2[j] = angle1[j];
				else
					angle2[j] = panimvalue[panimvalue->num.valid + 2].value;
			}

			angle1[j] = pbone->value[j + 3] + angle1[j] * pbone->scale[j + 3];
			angle2[j] = pbone->value[j + 3] + angle2[j] * pbone->scale[j + 3];
		}

		if ( pbone->bonecontroller[j + 3] != -1 )
		{
			angle1[j] += adj[pbone->bonecontroller[j + 3]];
			angle2[j] += adj[pbone->bonecontroller[j + 3]];
		}
	}

	if ( !VectorCompare( angle1, angle2 ) )
	{
		AngleQuaternion( angle1, q1 );
		AngleQuaternion( angle2, q2 );
		QuaternionSlerp( q1, q2, s, q );
	}
	else
		AngleQuaternion( angle1, q );
}

void StudioCalcBonePosition( int frame, float s, mstudiobone_t *pbone, mstudioanim_t *panim, float *adj, float *pos )
{
	int					j, k;
	mstudioanimvalue_t	*panimvalue;

	for ( j = 0; j < 3; j++ )
	{
		pos[j] = pbone->value[j];

		if ( panim->offset[j] != 0 )
		{
			panimvalue = ( mstudioanimvalue_t * )( ( byte * )panim + panim->offset[j] );

			k = frame;

			if ( panimvalue->num.total < panimvalue->num.valid )
				k = 0;

			while ( panimvalue->num.total <= k )
			{
				k -= panimvalue->num.total;
				panimvalue += panimvalue->num.valid + 1;

				if ( panimvalue->num.total < panimvalue->num.valid )
					k = 0;
			}

			if ( panimvalue->num.valid > k )
			{
				if ( panimvalue->num.valid > k + 1 )
					pos[j] += ( panimvalue[k + 1].value * ( 1.0 - s ) + s * panimvalue[k + 2].value ) * pbone->scale[j];
				else
					pos[j] += panimvalue[k + 1].value * pbone->scale[j];
			}
			else
			{
				if ( panimvalue->num.total <= k + 1 )
					pos[j] += ( panimvalue[panimvalue->num.valid].value * ( 1.0 - s ) + s * panimvalue[panimvalue->num.valid + 2].value ) * pbone->scale[j];
				else
					pos[j] += panimvalue[panimvalue->num.valid].value * pbone->scale[j];
			}
		}

		if ( pbone->bonecontroller[j] != -1 && adj )
			pos[j] += adj[pbone->bonecontroller[j]];
	}
}

void StudioSlerpBones( vec4_t q1[], float pos1[][3], vec4_t q2[], float pos2[][3], float s )
{
	int			i;
	vec4_t		q3;
	float		s1;

	if ( s < 0 )
		s = 0;
	else if ( s > 1.0 )
		s = 1.0;

	s1 = 1.0 - s;

	for ( i = 0; i < pstudiohdr->numbones; i++ )
	{
		QuaternionSlerp( q1[i], q2[i], s, q3 );

		q1[i][0] = q3[0];
		q1[i][1] = q3[1];
		q1[i][2] = q3[2];
		q1[i][3] = q3[3];

		pos1[i][0] = pos1[i][0] * s1 + pos2[i][0] * s;
		pos1[i][1] = pos1[i][1] * s1 + pos2[i][1] * s;
		pos1[i][2] = pos1[i][2] * s1 + pos2[i][2] * s;
	}
}

void SV_StudioSetupBones( model_t *pModel, float frame, int sequence, const vec3_t angles, const vec3_t origin, const byte *pcontroller,
                          const byte *pblending, int iBone, const edict_t *edict )
{
	int					i;
	double				f, s;
	float				adj[MAXSTUDIOCONTROLLERS];

	mstudiobone_t		*pbones;
	mstudioseqdesc_t	*pseqdesc;
	mstudioanim_t		*panim;

	static float		pos[MAXSTUDIOBONES][3];
	static vec4_t		q[MAXSTUDIOBONES];
	static float		pos2[MAXSTUDIOBONES][3];
	static vec4_t		q2[MAXSTUDIOBONES];

	float				bonematrix[3][4];

	vec3_t				angles2;

	int					gaitsequence;
	float				gaitframe;

	pstudiohdr = ( studiohdr_t * )IEngineStudio.Mod_Extradata( pModel );

	if (pstudiohdr->flags & META_STUDIO)
		return;

	if ( sequence < 0 || sequence >= pstudiohdr->numseq )
	{
		ALERT( at_console, "SV_StudioSetupBones:  sequence %i/%i out of range for model %s\n", sequence, pstudiohdr->numseq, pstudiohdr->name );
		sequence = 0;
	}

	pbones = ( mstudiobone_t * )( ( byte * )pstudiohdr + pstudiohdr->boneindex );

	pseqdesc = ( mstudioseqdesc_t * )( ( byte * )pstudiohdr + pstudiohdr->seqindex ) + sequence;
	panim = StudioGetAnim( pModel, pseqdesc );

	if ( pseqdesc->numframes <= 1 )
		f = 0;
	else
		f = ( frame * ( pseqdesc->numframes - 1 ) ) / 256.0;

	s = f - ( int )f;

	StudioCalcBoneAdj( 0, adj, pcontroller, pcontroller, 0 );

	for ( i = 0; i < pstudiohdr->numbones; i++ )
	{
		StudioCalcBoneQuaterion( frame, s, &pbones[i], &panim[i], adj, q[i] );
		StudioCalcBonePosition( frame, s, &pbones[i], &panim[i], adj, pos[i] );
	}

	gaitsequence = GetPlayerGaitsequence( edict );
	gaitframe = 0;

	if ( gaitsequence > 0 )
	{
		if ( gaitsequence >= pstudiohdr->numseq )
			gaitsequence = 0;

		pseqdesc = ( mstudioseqdesc_t * )( ( byte * )pstudiohdr + pstudiohdr->seqindex ) + gaitsequence;
		panim = StudioGetAnim( pModel, pseqdesc );

		for ( i = 0; i < pstudiohdr->numbones; i++ )
		{
			StudioCalcBoneQuaterion( gaitframe, 0, &pbones[i], &panim[i], adj, q2[i] );
			StudioCalcBonePosition( gaitframe, 0, &pbones[i], &panim[i], adj, pos2[i] );
		}

		for ( i = 0; i < pstudiohdr->numbones; i++ )
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

	VectorCopy( angles, angles2 );

	if ( gaitsequence > 0 )
	{
		// Make sure for the player
		angles2[PITCH] = 0.0;
	}

	AngleMatrix( angles2, ( *rotationmatrix ) );

	( *rotationmatrix )[0][3] = origin[0];
	( *rotationmatrix )[1][3] = origin[1];
	( *rotationmatrix )[2][3] = origin[2];

	for ( i = 0; i < pstudiohdr->numbones; i++ )
	{
		QuaternionMatrix( q[i], bonematrix );

		bonematrix[0][3] = pos[i][0];
		bonematrix[1][3] = pos[i][1];
		bonematrix[2][3] = pos[i][2];

		if ( pbones[i].parent == -1 )
			ConcatTransforms( ( *rotationmatrix ), bonematrix, ( *bonetransform )[i] );
		else
			ConcatTransforms( ( *bonetransform )[pbones[i].parent], bonematrix, ( *bonetransform )[i] );
	}
}

sv_blending_interface_t svBlending =
{
	SV_BLENDING_INTERFACE_VERSION,
	SV_StudioSetupBones,
};

extern "C"
{

BOOL DLLExport Server_GetBlendingInterface( int version, sv_blending_interface_t **pinterface, server_studio_api_t *pstudio, float ***rotationmatrix,
	    float ** **bonetransform )
{
	if ( version != SV_BLENDING_INTERFACE_VERSION )
		return FALSE;

	// Point the engine to our callbacks.
	*pinterface = &svBlending;

	// Copy in engine helper functions.
	IEngineStudio.Mem_Calloc = pstudio->Mem_Calloc;
	IEngineStudio.Cache_Check = pstudio->Cache_Check;
	IEngineStudio.LoadCacheFile = pstudio->LoadCacheFile;
	IEngineStudio.Mod_Extradata = pstudio->Mod_Extradata;

	::rotationmatrix = ( float ( * )[3][4] )rotationmatrix;
	::bonetransform = ( float ( * )[MAXSTUDIOBONES][3][4] )bonetransform;

	return TRUE;
}

}