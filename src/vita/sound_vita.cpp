
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vitasdk.h>
#include "sound_vita.h"

bool soundEnabled;

FSOUND_SAMPLE SFX[NUMSFX];

int SFXMasterVolume = 64;
int MasterVolume = 64;
int frequency=0;
int channel;

void soundInit()
{
	int i;
	for(i=0;i<NUMSFX;i++)
	{
		SFX[i].used=false;
	}

	soundEnabled=false;
}

void soundClose()
{
	int i;
	for(i=0;i<NUMSFX;i++)
	{
		if(SFX[i].used)
		{
			if(SFX[i].data)
			{
				free(SFX[i].data);
				SFX[i].data=NULL;
			}
			SFX[i].used=false;
		}
	}

}

FILE* openFile(const char* fn, const char* mode)
{
	if(!fn || !mode)return NULL;
	return fopen(fn, mode);
}

void* bufferizeFile(const char* filename, uint32_t* size, bool binary, bool linear)
{
	FILE* file;
	
	if(!binary)file = openFile(filename, "r");
	else file = openFile(filename, "rb");
	
	if(!file) return NULL;
	
	uint8_t* buffer;
	long lsize;
	fseek (file, 0 , SEEK_END);
	lsize = ftell (file);
	rewind (file);
	buffer=(uint8_t*)malloc(lsize);
	if(size)*size=lsize;
	
	if(!buffer)
	{
		fclose(file);
		return NULL;
	}
		
	fread(buffer, 1, lsize, file);
	fclose(file);
	return buffer;
}


int FSOUND_Init(uint32_t freq, uint32_t bps, uint32_t unkn)
{

	frequency = freq;
	
	return soundEnabled;
}

void initSFX(FMUSIC_MODULE* s)
{
	if(!s)return;

	s->data=NULL;
	s->size=0;
	s->used=true;
	s->loop=false;
}

void loadSFX(FMUSIC_MODULE* s, const char* filename, uint32_t format)
{
	if(!s)return;

	initSFX(s);

	s->data=(uint8_t*) bufferizeFile(filename, &s->size, true, true);
	s->format=format;
}

int FSOUND_GetSFXMasterVolume()
{
	return SFXMasterVolume;
}

int FMUSIC_GetMasterVolume(FMUSIC_MODULE* s)
{
	return MasterVolume;
}

void FMUSIC_SetMasterVolume(FMUSIC_MODULE* s, uint8_t volume)
{
	MasterVolume = volume;
}

void FSOUND_SetSFXMasterVolume(uint8_t volson)
{
	SFXMasterVolume = volson;
}

void FSOUND_PlaySound(int chl,FSOUND_SAMPLE* s)
{
	if(!s || !s->used || !s->data || !soundEnabled || SFXMasterVolume == 0)return;

	channel++;
	channel%=7;

	//csndPlaySound(channel+8, s->format, frequency, 1.0, 0.0, (uint32_t*)s->data, (uint32_t*)s->data, s->size);
}

void FMUSIC_StopSong(FMUSIC_MODULE* s)
{
	//CSND_SetPlayState(15, 0);//Stop music audio playback.
	//csndExecCmds(0);
}

void FMUSIC_PlaySong(FMUSIC_MODULE* s)
{
	int flag;
	if(!s || !s->used || !s->data || !soundEnabled || MasterVolume == 0)return;
	flag = s->format;
	//if(s->loop) flag |= SOUND_REPEAT;
	//csndPlaySound(15, flag, 8000, 1.0, 0.0, (uint32_t*)s->data, (uint32_t*)s->data, s->size);
}


FSOUND_SAMPLE* FSOUND_Sample_Load(int flag, const char * f,int a, int b, int c)
{
	int i;
	for(i=0;i<NUMSFX;i++)
	{
		if(!SFX[i].used)
		{
			//loadSFX(&SFX[i], f, SOUND_FORMAT_16BIT);

			if(!SFX[i].data)return NULL;
			SFX[i].used = true;
			SFX[i].loop=false;
			return &SFX[i];
		}
	}
	return NULL;
}

FMUSIC_MODULE* FMUSIC_LoadSong(const char * f)
{
	int size;
	int i;
	for(i=0;i<NUMSFX;i++)
	{
		if(!SFX[i].used)
		{
			//loadSFX(&SFX[i], f, SOUND_FORMAT_16BIT);
			
			if(!SFX[i].data) return NULL;
			SFX[i].used = true;
			SFX[i].loop=false;
			return &SFX[i];
		}
	}
	return NULL;
}

void FSOUND_Close()
{
	return;
}


void FMUSIC_SetLooping(FMUSIC_MODULE* s, bool flag)
{
	if (s)
		s->loop=flag;
}

void FSOUND_Sample_Free(FSOUND_SAMPLE* s)
{
	if(s) {
		if (s->data)
			free(s->data);
		s->size=0;
		s->used=false;
		s->loop=false;
	}
}


void FMUSIC_FreeSong(FMUSIC_MODULE* s)
{
	if(s) {
		if (s->data)
			free(s->data);
		s->size=0;
		s->used=false;
		s->loop=false;
	}
}


