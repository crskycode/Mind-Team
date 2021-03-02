#include <metahook.h>
#include "studio_model.h"

// File System
#include "plugins.h"

// Texture Loader
#include "surface.h"
#include "qgl.h"

void StudioModel::UploadTexture(mstudiotexture_t *ptexture, byte *data, byte *pal)
{
	static byte buffer[1024 * 1024 * 4];

	for (int i = 0; i < ptexture->width * ptexture->height; i++)
	{
		byte *pixel = &buffer[i * 4];

		pixel[0] = pal[(data[i] * 3) + 0];
		pixel[1] = pal[(data[i] * 3) + 1];
		pixel[2] = pal[(data[i] * 3) + 2];
		pixel[3] = 255;
	}

	int id = g_pSurface->CreateNewTextureID();

	qglBindTexture(GL_TEXTURE_2D, id);
	qglTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ptexture->width, ptexture->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
	qglTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	qglTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	ptexture->index = id;
}

studiohdr_t *StudioModel::LoadModel(const char *modelname)
{
	FileHandle_t	fp;
	long			size;
	void			*buffer;

	if ((fp = g_pFileSystem->Open(modelname, "rb")) == FILESYSTEM_INVALID_HANDLE)
	{
		Sys_Error("unable to open %s\n", modelname);
	}

	g_pFileSystem->Seek(fp, 0, FILESYSTEM_SEEK_TAIL);
	size = g_pFileSystem->Tell(fp);
	g_pFileSystem->Seek(fp, 0, FILESYSTEM_SEEK_HEAD);
	buffer = malloc(size);
	g_pFileSystem->Read(buffer, size, fp);

	int					i;
	byte				*pin;
	studiohdr_t			*phdr;
	mstudiotexture_t	*ptexture;

	pin = (byte *)buffer;
	phdr = (studiohdr_t *)pin;

	ptexture = (mstudiotexture_t *)(pin + phdr->textureindex);
	if (phdr->textureindex != 0)
	{
		for (i = 0; i < phdr->numtextures; i++)
		{
			UploadTexture(&ptexture[i], pin + ptexture[i].index, pin + ptexture[i].width * ptexture[i].height + ptexture[i].index);
		}
	}

	g_pFileSystem->Close(fp);

	return (studiohdr_t *)buffer;
}

studioseqhdr_t *StudioModel::LoadDemandSequences(const char *modelname)
{
	FileHandle_t	fp;
	long			size;
	void			*buffer;

	if ((fp = g_pFileSystem->Open(modelname, "rb")) == FILESYSTEM_INVALID_HANDLE)
	{
		Sys_Error("unable to open %s\n", modelname);
	}

	g_pFileSystem->Seek(fp, 0, FILESYSTEM_SEEK_TAIL);
	size = g_pFileSystem->Tell(fp);
	g_pFileSystem->Seek(fp, 0, FILESYSTEM_SEEK_HEAD);
	buffer = malloc(size);
	g_pFileSystem->Read(buffer, size, fp);

	g_pFileSystem->Close(fp);

	return (studioseqhdr_t *)buffer;
}

StudioModel::StudioModel()
{
	memset(this, 0, sizeof(*this));

	m_fovy = 45.0;
	m_aspect = 1.2;
	m_zNear = 0.01;
	m_zFar = 1000.0;

	m_scale = 1.0;

	m_lambert = 1.5;
}

StudioModel::~StudioModel()
{
	Free();
}

void StudioModel::Init(int width, int height)
{
	if (m_initailize)
		return;

	m_width = width;
	m_height = height;

	m_iColorBuffer = g_pSurface->CreateNewTextureID();

	qglGenFramebuffers(1, (GLuint *)&m_iFrameBuffer);
	qglGenRenderbuffers(1, (GLuint *)&m_iRenderBuffer);

	qglGetIntegerv(GL_FRAMEBUFFER_BINDING, &m_iPrevFrameBuffer);
	qglBindFramebuffer(GL_FRAMEBUFFER, m_iFrameBuffer);

	qglBindTexture(GL_TEXTURE_2D, m_iColorBuffer);
	qglTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	qglTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	qglTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	qglBindRenderbuffer(GL_RENDERBUFFER, m_iRenderBuffer);
	qglRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_width, m_height);

	qglFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_iColorBuffer, 0);
	qglFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_iRenderBuffer);
	qglFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_iRenderBuffer);

	qglBindFramebuffer(GL_FRAMEBUFFER, m_iPrevFrameBuffer);

	if (qglCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		Sys_Error("GL_FRAMEBUFFER_UNSUPPORTED");
	}

	m_initailize = true;
}

