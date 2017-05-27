/*

    Zelda Oni Link Begins

    Copyright (C) 2006-2008  Vincent Jouillat

    Please send bugreports with examples or suggestions to www.zeldaroth.fr

*/

#include <sstream>
#include <fstream>
#include <iostream>

//#include <SDL/SDL.h>
#include <vitasdk.h>
#include "Audio.h"

#include "vita/os_vita.h"
#include "vita/sound_vita.h"

Audio::Audio() : volume(0), musiqueId(0), specialId(0) {
    SOUND = FSOUND_Init(48000, 32, 0);
    music = NULL;
    
    if (SOUND) {
        previous_volume = -1;
        previous_volson = FSOUND_GetSFXMasterVolume();
        loadSounds();
    }
}

Audio::~Audio() {
    if (SOUND) {
        freeSounds();
        FMUSIC_StopSong(music);
        FMUSIC_SetMasterVolume(music, previous_volume);
        FMUSIC_FreeSong(music);
        FSOUND_SetSFXMasterVolume(previous_volson);
        FSOUND_Close();
    }
}

void Audio::setVolume(int vol) {
	volume=vol;
    FMUSIC_SetMasterVolume(music, volume);
}

void Audio::setVolson(int volson) {FSOUND_SetSFXMasterVolume(volson);
}

void Audio::loadSounds() {
    sons = new FSOUND_SAMPLE*[44];
    
    sons[0] = FSOUND_Sample_Load(FSOUND_FREE, "app0:/sound/text.raw",0,0,0); // lettres
    sons[1] = FSOUND_Sample_Load(FSOUND_FREE, "app0:/sound/menu1.raw",0,0,0); // menu 1
    sons[2] = FSOUND_Sample_Load(FSOUND_FREE, "app0:/sound/menu2.raw",0,0,0); // menu 2
    sons[3] = FSOUND_Sample_Load(FSOUND_FREE, "app0:/sound/menu3.raw",0,0,0); // menu 3
    sons[4] = FSOUND_Sample_Load(FSOUND_FREE, "app0:/sound/menu4.raw",0,0,0); // menu 4
    sons[5] = FSOUND_Sample_Load(FSOUND_FREE, "app0:/sound/timewarp.raw",0,0,0); // time retour
    sons[6] = FSOUND_Sample_Load(FSOUND_FREE, "app0:/sound/tombe.raw",0,0,0); // tombe (ennemi)
    sons[7] = FSOUND_Sample_Load(FSOUND_FREE, "app0:/sound/hitenemy.raw",0,0,0); //shot 1
    sons[8] = FSOUND_Sample_Load(FSOUND_FREE, "app0:/sound/killenemy.raw",0,0,0); // shot 2
    sons[9] = FSOUND_Sample_Load(FSOUND_FREE, "app0:/sound/surprise.raw",0,0,0); // surprise
    sons[10] = FSOUND_Sample_Load(FSOUND_FREE, "app0:/sound/monte.raw",0,0,0); // monte
    sons[11] = FSOUND_Sample_Load(FSOUND_FREE, "app0:/sound/descend.raw",0,0,0); // descend
    sons[12] = FSOUND_Sample_Load(FSOUND_FREE, "app0:/sound/chute.raw",0,0,0); // chute
    sons[13] = FSOUND_Sample_Load(FSOUND_FREE, "app0:/sound/item.raw",0,0,0); // item
    sons[14] = FSOUND_Sample_Load(FSOUND_FREE, "app0:/sound/rupee.raw",0,0,0); // rubis
    sons[15] = FSOUND_Sample_Load(FSOUND_FREE, "app0:/sound/heart.raw",0,0,0); // coeur
    sons[16] = FSOUND_Sample_Load(FSOUND_FREE, "app0:/sound/bomb.raw",0,0,0); // bombe
    sons[17] = FSOUND_Sample_Load(FSOUND_FREE, "app0:/sound/textnext.raw",0,0,0); // suite texte
    sons[18] = FSOUND_Sample_Load(FSOUND_FREE, "app0:/sound/textend.raw",0,0,0); // fin texte
    sons[19] = FSOUND_Sample_Load(FSOUND_FREE, "app0:/sound/happy.raw",0,0,0); // trouve objet
    sons[20] = FSOUND_Sample_Load(FSOUND_FREE, "app0:/sound/door.raw",0,0,0); // ouvre porte
    sons[21] = FSOUND_Sample_Load(FSOUND_FREE, "app0:/sound/pics.raw",0,0,0); // pics contre mur
    sons[22] = FSOUND_Sample_Load(FSOUND_FREE, "app0:/sound/sword.raw",0,0,0); // épée
    sons[23] = FSOUND_Sample_Load(FSOUND_FREE, "app0:/sound/SwordCharging.raw",0,0,0); // chargée
    sons[24] = FSOUND_Sample_Load(FSOUND_FREE, "app0:/sound/Sword360.raw",0,0,0); // spin
    sons[25] = FSOUND_Sample_Load(FSOUND_FREE, "app0:/sound/shoot.raw",0,0,0); // flèche
    sons[26] = FSOUND_Sample_Load(FSOUND_FREE, "app0:/sound/hookshot.raw",0,0,0); // grappin
    sons[27] = FSOUND_Sample_Load(FSOUND_FREE, "app0:/sound/stamp.raw",0,0,0); // pose bombe
    sons[28] = FSOUND_Sample_Load(FSOUND_FREE, "app0:/sound/magic.raw",0,0,0); // magie
    sons[29] = FSOUND_Sample_Load(FSOUND_FREE, "app0:/sound/burn.raw",0,0,0); // brûle
    sons[30] = FSOUND_Sample_Load(FSOUND_FREE, "app0:/sound/hammer.raw",0,0,0); // marteau
    sons[31] = FSOUND_Sample_Load(FSOUND_FREE, "app0:/sound/plouf.raw",0,0,0); // plouf
    sons[32] = FSOUND_Sample_Load(FSOUND_FREE, "app0:/sound/danger.raw",0,0,0); // danger
    sons[33] = FSOUND_Sample_Load(FSOUND_FREE, "app0:/sound/hurt.raw",0,0,0); // link se blesse
    sons[34] = FSOUND_Sample_Load(FSOUND_FREE, "app0:/sound/porte.raw",0,0,0); // porte objet
    sons[35] = FSOUND_Sample_Load(FSOUND_FREE, "app0:/sound/lance.raw",0,0,0); // lance objet
    sons[36] = FSOUND_Sample_Load(FSOUND_FREE, "app0:/sound/casse.raw",0,0,0); // casse objet
    sons[37] = FSOUND_Sample_Load(FSOUND_FREE, "app0:/sound/charge.raw",0,0,0); // charge magie
    sons[38] = FSOUND_Sample_Load(FSOUND_FREE, "app0:/sound/buisson.raw",0,0,0); // coupe buisson
    sons[39] = FSOUND_Sample_Load(FSOUND_FREE, "app0:/sound/pousse.raw",0,0,0); // pousse caisse
    sons[40] = FSOUND_Sample_Load(FSOUND_FREE, "app0:/sound/envol.raw",0,0,0); // chant envol
    sons[41] = FSOUND_Sample_Load(FSOUND_FREE, "app0:/sound/vide.raw",0,0,0); // hymne vide
    sons[42] = FSOUND_Sample_Load(FSOUND_FREE, "app0:/sound/masque.raw",0,0,0); // chant esprit
    sons[43] = FSOUND_Sample_Load(FSOUND_FREE, "app0:/sound/splash.raw",0,0,0); // ...
    
}

