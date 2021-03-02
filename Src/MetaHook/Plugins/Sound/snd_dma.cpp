
#include <metahook.h>
#include <cvardef.h>
#include "engfuncs.h"
#include "sound.h"
#include "vector.h"
#include "gl_model.h"

#include <string>

#if _MSC_VER > 1400
#include <unordered_map>
typedef std::unordered_map<std::string, sfx_t *> sfxmap_t;
#else
#include <hash_map>
typedef stdext::hash_map<std::string, sfx_t *> sfxmap_t;
#endif

sfxmap_t sfxmap;


channel_t	channels[MAX_CHANNELS];
int			total_channels;

qboolean snd_initialized = false;


sfx_t	known_sfx[MAX_SFX];
int		num_sfx;

qboolean sound_started = false;

cvar_t *volume;
cvar_t *nosound;


vec3_t vec3_origin = { 0.0f, 0.0f, 0.0f };

vec3_t listener_origin;
vec3_t listener_forward;


void S_Startup(void)
{
	g_pfn_S_Startup();	// call engine function

	//if (!snd_initialized)
	//	return;

	sound_started = true;
}

void AL_Startup(void)
{
	ALCdevice *device = qalcOpenDevice(NULL);

	if (!device)
	{
		Sys_Error("Couldn't to open sound device");
		return;
	}

	const ALCchar *deviceName = qalcGetString(device, ALC_DEVICE_SPECIFIER);
	gEngfuncs.Con_DPrintf("Sound device: %s\n", deviceName);

	ALCcontext *context = qalcCreateContext(device, NULL);

	if (!context)
	{
		qalcCloseDevice(device);
		Sys_Error("Couldn't to create sound context");
		return;
	}

	qalcMakeContextCurrent(context);

	// select distance model
	qalDistanceModel(AL_LINEAR_DISTANCE);
}

void S_Init(void)
{
	g_pfn_S_Init();

 	QAL_Init();
	AL_Startup();

	volume = gEngfuncs.pfnGetCvarPointer("volume");
	nosound = gEngfuncs.pfnGetCvarPointer("nosound");

	memset(channels, 0, sizeof(channels));
	total_channels = 0;

	snd_initialized = true;
	
	memset(known_sfx, 0, sizeof(known_sfx));
	num_sfx = 0;

	// fill address
	rgrgvoxword = (voxword_t(*)[CVOXWORDMAX])g_pfn_rgrgvoxword;

	S_StopAllSounds(true);

//	SX_Init();
}

void S_Shutdown(void)
{
	g_pfn_S_Shutdown();

	if (!sound_started)
		return;

	sound_started = false;
}

sfx_t *S_FindName(char *name, qboolean *pfInCache)
{
	int		i;
	sfx_t	*sfx = NULL;
	sfxcache_t	*sc;

	if (!name)
		Sys_Error("S_FindName: NULL\n");

	if (strlen(name) >= MAX_QPATH)
		Sys_Error("Sound name too long: %s", name);

	/*for (i = 0; i < num_sfx; i++)
	{
		if (!stricmp(known_sfx[i].name, name))
		{
			if (pfInCache)
			{
				*pfInCache = Cache_Check(&known_sfx[i].cache) ? true : false;
			}

			if (known_sfx[i].servercount > 0)
				known_sfx[i].servercount = cl_servercount;

			return &known_sfx[i];
		}

		if (!sfx)
		{
			if (known_sfx[i].servercount > 0)
			{
				if (known_sfx[i].servercount != cl_servercount)
					sfx = &known_sfx[i];
			}
		}
	}

	if (!sfx)
	{
		if (num_sfx >= MAX_SFX)
			Sys_Error("S_FindName: out of sfx_t");

		sfx = &known_sfx[i];
		num_sfx++;
	}
	else
	{
		sc = (sfxcache_t *)Cache_Check(&sfx->cache);
		if (sc)
		{
			if (qalIsBuffer(sc->al_buffer))
				qalDeleteBuffers(1, &sc->al_buffer);

			Cache_Free(&sfx->cache);
		}
	}

	strncpy(sfx->name, name, sizeof(sfx->name) - 1);
	sfx->name[sizeof(sfx->name) - 1] = 0;

	if (pfInCache)
	{
		*pfInCache = false;
	}

	sfx->servercount = cl_servercount;
	return sfx;*/

	sfxmap_t::iterator it = sfxmap.find(name);

	if (it != sfxmap.end())
		return it->second;

	sfx = new sfx_t;
	memset(sfx, 0, sizeof(sfx_t));
	strcpy(sfx->name, name);

	sfxmap.insert(sfxmap_t::value_type(name, sfx));
	return sfx;
}