void StudioModel::Load(const char *modelname)
{
	if (!m_initailize)
		return;

	Free();

	m_pstudiohdr = LoadModel(modelname);

	if (m_pstudiohdr->numtextures == 0)
	{
		char texturename[256];

		strcpy(texturename, modelname);
		strcpy(&texturename[strlen(texturename) - 4], "T.mdl");

		m_ptexturehdr = LoadModel(texturename);
	}
	else
	{
		m_ptexturehdr = m_pstudiohdr;
	}

	if (m_pstudiohdr->numseqgroups > 1)
	{
		for (int i = 1; i < m_pstudiohdr->numseqgroups; i++)
		{
			char seqgroupname[256];

			strcpy(seqgroupname, modelname);
			sprintf(&seqgroupname[strlen(seqgroupname) - 4], "%02d.mdl", i);

			m_panimhdr[i] = LoadDemandSequences(seqgroupname);
		}
	}
}

void StudioModel::Free(void)
{
	if (m_pstudiohdr)
	{
		free(m_pstudiohdr);
	}

	if (m_ptexturehdr != m_pstudiohdr)
	{
		free(m_ptexturehdr);
	}

	for (int i = 0; i < 32; i++)
	{
		if (m_panimhdr[i])
		{
			free(m_panimhdr[i]);
		}
	}


#define ZeroVar(x) memset(&x, 0, sizeof(x));

	ZeroVar(m_origin);
	ZeroVar(m_angles);
	//ZeroMEM(m_scale);
	ZeroVar(m_sequence);
	ZeroVar(m_frame);
	ZeroVar(m_bodynum);
	ZeroVar(m_skinnum);
	ZeroVar(m_controller);
	ZeroVar(m_blending);
	ZeroVar(m_mouth);
	ZeroVar(m_pstudiohdr);
	ZeroVar(m_pmodel);
	ZeroVar(m_ptexturehdr);
	ZeroVar(m_panimhdr);
	ZeroVar(m_adj);
	ZeroVar(m_vright);
	ZeroVar(m_xformverts);
	ZeroVar(m_lightvalues);
	ZeroVar(m_pxformverts);
	ZeroVar(m_pvlightvalues);
	ZeroVar(m_lightvec);
	ZeroVar(m_blightvec);
	ZeroVar(m_ambientlight);
	ZeroVar(m_shadelight);
	ZeroVar(m_lightcolor);
	ZeroVar(m_smodels_total);
	ZeroVar(m_bonetransform);
	ZeroVar(m_chrome);
	ZeroVar(m_chromeage);
	ZeroVar(m_chromeup);
	ZeroVar(m_chromeright);
}

int StudioModel::GetSequence(void)
{
	return m_sequence;
}

int StudioModel::SetSequence(int iSequence)
{
	if (!m_pstudiohdr)
		return 0;

	if (iSequence > m_pstudiohdr->numseq)
		iSequence = 0;
	if (iSequence < 0)
		iSequence = m_pstudiohdr->numseq - 1;

	m_sequence = iSequence;
	m_frame = 0;

	return m_sequence;
}

void StudioModel::ExtractBbox(float *mins, float *maxs)
{
	mstudioseqdesc_t *pseqdesc;

	if (!m_pstudiohdr)
		return;

	pseqdesc = (mstudioseqdesc_t *)((byte *)m_pstudiohdr + m_pstudiohdr->seqindex);

	mins[0] = pseqdesc[m_sequence].bbmin[0];
	mins[1] = pseqdesc[m_sequence].bbmin[1];
	mins[2] = pseqdesc[m_sequence].bbmin[2];

	maxs[0] = pseqdesc[m_sequence].bbmax[0];
	maxs[1] = pseqdesc[m_sequence].bbmax[1];
	maxs[2] = pseqdesc[m_sequence].bbmax[2];
}

