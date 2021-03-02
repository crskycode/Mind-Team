
#include <metahook.h>
#include <cvardef.h>
#include "engfuncs.h"
#include "sound.h"
#include "vector.h"


//voxword_t rgrgvoxword[CVOXSENTENCEMAX][CVOXWORDMAX];
voxword_t (*rgrgvoxword)[CVOXWORDMAX] = NULL;


/*
=================
S_FreeChannel
free channel so that it may be allocated by the
next request to play a sound.  If sound is a
word in a sentence, release the sentence.
Works for static, dynamic, sentence and stream sounds
=================
*/
void S_FreeChannel(channel_t *ch)
{
	ALuint	source;

	if (ch->entchannel == CHAN_STREAM)
	{
		//Wavstream_Close();
	}
	else
	{
	//	if (ch->entchannel >= CHAN_NETWORKVOICE_BASE && ch->entchannel <= CHAN_NETWORKVOICE_END)
	//		VoiceSE_NotifyFreeChannel(ch->entchannel);
	}

	if (ch->isentence >= 0)
		rgrgvoxword[ch->isentence][0].sfx = NULL;

	// clear all the data but without source object,
	// we can reuse the object
	source = ch->al_source;
	memset(ch, 0, sizeof(*ch));
	ch->isentence = -1;
	ch->al_source = source;

	// stop the source
	if (qalIsSource(ch->al_source))
	{
		qalSourceStop(ch->al_source);
		// release current buffer in the source
		qalSourcei(ch->al_source, AL_BUFFER, AL_NONE);
	}

//	int id = ch - channels;
//	gEngfuncs.Con_DPrintf("S_FreeChannel: (%d)\n", id);
}

qboolean S_CheckWavEnd(channel_t *ch, sfxcache_t **psc, int ltime, int ichan)
{
	sfxcache_t *sc = *psc;

	if (sc->al_loop >= 0)
	{
		qalSourcei(ch->al_source, AL_BYTE_OFFSET, sc->al_loop);
		qalSourcePlay(ch->al_source);
		return false;
	}

	if (ch->entchannel == CHAN_STREAM)
	{
		// play next chunk
	}
	else
	{
		if (ch->isentence >= 0)
		{
			voxword_t *word;
			sfx_t *sfx;

		//	if (rgrgvoxword[ch->isentence][ch->iword].sfx && !rgrgvoxword[ch->isentence][ch->iword].fKeepCached)
		//		Cache_Free(&rgrgvoxword[ch->isentence][ch->iword].sfx->cache);

			word = &rgrgvoxword[ch->isentence][ch->iword + 1];
			ch->sfx = sfx = word->sfx;

			if (sfx)
			{
				*psc = sc = S_LoadSound(sfx, ch);

				if (sc)
				{
					// -1 is default value in HL
					if (word->pitch <= 0)
						word->pitch = 100;

					// attach wave data buffer
					qalSourcei(ch->al_source, AL_BUFFER, sc->al_buffer);
					ch->al_sample = sc->al_sample;

					// set volume and pitch
					qalSourcef(ch->al_source, AL_GAIN, (ALfloat)word->volume / 255.0f);
					qalSourcef(ch->al_source, AL_PITCH, (ALfloat)word->pitch / 100.0f);

					// start playback
					qalSourcePlay(ch->al_source);

					ch->iword++;

//					gEngfuncs.Con_DPrintf("VOX: Play word(%s)\n", ch->sfx->name);
					return false;
				}
			}
		}
	}

	S_FreeChannel(ch);
	return true;
}


#define CSXROOM 29

typedef struct sx_preset_s
{
	float room_lp;					// for water fx, lowpass for entire room
	float room_mod;					// stereo amplitude modulation for room

	float room_size;				// reverb: initial reflection size
	float room_refl;				// reverb: decay time
	float room_rvblp;				// reverb: low pass filtering level

	float room_delay;				// mono delay: delay time
	float room_feedback;			// mono delay: decay time
	float room_dlylp;				// mono delay: low pass filtering level

	float room_left;				// left channel delay time
} sx_preset_t;


