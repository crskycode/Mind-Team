/****
 * 
 * MetaStudio is an improved MDL file format, that supports most of the features of 
 * the original MDL format. It implements skinning animation and keyframe animation.
 * Just storing little skeleton data can show very smooth animation (using quaternion 
 * for rotation interpolation).
 * This project is developed and shared by Crsky, and all explanatory rights are owned by Crsky.
 * 
 * Welcome anyone to improve this project or make suggestions to improve this project, 
 * if you can contact me, I will be very grateful to you.
 * 
 * Email : crskycode@foxmail.com
 * Date : 2017-8-29
 * 
****/

#include "cl_dll.h"
#include <com_model.h>
#include <r_studioint.h>
#include "util.h"
#include "qgl.h"
#include "shader.h"

#include "MetaStudioRenderer.h"
#include "MetaStudioMath.h"

#include "MetaHook_Interface.h"

// The instance
CMetaStudioRenderer g_MetaStudioRenderer;


engine_studio_api_t IEngineStudio;


static vec3_t vec3_origin = { 0.0f, 0.0f, 0.0f };

// make pointer
#define STUDIOHDR( base, offset ) ( (byte *)( base ) + ( offset ) )


CMetaStudioRenderer::CMetaStudioRenderer()
{
}

CMetaStudioRenderer::~CMetaStudioRenderer()
{
}

void CMetaStudioRenderer::Init(void)
{
	m_shader.program = R_CompileShader("shaders/metastudio.vsh", "shaders/metastudio.fsh");

	if (m_shader.program)
	{
		qglUseProgram(m_shader.program);

		m_shader.position = qglGetAttribLocation(m_shader.program, "position");
		m_shader.normal = qglGetAttribLocation(m_shader.program, "normal");
		m_shader.texcoord = qglGetAttribLocation(m_shader.program, "texcoord");
		m_shader.numbones = qglGetAttribLocation(m_shader.program, "numbones");
		m_shader.bones = qglGetAttribLocation(m_shader.program, "bones");
		m_shader.weights = qglGetAttribLocation(m_shader.program, "weights");

		m_shader.bonetransform = qglGetUniformLocation(m_shader.program, "bonetransform");
		m_shader.diffusemap = qglGetUniformLocation(m_shader.program, "diffusemap");

		qglUseProgram(0);
	}
}

void CMetaStudioRenderer::GetKeyFrame(vec3_t *pos, vec4_t *quat, metastudioanim_t *panim, int index)
{
	int						i;
	metastudioanimvalue_t	*pv;

	pv = (metastudioanimvalue_t *)STUDIOHDR(m_pStudioHeader, panim->valueindex);

	pv += index * panim->numbones;

	for (i = 0; i < panim->numbones; i++)
	{
		VectorCopy(pv[i].pos, pos[i]);
		VectorCopy4(pv[i].quat, quat[i]);
	}
}

void CMetaStudioRenderer::CalcRotations(vec3_t *pos, vec4_t *quat, metastudioanim_t *panim, int frametime)
{
	int		i, j;
	int		*ptimes;
	int		start, end;
	int		cur, nxt;

	static vec3_t pos1[META_STUDIO_BONES];
	static vec4_t quat1[META_STUDIO_BONES];
	static vec3_t pos2[META_STUDIO_BONES];
	static vec4_t quat2[META_STUDIO_BONES];

	ptimes = (int *)STUDIOHDR(m_pStudioHeader, panim->timeindex);

	start = ptimes[0];
	end = ptimes[panim->numkeyframes - 1];

	if (frametime < start)
		frametime = start;
	else if (frametime > end)
		frametime = end;

	// This animation only one keyframe
	if (panim->numkeyframes == 1)
	{
		GetKeyFrame(pos, quat, panim, 0);
		return;
	}

	for (i = 0; i < panim->numkeyframes - 1; i++)
	{
		cur = i;
		nxt = i + 1;

		if (ptimes[cur] == frametime)
		{
			GetKeyFrame(pos, quat, panim, cur);
			break;
		}
		else if (ptimes[nxt] == frametime)
		{
			GetKeyFrame(pos, quat, panim, nxt);
			break;
		}
		else if (ptimes[cur] < frametime && frametime < ptimes[nxt])
		{
			int t1, t2;
			float t;

			GetKeyFrame(pos1, quat1, panim, cur);
			GetKeyFrame(pos2, quat2, panim, nxt);

			t1 = ptimes[cur];
			t2 = ptimes[nxt];

			t = (float)(frametime - t1) / (float)(t2 - t1);

			for (j = 0; j < panim->numbones; j++)
			{
				GL_VectorSlerp(pos1[j], pos2[j], t, pos[j]);
				GL_QuaternionSlerp(quat1[j], quat2[j], t, quat[j]);
			}

			break;
		}
	}
}

