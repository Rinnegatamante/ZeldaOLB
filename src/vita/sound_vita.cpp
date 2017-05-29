
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vitasdk.h>
#include "sound_vita.h"
#include "audio_decoder.h"

#define BUFSIZE 8192     // Max dimension of audio buffer size
#define NSAMPLES 2048    // Number of samples for output
#define SOUND_CHANNELS 7 // PSVITA has 8 available audio channels, one is reserved for musics

SceUID AudioThreads[SOUND_CHANNELS + 1], Sound_Mutex, NewSound_Mutex, Music_Mutex, NewMusic_Mutex;
DecodedMusic* new_sound = NULL;
DecodedMusic* new_music = NULL;
bool availThreads[SOUND_CHANNELS];
std::unique_ptr<AudioDecoder> audio_decoder[SOUND_CHANNELS + 1];
volatile bool mustExit = false;
uint8_t ids[] = {0, 1, 2, 3, 4, 5, 6, 7};
bool soundEnabled;

FSOUND_SAMPLE SFX[NUMSFX];

int SFXMasterVolume = 64;
int MasterVolume = 64;
int frequency=0;
int channel;

// Audio thread code
static int audioThread(unsigned int args, void* arg){

	// Getting thread id
	uint8_t* argv = (uint8_t*)arg;
	uint8_t id = argv[0];
	
	// Initializing audio port
	int ch = sceAudioOutOpenPort(SCE_AUDIO_OUT_PORT_TYPE_MAIN, NSAMPLES, 48000, SCE_AUDIO_OUT_MODE_STEREO);
	sceAudioOutSetConfig(ch, -1, -1, -1);
	
	DecodedMusic* mus;
	int old_volume;
	for (;;){
		
		// Waiting for an audio output request
		sceKernelWaitSema((id == 7) ? Music_Mutex : Sound_Mutex, 1, NULL);
		
		// Setting thread as busy
		if (id < 7) availThreads[id] = false;
		
		// Fetching track
		mus = ((id == 7) ? new_music : new_sound);
		mus->audioThread = id;
		sceKernelSignalSema(((id == 7) ? NewMusic_Mutex : NewSound_Mutex), 1);
		
		// Checking if a new track is available
		if (mus == NULL){
			
			//If we enter here, we probably are in the exiting procedure
			if (mustExit){
				if (id < 7) sceKernelSignalSema(Sound_Mutex, 1);
				sceAudioOutReleasePort(ch);
				sceKernelExitDeleteThread(0);
			}
		
		}
		
		// Setting audio channel volume
		int vol_stereo[] = {32767, 32767};
		sceAudioOutSetVolume(ch, SCE_AUDIO_VOLUME_FLAG_L_CH | SCE_AUDIO_VOLUME_FLAG_R_CH, vol_stereo);
		old_volume = 32767;
		
		// Initializing audio decoder
		audio_decoder[id] = AudioDecoder::Create(mus->handle, "Track");
		if (audio_decoder[id] == NULL) continue; // TODO: Find why this case apparently can happen
		audio_decoder[id]->Open(mus->handle);
		audio_decoder[id]->SetLooping(mus->loop);
		audio_decoder[id]->SetFormat(48000, AudioDecoder::Format::S16, 2);
		
		// Initializing audio buffers
		memset(mus->audiobuf, 0, BUFSIZE);
		memset(mus->audiobuf2, 0, BUFSIZE);
		mus->cur_audiobuf = mus->audiobuf;
		
		// Audio playback loop
		for (;;){
		
			// Check if the music must be paused
			if (mus->pauseTrigger || mustExit){
			
				// Check if the music must be closed
				if (mus->closeTrigger){
					audio_decoder[id].reset();
					mus->isPlaying = false;
					mus->pauseTrigger = false;
					mus->closeTrigger = false;
					mus = NULL;
					if (id < 7) availThreads[id] = true;
					if (!mustExit) break;
				}
				
				// Check if the thread must be closed
				if (mustExit){
				
					// Check if the audio stream has already been closed
					if (mus != NULL){
						mus->closeTrigger = true;
						continue;
					}
					
					// Recursively closing all the threads
					if (id < 7) sceKernelSignalSema(Sound_Mutex, 1);
					sceAudioOutReleasePort(ch);
					sceKernelExitDeleteThread(0);
					
				}
			
				mus->isPlaying = !mus->isPlaying;
				mus->pauseTrigger = false;
			}
			
			// Check if a volume change request arrived
			if (mus->volume != old_volume){
				int vol_stereo_new[] = {mus->volume, mus->volume};
				sceAudioOutSetVolume(ch, SCE_AUDIO_VOLUME_FLAG_L_CH | SCE_AUDIO_VOLUME_FLAG_R_CH, vol_stereo_new);
				old_volume = mus->volume;
			}
			
			if (mus->isPlaying){
				
				// Check if audio playback finished
				if ((!mus->loop) && audio_decoder[id]->IsFinished()) mus->isPlaying = false;
				
				// Update audio output
				if (mus->cur_audiobuf == mus->audiobuf) mus->cur_audiobuf = mus->audiobuf2;
				else mus->cur_audiobuf = mus->audiobuf;
				audio_decoder[id]->Decode(mus->cur_audiobuf, BUFSIZE);	
				sceAudioOutOutput(ch, mus->cur_audiobuf);
				
			}else{
				
				// Check if we finished a non-looping audio playback
				if ((!mus->loop) && audio_decoder[id]->IsFinished()){
					
					// Releasing the audio file
					audio_decoder[id].reset();
					mus->isPlaying = false;
					if (mus->tempBlock){
						mus = NULL;
					}else{
						mus->audioThread = 0xFF;
					}
					if (id < 7) availThreads[id] = true;
					break;
					
				}else sceKernelDelayThread(1000); // Tricky way to call a re-scheduling
				
			}
			
		}
		
	}
	
}