sx_preset_t rgsxpre[CSXROOM] =
{
// SXROOM_OFF					0

//	lp		mod		size	refl	rvblp	delay	feedbk	dlylp	left  
	{ 0.0,	0.0,	0.0,	0.0,	1.0,	0.0,	0.0,	2.0,	0.0 },

// SXROOM_GENERIC				1		// general, low reflective, diffuse room

//	lp		mod		size	refl	rvblp	delay	feedbk	dlylp	left  
	{ 0.0,	0.0,	0.0,	0.0,	1.0,	0.065,	0.1,	0.0,	0.01 },

// SXROOM_METALIC_S				2		// highly reflective, parallel surfaces
// SXROOM_METALIC_M				3
// SXROOM_METALIC_L				4

//	lp		mod		size	refl	rvblp	delay	feedbk	dlylp	left  
	{ 0.0,	0.0,	0.0,	0.0,	1.0,	0.02,	0.75,	0.0,	0.01 }, // 0.001
	{ 0.0,	0.0,	0.0,	0.0,	1.0,	0.03,	0.78,	0.0,	0.02 }, // 0.002
	{ 0.0,	0.0,	0.0,	0.0,	1.0,	0.06,	0.77,	0.0,	0.03 }, // 0.003


// SXROOM_TUNNEL_S				5		// resonant reflective, long surfaces
// SXROOM_TUNNEL_M				6
// SXROOM_TUNNEL_L				7

//	lp		mod		size	refl	rvblp	delay	feedbk	dlylp	left  
	{ 0.0,	0.0,	0.05,	0.85,	1.0,	0.008,	0.96,	2.0,	0.01 }, // 0.01
	{ 0.0,	0.0,	0.05,	0.88,	1.0,	0.001,	0.98,	2.0,	0.02 }, // 0.02
	{ 0.0,	0.0,	0.05,	0.92,	1.0,	0.015,	0.995,	2.0,	0.04 }, // 0.04

// SXROOM_CHAMBER_S				8		// diffuse, moderately reflective surfaces
// SXROOM_CHAMBER_M				9
// SXROOM_CHAMBER_L				10

//	lp		mod		size	refl	rvblp	delay	feedbk	dlylp	left  
	{ 0.0,	0.0,	0.05,	0.84,	1.0,	0.0,	0.0,	2.0,	0.012 }, // 0.003
	{ 0.0,	0.0,	0.05,	0.90,	1.0,	0.0,	0.0,	2.0,	0.008 }, // 0.002
	{ 0.0,	0.0,	0.05,	0.95,	1.0,	0.0,	0.0,	2.0,	0.004 }, // 0.001

// SXROOM_BRITE_S				11		// diffuse, highly reflective
// SXROOM_BRITE_M				12
// SXROOM_BRITE_L				13

//	lp		mod		size	refl	rvblp	delay	feedbk	dlylp	left  
	{ 0.0,	0.0,	0.05,	0.7,	0.0,	0.0,	0.0,	2.0,	0.012 }, // 0.003
	{ 0.0,	0.0,	0.055,	0.78,	0.0,	0.0,	0.0,	2.0,	0.008 }, // 0.002
	{ 0.0,	0.0,	0.05,	0.86,	0.0,	0.0,	0.0,	2.0,	0.002 }, // 0.001

// SXROOM_WATER1				14		// underwater fx
// SXROOM_WATER2				15
// SXROOM_WATER3				16

//	lp		mod		size	refl	rvblp	delay	feedbk	dlylp	left  
	{ 1.0,	0.0,	0.0,	0.0,	1.0,	0.0,	0.0,	2.0,	0.01 },
	{ 1.0,	0.0,	0.0,	0.0,	1.0,	0.06,	0.85,	2.0,	0.02 },
	{ 1.0,	0.0,	0.0,	0.0,	1.0,	0.2,	0.6,	2.0,	0.05 },

// SXROOM_CONCRETE_S			17		// bare, reflective, parallel surfaces
// SXROOM_CONCRETE_M			18
// SXROOM_CONCRETE_L			19

//	lp		mod		size	refl	rvblp	delay	feedbk	dlylp	left  
	{ 0.0,	0.0,	0.05,	0.8,	1.0,	0.0,	0.48,	2.0,	0.016 }, // 0.15 delay, 0.008 left
	{ 0.0,	0.0,	0.06,	0.9,	1.0,	0.0,	0.52,	2.0,	0.01 }, // 0.22 delay, 0.005 left
	{ 0.0,	0.0,	0.07,	0.94,	1.0,	0.3,	0.6,	2.0,	0.008 }, // 0.001

// SXROOM_OUTSIDE1				20		// echoing, moderately reflective
// SXROOM_OUTSIDE2				21		// echoing, dull
// SXROOM_OUTSIDE3				22		// echoing, very dull

//	lp		mod		size	refl	rvblp	delay	feedbk	dlylp	left  
	{ 0.0,	0.0,	0.0,	0.0,	1.0,	0.3,	0.42,	2.0,	0.0 },
	{ 0.0,	0.0,	0.0,	0.0,	1.0,	0.3,	0.48,	2.0,	0.0 },
	{ 0.0,	0.0,	0.0,	0.0,	1.0,	0.3,	0.6,	2.0,	0.0 },

// SXROOM_CAVERN_S				23		// large, echoing area
// SXROOM_CAVERN_M				24
// SXROOM_CAVERN_L				25

//	lp		mod		size	refl	rvblp	delay	feedbk	dlylp	left  
	{ 0.0,	0.0,	0.05,	0.9,	1.0,	0.2,	0.28,	0.0,	0.0 },
	{ 0.0,	0.0,	0.07,	0.9,	1.0,	0.3,	0.4,	0.0,	0.0 },
	{ 0.0,	0.0,	0.09,	0.9,	1.0,	0.3,	0.5,	0.0,	0.0 },

// SXROOM_WEIRDO1				26		
// SXROOM_WEIRDO2				27
// SXROOM_WEIRDO3				28

//	lp		mod		size	refl	rvblp	delay	feedbk	dlylp	left  
	{ 0.0,	1.0,	0.01,	0.9,	0.0,	0.0,	0.0,	2.0,	0.05 },
	{ 0.0,	0.0,	0.0,	0.0,	1.0,	0.009,	0.999,	2.0,	0.04 },
	{ 0.0,	0.0,	0.001,	0.999,	0.0,	0.2,	0.8,	2.0,	0.05 }
};