int CMetaStudioRenderer::EstimateFrame(metastudioseqdesc_t *pseqdesc)
{
	double	dfdt, f;

	if (m_clTime < m_pCurrentEntity->curstate.animtime)
		dfdt = 0;
	else
		dfdt = (m_clTime - m_pCurrentEntity->curstate.animtime) * m_pCurrentEntity->curstate.framerate * pseqdesc->fps;

	if (pseqdesc->duration <= 1)
		f = 0;
	else
		f = m_pCurrentEntity->curstate.frame / 256.0 * pseqdesc->duration;

	f += dfdt;

	if (f < 0)
		f = 0;

	if (f > pseqdesc->duration)
	{
		if (pseqdesc->flags & META_STUDIO_LOOPING)
		{
			f = (int)f % (pseqdesc->duration + 1);
		}
		else
		{
			f = pseqdesc->duration;
		}
	}

	return (int)f;
}

void CMetaStudioRenderer::SetupBones(void)
{
	int					i;
	float				f;
	metastudioseqdesc_t	*pseqdesc;
	metastudioanim_t	*panim;
	metastudiobone_t	*pbone;

	static vec3_t		pos[META_STUDIO_BONES];
	static vec4_t		quat[META_STUDIO_BONES];

	if (m_pStudioHeader->numseq <= 0)
	{
		gEngfuncs.Con_Printf("not any sequences in (%s)\n", m_pCurrentEntity->model->name);
		return;
	}

	if (m_pCurrentEntity->curstate.sequence >= m_pStudioHeader->numseq)
		m_pCurrentEntity->curstate.sequence = 0;

	pseqdesc = (metastudioseqdesc_t *)STUDIOHDR(m_pStudioHeader, m_pStudioHeader->seqindex) + m_pCurrentEntity->curstate.sequence;

	panim = (metastudioanim_t *)STUDIOHDR(m_pStudioHeader, pseqdesc->animindex);

	f = EstimateFrame(pseqdesc);

	CalcRotations(pos, quat, panim, f);

	pbone = (metastudiobone_t *)STUDIOHDR(m_pStudioHeader, m_pStudioHeader->boneindex);

	for (i = 0; i < m_pStudioHeader->numbones; i++)
	{
		mat4_t	m;
		int		parent;

		GL_QuaternionMatrix(quat[i], m);
		VectorCopy(pos[i], m[3]);

		parent = pbone[i].parent;

		if (parent == -1)
			GL_ConcatTransforms(m_rotatematrix, m, m_bonematrix[i]);
		else
			GL_ConcatTransforms(m_bonematrix[parent], m, m_bonematrix[i]);
	}
}

void CMetaStudioRenderer::SetupSkin(void)
{
	int		i;
	metastudiobone_t	*pbone;
	mat4_t	*poffsetmat;

	pbone = (metastudiobone_t *)STUDIOHDR(m_pStudioHeader, m_pStudioHeader->boneindex);

	poffsetmat = (mat4_t *)STUDIOHDR(m_pStudioHeader, m_pModel->matindex);

	for (i = 0; i < m_pStudioHeader->numbones; i++)
	{
		GL_ConcatTransforms(m_bonematrix[i], poffsetmat[i], m_skinmatrix[i]);
	}
}

void CMetaStudioRenderer::SetupModel(int bodypart)
{
	int		index;
	metastudiobodyparts_t	*pbodypart;

	if (bodypart >= m_pStudioHeader->numbodyparts)
	{
		bodypart = 0;
	}

	pbodypart = (metastudiobodyparts_t *)STUDIOHDR(m_pStudioHeader, m_pStudioHeader->bodypartindex) + bodypart;

	index = m_pCurrentEntity->curstate.body / pbodypart->base;
	index = index % pbodypart->nummodels;

	m_pModel = (metastudiomodel_t *)STUDIOHDR(m_pStudioHeader, pbodypart->modelindex) + index;
}

