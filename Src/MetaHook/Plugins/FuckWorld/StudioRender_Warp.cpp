#include <metahook.h>
#include "studio_model.h"
#include "IStudioRender.h"

HSTUDIO StudioModel_Create(void)
{
	return new StudioModel;
}

void StudioModel_Release(HSTUDIO handle)
{
	delete static_cast<StudioModel *>(handle);
}

void StudioModel_Init(HSTUDIO handle, int width, int height)
{
	static_cast<StudioModel *>(handle)->Init(width, height);
}

void StudioModel_Load(HSTUDIO handle, const char *modelname)
{
	static_cast<StudioModel *>(handle)->Load(modelname);
}

void StudioModel_Free(HSTUDIO handle)
{
	static_cast<StudioModel *>(handle)->Free();
}

void StudioModel_Draw(HSTUDIO handle)
{
	static_cast<StudioModel *>(handle)->Draw();
}

void StudioModel_AdvanceFrame(HSTUDIO handle, float dt)
{
	static_cast<StudioModel *>(handle)->AdvanceFrame(dt);
}

void StudioModel_ExtractBbox(HSTUDIO handle, float *mins, float *maxs)
{
	static_cast<StudioModel *>(handle)->ExtractBbox(mins, maxs);
}

int StudioModel_SetSequence(HSTUDIO handle, int iSequence)
{
	return static_cast<StudioModel *>(handle)->SetSequence(iSequence);
}

int StudioModel_GetSequence(HSTUDIO handle)
{
	return static_cast<StudioModel *>(handle)->GetSequence();
}

void StudioModel_GetSequenceInfo(HSTUDIO handle, float *pflFrameRate, float *pflGroundSpeed)
{
	static_cast<StudioModel *>(handle)->GetSequenceInfo(pflFrameRate, pflGroundSpeed);
}

float StudioModel_SetController(HSTUDIO handle, int iController, float flValue)
{
	return static_cast<StudioModel *>(handle)->SetController(iController, flValue);
}

float StudioModel_SetMouth(HSTUDIO handle, float flValue)
{
	return static_cast<StudioModel *>(handle)->SetMouth(flValue);
}

float StudioModel_SetBlending(HSTUDIO handle, int iBlender, float flValue)
{
	return static_cast<StudioModel *>(handle)->SetBlending(iBlender, flValue);
}

int StudioModel_SetBodygroup(HSTUDIO handle, int iGroup, int iValue)
{
	return static_cast<StudioModel *>(handle)->SetBodygroup(iGroup, iValue);
}

int StudioModel_SetSkin(HSTUDIO handle, int iValue)
{
	return static_cast<StudioModel *>(handle)->SetSkin(iValue);
}

void StudioModel_SetPerspective(HSTUDIO handle, float fovy, float aspect, float zNear, float zFar)
{
	static_cast<StudioModel *>(handle)->SetPerspective(fovy, aspect, zNear, zFar);
}

void StudioModel_SetOrigin(HSTUDIO handle, float x, float y, float z)
{
	static_cast<StudioModel *>(handle)->SetOrigin(x, y, z);
}

void StudioModel_SetAngles(HSTUDIO handle, float x, float y, float z)
{
	static_cast<StudioModel *>(handle)->SetAngles(x, y, z);
}

void StudioModel_SetScale(HSTUDIO handle, float s)
{
	static_cast<StudioModel *>(handle)->SetScale(s);
}

int StudioModel_GetColorBuffer(HSTUDIO handle)
{
	return static_cast<StudioModel *>(handle)->GetColorBuffer();
}

studio_render_t gStudioRender = 
{
	StudioModel_Create,
	StudioModel_Release,
	StudioModel_Init,
	StudioModel_Load,
	StudioModel_Free,
	StudioModel_Draw,
	StudioModel_AdvanceFrame,
	StudioModel_ExtractBbox,
	StudioModel_SetSequence,
	StudioModel_GetSequence,
	StudioModel_GetSequenceInfo,
	StudioModel_SetController,
	StudioModel_SetMouth,
	StudioModel_SetBlending,
	StudioModel_SetBodygroup,
	StudioModel_SetSkin,
	StudioModel_SetPerspective,
	StudioModel_SetOrigin,
	StudioModel_SetAngles,
	StudioModel_SetScale,
	StudioModel_GetColorBuffer,
};