ALuint al_effectslot;

ALuint al_effects[CSXROOM];

qboolean sx_initialized = false;

cvar_t *sxroom_type;
cvar_t *sxroomwater_type;

// for fx switch
int sxroom_typeprev;

cvar_t *sxroom_off;


void SX_Init(void)
{
	int		i;

	sxroom_type = gEngfuncs.pfnGetCvarPointer("room_type");
	sxroomwater_type = gEngfuncs.pfnGetCvarPointer("waterroom_type");
	sxroom_off = gEngfuncs.pfnGetCvarPointer("room_off");

	if (!sxroom_off || !sxroomwater_type || !sxroom_off)
		return;

	sxroom_typeprev = 0;


	if (!qalGenAuxiliaryEffectSlots || !qalGenEffects)
		return;

	// clear last error
	qalGetError();

	qalGenAuxiliaryEffectSlots(1, &al_effectslot);

	if (qalGetError() != AL_NO_ERROR)
	{
		gEngfuncs.Con_DPrintf("OAL: qalGenAuxiliaryEffectSlots got an error !\n");
		return;
	}

	if (qalIsAuxiliaryEffectSlot(al_effectslot) == AL_FALSE)
	{
		gEngfuncs.Con_DPrintf("OAL: bad effect slot object !\n");
		return;
	}

	for (i = 1; i < CSXROOM; i++)
	{
		qalGetError();
		qalGenEffects(1, &al_effects[i]);

		if (qalGetError() != AL_NO_ERROR)
		{
			gEngfuncs.Con_DPrintf("OAL: qalGenEffects got an error !\n");
			return;
		}

		if (qalIsEffect(al_effects[i]) == AL_FALSE)
		{
			gEngfuncs.Con_DPrintf("OAL: bad effect object !\n");
			return;
		}

		qalEffecti(al_effects[i], AL_EFFECT_TYPE, AL_EFFECT_EAXREVERB);

		if (qalGetError() != AL_NO_ERROR)
		{
			gEngfuncs.Con_DPrintf("OAL: EAX Reverb no support\n");
			return;
		}

		//qalEffectf(al_effects[i], AL_EAXREVERB_GAINLF, rgsxpre[i].room_lp);
		//qalEffectf(al_effects[i], AL_EAXREVERB_MODULATION_DEPTH, rgsxpre[i].room_mod);

		// Reverb Density
		qalEffectf(al_effects[i], AL_EAXREVERB_DENSITY, rgsxpre[i].room_size);
		qalEffectf(al_effects[i], AL_EAXREVERB_DECAY_TIME, rgsxpre[i].room_refl);

		// Reflections
		qalEffectf(al_effects[i], AL_EAXREVERB_REFLECTIONS_DELAY, rgsxpre[i].room_delay);
		qalEffectf(al_effects[i], AL_EAXREVERB_REFLECTIONS_GAIN, rgsxpre[i].room_feedback);
	}

	sx_initialized = true;
}

void SX_RoomFX(void)
{
	int		i;
	channel_t	*ch;

	if (!sx_initialized)
		return;

	if (sxroom_off->value > 0.0f)
		return;

	if (sxroom_type->value < 0 || sxroom_type->value > CSXROOM - 1)
	{
		if (sxroom_type->value < 0)
			sxroom_type->value = 0;
		if (sxroom_type->value > CSXROOM - 1)
			sxroom_type->value = CSXROOM - 1;
		return;
	}

	if (sxroom_type->value != sxroom_typeprev)
	{
		sxroom_typeprev = sxroom_type->value;
		qalAuxiliaryEffectSloti(al_effectslot, AL_EFFECTSLOT_EFFECT, al_effects[sxroom_typeprev]);
	}

	ch = &channels[NUM_AMBIENTS];
	for (i = NUM_AMBIENTS; i < total_channels; i++, ch++)
	{
		if (ch->sfx == NULL)
			continue;

		if (qalIsSource(ch->al_source) == AL_FALSE)
			continue;

		if (sxroom_type->value > 0)
		{
			qalSource3i(ch->al_source, AL_AUXILIARY_SEND_FILTER, al_effectslot, 0, AL_FILTER_NULL);
		}
		else
		{
			qalSource3i(ch->al_source, AL_AUXILIARY_SEND_FILTER, AL_EFFECT_NULL, 0, AL_FILTER_NULL);
		}
	}
}