void CMetaStudioRenderer::DrawPoints( void )
{
	int						i, j;
	metastudiomesh_t		*pmesh;
	metastudiotexture_t		*ptexture;

	if (!m_shader.program)
		return;

	if (!m_pModel->vbo)
		return;

	pmesh = (metastudiomesh_t *)STUDIOHDR(m_pStudioHeader, m_pModel->meshindex);

	ptexture = (metastudiotexture_t *)STUDIOHDR(m_pStudioHeader, m_pStudioHeader->textureindex);

	qglUseProgram(m_shader.program);

	qglBindBuffer(GL_ARRAY_BUFFER, m_pModel->vbo);

	qglEnableVertexAttribArray(m_shader.position);	// float[3]
	qglEnableVertexAttribArray(m_shader.normal);	// float[3]
	qglEnableVertexAttribArray(m_shader.texcoord);	// float[2]
	qglEnableVertexAttribArray(m_shader.numbones);	// float[1]
	qglEnableVertexAttribArray(m_shader.bones);		// float[4]
	qglEnableVertexAttribArray(m_shader.weights);	// float[4]

	qglVertexAttribPointer(m_shader.position, 3, GL_FLOAT, GL_FALSE, sizeof(metastudiovertex_t), META_STUDIO_POINTER_POSITION);
	qglVertexAttribPointer(m_shader.normal, 3, GL_FLOAT, GL_FALSE, sizeof(metastudiovertex_t), META_STUDIO_POINTER_NORMAL);
	qglVertexAttribPointer(m_shader.texcoord, 2, GL_FLOAT, GL_FALSE, sizeof(metastudiovertex_t), META_STUDIO_POINTER_TEXCOORD);
	qglVertexAttribPointer(m_shader.numbones, 1, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(metastudiovertex_t), META_STUDIO_POINTER_NUMBONES);
	qglVertexAttribPointer(m_shader.bones, 4, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(metastudiovertex_t), META_STUDIO_POINTER_BONES);
	qglVertexAttribPointer(m_shader.weights, 4, GL_FLOAT, GL_FALSE, sizeof(metastudiovertex_t), META_STUDIO_POINTER_WEIGHTS);

	qglUniformMatrix4fv(m_shader.bonetransform, max(1, m_pStudioHeader->numbones), GL_FALSE, (GLfloat *)m_skinmatrix);

	qglUniform1i(m_shader.diffusemap, 0);

	for (i = 0; i < m_pModel->nummesh; i++, pmesh++)
	{
		if (ptexture[pmesh->texref[0]].glt)
			qglBindTexture(GL_TEXTURE_2D, ptexture[pmesh->texref[0]].glt);
		else
			qglBindTexture(GL_TEXTURE_2D, 0);

		if (!pmesh->ebo)
			continue;

		qglBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pmesh->ebo);

		qglDrawElements(GL_TRIANGLES, pmesh->numtris*3, GL_UNSIGNED_SHORT, NULL);
	}

	qglDisableVertexAttribArray(m_shader.position);
	qglDisableVertexAttribArray(m_shader.normal);
	qglDisableVertexAttribArray(m_shader.texcoord);
	qglDisableVertexAttribArray(m_shader.numbones);
	qglDisableVertexAttribArray(m_shader.bones);
	qglDisableVertexAttribArray(m_shader.weights);

	qglBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	qglBindBuffer(GL_ARRAY_BUFFER, 0);

	qglUseProgram(0);
}

void CMetaStudioRenderer::RenderModel(void)
{
	int		i;

	for (i = 0; i < m_pStudioHeader->numbodyparts; i++)
	{
		SetupModel(i);
		SetupSkin();

		//GL_SetRenderMode();
		DrawPoints();
	}
}

void CMetaStudioRenderer::SetUpTransform(void)
{
	vec3_t	origin;
	vec3_t	angles;

	VectorCopy(m_pCurrentEntity->curstate.origin, origin);
	VectorCopy(m_pCurrentEntity->curstate.angles, angles);

	GL_AngleMatrix(angles, m_rotatematrix);
	VectorCopy(origin, m_rotatematrix[3]);
}

// Fake bboxes for models
static vec3_t gFakeHullMins = { -16, -16, -16 };
static vec3_t gFakeHullMaxs = { 16, 16, 16 };

