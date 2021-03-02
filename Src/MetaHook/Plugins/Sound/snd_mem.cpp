
#include <metahook.h>
#include "engfuncs.h"
#include "sound.h"
#include "filesystem.h"



#pragma pack(push, 4)

struct RIFF_HEADER_CHUNK
{
	unsigned long	nID;
	unsigned long	nSize;
	unsigned long	nFormat;
};

struct RIFF_CHUNK
{
	unsigned long	nID;
	unsigned long	nSize;
};

struct PCM_WAVE_FORMAT
{
	unsigned short	wFormatTag;
	unsigned short	nChannels;
	unsigned long	nSamplesPerSec;
	unsigned long	nAvgBytesPerSec;
	unsigned short	nBlockAlign;
	// PCM
	unsigned short	wBitsPerSample;
	// fuck
	unsigned long	unknow[4];
};

struct WAVE_CUE_POINT
{
	unsigned long	nID;
	unsigned long	dwPosition;
	unsigned long	fccChunk;		// "data"
	unsigned long	dwChunkStart;
	unsigned long	dwBlockStart;
	unsigned long	dwSampleOffset;
};

struct WAVE_CUE_CHUNK
{
	unsigned long	dwCuePoints;
	// with 1 point
	WAVE_CUE_POINT	point0;
};

#pragma pack(pop)

#ifndef WAVE_FORMAT_PCM
#define WAVE_FORMAT_PCM		1
#endif

typedef struct
{
	ALenum		format;
	ALubyte		*data;
	ALsizei		size;
	ALsizei		frequency;
	ALint		cuepoint;	// loop start position (bytes)
} waveinfo_t;

bool LoadWAVFile(const char *file_name, waveinfo_t *info)
{
	FileHandle_t		file;
	RIFF_HEADER_CHUNK	header_chunk;
	RIFF_CHUNK			chunk;
	PCM_WAVE_FORMAT		format;
	WAVE_CUE_CHUNK		cue_chunk;

	// initial
	{
		info->format = 0;
		info->data = NULL;
		info->size = 0;
		info->frequency = 0;
		info->cuepoint = -1;
	}

	bool result = false;

	file = g_pFileSystem->Open(file_name, "rb");

	if (!file)
		return false;

	if (g_pFileSystem->Read(&header_chunk, sizeof(header_chunk), file) != sizeof(header_chunk))
		goto err;

	if (memcmp(&header_chunk.nID, "RIFF", 4) != 0)
		goto err;

	if (memcmp(&header_chunk.nFormat, "WAVE", 4) != 0)
		goto err;

	while (g_pFileSystem->Read(&chunk, sizeof(chunk), file) == sizeof(chunk))
	{
		if (memcmp(&chunk.nID, "fmt ", 4) == 0)
		{
//			int i = sizeof(format);
			if (chunk.nSize < 16 || chunk.nSize > sizeof(format))
				goto err;

			if (g_pFileSystem->Read(&format, chunk.nSize, file) < chunk.nSize)
				goto err;

			if (format.wFormatTag != WAVE_FORMAT_PCM)
				goto err;

			if (format.nChannels == 1)
			{
				if (format.wBitsPerSample == 8)
					info->format = AL_FORMAT_MONO8;
				else if (format.wBitsPerSample == 16)
					info->format = AL_FORMAT_MONO16;
				else
					goto err;
			}
			else if (format.nChannels == 2)
			{
				if (format.wBitsPerSample == 8)
					info->format = AL_FORMAT_STEREO8;
				else if (format.wBitsPerSample == 16)
					info->format = AL_FORMAT_STEREO16;
				else
					goto err;
			}
			else
			{
				goto err;
			}

			info->frequency = format.nSamplesPerSec;
		}
		else if (memcmp(&chunk.nID, "data", 4) == 0)
		{
			info->data = new ALubyte[chunk.nSize];

			if (info->data == NULL)
				goto err;

			if (g_pFileSystem->Read(info->data, chunk.nSize, file) != chunk.nSize)
				goto err;

			info->size = chunk.nSize;
		}
		else if (memcmp(&chunk.nID, "cue ", 4) == 0)
		{
			if (chunk.nSize != sizeof(cue_chunk))
				goto skip;	// don't do fail

			if (g_pFileSystem->Read(&cue_chunk, sizeof(cue_chunk), file) != sizeof(cue_chunk))
				goto err;

			info->cuepoint = cue_chunk.point0.dwSampleOffset;
		}
		else
		{
skip:
			g_pFileSystem->Seek(file, chunk.nSize, FILESYSTEM_SEEK_CURRENT);
		}

		if (chunk.nSize & 1)
			g_pFileSystem->Seek(file, 1, FILESYSTEM_SEEK_CURRENT);
	}

	if (info->data == NULL)
		goto err;

	result = true;

err:
	if (file)
		g_pFileSystem->Close(file);

	return result;
}

