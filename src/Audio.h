/*

    Zelda Oni Link Begins

    Copyright (C) 2006-2008  Vincent Jouillat

    Please send bugreports with examples or suggestions to www.zeldaroth.fr

*/

#ifndef __AUDIO_H__
#define __AUDIO_H__

#include <vitasdk.h>
#include "vita/os_vita.h"
#include "vita/sound_vita.h"

class Audio {
    public :
        Audio();
        ~Audio();
        void playSound(int id, int chl = FSOUND_FREE);
        void playMusic(int id);
        void stopMusic();
        void replayMusic();
        void playSpecial(int id);
        void stopSpecial();
        void setVolume(int vol);
        void setVolson(int volson);
        bool isSpecial();
    private :
        void loadSounds();
        void freeSounds();
        FMUSIC_MODULE* choixMusique(int id);
        FMUSIC_MODULE* choixSpecial(int id);
        
        bool SOUND;
        int previous_volume;
        int previous_volson;
        int volume;
        int musiqueId;
        int specialId;
        FSOUND_SAMPLE** sons;
        FMUSIC_MODULE* music;

};

#endif  // Audio.h
