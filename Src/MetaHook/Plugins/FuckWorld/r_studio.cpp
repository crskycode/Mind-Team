
#include <metahook.h>
#include "plugins.h"
#include <cvardef.h>
#include <triangleapi.h>
#include <studio.h>
#include <com_model.h>
#include "mathlib.h"
#include "qgl.h"

// studio.h
#define STUDIO_NF_ADDITIVE      0x0020	//334
#define STUDIO_NF_MASKED        0x0040	//335

// gl_rmain.c
cl_entity_t			**currententity = (cl_entity_t **)0x2C2023C;	//22 (2C2023C)
int					*c_alias_polys = (int *)0x2C202FC;	//29 (2C202FC)
float				*r_blend = (float *)0x2788E14;	//55 (2788E14)
cvar_t				*r_fullbright = (cvar_t *)0x1EC7608;	//79 (1EC7608)

// cl_tent.c
model_t				**cl_sprite_white = (model_t **)0x2D5E168;	// (2D5E168)

// r_studio.c
studiohdr_t			**pstudiohdr = (studiohdr_t **)0x24849C0;	//53 (24849C0)
vec3_t				*r_colormix = (vec3_t *)0x27B1800;	//57 (27B1800)
mstudiomodel_t		**psubmodel = (mstudiomodel_t **)0x2452EE8;	//1168 (2452EE8)
model_t				**r_model = (model_t **)0x2435490;	//1171 (2435490)
int					*g_ForcedFaceFlags = (int *)0x246E238;	//1173 (246E238)
int					(*chrome)[MAXSTUDIOVERTS][2] = (int (*)[MAXSTUDIOVERTS][2])0x244CEE0;	//1175 (244CEE0)
int					(*g_NormalIndex)[MAXSTUDIOVERTS] = (int (*)[MAXSTUDIOVERTS])0x2450EE8;	//1176 (2450EE8)
float				(*lightpos)[MAXSTUDIOVERTS][3][4] = (float (*)[MAXSTUDIOVERTS][3][4])0x2454D08;	//1185 (2454D08)
auxvert_t			**pauxverts = (auxvert_t **)0x278E7A0;	//1190 (278E7A0)
vec3_t				**pvlightvalues = (vec3_t **)0x24849C8;	//3791 (24849C8)


// R_LoadTextures (sub_1D8DC20)
studiohdr_t *(*R_LoadTextures)(model_t *psubmodel) = (studiohdr_t *(*)(model_t *))0x1D8DC20;
// BuildNormalIndexTable (sub_1D8BAD0)
void (*BuildNormalIndexTable)(void) = (void (*)(void))0x1D8BAD0;
// BuildGlowShellVerts (sub_1D8BB50)
void (*BuildGlowShellVerts)(vec3_t *pstudioverts, auxvert_t *pauxverts) = (void (*)(vec3_t *, auxvert_t *))0x1D8BB50;
// R_StudioTransformAuxVert (sub_1D8D4B0)
void (*R_StudioTransformAuxVert)(auxvert_t *av, int bone, vec3_t vert) = (void (*)(auxvert_t *, int, vec3_t))0x1D8D4B0;
// R_LightStrength (sub_1D8D700)
void (*R_LightStrength)(int bone, float *vert, float light[3][4]) = (void (*)(int, float *, float [][4]))0x1D8D700;
// R_StudioChrome (sub_1D8DAA0)
void (*R_StudioChrome)(int *pchrome, int bone, vec3_t normal) = (void (*)(int *, int, vec3_t))0x1D8DAA0;
// R_StudioLighting (sub_1D8D540)
void (*R_StudioLighting)(float *lv, int bone, int flags, vec3_t normal) = (void (*)(float *, int, int, vec3_t))0x1D8D540;
// IsFlippedViewModel (sub_1D8BCA0)
int (*IsFlippedViewModel)(void) = (int (*)(void))0x1D8BCA0;
// R_StudioSetupSkin (sub_1D8E250)
void (*R_StudioSetupSkin)(studiohdr_t *ptexturehdr, int index) = (void (*)(studiohdr_t *, int))0x1D8E250;
// R_LightLambert (sub_1D8D7F0)
void (*R_LightLambert)(float light[3][4], float *normal, float *src, float *lambert) = (void (*)(float light[][4], float *, float *, float *))0x1D8D7F0;