bool CMetaStudioRenderer::CheckBBox(void)
{
	vec3_t	mins, maxs;

	if (!VectorCompare(vec3_origin, m_pStudioHeader->bbmin))
	{
		VectorAdd(m_pCurrentEntity->curstate.origin, m_pStudioHeader->bbmin, mins);
		VectorAdd(m_pCurrentEntity->curstate.origin, m_pStudioHeader->bbmax, maxs);
	}
	else if (!VectorCompare(vec3_origin, m_pStudioHeader->min))
	{
		VectorAdd(m_pCurrentEntity->curstate.origin, m_pStudioHeader->min, mins);
		VectorAdd(m_pCurrentEntity->curstate.origin, m_pStudioHeader->max, maxs);
	}
	else
	{
		VectorAdd(m_pCurrentEntity->curstate.origin, gFakeHullMins, mins);
		VectorAdd(m_pCurrentEntity->curstate.origin, gFakeHullMaxs, maxs);
	}

	if (!gpMTEngine->R_CullBox(mins, maxs))
		return true;

	return false;
}

int CMetaStudioRenderer::DrawModel(int flags)
{
	// update the clock
	IEngineStudio.GetTimes(&m_nFrameCount, &m_clTime, &m_clOldTime);
	m_clFrameTime = m_clTime - m_clOldTime;

	m_pCurrentEntity = IEngineStudio.GetCurrentEntity();

	if (m_pCurrentEntity->curstate.renderfx == kRenderFxDeadPlayer)
		return 1;

	if (m_pCurrentEntity->curstate.movetype == MOVETYPE_FOLLOW)
		return 1;

	m_pRenderModel = m_pCurrentEntity->model;

	m_pStudioHeader = (metastudiohdr_t *)IEngineStudio.Mod_Extradata(m_pRenderModel);

	if (!(m_pStudioHeader->flags & META_STUDIO))
		return 1;

	if (flags & STUDIO_RENDER)
	{
		//	if (m_pStudioHeader->numbodyparts == 0)
		//		return 1;

		if (CheckBBox() == false)
			return 1;
	}

	LoadModel();

	SetUpTransform();

	SetupBones();

	if (flags & STUDIO_EVENTS)
	{
		//StudioCalcAttachments();
		//StudioClientEvents();
	}

	if (flags & STUDIO_RENDER)
	{
		//StudioSetupLighting();
		RenderModel();
		//DrawHulls();
	}

	return 1;
}

// Max engine players
#define MAX_PLAYERS 32

// The angles of feet
vec3_t	player_feet_angles[ MAX_PLAYERS+1 ] = { 0 };
// If true, the feet_angles approach to entity->curstate.angles
bool	player_feet_follow[ MAX_PLAYERS+1 ] = { false };

