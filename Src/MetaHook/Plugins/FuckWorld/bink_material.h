#ifndef BINK_MATERIAL_H
#define BINK_MATERIAL_H

#include "bink/bink.h"

#define PBO

class CBIKMaterial
{
public:
	CBIKMaterial();
	~CBIKMaterial();

	bool Init(const char *pFileName, bool bAlphaChannel);
	void Shutdown(void);
	bool Update(void);
	int GetTexture(void);
	void GetFrameSize(int *pWidth, int *pHeight);
	int GetFrameRate(void);
	int GetFrameCount(void);
	void SetFrame(float flFrame);
	void SetLoop(bool state);
	void SetPause(bool state);

private:
	void CreateProceduralTexture(void);
	void DestroyProceduralTexture(void);

	HBINK m_pHBINK;

	int m_GLTexture;

#ifndef PBO
	U8 *m_pBuffer;
#else
	int m_GLPixelBufferObj;
#endif

	int m_nBIKWidth;
	int m_nBIKHeight;
	int m_bBIKTransparent;

	int m_nFrameRate;
	int m_nFrameCount;

	bool m_bLoop;
};

#endif