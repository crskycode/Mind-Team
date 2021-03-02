/***
*
*	Copyright (c) 1996-2002, Valve LLC. All rights reserved.
*	
*	This product contains software technology licensed from Id 
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc. 
*	All Rights Reserved.
*
****/
// studio_render.cpp: routines for drawing Half-Life 3DStudio models
// updates:
// 1-4-99	fixed AdvanceFrame wraping bug

#include <windows.h>

#include <gl\gl.h>
#include <gl\glu.h>
#include "glext.h"

#pragma warning( disable : 4244 ) // double to float


extern "C"
{
extern void (APIENTRY *glDeleteObjectARB)(GLhandleARB obj);
extern GLhandleARB (APIENTRY *glGetHandleARB)(GLenum pname);
extern void (APIENTRY *glDetachObjectARB)(GLhandleARB containerObj, GLhandleARB attachedObj);
extern GLhandleARB (APIENTRY *glCreateShaderObjectARB)(GLenum shaderType);
extern void (APIENTRY *glShaderSourceARB)(GLhandleARB shaderObj, GLsizei count, const GLcharARB **string, const GLint *length);
extern void (APIENTRY *glCompileShaderARB)(GLhandleARB shaderObj);
extern GLhandleARB (APIENTRY *glCreateProgramObjectARB)(void);
extern void (APIENTRY *glAttachObjectARB)(GLhandleARB containerObj, GLhandleARB obj);
extern void (APIENTRY *glLinkProgramARB)(GLhandleARB programObj);
extern void (APIENTRY *glUseProgramObjectARB)(GLhandleARB programObj);
extern void (APIENTRY *glValidateProgramARB)(GLhandleARB programObj);
extern void (APIENTRY *glUniform1fARB)(GLint location, GLfloat v0);
extern void (APIENTRY *glUniform2fARB)(GLint location, GLfloat v0, GLfloat v1);
extern void (APIENTRY *glUniform3fARB)(GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
extern void (APIENTRY *glUniform4fARB)(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
extern void (APIENTRY *glUniform1iARB)(GLint location, GLint v0);
extern void (APIENTRY *glUniform2iARB)(GLint location, GLint v0, GLint v1);
extern void (APIENTRY *glUniform3iARB)(GLint location, GLint v0, GLint v1, GLint v2);
extern void (APIENTRY *glUniform4iARB)(GLint location, GLint v0, GLint v1, GLint v2, GLint v3);
extern void (APIENTRY *glUniform1fvARB)(GLint location, GLsizei count, const GLfloat *value);
extern void (APIENTRY *glUniform2fvARB)(GLint location, GLsizei count, const GLfloat *value);
extern void (APIENTRY *glUniform3fvARB)(GLint location, GLsizei count, const GLfloat *value);
extern void (APIENTRY *glUniform4fvARB)(GLint location, GLsizei count, const GLfloat *value);
extern void (APIENTRY *glUniform1ivARB)(GLint location, GLsizei count, const GLint *value);
extern void (APIENTRY *glUniform2ivARB)(GLint location, GLsizei count, const GLint *value);
extern void (APIENTRY *glUniform3ivARB)(GLint location, GLsizei count, const GLint *value);
extern void (APIENTRY *glUniform4ivARB)(GLint location, GLsizei count, const GLint *value);
extern void (APIENTRY *glUniformMatrix2fvARB)(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
extern void (APIENTRY *glUniformMatrix3fvARB)(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
extern void (APIENTRY *glUniformMatrix4fvARB)(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
extern void (APIENTRY *glGetObjectParameterfvARB)(GLhandleARB obj, GLenum pname, GLfloat *params);
extern void (APIENTRY *glGetObjectParameterivARB)(GLhandleARB obj, GLenum pname, GLint *params);
extern void (APIENTRY *glGetInfoLogARB)(GLhandleARB obj, GLsizei maxLength, GLsizei *length, GLcharARB *infoLog);
extern void (APIENTRY *glGetAttachedObjectsARB)(GLhandleARB containerObj, GLsizei maxCount, GLsizei *count, GLhandleARB *obj);
extern GLint (APIENTRY *glGetUniformLocationARB)(GLhandleARB programObj, const GLcharARB *name);
extern void (APIENTRY *glGetActiveUniformARB)(GLhandleARB programObj, GLuint index, GLsizei maxLength, GLsizei *length, GLint *size, GLenum *type, GLcharARB *name);
extern void (APIENTRY *glGetUniformfvARB)(GLhandleARB programObj, GLint location, GLfloat *params);
extern void (APIENTRY *glGetUniformivARB)(GLhandleARB programObj, GLint location, GLint *params);
extern void (APIENTRY *glGetShaderSourceARB)(GLhandleARB obj, GLsizei maxLength, GLsizei *length, GLcharARB *source);

void (APIENTRY *glActiveTextureARB)(GLenum texture) = NULL;

extern void (APIENTRY *glVertexAttrib3fARB)(GLuint index, GLfloat x, GLfloat y, GLfloat z);
extern GLint (APIENTRY *glGetAttribLocationARB)(GLhandleARB programObj, const GLcharARB *name);
}

extern int g_StudioShaderProgram;
extern int g_StudioDiffuseMap;
extern int g_StudioSpecularMap;
extern int g_StudioEnvCubeMap;
extern int g_StudioNormalMap;

extern float transx, transy, transz;

////////////////////////////////////////////////////////////////////////

#include "mathlib.h"
#include "..\..\engine\studio.h"
#include "mdlviewer.h"
#include "vector3d.h"
#include <vector>
#include <map>
#include <algorithm>
extern vec3_t light_pos;

////////////////////////////////////////////////////////////////////////

vec3_t			g_xformverts[4096];	// transformed vertices
vec3_t			g_lightvalues[4096];	// light surface normals
vec3_t			*g_pxformverts;
vec3_t			*g_pvlightvalues;
vec3_t			g_studionormal[4096];
vec3_t			g_studiotangent[4096];
vec3_t			g_studiobitangent[4096];

vec3_t			g_lightvec;						// light vector in model reference frame
vec3_t			g_blightvec[MAXSTUDIOBONES];	// light vectors in bone reference frames
int				g_ambientlight;					// ambient world light
float			g_shadelight;					// direct world light
vec3_t			g_lightcolor;

int				g_smodels_total;				// cookie

float			g_bonetransform[MAXSTUDIOBONES][3][4];	// bone transformation matrix

int				g_chrome[MAXSTUDIOVERTS][2];	// texture coords for surface normals
int				g_chromeage[MAXSTUDIOBONES];	// last time chrome vectors were updated
vec3_t			g_chromeup[MAXSTUDIOBONES];		// chrome vector "up" in bone reference frames
vec3_t			g_chromeright[MAXSTUDIOBONES];	// chrome vector "right" in bone reference frames

////////////////////////////////////////////////////////////////////////

void StudioModel::CalcBoneAdj( )
{
	int					i, j;
	float				value;
	mstudiobonecontroller_t *pbonecontroller;
	
	pbonecontroller = (mstudiobonecontroller_t *)((byte *)m_pstudiohdr + m_pstudiohdr->bonecontrollerindex);

	for (j = 0; j < m_pstudiohdr->numbonecontrollers; j++)
	{
		i = pbonecontroller[j].index;
		if (i <= 3)
		{
			// check for 360% wrapping
			if (pbonecontroller[j].type & STUDIO_RLOOP)
			{
				value = m_controller[i] * (360.0/256.0) + pbonecontroller[j].start;
			}
			else 
			{
				value = m_controller[i] / 255.0;
				if (value < 0) value = 0;
				if (value > 1.0) value = 1.0;
				value = (1.0 - value) * pbonecontroller[j].start + value * pbonecontroller[j].end;
			}
			// Con_DPrintf( "%d %d %f : %f\n", m_controller[j], m_prevcontroller[j], value, dadt );
		}
		else
		{
			value = m_mouth / 64.0;
			if (value > 1.0) value = 1.0;
			value = (1.0 - value) * pbonecontroller[j].start + value * pbonecontroller[j].end;
			// Con_DPrintf("%d %f\n", mouthopen, value );
		}
		switch(pbonecontroller[j].type & STUDIO_TYPES)
		{
		case STUDIO_XR:
		case STUDIO_YR:
		case STUDIO_ZR:
			m_adj[j] = value * (Q_PI / 180.0);
			break;
		case STUDIO_X:
		case STUDIO_Y:
		case STUDIO_Z:
			m_adj[j] = value;
			break;
		}
	}
}


void StudioModel::CalcBoneQuaternion( int frame, float s, mstudiobone_t *pbone, mstudioanim_t *panim, float *q )
{
	int					j, k;
	vec4_t				q1, q2;
	vec3_t				angle1, angle2;
	mstudioanimvalue_t	*panimvalue;

	for (j = 0; j < 3; j++)
	{
		if (panim->offset[j+3] == 0)
		{
			angle2[j] = angle1[j] = pbone->value[j+3]; // default;
		}
		else
		{
			panimvalue = (mstudioanimvalue_t *)((byte *)panim + panim->offset[j+3]);
			k = frame;
			while (panimvalue->num.total <= k)
			{
				k -= panimvalue->num.total;
				panimvalue += panimvalue->num.valid + 1;
			}
			// Bah, missing blend!
			if (panimvalue->num.valid > k)
			{
				angle1[j] = panimvalue[k+1].value;

				if (panimvalue->num.valid > k + 1)
				{
					angle2[j] = panimvalue[k+2].value;
				}
				else
				{
					if (panimvalue->num.total > k + 1)
						angle2[j] = angle1[j];
					else
						angle2[j] = panimvalue[panimvalue->num.valid+2].value;
				}
			}
			else
			{
				angle1[j] = panimvalue[panimvalue->num.valid].value;
				if (panimvalue->num.total > k + 1)
				{
					angle2[j] = angle1[j];
				}
				else
				{
					angle2[j] = panimvalue[panimvalue->num.valid + 2].value;
				}
			}
			angle1[j] = pbone->value[j+3] + angle1[j] * pbone->scale[j+3];
			angle2[j] = pbone->value[j+3] + angle2[j] * pbone->scale[j+3];
		}

		if (pbone->bonecontroller[j+3] != -1)
		{
			angle1[j] += m_adj[pbone->bonecontroller[j+3]];
			angle2[j] += m_adj[pbone->bonecontroller[j+3]];
		}
	}

	if (!VectorCompare( angle1, angle2 ))
	{
		AngleQuaternion( angle1, q1 );
		AngleQuaternion( angle2, q2 );
		QuaternionSlerp( q1, q2, s, q );
	}
	else
	{
		AngleQuaternion( angle1, q );
	}
}


void StudioModel::CalcBonePosition( int frame, float s, mstudiobone_t *pbone, mstudioanim_t *panim, float *pos )
{
	int					j, k;
	mstudioanimvalue_t	*panimvalue;

	for (j = 0; j < 3; j++)
	{
		pos[j] = pbone->value[j]; // default;
		if (panim->offset[j] != 0)
		{
			panimvalue = (mstudioanimvalue_t *)((byte *)panim + panim->offset[j]);
			
			k = frame;
			// find span of values that includes the frame we want
			while (panimvalue->num.total <= k)
			{
				k -= panimvalue->num.total;
				panimvalue += panimvalue->num.valid + 1;
			}
			// if we're inside the span
			if (panimvalue->num.valid > k)
			{
				// and there's more data in the span
				if (panimvalue->num.valid > k + 1)
				{
					pos[j] += (panimvalue[k+1].value * (1.0 - s) + s * panimvalue[k+2].value) * pbone->scale[j];
				}
				else
				{
					pos[j] += panimvalue[k+1].value * pbone->scale[j];
				}
			}
			else
			{
				// are we at the end of the repeating values section and there's another section with data?
				if (panimvalue->num.total <= k + 1)
				{
					pos[j] += (panimvalue[panimvalue->num.valid].value * (1.0 - s) + s * panimvalue[panimvalue->num.valid + 2].value) * pbone->scale[j];
				}
				else
				{
					pos[j] += panimvalue[panimvalue->num.valid].value * pbone->scale[j];
				}
			}
		}
		if (pbone->bonecontroller[j] != -1)
		{
			pos[j] += m_adj[pbone->bonecontroller[j]];
		}
	}
}


void StudioModel::CalcRotations ( vec3_t *pos, vec4_t *q, mstudioseqdesc_t *pseqdesc, mstudioanim_t *panim, float f )
{
	int					i;
	int					frame;
	mstudiobone_t		*pbone;
	float				s;

	frame = (int)f;
	s = (f - frame);

	// add in programatic controllers
	CalcBoneAdj( );

	pbone		= (mstudiobone_t *)((byte *)m_pstudiohdr + m_pstudiohdr->boneindex);
	for (i = 0; i < m_pstudiohdr->numbones; i++, pbone++, panim++) 
	{
		CalcBoneQuaternion( frame, s, pbone, panim, q[i] );
		CalcBonePosition( frame, s, pbone, panim, pos[i] );
	}

	if (pseqdesc->motiontype & STUDIO_X)
		pos[pseqdesc->motionbone][0] = 0.0;
	if (pseqdesc->motiontype & STUDIO_Y)
		pos[pseqdesc->motionbone][1] = 0.0;
	if (pseqdesc->motiontype & STUDIO_Z)
		pos[pseqdesc->motionbone][2] = 0.0;
}


mstudioanim_t * StudioModel::GetAnim( mstudioseqdesc_t *pseqdesc )
{
	mstudioseqgroup_t	*pseqgroup;
	pseqgroup = (mstudioseqgroup_t *)((byte *)m_pstudiohdr + m_pstudiohdr->seqgroupindex) + pseqdesc->seqgroup;

	if (pseqdesc->seqgroup == 0)
	{
		return (mstudioanim_t *)((byte *)m_pstudiohdr + pseqgroup->data + pseqdesc->animindex);
	}

	return (mstudioanim_t *)((byte *)m_panimhdr[pseqdesc->seqgroup] + pseqdesc->animindex);
}


void StudioModel::SlerpBones( vec4_t q1[], vec3_t pos1[], vec4_t q2[], vec3_t pos2[], float s )
{
	int			i;
	vec4_t		q3;
	float		s1;

	if (s < 0) s = 0;
	else if (s > 1.0) s = 1.0;

	s1 = 1.0 - s;

	for (i = 0; i < m_pstudiohdr->numbones; i++)
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


void StudioModel::AdvanceFrame( float dt )
{
	mstudioseqdesc_t	*pseqdesc;
	pseqdesc = (mstudioseqdesc_t *)((byte *)m_pstudiohdr + m_pstudiohdr->seqindex) + m_sequence;

	if (dt > 0.1)
		dt = (float)0.1;
	m_frame += dt * pseqdesc->fps;

	if (pseqdesc->numframes <= 1)
	{
		m_frame = 0;
	}
	else
	{
		// wrap
		m_frame -= (int)(m_frame / (pseqdesc->numframes - 1)) * (pseqdesc->numframes - 1);
	}
}


void StudioModel::SetUpBones ( void )
{
	int					i;

	mstudiobone_t		*pbones;
	mstudioseqdesc_t	*pseqdesc;
	mstudioanim_t		*panim;

	static vec3_t		pos[MAXSTUDIOBONES];
	float				bonematrix[3][4];
	static vec4_t		q[MAXSTUDIOBONES];

	static vec3_t		pos2[MAXSTUDIOBONES];
	static vec4_t		q2[MAXSTUDIOBONES];
	static vec3_t		pos3[MAXSTUDIOBONES];
	static vec4_t		q3[MAXSTUDIOBONES];
	static vec3_t		pos4[MAXSTUDIOBONES];
	static vec4_t		q4[MAXSTUDIOBONES];


	if (m_sequence >=  m_pstudiohdr->numseq) {
		m_sequence = 0;
	}

	pseqdesc = (mstudioseqdesc_t *)((byte *)m_pstudiohdr + m_pstudiohdr->seqindex) + m_sequence;

	panim = GetAnim( pseqdesc );
	CalcRotations( pos, q, pseqdesc, panim, m_frame );

	if (pseqdesc->numblends > 1)
	{
		float				s;

		panim += m_pstudiohdr->numbones;
		CalcRotations( pos2, q2, pseqdesc, panim, m_frame );
		s = m_blending[0] / 255.0;

		SlerpBones( q, pos, q2, pos2, s );

		if (pseqdesc->numblends == 4)
		{
			panim += m_pstudiohdr->numbones;
			CalcRotations( pos3, q3, pseqdesc, panim, m_frame );

			panim += m_pstudiohdr->numbones;
			CalcRotations( pos4, q4, pseqdesc, panim, m_frame );

			s = m_blending[0] / 255.0;
			SlerpBones( q3, pos3, q4, pos4, s );

			s = m_blending[1] / 255.0;
			SlerpBones( q, pos, q3, pos3, s );
		}
	}

	pbones = (mstudiobone_t *)((byte *)m_pstudiohdr + m_pstudiohdr->boneindex);

	for (i = 0; i < m_pstudiohdr->numbones; i++) {
		QuaternionMatrix( q[i], bonematrix );

		bonematrix[0][3] = pos[i][0];
		bonematrix[1][3] = pos[i][1];
		bonematrix[2][3] = pos[i][2];

		if (pbones[i].parent == -1) {
			memcpy(g_bonetransform[i], bonematrix, sizeof(float) * 12);
		} 
		else {
			R_ConcatTransforms (g_bonetransform[pbones[i].parent], bonematrix, g_bonetransform[i]);
		}
	}
}



/*
================
StudioModel::TransformFinalVert
================
*/
void StudioModel::Lighting (float *lv, int bone, int flags, vec3_t normal)
{
	float 	illum;
	float	lightcos;

	illum = g_ambientlight;

	if (flags & STUDIO_NF_FLATSHADE)
	{
		illum += g_shadelight * 0.8;
	} 
	else 
	{
		float r;
		lightcos = DotProduct (normal, g_blightvec[bone]); // -1 colinear, 1 opposite

		if (lightcos > 1.0)
			lightcos = 1;

		illum += g_shadelight;

		r = g_lambert;
		if (r <= 1.0) r = 1.0;

		lightcos = (lightcos + (r - 1.0)) / r; 		// do modified hemispherical lighting
		if (lightcos > 0.0) 
		{
			illum -= g_shadelight * lightcos; 
		}
		if (illum <= 0)
			illum = 0;
	}

	if (illum > 255) 
		illum = 255;
	*lv = illum / 255.0;	// Light from 0 to 1.0
}


void StudioModel::Chrome (int *pchrome, int bone, vec3_t normal)
{
	float n;

	if (g_chromeage[bone] != g_smodels_total)
	{
		// calculate vectors from the viewer to the bone. This roughly adjusts for position
		vec3_t chromeupvec;		// g_chrome t vector in world reference frame
		vec3_t chromerightvec;	// g_chrome s vector in world reference frame
		vec3_t tmp;				// vector pointing at bone in world reference frame
		VectorScale( m_origin, -1, tmp );
		tmp[0] += g_bonetransform[bone][0][3];
		tmp[1] += g_bonetransform[bone][1][3];
		tmp[2] += g_bonetransform[bone][2][3];
		VectorNormalize( tmp );
		CrossProduct( tmp, g_vright, chromeupvec );
		VectorNormalize( chromeupvec );
		CrossProduct( tmp, chromeupvec, chromerightvec );
		VectorNormalize( chromerightvec );

		VectorIRotate( chromeupvec, g_bonetransform[bone], g_chromeup[bone] );
		VectorIRotate( chromerightvec, g_bonetransform[bone], g_chromeright[bone] );

		g_chromeage[bone] = g_smodels_total;
	}

	// calc s coord
	n = DotProduct( normal, g_chromeright[bone] );
	pchrome[0] = (n + 1.0) * 32; // FIX: make this a float

	// calc t coord
	n = DotProduct( normal, g_chromeup[bone] );
	pchrome[1] = (n + 1.0) * 32; // FIX: make this a float
}


/*
================
StudioModel::SetupLighting
	set some global variables based on entity position
inputs:
outputs:
	g_ambientlight
	g_shadelight
================
*/
void StudioModel::SetupLighting ( )
{
	int i;
	g_ambientlight = 32;
	g_shadelight = 192;

	g_lightvec[0] = 0;
	g_lightvec[1] = 0;
	g_lightvec[2] = -1.0;

	g_lightcolor[0] = 1.0;
	g_lightcolor[1] = 1.0;
	g_lightcolor[2] = 1.0;

	// TODO: only do it for bones that actually have textures
	for (i = 0; i < m_pstudiohdr->numbones; i++)
	{
		VectorIRotate( g_lightvec, g_bonetransform[i], g_blightvec[i] );
	}
}


/*
=================
StudioModel::SetupModel
	based on the body part, figure out which mesh it should be using.
inputs:
	currententity
outputs:
	pstudiomesh
	pmdl
=================
*/

void StudioModel::SetupModel ( int bodypart )
{
	int index;

	if (bodypart > m_pstudiohdr->numbodyparts)
	{
		// Con_DPrintf ("StudioModel::SetupModel: no such bodypart %d\n", bodypart);
		bodypart = 0;
	}

	mstudiobodyparts_t   *pbodypart = (mstudiobodyparts_t *)((byte *)m_pstudiohdr + m_pstudiohdr->bodypartindex) + bodypart;

	index = m_bodynum / pbodypart->base;
	index = index % pbodypart->nummodels;

	m_pmodel = (mstudiomodel_t *)((byte *)m_pstudiohdr + pbodypart->modelindex) + index;
}


/*
================
StudioModel::DrawModel
inputs:
	currententity
	r_entorigin
================
*/
void StudioModel::DrawModel( )
{
	int i;

	g_smodels_total++; // render data cache cookie

	g_pxformverts = &g_xformverts[0];
	g_pvlightvalues = &g_lightvalues[0];

	if (m_pstudiohdr->numbodyparts == 0)
		return;

	glPushMatrix ();

    glTranslatef (m_origin[0],  m_origin[1],  m_origin[2]);

    glRotatef (m_angles[1],  0, 0, 1);
    glRotatef (m_angles[0],  0, 1, 0);
    glRotatef (m_angles[2],  1, 0, 0);

	// glShadeModel (GL_SMOOTH);

	//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	// glHint (GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);

	SetUpBones ( );

	SetupLighting( );
	
	//glUseProgramObjectARB(g_StudioShaderProgram);
	//glActiveTextureARB(GL_TEXTURE1);
	//glBindTexture(GL_TEXTURE_CUBE_MAP, g_StudioCubeMap);
	//glActiveTextureARB(GL_TEXTURE0);

	for (i=0 ; i < m_pstudiohdr->numbodyparts ; i++) 
	{
		SetupModel( i );
		DrawPoints( );
	}

	//glUseProgramObjectARB(0);

	//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	// glShadeModel (GL_FLAT);

	// glHint (GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glPopMatrix ();
}
/*
// Vertex array
static Vector3D modelPos[MAXSTUDIOVERTS];
static Vector3D modelNorm[MAXSTUDIOVERTS];
static Vector3D modelTex[MAXSTUDIOVERTS];
static Vector3D modelTang[MAXSTUDIOVERTS];
static Vector3D modelBitang[MAXSTUDIOVERTS];
static byte modelVertBone[MAXSTUDIOVERTS];	// vertex->bone
static byte modelNormBone[MAXSTUDIOVERTS];	// normal->bone
static bool modelVertDone[MAXSTUDIOVERTS];	//done
static unsigned short mNumVert;

// Indices For sub-model
static unsigned short mTris[MAXSTUDIOTRIANGLES][3];
static unsigned short mNumModelTris;
// Indices For mesh
static unsigned short meshTris[MAXSTUDIOMESHES][MAXSTUDIOTRIANGLES][3];
static unsigned short mNumMeshTris[MAXSTUDIOMESHES];

unsigned int FindVertex(float *pos, float *norm, float s, float t)
{
	unsigned short n;

	for (n = 0; n < mNumVert; n++)
	{
		if (modelPos[n].x == pos[0] && 
			modelPos[n].y == pos[1] &&
			modelPos[n].z == pos[2] &&
			modelNorm[n].x == norm[0] && 
			modelNorm[n].y == norm[1] && 
			modelNorm[n].z == norm[2] && 
			modelTex[n].x == s &&
			modelTex[n].y == t)
		{
			return n;
		}
	}

	__asm int 3;
	return 0;
}
*/

#if 0
// old code

class CVector
{
public:
	CVector(float x, float y, float z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}

	CVector()
	{
		this->x = 0;
		this->y = 0;
		this->z = 0;
	}

	CVector(float *other)
	{
		this->x = other[0];
		this->y = other[1];
		this->z = other[2];
	}

//	CVector(CVector &other)
//	{
//		this->x = other.x;
//		this->y = other.y;
//		this->z = other.z;
//	}

	bool operator ==(CVector &other)
	{
		if (other.x == this->x && 
			other.y == this->y && 
			other.z == this->z)
		{
			return true;
		}

		return false;
	}

	CVector operator -(CVector &other)
	{
		return CVector(
			this->x - other.x,
			this->y - other.y,
			this->z - other.z);
	}

	CVector operator *(float s)
	{
		return CVector(
			this->x * s,
			this->y * s,
			this->z * s);
	}

	operator float *()
	{
		return &x;
	}

	float Length(void)
	{
		return sqrtf(
			this->x * this->x + 
			this->y * this->y + 
			this->z * this->z);
	}

	float Dot(CVector &other)
	{
		return (
			this->x * other.x + 
			this->y * other.y + 
			this->z * other.z);
	}

	CVector Normalize(void)
	{
		float length = Length();

		this->x /= length;
		this->y /= length;
		this->z /= length;

		return *this;
	}

	float x, y, z;
};

class CVertex
{
public:

	void Setup(float *pos, float *norm, float s, float t, byte vb, byte nb)
	{
		this->Pos = pos;
		this->Norm = norm;
		this->UV.x = s;
		this->UV.y = t;
		this->VertBone = vb;
		this->NormBone = nb;
	}

	bool operator ==(CVertex &other)
	{
		if (other.Pos == this->Pos && 
			other.Norm == this->Norm && 
			other.UV == this->UV)
		{
			return true;
		}

		return false;
	}

	byte VertBone;
	byte NormBone;

	CVector Pos;
	CVector Norm;
	CVector UV;
	CVector Tang;
	CVector Bitang;
};

class CTriangle
{
public:
	CTriangle()
	{
		indices[0] = 0;
		indices[1] = 0;
		indices[2] = 0;
	}

	int indices[3];
};

class CMesh
{
public:
	typedef std::vector<CTriangle> TTriangle;
	TTriangle m_triangles;
};

class CStudioTangentSpaceModel
{
public:
	typedef std::vector<CVertex> TVertex;
	TVertex m_vertices;

	typedef std::vector<CTriangle> TTriangle;
	TTriangle m_triangles;

	typedef std::vector<CMesh> TMesh;
	TMesh m_meshes;

	CStudioTangentSpaceModel()
	{
		m_vertices.reserve( 2000 );
		m_triangles.reserve( 2000 );
	}

	void Convert(studiohdr_t *pstudiohdr, studiohdr_t *texturehdr, mstudiomodel_t *pmodel, short *pskinref)
	{
		int					i, j, k;
		byte				*pvertbone;
		byte				*pnormbone;
		vec3_t				*pstudioverts;
		vec3_t				*pstudionorms;
		mstudiotexture_t	*ptexture;
		mstudiomesh_t		*pmesh;

		pvertbone = ((byte *)pstudiohdr + pmodel->vertinfoindex);
		pnormbone = ((byte *)pstudiohdr + pmodel->norminfoindex);
		pstudioverts = (vec3_t *)((byte *)pstudiohdr + pmodel->vertindex);
		pstudionorms = (vec3_t *)((byte *)pstudiohdr + pmodel->normindex);
		ptexture = (mstudiotexture_t *)((byte *)texturehdr + texturehdr->textureindex);

		m_meshes.resize( pmodel->nummesh );

		for (j = 0; j < pmodel->nummesh; j++)
		{
			float	s, t;
			short	*ptricmds;

			pmesh = (mstudiomesh_t *)((byte *)pstudiohdr + pmodel->meshindex) + j;
			ptricmds = (short *)((byte *)pstudiohdr + pmesh->triindex);

			s = 1.0 / (float)ptexture[pskinref[pmesh->skinref]].width;
			t = 1.0 / (float)ptexture[pskinref[pmesh->skinref]].height;

			m_meshes[j].m_triangles.reserve( 2000 );

			while (i = *(ptricmds++))
			{
				bool			fan;
				static short	*cmds[2048];

				fan = false;

				if (i < 0)
				{
					fan = true;
					i = -i;
				}

				for (k = 0; i > 0; k++, i--, ptricmds += 4)
				{
					cmds[k] = ptricmds;

					if (k < 2)
					{
						continue;
					}

					short *indices[3];

					if (!fan)
					{
						if (k % 2)
						{
							indices[0] = cmds[k - 1];
							indices[1] = cmds[k - 2];
							indices[2] = cmds[k];
						}
						else
						{
							indices[0] = cmds[k - 2];
							indices[1] = cmds[k - 1];
							indices[2] = cmds[k];
						}
					}
					else
					{
						indices[0] = cmds[k];
						indices[1] = cmds[0];
						indices[2] = cmds[k - 1];
					}

					static CTriangle triangle;
					triangle.indices[0] = SaveVertex(pstudioverts[indices[0][0]], pstudionorms[indices[0][1]], indices[0][2]*s, indices[0][3]*t, pvertbone[indices[0][0]], pnormbone[indices[0][1]]);
					triangle.indices[1] = SaveVertex(pstudioverts[indices[1][0]], pstudionorms[indices[1][1]], indices[1][2]*s, indices[1][3]*t, pvertbone[indices[1][0]], pnormbone[indices[1][1]]);
					triangle.indices[2] = SaveVertex(pstudioverts[indices[2][0]], pstudionorms[indices[2][1]], indices[2][2]*s, indices[2][3]*t, pvertbone[indices[2][0]], pnormbone[indices[2][1]]);

					m_triangles.push_back(triangle);
					m_meshes[j].m_triangles.push_back(triangle);
				}
			}

			printf("Do Mesh !\n");
		}

		printf("Do Model !\n");

		CalcTangentSpace();

		printf("CalcTangentSpace !\n");
	}

	int SaveVertex(float *pos, float *norm, float s, float t, byte vb, byte nb)
	{
		static CVertex vert;

		vert.Setup(pos, norm, s, t, vb, nb);

		for (TVertex::iterator it = m_vertices.begin(); it != m_vertices.end(); it++)
		{
			if (*it == vert)
				return it - m_vertices.begin();
		}

		m_vertices.push_back(vert);

		return m_vertices.size() - 1;
	}

	void CalcTangentSpace(void)
	{
		// calculate the tangent and bitangent for every face
		for (unsigned short a = 0; a < m_triangles.size(); a++)
		{
			CTriangle &face = m_triangles[a];

			// triangle or polygon... we always use only the first three indices. A polygon
			// is supposed to be planar anyways....
			// FIXME: (thom) create correct calculation for multi-vertex polygons maybe?
			unsigned short p0 = face.indices[0], p1 = face.indices[1], p2 = face.indices[2];

			// position differences p1->p2 and p1->p3
			CVector v = m_vertices[p1].Pos - m_vertices[p0].Pos, w = m_vertices[p2].Pos - m_vertices[p0].Pos;

			// texture offset p1->p2 and p1->p3
			float sx = m_vertices[p1].UV.x - m_vertices[p0].UV.x, sy = m_vertices[p1].UV.y - m_vertices[p0].UV.y;
			float tx = m_vertices[p2].UV.x - m_vertices[p0].UV.x, ty = m_vertices[p2].UV.y - m_vertices[p0].UV.y;
			float dirCorrection = (tx * sy - ty * sx) < 0.0f ? -1.0f : 1.0f;

			// tangent points in the direction where to positive X axis of the texture coords would point in model space
			// bitangents points along the positive Y axis of the texture coords, respectively
			CVector tangent, bitangent;
			tangent.x = (w.x * sy - v.x * ty) * dirCorrection;
			tangent.y = (w.y * sy - v.y * ty) * dirCorrection;
			tangent.z = (w.z * sy - v.z * ty) * dirCorrection;
			bitangent.x = (w.x * sx - v.x * tx) * dirCorrection;
			bitangent.y = (w.y * sx - v.y * tx) * dirCorrection;
			bitangent.z = (w.z * sx - v.z * tx) * dirCorrection;

			// store for every vertex of that face
			for (unsigned short b = 0; b < 3; b++)
			{
				unsigned short p = face.indices[b];

				// project tangent and bitangent into the plane formed by the vertex' normal
				CVector nt = m_vertices[p].Norm * tangent.Dot(m_vertices[p].Norm);
				CVector localTangent = tangent - nt;
				CVector nb = m_vertices[p].Norm * bitangent.Dot(m_vertices[p].Norm);
				CVector localBitangent = bitangent - nb;
				localTangent.Normalize(); localBitangent.Normalize();

				// and write it into the mesh.
				m_vertices[p].Tang = localTangent;
				m_vertices[p].Bitang = localBitangent;
			}
		}
	}
};

#else
// new code

class CVector
{
public:
	CVector(float x, float y, float z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}

	CVector()
	{
		this->x = 0;
		this->y = 0;
		this->z = 0;
	}

	CVector(float *other)
	{
		this->x = other[0];
		this->y = other[1];
		this->z = other[2];
	}

	CVector(const CVector &other)
	{
		this->x = other.x;
		this->y = other.y;
		this->z = other.z;
	}

	bool operator ==(CVector &other)
	{
		if (other.x == this->x && 
			other.y == this->y && 
			other.z == this->z)
		{
			return true;
		}

		return false;
	}

	CVector operator -(const CVector &other) const
	{
		return CVector(
			this->x - other.x,
			this->y - other.y,
			this->z - other.z);
	}

	CVector operator *(float s) const
	{
		return CVector(
			this->x * s,
			this->y * s,
			this->z * s);
	}

	float operator *(CVector &other)
	{	//scalar product
		return
			this->x * other.x + 
			this->y * other.y + 
			this->z * other.z;
	}

	void operator +=(const CVector &other)
	{
		this->x += other.x;
		this->y += other.y;
		this->z += other.z;
	}

	operator const float *()
	{
		return &this->x;
	}

	float SquareLength(void)
	{
		return
			this->x*this->x + 
			this->y*this->y + 
			this->z*this->z;
	}

	float Length(void)
	{
		return sqrtf(
			this->x * this->x + 
			this->y * this->y + 
			this->z * this->z);
	}

	float Dot(CVector &other)
	{
		return (
			this->x * other.x + 
			this->y * other.y + 
			this->z * other.z);
	}

	CVector Normalize(void)
	{
		float length = Length();

		this->x /= length;
		this->y /= length;
		this->z /= length;

		return *this;
	}

	float x, y, z;
};

class SpatialSort
{
public:
	SpatialSort();

public:
	void Fill(CVector *pPositions, unsigned int pNumPositions, unsigned int pElementOffset, bool pFinalize);
	void Append(CVector *pPositions, unsigned int pNumPositions, unsigned int pElementOffset, bool pFinalize);
	void Finalize();
	void FindPositions(CVector &pPosition, float pRadius, std::vector<unsigned int> &poResults);

private:
	CVector mPlaneNormal;

	struct Entry
	{
		unsigned int mIndex;
		CVector mPosition;
		float mDistance;

		Entry() {}
		Entry(unsigned int pIndex, CVector &pPosition, float pDistance)
		{
			mIndex = pIndex;
			mPosition = pPosition;
			mDistance = mDistance;
		}

		bool operator <(Entry &e)
		{
			return this->mDistance < e.mDistance;
		}
	};

	std::vector<Entry> mPositions;
};

SpatialSort::SpatialSort() : mPlaneNormal(0.8523f, 0.34321f, 0.5736f)
{
	mPlaneNormal.Normalize();
}

void SpatialSort::Fill(CVector *pPositions, unsigned int pNumPositions, unsigned int pElementOffset, bool pFinalize)
{
	mPositions.clear();
	Append(pPositions, pNumPositions, pElementOffset, pFinalize);
}

void SpatialSort::Append(CVector *pPositions, unsigned int pNumPositions, unsigned int pElementOffset, bool pFinalize)
{
	const size_t initial = mPositions.size();
	mPositions.reserve(initial + (pFinalize ? pNumPositions : pNumPositions * 2));
	for(unsigned int a = 0; a < pNumPositions; a++)
	{
		char *tempPointer = reinterpret_cast<char *>(pPositions);
		CVector *vec = reinterpret_cast<CVector *>(tempPointer + a * pElementOffset);

		float distance = *vec * mPlaneNormal;
		mPositions.push_back(Entry(a + initial, *vec, distance));
	}

	if (pFinalize)
	{
		Finalize();
	}
}

void SpatialSort::Finalize()
{
	std::sort(mPositions.begin(), mPositions.end());
}

// ------------------------------------------------------------------------------------------------
// Returns an iterator for all positions close to the given position.
void SpatialSort::FindPositions( CVector& pPosition, 
	float pRadius, std::vector<unsigned int>& poResults)
{
	const float dist = pPosition * mPlaneNormal;
	const float minDist = dist - pRadius, maxDist = dist + pRadius;

	// clear the array in this strange fashion because a simple clear() would also deallocate
    // the array which we want to avoid
	poResults.erase( poResults.begin(), poResults.end());

	// quick check for positions outside the range
	if( mPositions.size() == 0)
		return;
	if( maxDist < mPositions.front().mDistance)
		return;
	if( minDist > mPositions.back().mDistance)
		return;

	// do a binary search for the minimal distance to start the iteration there
	unsigned int index = (unsigned int)mPositions.size() / 2;
	unsigned int binaryStepSize = (unsigned int)mPositions.size() / 4;
	while( binaryStepSize > 1)
	{
		if( mPositions[index].mDistance < minDist)
			index += binaryStepSize;
		else
			index -= binaryStepSize;

		binaryStepSize /= 2;
	}

	// depending on the direction of the last step we need to single step a bit back or forth
	// to find the actual beginning element of the range
	while( index > 0 && mPositions[index].mDistance > minDist)
		index--;
	while( index < (mPositions.size() - 1) && mPositions[index].mDistance < minDist)
		index++;
	
	// Mow start iterating from there until the first position lays outside of the distance range.
	// Add all positions inside the distance range within the given radius to the result aray
	std::vector<Entry>::const_iterator it = mPositions.begin() + index;
	const float pSquared = pRadius*pRadius;
	while( it->mDistance < maxDist)
	{
		if( (it->mPosition - pPosition).SquareLength() < pSquared)
			poResults.push_back( it->mIndex);
		++it;
		if( it == mPositions.end())
			break;
	}

	// that's it
}


class CVertex
{
public:

	void Setup(float *pos, float *norm, float s, float t, byte vb, byte nb)
	{
		this->Pos = pos;
		this->Norm = norm;
		this->UV.x = s;
		this->UV.y = t;
		this->VertBone = vb;
		this->NormBone = nb;
	}

	bool operator ==(CVertex &other)
	{
		if (other.Pos == this->Pos && 
			other.Norm == this->Norm && 
			other.UV == this->UV)
		{
			return true;
		}

		return false;
	}

	byte VertBone;
	byte NormBone;

	CVector Pos;
	CVector Norm;
	CVector UV;
	CVector Tang;
	CVector Bitang;
};

class CTriangle
{
public:
	CTriangle()
	{
		indices[0] = 0;
		indices[1] = 0;
		indices[2] = 0;
	}

	int indices[3];
};

class CMesh
{
public:
	typedef std::vector<CTriangle> TTriangle;
	TTriangle m_triangles;
};

class CStudioTangentSpaceModel
{
public:
	typedef std::vector<CVertex> TVertex;
	TVertex m_vertices;

	typedef std::vector<CTriangle> TTriangle;
	TTriangle m_triangles;

	typedef std::vector<CMesh> TMesh;
	TMesh m_meshes;

	CStudioTangentSpaceModel()
	{
		m_vertices.reserve( 2000 );
		m_triangles.reserve( 2000 );
	}

	void Convert(studiohdr_t *pstudiohdr, studiohdr_t *texturehdr, mstudiomodel_t *pmodel, short *pskinref)
	{
		printf("Rebuilding mesh...\n");

		int					i, j, k;
		byte				*pvertbone;
		byte				*pnormbone;
		vec3_t				*pstudioverts;
		vec3_t				*pstudionorms;
		mstudiotexture_t	*ptexture;
		mstudiomesh_t		*pmesh;

		pvertbone = ((byte *)pstudiohdr + pmodel->vertinfoindex);
		pnormbone = ((byte *)pstudiohdr + pmodel->norminfoindex);
		pstudioverts = (vec3_t *)((byte *)pstudiohdr + pmodel->vertindex);
		pstudionorms = (vec3_t *)((byte *)pstudiohdr + pmodel->normindex);
		ptexture = (mstudiotexture_t *)((byte *)texturehdr + texturehdr->textureindex);

		m_meshes.resize( pmodel->nummesh );

		for (j = 0; j < pmodel->nummesh; j++)
		{
			float	s, t;
			short	*ptricmds;

			pmesh = (mstudiomesh_t *)((byte *)pstudiohdr + pmodel->meshindex) + j;
			ptricmds = (short *)((byte *)pstudiohdr + pmesh->triindex);

			s = 1.0 / (float)ptexture[pskinref[pmesh->skinref]].width;
			t = 1.0 / (float)ptexture[pskinref[pmesh->skinref]].height;

			m_meshes[j].m_triangles.reserve( 2000 );

			while (i = *(ptricmds++))
			{
				bool			fan;
				static short	*cmds[2048];

				fan = false;

				if (i < 0)
				{
					fan = true;
					i = -i;
				}

				for (k = 0; i > 0; k++, i--, ptricmds += 4)
				{
					cmds[k] = ptricmds;

					if (k < 2)
					{
						continue;
					}

					short *indices[3];

					if (!fan)
					{
						if (k % 2)
						{
							indices[0] = cmds[k - 1];
							indices[1] = cmds[k - 2];
							indices[2] = cmds[k];
						}
						else
						{
							indices[0] = cmds[k - 2];
							indices[1] = cmds[k - 1];
							indices[2] = cmds[k];
						}
					}
					else
					{
						indices[0] = cmds[k];
						indices[1] = cmds[0];
						indices[2] = cmds[k - 1];
					}

					static CTriangle triangle;
					triangle.indices[0] = SaveVertex(pstudioverts[indices[0][0]], pstudionorms[indices[0][1]], indices[0][2]*s, indices[0][3]*t, pvertbone[indices[0][0]], pnormbone[indices[0][1]]);
					triangle.indices[1] = SaveVertex(pstudioverts[indices[1][0]], pstudionorms[indices[1][1]], indices[1][2]*s, indices[1][3]*t, pvertbone[indices[1][0]], pnormbone[indices[1][1]]);
					triangle.indices[2] = SaveVertex(pstudioverts[indices[2][0]], pstudionorms[indices[2][1]], indices[2][2]*s, indices[2][3]*t, pvertbone[indices[2][0]], pnormbone[indices[2][1]]);

					m_triangles.push_back(triangle);
					m_meshes[j].m_triangles.push_back(triangle);
				}
			}

			//printf("Do Mesh !\n");
		}

		printf("Done.\n");

		CalcTangentSpace();
	}

	int SaveVertex(float *pos, float *norm, float s, float t, byte vb, byte nb)
	{
		static CVertex vert;

		vert.Setup(pos, norm, s, t, vb, nb);

		//for (TVertex::iterator it = m_vertices.begin(); it != m_vertices.end(); it++)
		//{
		//	if (*it == vert)
		//		return it - m_vertices.begin();
		//}
		for (int i = 0; i < m_vertices.size(); i++)
		{
			if (m_vertices[i] == vert)
				return i;
		}

		m_vertices.push_back(vert);

		return m_vertices.size() - 1;
	}

	// -------------------------------------------------------------------------------
	// Compute a good epsilon value for position comparisons on a mesh
	float ComputePositionEpsilon(CVertex *pVertices, unsigned int pCount)
	{
		const float epsilon = 1e-4f;

		// calculate the position bounds so we have a reliable epsilon to check position differences against 
		CVector minVec(9999.0f,9999.0f,9999.0f), maxVec(-9999.0f,-9999.0f,-9999.0f);
		//ArrayBounds(pMesh->mVertices,pMesh->mNumVertices,minVec,maxVec);
		for (unsigned int a = 0; a < pCount; a++)
		{
			CVertex &v = pVertices[a];

			if (v.Pos.x < minVec.x)
				minVec.x = v.Pos.x;
			if (v.Pos.y < minVec.y)
				minVec.y = v.Pos.y;
			if (v.Pos.z < minVec.z)
				minVec.z = v.Pos.z;

			if (v.Pos.x > maxVec.x)
				maxVec.x = v.Pos.x;
			if (v.Pos.y > maxVec.y)
				maxVec.y = v.Pos.y;
			if (v.Pos.z > maxVec.z)
				maxVec.z = v.Pos.z;
		}
		return (maxVec - minVec).Length() * epsilon;
	}

	void CalcTangentSpace(void)
	{
		printf("Calculating tangent space ...\n");

		// calculate the tangent and bitangent for every face
		for (unsigned short a = 0; a < m_triangles.size(); a++)
		{
			CTriangle &face = m_triangles[a];

			// triangle or polygon... we always use only the first three indices. A polygon
			// is supposed to be planar anyways....
			// FIXME: (thom) create correct calculation for multi-vertex polygons maybe?
			unsigned short p0 = face.indices[0], p1 = face.indices[1], p2 = face.indices[2];

			// position differences p1->p2 and p1->p3
			CVector v = m_vertices[p1].Pos - m_vertices[p0].Pos, w = m_vertices[p2].Pos - m_vertices[p0].Pos;

			// texture offset p1->p2 and p1->p3
			float sx = m_vertices[p1].UV.x - m_vertices[p0].UV.x, sy = m_vertices[p1].UV.y - m_vertices[p0].UV.y;
			float tx = m_vertices[p2].UV.x - m_vertices[p0].UV.x, ty = m_vertices[p2].UV.y - m_vertices[p0].UV.y;
			float dirCorrection = (tx * sy - ty * sx) < 0.0f ? -1.0f : 1.0f;

			// tangent points in the direction where to positive X axis of the texture coords would point in model space
			// bitangents points along the positive Y axis of the texture coords, respectively
			CVector tangent, bitangent;
			tangent.x = (w.x * sy - v.x * ty) * dirCorrection;
			tangent.y = (w.y * sy - v.y * ty) * dirCorrection;
			tangent.z = (w.z * sy - v.z * ty) * dirCorrection;
			bitangent.x = (w.x * sx - v.x * tx) * dirCorrection;
			bitangent.y = (w.y * sx - v.y * tx) * dirCorrection;
			bitangent.z = (w.z * sx - v.z * tx) * dirCorrection;

			// store for every vertex of that face
			for (unsigned short b = 0; b < 3; b++)
			{
				unsigned short p = face.indices[b];

				// project tangent and bitangent into the plane formed by the vertex' normal
				CVector nt = m_vertices[p].Norm * tangent.Dot(m_vertices[p].Norm);
				CVector localTangent = tangent - nt;
				CVector nb = m_vertices[p].Norm * bitangent.Dot(m_vertices[p].Norm);
				CVector localBitangent = bitangent - nb;
				localTangent.Normalize(); localBitangent.Normalize();

				// and write it into the mesh.
				m_vertices[p].Tang = localTangent;
				m_vertices[p].Bitang = localBitangent;
			}
		}

#if 0
		const float angleEpsilon = 0.9999f;

		std::vector<bool> vertexDone( m_vertices.size(), false);

		// create a helper to quickly find locally close vertices among the vertex array
		// FIX: check whether we can reuse the SpatialSort of a previous step
		SpatialSort* vertexFinder = NULL;
		SpatialSort  _vertexFinder;
		float posEpsilon = 100.0f;	//ÔÝ¶¨
		if (!vertexFinder)
		{
			_vertexFinder.Fill(&m_vertices[0].Pos, m_vertices.size(), sizeof( CVector)*5, true);
			vertexFinder = &_vertexFinder;
			posEpsilon = ComputePositionEpsilon(&m_vertices[0], m_vertices.size());
		}
		std::vector<unsigned int> verticesFound;

		const float fLimit = cosf( 0.17453293055555555f );	//Æ½»¬½Ç¶È: 10¡ã
		std::vector<unsigned int> closeVertices;

		// in the second pass we now smooth out all tangents and bitangents at the same local position
		// if they are not too far off.
		for( unsigned int a = 0; a < m_vertices.size(); a++)
		{
			if( vertexDone[a])
				continue;

			CVector& origPos = m_vertices[a].Pos;
			CVector& origNorm = m_vertices[a].Norm;
			CVector& origTang = m_vertices[a].Tang;
			CVector& origBitang = m_vertices[a].Bitang;
			closeVertices.clear();

			// find all vertices close to that position
			vertexFinder->FindPositions( origPos, posEpsilon, verticesFound);

			closeVertices.reserve (verticesFound.size()+5);
			closeVertices.push_back( a);

			// look among them for other vertices sharing the same normal and a close-enough tangent/bitangent
			for( unsigned int b = 0; b < verticesFound.size(); b++)
			{
				unsigned int idx = verticesFound[b];
				if( vertexDone[idx])
					continue;
				if( m_vertices[idx].Norm * origNorm < angleEpsilon)
					continue;
				if(  m_vertices[idx].Tang * origTang < fLimit)
					continue;
				if( m_vertices[idx].Bitang * origBitang < fLimit)
					continue;

				// it's similar enough -> add it to the smoothing group
				closeVertices.push_back( idx);
				vertexDone[idx] = true;
			}

			// smooth the tangents and bitangents of all vertices that were found to be close enough
			CVector smoothTangent( 0, 0, 0), smoothBitangent( 0, 0, 0);
			for( unsigned int b = 0; b < closeVertices.size(); ++b)
			{
				smoothTangent += m_vertices[ closeVertices[b] ].Tang;
				smoothBitangent += m_vertices[ closeVertices[b] ].Bitang;
			}
			smoothTangent.Normalize();
			smoothBitangent.Normalize();

			// and write it back into all affected tangents
			for( unsigned int b = 0; b < closeVertices.size(); ++b)
			{
				m_vertices[ closeVertices[b] ].Tang = smoothTangent;
				m_vertices[ closeVertices[b] ].Bitang = smoothBitangent;
			}
		}
#endif
		printf("Done.\n");
	}
};
#endif

typedef std::map<DWORD, CStudioTangentSpaceModel *> TModel;
TModel g_Model;

/*
================

================
*/
void StudioModel::DrawPoints ( )
{
	int					i, j;
	mstudiomesh_t		*pmesh;
	byte				*pvertbone;
	byte				*pnormbone;
	vec3_t				*pstudioverts;
	vec3_t				*pstudionorms;
	mstudiotexture_t	*ptexture;
	float 				*av;
	float				*lv;
	float				lv_tmp;
	short				*pskinref;

	pvertbone = ((byte *)m_pstudiohdr + m_pmodel->vertinfoindex);
	pnormbone = ((byte *)m_pstudiohdr + m_pmodel->norminfoindex);
	ptexture = (mstudiotexture_t *)((byte *)m_ptexturehdr + m_ptexturehdr->textureindex);

	pmesh = (mstudiomesh_t *)((byte *)m_pstudiohdr + m_pmodel->meshindex);

	pstudioverts = (vec3_t *)((byte *)m_pstudiohdr + m_pmodel->vertindex);
	pstudionorms = (vec3_t *)((byte *)m_pstudiohdr + m_pmodel->normindex);

	pskinref = (short *)((byte *)m_ptexturehdr + m_ptexturehdr->skinindex);
	if (m_skinnum != 0 && m_skinnum < m_ptexturehdr->numskinfamilies)
		pskinref += (m_skinnum * m_ptexturehdr->numskinref);

	//for (i = 0; i < m_pmodel->numverts; i++)
	//{
	//	VectorTransform (pstudioverts[i], g_bonetransform[pvertbone[i]], g_pxformverts[i]);
	//}

//
// Calculate tangent space
//
	CStudioTangentSpaceModel *pModel = g_Model[ (DWORD)m_pmodel ];

	if ( !pModel )
	{
		pModel = new CStudioTangentSpaceModel();
		if ( !pModel )
		{
			printf("Out of memory\n");
			return;
		}

		pModel->Convert( m_pstudiohdr, m_pstudiohdr, m_pmodel, pskinref );

		g_Model[ (DWORD)m_pmodel ] = pModel;
	}

	if ( !pModel )
	{
		return;
	}

	//Merge vertexes
/*	mNumVert = 0;

	for (j = 0; j < m_pmodel->nummesh; j++)
	{
		float s, t;
		short *ptricmds;

		pmesh = (mstudiomesh_t *)((byte *)m_pstudiohdr + m_pmodel->meshindex) + j;
		ptricmds = (short *)((byte *)m_pstudiohdr + pmesh->triindex);

		s = 1.0 / (float)ptexture[pskinref[pmesh->skinref]].width;
		t = 1.0 / (float)ptexture[pskinref[pmesh->skinref]].height;

		while (i = *(ptricmds++))
		{
			if (i < 0)
			{
				i = -i;
			}

			for (; i > 0; i--, ptricmds += 4)
			{
				unsigned int n;

				for (n = 0; n < mNumVert; n++)
				{
					if (modelPos[n].x == pstudioverts[ptricmds[0]][0] && 
						modelPos[n].y == pstudioverts[ptricmds[0]][1] &&
						modelPos[n].z == pstudioverts[ptricmds[0]][2] &&
						modelNorm[n].x == pstudionorms[ptricmds[1]][0] && 
						modelNorm[n].y == pstudionorms[ptricmds[1]][1] && 
						modelNorm[n].z == pstudionorms[ptricmds[1]][2] && 
						modelTex[n].x == ptricmds[2] * s &&
						modelTex[n].y == ptricmds[3] * t)
					{
						break;
					}
				}

				if (n < mNumVert)
				{
					// This vertex is already in the array
					continue;
				}

				modelPos[mNumVert] = pstudioverts[ptricmds[0]];
				modelNorm[mNumVert] = pstudionorms[ptricmds[1]];
				modelTex[mNumVert] = Vector3D(ptricmds[2]*s, ptricmds[3]*t);
				//
				modelVertBone[mNumVert] = pvertbone[ptricmds[0]];
				modelNormBone[mNumVert] = pnormbone[ptricmds[1]];
				//
				modelVertDone[mNumVert] =false;
				//
				mNumVert++;
			}
		}
	}

	// Build triangle indices

	mNumModelTris = 0;

	for (j = 0; j < m_pmodel->nummesh; j++)
	{
		float s, t;
		short *ptricmds;

		pmesh = (mstudiomesh_t *)((byte *)m_pstudiohdr + m_pmodel->meshindex) + j;
		ptricmds = (short *)((byte *)m_pstudiohdr + pmesh->triindex);

		s = 1.0 / (float)ptexture[pskinref[pmesh->skinref]].width;
		t = 1.0 / (float)ptexture[pskinref[pmesh->skinref]].height;

		mNumMeshTris[j] = 0;

		while (i = *(ptricmds++))
		{
			bool strip = true;

			if (i < 0)
			{
				i = -i;
				strip = false;
			}

			static short *vertcmds[ MAXSTUDIOVERTS ];

			unsigned int n = 0;

			for (; i > 0; i--, n++, ptricmds += 4)
			{
				vertcmds[ n ] = ptricmds;

				if (n < 2)
				{
					continue;
				}

				short *indices[3];

				if (strip)
				{
					if (n % 2)
					{
						indices[0] = vertcmds[n-1];
						indices[1] = vertcmds[n-2];
						indices[2] = vertcmds[ n ];
					}
					else
					{
						indices[0] = vertcmds[n-2];
						indices[1] = vertcmds[n-1];
						indices[2] = vertcmds[ n ];
					}
				}
				else
				{
					indices[0] = vertcmds[ n ];
					indices[1] = vertcmds[ 0 ];
					indices[2] = vertcmds[n-1];
				}

				unsigned short iVertIdx[3];

				iVertIdx[0] = FindVertex( pstudioverts[indices[0][0]], pstudionorms[indices[0][1]], indices[0][2]*s, indices[0][3]*t );
				iVertIdx[1] = FindVertex( pstudioverts[indices[1][0]], pstudionorms[indices[1][1]], indices[1][2]*s, indices[1][3]*t );
				iVertIdx[2] = FindVertex( pstudioverts[indices[2][0]], pstudionorms[indices[2][1]], indices[2][2]*s, indices[2][3]*t );

				// Save for mesh
				meshTris[j][mNumMeshTris[j]][0] = iVertIdx[0];
				meshTris[j][mNumMeshTris[j]][1] = iVertIdx[1];
				meshTris[j][mNumMeshTris[j]][2] = iVertIdx[2];

				// Save for sub-model
				mTris[mNumModelTris][0] = iVertIdx[0];
				mTris[mNumModelTris][1] = iVertIdx[1];
				mTris[mNumModelTris][2] = iVertIdx[2];

				mNumMeshTris[j]++;
				mNumModelTris++;
			}
		}
		// End mesh
	}

	// calculate the tangent and bitangent for every face
	for (unsigned short a = 0; a < mNumModelTris; a++)
	{
		unsigned short *face = mTris[a];

		// triangle or polygon... we always use only the first three indices. A polygon
		// is supposed to be planar anyways....
		// FIXME: (thom) create correct calculation for multi-vertex polygons maybe?
		unsigned short p0 = face[0], p1 = face[1], p2 = face[2];

		// position differences p1->p2 and p1->p3
		Vector3D v = modelPos[p1] - modelPos[p0], w = modelPos[p2] - modelPos[p0];

		// texture offset p1->p2 and p1->p3
		float sx = modelTex[p1].x - modelTex[p0].x, sy = modelTex[p1].y - modelTex[p0].y;
		float tx = modelTex[p2].x - modelTex[p0].x, ty = modelTex[p2].y - modelTex[p0].y;
		float dirCorrection = (tx * sy - ty * sx) < 0.0f ? -1.0f : 1.0f;

		// tangent points in the direction where to positive X axis of the texture coords would point in model space
		// bitangents points along the positive Y axis of the texture coords, respectively
		Vector3D tangent, bitangent;
		tangent.x = (w.x * sy - v.x * ty) * dirCorrection;
		tangent.y = (w.y * sy - v.y * ty) * dirCorrection;
		tangent.z = (w.z * sy - v.z * ty) * dirCorrection;
		bitangent.x = (w.x * sx - v.x * tx) * dirCorrection;
		bitangent.y = (w.y * sx - v.y * tx) * dirCorrection;
		bitangent.z = (w.z * sx - v.z * tx) * dirCorrection;

		// store for every vertex of that face
		for (unsigned short b = 0; b < 3; b++)
		{
			unsigned short p = face[b];

			// project tangent and bitangent into the plane formed by the vertex' normal
			Vector3D nt = modelNorm[p] * tangent.Dot(modelNorm[p]);
			Vector3D localTangent = tangent - nt;
			Vector3D nb = modelNorm[p] * bitangent.Dot(modelNorm[p]);
			Vector3D localBitangent = bitangent - nb;
			localTangent.Normalize(); localBitangent.Normalize();

			// and write it into the mesh.
			modelTang[p] = localTangent;
			modelBitang[p] = localBitangent;
		}
	}*/

	// in the second pass we now smooth out all tangents and bitangents at the same local position
	// if they are not too far off.
	/*for (unsigned short a = 0; a < mNumVert; a++)
	{
		if (modelVertDone[a])
			continue;

		Vector3D &origPos = modelPos[a];
		Vector3D &origNorm = modelNorm[a];
		Vector3D &origTang = modelTang[a];
		Vector3D &origBitang = modelBitang[a];

		// find all vertices close to that position
	}*/

	//transform vector
/*	for (i = 0; i < mNumVert; i++)
	{
		VectorTransform (modelPos[i], g_bonetransform[modelVertBone[i]], g_pxformverts[i]);
	}
	for (i = 0; i < mNumVert; i++)
	{
		//FIXME:
		VectorRotate (modelNorm[i], g_bonetransform[modelNormBone[i]], g_studionormal[i]);
		VectorRotate (modelTang[i], g_bonetransform[modelNormBone[i]], g_studiotangent[i]);
		VectorRotate (modelBitang[i], g_bonetransform[modelNormBone[i]], g_studiobitangent[i]);
	}
*/

	for (i = 0; i < pModel->m_vertices.size(); i++)
	{
		VectorTransform (pModel->m_vertices[i].Pos, g_bonetransform[pModel->m_vertices[i].VertBone], g_pxformverts[i]);

		VectorRotate (pModel->m_vertices[i].Norm, g_bonetransform[pModel->m_vertices[i].NormBone], g_studionormal[i]);
		VectorRotate (pModel->m_vertices[i].Tang, g_bonetransform[pModel->m_vertices[i].NormBone], g_studiotangent[i]);
		VectorRotate (pModel->m_vertices[i].Bitang, g_bonetransform[pModel->m_vertices[i].NormBone], g_studiobitangent[i]);
	}

	glEnable(GL_TEXTURE_2D);
	glCullFace(GL_FRONT);

//
// Shader
//
			GLint attribTangent, attribBitangent;

			glActiveTextureARB(GL_TEXTURE0_ARB);
			glBindTexture(GL_TEXTURE_2D, g_StudioDiffuseMap);

			glActiveTextureARB(GL_TEXTURE1_ARB);
			glBindTexture(GL_TEXTURE_2D, g_StudioSpecularMap);

			glActiveTextureARB(GL_TEXTURE2_ARB);
			glBindTexture(GL_TEXTURE_2D, g_StudioNormalMap);

			glActiveTextureARB(GL_TEXTURE3_ARB);
			glBindTexture(GL_TEXTURE_CUBE_MAP, g_StudioEnvCubeMap);

			glColor4f(1, 1, 1, 1);

			glUseProgramObjectARB(g_StudioShaderProgram);

			vec3_t lightPos;
			vec3_t viewPos;

			viewPos[0] = -transx;
			viewPos[1] = -transy;
			viewPos[2] = -transz;

			//VectorCopy(viewPos, lightPos);

			lightPos[0] = light_pos[0];
			lightPos[1] = light_pos[1];
			lightPos[2] = light_pos[2];

			glUniform3fARB(glGetUniformLocationARB(g_StudioShaderProgram, "lightPos"), lightPos[0], lightPos[1], lightPos[2]);
			glUniform3fARB(glGetUniformLocationARB(g_StudioShaderProgram, "viewPos"), viewPos[0], viewPos[1], viewPos[2]);

			glUniform1iARB(glGetUniformLocationARB(g_StudioShaderProgram, "diffuseMap"), 0);
			glUniform1iARB(glGetUniformLocationARB(g_StudioShaderProgram, "specularMap"), 1);
			glUniform1iARB(glGetUniformLocationARB(g_StudioShaderProgram, "normalMap"), 2);
			glUniform1iARB(glGetUniformLocationARB(g_StudioShaderProgram, "envCubeMap"), 3);

			glUniform1fARB(glGetUniformLocationARB(g_StudioShaderProgram, "lightAmbient"), 0.5);
			glUniform1fARB(glGetUniformLocationARB(g_StudioShaderProgram, "lightDiffuse"), 3.1);
			glUniform1fARB(glGetUniformLocationARB(g_StudioShaderProgram, "lightSpecular"), 5);

			glUniform1fARB(glGetUniformLocationARB(g_StudioShaderProgram, "materialShininess"), 8.0);

			attribTangent = glGetAttribLocationARB(g_StudioShaderProgram, "tangent");
			attribBitangent = glGetAttribLocationARB(g_StudioShaderProgram, "bitangent");

	//glEnableClientState(GL_VERTEX_ARRAY);
	//glVertexPointer(3, GL_FLOAT, 0, g_pxformverts);

	//glEnableClientState(GL_NORMAL_ARRAY);
	//glNormalPointer(GL_FLOAT, 0, g_studionormal);

	//glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	//glTexCoordPointer(2, GL_FLOAT, 12, modelTex);

	glColor4f(1, 1, 1, 1);

	for (j = 0; j < pModel->m_meshes.size(); j++)
	{
		CMesh &mesh = pModel->m_meshes[ j ];

		glBegin(GL_TRIANGLES);

		for (i = 0; i < mesh.m_triangles.size(); i++)
		{
			for (int k=0; k<3; k++)
			{
				int index = mesh.m_triangles[i].indices[k];

				CVertex &vert = pModel->m_vertices[ index ];

				glTexCoord2f( vert.UV.x, vert.UV.y );

				glVertexAttrib3fARB( attribTangent, g_studiotangent[index][0], g_studiotangent[index][1], g_studiotangent[index][2] );
				glVertexAttrib3fARB( attribBitangent, g_studiobitangent[index][0], g_studiobitangent[index][1], g_studiobitangent[index][2] );
				glNormal3f( g_studionormal[index][0], g_studionormal[index][1], g_studionormal[index][2] );
				glVertex3f( g_pxformverts[index][0], g_pxformverts[index][1], g_pxformverts[index][2] );
			}
		}

		glEnd();
	}

/*	for (j = 0; j < m_pmodel->nummesh; j++)
	{
		//glBindTexture(GL_TEXTURE_2D, g_StudioSpecularMap);
		//glDrawElements(GL_TRIANGLES, mNumMeshTris[j]*3, GL_UNSIGNED_SHORT, meshTris[j]);
		glBegin(GL_TRIANGLES);

		for (i = 0; i < mNumMeshTris[j]; i++)
		{
			for (int k=0; k<3; k++)
			{
				unsigned short p = meshTris[j][i][k];

				glTexCoord2f(modelTex[p].x, modelTex[p].y);
				glVertexAttrib3fARB(attribTangent, g_studiotangent[p][0], g_studiotangent[p][1], g_studiotangent[p][2]);
				//glVertexAttrib3fARB(attribBitangent, g_studiobitangent[p][0], g_studiobitangent[p][1], g_studiobitangent[p][2]);
				glNormal3f(g_studionormal[p][0], g_studionormal[p][1], g_studionormal[p][2]);
				glVertex3f(g_pxformverts[p][0], g_pxformverts[p][1], g_pxformverts[p][2]);
			}
		}

		glEnd();
	}*/

	//glDisableClientState(GL_VERTEX_ARRAY);
	//glDisableClientState(GL_NORMAL_ARRAY);
	//glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	//disable shader
	glUseProgramObjectARB(0);
	glActiveTextureARB(GL_TEXTURE0_ARB);

	/*glDisable(GL_TEXTURE_2D);

	for (j = 0; j < m_pmodel->nummesh; j++)
	{
		glBegin(GL_LINES);
		glLineWidth(0.1);

		for (i = 0; i < 50; i++)
		{
			for (int k=0; k<3; k++)
			{
				unsigned short p = meshTris[j][i][k];

				glColor4f(1, 0, 0, 1);
				glVertex3f(g_pxformverts[p][0], g_pxformverts[p][1], g_pxformverts[p][2]);
				glVertex3f(g_pxformverts[p][0]+g_studionormal[p][0], g_pxformverts[p][1]+g_studionormal[p][1], g_pxformverts[p][2]+g_studionormal[p][2]);

				glColor4f(0, 1, 0, 1);
				glVertex3f(g_pxformverts[p][0], g_pxformverts[p][1], g_pxformverts[p][2]);
				glVertex3f(g_pxformverts[p][0]+g_studiotangent[p][0], g_pxformverts[p][1]+g_studiotangent[p][1], g_pxformverts[p][2]+g_studiotangent[p][2]);

				glColor4f(0, 0, 1, 1);
				glVertex3f(g_pxformverts[p][0], g_pxformverts[p][1], g_pxformverts[p][2]);
				glVertex3f(g_pxformverts[p][0]+g_studiobitangent[p][0], g_pxformverts[p][1]+g_studiobitangent[p][1], g_pxformverts[p][2]+g_studiobitangent[p][2]);
			}
		}

		glEnd();
	}

	glEnable(GL_TEXTURE_2D);*/

#if 0
	glDisable(GL_TEXTURE_2D);

	for (j = 0; j < pModel->m_meshes.size(); j++)
	{
		CMesh &mesh = pModel->m_meshes[ j ];

		glBegin(GL_LINES);
		glLineWidth(0.05);

		for (i = 0; i < mesh.m_triangles.size(); i++)
		{
			for (int k=0; k<3; k++)
			{
				int index = mesh.m_triangles[i].indices[k];

				glColor4f(1, 0, 0, 1);
				glVertex3f( g_pxformverts[index][0], g_pxformverts[index][1], g_pxformverts[index][2] );
				glVertex3f( g_pxformverts[index][0]+g_studionormal[index][0], g_pxformverts[index][1]+g_studionormal[index][1], g_pxformverts[index][2]+g_studionormal[index][2] );

				glColor4f(0, 1, 0, 1);
				glVertex3f( g_pxformverts[index][0], g_pxformverts[index][1], g_pxformverts[index][2] );
				glVertex3f( g_pxformverts[index][0]+g_studiotangent[index][0], g_pxformverts[index][1]+g_studiotangent[index][1], g_pxformverts[index][2]+g_studiotangent[index][2] );

				glColor4f(0, 0, 1, 1);
				glVertex3f( g_pxformverts[index][0], g_pxformverts[index][1], g_pxformverts[index][2] );
				glVertex3f( g_pxformverts[index][0]+g_studiobitangent[index][0], g_pxformverts[index][1]+g_studiobitangent[index][1], g_pxformverts[index][2]+g_studiobitangent[index][2] );
			}
		}

		glEnd();
	}

	glEnable(GL_TEXTURE_2D);
#endif

//
// clip and draw all triangles
//

	/*lv = (float *)g_pvlightvalues;
	for (j = 0; j < m_pmodel->nummesh; j++) 
	{
		int flags;
		flags = ptexture[pskinref[pmesh[j].skinref]].flags;
		for (i = 0; i < pmesh[j].numnorms; i++, lv += 3, pstudionorms++, pnormbone++)
		{
			Lighting (&lv_tmp, *pnormbone, flags, (float *)pstudionorms);

			// FIX: move this check out of the inner loop
			if (flags & STUDIO_NF_CHROME)
				Chrome( g_chrome[(float (*)[3])lv - g_pvlightvalues], *pnormbone, (float *)pstudionorms );

			lv[0] = lv_tmp * g_lightcolor[0];
			lv[1] = lv_tmp * g_lightcolor[1];
			lv[2] = lv_tmp * g_lightcolor[2];
		}
	}*/

	//glCullFace(GL_FRONT);

	/*for (j = 0; j < m_pmodel->nummesh; j++) 
	{
		float s, t;
		short		*ptricmds;

		pmesh = (mstudiomesh_t *)((byte *)m_pstudiohdr + m_pmodel->meshindex) + j;
		ptricmds = (short *)((byte *)m_pstudiohdr + pmesh->triindex);

		if (ptexture[pskinref[pmesh->skinref]].flags != 0)
			continue;
		
		s = 1.0/(float)ptexture[pskinref[pmesh->skinref]].width;
		t = 1.0/(float)ptexture[pskinref[pmesh->skinref]].height;

		//Activate shader program here
		//bool useShader = !stricmp( ptexture[pskinref[pmesh->skinref]].name, "PV-KUKRI_BEAST.BMP" );
		bool useShader = true;

		GLint attribTangent, attribNormal;

		if ( useShader )
		{
			glActiveTextureARB(GL_TEXTURE0_ARB);
			glBindTexture(GL_TEXTURE_2D, g_StudioDiffuseMap);

			//glActiveTextureARB(GL_TEXTURE1_ARB);
			//glBindTexture(GL_TEXTURE_2D, g_StudioSpecularMap);

			//glActiveTextureARB(GL_TEXTURE2_ARB);
			//glBindTexture(GL_TEXTURE_2D, g_StudioNormalMap);

			//glColor4f(1, 1, 1, 1);

			//glUseProgramObjectARB(g_StudioShaderProgram);

			//vec3_t lightPos;
			//vec3_t viewPos;

			//viewPos[0] = -transx;
			//viewPos[1] = -transy;
			//viewPos[2] = -transz;

			////VectorCopy(viewPos, lightPos);

			//lightPos[0] = light_pos[0];
			//lightPos[1] = light_pos[1];
			//lightPos[2] = light_pos[2];

			//glUniform3fARB(glGetUniformLocationARB(g_StudioShaderProgram, "lightPos"), lightPos[0], lightPos[1], lightPos[2]);
			//glUniform3fARB(glGetUniformLocationARB(g_StudioShaderProgram, "viewPos"), viewPos[0], viewPos[1], viewPos[2]);

			//glUniform1iARB(glGetUniformLocationARB(g_StudioShaderProgram, "diffuseMap"), 0);
			//glUniform1iARB(glGetUniformLocationARB(g_StudioShaderProgram, "specularMap"), 1);
			//glUniform1iARB(glGetUniformLocationARB(g_StudioShaderProgram, "normalMap"), 2);

			//glUniform1fARB(glGetUniformLocationARB(g_StudioShaderProgram, "lightAmbient"), 0.1);
			//glUniform1fARB(glGetUniformLocationARB(g_StudioShaderProgram, "lightDiffuse"), 1.5);
			//glUniform1fARB(glGetUniformLocationARB(g_StudioShaderProgram, "lightSpecular"), 5.5);

			//glUniform1fARB(glGetUniformLocationARB(g_StudioShaderProgram, "materialShininess"), 2.0);

			//attribTangent = glGetAttribLocationARB(g_StudioShaderProgram, "tangent");
			//attribNormal = glGetAttribLocationARB(g_StudioShaderProgram, "normal");
		}
		else
		{
			glBindTexture(GL_TEXTURE_2D, ptexture[pskinref[pmesh->skinref]].index);
		}

		if (ptexture[pskinref[pmesh->skinref]].flags & STUDIO_NF_CHROME)
		{
			while (i = *(ptricmds++))
			{
				if (i < 0)
				{
					glBegin( GL_TRIANGLE_FAN );
					i = -i;
				}
				else
				{
					glBegin( GL_TRIANGLE_STRIP );
				}


				for( ; i > 0; i--, ptricmds += 4)
				{
					// FIX: put these in as integer coords, not floats
					glTexCoord2f(g_chrome[ptricmds[1]][0]*s, g_chrome[ptricmds[1]][1]*t);
					
					lv = g_pvlightvalues[ptricmds[1]];
					glColor4f( lv[0], lv[1], lv[2], 1.0 );

					av = g_pxformverts[ptricmds[0]];
					glVertex3f(av[0], av[1], av[2]);
				}
				glEnd( );
			}	
		} 
		else 
		{
			if ( useShader )
			{
				//ptricmds = (short *)((byte *)m_pstudiohdr + pmesh->triindex);

				//pnormbone = ((byte *)m_pstudiohdr + m_pmodel->norminfoindex);
				//pstudionorms = (vec3_t *)((byte *)m_pstudiohdr + m_pmodel->normindex);

				//static short *indices[MAXSTUDIOTRIANGLES][3];
				//int k = 0;

				//// build all triangles
				//while (i = *(ptricmds++))
				//{
				//	bool strip;
				//	int n;

				//	if (i < 0)
				//	{
				//		strip = false;
				//		i = -i;
				//	}
				//	else
				//	{
				//		strip = true;
				//	}

				//	static short *r_commands[1000];

				//	for (n = 0; i > 0; i--, n++, ptricmds += 4)
				//	{
				//		short *cmd[3];
				//		vec3_t uv[3];
				//		vec3_t edge1, edge2, delta1, delta2;
				//		vec3_t tangent;

				//		r_commands[n] = ptricmds;

				//		// calculate normals
				//		VectorRotate(pstudionorms[ptricmds[1]], g_bonetransform[pnormbone[ptricmds[1]]], r_studionormal[ptricmds[0]]);
				//		VectorNormalize(r_studionormal[ptricmds[0]]);

				//		if (n < 2)
				//		{
				//			continue;
				//		}

				//		if (strip)
				//		{
				//			if (n % 2)
				//			{
				//				indices[k][0] = r_commands[n-1];
				//				indices[k][1] = r_commands[n-2];
				//				indices[k][2] = r_commands[ n ];
				//			}
				//			else
				//			{
				//				indices[k][0] = r_commands[n-2];
				//				indices[k][1] = r_commands[n-1];
				//				indices[k][2] = r_commands[ n ];
				//			}
				//		}
				//		else
				//		{
				//			indices[k][0] = r_commands[ 0 ];
				//			indices[k][1] = r_commands[n-1];
				//			indices[k][2] = r_commands[ n ];
				//		}

				//		// calculate tangent
				//		cmd[0] = indices[k][0];
				//		cmd[1] = indices[k][1];
				//		cmd[2] = indices[k][2];

				//		for (int w = 0; w < 3; w++)
				//		{
				//			uv[w][0] = cmd[w][2] * s;
				//			uv[w][1] = cmd[w][3] * t;
				//		}

				//		VectorSubtract(g_pxformverts[cmd[1][0]], g_pxformverts[cmd[0][0]], edge1);
				//		VectorSubtract(g_pxformverts[cmd[2][0]], g_pxformverts[cmd[0][0]], edge2);

				//		VectorSubtract(uv[1], uv[0], delta1);
				//		VectorSubtract(uv[2], uv[0], delta2);

				//		float f = 1.0f / (delta1[0] * delta2[1] - delta2[0] * delta1[1]);

				//		tangent[0] = f * (delta2[1] * edge1[0] - delta1[1] * edge2[0]);
				//		tangent[1] = f * (delta2[1] * edge1[1] - delta1[1] * edge2[1]);
				//		tangent[2] = f * (delta2[1] * edge1[2] - delta1[1] * edge2[2]);

				//		VectorNormalize(tangent);

				//		// FIXME:
				//		VectorRotate(tangent, g_bonetransform[pnormbone[cmd[0][1]]], r_studiotangent[cmd[0][0]]);

				//		k++;
				//	}
				//}

				//// draw all triangles
				//for (int j = 0; j < k; j++)
				//{
				//	glBegin(GL_TRIANGLES);

				//	for (int l = 0; l < 3; l++)
				//	{
				//		short *cmd = indices[j][l];

				//		glTexCoord2f(cmd[2] * s, cmd[3] * t);

				//		// FIXME:
				//		glVertexAttrib3fARB(attribTangent, r_studiotangent[indices[j][0][0]][0], r_studiotangent[indices[j][0][0]][1], r_studiotangent[indices[j][0][0]][2]);
				//		//glVertexAttrib3fARB(attribTangent, 1, 0, 0);
				//		glNormal3f(r_studionormal[cmd[0]][0], r_studionormal[cmd[0]][1], r_studionormal[cmd[0]][2]);

				//		av = g_pxformverts[cmd[0]];
				//		glVertex3f(av[0], av[1], av[2]);
				//	}

				//	glEnd();
				//}

				// THE TRIANGLE DRAWING COMMAND
				// ------------------------------------------------------
				// command[0]  - Number of vertex ( And drawing mode )
				// ------------------------------------------------------
				// command[1]  - Vertex position index in the array
				// command[2]  - Normal index in the array
				// command[3]  - Texture UV.s
				// command[4]  - Texture UV.t
				// ------------------------------------------------------
				// NEXT COMMAND ...
			}
			else
			{
				while (i = *(ptricmds++))
				{
					if (i < 0)
					{
						glBegin( GL_TRIANGLE_FAN );
						i = -i;
					}
					else
					{
						glBegin( GL_TRIANGLE_STRIP );
					}


					for( ; i > 0; i--, ptricmds += 4)
					{
						// FIX: put these in as integer coords, not floats
						glTexCoord2f(ptricmds[2]*s, ptricmds[3]*t);
						
						lv = g_pvlightvalues[ptricmds[1]];
						glColor4f( lv[0], lv[1], lv[2], 1.0 );

						av = g_pxformverts[ptricmds[0]];
						glVertex3f(av[0], av[1], av[2]);
					}
					glEnd( );
				}
			}
		}

		if ( useShader )
		{
			glUseProgramObjectARB(0);
		}

		glActiveTextureARB(GL_TEXTURE0_ARB);
	}*/
}