void CMetaStudioRenderer::SetupPlayerBones(void)
{
	int						i;
	int						f;
	metastudioseqdesc_t *	pseqdesc;
	metastudioanim_t *		panim;
	metastudiobone_t *		pbone;

	static vec3_t	pos[META_STUDIO_BONES];
	static vec4_t	quat[META_STUDIO_BONES];


	if (m_pStudioHeader->numseq <= 0)
	{
		gEngfuncs.Con_Printf("not any sequences in (%s)\n", m_pCurrentEntity->model->name);
		return;
	}

	if (m_pCurrentEntity->curstate.sequence >= m_pStudioHeader->numseq)
		m_pCurrentEntity->curstate.sequence = 0;

	pseqdesc = (metastudioseqdesc_t *)STUDIOHDR(m_pStudioHeader, m_pStudioHeader->seqindex) + m_pCurrentEntity->curstate.sequence;

	panim = (metastudioanim_t *)STUDIOHDR(m_pStudioHeader, pseqdesc->animindex);	//anim[0]

	f = EstimateFrame(pseqdesc);

	CalcRotations(pos, quat, panim, f);

	pbone = (metastudiobone_t *)STUDIOHDR(m_pStudioHeader, m_pStudioHeader->boneindex);

	// [03] "M-bone Spine"
	// [04] "M-bone Spine1"
	const int spine = 3;
	const int spine1 = 4;
	{
		float		pitch, yaw;
		float		delta;
		vec4_t		q, tmp;
		vec3_t		axis;

		// axis[0]  -left/right
		// axis[1]  -up/down
		// axis[2]  -roll

		pitch = m_pCurrentEntity->curstate.angles[PITCH] * -3;
		yaw = m_pCurrentEntity->curstate.angles[YAW];

		if (pitch < -45.0)
			pitch = -45.0;
		else if (pitch > 45.0)
			pitch = 45.0;

		// rotate for look up/down
		axis[0] = axis[2] = 0;
		axis[1] = 1;
		// "Spine1"
		GL_AxisQuaternion(axis, pitch, q);
		GL_QuaternionMul(q, quat[spine1], tmp);
		VectorCopy4(tmp, quat[spine1]);
		// "Spine"
		GL_AxisQuaternion(axis, pitch*0.46, q);
		GL_QuaternionMul(q, quat[spine], tmp);
		VectorCopy4(tmp, quat[spine]);

		//if (pitch < - 20)		// maby need to adjust 'Clavicle'
		//{
		//}

		delta = UTIL_AngleDistance(yaw, player_feet_angles[m_pCurrentEntity->index][YAW]);

		// rotate for look left/right
		axis[1] = axis[2] = 0;
		axis[0] = 1;
		// "Spine"
		GL_AxisQuaternion(axis, delta, q);
		GL_QuaternionMul(q, quat[spine], tmp);
		VectorCopy4(tmp, quat[spine]);
	}

	for (i = 0; i < m_pStudioHeader->numbones; i++)
	{
		mat4_t	m;
		int		parent;

		GL_QuaternionMatrix(quat[i], m);
		VectorCopy(pos[i], m[3]);

		parent = pbone[i].parent;

		if (parent == -1)
			GL_ConcatTransforms(m_rotatematrix, m, m_bonematrix[i]);
		else
			GL_ConcatTransforms(m_bonematrix[parent], m, m_bonematrix[i]);
	}
}

void CMetaStudioRenderer::SetupAttachmentBones(int parentBone)
{
	int						i;
	metastudioseqdesc_t *	pseqdesc;
	metastudioanim_t *		panim;
	metastudiobone_t *		pbone;

	static vec3_t	pos[META_STUDIO_BONES];
	static vec4_t	quat[META_STUDIO_BONES];

	if (m_pStudioHeader->numseq == 0)
	{
		gEngfuncs.Con_Printf("not any sequences in (%s)\n", m_pStudioHeader->name);
		return;
	}

	pseqdesc = (metastudioseqdesc_t *)STUDIOHDR(m_pStudioHeader, m_pStudioHeader->seqindex);	// just sequence[0]

	panim = (metastudioanim_t *)STUDIOHDR(m_pStudioHeader, pseqdesc->animindex);	// anim[0]

	CalcRotations(pos, quat, panim, 0);		// just frame[0]

	pbone = (metastudiobone_t *)STUDIOHDR(m_pStudioHeader, m_pStudioHeader->boneindex);

	for (i = 0; i < m_pStudioHeader->numbones; i++)
	{
		mat4_t	m;
		int		parent;

		GL_QuaternionMatrix(quat[i], m);
		VectorCopy(pos[i], m[3]);

		parent = pbone[i].parent;

		if (parent == -1)
			GL_ConcatTransforms(m_savematrix[parentBone], m, m_bonematrix[i]);
		else
			GL_ConcatTransforms(m_bonematrix[parent], m, m_bonematrix[i]);
	}
}

void CMetaStudioRenderer::SavePlayerBone(int bone)
{
	GL_MatrixCopy(m_bonematrix[bone], m_savematrix[bone]);
}