void StudioModel::GetSequenceInfo(float *pflFrameRate, float *pflGroundSpeed)
{
	mstudioseqdesc_t *pseqdesc;

	if (!m_pstudiohdr)
		return;

	pseqdesc = (mstudioseqdesc_t *)((byte *)m_pstudiohdr + m_pstudiohdr->seqindex) + (int)m_sequence;

	if (pseqdesc->numframes > 1)
	{
		*pflFrameRate = 256 * pseqdesc->fps / (pseqdesc->numframes - 1);
		*pflGroundSpeed = sqrt(pseqdesc->linearmovement[0]*pseqdesc->linearmovement[0] + pseqdesc->linearmovement[1]*pseqdesc->linearmovement[1] + pseqdesc->linearmovement[2]*pseqdesc->linearmovement[2]);
		*pflGroundSpeed = *pflGroundSpeed * pseqdesc->fps / (pseqdesc->numframes - 1);
	}
	else
	{
		*pflFrameRate = 256.0;
		*pflGroundSpeed = 0.0;
	}
}

float StudioModel::SetController(int iController, float flValue)
{
	int		i;

	if (!m_pstudiohdr)
		return flValue;

	mstudiobonecontroller_t	*pbonecontroller = (mstudiobonecontroller_t *)((byte *)m_pstudiohdr + m_pstudiohdr->bonecontrollerindex);

	for (i = 0; i < m_pstudiohdr->numbonecontrollers; i++, pbonecontroller++)
	{
		if (pbonecontroller->index == iController)
			break;
	}
	if (i >= m_pstudiohdr->numbonecontrollers)
		return flValue;

	if (pbonecontroller->type & (STUDIO_XR | STUDIO_YR | STUDIO_ZR))
	{
		if (pbonecontroller->end < pbonecontroller->start)
			flValue = -flValue;

		if (pbonecontroller->start + 359.0 >= pbonecontroller->end)
		{
			if (flValue > ((pbonecontroller->start + pbonecontroller->end) / 2.0) + 180)
				flValue = flValue - 360;
			if (flValue < ((pbonecontroller->start + pbonecontroller->end) / 2.0) - 180)
				flValue = flValue + 360;
		}
		else
		{
			if (flValue > 360)
				flValue = flValue - (int)(flValue / 360.0) * 360.0;
			else if (flValue < 0)
				flValue = flValue + (int)((flValue / -360.0) + 1) * 360.0;
		}
	}

	int setting = 255 * (flValue - pbonecontroller->start) / (pbonecontroller->end - pbonecontroller->start);

	if (setting < 0) setting = 0;
	if (setting > 255) setting = 255;
	m_controller[iController] = setting;

	return setting * (1.0 / 255.0) * (pbonecontroller->end - pbonecontroller->start) + pbonecontroller->start;
}

float StudioModel::SetMouth(float flValue)
{
	if (!m_pstudiohdr)
		return flValue;

	mstudiobonecontroller_t	*pbonecontroller = (mstudiobonecontroller_t *)((byte *)m_pstudiohdr + m_pstudiohdr->bonecontrollerindex);

	for (int i = 0; i < m_pstudiohdr->numbonecontrollers; i++, pbonecontroller++)
	{
		if (pbonecontroller->index == 4)
			break;
	}

	if (pbonecontroller->type & (STUDIO_XR | STUDIO_YR | STUDIO_ZR))
	{
		if (pbonecontroller->end < pbonecontroller->start)
			flValue = -flValue;

		if (pbonecontroller->start + 359.0 >= pbonecontroller->end)
		{
			if (flValue > ((pbonecontroller->start + pbonecontroller->end) / 2.0) + 180)
				flValue = flValue - 360;
			if (flValue < ((pbonecontroller->start + pbonecontroller->end) / 2.0) - 180)
				flValue = flValue + 360;
		}
		else
		{
			if (flValue > 360)
				flValue = flValue - (int)(flValue / 360.0) * 360.0;
			else if (flValue < 0)
				flValue = flValue + (int)((flValue / -360.0) + 1) * 360.0;
		}
	}

	int setting = 64 * (flValue - pbonecontroller->start) / (pbonecontroller->end - pbonecontroller->start);

	if (setting < 0) setting = 0;
	if (setting > 64) setting = 64;
	m_mouth = setting;

	return setting * (1.0 / 64.0) * (pbonecontroller->end - pbonecontroller->start) + pbonecontroller->start;
}

