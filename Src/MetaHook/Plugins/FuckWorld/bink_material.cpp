
#include <metahook.h>
#include "qgl.h"
#include "surface.h"
#include "bink_material.h"

#pragma comment(lib, "binkw32.lib")

CBIKMaterial::CBIKMaterial()
{
	m_GLTexture = 0;
#ifndef PBO
	m_pBuffer = NULL;
#else
	m_GLPixelBufferObj = 0;
#endif
	m_pHBINK = NULL;
	m_nBIKWidth = 0;
	m_nBIKHeight = 0;
	m_nFrameRate = 0;
	m_nFrameCount = 0;
	m_bLoop = 0;
}

CBIKMaterial::~CBIKMaterial()
{
}

bool CBIKMaterial::Init(const char *pFileName, bool bAlphaChannel)
{
	m_pHBINK = BinkOpen(pFileName, BINKSNDTRACK);

	if (!m_pHBINK)
	{
		return false;
	}

	m_nBIKWidth = m_pHBINK->Width;
	m_nBIKHeight = m_pHBINK->Height;
	m_bBIKTransparent = bAlphaChannel;
	m_nFrameRate = (int)((float)m_pHBINK->FrameRate / (float)m_pHBINK->FrameRateDiv);
	m_nFrameCount = m_pHBINK->Frames;

#ifndef PBO
	int nBufferSize = m_nBIKWidth * m_nBIKHeight * (m_bBIKTransparent ? 4 : 3);
	m_pBuffer = new U8[nBufferSize];
	memset(m_pBuffer, 0, nBufferSize);
#endif

	CreateProceduralTexture();

	for (int i = 0; i < m_pHBINK->NumTracks; ++i)
	{
		BinkSetVolume(m_pHBINK, BinkGetTrackID(m_pHBINK, i), 0x8000);
	}

	BinkPause(m_pHBINK, TRUE);
	return true;
}

void CBIKMaterial::Shutdown(void)
{
	if (!m_pHBINK)
		return;

	DestroyProceduralTexture();

	if (m_pHBINK)
	{
		BinkClose(m_pHBINK);
		m_pHBINK = NULL;
	}

#ifndef PBO
	if (m_pBuffer)
	{
		delete[] m_pBuffer;
		m_pBuffer = NULL;
	}
#endif
}

bool CBIKMaterial::Update(void)
{
	if (!m_pHBINK)
		return true;

	BinkDoFrame(m_pHBINK);

	if (BinkWait(m_pHBINK))
		return true;

	while (BinkShouldSkip(m_pHBINK))
	{
		BinkNextFrame(m_pHBINK);
		BinkDoFrame(m_pHBINK);
	}

	if (m_pHBINK->FrameNum == m_pHBINK->Frames)
	{
		if (m_bLoop)
		{
			BinkGoto(m_pHBINK, 0, 0);
		}

		return false;
	}

#ifndef PBO
	BinkCopyToBuffer(m_pHBINK, m_pBuffer, m_nBIKWidth * (m_bBIKTransparent ? 4 : 3), m_nBIKHeight, 0, 0, BINKSURFACE24R);
	qglBindTexture(GL_TEXTURE_2D, m_GLTexture);
	qglTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_nBIKWidth, m_nBIKHeight, m_bBIKTransparent ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, m_pBuffer);
	qglBindTexture(GL_TEXTURE_2D, 0);
#else
	// bind the texture and PBO
	qglBindTexture(GL_TEXTURE_2D, m_GLTexture);
	qglBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, m_GLPixelBufferObj);

	// copy pixels from PBO to texture object
	// Use offset instead of ponter.
	qglTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_nBIKWidth, m_nBIKHeight, m_bBIKTransparent ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, NULL);

	// map the buffer object into client's memory
	void *ptr = qglMapBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, GL_WRITE_ONLY_ARB);
	if (ptr != NULL)
	{
		// update data directly on the mapped buffer
		BinkCopyToBuffer(m_pHBINK, ptr, m_nBIKWidth * (m_bBIKTransparent ? 4 : 3), m_nBIKHeight, 0, 0, m_bBIKTransparent ? BINKSURFACE32R : BINKSURFACE24R);
		qglUnmapBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB);	// release pointer to mapping buffer
	}
	qglBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, 0);
	qglBindTexture(GL_TEXTURE_2D, 0);
#endif

	BinkNextFrame(m_pHBINK);

	return true;
}

int CBIKMaterial::GetTexture(void)
{
	return m_GLTexture;
}

void CBIKMaterial::GetFrameSize(int *pWidth, int *pHeight)
{
	if (pWidth)
		*pWidth = m_nBIKWidth;

	if (pHeight)
		*pHeight = m_nBIKHeight;
}

int CBIKMaterial::GetFrameRate(void)
{
	return m_nFrameRate;
}

int CBIKMaterial::GetFrameCount(void)
{
	return m_nFrameCount;
}

void CBIKMaterial::SetFrame(float flFrame)
{
	if (!m_pHBINK)
		return;

	U32 iFrame = (U32)flFrame + 1;

	if (m_pHBINK->LastFrameNum != iFrame)
	{
		BinkGoto(m_pHBINK, iFrame, 0);
	}
}

void CBIKMaterial::SetLoop(bool state)
{
	m_bLoop = state;
}

void CBIKMaterial::SetPause(bool state)
{
	if (!m_pHBINK)
		return;

	BinkPause(m_pHBINK, state);
}

void CBIKMaterial::CreateProceduralTexture(void)
{
	//qglGenTextures(1, (GLuint *)&m_GLTexture);
	m_GLTexture = g_pSurface->CreateNewTextureID();
	qglBindTexture(GL_TEXTURE_2D, m_GLTexture);
	qglTexImage2D(GL_TEXTURE_2D, 0, m_bBIKTransparent ? GL_RGBA : GL_RGB, m_nBIKWidth, m_nBIKHeight, 0, m_bBIKTransparent ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, NULL);
	qglTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	qglTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	qglBindTexture(GL_TEXTURE_2D, 0);
#ifdef PBO
	qglGenBuffersARB(1, (GLuint *)&m_GLPixelBufferObj);
	qglBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, m_GLPixelBufferObj);
	qglBufferDataARB(GL_PIXEL_UNPACK_BUFFER_ARB, m_nBIKWidth * m_nBIKHeight * (m_bBIKTransparent ? 4 : 3), NULL, GL_STREAM_DRAW_ARB);
	qglBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, 0);
#endif
}

void CBIKMaterial::DestroyProceduralTexture(void)
{
	qglDeleteTextures(1, (GLuint *)&m_GLTexture);
#ifdef PBO
	qglDeleteBuffersARB(1, (GLuint *)&m_GLPixelBufferObj);
#endif
}