void soundInit()
{
	int i;
	for(i=0;i<NUMSFX;i++)
	{
		SFX[i].used=false;
		SFX[i].audiobuf = (uint8_t*)malloc(BUFSIZE);
		SFX[i].audiobuf2 = (uint8_t*)malloc(BUFSIZE);
		
	}
	
	// Creating audio mutexs
	Sound_Mutex = sceKernelCreateSema("Sound Mutex", 0, 0, 1, NULL);
	NewSound_Mutex = sceKernelCreateSema("NewSound Mutex", 0, 1, 1, NULL);
	Music_Mutex = sceKernelCreateSema("Music Mutex", 0, 0, 1, NULL);
	NewMusic_Mutex = sceKernelCreateSema("NewMusic Mutex", 0, 1, 1, NULL);

	// Starting audio threads
	for (int i=0;i < (SOUND_CHANNELS + 1); i++){
		if (i < SOUND_CHANNELS) availThreads[i] = true;
		AudioThreads[i] = sceKernelCreateThread("Audio Thread", &audioThread, 0x10000100, 0x10000, 0, 0, NULL);
		sceKernelStartThread(AudioThreads[i], sizeof(ids[i]), &ids[i]);
	}
	
	soundEnabled=true;
}

void soundClose()
{

	// Starting exit procedure for audio threads
	mustExit = true;
	sceKernelSignalSema(Sound_Mutex, 1);
	for (int i=0;i<SOUND_CHANNELS;i++){
		sceKernelWaitThreadEnd(AudioThreads[i], NULL, NULL);
	}
	sceKernelSignalSema(Music_Mutex, 1);
	sceKernelWaitThreadEnd(AudioThreads[SOUND_CHANNELS], NULL, NULL);
	mustExit = false;
		
	// Deleting audio mutex
	sceKernelDeleteSema(Sound_Mutex);
	sceKernelDeleteSema(NewSound_Mutex);
	sceKernelDeleteSema(Music_Mutex);
	sceKernelDeleteSema(NewMusic_Mutex);
	
	int i;
	for(i=0;i<NUMSFX;i++)
	{
		if(SFX[i].used)
		{
			SFX[i].used=false;
			free(SFX[i].audiobuf);
			free(SFX[i].audiobuf2);
		}
	}

}

int FSOUND_Init(uint32_t freq, uint32_t bps, uint32_t unkn)
{

	frequency = freq;
	
	return soundEnabled;
}

void initSFX(FMUSIC_MODULE* s)
{
	if(!s)return;
	
	s->used=true;
	s->loop=false;
}

void loadSFX(FMUSIC_MODULE* s, const char* filename, uint32_t format)
{
	if(!s)return;

	initSFX(s);
	
	sprintf(s->filepath, "%s", filename);

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
	if(!s || !s->used || !soundEnabled || s->isPlaying || SFXMasterVolume == 0)return;
	
	// Wait till a thread is available
	bool found = false;
	for (int i=0; i<SOUND_CHANNELS; i++){
		found = availThreads[i];
		if (found) break;
	}
	if (!found) return;
	
	FILE* f = fopen(s->filepath, "rb");
	s->handle = f;
	if (s->handle == NULL) return;
	s->isPlaying = true;
	s->audioThread = 0xFF;
	s->tempBlock = false;
	s->pauseTrigger = false;
	s->closeTrigger = false;
	s->volume = (32767*SFXMasterVolume)/64;
	
	// Waiting till track slot is free
	sceKernelWaitSema(NewSound_Mutex, 1, NULL);
	
	// Passing music to the audio thread
	new_sound = s;
	sceKernelSignalSema(Sound_Mutex, 1);

}

void FMUSIC_StopSong(FMUSIC_MODULE* s)
{
	if (s && s->isPlaying){
		s->closeTrigger = true;
		s->pauseTrigger = true;
	}
}

void FMUSIC_PlaySong(FMUSIC_MODULE* s)
{

	if(!s || !s->used || s->isPlaying || !soundEnabled || MasterVolume == 0)return;
	
	FILE* f = fopen(s->filepath, "rb");
	s->handle = f;
	if (s->handle == NULL) return;
	s->isPlaying = true;
	s->audioThread = 0xFF;
	s->tempBlock = false;
	s->pauseTrigger = false;
	s->closeTrigger = false;
	s->volume = (32767*MasterVolume)/64;
	
	// Waiting till track slot is free
	sceKernelWaitSema(NewMusic_Mutex, 1, NULL);
	
	// Passing music to the audio thread
	new_music = s;
	sceKernelSignalSema(Music_Mutex, 1);
}


FSOUND_SAMPLE* FSOUND_Sample_Load(int flag, const char * f,int a, int b, int c)
{
	int i;
	for(i=0;i<NUMSFX;i++)
	{
		if(!SFX[i].used)
		{
			loadSFX(&SFX[i], f, 0);

			SFX[i].used = true;
			SFX[i].loop = false;
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
			loadSFX(&SFX[i], f, 0);
			
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
		s->used=false;
		s->loop=false;
	}
}


void FMUSIC_FreeSong(FMUSIC_MODULE* s)
{
	if(s) {
		s->used=false;
		s->loop=false;
	}
}