qboolean SND_FStreamIsPlaying(sfx_t *sfx)
{
	int	ch_idx;

	for (ch_idx = NUM_AMBIENTS; ch_idx < total_channels; ch_idx++)
	{
		if (channels[ch_idx].sfx == sfx)
			return true;
	}

	return false;
}

/*
=================
SND_PickDynamicChannel
Select a channel from the dynamic channel allocation area.  For the given entity,
override any other sound playing on the same channel (see code comments below for
exceptions).
=================
*/
channel_t *SND_PickDynamicChannel(int entnum, int entchannel, sfx_t *sfx)
{
	int		ch_idx;
	int		first_to_die;
	ALuint	source;
	ALint	left;	// samples
	ALint	value;

	if (entchannel == CHAN_STREAM && SND_FStreamIsPlaying(sfx))
		return NULL;

	// Check for replacement sound, or find the best one to replace
	first_to_die = -1;
	left = 0x7fffffff;
	for (ch_idx = NUM_AMBIENTS; ch_idx < NUM_AMBIENTS + MAX_DYNAMIC_CHANNELS; ch_idx++)
	{
		// Never override a streaming sound that is currently playing or
		// voice over IP data that is playing or any sound on CHAN_VOICE( acting )
		if (channels[ch_idx].entchannel == CHAN_STREAM)
		{
			if (entchannel == CHAN_VOICE)
				return NULL;

			continue;
		}

		if (entchannel != 0		// channel 0 never overrides
			&& channels[ch_idx].entnum == entnum
			&& (channels[ch_idx].entchannel == entchannel || entchannel == -1))
		{	// allways override sound from same entity
			first_to_die = ch_idx;
			break;
		}

		// don't let monster sounds override player sounds
		if (channels[ch_idx].entnum == cl_viewentity && entnum != cl_viewentity && channels[ch_idx].sfx)
			continue;

		// looking for unused channel
		if (channels[ch_idx].sfx == NULL)
		{
			first_to_die = ch_idx;
			break;
		}

		// if the channel is playing, pick one with the least time left
		source = channels[ch_idx].al_source;
		if (qalIsSource(source))
		{
			qalGetSourcei(source, AL_SOURCE_STATE, &value);

			// the source has finished playing
			if (value == AL_INITIAL || value == AL_STOPPED)
			{
				first_to_die = ch_idx;
				break;
			}
			else
			{
				ALint count;
				// get played samples count
				qalGetSourcei(source, AL_SAMPLE_OFFSET, &value);
				count = channels[ch_idx].al_sample - value;
				if (count < left)
				{
					left = count;
					first_to_die = ch_idx;
				}
			}
		}
		else
		{
			// the channel not source object
			first_to_die = ch_idx;
			break;
		}
	}

	if (first_to_die == -1)
		return NULL;

	if (channels[first_to_die].sfx)
	{
//		gEngfuncs.Con_DPrintf("SND_PickDynamicChannel: replacing channel(%d)\n", first_to_die);
		// be sure and release previous channel
		S_FreeChannel(&channels[first_to_die]);
	}

	return &channels[first_to_die];
}

/*
=================
SND_PickStaticChannel
Pick an empty channel from the static sound area, or allocate a new
channel.  Only fails if we're at max_channels (128!!!) or if
we're trying to allocate a channel for a stream sound that is
already playing.
=================
*/
channel_t *SND_PickStaticChannel(int entnum, int entchannel, sfx_t *sfx)
{
	int			i;
	channel_t	*ch = NULL;

	// Check for replacement sound, or find the best one to replace
	for (i = NUM_AMBIENTS + MAX_DYNAMIC_CHANNELS; i < total_channels; i++)
		if (channels[i].sfx == NULL)
			break;

	if (i < total_channels)
	{
		// reuse an empty static sound channel
		ch = &channels[i];
	}
	else
	{
		// no empty slots, alloc a new static sound channel
		if (total_channels >= MAX_CHANNELS)
		{
			gEngfuncs.Con_DPrintf("total_channels == MAX_CHANNELS\n");
			return NULL;
		}

		// get a channel for the static sound
		ch = &channels[total_channels];
		total_channels++;
	}

	return ch;
}

