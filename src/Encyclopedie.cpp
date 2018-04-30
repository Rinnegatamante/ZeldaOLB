/*

    Zelda Oni Link Begins

    Copyright (C) 2006-2008  Vincent Jouillat

    Please send bugreports with examples or suggestions to www.zeldaroth.fr

*/

#include <SDL/SDL_image.h>
#include "Encyclopedie.h"

Encyclopedie::Encyclopedie(Jeu* jeu) : gpJeu(jeu), page(0), gpEnnemi(0) {
    image = SDL_CreateRGBSurface(SDL_HWSURFACE, 320, 240, 32, 0, 0, 0, 0);
    imageCadre = IMG_Load("app0:/images/logos/cadres.png");
    gpEnnemi = new Ennemi(gpJeu, 0, 0, 0, true);
}

Encyclopedie::~Encyclopedie() {
    SDL_FreeSurface(image);
    SDL_FreeSurface(imageCadre);
    delete gpEnnemi;
}

void Encyclopedie::draw(SDL_Surface* gpScreen) {
    SDL_Rect dst; dst.x = 0; dst.y = 0;
    SDL_BlitSurface(image, NULL, gpScreen, &dst);
}

void Encyclopedie::init() {
    fond();
}

void Encyclopedie::fond() {
    SDL_Rect src; src.w = 16; src.h = 16;
    SDL_Rect dst;
    
    src.x = 16; src.y = 16;
    for (int j = 0; j < 240; j+=16)
        for (int i = 0; i < 320; i+=16) {
            dst.x = i; dst.y = j; SDL_BlitSurface(imageCadre, &src, image, &dst);
        }
        
    src.x = 0; src.y = 0; dst.x = 16; dst.y = 0; 
    SDL_BlitSurface(imageCadre, &src, image, &dst);
    src.x = 0; src.y = 16; dst.x = 16; dst.y = 16; 
    SDL_BlitSurface(imageCadre, &src, image, &dst);
    src.x = 0; src.y = 32; dst.x = 16; dst.y = 32; 
    SDL_BlitSurface(imageCadre, &src, image, &dst);
    
    for (int i = 0; i < 5; i++) {
        src.x = 16; src.y = 0; dst.x = 32+16*i; dst.y = 0; 
        SDL_BlitSurface(imageCadre, &src, image, &dst);
        src.x = 16; src.y = 64; dst.x = 32+16*i; dst.y = 16; 
        SDL_BlitSurface(imageCadre, &src, image, &dst);
        src.x = 16; src.y = 32; dst.x = 32+16*i; dst.y = 32; 
        SDL_BlitSurface(imageCadre, &src, image, &dst);
    }
    
    src.x = 32; src.y = 0; dst.x = 112; dst.y = 0; 
    SDL_BlitSurface(imageCadre, &src, image, &dst);
    src.x = 32; src.y = 16; dst.x = 112; dst.y = 16; 
    SDL_BlitSurface(imageCadre, &src, image, &dst);
    src.x = 32; src.y = 32; dst.x = 112; dst.y = 32; 
    SDL_BlitSurface(imageCadre, &src, image, &dst);
    
    
    ostringstream os; os << (page+1);
    gpJeu->affiche(image, "MONSTERS "+os.str(), 40, 17);
    
    placeCadres();
    
    gpJeu->setStop(true);
    gpEnnemi->draw(image);
    gpJeu->setStop(false);
    
    delete gpEnnemi;
    gpEnnemi = new Ennemi(gpJeu, 0, 0, 0, true);
}

