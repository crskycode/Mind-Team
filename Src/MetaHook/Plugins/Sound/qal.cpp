
#include <metahook.h>
#include "engfuncs.h"
#include "qal.h"

LPALBUFFERDATA qalBufferData = NULL;
LPALDELETEBUFFERS qalDeleteBuffers = NULL;
LPALDISTANCEMODEL qalDistanceModel = NULL;
LPALGENBUFFERS qalGenBuffers = NULL;
LPALGENSOURCES qalGenSources = NULL;
LPALGETERROR qalGetError = NULL;
LPALGETBUFFERI qalGetBufferi = NULL;
LPALGETPROCADDRESS qalGetProcAddress = NULL;
LPALGETSOURCEI qalGetSourcei = NULL;
LPALISBUFFER qalIsBuffer = NULL;
LPALISSOURCE qalIsSource = NULL;
LPALLISTENERF qalListenerf = NULL;
LPALLISTENERFV qalListenerfv = NULL;
LPALSOURCE3I qalSource3i = NULL;
LPALSOURCEF qalSourcef = NULL;
LPALSOURCEFV qalSourcefv = NULL;
LPALSOURCEI qalSourcei = NULL;
LPALSOURCEPLAY qalSourcePlay = NULL;
LPALSOURCESTOP qalSourceStop = NULL;

LPALCCLOSEDEVICE qalcCloseDevice = NULL;
LPALCCREATECONTEXT qalcCreateContext = NULL;
LPALCDESTROYCONTEXT qalcDestroyContext = NULL;
LPALCGETSTRING qalcGetString = NULL;
LPALCMAKECONTEXTCURRENT qalcMakeContextCurrent = NULL;
LPALCOPENDEVICE qalcOpenDevice = NULL;

LPALDELETEEFFECTS qalDeleteEffects = NULL;
LPALEFFECTF qalEffectf = NULL;
LPALEFFECTI qalEffecti = NULL;
LPALGENEFFECTS qalGenEffects = NULL;
LPALISEFFECT qalIsEffect = NULL;

LPALAUXILIARYEFFECTSLOTI qalAuxiliaryEffectSloti = NULL;
LPALDELETEAUXILIARYEFFECTSLOTS qalDeleteAuxiliaryEffectSlots = NULL;
LPALGENAUXILIARYEFFECTSLOTS qalGenAuxiliaryEffectSlots = NULL;
LPALISAUXILIARYEFFECTSLOT qalIsAuxiliaryEffectSlot = NULL;


#ifdef AL_CHECK_ERROR

void DisplayALError(const char *tag)
{
	ALenum error = qalGetError();

	switch (error)
	{
	case AL_NO_ERROR:
		break;
	case AL_INVALID_NAME:
		Sys_Error("%s: AL_INVALID_NAME\n", tag);
		break;
	case AL_INVALID_ENUM:
		Sys_Error("%s: AL_INVALID_ENUM\n", tag);
		break;
	case AL_INVALID_VALUE:
		Sys_Error("%s: AL_INVALID_VALUE\n", tag);
		break;
	case AL_INVALID_OPERATION:
		Sys_Error("%s: AL_INVALID_OPERATION\n", tag);
		break;
	case AL_OUT_OF_MEMORY:
		Sys_Error("%s: AL_OUT_OF_MEMORY\n", tag);
		break;
	default:
		Sys_Error("%s: 0x%x\n", tag, error);
		break;
	}
}

LPALBUFFERDATA src_qalBufferData;
LPALDELETEBUFFERS src_qalDeleteBuffers;
LPALDISTANCEMODEL src_qalDistanceModel;
LPALGENBUFFERS src_qalGenBuffers;
LPALGENSOURCES src_qalGenSources;
//LPALGETERROR src_qalGetError;
LPALGETBUFFERI src_qalGetBufferi;
LPALGETSOURCEI src_qalGetSourcei;
//LPALISBUFFER src_qalIsBuffer;
//LPALISSOURCE src_qalIsSource;
LPALLISTENERFV src_qalListenerfv;
LPALSOURCEF src_qalSourcef;
LPALSOURCEFV src_qalSourcefv;
LPALSOURCEI src_qalSourcei;
LPALSOURCEPLAY src_qalSourcePlay;
LPALSOURCESTOP src_qalSourceStop;

void dbg_qalBufferData(ALuint bid, ALenum format, const ALvoid* data, ALsizei size, ALsizei freq)
{
	qalGetError();
	src_qalBufferData(bid, format, data, size, freq);
	DisplayALError(__FUNCTION__);
}

void dbg_qalDeleteBuffers(ALsizei n, const ALuint* buffers)
{
	qalGetError();
	src_qalDeleteBuffers(n, buffers);
	DisplayALError(__FUNCTION__);
}