void SND_Spatialize(channel_t *ch)
{
	cl_entity_t	*pent;
	ALboolean isal;

	isal = qalIsSource(ch->al_source);

	// anything coming from the view entity will allways be full volume
	if (ch->entnum == cl_viewentity)
	{
		if (isal)
		{
			qalSourcefv(ch->al_source, AL_POSITION, vec3_origin);
			qalSourcei(ch->al_source, AL_SOURCE_RELATIVE, AL_TRUE);
		}
		return;
	}

	pent = gEngfuncs.GetEntityByIndex(ch->entnum);

	if (pent && pent->model && pent->curstate.messagenum == cl_parsecount)
	{
		VectorCopy(pent->origin, ch->origin);

		if (pent->model->type == mod_brush)
		{
			ch->origin[0] = (pent->model->mins[0] + pent->model->maxs[0]) * 0.5f;
			ch->origin[1] = (pent->model->mins[1] + pent->model->maxs[1]) * 0.5f;
			ch->origin[2] = (pent->model->mins[2] + pent->model->maxs[2]) * 0.5f;
		}
	}
	
	if (isal)
	{
		vec3_t	pos;
		VectorCopy_HL2AL(ch->origin, pos);
		qalSourcefv(ch->al_source, AL_POSITION, pos);
		qalSourcei(ch->al_source, AL_SOURCE_RELATIVE, AL_FALSE);
	}
}

/*
=================
S_AlterChannel
search through all channels for a channel that matches this
soundsource, entchannel and sfx, and perform alteration on channel
as indicated by 'flags' parameter. If shut down request and
sfx contains a sentence name, shut off the sentence.
returns TRUE if sound was altered,
returns FALSE if sound was not found (sound is not playing)
=================
*/
qboolean S_AlterChannel(int entnum, int entchannel, sfx_t *sfx, int vol, int pitch, int flags)
{
	int	ch_idx;
	ALuint	source;

	if (sfx->name[0] == '!')
	{
		// This is a sentence name.
		// For sentences: assume that the entity is only playing one sentence
		// at a time, so we can just shut off
		// any channel that has ch->isentence >= 0 and matches the
		// soundsource.
		for (ch_idx = 0; ch_idx < total_channels; ch_idx++)
		{
			if (channels[ch_idx].entnum == entnum
				&& channels[ch_idx].entchannel == entchannel
				&& channels[ch_idx].sfx != NULL
				&& channels[ch_idx].isentence >= 0)
			{
				if (flags & SND_CHANGE_PITCH)
					channels[ch_idx].pitch = pitch;

				if (flags & SND_CHANGE_VOL)
					channels[ch_idx].master_vol = vol;

				source = channels[ch_idx].al_source;
				if (qalIsSource(source))
				{
					if (flags & SND_CHANGE_PITCH)
						qalSourcef(source, AL_PITCH, (ALfloat)pitch / 100.0f);

					if (flags & SND_CHANGE_VOL)
						qalSourcef(source, AL_GAIN, (ALfloat)vol / 255.0f);
				}

				if (flags & SND_STOP)
				{
					S_FreeChannel(&channels[ch_idx]);
				}

				return true;
			}
		}
		// channel not found
		return false;
	}

	// regular sound or streaming sound

	for (ch_idx = 0; ch_idx < total_channels; ch_idx++)
	{
		if (channels[ch_idx].entnum == entnum
			&& channels[ch_idx].entchannel == entchannel
			&& channels[ch_idx].sfx == sfx)
		{
			if (flags & SND_CHANGE_PITCH)
				channels[ch_idx].pitch = pitch;

			if (flags & SND_CHANGE_VOL)
				channels[ch_idx].master_vol = vol;

			source = channels[ch_idx].al_source;
			if (qalIsSource(source))
			{
				if (flags & SND_CHANGE_PITCH)
					qalSourcef(source, AL_PITCH, (ALfloat)pitch / 100.0f);

				if (flags & SND_CHANGE_VOL)
					qalSourcef(source, AL_GAIN, (ALfloat)vol / 255.0f);
			}

			if (flags & SND_STOP)
			{
				S_FreeChannel(&channels[ch_idx]);
			}

			return true;
		}
	}

	return false;
}

