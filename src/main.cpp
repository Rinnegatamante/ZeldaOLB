/*

    Zelda Oni Link Begins

    Copyright (C) 2006-2008  Vincent Jouillat

    Please send bugreports with examples or suggestions to www.zeldaroth.fr

*/

#include "vita/os_vita.h"

#include <iostream>
#include <cstdlib>
#include <ctime>
#include "Keyboard.h"
#include "Generique.h"

SDL_Surface* init() {             // initialise SDL
    if(SDL_Init(SDL_INIT_VIDEO) == -1) {
        exit(-1);
    }
    atexit(SDL_Quit);

    SDL_ShowCursor(SDL_DISABLE);
	 
	return SDL_SetVideoMode(640, 480, 32, SDL_HWSURFACE|SDL_DOUBLEBUF|SDL_FULLSCREEN);
}

int main(int argc, char** argv) {
    if (argc && argv); //pour éviter un warning.....
    
    std::srand(std::time(NULL));
    
    SDL_Surface* gpScreen = NULL;
    
    int mode = 2; //mode=0;
    
    gpScreen = init();
	
    Audio* gpAudio = new Audio();
    Jeu* gpJeu = new Jeu(gpAudio);
    Carte* gpCarte = new Carte(gpJeu);
    Encyclopedie* gpEncyclopedie = new Encyclopedie(gpJeu);
    Keyboard* gpKeyboard = new Keyboard(gpJeu, gpCarte, gpEncyclopedie, gpScreen, mode);
    gpJeu->setKeyboard(gpKeyboard);
    Generique* gpGenerique = new Generique(gpJeu);
    gpJeu->setGenerique(gpGenerique);
    gpGenerique->initLogo();
    
    //gpJeu->init(0);
    
    bool gLoop = true;
    
    Uint32 lastAnimTime = SDL_GetTicks();
    
    while (gLoop) {
        
        if (gpKeyboard->gererClavier() == -1) {gLoop = false;}
        
        switch (gpKeyboard->getMode()) {
            case 0 : //jeu normal
				gpJeu->draw(gpScreen); break;
            case 1 : //disclamer
            case 2 : //logo
            case 3 : //titre
            case 14 : //générique score
            case 17 : //menu d'aide 1
            case 18 : //menu d'aide 2
				gpGenerique->draw(gpScreen); break;
            case 4 : //selection
                gpGenerique->drawSelection(gpScreen, gpKeyboard->getLigne(), 
                    gpKeyboard->getColonne()); break;
            case 6 : //options
                gpGenerique->drawOption(gpScreen, gpKeyboard->getLigneOption(),
                gpKeyboard->getVolume()/4, gpKeyboard->getVolson()/4); break;
            case 7 : //charger partie
                gpGenerique->drawCharger(gpScreen, gpKeyboard->getLigne(), 
                    gpKeyboard->getLigneVal()); break;
            case 8 : //générique intro
                gpGenerique->drawIntro(gpScreen, gpKeyboard->getIntro()); break;
            case 9 : //effacer partie
                gpGenerique->drawEffacerSave(gpScreen, gpKeyboard->getLigne(), 
                    gpKeyboard->getLigneVal()); break;
            case 10 : //générique début chez link
                gpGenerique->drawDebut(gpScreen); break;
            case 11 : //générique fin
                gpGenerique->drawFin(gpScreen); break;
            case 12 : //carte
            case 22 : //carte téléportation
                gpCarte->draw(gpScreen); break;
            case 13 : //encyclopédie des monstres
                gpEncyclopedie->draw(gpScreen); break;
            case 15 : //records
            case 19 : //rang 100%
            case 20 : //rang ultime
            case 21 : //rang de rapidité
                gpGenerique->drawRecord(gpScreen, gpKeyboard->getLigneRecord(),
                    gpKeyboard->getColonneRecord()); break;
            case 16 : //effacer record
                gpGenerique->drawEffacer(gpScreen, gpKeyboard->getLigneVal()); break;
            default : break;
        }

        SDL_Flip(gpScreen);
        
        if (SDL_GetTicks() < lastAnimTime + 20) SDL_Delay(lastAnimTime+20-SDL_GetTicks());
        lastAnimTime = SDL_GetTicks();
    
    }
    
    SDL_FreeSurface(gpScreen); //does nothing
    
    delete gpEncyclopedie;
    delete gpGenerique;
    delete gpKeyboard;
    delete gpCarte;
    delete gpJeu;
    delete gpAudio;
    
    SDL_ShowCursor(SDL_ENABLE);
    SDL_Quit();
    exit(0);
    
    return 0;
}