void Audio::freeSounds() {
    if (SOUND) {
        for (int i = 0; i < 44; i++) FSOUND_Sample_Free(sons[i]);
        delete[] sons;
    }
}

void Audio::playSound(int id, int chl) {
    if (SOUND) //Mix_PlayChannel(chl,sons[id],0);
        FSOUND_PlaySound(chl, sons[id]);
}

void Audio::playMusic(int id) {
    if (SOUND) {
        if (musiqueId != id) {
            musiqueId = id;            
            if (specialId == 0) {
                FMUSIC_StopSong(music);
                FMUSIC_FreeSong(music);
                music = choixMusique(id);
                if (previous_volume == -1) previous_volume = FMUSIC_GetMasterVolume(music);
                FMUSIC_SetMasterVolume(music, volume);
                FMUSIC_SetLooping(music, 1);
                FMUSIC_PlaySong(music);
                specialId = 0;
            }
		}
    }
}

bool Audio::isSpecial() {return (specialId>0);}

void Audio::stopMusic() {
    if (SOUND) FMUSIC_StopSong(music);
}

void Audio::replayMusic() {
    if (SOUND) FMUSIC_PlaySong(music);
}

FMUSIC_MODULE* Audio::choixMusique(int id) {
    switch (id) {
        case 1 : return FMUSIC_LoadSong("app0:/music/Foret.raw");
        case 2 : return FMUSIC_LoadSong("app0:/music/Plaine.raw");
        case 3 : return FMUSIC_LoadSong("app0:/music/Villageest.raw");
        case 4 : return FMUSIC_LoadSong("app0:/music/Vallee.raw");
        case 5 : return FMUSIC_LoadSong("app0:/music/Desert.raw");
        case 6 : return FMUSIC_LoadSong("app0:/music/Lac.raw");
        case 7 : return FMUSIC_LoadSong("app0:/music/Rauru.raw");
        case 8 : return FMUSIC_LoadSong("app0:/music/Chateau.raw");
        case 9 : return FMUSIC_LoadSong("app0:/music/PlaineO.raw");
        case 10 : return FMUSIC_LoadSong("app0:/music/Marais.raw");
        case 11 : return FMUSIC_LoadSong("app0:/music/Ombre.raw");
        case 12 : return FMUSIC_LoadSong("app0:/music/Secret.raw");
        case 13 : return FMUSIC_LoadSong("app0:/music/Mido.raw");
        case 14 : return FMUSIC_LoadSong("app0:/music/Mont.raw");
        case 15 : case 17 : return FMUSIC_LoadSong("app0:/music/Donjon1.raw");
        case 18 : return FMUSIC_LoadSong("app0:/music/Manoir.raw");
        case 19 : return FMUSIC_LoadSong("app0:/music/Ordinn.raw");
        case 20 : return FMUSIC_LoadSong("app0:/music/Colisee.raw");
        case 21 : return FMUSIC_LoadSong("app0:/music/Lanelle.raw");
        case 22 : return FMUSIC_LoadSong("app0:/music/DFinal.raw");
        case 23 : return FMUSIC_LoadSong("app0:/music/DDesert.raw");
        case 24 : return FMUSIC_LoadSong("app0:/music/Glace.raw");
        case 25 : return FMUSIC_LoadSong("app0:/music/Farore.raw");
        case 26 : return FMUSIC_LoadSong("app0:/music/Xanto.raw");
        case 27 : return FMUSIC_LoadSong("app0:/music/DSecret.raw");
        case 28 : return FMUSIC_LoadSong("app0:/music/TourD.raw");
        case 16 : case 29 : case 31 : case 32 : case 33 : case 34 : case 35 :
        case 36 : case 37 : case 38 : case 40 : case 41 : case 42 : case 43 :
            return FMUSIC_LoadSong("app0:/music/Cave.raw");
        case 30 : case 39 : return FMUSIC_LoadSong("app0:/music/Fee.raw");
        case 44 : return FMUSIC_LoadSong("app0:/music/Pit.raw");
        case 45 : case 46 : case 54 : case 57 : case 58 : case 63 : case 64 :
        case 66 : case 67 : case 71 : return FMUSIC_LoadSong("app0:/music/Maison.raw");
        case 52 : case 56 : case 61 : case 70 :
            return FMUSIC_LoadSong("app0:/music/Potion.raw");
        case 48 : case 53 : return FMUSIC_LoadSong("app0:/music/Bar.raw");
        case 49 : case 62 : case 68 : case 79 : return FMUSIC_LoadSong("app0:/music/Maire.raw");
        case 50 : case 51 : case 59 : case 60 : case 65 : case 69 :
            return FMUSIC_LoadSong("app0:/music/Magasin.raw");
        case 47 : return FMUSIC_LoadSong("app0:/music/Jeu.raw");
        case 55 : return FMUSIC_LoadSong("app0:/music/Triste.raw");
        case 80 : return FMUSIC_LoadSong("app0:/music/Titre.raw");
        case 90 : return FMUSIC_LoadSong("app0:/music/Selection.raw");
        case 100 : return FMUSIC_LoadSong("app0:/music/Debut.raw");
        default : return FMUSIC_LoadSong("app0:/music/Maison.raw");
    }
}

