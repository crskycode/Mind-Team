
#ifndef SOUND_H
#define SOUND_H

#define MAX_QPATH	64	// Must match value in quakedefs.h

// bspfile.h
#define NUM_AMBIENTS	4	// automatic ambient sounds

// a sound with no channel is a local only sound
#define SND_STOP			(1<<5)
#define SND_CHANGE_VOL		(1<<6)
#define SND_CHANGE_PITCH	(1<<7)


#include "zone.h"
#include "qal.h"


#define MAX_SFX	1024

typedef struct sfx_s
{
	char			name[MAX_QPATH];
	cache_user_t	cache;
	int				servercount;
} sfx_t;

typedef struct sfxcache_s
{
	int		length;			// sample count
	int		loopstart;		// loop start position
	int		speed;			// samples per second
	int		width;			// bits per sample
	int		stereo;			// is stereo ?
	byte	data[1];
	// al defines
	ALuint	al_buffer;		// openal buffer object
	ALenum	al_format;		// sample data format
	ALint	al_sample;		// samples count
	ALint	al_freq;		// specified in samples per second, units of Hertz [Hz]
	ALint	al_size;		// bytes of the buffer data
	ALint	al_bits;		// number of bits per sample
	ALint	al_chan;		// number of channels
	ALfloat	al_time;		// time of samples, units of second
	ALint	al_loop;		// loop start position in bytes
} sfxcache_t;

typedef struct channel_s
{
	sfx_t	*sfx;			// wave info and data
	int		leftvol;		// <unused>
	int		rightvol;		// <unused>
	int		end;			// <unused>
	int		pos;			// <unused>
	int		looping;		// <unused>
	int		entnum;			// entity index, make source position follow to it
	int		entchannel;		// channel index
	vec3_t	origin;			// static source position
	vec_t	dist_mult;		// <unused>
	int		master_vol;		// volume of this source (0~255)
	int		isentence;		// sentence index
	int		iword;			// current playing word in sentence
	int		pitch;			// frequency shift by pitch (0~100)
	// al defines
	ALuint	al_source;		// openal source object
	ALint	al_sample;		// total samples of current buffer object
} channel_t;

#define MAX_CHANNELS	128
#define MAX_DYNAMIC_CHANNELS	8

extern channel_t channels[MAX_CHANNELS];
extern int total_channels;

typedef struct voxword_s
{
	int		volume;						// increase percent, ie: 125 = 125% increase
	int		pitch;						// pitch shift up percent
	int		start;						// offset start of wave percent
	int		end;						// offset end of wave percent
	int		cbtrim;						// end of wave after being trimmed to 'end'
	int		fKeepCached;				// 1 if this word was already in cache before sentence referenced it
	int		samplefrac;					// if pitch shifting, this is position into wav * 256
	int		timecompress;				// % of wave to skip during playback (causes no pitch shift)
	sfx_t	*sfx;						// name and cache pointer
} voxword_t;

#define CVOXWORDMAX	32
#define CVOXSENTENCEMAX	16
extern voxword_t (*rgrgvoxword)[CVOXWORDMAX];

#define CVOXFILESENTENCEMAX		1536
//extern char *rgpszrawsentence[CVOXFILESENTENCEMAX];
//extern int cszrawsentences;


void S_Startup(void);
void S_Init(void);
void S_Shutdown(void);
sfx_t *S_FindName(char *name, qboolean *pfInCache);
void S_StopAllSounds(qboolean clear);

void S_FreeChannel(channel_t *ch);
qboolean S_CheckWavEnd(channel_t *ch, sfxcache_t **psc, int ltime, int ichan);

void VOX_MakeSingleWordSentence(channel_t *ch, int pitch);
sfxcache_t *S_LoadSound(sfx_t *s, channel_t *ch);

void SX_Init(void);
void SX_RoomFX(void);


void VoiceSE_NotifyFreeChannel(int iChannel);


extern vec3_t vec3_origin;

#endif