int CMetaStudioRenderer::DrawPlayer(int flags, entity_state_t *player)
{
	vec3_t			angles;
	int				playerIndex;		// player index
	float			dist;

	// update the clock
	IEngineStudio.GetTimes(&m_nFrameCount, &m_clTime, &m_clOldTime);
	m_clFrameTime = m_clTime - m_clOldTime;

	m_pCurrentEntity = IEngineStudio.GetCurrentEntity();

	if (m_pCurrentEntity->curstate.renderfx == kRenderFxDeadPlayer)
		return 1;

	if (m_pCurrentEntity->curstate.movetype == MOVETYPE_FOLLOW)
		return 1;

	m_pRenderModel = m_pCurrentEntity->model;

	m_pStudioHeader = (metastudiohdr_t *)IEngineStudio.Mod_Extradata(m_pRenderModel);

	if (!(m_pStudioHeader->flags & META_STUDIO))
		return 1;

	if (flags & STUDIO_RENDER)
	{
		if (CheckBBox() == false)
			return 1;
	}

	LoadModel();

	//
	// SetUpTransform
	//

	playerIndex = m_pCurrentEntity->index;

	VectorCopy(player_feet_angles[playerIndex], angles);

	// player model is never skew
	angles[PITCH] = 0.0;
	angles[ROLL] = 0.0;

	// make rotate matrix for entity
	GL_AngleMatrix(angles, m_rotatematrix);
	VectorCopy(m_pCurrentEntity->curstate.origin, m_rotatematrix[3]);

	dist = UTIL_AngleDistance(m_pCurrentEntity->curstate.angles[YAW],
		player_feet_angles[playerIndex][YAW]);

	if (dist < -45.0)
	{
		// follow to limits
		player_feet_angles[playerIndex][YAW]
			= UTIL_AngleMod(m_pCurrentEntity->curstate.angles[YAW] + 45.0);

		player_feet_follow[playerIndex] = true;
	}
	else if (dist > 45.0)
	{
		// follow to limits
		player_feet_angles[playerIndex][YAW]
			= UTIL_AngleMod(m_pCurrentEntity->curstate.angles[YAW] - 45.0);

		player_feet_follow[playerIndex] = true;
	}
	else if (dist == 0.000)
	{
		// finished follow
		player_feet_follow[playerIndex] = false;
	}

	SetupPlayerBones();

	if (player_feet_follow[playerIndex])
	{
		player_feet_angles[playerIndex][YAW]
			= UTIL_ApproachAngle(m_pCurrentEntity->curstate.angles[YAW],
				player_feet_angles[playerIndex][YAW], m_clFrameTime * 700.0);	// (1/45)
	}

	if (flags & STUDIO_RENDER)
	{
		RenderModel();
	}

	return 1;
}

int boxpnt[6][4] =
{
	{ 0, 4, 6, 2 }, // +X
	{ 0, 1, 5, 4 }, // +Y
	{ 0, 2, 3, 1 }, // +Z
	{ 7, 5, 1, 3 }, // -X
	{ 7, 3, 2, 6 }, // -Y
	{ 7, 6, 4, 5 }, // -Z
};

vec3_t hullcolor[8] =
{
	{ 1.0, 1.0, 1.0 },
	{ 1.0, 0.5, 0.5 },
	{ 0.5, 1.0, 0.5 },
	{ 1.0, 1.0, 0.5 },
	{ 0.5, 0.5, 1.0 },
	{ 1.0, 0.5, 1.0 },
	{ 0.5, 1.0, 1.0 },
	{ 1.0, 1.0, 1.0 },
};

void CMetaStudioRenderer::DrawHulls(void)
{
	int						i, j;
	metastudiohitbox_t *	pbbox;
	vec3_t					tmp;
	vec3_t					p[8];

	pbbox = (metastudiohitbox_t *)STUDIOHDR(m_pStudioHeader, m_pStudioHeader->hitboxindex);

	gEngfuncs.pTriAPI->RenderMode(kRenderTransColor);

	for (i = 0; i < m_pStudioHeader->numhitboxes; i++)
	{
		for (j = 0; j < 8; j++)
		{
			tmp[0] = (j & 1) ? pbbox[i].bmin[0] : pbbox[i].bmax[0];
			tmp[1] = (j & 2) ? pbbox[i].bmin[1] : pbbox[i].bmax[1];
			tmp[2] = (j & 4) ? pbbox[i].bmin[2] : pbbox[i].bmax[2];

			GL_VectorTransform(tmp, m_bonematrix[pbbox[i].bone], p[j]);
		}

		j = (pbbox[i].group % 8);
		gEngfuncs.pTriAPI->Begin(TRI_QUADS);
		gEngfuncs.pTriAPI->Color4f(1.0, 1.0, 1.0, 0.5);

		for (j = 0; j < 6; j++)
		{
			gEngfuncs.pTriAPI->Vertex3fv(p[boxpnt[j][0]]);
			gEngfuncs.pTriAPI->Vertex3fv(p[boxpnt[j][1]]);
			gEngfuncs.pTriAPI->Vertex3fv(p[boxpnt[j][2]]);
			gEngfuncs.pTriAPI->Vertex3fv(p[boxpnt[j][3]]);
		}

		gEngfuncs.pTriAPI->End();
	}
}