void Audio::playSpecial(int id) {
    if (SOUND) {
        if (specialId != id) {
            FMUSIC_StopSong(music);
            FMUSIC_FreeSong(music);
            music = choixSpecial(id);
            if (previous_volume == -1) previous_volume = FMUSIC_GetMasterVolume(music);
            FMUSIC_SetMasterVolume(music, volume);
            FMUSIC_SetLooping(music, 1);
            FMUSIC_PlaySong(music);
            specialId=id;
        }    
    }
}

void Audio::stopSpecial() {
    if (!specialId) return;
    int tmp = musiqueId;
    musiqueId = 0;
    specialId = 0;
    playMusic(tmp);
}

FMUSIC_MODULE* Audio::choixSpecial(int id) {
    switch (id) {
        case 1 : return FMUSIC_LoadSong("app0:/music/Boss.raw");
        case 2 : return FMUSIC_LoadSong("app0:/music/Mort.raw");
        case 3 : return FMUSIC_LoadSong("app0:/music/Epee.raw");
        case 4 : return FMUSIC_LoadSong("app0:/music/BossF.raw");
        case 5 : return FMUSIC_LoadSong("app0:/music/Fin.raw");
        case 6 : return FMUSIC_LoadSong("app0:/music/BossM.raw");
        case 7 : return FMUSIC_LoadSong("app0:/music/Area81.raw");
        case 8 : return FMUSIC_LoadSong("app0:/music/OniLink.raw");
        default : return FMUSIC_LoadSong("app0:/music/Boss.raw");
    }
}