void dbg_qalDistanceModel(ALenum distanceModel)
{
	qalGetError();
	src_qalDistanceModel(distanceModel);
	DisplayALError(__FUNCTION__);
}

void dbg_qalGenBuffers(ALsizei n, ALuint* buffers)
{
	qalGetError();
	src_qalGenBuffers(n, buffers);
	DisplayALError(__FUNCTION__);
}

void dbg_qalGenSources(ALsizei n, ALuint* sources)
{
	qalGetError();
	src_qalGenSources(n, sources);
	DisplayALError(__FUNCTION__);
}

void dbg_qalGetBufferi(ALuint sid, ALenum param, ALint* value)
{
	qalGetError();
	src_qalGetBufferi(sid, param, value);
	DisplayALError(__FUNCTION__);
}

void dbg_qalGetSourcei(ALuint sid, ALenum param, ALint* value)
{
	qalGetError();
	src_qalGetSourcei(sid, param, value);
	DisplayALError(__FUNCTION__);
}

void dbg_qalListenerfv(ALenum param, const ALfloat* values)
{
	qalGetError();
	src_qalListenerfv(param, values);
	DisplayALError(__FUNCTION__);
}

void dbg_qalSourcef(ALuint sid, ALenum param, ALfloat value)
{
	qalGetError();
	src_qalSourcef(sid, param, value);
	DisplayALError(__FUNCTION__);
}

void dbg_qalSourcefv(ALuint sid, ALenum param, const ALfloat* values)
{
	qalGetError();
	src_qalSourcefv(sid, param, values);
	DisplayALError(__FUNCTION__);
}

void dbg_qalSourcei(ALuint sid, ALenum param, ALint value)
{
	qalGetError();
	src_qalSourcei(sid, param, value);
	DisplayALError(__FUNCTION__);
}

void dbg_qalSourcePlay(ALuint sid)
{
	qalGetError();
	src_qalSourcePlay(sid);
	DisplayALError(__FUNCTION__);
}

void dbg_qalSourceStop(ALuint sid)
{
	qalGetError();
	src_qalSourceStop(sid);
	DisplayALError(__FUNCTION__);
}

#endif


