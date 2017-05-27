/*

    Zelda Oni Link Begins

    Copyright (C) 2006-2008  Vincent Jouillat

    Please send bugreports with examples or suggestions to www.zeldaroth.fr

*/

#ifndef __ENCYCLOPEDIE_H__
#define __ENCYCLOPEDIE_H__

#include "vita/os_vita.h"
#include "Jeu.h"

class Encyclopedie {
    public :
        Encyclopedie(Jeu* jeu);
        ~Encyclopedie();
        void draw(SDL_Surface* gpScreen);
        void init();
        void moveL();
        void moveR();
        void setPage(int i);
    private :
        void fond();
        void placeCadres();
        void cadre(int x, int y, int w, int h, int id);
        void verif(int sens);
        bool isVide();
        
        Jeu* gpJeu;
        int page;
        Ennemi* gpEnnemi;
        SDL_Surface* image;
        SDL_Surface* imageCadre;
};

#endif  // Encyclopedie.h