void Encyclopedie::placeCadres() {
    switch (page) {
        case 0 :
            cadre(152,8,32,32,4);
            cadre(208,8,32,32,53);
            cadre(264,8,32,32,54);
            cadre(16,48,48,48,27);
            cadre(96,48,48,48,10);
            cadre(176,48,48,48,9);
            cadre(256,48,48,48,8);
            cadre(16,112,48,48,1);
            cadre(96,112,48,48,6);
            cadre(176,112,48,48,5);
            cadre(256,112,48,48,30);
            cadre(16,176,48,48,15);
            cadre(96,176,48,48,3);
            cadre(176,176,48,48,13);
            cadre(252,176,56,48,14);
            break;
        case 1 :
            cadre(152,8,32,32,18);
            cadre(208,8,32,32,26);
            cadre(264,8,32,32,7);
            cadre(16,48,48,48,2);
            cadre(96,48,48,48,25);
            cadre(176,48,48,48,21);
            cadre(256,48,48,48,29);
            cadre(16,112,48,48,32);
            cadre(96,112,48,48,22);
            cadre(176,112,48,48,16);
            cadre(256,112,48,48,23);
            cadre(16,176,48,48,42);
            cadre(96,176,48,48,37);
            cadre(176,176,48,48,41);
            cadre(252,176,56,48,64);
            break;
        case 2 :
            cadre(152,8,32,32,11);
            cadre(208,8,32,32,12);
            cadre(264,8,32,32,56);
            cadre(16,48,48,48,33);
            cadre(96,48,48,48,34);
            cadre(176,48,48,48,38);
            cadre(256,48,48,48,39);
            cadre(16,112,48,48,72);
            cadre(96,112,48,48,55);
            cadre(176,112,48,48,44);
            cadre(256,112,48,48,46);
            cadre(16,176,48,48,47);
            cadre(96,176,48,48,48);
            cadre(176,176,48,48,75);
            break;
        case 3 :
            cadre(16,48,64,80,57);
            cadre(16,144,64,80,60);
            cadre(240,112,48,56,62);
            cadre(240,184,48,48,28);
            cadre(224,8,80,88,45);
            cadre(144,8,64,56,40);
            cadre(96,80,112,144,66);
            break;
        case 4 :
            cadre(16,48,72,80,63);
            cadre(104,48,64,72,71);
            cadre(184,96,48,48,43);
            cadre(200,32,80,48,19);
            cadre(248,96,48,48,35);
            cadre(16,144,48,56,24);
            cadre(80,144,72,64,76);
            cadre(168,160,48,48,31);
            cadre(232,160,64,48,50);
            break;
        case 5 :
            cadre(24,48,120,176,73);
            cadre(176,48,120,176,74);
            break;
    }
}

void Encyclopedie::cadre(int x, int y, int w, int h, int id) {
    SDL_Rect src; src.w = 16; src.h = 16;
    SDL_Rect dst;
    
    for (int j = y+16; j < y+h-16; j+=16)
        for (int i = x+16; i < x+w-16; i+=16) {
            src.x = 16; src.y = 64; dst.x = i; dst.y = j; 
            SDL_BlitSurface(imageCadre, &src, image, &dst);
        }
    
    //haut et bas
    for (int i = x+16; i < x+w-16; i+=16) {
        src.x = 16; src.y = 48; dst.x = i; dst.y = y; 
        SDL_BlitSurface(imageCadre, &src, image, &dst);
        src.x = 16; src.y = 80; dst.x = i; dst.y = y+h-16; 
        SDL_BlitSurface(imageCadre, &src, image, &dst);
    }
    
    //gauche et droite
    for (int j = y+16; j < y+h-16; j+=16) {
        src.x = 0; src.y = 64; dst.x = x; dst.y = j; 
        SDL_BlitSurface(imageCadre, &src, image, &dst);
        src.x = 32; src.y = 64; dst.x = x+w-16; dst.y = j;
        SDL_BlitSurface(imageCadre, &src, image, &dst);
    }
    
    //haut gauche
    src.x = 0; src.y = 48; dst.x = x; dst.y = y; 
    SDL_BlitSurface(imageCadre, &src, image, &dst);
    
    //haut droite
    src.x = 32; src.y = 48; dst.x = x+w-16; dst.y = y; 
    SDL_BlitSurface(imageCadre, &src, image, &dst);
    
    //bas gauche
    src.x = 0; src.y = 80; dst.x = x; dst.y = y+h-16; 
    SDL_BlitSurface(imageCadre, &src, image, &dst);
    
    //bas droite
    src.x = 32; src.y = 80; dst.x = x+w-16; dst.y = y+h-16; 
    SDL_BlitSurface(imageCadre, &src, image, &dst);
    
    if (!gpJeu->getJoueur()->getEnnemi(id)) return;
    Ennemi* enn = new Ennemi(gpJeu, id, 0, 0, true);
    if (id == 66) enn->setH(124);
    enn->setX(gpJeu->getPhg(0)+x+((w-enn->getW())/2));
    enn->setY(gpJeu->getPhg(1)+y+((h-enn->getH())/2));
    gpEnnemi->ajout(enn);
}