void FreeWAVFile(waveinfo_t *info)
{
	if (info->data)
	{
		delete[] info->data;
		info->data = NULL;
	}
}


sfxcache_t *S_LoadStreamSound(sfx_t *s, channel_t *ch);


sfxcache_t *S_LoadSound(sfx_t *sfx, channel_t *ch)
{
	char	namebuffer[256];
	sfxcache_t	*sc;
	waveinfo_t	wav;
	ALuint	al_buffer;

	if (sfx->name[0] == '*')
	{
		//return S_LoadStreamSound(s, ch);
		return NULL;
	}

	if (sfx->name[0] == '?')
	{
		return NULL;
	}

	sc = (sfxcache_t *)Cache_Check(&sfx->cache);
	if (sc)
	{
		return sc;
	}

	_snprintf(namebuffer, sizeof(namebuffer), "sound/%s", sfx->name);

	// try to load file with "sound/" prefix
	if (LoadWAVFile(namebuffer, &wav) == false)
	{
		// try to load file without "sound/" prefix
		if (LoadWAVFile(sfx->name, &wav) == false)
		{
			gEngfuncs.Con_DPrintf("S_LoadSound: Couldn't load %s\n", sfx->name);
			return NULL;
		}
	}

	qalGetError();	//clear
	qalGenBuffers(1, &al_buffer);

	if (qalGetError() != AL_NO_ERROR)
	{
		FreeWAVFile(&wav);
		gEngfuncs.Con_DPrintf("AL: qalGenBuffers got an error !\n");
		return NULL;
	}

//	gEngfuncs.Con_DPrintf("AL: Generated buffer object(%d) for sound(%s)\n", al_buffer, sfx->name);
	
	if (qalIsBuffer(al_buffer))
	{
		qalBufferData(al_buffer, wav.format, wav.data, wav.size, wav.frequency);

		// wave data has been copy to OpenAL, so we can free the data
		FreeWAVFile(&wav);

		if (qalGetError() != AL_NO_ERROR)
		{
			gEngfuncs.Con_DPrintf("AL: qalBufferData got an error !\n");
			return NULL;
		}
	}
	else
	{
		FreeWAVFile(&wav);
		gEngfuncs.Con_DPrintf("AL: Bad buffer object\n");
		return NULL;
	}

	sc = (sfxcache_t *)Cache_Alloc(&sfx->cache, sizeof(sfxcache_t), sfx->name);
	if (!sc)
		return NULL;

	qalGetBufferi(al_buffer, AL_FREQUENCY, &sc->al_freq);
	qalGetBufferi(al_buffer, AL_SIZE, &sc->al_size);
	qalGetBufferi(al_buffer, AL_BITS, &sc->al_bits);
	qalGetBufferi(al_buffer, AL_CHANNELS, &sc->al_chan);

	// get samples count
	sc->al_sample = sc->al_size / (sc->al_bits / 8) / sc->al_chan;

	// get playback time
	sc->al_time = (ALfloat)sc->al_sample / (ALfloat)sc->al_freq;

	// for sound loop
	sc->al_loop = wav.cuepoint;

	sc->al_buffer = al_buffer;
	sc->al_format = wav.format;

	return sc;
}