/*

    Zelda Oni Link Begins

    Copyright (C) 2006-2008  Vincent Jouillat

    Please send bugreports with examples or suggestions to www.zeldaroth.fr

*/

#ifndef __STATUT_H__
#define __STATUT_H__

#include "vita/gfx_vita.h"

class Jeu;

class Statut {
    public :
        Statut(Jeu* jeu);
        ~Statut();
        void draw(SDL_Surface* gpScreen);
        bool getJauge();
        void setJauge(bool b);
        void setValJauge(int i);
        void setMaxJauge(int i);
        void setValJauge2(int i);
        void setMaxJauge2(int i);
    private :
        void drawBoss(SDL_Surface* gpScreen);
        void drawVie(SDL_Surface* gpScreen);
        void drawItems(SDL_Surface* gpScreen);
        void drawMagie(SDL_Surface* gpScreen);
        void drawNombre(SDL_Surface* gpScreen, int val, int max, int x, int y, int nb);
        SDL_Surface* imageStatut;
        SDL_Surface* imageChiffre;
        SDL_Surface* imageInventaire;
        Jeu* gpJeu;
        bool jauge; // jauge activée
        int valjauge;
        int maxjauge;
        int nivjauge; // hauteur 
        int valjauge2;
        int maxjauge2;
        int nivjauge2; // hauteur 
        int dec;
};

#endif  // Statut.h