void QAL_Init(void)
{
	HMODULE hOpenAL = LoadLibrary("soft_oal.dll");

	if (!hOpenAL)
	{
		Sys_Error("QAL: Failed to load OpenAL library");
		return;
	}

	qalBufferData = (LPALBUFFERDATA)GetProcAddress(hOpenAL, "alBufferData");
	qalDeleteBuffers = (LPALDELETEBUFFERS)GetProcAddress(hOpenAL, "alDeleteBuffers");
	qalDistanceModel = (LPALDISTANCEMODEL)GetProcAddress(hOpenAL, "alDistanceModel");
	qalGenBuffers = (LPALGENBUFFERS)GetProcAddress(hOpenAL, "alGenBuffers");
	qalGenSources = (LPALGENSOURCES)GetProcAddress(hOpenAL, "alGenSources");
	qalGetError = (LPALGETERROR)GetProcAddress(hOpenAL, "alGetError");
	qalGetBufferi = (LPALGETBUFFERI)GetProcAddress(hOpenAL, "alGetBufferi");
	qalGetProcAddress = (LPALGETPROCADDRESS)GetProcAddress(hOpenAL, "alGetProcAddress");
	qalGetSourcei = (LPALGETSOURCEI)GetProcAddress(hOpenAL, "alGetSourcei");
	qalIsBuffer = (LPALISBUFFER)GetProcAddress(hOpenAL, "alIsBuffer");
	qalIsSource = (LPALISSOURCE)GetProcAddress(hOpenAL, "alIsSource");
	qalListenerf = (LPALLISTENERF)GetProcAddress(hOpenAL, "alListenerf");
	qalListenerfv = (LPALLISTENERFV)GetProcAddress(hOpenAL, "alListenerfv");
	qalSourcePlay = (LPALSOURCEPLAY)GetProcAddress(hOpenAL, "alSourcePlay");
	qalSourceStop = (LPALSOURCESTOP)GetProcAddress(hOpenAL, "alSourceStop");
	qalSource3i = (LPALSOURCE3I)GetProcAddress(hOpenAL, "alSource3i");
	qalSourcef = (LPALSOURCEF)GetProcAddress(hOpenAL, "alSourcef");
	qalSourcefv = (LPALSOURCEFV)GetProcAddress(hOpenAL, "alSourcefv");
	qalSourcei = (LPALSOURCEI)GetProcAddress(hOpenAL, "alSourcei");

	if (!qalBufferData || !qalDeleteBuffers || !qalDistanceModel || !qalGenBuffers
		|| !qalGenSources || !qalGetError || !qalGetBufferi || !qalGetSourcei || !qalIsBuffer
		|| !qalIsSource || !qalListenerf || !qalListenerfv || !qalSourcePlay
		|| !qalSourceStop || !qalSource3i || !qalSourcef || !qalSourcefv || !qalSourcei)
	{
		Sys_Error("QAL: Failed to get AL APIs");
	}

#if AL_CHECK_ERROR
	src_qalBufferData = qalBufferData;
	src_qalDeleteBuffers = qalDeleteBuffers;
	src_qalDistanceModel = qalDistanceModel;
	src_qalGenBuffers = qalGenBuffers;
	src_qalGenSources = qalGenSources;
	src_qalGetBufferi = qalGetBufferi;
	src_qalGetSourcei = qalGetSourcei;
	src_qalListenerfv = qalListenerfv;
	src_qalSourcePlay = qalSourcePlay;
	src_qalSourceStop = qalSourceStop;
	src_qalSourcef = qalSourcef;
	src_qalSourcefv = qalSourcefv;
	src_qalSourcei = qalSourcei;

	qalBufferData = dbg_qalBufferData;
	qalDeleteBuffers = dbg_qalDeleteBuffers;
	qalDistanceModel = dbg_qalDistanceModel;
	qalGenBuffers = dbg_qalGenBuffers;
	qalGenSources = dbg_qalGenSources;
	qalGetBufferi = dbg_qalGetBufferi;
	qalGetSourcei = dbg_qalGetSourcei;
	qalListenerfv = dbg_qalListenerfv;
	qalSourcePlay = dbg_qalSourcePlay;
	qalSourceStop = dbg_qalSourceStop;
	qalSourcef = dbg_qalSourcef;
	qalSourcefv = dbg_qalSourcefv;
	qalSourcei = dbg_qalSourcei;
#endif

	qalcCloseDevice = (LPALCCLOSEDEVICE)GetProcAddress(hOpenAL, "alcCloseDevice");
	qalcCreateContext = (LPALCCREATECONTEXT)GetProcAddress(hOpenAL, "alcCreateContext");
	qalcDestroyContext = (LPALCDESTROYCONTEXT)GetProcAddress(hOpenAL, "alcDestroyContext");
	qalcGetString = (LPALCGETSTRING)GetProcAddress(hOpenAL, "alcGetString");
	qalcMakeContextCurrent = (LPALCMAKECONTEXTCURRENT)GetProcAddress(hOpenAL, "alcMakeContextCurrent");
	qalcOpenDevice = (LPALCOPENDEVICE)GetProcAddress(hOpenAL, "alcOpenDevice");

	if (!qalcCloseDevice || !qalcCreateContext || !qalcDestroyContext
		|| !qalcGetString || !qalcMakeContextCurrent || !qalcOpenDevice)
	{
		Sys_Error("QAL: Failed to get ALC APIs");
	}

	qalDeleteEffects = (LPALDELETEEFFECTS)qalGetProcAddress("alDeleteEffects");
	qalEffectf = (LPALEFFECTF)qalGetProcAddress("alEffectf");
	qalEffecti = (LPALEFFECTI)qalGetProcAddress("alEffecti");
	qalGenEffects = (LPALGENEFFECTS)qalGetProcAddress("alGenEffects");
	qalIsEffect = (LPALISEFFECT)qalGetProcAddress("alIsEffect");

	if (!qalDeleteEffects || !qalEffectf || !qalEffecti || !qalGenEffects
		|| !qalIsEffect)
	{
		gEngfuncs.Con_DPrintf("QAL: Failed to get Effect APIs");
	}

	qalAuxiliaryEffectSloti = (LPALAUXILIARYEFFECTSLOTI)qalGetProcAddress("alAuxiliaryEffectSloti");
	qalDeleteAuxiliaryEffectSlots = (LPALDELETEAUXILIARYEFFECTSLOTS)qalGetProcAddress("alDeleteAuxiliaryEffectSlots");
	qalGenAuxiliaryEffectSlots = (LPALGENAUXILIARYEFFECTSLOTS)qalGetProcAddress("alGenAuxiliaryEffectSlots");
	qalIsAuxiliaryEffectSlot = (LPALISAUXILIARYEFFECTSLOT)qalGetProcAddress("alIsAuxiliaryEffectSlot");

	if (!qalAuxiliaryEffectSloti || !qalDeleteAuxiliaryEffectSlots
		|| !qalGenAuxiliaryEffectSlots || !qalIsAuxiliaryEffectSlot)
	{
		gEngfuncs.Con_DPrintf("QAL: Failed to get EffectSlot APIs");
	}
}