/*
=================
S_StartDynamicSound
Start a sound effect for the given entity on the given channel (ie; voice, weapon etc).
Try to grab a channel out of the 8 dynamic spots available.
Currently used for looping sounds, streaming sounds, sentences, and regular entity sounds.
NOTE: volume is 0.0 - 1.0 and attenuation is 0.0 - 1.0 when passed in.
Pitch changes playback pitch of wave by % above or below 100.  Ignored if pitch == 100
NOTE: it's not a good idea to play looping sounds through StartDynamicSound, because
if the looping sound starts out of range, or is bumped from the buffer by another sound
it will never be restarted.  Use StartStaticSound (pass CHAN_STATIC to EMIT_SOUND or
SV_StartSound.
=================
*/
void S_StartDynamicSound(int entnum, int entchannel, sfx_t *sfx, vec3_t origin, float fvol, float attenuation, int flags, int pitch)
{
	channel_t	*ch;
	sfxcache_t	*sc;
	int			vol;

	if (!sound_started)
		return;

	if (!sfx)
		return;

	if (nosound && nosound->value)
		return;

	// override the entchannel to CHAN_STREAM if this is a 
	// stream sound.
	if (sfx->name[0] == '*')
		entchannel = CHAN_STREAM;

	// does not support voice data
	if (sfx->name[0] == '?')
	{
		gEngfuncs.Con_DPrintf("Warning: naive?\n");
		return;
	}

	if (entchannel == CHAN_STREAM && pitch != 100)
	{
		gEngfuncs.Con_DPrintf("Warning: pitch shift ignored on stream sound %s\n", sfx->name);
		pitch = 100;
	}

	vol = fvol * 255;

	if (vol > 255)
	{
		gEngfuncs.Con_Printf("S_StartDynamicSound: %s volume > 255\n", sfx->name);
		vol = 255;
	}

	if (flags & (SND_STOP | SND_CHANGE_VOL | SND_CHANGE_PITCH))
	{
		if (S_AlterChannel(entnum, entchannel, sfx, vol, pitch, flags))
			return;
		if (flags & SND_STOP)
			return;
		// fall through - if we're not trying to stop the sound, 
		// and we didn't find it (it's not playing), go ahead and start it up
	}

	if (pitch == 0)
	{
		gEngfuncs.Con_DPrintf("Warning: S_StartDynamicSound Ignored, called with pitch 0\n");
		return;
	}

	// pick a channel to play on
	ch = SND_PickDynamicChannel(entnum, entchannel, sfx);

	if (!ch)
		return;

	if (sfx->name[0] == '!' || sfx->name[0] == '#')
	{
		// this is a sentence
		// link all words and load the first word

		// NOTE: sentence names stored in the cache lookup are
		// prepended with a '!'.  Sentence names stored in the
		// sentence file do not have a leading '!'. 

		char name[MAX_QPATH];
		strncpy(name, sfx->name + 1, sizeof(name) - 1);
		name[sizeof(name) - 1] = 0;
		sc = g_pfn_VOX_LoadSound(ch, name);
	}
	else
	{
		// regular or streamed sound fx
		sc = S_LoadSound(sfx, ch);
		ch->sfx = sfx;
		ch->isentence = -1;
	}

	if (!sc)
	{
		ch->sfx = NULL;
		return;		// couldn't load the sound's data
	}

	ch->entnum = entnum;
	ch->entchannel = entchannel;
	VectorCopy(origin, ch->origin);
	ch->master_vol = vol;
	ch->pitch = pitch;

	if (qalIsSource(ch->al_source) == AL_FALSE)
	{
		qalGetError();	//clear
		qalGenSources(1, &ch->al_source);

		if (qalGetError() != AL_NO_ERROR)
		{
			memset(ch, 0, sizeof(*ch));
			ch->isentence = -1;
			gEngfuncs.Con_DPrintf("AL: qalGenSources got an error!\n");
			return;
		}

		int chidx = ch - channels;
//		gEngfuncs.Con_DPrintf("AL: Generated source object(%d) for dynamic channel(%d)\n", ch->al_source, chidx);
	}

	SND_Spatialize(ch);

	if (qalIsSource(ch->al_source))
	{
		// setup distance model
		qalSourcef(ch->al_source, AL_REFERENCE_DISTANCE, 0.0f);
		qalSourcef(ch->al_source, AL_MAX_DISTANCE, 1024.0f);
		qalSourcef(ch->al_source, AL_ROLLOFF_FACTOR, attenuation);

		// attach wave data buffer
		qalSourcei(ch->al_source, AL_BUFFER, sc->al_buffer);
		ch->al_sample = sc->al_sample;

		// set volume and pitch
		qalSourcef(ch->al_source, AL_GAIN, (ALfloat)ch->master_vol / 255.0f);
		qalSourcef(ch->al_source, AL_PITCH, (ALfloat)ch->pitch / 100.0f);

		// start playback
		qalSourcePlay(ch->al_source);
	}

//	int chidx = ch - channels;
//	gEngfuncs.Con_DPrintf("S_StartDynamicSound: Play sound(%s) entnum(%d) entchannel(%d) at channel(%d)\n",
//		sfx->name, entnum, entchannel, chidx);
}

