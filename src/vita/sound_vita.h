
#ifndef __SOUND_VITA_H__
#define __SOUND_VITA_H__

#define FSOUND_FREE 1
#define NUMSFX (50)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Music block struct
typedef struct DecodedMusic
{
	uint8_t* audiobuf;
	uint8_t* audiobuf2;
	uint8_t* cur_audiobuf;
	FILE* handle;
	volatile bool isPlaying;
	bool loop;
	volatile bool pauseTrigger;
	volatile bool closeTrigger;
	volatile uint8_t audioThread;
	volatile int volume;
	char filepath[256];
	bool tempBlock;
	bool used;
} DecodedMusic;

typedef DecodedMusic FSOUND_SAMPLE;
typedef DecodedMusic FMUSIC_MODULE;

void soundInit();
void soundClose();

int FSOUND_Init(uint32_t freq, uint32_t bpd, uint32_t unkn);

int FSOUND_GetSFXMasterVolume(); //
int FMUSIC_GetMasterVolume(FMUSIC_MODULE* s); 


void FMUSIC_SetMasterVolume(FMUSIC_MODULE* s, uint8_t volume);
void FMUSIC_FreeSong(FMUSIC_MODULE* s);
void FSOUND_SetSFXMasterVolume(uint8_t volson);

void initSFX(FSOUND_SAMPLE* s);
void loadSFX(FSOUND_SAMPLE* s, const char* filename, uint32_t format);

void FSOUND_PlaySound(int chl,FSOUND_SAMPLE* s);
void FMUSIC_StopSong(FMUSIC_MODULE* s);
void FMUSIC_SetLooping(FMUSIC_MODULE* s, bool flag);
void FMUSIC_PlaySong(FMUSIC_MODULE* s);

FSOUND_SAMPLE* FSOUND_Sample_Load(int flag, const char * f,int a, int b, int c);
FMUSIC_MODULE* FMUSIC_LoadSong(const char * f);

void FSOUND_Sample_Free(FSOUND_SAMPLE* s);

void FSOUND_Close();


#endif