float StudioModel::SetBlending(int iBlender, float flValue)
{
	mstudioseqdesc_t *pseqdesc;

	if (!m_pstudiohdr)
		return flValue;

	pseqdesc = (mstudioseqdesc_t *)((byte *)m_pstudiohdr + m_pstudiohdr->seqindex) + (int)m_sequence;

	if (pseqdesc->blendtype[iBlender] == 0)
		return flValue;

	if (pseqdesc->blendtype[iBlender] & (STUDIO_XR | STUDIO_YR | STUDIO_ZR))
	{
		if (pseqdesc->blendend[iBlender] < pseqdesc->blendstart[iBlender])
			flValue = -flValue;

		if (pseqdesc->blendstart[iBlender] + 359.0 >= pseqdesc->blendend[iBlender])
		{
			if (flValue > ((pseqdesc->blendstart[iBlender] + pseqdesc->blendend[iBlender]) / 2.0) + 180)
				flValue = flValue - 360;
			if (flValue < ((pseqdesc->blendstart[iBlender] + pseqdesc->blendend[iBlender]) / 2.0) - 180)
				flValue = flValue + 360;
		}
	}

	int setting = 255 * (flValue - pseqdesc->blendstart[iBlender]) / (pseqdesc->blendend[iBlender] - pseqdesc->blendstart[iBlender]);

	if (setting < 0) setting = 0;
	if (setting > 255) setting = 255;

	m_blending[iBlender] = setting;

	return setting * (1.0 / 255.0) * (pseqdesc->blendend[iBlender] - pseqdesc->blendstart[iBlender]) + pseqdesc->blendstart[iBlender];
}

int StudioModel::SetBodygroup(int iGroup, int iValue)
{
	if (!m_pstudiohdr)
		return iValue;

	if (iGroup > m_pstudiohdr->numbodyparts)
		return -1;

	mstudiobodyparts_t *pbodypart = (mstudiobodyparts_t *)((byte *)m_pstudiohdr + m_pstudiohdr->bodypartindex) + iGroup;

	int iCurrent = (m_bodynum / pbodypart->base) % pbodypart->nummodels;

	if (iValue >= pbodypart->nummodels)
		return iCurrent;

	m_bodynum = (m_bodynum - (iCurrent * pbodypart->base) + (iValue * pbodypart->base));

	return iValue;
}

int StudioModel::SetSkin(int iValue)
{
	if (!m_pstudiohdr)
		return iValue;

	if (iValue < m_pstudiohdr->numskinfamilies)
	{
		return m_skinnum;
	}

	m_skinnum = iValue;

	return iValue;
}

void StudioModel::SetPerspective(float fovy, float aspect, float zNear, float zFar)
{
	m_fovy = fovy;
	m_aspect = aspect;
	m_zNear = zNear;
	m_zFar = zFar;
}

void StudioModel::SetOrigin(float x, float y, float z)
{
	m_origin[0] = x;
	m_origin[1] = y;
	m_origin[2] = z;
}

void StudioModel::SetAngles(float x, float y, float z)
{
	m_angles[0] = x;
	m_angles[1] = y;
	m_angles[2] = z;
}

void StudioModel::SetScale(float s)
{
	m_scale = s;
}

int StudioModel::GetColorBuffer(void)
{
	return m_iColorBuffer;
}