/*
=================
S_StartStaticSound
Start playback of a sound, loaded into the static portion of the channel array.
Currently, this should be used for looping ambient sounds, looping sounds
that should not be interrupted until complete, non-creature sentences,
and one-shot ambient streaming sounds.  Can also play 'regular' sounds one-shot,
in case designers want to trigger regular game sounds.
Pitch changes playback pitch of wave by % above or below 100.  Ignored if pitch == 100
NOTE: volume is 0.0 - 1.0 and attenuation is 0.0 - 1.0 when passed in.
=================
*/
void S_StartStaticSound(int entnum, int entchannel, sfx_t *sfx, vec3_t origin, float fvol, float attenuation, int flags, int pitch)
{
	channel_t	*ch;
	sfxcache_t	*sc;
	qboolean	fvox;
	int		vol;

	if (!sfx)
		return;

	if (nosound && nosound->value)
		return;

	// override the entchannel to CHAN_STREAM if this is a
	// stream sound.
	if (sfx->name[0] == '*')
		entchannel = CHAN_STREAM;

	// does not support voice data
	if (sfx->name[0] == '?')
	{
		gEngfuncs.Con_DPrintf("Warning: naive?\n");
		return;
	}

	if (entchannel == CHAN_STREAM && pitch != 100)
	{
		gEngfuncs.Con_DPrintf("Warning: pitch shift ignored on stream sound %s\n", sfx->name);
		pitch = 100;
	}
	
	vol = fvol * 255;

	if (vol > 255)
	{
		gEngfuncs.Con_DPrintf("S_StartStaticSound: %s volume > 255\n", sfx->name);
		vol = 255;
	}

	if (flags & (SND_STOP | SND_CHANGE_VOL | SND_CHANGE_PITCH))
	{
		if (S_AlterChannel(entnum, entchannel, sfx, vol, pitch, flags))
			return;
		if (flags & SND_STOP)
			return;
		// fall through - if we're not trying to stop the sound,
		// and we didn't find it (it's not playing), go ahead and start it up
	}

	if (pitch == 0)
	{
		gEngfuncs.Con_DPrintf("Warning: S_StartStaticSound Ignored, called with pitch 0\n");
		return;
	}

	// pick a channel to play on from the static area
	ch = SND_PickStaticChannel(entnum, entchannel, sfx);

	if (!ch)
		return;
	
	if (sfx->name[0] == '!' || sfx->name[0] == '#')
	{
		// this is a sentence. link words to play in sequence.

		// NOTE: sentence names stored in the cache lookup are
		// prepended with a '!'.  Sentence names stored in the
		// sentence file do not have a leading '!'. 
		
		// link all words and load the first word

		char name[MAX_QPATH];
		strncpy(name, sfx->name + 1, sizeof(name) - 1);
		name[sizeof(name) - 1] = 0;
		sc = g_pfn_VOX_LoadSound(ch, name);
		fvox = true;
	}
	else
	{
		// regular or streamed sound fx
		sc = S_LoadSound(sfx, ch);
		ch->sfx = sfx;
		ch->isentence = -1;
	}

	if (!sc)
	{
		ch->sfx = NULL;
		return;
	}

	ch->entnum = entnum;
	ch->entchannel = entchannel;
	VectorCopy(origin, ch->origin);
	ch->master_vol = vol;
	ch->pitch = pitch;

	if (qalIsSource(ch->al_source) == AL_FALSE)
	{
		qalGetError();	//clear
		qalGenSources(1, &ch->al_source);

		if (qalGetError() != AL_NO_ERROR)
		{
			memset(ch, 0, sizeof(*ch));
			ch->isentence = -1;
			gEngfuncs.Con_DPrintf("AL: qalGenSources got an error!\n");
			return;
		}

		int chidx = ch - channels;
//		gEngfuncs.Con_DPrintf("AL: Generated source object(%d) for static channel(%d)\n", ch->al_source, chidx);
	}

	SND_Spatialize(ch);

	if (qalIsSource(ch->al_source))
	{
		// setup distance model
		qalSourcef(ch->al_source, AL_REFERENCE_DISTANCE, 0.0f);
		qalSourcef(ch->al_source, AL_MAX_DISTANCE, 1024.0f);
		qalSourcef(ch->al_source, AL_ROLLOFF_FACTOR, attenuation);

		// attach wave data buffer
		qalSourcei(ch->al_source, AL_BUFFER, sc->al_buffer);
		ch->al_sample = sc->al_sample;

		// set volume and pitch
		qalSourcef(ch->al_source, AL_GAIN, (ALfloat)ch->master_vol / 255.0f);
		qalSourcef(ch->al_source, AL_PITCH, (ALfloat)ch->pitch / 100.0f);

		// start playback
		qalSourcePlay(ch->al_source);
	}

//	int chidx = ch - channels;
//	gEngfuncs.Con_DPrintf("S_StartStaticSound: Play sound(%s) entnum(%d) entchannel(%d) at channel(%d)\n",
//		sfx->name, entnum, entchannel, chidx);
}