void CMetaStudioRenderer::LoadModel(void)
{
	int						i, j, k;
	metastudiobodyparts_t	*pbodypart;
	metastudiomodel_t		*pmodel;
	metastudiovertex_t		*pvert;
	metastudiomesh_t		*pmesh;
	metastudiotriangle_t	*ptris;
	metastudiotexture_t		*ptexture;
	tex_t					*texinfo;

	if (m_pStudioHeader->flags & META_STUDIO_LOADED)
		return;

	pbodypart = (metastudiobodyparts_t *)STUDIOHDR(m_pStudioHeader, m_pStudioHeader->bodypartindex);

	for (i = 0; i < m_pStudioHeader->numbodyparts; i++, pbodypart++)
	{
		pmodel = (metastudiomodel_t *)STUDIOHDR(m_pStudioHeader, pbodypart->modelindex);

		for (j = 0; j < pbodypart->nummodels; j++, pmodel++)
		{
			GLuint	iVertexBuffer;
			GLuint	iIndexBuffer;

			pvert = (metastudiovertex_t *)STUDIOHDR(m_pStudioHeader, pmodel->vertindex);
			pmesh = (metastudiomesh_t *)STUDIOHDR(m_pStudioHeader, pmodel->meshindex);

			qglGenBuffers(1, &iVertexBuffer);
			qglBindBuffer(GL_ARRAY_BUFFER, iVertexBuffer);
			qglBufferData(GL_ARRAY_BUFFER, pmodel->numverts * sizeof(metastudiovertex_t), pvert, GL_STATIC_DRAW);
			qglBindBuffer(GL_ARRAY_BUFFER, 0);

			pmodel->vbo = iVertexBuffer;

			for (k = 0; k < pmodel->nummesh; k++, pmesh++)
			{
				ptris = (metastudiotriangle_t *)STUDIOHDR(m_pStudioHeader, pmesh->triindex);

				qglGenBuffers(1, &iIndexBuffer);
				qglBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iIndexBuffer);
				qglBufferData(GL_ELEMENT_ARRAY_BUFFER, pmesh->numtris * sizeof(metastudiotriangle_t), ptris, GL_STATIC_DRAW);
				qglBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

				pmesh->ebo = iIndexBuffer;
			}
		}
	}

	ptexture = (metastudiotexture_t *)STUDIOHDR(m_pStudioHeader, m_pStudioHeader->textureindex);

	for (i = 0; i < m_pStudioHeader->numtextures; i++, ptexture++)
	{
		if (!ptexture->path[0])
			continue;

		texinfo = gTexAPI.Load(ptexture->path);

		ptexture->width = texinfo->wide;
		ptexture->height = texinfo->tall;
		ptexture->glt = texinfo->id;
	}

	m_pStudioHeader->flags |= META_STUDIO_LOADED;
	gEngfuncs.Con_DPrintf("Load MetaStudio: %s\n", m_pStudioHeader->name);
}

int R_MetaStudioDrawModel(int flags)
{
	return g_MetaStudioRenderer.DrawModel(flags);
}

int R_MetaStudioDrawPlayer(int flags, entity_state_t *player)
{
	return g_MetaStudioRenderer.DrawPlayer(flags, player);
}

void R_MetaStudioInit(void)
{
	g_MetaStudioRenderer.Init();
}

// The simple drawing interface we'll pass back to the engine
r_studio_interface_t studio = 
{
	STUDIO_INTERFACE_VERSION,
	R_MetaStudioDrawModel,
	R_MetaStudioDrawPlayer,
};

extern "C"
{

DLLExport BOOL HUD_GetStudioModelInterface(int version, r_studio_interface_t **ppinterface, engine_studio_api_t *pstudio)
{
	if (version != STUDIO_INTERFACE_VERSION)
		return FALSE;

	// Point the engine to our callbacks
	*ppinterface = &studio;

	// Copy in engine helper functions
	memcpy(&IEngineStudio, pstudio, sizeof(IEngineStudio));

	// Initialize local variables, etc
	R_MetaStudioInit();

	return TRUE;
}

}