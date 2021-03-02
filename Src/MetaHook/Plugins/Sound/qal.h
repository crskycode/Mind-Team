
#ifndef QAL_H
#define QAL_H

#define AL_NO_PROTOTYPES
#include "al/al.h"
#include "al/alc.h"
#include "al/efx.h"
#include "al/efx-creative.h"

extern LPALBUFFERDATA qalBufferData;
extern LPALDELETEBUFFERS qalDeleteBuffers;
extern LPALDISTANCEMODEL qalDistanceModel;
extern LPALGENBUFFERS qalGenBuffers;
extern LPALGENSOURCES qalGenSources;
extern LPALGETERROR qalGetError;
extern LPALGETBUFFERI qalGetBufferi;
extern LPALGETSOURCEI qalGetSourcei;
extern LPALISBUFFER qalIsBuffer;
extern LPALISSOURCE qalIsSource;
extern LPALLISTENERF qalListenerf;
extern LPALLISTENERFV qalListenerfv;
extern LPALSOURCE3I qalSource3i;
extern LPALSOURCEF qalSourcef;
extern LPALSOURCEFV qalSourcefv;
extern LPALSOURCEI qalSourcei;
extern LPALSOURCEPLAY qalSourcePlay;
extern LPALSOURCESTOP qalSourceStop;

extern LPALCCLOSEDEVICE qalcCloseDevice;
extern LPALCCREATECONTEXT qalcCreateContext;
extern LPALCDESTROYCONTEXT qalcDestroyContext;
extern LPALCGETSTRING qalcGetString;
extern LPALCMAKECONTEXTCURRENT qalcMakeContextCurrent;
extern LPALCOPENDEVICE qalcOpenDevice;

extern LPALDELETEEFFECTS qalDeleteEffects;
extern LPALEFFECTF qalEffectf;
extern LPALEFFECTI qalEffecti;
extern LPALGENEFFECTS qalGenEffects;
extern LPALISEFFECT qalIsEffect;

extern LPALAUXILIARYEFFECTSLOTI qalAuxiliaryEffectSloti;
extern LPALDELETEAUXILIARYEFFECTSLOTS qalDeleteAuxiliaryEffectSlots;
extern LPALGENAUXILIARYEFFECTSLOTS qalGenAuxiliaryEffectSlots;
extern LPALISAUXILIARYEFFECTSLOT qalIsAuxiliaryEffectSlot;


void QAL_Init(void);

#endif