bool Encyclopedie::isVide() {
    Joueur* gpJoueur = gpJeu->getJoueur();
    switch (page) {
        case 0 :
            if (gpJoueur->getEnnemi(4) || gpJoueur->getEnnemi(53) || gpJoueur->getEnnemi(54)
            || gpJoueur->getEnnemi(27) || gpJoueur->getEnnemi(10) || gpJoueur->getEnnemi(9)
            || gpJoueur->getEnnemi(8) || gpJoueur->getEnnemi(1) || gpJoueur->getEnnemi(6)
            || gpJoueur->getEnnemi(5) || gpJoueur->getEnnemi(30) || gpJoueur->getEnnemi(15)
            || gpJoueur->getEnnemi(3) || gpJoueur->getEnnemi(13) || gpJoueur->getEnnemi(14))
                return false;
            return true;
            break;
        case 1 :
            if (gpJoueur->getEnnemi(18) || gpJoueur->getEnnemi(16) || gpJoueur->getEnnemi(7)
            || gpJoueur->getEnnemi(2) || gpJoueur->getEnnemi(25) || gpJoueur->getEnnemi(21)
            || gpJoueur->getEnnemi(29) || gpJoueur->getEnnemi(32) || gpJoueur->getEnnemi(22)
            || gpJoueur->getEnnemi(16) || gpJoueur->getEnnemi(23) || gpJoueur->getEnnemi(42)
            || gpJoueur->getEnnemi(37) || gpJoueur->getEnnemi(41) || gpJoueur->getEnnemi(64))
                return false;
            return true;
            break;
        case 2 :
            if (gpJoueur->getEnnemi(11) || gpJoueur->getEnnemi(12) || gpJoueur->getEnnemi(56)
            || gpJoueur->getEnnemi(33) || gpJoueur->getEnnemi(34) || gpJoueur->getEnnemi(38)
            || gpJoueur->getEnnemi(39) || gpJoueur->getEnnemi(72) || gpJoueur->getEnnemi(55)
            || gpJoueur->getEnnemi(44) || gpJoueur->getEnnemi(46) || gpJoueur->getEnnemi(47)
            || gpJoueur->getEnnemi(48) || gpJoueur->getEnnemi(75))
                return false;
            return true;
            break;
        case 3 :
            if (gpJoueur->getEnnemi(57) || gpJoueur->getEnnemi(60) || gpJoueur->getEnnemi(62)
            || gpJoueur->getEnnemi(28) || gpJoueur->getEnnemi(45) || gpJoueur->getEnnemi(40)
            || gpJoueur->getEnnemi(66))
                return false;
            return true;
            break;
        case 4 :
            if (gpJoueur->getEnnemi(63) || gpJoueur->getEnnemi(71) || gpJoueur->getEnnemi(43)
            || gpJoueur->getEnnemi(19) || gpJoueur->getEnnemi(35) || gpJoueur->getEnnemi(24)
            || gpJoueur->getEnnemi(76) || gpJoueur->getEnnemi(31) || gpJoueur->getEnnemi(50))
                return false;
            return true;
            break;
        case 5 :
            if (gpJoueur->getEnnemi(73) || gpJoueur->getEnnemi(74)) return false;
            return true;
            break;
    }
    return true;
}

void Encyclopedie::verif(int sens) {
    if (sens == 0) {while (isVide()) {page--; if (page < 0) page = 5;}}
    else {while (isVide()) {page++; if (page > 5) page = 0;}}
}

void Encyclopedie::moveL() {page--; verif(0); init();}

void Encyclopedie::moveR() {page++; verif(1); init();}

void Encyclopedie::setPage(int i) {
    page = i; if (page < 0) page = 0; if (page > 3) page = 3; verif(1);
}