void R_GLStudioDrawPoints ( )
{
	int					i, j;
	byte				*pvertbone;
	byte				*pnormbone;
	vec3_t				*pstudioverts;
	vec3_t				*pstudionorms;
	studiohdr_t			*ptexturehdr;
	mstudiotexture_t	*ptexture;
	mstudiomesh_t		*pmesh;
	auxvert_t 			*av;
	float 				*lv;
	vec3_t				fl;
	float				lv_tmp;
	short				*pskinref;
	int					flags;
	int					iFlippedVModel = 0;

	pvertbone = ((byte *)(*pstudiohdr) + (*psubmodel)->vertinfoindex);
	pnormbone = ((byte *)(*pstudiohdr) + (*psubmodel)->norminfoindex);
	ptexturehdr = R_LoadTextures( (*r_model) );
	ptexture = (mstudiotexture_t *)((byte *)ptexturehdr + ptexturehdr->textureindex);

	pmesh = (mstudiomesh_t *)((byte *)(*pstudiohdr) + (*psubmodel)->meshindex);

	pstudioverts = (vec3_t *)((byte *)(*pstudiohdr) + (*psubmodel)->vertindex);
	pstudionorms = (vec3_t *)((byte *)(*pstudiohdr) + (*psubmodel)->normindex);

	pskinref = (short *)((byte *)ptexturehdr + ptexturehdr->skinindex);
	if ((*currententity)->curstate.skin != 0 && (*currententity)->curstate.skin < ptexturehdr->numskinfamilies)
		pskinref += ((*currententity)->curstate.skin * ptexturehdr->numskinref);

	if ((*currententity)->curstate.renderfx == kRenderFxGlowShell)
	{
		BuildNormalIndexTable();
		BuildGlowShellVerts( pstudioverts, (*pauxverts) );
	}
	else
	{
		for (i = 0; i < (*psubmodel)->numverts; i++)
		{
			av = &(*pauxverts)[i];
			R_StudioTransformAuxVert( av, pvertbone[i], pstudioverts[i] );
		}
	}

	for (i = 0; i < (*psubmodel)->numverts; i++)
	{
		R_LightStrength( pvertbone[i], pstudioverts[i], (*lightpos)[i] );
	}

//
// clip and draw all triangles
//

	lv = (float *)(*pvlightvalues);
	for (j = 0; j < (*psubmodel)->nummesh; j++) 
	{
		int k;

		flags = ptexture[pskinref[pmesh[j].skinref]].flags | (*g_ForcedFaceFlags);
		if ((*r_fullbright).value >= 2)
			flags &= ~STUDIO_NF_FULLBRIGHT;

		if ((*currententity)->curstate.rendermode == kRenderTransAdd)
		{
			for (k = 0; k < pmesh[j].numnorms; k++, lv += 3, pstudionorms++, pnormbone++)
			{
				lv[0] = (*r_blend);
				lv[1] = (*r_blend);
				lv[2] = (*r_blend);

				if (flags & STUDIO_NF_CHROME)
					R_StudioChrome( (*chrome)[(float (*)[3])lv - (*pvlightvalues)], *pnormbone, (float *)pstudionorms );
			}
		}
		else
		{
			for (k = 0; k < pmesh[j].numnorms; k++, lv += 3, pstudionorms++, pnormbone++)
			{
				R_StudioLighting (&lv_tmp, *pnormbone, flags, (float *)pstudionorms);

				// FIX: move this check out of the inner loop
				if (flags & STUDIO_NF_CHROME)
					R_StudioChrome( (*chrome)[(float (*)[3])lv - (*pvlightvalues)], *pnormbone, (float *)pstudionorms );

				lv[0] = lv_tmp * (*r_colormix)[0];
				lv[1] = lv_tmp * (*r_colormix)[1];
				lv[2] = lv_tmp * (*r_colormix)[2];
			}
		}
	}

	qglCullFace(GL_FRONT);

	if ( IsFlippedViewModel() == 1 )
	{
		qglDisable(GL_CULL_FACE);
		iFlippedVModel = 1;
	}

	pstudionorms = (vec3_t *)((byte *)(*pstudiohdr) + (*psubmodel)->normindex);

	for (j = 0; j < (*psubmodel)->nummesh; j++) 
	{
		float s, t;
		short		*ptricmds;

		pmesh = (mstudiomesh_t *)((byte *)(*pstudiohdr) + (*psubmodel)->meshindex) + j;
		ptricmds = (short *)((byte *)(*pstudiohdr) + pmesh->triindex);

		(*c_alias_polys) += pmesh->numtris;

		flags = ptexture[pskinref[pmesh->skinref]].flags | (*g_ForcedFaceFlags);
		if ((*r_fullbright).value >= 2)
			flags &= STUDIO_NF_FULLBRIGHT;

		if (flags & STUDIO_NF_MASKED)
		{
			qglEnable(GL_ALPHA_TEST);
			qglAlphaFunc(GL_GREATER, 0.5);
			qglDepthMask(1);
		}

		if ((flags & STUDIO_NF_ADDITIVE) && (*currententity)->curstate.rendermode == kRenderNormal)
		{
			qglBlendFunc(GL_ONE, GL_ONE);
			qglEnable(GL_BLEND);
			qglDepthMask(0);
			qglShadeModel(GL_SMOOTH);
		}

		if ((*r_fullbright).value >= 2)
		{
			gEngfuncs.pTriAPI->SpriteTexture( (*cl_sprite_white), 0 );

			t = 1.0/256;
			s = 1.0/256;
		}
		else
		{
			s = 1.0/(float)ptexture[pskinref[pmesh->skinref]].width;
			t = 1.0/(float)ptexture[pskinref[pmesh->skinref]].height;

			R_StudioSetupSkin( ptexturehdr, pskinref[pmesh->skinref] );
		}

		if (flags & STUDIO_NF_CHROME)
		{
			s *= 1.0/1024;
			t *= 1.0/1024;

			if ((*g_ForcedFaceFlags) & STUDIO_NF_CHROME)
			{
				while (i = *(ptricmds++))
				{
					if (i < 0)
					{
						qglBegin( GL_TRIANGLE_FAN );
						i = -i;
					}
					else
					{
						qglBegin( GL_TRIANGLE_STRIP );
					}


					for( ; i > 0; i--, ptricmds += 4)
					{
						int normalIndex;

						// FIX: put these in as integer coords, not floats
						normalIndex = (*g_NormalIndex)[ptricmds[0]];
						qglTexCoord2f((*chrome)[normalIndex][0]*s, (*chrome)[normalIndex][1]*t);

						av = &(*pauxverts)[ptricmds[0]];
						qglVertex3f(av->fv[0], av->fv[1], av->fv[2]);
					}

					qglEnd( );
				}
			}
			else
			{
				while (i = *(ptricmds++))
				{
					if (i < 0)
					{
						qglBegin( GL_TRIANGLE_FAN );
						i = -i;
					}
					else
					{
						qglBegin( GL_TRIANGLE_STRIP );
					}


					for( ; i > 0; i--, ptricmds += 4)
					{
						// FIX: put these in as integer coords, not floats
						qglTexCoord2f((*chrome)[ptricmds[1]][0]*s, (*chrome)[ptricmds[1]][1]*t);

						lv = (*pvlightvalues)[ptricmds[1]];
						R_LightLambert( (*lightpos)[ptricmds[0]], pstudionorms[ptricmds[1]], lv, fl );
						qglColor4f( fl[0], fl[1], fl[2], (*r_blend) );

						av = &(*pauxverts)[ptricmds[0]];
						qglVertex3f(av->fv[0], av->fv[1], av->fv[2]);
					}

					qglEnd( );
				}	
			}
		}
		else 
		{
			while (i = *(ptricmds++))
			{
				if (i < 0)
				{
					qglBegin( GL_TRIANGLE_FAN );
					i = -i;
				}
				else
				{
					qglBegin( GL_TRIANGLE_STRIP );
				}


				for( ; i > 0; i--, ptricmds += 4)
				{
					// FIX: put these in as integer coords, not floats
					qglTexCoord2f(ptricmds[2]*s, ptricmds[3]*t);

					lv = (*pvlightvalues)[ptricmds[1]];
					if ( iFlippedVModel == 1 )
					{
						vec3_t vNormal;
						VectorCopy( pstudionorms[ptricmds[1]], vNormal );
						VectorScale( vNormal, -1, vNormal );
						R_LightLambert( (*lightpos)[ptricmds[0]], vNormal, lv, fl );
					}
					else
					{
						R_LightLambert( (*lightpos)[ptricmds[0]], pstudionorms[ptricmds[1]], lv, fl );
					}
					qglColor4f( fl[0], fl[1], fl[2], (*r_blend) );

					av = &(*pauxverts)[ptricmds[0]];
					qglVertex3f(av->fv[0], av->fv[1], av->fv[2]);
				}

				qglEnd( );
			}	
		}

		if (flags & STUDIO_NF_MASKED)
		{
			qglAlphaFunc(GL_NOTEQUAL, 0);
			qglDisable(GL_ALPHA_TEST);
		}

		if ((flags & STUDIO_NF_ADDITIVE) && (*currententity)->curstate.rendermode == kRenderNormal)
		{
			qglDisable(GL_BLEND);
			qglDepthMask(1);
			qglShadeModel(GL_FLAT);
		}
	}

	qglEnable(GL_CULL_FACE);
}

void (*g_pfnR_GLStudioDrawPoints)(void) = NULL;

void Studio_InstallHook(void)
{
	InlineHook( (void*)0x1D90660, &R_GLStudioDrawPoints, (void *&)g_pfnR_GLStudioDrawPoints );
}