void S_StopSound(int entnum, int entchannel)
{
	int	i;

	for (i = NUM_AMBIENTS; i < total_channels; i++)
	{
		if (channels[i].entnum == entnum
			&& channels[i].entchannel == entchannel)
		{
			S_FreeChannel(&channels[i]);
		}
	}
}

void S_StopAllSounds(qboolean clear)
{
	int	i;

	if (!sound_started)
		return;

	total_channels = NUM_AMBIENTS + MAX_DYNAMIC_CHANNELS;

	for (i = 0; i < MAX_CHANNELS; i++)
		if (channels[i].sfx)
			S_FreeChannel(&channels[i]);
}

void S_UpdateAmbientSounds(void)
{
}

void S_Update(vec3_t origin, vec3_t forward, vec3_t right, vec3_t up)
{
	int			i;
	vec3_t		position;
	float		orientation[6];
	channel_t	*ch;
	sfxcache_t	*sc;
	ALint		value;

	if (!sound_started)
		return;

	VectorCopy_HL2AL(origin, position);
	qalListenerfv(AL_POSITION, position);

	VectorCopy_HL2AL(forward, orientation);
	VectorCopy_HL2AL(up, orientation + 3);
	qalListenerfv(AL_ORIENTATION, orientation);

	if (volume)
	{
		if (volume->value < 0)
			volume->value = 0;
		if (volume->value > 2)
			volume->value = 2;

		// update volume
		qalListenerf(AL_GAIN, volume->value);
	}

	// update general area ambient sound sources
	S_UpdateAmbientSounds();

	// update spatialization for static and dynamic sounds
	ch = &channels[NUM_AMBIENTS];
	for (i = NUM_AMBIENTS; i < total_channels; i++, ch++)
	{
		if (!ch->sfx)
			continue;
		SND_Spatialize(ch);		// respatialize channel

		// if the channel finished playback, free it
		if (qalIsSource(ch->al_source))
		{
			qalGetSourcei(ch->al_source, AL_SOURCE_STATE, &value);
			if (value == AL_STOPPED)
			{
				sc = (sfxcache_t *)Cache_Check(&ch->sfx->cache);
				S_CheckWavEnd(ch, &sc, 0, 0);
			}
		}
	}

//	SX_RoomFX();
}