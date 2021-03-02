#include <metahook.h>
#include "studio_model.h"
#include "qgl.h"

void StudioModel::CalcBoneAdj(void)
{
	int						i, j;
	float					value;
	mstudiobonecontroller_t	*pbonecontroller;

	pbonecontroller = (mstudiobonecontroller_t *)((byte *)m_pstudiohdr + m_pstudiohdr->bonecontrollerindex);

	for (j = 0; j < m_pstudiohdr->numbonecontrollers; j++)
	{
		i = pbonecontroller[j].index;
		if (i <= 3)
		{
			if (pbonecontroller[j].type & STUDIO_RLOOP)
			{
				value = m_controller[i] * (360.0 / 256.0) + pbonecontroller[j].start;
			}
			else
			{
				value = m_controller[i] / 255.0;
				if (value < 0) value = 0;
				if (value > 1.0) value = 1.0;
				value = (1.0 - value) * pbonecontroller[j].start + value * pbonecontroller[j].end;
			}
		}
		else
		{
			value = m_mouth / 64.0;
			if (value > 1.0) value = 1.0;
			value = (1.0 - value) * pbonecontroller[j].start + value * pbonecontroller[j].end;
		}
		switch (pbonecontroller[j].type & STUDIO_TYPES)
		{
		case STUDIO_XR:
		case STUDIO_YR:
		case STUDIO_ZR:
			m_adj[j] = value * (M_PI / 180.0);
			break;
		case STUDIO_X:
		case STUDIO_Y:
		case STUDIO_Z:
			m_adj[j] = value;
			break;
		}
	}
}

void StudioModel::CalcBoneQuaternion(int frame, float s, mstudiobone_t *pbone, mstudioanim_t *panim, float *q)
{
	int					j, k;
	vec4_t				q1, q2;
	vec3_t				angle1, angle2;
	mstudioanimvalue_t	*panimvalue;

	for (j = 0; j < 3; j++)
	{
		if (panim->offset[j + 3] == 0)
		{
			angle2[j] = angle1[j] = pbone->value[j + 3];
		}
		else
		{
			panimvalue = (mstudioanimvalue_t *)((byte *)panim + panim->offset[j + 3]);
			k = frame;
			while (panimvalue->num.total <= k)
			{
				k -= panimvalue->num.total;
				panimvalue += panimvalue->num.valid + 1;
			}
			if (panimvalue->num.valid > k)
			{
				angle1[j] = panimvalue[k + 1].value;

				if (panimvalue->num.valid > k + 1)
				{
					angle2[j] = panimvalue[k + 2].value;
				}
				else
				{
					if (panimvalue->num.total > k + 1)
						angle2[j] = angle1[j];
					else
						angle2[j] = panimvalue[panimvalue->num.valid + 2].value;
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
			angle1[j] = pbone->value[j + 3] + angle1[j] * pbone->scale[j + 3];
			angle2[j] = pbone->value[j + 3] + angle2[j] * pbone->scale[j + 3];
		}

		if (pbone->bonecontroller[j + 3] != -1)
		{
			angle1[j] += m_adj[pbone->bonecontroller[j + 3]];
			angle2[j] += m_adj[pbone->bonecontroller[j + 3]];
		}
	}

	if (!VectorCompare(angle1, angle2))
	{
		AngleQuaternion(angle1, q1);
		AngleQuaternion(angle2, q2);
		QuaternionSlerp(q1, q2, s, q);
	}
	else
	{
		AngleQuaternion(angle1, q);
	}
}

void StudioModel::CalcBonePosition(int frame, float s, mstudiobone_t *pbone, mstudioanim_t *panim, float *pos)
{
	int					j, k;
	mstudioanimvalue_t	*panimvalue;

	for (j = 0; j < 3; j++)
	{
		pos[j] = pbone->value[j];
		if (panim->offset[j] != 0)
		{
			panimvalue = (mstudioanimvalue_t *)((byte *)panim + panim->offset[j]);

			k = frame;
			while (panimvalue->num.total <= k)
			{
				k -= panimvalue->num.total;
				panimvalue += panimvalue->num.valid + 1;
			}
			if (panimvalue->num.valid > k)
			{
				if (panimvalue->num.valid > k + 1)
				{
					pos[j] += (panimvalue[k + 1].value * (1.0 - s) + s * panimvalue[k + 2].value) * pbone->scale[j];
				}
				else
				{
					pos[j] += panimvalue[k + 1].value * pbone->scale[j];
				}
			}
			else
			{
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

void StudioModel::CalcRotations(vec3_t *pos, vec4_t *q, mstudioseqdesc_t *pseqdesc, mstudioanim_t *panim, float f)
{
	int				i;
	int				frame;
	mstudiobone_t	*pbone;
	float			s;

	frame = (int)f;
	s = (f - frame);

	CalcBoneAdj();

	pbone = (mstudiobone_t *)((byte *)m_pstudiohdr + m_pstudiohdr->boneindex);
	for (i = 0; i < m_pstudiohdr->numbones; i++, pbone++, panim++)
	{
		CalcBoneQuaternion(frame, s, pbone, panim, q[i]);
		CalcBonePosition(frame, s, pbone, panim, pos[i]);
	}

	if (pseqdesc->motiontype & STUDIO_X)
		pos[pseqdesc->motionbone][0] = 0.0;
	if (pseqdesc->motiontype & STUDIO_Y)
		pos[pseqdesc->motionbone][1] = 0.0;
	if (pseqdesc->motiontype & STUDIO_Z)
		pos[pseqdesc->motionbone][2] = 0.0;
}

mstudioanim_t * StudioModel::GetAnim(mstudioseqdesc_t *pseqdesc)
{
	mstudioseqgroup_t	*pseqgroup;
	pseqgroup = (mstudioseqgroup_t *)((byte *)m_pstudiohdr + m_pstudiohdr->seqgroupindex) + pseqdesc->seqgroup;

	if (pseqdesc->seqgroup == 0)
	{
		return (mstudioanim_t *)((byte *)m_pstudiohdr + pseqgroup->data + pseqdesc->animindex);
	}

	return (mstudioanim_t *)((byte *)m_panimhdr[pseqdesc->seqgroup] + pseqdesc->animindex);
}

void StudioModel::SlerpBones(vec4_t q1[], vec3_t pos1[], vec4_t q2[], vec3_t pos2[], float s)
{
	int			i;
	vec4_t		q3;
	float		s1;

	if (s < 0) s = 0;
	else if (s > 1.0) s = 1.0;

	s1 = 1.0 - s;

	for (i = 0; i < m_pstudiohdr->numbones; i++)
	{
		QuaternionSlerp(q1[i], q2[i], s, q3);
		q1[i][0] = q3[0];
		q1[i][1] = q3[1];
		q1[i][2] = q3[2];
		q1[i][3] = q3[3];
		pos1[i][0] = pos1[i][0] * s1 + pos2[i][0] * s;
		pos1[i][1] = pos1[i][1] * s1 + pos2[i][1] * s;
		pos1[i][2] = pos1[i][2] * s1 + pos2[i][2] * s;
	}
}

void StudioModel::AdvanceFrame(float dt)
{
	mstudioseqdesc_t	*pseqdesc;

	if (!m_pstudiohdr)
		return;

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
		m_frame -= (int)(m_frame / (pseqdesc->numframes - 1)) * (pseqdesc->numframes - 1);
	}
}

void StudioModel::SetUpBones(void)
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

	if (m_sequence >=  m_pstudiohdr->numseq)
		m_sequence = 0;

	pseqdesc = (mstudioseqdesc_t *)((byte *)m_pstudiohdr + m_pstudiohdr->seqindex) + m_sequence;

	panim = GetAnim(pseqdesc);
	CalcRotations(pos, q, pseqdesc, panim, m_frame);

	if (pseqdesc->numblends > 1)
	{
		float	s;

		panim += m_pstudiohdr->numbones;
		CalcRotations(pos2, q2, pseqdesc, panim, m_frame);
		s = m_blending[0] / 255.0;

		SlerpBones(q, pos, q2, pos2, s);

		if (pseqdesc->numblends == 4)
		{
			panim += m_pstudiohdr->numbones;
			CalcRotations(pos3, q3, pseqdesc, panim, m_frame);

			panim += m_pstudiohdr->numbones;
			CalcRotations(pos4, q4, pseqdesc, panim, m_frame);

			s = m_blending[0] / 255.0;
			SlerpBones(q3, pos3, q4, pos4, s);

			s = m_blending[1] / 255.0;
			SlerpBones(q, pos, q3, pos3, s);
		}
	}

	pbones = (mstudiobone_t *)((byte *)m_pstudiohdr + m_pstudiohdr->boneindex);

	for (i = 0; i < m_pstudiohdr->numbones; i++)
	{
		QuaternionMatrix(q[i], bonematrix);

		bonematrix[0][3] = pos[i][0];
		bonematrix[1][3] = pos[i][1];
		bonematrix[2][3] = pos[i][2];

		if (pbones[i].parent == -1)
		{
			memcpy(m_bonetransform[i], bonematrix, sizeof(float) * 12);
		}
		else
		{
			ConcatTransforms(m_bonetransform[pbones[i].parent], bonematrix, m_bonetransform[i]);
		}
	}
}

void StudioModel::Lighting(float *lv, int bone, int flags, vec3_t normal)
{
	float 	illum;
	float	lightcos;

	illum = m_ambientlight;

	if (flags & STUDIO_NF_FLATSHADE)
	{
		illum += m_shadelight * 0.8;
	}
	else
	{
		float	r;
		lightcos = DotProduct(normal, m_blightvec[bone]);

		if (lightcos > 1.0)
			lightcos = 1;

		illum += m_shadelight;

		r = m_lambert;
		if (r <= 1.0) r = 1.0;

		lightcos = (lightcos + (r - 1.0)) / r;
		if (lightcos > 0.0)
		{
			illum -= m_shadelight * lightcos;
		}
		if (illum <= 0)
			illum = 0;
	}

	if (illum > 255)
		illum = 255;
	*lv = illum / 255.0;
}

void StudioModel::Chrome(int *pchrome, int bone, vec3_t normal)
{
	float	n;

	if (m_chromeage[bone] != m_smodels_total)
	{
		vec3_t	chromeupvec;
		vec3_t	chromerightvec;
		vec3_t	tmp;
		VectorScale(m_origin, -1, tmp);
		tmp[0] += m_bonetransform[bone][0][3];
		tmp[1] += m_bonetransform[bone][1][3];
		tmp[2] += m_bonetransform[bone][2][3];
		VectorNormalize(tmp);
		CrossProduct(tmp, m_vright, chromeupvec);
		VectorNormalize(chromeupvec);
		CrossProduct(tmp, chromeupvec, chromerightvec);
		VectorNormalize(chromerightvec);

		VectorIRotate(chromeupvec, m_bonetransform[bone], m_chromeup[bone]);
		VectorIRotate(chromerightvec, m_bonetransform[bone], m_chromeright[bone]);

		m_chromeage[bone] = m_smodels_total;
	}

	n = DotProduct(normal, m_chromeright[bone]);
	pchrome[0] = (n + 1.0) * 32;

	n = DotProduct(normal, m_chromeup[bone]);
	pchrome[1] = (n + 1.0) * 32;
}

void StudioModel::SetupLighting ()
{
	int		i;
	m_ambientlight = 32;
	m_shadelight = 192;

	m_lightvec[0] = 0;
	m_lightvec[1] = 0;
	m_lightvec[2] = -1.0;

	m_lightcolor[0] = 1.0;
	m_lightcolor[1] = 1.0;
	m_lightcolor[2] = 1.0;

	for (i = 0; i < m_pstudiohdr->numbones; i++)
	{
		VectorIRotate(m_lightvec, m_bonetransform[i], m_blightvec[i]);
	}
}

void StudioModel::SetupModel(int bodypart)
{
	int		index;

	if (bodypart > m_pstudiohdr->numbodyparts)
	{
		bodypart = 0;
	}

	mstudiobodyparts_t *pbodypart = (mstudiobodyparts_t *)((byte *)m_pstudiohdr + m_pstudiohdr->bodypartindex) + bodypart;

	index = m_bodynum / pbodypart->base;
	index = index % pbodypart->nummodels;

	m_pmodel = (mstudiomodel_t *)((byte *)m_pstudiohdr + pbodypart->modelindex) + index;
}

static void qgluPerspective(GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar)
{
	GLdouble xmin, xmax, ymin, ymax;

	ymax = zNear * tan(fovy * M_PI / 360.0);
	ymin = -ymax;

	xmin = ymin * aspect;
	xmax = ymax * aspect;

	qglFrustum(xmin, xmax, ymin, ymax, zNear, zFar);
}

void StudioModel::Draw(void)
{
	int		i;

	if (!m_pstudiohdr)
		return;

	m_smodels_total++;

	m_pxformverts = &m_xformverts[0];
	m_pvlightvalues = &m_lightvalues[0];

	if (m_pstudiohdr->numbodyparts == 0)
		return;

	qglPushAttrib(GL_ALL_ATTRIB_BITS);

	qglViewport(0, 0, m_width, m_height);

	qglMatrixMode(GL_PROJECTION);
	qglPushMatrix();
	qglLoadIdentity();
	qgluPerspective(m_fovy, m_aspect, m_zNear, m_zFar);

	qglMatrixMode(GL_MODELVIEW);
	qglPushMatrix();
	qglLoadIdentity();

	qglGetIntegerv(GL_FRAMEBUFFER_BINDING, &m_iPrevFrameBuffer);
	qglBindFramebuffer(GL_FRAMEBUFFER, m_iFrameBuffer);

	qglClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	qglClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	qglEnable(GL_DEPTH_TEST);
	qglDepthFunc(GL_LEQUAL);
	qglDepthRange(0.0, 10.0);
	qglDepthMask(GL_TRUE);

	qglPushMatrix();

	qglTranslatef(m_origin[0], m_origin[1], m_origin[2]);

	qglRotatef(m_angles[0], 1, 0, 0);
	qglRotatef(m_angles[1], 0, 1, 0);
	qglRotatef(m_angles[2], 0, 0, 1);

	qglScalef(m_scale, m_scale, m_scale);

	qglTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	SetUpBones();

	SetupLighting();

	for (i = 0; i < m_pstudiohdr->numbodyparts; i++)
	{
		SetupModel(i);
		DrawPoints();
	}

	qglTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	qglPopMatrix();

	qglDisable(GL_DEPTH_TEST);
	qglBindFramebuffer(GL_FRAMEBUFFER, m_iPrevFrameBuffer);

	qglMatrixMode(GL_PROJECTION);
	qglPopMatrix();

	qglMatrixMode(GL_MODELVIEW);
	qglPopMatrix();

	qglPopAttrib();
}

void StudioModel::DrawPoints(void)
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

	for (i = 0; i < m_pmodel->numverts; i++)
	{
		VectorTransform(pstudioverts[i], m_bonetransform[pvertbone[i]], m_pxformverts[i]);
	}

	lv = (float *)m_pvlightvalues;
	for (j = 0; j < m_pmodel->nummesh; j++)
	{
		int flags;
		flags = ptexture[pskinref[pmesh[j].skinref]].flags;
		for (i = 0; i < pmesh[j].numnorms; i++, lv += 3, pstudionorms++, pnormbone++)
		{
			Lighting(&lv_tmp, *pnormbone, flags, (float *)pstudionorms);

			if (flags & STUDIO_NF_CHROME)
				Chrome(m_chrome[(float (*)[3])lv - m_pvlightvalues], *pnormbone, (float *)pstudionorms);

			lv[0] = lv_tmp * m_lightcolor[0];
			lv[1] = lv_tmp * m_lightcolor[1];
			lv[2] = lv_tmp * m_lightcolor[2];
		}
	}

	qglCullFace(GL_FRONT);

	for (j = 0; j < m_pmodel->nummesh; j++)
	{
		float	s, t;
		short	*ptricmds;

		pmesh = (mstudiomesh_t *)((byte *)m_pstudiohdr + m_pmodel->meshindex) + j;
		ptricmds = (short *)((byte *)m_pstudiohdr + pmesh->triindex);

		s = 1.0 / (float)ptexture[pskinref[pmesh->skinref]].width;
		t = 1.0 / (float)ptexture[pskinref[pmesh->skinref]].height;

		qglBindTexture(GL_TEXTURE_2D, ptexture[pskinref[pmesh->skinref]].index);

		if (ptexture[pskinref[pmesh->skinref]].flags & STUDIO_NF_CHROME)
		{
			while (i = *(ptricmds++))
			{
				if (i < 0)
				{
					qglBegin(GL_TRIANGLE_FAN);
					i = -i;
				}
				else
				{
					qglBegin(GL_TRIANGLE_STRIP);
				}

				for(; i > 0; i--, ptricmds += 4)
				{
					qglTexCoord2f(m_chrome[ptricmds[1]][0] * s, m_chrome[ptricmds[1]][1] * t);

					lv = m_pvlightvalues[ptricmds[1]];
					qglColor4f(lv[0], lv[1], lv[2], 1.0);

					av = m_pxformverts[ptricmds[0]];
					qglVertex3f(av[0], av[1], av[2]);
				}
				qglEnd();
			}
		}
		else
		{
			while (i = *(ptricmds++))
			{
				if (i < 0)
				{
					qglBegin(GL_TRIANGLE_FAN);
					i = -i;
				}
				else
				{
					qglBegin(GL_TRIANGLE_STRIP);
				}

				for(; i > 0; i--, ptricmds += 4)
				{
					qglTexCoord2f(ptricmds[2] * s, ptricmds[3] * t);

					lv = m_pvlightvalues[ptricmds[1]];
					qglColor4f(lv[0], lv[1], lv[2], 1.0);

					av = m_pxformverts[ptricmds[0]];
					qglVertex3f(av[0], av[1], av[2]);
				}
				qglEnd();
			}
		}
	}
}