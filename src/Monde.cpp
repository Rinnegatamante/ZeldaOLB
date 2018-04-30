/*

    Zelda Oni Link Begins

    Copyright (C) 2006-2008  Vincent Jouillat

    Please send bugreports with examples or suggestions to www.zeldaroth.fr

*/

#include <string>
#include <sstream>
#include <fstream>

#include <SDL/SDL_image.h>
#include "vita/os_vita.h"

#include "Menu.h"
#include "Texte.h"
#include "Joueur.h"
#include "Monde.h"
#include "Projectile.h"
#include "Jeu.h"
#include "Common.h"

extern "C"{
int filledEllipseRGBA(SDL_Surface * dst, int16_t x, int16_t y,
    int16_t rx, int16_t ry, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
};

Monde::Monde(Jeu* jeu) : largeur(0), hauteur(0), plafondmax(0), anim(0), utile(0),
musique(0), gpJeu(jeu), animtransX(0), animtransY(0), animtransC(0), niveau(0), charge(1),
avion(0), ovni(0), ecl1(0), ecl2(0), xecl1(0), xecl2(0) {
    
    for (int i = 0; i < 5; i++) {
        std::ostringstream im;
        im << (i+1);
        image[i] = IMG_Load(("app0:/images/motifs/image" + im.str() + ".png").c_str());
        SDL_SetColorKey(image[i],SDL_SRCCOLORKEY,SDL_MapRGB(image[i]->format,0,0,255));
    }
    
    imagetransit = SDL_CreateRGBSurface(SDL_HWSURFACE, 320, 240, 32, 0, 0, 0, 0);
    
    imagelevel = IMG_Load("app0:/images/menu/level.png");
    SDL_SetColorKey(imagelevel,SDL_SRCCOLORKEY,SDL_MapRGB(imagelevel->format,0,0,255));
    
    imageSpe[0] = IMG_Load("app0:/images/motifs/fond.png");
    imageSpe[1] = IMG_Load("app0:/images/motifs/fond2.png");
    imageSpe[2] = IMG_Load("app0:/images/motifs/fond3.png");
    imageSpe[3] = IMG_Load("app0:/images/motifs/eclair.png");
    imageSpe[4] = IMG_Load("app0:/images/motifs/pluie1.png");
    imageSpe[5] = IMG_Load("app0:/images/motifs/pluie2.png");
    imageSpe[6] = IMG_Load("app0:/images/motifs/pluie3.png");
    imageSpe[7] = IMG_Load("app0:/images/motifs/ciel.png");
    
    for (int i = 3; i < 8; i++)
        SDL_SetColorKey(imageSpe[i],SDL_SRCCOLORKEY,SDL_MapRGB(imageSpe[i]->format,0,0,255));
    
    changeZone(gpJeu->getZone());
    
    charge=0;
}

Monde::~Monde() {
    for (int i = 0; i < 5; i++) SDL_FreeSurface(image[i]);
    SDL_FreeSurface(imagetransit);
    SDL_FreeSurface(imagelevel);
    for (int i = 0; i < 8; i++) SDL_FreeSurface(imageSpe[i]);
}

void Monde::changeZone(int newZone) {
    
    chargeMap(newZone);
    
    initMatrice(newZone);
    
    detRegion(newZone);
    
    lastAnimTime = SDL_GetTicks();
}

void Monde::corrigeMap(int zone) {
    Joueur* gpJoueur = gpJeu->getJoueur();
    
    switch(zone) {
        case 1 :
            if (!gpJoueur->hasObjet(O_BOTTES)) setValeur(65*16, 19*16, 303, 0, PLEIN, C_BOTTES);
            else setValeur(65*16, 19*16, 304, 0, PLEIN, C_RIEN); break;
        case 2 :
            if (gpJoueur->getCoffre(14, 0)) passage(14,29,1);
            if (gpJoueur->getCoffre(14, 1)) passage(87,35,3); break;
        case 3 : if (gpJoueur->getCoffre(14,2)) passage(10,12,0); break;
        case 5 : if (gpJoueur->getCoffre(14,3)) passage(23,70,0); break;
        case 6 : if (gpJoueur->getCoffre(14,4)) passage(93,31,1); break;
        case 8 : if (gpJoueur->getCoffre(14,5)) passage(21,20,1); break;
        case 9 : if (gpJoueur->getCoffre(14,6)) passage(39,86,0); break;
        case 10 : if (gpJoueur->getCoffre(14,7)) passage(24,38,1); break;
        case 14 :
            if (gpJoueur->getCoffre(14,8)) passage(16,55,1);
            if (gpJoueur->getCoffre(14,9)) passage(94,34,1);
            if (gpJoueur->getCoffre(14,10)) passage(94,36,4);
            break;
        case 15 :
            //carte
            if(!gpJoueur->getCle(0,0)) setValeur(31*16,47*16,303,-1,PLEIN,C_CARTE);
            else setValeur(31*16,47*16,304,-1,PLEIN,C_RIEN);
            //boussole
            if(!gpJoueur->getCle(0,1)) setValeur(90*16,36*16,303,-1,PLEIN,C_BOUSSOLE);
            else setValeur(90*16,36*16,304,-1,PLEIN,C_RIEN);
            //cle boss
            if(!gpJoueur->getCle(0,2)) setValeur(170*16,22*16,303,-1,PLEIN,C_CLE_BOSS);
            else setValeur(170*16,22*16,304,-1,PLEIN,C_RIEN);
            //cles
            if(!gpJoueur->getCoffre(0,8)) setValeur(70*16,52*16,303,-1,PLEIN,C_CLE);
            else setValeur(70*16,52*16,304,-1,PLEIN,C_RIEN);
            if(!gpJoueur->getCoffre(0,9)) setValeur(170*16,52*16,303,-1,PLEIN,C_CLE);
            else setValeur(170*16,52*16,304,-1,PLEIN,C_RIEN);
            if(!gpJoueur->getCoffre(0,10)) setValeur(30*16,6*16,303,-1,PLEIN,C_CLE);
            else setValeur(30*16,6*16,304,-1,PLEIN,C_RIEN);
            if(!gpJoueur->getCoffre(0,11)) setValeur(190*16,7*16,303,-1,PLEIN,C_CLE);
            else setValeur(190*16,7*16,304,-1,PLEIN,C_RIEN);
            //épée
            if (!gpJoueur->getEpee()) setValeur(130*16,7*16,303,-1,PLEIN,C_EPEE);
            else setValeur(130*16,7*16,304,-1,PLEIN,C_RIEN);
            //porte boss
            if (gpJoueur->getCoffre(0,0)) passage(89,30,0);
            //portes
            if (gpJoueur->getCoffre(0,1)) passage(40,51,1);
            if (gpJoueur->getCoffre(0,2)) passage(109,45,0);
            if (gpJoueur->getCoffre(0,3)) passage(20,21,1);
            if (gpJoueur->getCoffre(0,4)) passage(180,21,1);
            if (gpJoueur->getCoffre(0,5)) {passage(9,45,0); map[7][42]=1005;}
            if (gpJoueur->getCoffre(0,6)) {passage(49,45,0); map[52][42]=1005;}
            if (gpJoueur->getCoffre(0,7)) {passage(29,15,0); map[27][12]=1005;}
            break;
        case 16 :
            //carte
            if(!gpJoueur->getCle(1,0)) setValeur(10*16,7*16,303,-1,PLEIN,C_CARTE);
            else setValeur(10*16,7*16,304,-1,PLEIN,C_RIEN);
            //boussole
            if(!gpJoueur->getCle(1,1)) setValeur(139*16,32*16,303,-1,PLEIN,C_BOUSSOLE);
            else setValeur(139*16,32*16,304,-1,PLEIN,C_RIEN);
            //cle boss
            if(!gpJoueur->getCle(1,2)) setValeur(251*16,29*16,303,-1,PLEIN,C_CLE_BOSS);
            else setValeur(251*16,29*16,304,-1,PLEIN,C_RIEN);
            //porte boss
            if (gpJoueur->getCoffre(1,0)) passage(249,90,0);
            //portes
            if (gpJoueur->getCoffre(1,1)) passage(260,66,3);
            if (gpJoueur->getCoffre(1,2)) passage(269,75,4);
            if (gpJoueur->getCoffre(1,3)) passage(160,81,3);
            if (gpJoueur->getCoffre(1,4)) passage(109,30,0);
            if (gpJoueur->getCoffre(1,5)) passage(120,66,1);
            if (gpJoueur->getCoffre(1,6)) passage(124,75,0);
            if (gpJoueur->getCoffre(1,7)) passage(134,75,0);
            if (gpJoueur->getCoffre(1,8)) passage(249,15,0);
            if (gpJoueur->getCoffre(1,9)) {passage(60,51,1); contenu[42][47]=51;}
            if (gpJoueur->getCoffre(1,10)) {passage(89,15,0); contenu[91][21]=51;}
            if (gpJoueur->getCoffre(1,11)) {passage(9,15,0); map[3][3]=1005;}
            if (gpJoueur->getCoffre(1,12)) {passage(204,15,0); map[207][25]=1005;}
            if (gpJoueur->getCoffre(1,13)) {passage(240,2,1); map[223][3]=1005;}
            if (gpJoueur->getCoffre(1,14)) {passage(240,10,1); map[256][11]=1005;}
            if (gpJoueur->getCoffre(1,15)) {passage(280,6,1); map[287][11]=1005;}
            if (gpJoueur->getCoffre(1,16)) {passage(289,30,0); map[292][18]=1005;}
            if (gpJoueur->getCoffre(1,17)) {passage(280,36,1); map[296][33]=1005;}
            if (gpJoueur->getCoffre(1,18)) {passage(269,45,0); map[269][38]=1005;}
            if (gpJoueur->getCoffre(1,19)) {passage(229,45,0); map[236][52]=1005;}
            if (gpJoueur->getCoffre(1,20)) {passage(240,92,1); map[256][93]=1005;}
            if (gpJoueur->getCoffre(1,21)) {passage(180,66,1); map[185][62]=1005;}
            if (gpJoueur->getCoffre(1,22)) {setValeur(174*16,22*16,1362,0); map[163][18]=1005;}
            if (gpJoueur->getCoffre(1,23)) {setValeur(185*16,22*16,1362,0); map[196][18]=1005;}
            if (gpJoueur->getCoffre(1,24)) {setValeur(194*16,66*16,1362,0); map[196][69]=1005;}
            break;
        case 17 :
            //carte
            if(!gpJoueur->getCle(2,0)) setValeur(230*16,112*16,303,-1,PLEIN,C_CARTE);
            else setValeur(230*16,112*16,304,-1,PLEIN,C_RIEN);
            //boussole
            if(!gpJoueur->getCle(2,1)) setValeur(230*16,97*16,303,-1,PLEIN,C_BOUSSOLE);
            else setValeur(230*16,97*16,304,-1,PLEIN,C_RIEN);
            //cle boss
            if(!gpJoueur->getCle(2,2)) setValeur(56*16,63*16,303,-1,PLEIN,C_CLE_BOSS);
            else setValeur(56*16,63*16,304,-1,PLEIN,C_RIEN);
            //cles
            if(!gpJoueur->getCoffre(2,17)) setValeur(50*16,52*16,303,-1,PLEIN,C_CLE);
            else setValeur(50*16,52*16,304,-1,PLEIN,C_RIEN);
            if(!gpJoueur->getCoffre(2,18)) setValeur(90*16,52*16,303,-1,PLEIN,C_CLE);
            else setValeur(90*16,52*16,304,-1,PLEIN,C_RIEN);
            if(!gpJoueur->getCoffre(2,19)) setValeur(150*16,97*16,303,-1,PLEIN,C_CLE);
            else setValeur(150*16,97*16,304,-1,PLEIN,C_RIEN);
            if(!gpJoueur->getCoffre(2,20)) setValeur(190*16,52*16,303,-1,PLEIN,C_CLE);
            else setValeur(190*16,52*16,304,-1,PLEIN,C_RIEN);
            //gants
            if (!gpJoueur->hasObjet(O_GANTS)) setValeur(70*16,6*16,303,-1,PLEIN,C_GANTS);
            else setValeur(70*16,6*16,304,-1,PLEIN,C_RIEN);
            //porte boss
            if (gpJoueur->getCoffre(2,0)) passage(9,45,0);
            //portes
            if (gpJoueur->getCoffre(2,1)) passage(49,45,4);
            if (gpJoueur->getCoffre(2,2)) passage(80,51,3);
            if (gpJoueur->getCoffre(2,3)) passage(89,75,4);
            if (gpJoueur->getCoffre(2,4)) passage(89,66,8);
            if (gpJoueur->getCoffre(2,5)) passage(160,111,3);
            if (gpJoueur->getCoffre(2,6)) passage(189,45,4);
            if (gpJoueur->getCoffre(2,7)) passage(229,105,4);
            if (gpJoueur->getCoffre(2,8)) passage(69,15,0);
            if (gpJoueur->getCoffre(2,9)) passage(69,45,0);
            if (gpJoueur->getCoffre(2,10)) passage(69,75,0);
            if (gpJoueur->getCoffre(2,11)) passage(189,30,0);
            if (gpJoueur->getCoffre(2,12)) {passage(154,75,0); map[146][66]=1005;}
            if (gpJoueur->getCoffre(2,13)) {passage(180,21,1); map[172][27]=1005;}
            if (gpJoueur->getCoffre(2,14)) {passage(200,81,1); map[196][78]=1005;}
            if (gpJoueur->getCoffre(2,15)) {passage(209,75,0); map[207][71]=1005;}
            if (gpJoueur->getCoffre(2,16)) {passage(200,111,1); map[203][108]=1005;}
            break;
        case 18 :
            //carte
            if(!gpJoueur->getCle(3,0)) setValeur(150*16,37*16,303,-1,PLEIN,C_CARTE);
            else setValeur(150*16,37*16,304,-1,PLEIN,C_RIEN);
            //boussole
            if(!gpJoueur->getCle(3,1)) setValeur(116*16,18*16,303,-1,PLEIN,C_BOUSSOLE);
            else setValeur(116*16,18*16,304,-1,PLEIN,C_RIEN);
            //cle boss
            if(!gpJoueur->getCle(3,2)) setValeur(270*16,67*16,303,-1,PLEIN,C_CLE_BOSS);
            else setValeur(270*16,67*16,304,-1,PLEIN,C_RIEN);
            //cles
            if(!gpJoueur->getCoffre(3,24)) setValeur(106*16,33*16,303,-1,PLEIN,C_CLE);
            else setValeur(106*16,33*16,304,-1,PLEIN,C_RIEN);
            if(!gpJoueur->getCoffre(3,25)) setValeur(130*16,67*16,303,-1,PLEIN,C_CLE);
            else setValeur(130*16,67*16,304,-1,PLEIN,C_RIEN);
            if(!gpJoueur->getCoffre(3,26)) setValeur(170*16,21*16,303,-1,PLEIN,C_CLE);
            else setValeur(170*16,21*16,304,-1,PLEIN,C_RIEN);
            if(!gpJoueur->getCoffre(3,27)) setValeur(210*16,67*16,303,-1,PLEIN,C_CLE);
            else setValeur(210*16,67*16,304,-1,PLEIN,C_RIEN);
            if(!gpJoueur->getCoffre(3,28)) setValeur(230*16,52*16,303,-1,PLEIN,C_CLE);
            else setValeur(230*16,52*16,304,-1,PLEIN,C_RIEN);
            if(!gpJoueur->getCoffre(3,29)) setValeur(292*16,25*16,303,-1,PLEIN,C_CLE);
            else setValeur(292*16,25*16,304,-1,PLEIN,C_RIEN);
            if(!gpJoueur->getCoffre(3,30)) setValeur(292*16,56*16,303,-1,PLEIN,C_CLE);
            else setValeur(292*16,56*16,304,-1,PLEIN,C_RIEN);
            if(!gpJoueur->getCoffre(3,31)) setValeur(230*16,37*16,303,-1,PLEIN,C_CLE);
            else setValeur(230*16,37*16,304,-1,PLEIN,C_RIEN);
            //porte boss
            if (gpJoueur->getCoffre(3,0)) passage(269,30,0);
            //portes
            if (gpJoueur->getCoffre(3,1)) passage(100,21,1);
            if (gpJoueur->getCoffre(3,2)) passage(100,51,1);
            if (gpJoueur->getCoffre(3,3)) passage(149,30,0);
            if (gpJoueur->getCoffre(3,4)) passage(140,51,1);
            if (gpJoueur->getCoffre(3,5)) passage(280,36,1);
            if (gpJoueur->getCoffre(3,6)) passage(289,45,0);
            if (gpJoueur->getCoffre(3,7)) passage(309,45,0);
            if (gpJoueur->getCoffre(3,8)) passage(280,66,1);
            if (gpJoueur->getCoffre(3,9)) {
                setValeur(69*16,21*16,1872,-1);
                setValeur(70*16,21*16,1874,-1);
                setValeur(69*16,22*16,1878,-1);
                setValeur(70*16,22*16,1880,-1);
            }
            if (gpJoueur->getCoffre(3,10)) passage(249,45,4);
            if (gpJoueur->getCoffre(3,11)) passage(289,60,4);
            if (gpJoueur->getCoffre(3,12)) {passage(160,21,1); map[155][15]=1005;}
            if (gpJoueur->getCoffre(3,13)) {passage(189,60,0); map[197][62]=1005;}
            if (gpJoueur->getCoffre(3,14)) {passage(169,60,0); map[176][56]=1005;}
            if (gpJoueur->getCoffre(3,15)) {passage(149,60,0); map[143][56]=1005;}
            if (gpJoueur->getCoffre(3,16)) {passage(200,66,1); map[203][71]=1005;}
            if (gpJoueur->getCoffre(3,17)) {passage(260,51,1); map[247][48]=1005;}
            if (gpJoueur->getCoffre(3,18)) {passage(240,51,1); map[236][48]=1005;}
            if (gpJoueur->getCoffre(3,19)) {passage(240,36,1); map[236][41]=1005;}
            if (gpJoueur->getCoffre(3,20)) {passage(289,30,0); map[287][25]=1005;}
            if (gpJoueur->getCoffre(3,21)) {passage(280,51,1); map[287][48]=1005;}
            if (gpJoueur->getCoffre(3,22)) {passage(309,60,0); map[307][48]=1005;}
            if (gpJoueur->getCoffre(3,23)) {setValeur(270*16,8*16,1727,0); map[263][11]=1005;}
            break;
        case 19 :
            //carte
            if(!gpJoueur->getCle(4,0)) setValeur(43*16,33*16,303,-1,PLEIN,C_CARTE);
            else setValeur(43*16,33*16,304,-1,PLEIN,C_RIEN);
            //boussole
            if(!gpJoueur->getCle(4,1)) setValeur(10*16,65*16,303,-1,PLEIN,C_BOUSSOLE);
            else setValeur(10*16,65*16,304,-1,PLEIN,C_RIEN);
            //cle boss
            if(!gpJoueur->getCle(4,2)) setValeur(196*16,52*16,303,-1,PLEIN,C_CLE_BOSS);
            else setValeur(196*16,52*16,304,-1,PLEIN,C_RIEN);
            //cles
            if(!gpJoueur->getCoffre(4,14)) setValeur(10*16,51*16,303,-1,PLEIN,C_CLE);
            else setValeur(10*16,51*16,304,-1,PLEIN,C_RIEN);
            if(!gpJoueur->getCoffre(4,15)) setValeur(146*16,66*16,303,-1,PLEIN,C_CLE);
            else setValeur(146*16,66*16,304,-1,PLEIN,C_RIEN);
            if(!gpJoueur->getCoffre(4,16)) setValeur(169*16,23*16,303,-1,PLEIN,C_CLE);
            else setValeur(169*16,23*16,304,-1,PLEIN,C_RIEN);
            if(!gpJoueur->getCoffre(4,17)) setValeur(188*16,67*16,303,-1,PLEIN,C_CLE);
            else setValeur(188*16,67*16,304,-1,PLEIN,C_RIEN);
            if(!gpJoueur->getCoffre(4,18)) setValeur(112*16,67*16,303,-1,PLEIN,C_CLE);
            else setValeur(112*16,67*16,304,-1,PLEIN,C_RIEN);
            //marteau
            if (!gpJoueur->hasObjet(O_MARTEAU)) setValeur(170*16,17*16,303,-1,PLEIN,C_MARTEAU);
            else setValeur(170*16,17*16,304,-1,PLEIN,C_RIEN);
            //porte boss
            if (gpJoueur->getCoffre(4,0)) passage(89,45,0);
            //portes
            if (gpJoueur->getCoffre(4,1)) passage(160,21,3);
            if (gpJoueur->getCoffre(4,2)) passage(49,45,0);
            if (gpJoueur->getCoffre(4,3)) passage(49,60,0);
            if (gpJoueur->getCoffre(4,4)) passage(149,30,0);
            if (gpJoueur->getCoffre(4,5)) passage(160,6,1);
            if (gpJoueur->getCoffre(4,6)) passage(160,66,1);
            if (gpJoueur->getCoffre(4,7)) {passage(9,45,0); map[7][41]=1005;}
            if (gpJoueur->getCoffre(4,8)) {passage(80,51,1); contenu[74][47]=51;}
            if (gpJoueur->getCoffre(4,9)) {passage(129,30,0); map[136][22]=1005;}
            if (gpJoueur->getCoffre(4,10)) {passage(129,60,0); map[116][67]=1005;}
            if (gpJoueur->getCoffre(4,11)) {passage(149,15,0); map[143][7]=1005;}
            if (gpJoueur->getCoffre(4,12)) {passage(140,66,1); map[156][67]=1005;}
            if (gpJoueur->getCoffre(4,13)) {passage(180,36,1); map[194][38]=1005;}
            break;
        case 20 :
            //carte
            if(!gpJoueur->getCle(5,0)) setValeur(75*16,17*16,303,-1,PLEIN,C_CARTE);
            else setValeur(75*16,17*16,304,-1,PLEIN,C_RIEN);
            //boussole
            if(!gpJoueur->getCle(5,1)) setValeur(91*16,41*16,303,-1,PLEIN,C_BOUSSOLE);
            else setValeur(91*16,41*16,304,-1,PLEIN,C_RIEN);
            //cle boss
            if(!gpJoueur->getCle(5,2)) setValeur(96*16,48*16,303,-1,PLEIN,C_CLE_BOSS);
            else setValeur(96*16,48*16,304,-1,PLEIN,C_RIEN);
            //grappin
            if (!gpJoueur->hasObjet(O_GRAPPIN)) setValeur(10*16,19*16,303,-1,PLEIN,C_GRAPPIN);
            else setValeur(10*16,19*16,304,-1,PLEIN,C_RIEN);
            //cles
            if(!gpJoueur->getCoffre(5,1)) gpJeu->ajouteObjet(I_PETITE_CLE,16*16,16*54,0,1);
            if(!gpJoueur->getCoffre(5,2)) setValeur(10*16,81*16,303,-1,PLEIN,C_CLE);
            else setValeur(10*16,81*16,304,-1,PLEIN,C_RIEN);
            if(!gpJoueur->getCoffre(5,3)) gpJeu->ajouteObjet(I_PETITE_CLE,16*23,16*3,0,1);
            if(!gpJoueur->getCoffre(5,9)) gpJeu->ajouteObjet(I_PETITE_CLE,16*132,16*39,0,1);
            if(!gpJoueur->getCoffre(5,10)) gpJeu->ajouteObjet(I_PETITE_CLE,16*137,16*87,0,1);
            //porte boss
            if (gpJoueur->getCoffre(5,0)) passage(9,45,0);
            //portes
            if (gpJoueur->getCoffre(5,15)) passage(84,45,0);
            if (gpJoueur->getCoffre(5,16)) passage(80,40,1);
            if (gpJoueur->getCoffre(5,17)) passage(80,32,1);
            if (gpJoueur->getCoffre(5,18)) passage(84,30,0);
            if (gpJoueur->getCoffre(5,19)) passage(80,25,1);
            if (gpJoueur->getCoffre(5,20)) passage(80,17,1);
            if (gpJoueur->getCoffre(5,21)) passage(84,15,0);
            if (gpJoueur->getCoffre(5,22)) passage(94,15,0);
            if (gpJoueur->getCoffre(5,23)) passage(100,17,1);
            if (gpJoueur->getCoffre(5,24)) passage(100,25,1);
            if (gpJoueur->getCoffre(5,25)) passage(94,30,0);
            if (gpJoueur->getCoffre(5,26)) passage(100,32,1);
            if (gpJoueur->getCoffre(5,27)) passage(100,40,1);
            if (gpJoueur->getCoffre(5,28)) passage(94,45,0);
            if (gpJoueur->getCoffre(5,29)) passage(60,51,3);
            if (gpJoueur->getCoffre(5,30)) passage(60,66,3);
            if (gpJoueur->getCoffre(5,31)) passage(120,51,3);
            if (gpJoueur->getCoffre(5,32)) passage(120,66,3);
            break;
        case 21 :
            //carte
            if(!gpJoueur->getCle(6,0)) setValeur(96*16,18*16,303,-1,PLEIN,C_CARTE);
            else setValeur(96*16,18*16,304,-1,PLEIN,C_RIEN);
            //boussole
            if(!gpJoueur->getCle(6,1)) setValeur(30*16,2*16,303,-1,PLEIN,C_BOUSSOLE);
            else setValeur(30*16,2*16,304,-1,PLEIN,C_RIEN);
            //cle boss
            if(!gpJoueur->getCle(6,2)) setValeur(50*16,6*16,303,-1,PLEIN,C_CLE_BOSS);
            else setValeur(50*16,6*16,304,-1,PLEIN,C_RIEN);
            //cles
            if(!gpJoueur->getCoffre(6,18)) setValeur(89*16,2*16,303,-1,PLEIN,C_CLE);
            else setValeur(89*16,2*16,304,-1,PLEIN,C_RIEN);
            if(!gpJoueur->getCoffre(6,19)) setValeur(106*16,2*16,303,-1,PLEIN,C_CLE);
            else setValeur(106*16,2*16,304,-1,PLEIN,C_RIEN);
            if(!gpJoueur->getCoffre(6,20)) setValeur(124*16,37*16,303,-1,PLEIN,C_CLE);
            else setValeur(124*16,37*16,304,-1,PLEIN,C_RIEN);
            if(!gpJoueur->getCoffre(6,21)) setValeur(159*16,22*16,303,-1,PLEIN,C_CLE);
            else setValeur(159*16,22*16,304,-1,PLEIN,C_RIEN);
            if(!gpJoueur->getCoffre(6,22)) setValeur(158*16,37*16,303,-1,PLEIN,C_CLE);
            else setValeur(158*16,37*16,304,-1,PLEIN,C_RIEN);
            if(!gpJoueur->getCoffre(6,23)) setValeur(231*16,19*16,303,-1,PLEIN,C_CLE);
            else setValeur(231*16,19*16,304,-1,PLEIN,C_RIEN);
            if(!gpJoueur->getCoffre(6,24)) setValeur(250*16,49*16,303,-1,PLEIN,C_CLE);
            else setValeur(250*16,49*16,304,-1,PLEIN,C_RIEN);
            //palmes
            if (!gpJoueur->hasObjet(O_PALMES)) setValeur(256*16,50*16,303,-1,PLEIN,C_PALMES);
            else setValeur(256*16,50*16,304,-1,PLEIN,C_RIEN);
            //porte boss
            if (gpJoueur->getCoffre(6,0)) passage(249,30,0);
            //portes
            if (gpJoueur->getCoffre(6,1)) {passage(29,15,0); map[24][22]=1005;}
            if (gpJoueur->getCoffre(6,2)) {passage(40,51,1); map[23][56]=1005;}
            if (gpJoueur->getCoffre(6,3)) {passage(40,6,1); map[57][7]=1005;}
            if (gpJoueur->getCoffre(6,4)) {passage(89,30,0); map[96][26]=1005;}
            if (gpJoueur->getCoffre(6,5)) {passage(120,51,1); map[130][49]=1005;}
            if (gpJoueur->getCoffre(6,6)) {passage(169,15,0); map[176][7]=1005;}
            if (gpJoueur->getCoffre(6,7)) {passage(189,15,0); map[196][11]=1005;}
            if (gpJoueur->getCoffre(6,8)) {passage(229,30,0); map[230][25]=1005;}
            if (gpJoueur->getCoffre(6,9)) {passage(229,45,0); map[223][52]=1005;}
            if (gpJoueur->getCoffre(6,10)) {passage(280,21,1); map[272][26]=1005;}
            if (gpJoueur->getCoffre(6,11)) passage(29,45,0);
            if (gpJoueur->getCoffre(6,12)) passage(49,15,0);
            if (gpJoueur->getCoffre(6,13)) passage(120,21,1);
            if (gpJoueur->getCoffre(6,14)) passage(160,51,1);
            if (gpJoueur->getCoffre(6,15)) passage(180,51,1);
            if (gpJoueur->getCoffre(6,16)) passage(209,45,0);
            if (gpJoueur->getCoffre(6,17)) passage(240,36,1);
            break;
        case 22 :
            //carte
            if(!gpJoueur->getCle(7,0)) setValeur(132*16,77*16,303,-1,PLEIN,C_CARTE);
            else setValeur(132*16,77*16,304,-1,PLEIN,C_RIEN);
            //boussole
            if(!gpJoueur->getCle(7,1)) setValeur(39*16,32*16,303,-1,PLEIN,C_BOUSSOLE);
            else setValeur(39*16,32*16,304,-1,PLEIN,C_RIEN);
            //cle boss
            if(!gpJoueur->getCle(7,2)) setValeur(50*16,21*16,303,-1,PLEIN,C_CLE_BOSS);
            else setValeur(50*16,21*16,304,-1,PLEIN,C_RIEN);
            //cles
            if(!gpJoueur->getCoffre(7,15)) setValeur(9*16,23*16,303,-1,PLEIN,C_CLE);
            else setValeur(9*16,23*16,304,-1,PLEIN,C_RIEN);
            if(!gpJoueur->getCoffre(7,16)) setValeur(83*16,48*16,303,-1,PLEIN,C_CLE);
            else setValeur(83*16,48*16,304,-1,PLEIN,C_RIEN);
            if(!gpJoueur->getCoffre(7,17)) setValeur(119*16,35*16,303,-1,PLEIN,C_CLE);
            else setValeur(119*16,35*16,304,-1,PLEIN,C_RIEN);
            if(!gpJoueur->getCoffre(7,18)) setValeur(156*16,33*16,303,-1,PLEIN,C_CLE);
            else setValeur(156*16,33*16,304,-1,PLEIN,C_RIEN);
            if(!gpJoueur->getCoffre(7,19)) setValeur(169*16,81*16,303,-1,PLEIN,C_CLE);
            else setValeur(169*16,81*16,304,-1,PLEIN,C_RIEN);
            if(!gpJoueur->getCoffre(7,20)) setValeur(179*16,7*16,303,-1,PLEIN,C_CLE);
            else setValeur(179*16,7*16,304,-1,PLEIN,C_RIEN);
            //tunique bleue
            if (!gpJoueur->getCoffre(7,21)) {
                if (gpJoueur->getTunique()==1)
                    setValeur(110*16,7*16,303,-1,PLEIN,C_TUNIQUE_BLEUE);
                else
                    setValeur(110*16,7*16,303,-1,PLEIN,C_TUNIQUE_ROUGE);
            }
            else setValeur(110*16,7*16,304,-1,PLEIN,C_RIEN);
            //porte boss
            if (gpJoueur->getCoffre(7,0)) passage(89,30,0);
            //portes
            if (gpJoueur->getCoffre(7,1)) passage(9,30,4);
            if (gpJoueur->getCoffre(7,2)) passage(69,30,4);
            if (gpJoueur->getCoffre(7,3)) passage(100,6,3);
            if (gpJoueur->getCoffre(7,4)) passage(49,30,0);
            if (gpJoueur->getCoffre(7,5)) passage(60,36,1);
            if (gpJoueur->getCoffre(7,6)) passage(100,81,1);
            if (gpJoueur->getCoffre(7,7)) passage(120,21,1);
            if (gpJoueur->getCoffre(7,8)) passage(140,81,1);
            if (gpJoueur->getCoffre(7,9)) passage(149,30,0);
            if (gpJoueur->getCoffre(7,10)) {passage(9,75,0); map[16][80]=1005;}
            if (gpJoueur->getCoffre(7,11)) {passage(40,21,1); map[52][26]=1005;}
            if (gpJoueur->getCoffre(7,12)) {passage(169,30,0); map[166][27]=1005;}
            if (gpJoueur->getCoffre(7,13)) {passage(189,75,0); map[196][77]=1005;}
            if (gpJoueur->getCoffre(7,14)) {passage(209,45,0); contenu[217][42]=51;}
            break;
        case 23 : 
            //carte
            if(!gpJoueur->getCle(8,0)) setValeur(170*16,3*16,303,-1,PLEIN,C_CARTE);
            else setValeur(170*16,3*16,304,-1,PLEIN,C_RIEN);
            //boussole
            if(!gpJoueur->getCle(8,1)) setValeur(230*16,66*16,303,-1,PLEIN,C_BOUSSOLE);
            else setValeur(230*16,66*16,304,-1,PLEIN,C_RIEN);
            //cle boss
            if(!gpJoueur->getCle(8,2)) setValeur(270*16,65*16,303,-1,PLEIN,C_CLE_BOSS);
            else setValeur(270*16,65*16,304,-1,PLEIN,C_RIEN);
            //cles
            if(!gpJoueur->getCoffre(8,15)) setValeur(110*16,23*16,303,-1,PLEIN,C_CLE);
            else setValeur(110*16,23*16,304,-1,PLEIN,C_RIEN);
            if(!gpJoueur->getCoffre(8,17)) setValeur(270*16,5*16,303,-1,PLEIN,C_CLE);
            else setValeur(270*16,5*16,304,-1,PLEIN,C_RIEN);
            if(!gpJoueur->getCoffre(8,18)) setValeur(350*16,65*16,303,-1,PLEIN,C_CLE);
            else setValeur(350*16,65*16,304,-1,PLEIN,C_RIEN);
            //baguette de feu
            if (!gpJoueur->hasObjet(O_BFEU)) setValeur(170*16,52*16,303,-1,PLEIN,C_BFEU);
            else setValeur(170*16,52*16,304,-1,PLEIN,C_RIEN);
            //porte boss
            if (gpJoueur->getCoffre(8,0)) passage(209,45,0);
            //portes
            if (gpJoueur->getCoffre(8,1)) passage(100,36,3);
            if (gpJoueur->getCoffre(8,2)) passage(120,36,3);
            if (gpJoueur->getCoffre(8,3)) passage(169,7,8);
            if (gpJoueur->getCoffre(8,4)) passage(240,51,3);
            if (gpJoueur->getCoffre(8,5)) passage(240,66,3);
            if (gpJoueur->getCoffre(8,6)) {passage(89,30,0); map[92][26]=1005;}
            if (gpJoueur->getCoffre(8,7)) {passage(140,51,1); map[150][51]=1005;}
            if (gpJoueur->getCoffre(8,8)) {passage(180,21,1); map[165][24]=1005;}
            if (gpJoueur->getCoffre(8,9)) {passage(200,6,1); map[212][7]=1005;}
            if (gpJoueur->getCoffre(8,10)) {passage(220,6,1); map[207][7]=1005;}
            if (gpJoueur->getCoffre(8,11)) passage(49,15,0);
            if (gpJoueur->getCoffre(8,12)) passage(120,66,1);
            if (gpJoueur->getCoffre(8,13)) passage(140,36,1);
            if (gpJoueur->getCoffre(8,14)) passage(169,45,0);
            passage(169,15,0);  //salle piégée
            break;
        case 24 :
            //carte
            if(!gpJoueur->getCle(9,0)) setValeur(150*16,21*16,303,-1,PLEIN,C_CARTE);
            else setValeur(150*16,21*16,304,-1,PLEIN,C_RIEN);
            //boussole
            if(!gpJoueur->getCle(9,1)) setValeur(97*16,40*16,303,-1,PLEIN,C_BOUSSOLE);
            else setValeur(97*16,40*16,304,-1,PLEIN,C_RIEN);
            //getCle boss
            if(!gpJoueur->getCle(9,2)) setValeur(152*16,9*16,303,-1,PLEIN,C_CLE_BOSS);
            else setValeur(152*16,9*16,304,-1,PLEIN,C_RIEN);
            //cles
            if(!gpJoueur->getCoffre(9,19)) setValeur(26*16,4*16,303,-1,PLEIN,C_CLE);
            else setValeur(26*16,4*16,304,-1,PLEIN,C_RIEN);
            if(!gpJoueur->getCoffre(9,20)) setValeur(88*16,69*16,303,-1,PLEIN,C_CLE);
            else setValeur(88*16,69*16,304,-1,PLEIN,C_RIEN);
            if(!gpJoueur->getCoffre(9,21)) setValeur(150*16,54*16,303,-1,PLEIN,C_CLE);
            else setValeur(150*16,54*16,304,-1,PLEIN,C_RIEN);
            if(!gpJoueur->getCoffre(9,22)) setValeur(169*16,3*16,303,-1,PLEIN,C_CLE);
            else setValeur(169*16,3*16,304,-1,PLEIN,C_RIEN);
            if(!gpJoueur->getCoffre(9,23)) setValeur(109*16,66*16,303,-1,PLEIN,C_CLE);
            else setValeur(109*16,66*16,304,-1,PLEIN,C_RIEN);
            //baguette de glace
            if (!gpJoueur->hasObjet(O_BGLACE)) setValeur(49*16,52*16,303,-1,PLEIN,C_BGLACE);
            else setValeur(49*16,52*16,304,-1,PLEIN,C_RIEN);
            //porte boss
            if (gpJoueur->getCoffre(9,0)) passage(49,30,0);
            //portes
            if (gpJoueur->getCoffre(9,1)) passage(49,66,8);
            if (gpJoueur->getCoffre(9,2)) passage(69,6,8);
            if (gpJoueur->getCoffre(9,3)) passage(180,6,3);
            if (gpJoueur->getCoffre(9,4)) passage(189,15,4);
            if (gpJoueur->getCoffre(9,5)) passage(40,36,1);
            if (gpJoueur->getCoffre(9,6)) passage(40,66,1);
            if (gpJoueur->getCoffre(9,7)) passage(60,51,1);
            if (gpJoueur->getCoffre(9,8)) passage(140,66,1);
            if (gpJoueur->getCoffre(9,9)) passage(180,21,1);
            if (gpJoueur->getCoffre(9,10)) {passage(60,36,1); map[43][37]=1005;}
            if (gpJoueur->getCoffre(9,11)) {passage(89,60,0); map[83][67]=1005;}
            if (gpJoueur->getCoffre(9,12)) {passage(109,60,0); map[107][63]=1005;}
            if (gpJoueur->getCoffre(9,13)) {passage(120,51,1); contenu[124][47]=51;}
            if (gpJoueur->getCoffre(9,14)) {passage(129,45,0); map[127][39]=1005;}
            if (gpJoueur->getCoffre(9,15)) {passage(149,15,0); map[147][9]=1005;}
            if (gpJoueur->getCoffre(9,16)) {passage(160,66,1); map[164][72]=1005;}
            if (gpJoueur->getCoffre(9,17)) {passage(180,66,1); map[185][70]=1005;}
            if (gpJoueur->getCoffre(9,18)) {passage(240,6,1); map[233][13]=1005;}
            break;
        case 25 :
            //carte
            if(!gpJoueur->getCle(10,0)) setValeur(130*16,7*16,303,-1,PLEIN,C_CARTE);
            else setValeur(130*16,7*16,304,-1,PLEIN,C_RIEN);
            //boussole
            if(!gpJoueur->getCle(10,1)) setValeur(89*16,6*16,303,-1,PLEIN,C_BOUSSOLE);
            else setValeur(89*16,6*16,304,-1,PLEIN,C_RIEN);
            //cle boss
            if(!gpJoueur->getCle(10,2)) setValeur(208*16,112*16,303,-1,PLEIN,C_CLE_BOSS);
            else setValeur(208*16,112*16,304,-1,PLEIN,C_RIEN);
            //cles
            if(!gpJoueur->getCoffre(10,15)) setValeur(126*16,52*16,303,-1,PLEIN,C_CLE);
            else setValeur(126*16,52*16,304,-1,PLEIN,C_RIEN);
            if(!gpJoueur->getCoffre(10,16)) setValeur(132*16,52*16,303,-1,PLEIN,C_CLE);
            else setValeur(132*16,52*16,304,-1,PLEIN,C_RIEN);
            if(!gpJoueur->getCoffre(10,17)) setValeur(190*16,67*16,303,-1,PLEIN,C_CLE);
            else setValeur(190*16,67*16,304,-1,PLEIN,C_RIEN);
            if(!gpJoueur->getCoffre(10,18)) setValeur(189*16,95*16,303,-1,PLEIN,C_CLE);
            else setValeur(189*16,95*16,304,-1,PLEIN,C_RIEN);
            if(!gpJoueur->getCoffre(10,19)) setValeur(230*16,97*16,303,-1,PLEIN,C_CLE);
            else setValeur(230*16,97*16,304,-1,PLEIN,C_RIEN);
            //bouclier 3
            if (gpJoueur->getBouclier()<3) setValeur(129*16,21*16,303,-1,PLEIN,C_BOUCLIER_J);
            else setValeur(129*16,21*16,304,-1,PLEIN,C_RIEN);
            //porte boss
            if (gpJoueur->getCoffre(10,0)) passage(49,90,0);
            //portes
            if (gpJoueur->getCoffre(10,1)) passage(40,21,3);
            if (gpJoueur->getCoffre(10,2)) passage(49,30,4);
            if (gpJoueur->getCoffre(10,3)) passage(109,15,4);
            if (gpJoueur->getCoffre(10,4)) passage(129,30,4);
            if (gpJoueur->getCoffre(10,5)) passage(9,45,0);
            if (gpJoueur->getCoffre(10,6)) passage(20,96,1);
            if (gpJoueur->getCoffre(10,7)) passage(29,90,0);
            if (gpJoueur->getCoffre(10,8)) passage(29,45,0);
            if (gpJoueur->getCoffre(10,9)) passage(89,45,0);
            if (gpJoueur->getCoffre(10,10)) {passage(69,30,0); map[67][26]=1005;}
            if (gpJoueur->getCoffre(10,11)) {passage(69,90,0); contenu[77][85]=51;}
            if (gpJoueur->getCoffre(10,12)) {passage(69,105,0); passage(60,96,1); map[77][97]=1005;}
            if (gpJoueur->getCoffre(10,13)) {passage(89,90,0); map[96][97]=1005;}
            if (gpJoueur->getCoffre(10,14)) {passage(129,75,0); map[123][67]=1005;}
            break;
        case 26 :
            //carte
            if(!gpJoueur->getCle(11,0)) setValeur(70*16,10*16,303,-1,PLEIN,C_CARTE);
            else setValeur(70*16,10*16,304,-1,PLEIN,C_RIEN);
            //boussole
            if(!gpJoueur->getCle(11,1)) setValeur(70*16,52*16,303,-1,PLEIN,C_BOUSSOLE);
            else setValeur(70*16,52*16,304,-1,PLEIN,C_RIEN);
            //cle boss
            if(!gpJoueur->getCle(11,2)) setValeur(289*16,19*16,303,-1,PLEIN,C_CLE_BOSS);
            else setValeur(289*16,19*16,304,-1,PLEIN,C_RIEN);
            //cles
            if(!gpJoueur->getCoffre(11,19)) setValeur(9*16,51*16,303,-1,PLEIN,C_CLE);
            else setValeur(9*16,51*16,304,-1,PLEIN,C_RIEN);
            if(!gpJoueur->getCoffre(11,20)) setValeur(69*16,2*16,303,-1,PLEIN,C_CLE);
            else setValeur(69*16,2*16,304,-1,PLEIN,C_RIEN);
            if(!gpJoueur->getCoffre(11,21)) setValeur(69*16,77*16,303,-1,PLEIN,C_CLE);
            else setValeur(69*16,77*16,304,-1,PLEIN,C_RIEN);
            if(!gpJoueur->getCoffre(11,22)) setValeur(130*16,51*16,303,-1,PLEIN,C_CLE);
            else setValeur(130*16,51*16,304,-1,PLEIN,C_RIEN);
            if(!gpJoueur->getCoffre(11,23)) setValeur(330*16,19*16,303,-1,PLEIN,C_CLE);
            else setValeur(330*16,19*16,304,-1,PLEIN,C_RIEN);
            //tunique rouge
            if (!gpJoueur->getCoffre(11,25)) {
                if (gpJoueur->getTunique()==1)
                    setValeur(69*16,64*16,303,-1,PLEIN,C_TUNIQUE_BLEUE);
                else
                    setValeur(69*16,64*16,303,-1,PLEIN,C_TUNIQUE_ROUGE);
            }
            else setValeur(69*16,64*16,304,-1,PLEIN,C_RIEN);
            //bottes
            if (!gpJoueur->hasObjet(O_BOTTES)) setValeur(304*16,49*16,303,-1,PLEIN,C_BOTTES);
            //porte boss
            if (gpJoueur->getCoffre(11,0)) passage(309,45,0);
            //portes
            if (gpJoueur->getCoffre(11,1)) passage(80,66,3);
            if (gpJoueur->getCoffre(11,2)) {passage(9,60,0); map[16][67]=1005;}
            if (gpJoueur->getCoffre(11,3)) {passage(29,60,0); map[36][52]=1005;}
            if (gpJoueur->getCoffre(11,4)) {passage(49,90,0); map[55][85]=1005;}
            if (gpJoueur->getCoffre(11,5)) {passage(60,66,1); map[76][67]=1005;}
            if (gpJoueur->getCoffre(11,6)) {passage(60,21,1); map[52][18]=1005;}
            if (gpJoueur->getCoffre(11,7)) {passage(69,30,0); map[63][22]=1005;}
            if (gpJoueur->getCoffre(11,8)) {passage(69,15,0); map[76][22]=1005;}
            if (gpJoueur->getCoffre(11,9)) {passage(80,21,1); map[87][26]=1005;}
            if (gpJoueur->getCoffre(11,10)) {passage(89,60,0); map[87][71]=1005;}
            if (gpJoueur->getCoffre(11,11)) {passage(89,90,0); map[96][82]=1005;}
            if (gpJoueur->getCoffre(11,12)) {passage(109,60,0); map[103][52]=1005;}
            if (gpJoueur->getCoffre(11,13)) {passage(120,21,1); map[112][26]=1005;}
            if (gpJoueur->getCoffre(11,14)) passage(29,45,0);
            if (gpJoueur->getCoffre(11,15)) passage(60,96,1);
            if (gpJoueur->getCoffre(11,16)) passage(80,96,1);
            if (gpJoueur->getCoffre(11,17)) passage(109,45,0);
            if (gpJoueur->getCoffre(11,18)) passage(289,30,0);
            break;
        case 27 :
            //carte
            if(!gpJoueur->getCle(12,0)) setValeur(170*16,67*16,303,-1,PLEIN,C_CARTE);
            else setValeur(170*16,67*16,304,-1,PLEIN,C_RIEN);
            //boussole
            if(!gpJoueur->getCle(12,1)) setValeur(149*16,52*16,303,-1,PLEIN,C_BOUSSOLE);
            else setValeur(149*16,52*16,304,-1,PLEIN,C_RIEN);
            //cle boss
            if(!gpJoueur->getCle(12,2)) setValeur(9*16,6*16,303,-1,PLEIN,C_CLE_BOSS);
            else setValeur(9*16,6*16,304,-1,PLEIN,C_RIEN);
            //cles
            if(!gpJoueur->getCoffre(12,12)) setValeur(9*16,66*16,303,-1,PLEIN,C_CLE);
            else setValeur(9*16,66*16,304,-1,PLEIN,C_RIEN);
            if(!gpJoueur->getCoffre(12,13)) setValeur(9*16,126*16,303,-1,PLEIN,C_CLE);
            else setValeur(9*16,126*16,304,-1,PLEIN,C_RIEN);
            if(!gpJoueur->getCoffre(12,14)) setValeur(29*16,82*16,303,-1,PLEIN,C_CLE);
            else setValeur(29*16,82*16,304,-1,PLEIN,C_RIEN);
            if(!gpJoueur->getCoffre(12,15)) setValeur(149*16,82*16,303,-1,PLEIN,C_CLE);
            else setValeur(149*16,82*16,304,-1,PLEIN,C_RIEN);
            //arc 2
            if (gpJoueur->hasObjet(O_ARC)!=5) setValeur(29*16,36*16,303,-1,PLEIN,C_ARC_FEE);
            else setValeur(29*16,36*16,304,-1,PLEIN,C_RIEN);
            //porte boss
            if (gpJoueur->getCoffre(12,0)) passage(29,60,0);
            //portes
            if (gpJoueur->getCoffre(12,1)) passage(109,60,4);
            if (gpJoueur->getCoffre(12,2)) passage(149,60,4);
            if (gpJoueur->getCoffre(12,3)) passage(149,75,4);
            if (gpJoueur->getCoffre(12,4)) passage(40,126,1);
            if (gpJoueur->getCoffre(12,5)) passage(89,105,0);
            if (gpJoueur->getCoffre(12,6)) passage(109,45,0);
            if (gpJoueur->getCoffre(12,7)) passage(160,66,1);
            if (gpJoueur->getCoffre(12,8)) {passage(29,75,0); map[26][71]=1005;}
            if (gpJoueur->getCoffre(12,9)) {passage(29,120,0); map[26][116]=1005;}
            if (gpJoueur->getCoffre(12,10)) {passage(40,66,1); map[43][71]=1005;}
            if (gpJoueur->getCoffre(12,11)) {passage(40,111,1); map[56][108]=1005;}
            break;
        case 28 :
            //carte
            if(!gpJoueur->getCle(13,0)) setValeur(29*16,112*16,303,-1,PLEIN,C_CARTE);
            else setValeur(29*16,112*16,304,-1,PLEIN,C_RIEN);
            //boussole
            if(!gpJoueur->getCle(13,1)) setValeur(109*16,37*16,303,-1,PLEIN,C_BOUSSOLE);
            else setValeur(109*16,37*16,304,-1,PLEIN,C_RIEN);
            //cle boss
            if(!gpJoueur->getCle(13,2)) setValeur(149*16,36*16,303,-1,PLEIN,C_CLE_BOSS);
            else setValeur(149*16,36*16,304,-1,PLEIN,C_RIEN);
            //cles
            if(!gpJoueur->getCoffre(13,7)) setValeur(70*16,112*16,303,-1,PLEIN,C_CLE);
            else setValeur(70*16,112*16,304,-1,PLEIN,C_RIEN);
            if(!gpJoueur->getCoffre(13,9)) setValeur(190*16,37*16,303,-1,PLEIN,C_CLE);
            else setValeur(190*16,37*16,304,-1,PLEIN,C_RIEN);
            //porte boss
            if (gpJoueur->getCoffre(13,0)) passage(49,135,0);
            //portes
            if (gpJoueur->getCoffre(13,1)) {passage(60,66,1); map[72][69]=1005;}
            if (gpJoueur->getCoffre(13,2)) {passage(60,141,1); map[69][146]=1005;}
            if (gpJoueur->getCoffre(13,3)) {passage(160,51,1); map[163][55]=1005;}
            if (gpJoueur->getCoffre(13,4)) passage(49,60,0);
            if (gpJoueur->getCoffre(13,5)) passage(80,96,1);
            if (gpJoueur->getCoffre(13,6)) passage(140,66,1);
            break;
        case 29 :
            if (!gpJoueur->getBouclier()) setValeur(9*16, 37*16, 303, -1, PLEIN, C_BOUCLIER);
            else setValeur(9*16, 37*16, 304, -1,PLEIN,C_RIEN);
            break;
        case 32 :
            if(!gpJoueur->hasObjet(O_OCARINA)) setValeur(9*16,5*16,303,-1,PLEIN,C_OCARINA);
            else setValeur(9*16,5*16,304,-1,PLEIN,C_RIEN);
            break;
        case 35 :
            if (!gpJoueur->hasObjet(O_LANTERNE)) setValeur(9*16,5*16,303,-1,PLEIN,C_LANTERNE);
            else setValeur(9*16,5*16,304,-1,PLEIN,C_RIEN);
            break;
        case 36 :
            if (!gpJoueur->hasObjet(O_MASQUE)) setValeur(9*16,5*16,303,-1,PLEIN,C_MASQUE);
            else setValeur(9*16,5*16,304,-1,PLEIN,C_RIEN);
            break;
        case 41 :
            if(!gpJoueur->hasBouteille(2)) setValeur(9*16,19*16,303,-1,PLEIN,C_BOUTEILLE);
            else setValeur(9*16,19*16,304,-1,PLEIN,C_RIEN);
            break;
        case 42 :
            if(gpJoueur->hasObjet(O_GANTS)!=2) setValeur(9*16,5*16,303,-1,PLEIN,C_GANTS_R);
            else setValeur(9*16,5*16,304,-1,PLEIN,C_RIEN);
            break;
        case 45 :
            if (gpJoueur->getEnnemi(19) && gpJoueur->getEnnemi(24) && gpJoueur->getEnnemi(35)) 
                passage(29,0,0);
            break;
        case 53 :
            if (!gpJoueur->hasBouteille(1)) setValeur(4*16,2*16,303,-1, PLEIN, C_BOUTEILLE);
            else setValeur(4*16,2*16,304,-1,PLEIN,C_RIEN);
            break;
    }
}

void Monde::initMatrice(int zone) {
    Joueur* gpJoueur = gpJeu->getJoueur();
    
    int monstre1=0, monstre2=0, monstre3=0;
    if (!gpJoueur->getOnilink()) {
        if (zone==12 || zone==5 || zone==6 || zone==4 || zone==10 || zone==2 || zone==9) {
            if (!gpJoueur->getEnnemi(19)) monstre1=1+(rand()%14);
            if (!gpJoueur->getEnnemi(24)) monstre2=1+(rand()%14); 
            if (monstre2==monstre1) monstre2=0;
            if (!gpJoueur->getEnnemi(35)) monstre3=1+(rand()%14); 
            if (monstre3==monstre1 || monstre3==monstre2) monstre3=0;
        }
    }
    
    switch(zone) {
        case 1 :
            if (!gpJoueur->getCoeur(13)) gpJeu->ajouteObjet(I_QUART_COEUR,9*16+8,4*16+8,13);
            if (!gpJoueur->getCoeur(14)) gpJeu->ajouteObjet(I_QUART_COEUR,76*16+8,56*16+8,14);
            if (!gpJoueur->getAvancement()) gpJeu->ajoutePnj(47,29*16+8,15*16,172);
            gpJeu->ajouteEnnemi(4,29*16+8,29*16); gpJeu->ajouteEnnemi(4,13*16,52*16);
            gpJeu->ajouteEnnemi(3,56*16,45*16); gpJeu->ajouteEnnemi(3,69*16,24*16);
            gpJeu->ajouteEnnemi(3,58*16,22*16);
            break;
        case 2 :
            if (!gpJoueur->getCoeur(15)) gpJeu->ajouteObjet(I_QUART_COEUR,115*16+8,32*16,15);
            if (!gpJoueur->getCoeur(16)) gpJeu->ajouteObjet(I_QUART_COEUR,14*16+8,22*16+8,16);
            gpJeu->ajouteEnnemi(53,108*16,45*16); gpJeu->ajouteEnnemi(53,70*16,50*16);
            gpJeu->ajouteEnnemi(53,70*16,22*16); gpJeu->ajouteEnnemi(53,20*16,38*16);
            gpJeu->ajouteEnnemi(53,11*16,9*16); gpJeu->ajouteEnnemi(53,70*16,6*16);
            gpJeu->ajouteEnnemi(53,35*16,15*16); gpJeu->ajouteEnnemi(53,105*16,6*16);
            gpJeu->ajouteEnnemi(54,114*16,12*16); gpJeu->ajouteEnnemi(54,114*16,25*16);
            gpJeu->ajouteEnnemi(54,112*16,36*16);
            gpJeu->ajouteEnnemi(8,21*16,9*16);
            if (monstre1==zone) gpJeu->ajouteEnnemi(19,50*16,49*16);
            if (monstre2==zone) gpJeu->ajouteEnnemi(24,50*16,49*16);
            if (monstre3==zone) gpJeu->ajouteEnnemi(35,50*16,49*16);
            break;
        case 3 :
            if (!gpJoueur->getCoeur(17)) gpJeu->ajouteObjet(I_QUART_COEUR,13*16+8,9*16+8,17);
            if (!gpJoueur->hasBouteille(0)) gpJeu->ajoutePnj(7,16*16,21*16,41);
            else gpJeu->ajoutePnj(7,16*16,21*16,40);
            gpJeu->ajoutePnj(13,22*16,12*16,49);
            gpJeu->ajoutePnj(14,60*16,12*16,47);
            if (gpJoueur->getAvancement()<9) gpJeu->ajoutePnj(33,72*16,16*16,50);
            else gpJeu->ajoutePnj(33,69*16+8,15*16+8,50);
            gpJeu->ajoutePnj(22,34*16+8,45*16+8,54);
            gpJeu->ajoutePnj(23,24*16+8,33*16+8,55);
            gpJeu->ajoutePnj(1,50*16+8,32*16+8,56);
            break;
        case 4 :
            if (!gpJoueur->getCoeur(18)) gpJeu->ajouteObjet(I_QUART_COEUR,63*16+8,5*16,18);
            gpJeu->ajouteEnnemi(1,28*16,33*16); gpJeu->ajouteEnnemi(1,40*16,34*16);
            gpJeu->ajouteEnnemi(1,52*16,29*16); gpJeu->ajouteEnnemi(1,59*16,35*16);
            gpJeu->ajouteEnnemi(1,50*16,41*16); gpJeu->ajouteEnnemi(1,30*16,41*16);
            gpJeu->ajouteEnnemi(1,19*16,35*16); gpJeu->ajouteEnnemi(1,12*16,25*16);
            gpJeu->ajouteEnnemi(1,12*16,40*16); gpJeu->ajouteEnnemi(1,30*16,47*16);
            gpJeu->ajouteEnnemi(1,50*16,47*16); gpJeu->ajouteEnnemi(1,65*16,40*16);
            gpJeu->ajouteEnnemi(1,65*16,25*16); gpJeu->ajouteEnnemi(1,40*16,15*16);
            gpJeu->ajouteEnnemi(1,25*16,15*16); gpJeu->ajouteEnnemi(54,12*16,8*16);
            gpJeu->ajouteEnnemi(54,7*16,18*16); gpJeu->ajouteEnnemi(54,7*16,47*16);
            gpJeu->ajouteEnnemi(54,19*16,53*16); gpJeu->ajouteEnnemi(54,33*16,53*16);
            gpJeu->ajouteEnnemi(54,47*16,53*16); gpJeu->ajouteEnnemi(54,26*16,8*16);
            gpJeu->ajouteEnnemi(54,40*16,8*16); gpJeu->ajouteEnnemi(54,65*16,15*16);
            gpJeu->ajouteEnnemi(54,71*16,29*16); gpJeu->ajouteEnnemi(54,71*16,43*16);
            if (monstre1==zone) gpJeu->ajouteEnnemi(19,23*16,32*16);
            if (monstre2==zone) gpJeu->ajouteEnnemi(24,23*16,32*16);
            if (monstre3==zone) gpJeu->ajouteEnnemi(35,23*16,32*16);
            break;
        case 5 :
            if (!gpJoueur->getCoeur(19)) gpJeu->ajouteObjet(I_QUART_COEUR,189*16,18*16,19);
            if (!gpJoueur->getCoeur(20)) gpJeu->ajouteObjet(I_QUART_COEUR,106*16,73*16+8,20);
            gpJeu->ajouteEnnemi(15,17*16,34*16); gpJeu->ajouteEnnemi(15,25*16,34*16);
            gpJeu->ajouteEnnemi(15,74*16,36*16); gpJeu->ajouteEnnemi(15,89*16,55*16);
            gpJeu->ajouteEnnemi(15,157*16,23*16); gpJeu->ajouteEnnemi(15,135*16,60*16);
            gpJeu->ajouteEnnemi(14,169*16,54*16); gpJeu->ajouteEnnemi(14,67*16,17*16);
            gpJeu->ajouteEnnemi(14,91*16,18*16); gpJeu->ajouteEnnemi(14,112*16,27*16);
            gpJeu->ajouteEnnemi(14,125*16,13*16); gpJeu->ajouteEnnemi(14,134*16,34*16);
            gpJeu->ajouteEnnemi(14,154*16,51*16); gpJeu->ajouteEnnemi(14,179*16,29*16);
            gpJeu->ajouteEnnemi(14,168*16,43*16); gpJeu->ajouteEnnemi(14,175*16,57*16);
            gpJeu->ajouteEnnemi(14,150*16,69*16); gpJeu->ajouteEnnemi(14,122*16,55*16);
            gpJeu->ajouteEnnemi(14,103*16,50*16); gpJeu->ajouteEnnemi(14,123*16,38*16);
            gpJeu->ajouteEnnemi(14,82*16,32*16); gpJeu->ajouteEnnemi(13,6*16-9,24*16-4);
            gpJeu->ajouteEnnemi(13,37*16-9,24*16-4); gpJeu->ajouteEnnemi(13,68*16-9,40*16-4);
            gpJeu->ajouteEnnemi(13,69*16-9,29*16-4); gpJeu->ajouteEnnemi(13,79*16-9,49*16-4);
            gpJeu->ajouteEnnemi(13,93*16-9,32*16-4);
            gpJeu->ajouteEnnemi(13,104*16-9,10*16-4); gpJeu->ajouteEnnemi(13,122*16-9,63*16-4);
            gpJeu->ajouteEnnemi(13,135*16-9,48*16-4); gpJeu->ajouteEnnemi(13,139*16-9,69*16-4);
            gpJeu->ajouteEnnemi(13,154*16-9,16*16-4); gpJeu->ajouteEnnemi(13,175*16-9,26*16-4);
            gpJeu->ajouteEnnemi(13,177*16-9,42*16-4); gpJeu->ajouteEnnemi(13,193*16-9,49*16-4);
            gpJeu->ajouteEnnemi(13,105*16-9,70*16-4); gpJeu->ajouteEnnemi(13,70*16-9,70*16-4);
            gpJeu->ajouteEnnemi(7,189*16,14*16); gpJeu->ajouteEnnemi(7,160*16,61*16);
            gpJeu->ajouteEnnemi(7,139*16,45*16); gpJeu->ajouteEnnemi(7,130*16,75*16);
            gpJeu->ajouteEnnemi(7,104*16,61*16); gpJeu->ajouteEnnemi(7,80*16,43*16);
            gpJeu->ajouteEnnemi(7,73*16,25*16); gpJeu->ajouteEnnemi(7,100*16,28*16);
            gpJeu->ajouteEnnemi(7,122*16,20*16); gpJeu->ajouteEnnemi(7,95*16,40*16);
            gpJeu->ajouteEnnemi(7,60*16,42*16); gpJeu->ajouteEnnemi(7,30*16,52*16);
            gpJeu->ajouteEnnemi(7,4*16,60*16); gpJeu->ajouteEnnemi(7,16*16,86*16);
            gpJeu->ajouteEnnemi(7,45*16,87*16);
            if (monstre1==zone) gpJeu->ajouteEnnemi(19,157*16,32*16);
            if (monstre2==zone) gpJeu->ajouteEnnemi(24,157*16,32*16);
            if (monstre3==zone) gpJeu->ajouteEnnemi(35,157*16,32*16);
            break;
        case 6 :
            if (!gpJoueur->getCoeur(21)) gpJeu->ajouteObjet(I_QUART_COEUR,89*16+8,28*16,21);
            gpJeu->ajouteEnnemi(10,9*16,28*16); gpJeu->ajouteEnnemi(10,15*16,12*16); gpJeu->ajouteEnnemi(10,23*16,40*16);
            gpJeu->ajouteEnnemi(10,48*16,18*16); gpJeu->ajouteEnnemi(10,65*16,40*16); gpJeu->ajouteEnnemi(10,70*16,25*16);
            gpJeu->ajouteEnnemi(27,41*16,32*16); gpJeu->ajouteEnnemi(27,52*16,32*16);
            gpJeu->ajouteEnnemi(9,46*16+8,46*16); gpJeu->ajouteEnnemi(9,85*16,54*16); gpJeu->ajouteEnnemi(9,103*16,46*16);
            gpJeu->ajouteEnnemi(9,110*16,41*16); gpJeu->ajouteEnnemi(9,50*16,7*16); gpJeu->ajouteEnnemi(9,66*16,8*16);
            gpJeu->ajouteEnnemi(9,80*16,6*16); gpJeu->ajouteEnnemi(9,94*16,7*16); gpJeu->ajouteEnnemi(9,32*16,54*16);
            gpJeu->ajouteEnnemi(9,51*16,55*16);
            if (monstre1==zone) gpJeu->ajouteEnnemi(19,44*16,50*16);
            if (monstre2==zone) gpJeu->ajouteEnnemi(24,44*16,50*16);
            if (monstre3==zone) gpJeu->ajouteEnnemi(35,44*16,50*16);
            break;
        case 7 : //avancement = 15 quand poussé, 17 quand sauvetage fini
            if (gpJoueur->getAvancement() >= 14) 
                gpJeu->ajoutePnj(46,13*16+8,12*16+8,154);
            else gpJeu->ajoutePnj(46,16*16-3,11*16+8+3,154);
            if (gpJeu->getJoueur()->getOnilink() || gpJeu->getJoueur()->getAvancement()!=64) {
                gpJeu->ajoutePnj(2,34*16+8,35*16,76);
                gpJeu->ajoutePnj(10,23*16+8,34*16,79);
                gpJeu->ajoutePnj(9,68*16+8,20*16,80);
                gpJeu->ajoutePnj(11,42*16,27*16,81);
                if (gpJoueur->hasObjet(O_OCARINA)) gpJeu->ajoutePnj(34,45*16+8,30*16+8,83);
                else gpJeu->ajoutePnj(34,45*16+8,30*16+8,82);
                gpJeu->ajoutePnj(35,27*16,14*16,84);
                gpJeu->ajoutePnj(36,61*16,39*16,85);
                gpJeu->ajouteEnnemi(52,15*16,34*16); gpJeu->ajouteEnnemi(52,16*16,36*16);
                gpJeu->ajouteEnnemi(52,18*16,34*16); gpJeu->ajouteEnnemi(52,20*16,36*16);
                gpJeu->ajouteEnnemi(52,22*16,34*16);
            }
            //switch
            gpJeu->ajouteEnnemi(20,5*16,45*16); setValeur(5*16,45*16,0,2);
            break;
        case 8 :
            gpJeu->ajouteEnnemi(8,10*16,31*16); gpJeu->ajouteEnnemi(8,17*16,35*16);
            gpJeu->ajouteEnnemi(8,19*16,46*16); gpJeu->ajouteEnnemi(8,27*16,47*16);
            gpJeu->ajouteEnnemi(8,31*16,31*16); gpJeu->ajouteEnnemi(8,21*16,14*16);
            gpJeu->ajouteEnnemi(54,1*16,35*16); gpJeu->ajouteEnnemi(54,4*16,47*16); 
            gpJeu->ajouteEnnemi(54,21*16,51*16); gpJeu->ajouteEnnemi(54,32*16,51*16);
            break;
        case 9 :
            if (!gpJoueur->getCoeur(22)) gpJeu->ajouteObjet(I_QUART_COEUR,65*16+8,30*16,22);
            if (!gpJoueur->getCoeur(23)) gpJeu->ajouteObjet(I_QUART_COEUR,44*16,80*16,23);
            if (!gpJoueur->getCoeur(24)) gpJeu->ajouteObjet(I_QUART_COEUR,18*16,115*16,24);
            //armée de ganon
            if (gpJoueur->getAvancement()==67) {
                gpJeu->getAudio()->playSpecial(1);
                gpJeu->getStatut()->setJauge(true);
                gpJeu->ajouteEnnemi(46,65*16-18,30*16-19);gpJeu->ajouteEnnemi(46,60*16-18,41*16-19);
                gpJeu->ajouteEnnemi(46,27*16-18,72*16-19);gpJeu->ajouteEnnemi(46,23*16-18,82*16-19);
                gpJeu->ajouteEnnemi(46,28*16-18,95*16-19);gpJeu->ajouteEnnemi(46,42*16-18,101*16-19);
                gpJeu->ajouteEnnemi(46,55*16-18,98*16-19);gpJeu->ajouteEnnemi(46,66*16-18,87*16-19);
                gpJeu->ajouteEnnemi(46,53*16-18,81*16-19);gpJeu->ajouteEnnemi(46,41*16-18,74*16-19);
                gpJeu->ajouteEnnemi(46,67*16-18,72*16-19);gpJeu->ajouteEnnemi(46,59*16-18,60*16-19);
                gpJeu->ajouteEnnemi(47,21*16-18,53*16-19);gpJeu->ajouteEnnemi(47,32*16-18,53*16-19);
                gpJeu->ajouteEnnemi(47,18*16-18,42*16-19);gpJeu->ajouteEnnemi(47,9*16-18,36*16-19);
                gpJeu->ajouteEnnemi(47,15*16-18,16*16-19);gpJeu->ajouteEnnemi(47,34*16-18,11*16-19);
                gpJeu->ajouteEnnemi(47,14*16-18,25*16-19);
                gpJeu->ajouteEnnemi(48,52*16-7,9*16-3);
            }else{
                gpJeu->ajouteEnnemi(55,46*16,9*16);gpJeu->ajouteEnnemi(55,14*16,10*16);
                gpJeu->ajouteEnnemi(55,69*16,26*16);gpJeu->ajouteEnnemi(55,12*16,34*16);
                gpJeu->ajouteEnnemi(55,21*16,53*16);gpJeu->ajouteEnnemi(55,32*16,53*16);
                gpJeu->ajouteEnnemi(55,61*16,53*16);gpJeu->ajouteEnnemi(55,40*16,71*16);
                gpJeu->ajouteEnnemi(55,59*16,81*16);gpJeu->ajouteEnnemi(55,26*16,91*16);
                gpJeu->ajouteEnnemi(55,43*16,98*16);
                gpJeu->ajouteEnnemi(54,12*16,3*16);gpJeu->ajouteEnnemi(54,30*16,3*16);
                gpJeu->ajouteEnnemi(54,48*16,3*16);gpJeu->ajouteEnnemi(54,76*16,24*16);
                gpJeu->ajouteEnnemi(54,67*16,40*16);gpJeu->ajouteEnnemi(54,67*16,60*16);
                gpJeu->ajouteEnnemi(54,73*16,77*16);gpJeu->ajouteEnnemi(54,73*16,98*16);
                gpJeu->ajouteEnnemi(54,7*16,53*16);gpJeu->ajouteEnnemi(54,34*16,79*16);
                gpJeu->ajouteEnnemi(54,45*16,84*16);
                if (monstre1==zone) gpJeu->ajouteEnnemi(19,50*16,92*16);
                if (monstre2==zone) gpJeu->ajouteEnnemi(24,50*16,92*16);
                if (monstre3==zone) gpJeu->ajouteEnnemi(35,50*16,92*16);
            }
            break;
        case 10 :
            if (!gpJoueur->getCoeur(25)) gpJeu->ajouteObjet(I_QUART_COEUR,66*16,7*16,25);
            gpJeu->ajouteEnnemi(12,11*16,19*16);gpJeu->ajouteEnnemi(12,11*16,53*16);
            gpJeu->ajouteEnnemi(12,20*16,7*16);gpJeu->ajouteEnnemi(12,30*16,21*16);
            gpJeu->ajouteEnnemi(12,38*16,55*16);gpJeu->ajouteEnnemi(12,48*16,26*16);
            gpJeu->ajouteEnnemi(12,12*16,36*16);gpJeu->ajouteEnnemi(12,53*16,36*16);
            gpJeu->ajouteEnnemi(12,58*16,5*16);gpJeu->ajouteEnnemi(12,60*16,19*16);
            gpJeu->ajouteEnnemi(12,74*16,33*16);gpJeu->ajouteEnnemi(53,8*16,26*16);
            gpJeu->ajouteEnnemi(53,11*16,43*16);gpJeu->ajouteEnnemi(53,16*16,16*16);
            gpJeu->ajouteEnnemi(53,25*16,55*16);gpJeu->ajouteEnnemi(53,27*16,34*16);
            gpJeu->ajouteEnnemi(53,35*16,44*16);gpJeu->ajouteEnnemi(53,51*16,52*16);
            gpJeu->ajouteEnnemi(53,70*16,44*16);
            if (monstre1==zone) gpJeu->ajouteEnnemi(19,60*16,24*16);
            if (monstre2==zone) gpJeu->ajouteEnnemi(24,60*16,24*16);
            if (monstre3==zone) gpJeu->ajouteEnnemi(35,60*16,24*16);
            break;
        case 11 :
            gpJeu->ajoutePnj(3,20*16+8,17*16,106);
            gpJeu->ajoutePnj(4,51*16+8,16*16,107);
            gpJeu->ajoutePnj(5,35*16+8,27*16,108);
            gpJeu->ajoutePnj(6,56*16+8,34*16,109);
            break;
        case 12 :
            gpJeu->ajouteEnnemi(11,10*16,18*16);gpJeu->ajouteEnnemi(11,15*16,50*16);
            gpJeu->ajouteEnnemi(11,58*16,38*16);gpJeu->ajouteEnnemi(11,60*16,12*16);
            gpJeu->ajouteEnnemi(56,7*16,10*16);gpJeu->ajouteEnnemi(56,7*16,16*16);
            gpJeu->ajouteEnnemi(56,7*16,47*16);gpJeu->ajouteEnnemi(56,20*16,10*16);
            gpJeu->ajouteEnnemi(56,28*16,39*16);gpJeu->ajouteEnnemi(56,33*16,10*16);
            gpJeu->ajouteEnnemi(56,33*16,24*16);gpJeu->ajouteEnnemi(56,46*16,24*16);
            gpJeu->ajouteEnnemi(56,58*16,16*16);gpJeu->ajouteEnnemi(56,49*16,42*16);
            gpJeu->ajouteEnnemi(56,62*16,31*16);gpJeu->ajouteEnnemi(56,69*16,16*16);
            if (monstre1==zone) gpJeu->ajouteEnnemi(19,55*16,43*16);
            if (monstre2==zone) gpJeu->ajouteEnnemi(24,55*16,43*16);
            if (monstre3==zone) gpJeu->ajouteEnnemi(35,55*16,43*16);
            break;
        case 13 :
            gpJeu->ajoutePnj(37,42*16,32*16,116);
            gpJeu->ajoutePnj(38,58*16,30*16,117);
            gpJeu->ajoutePnj(39,26*16+4,33*16,118);
            break;
        case 14 :
            if (!gpJoueur->getCoeur(26)) gpJeu->ajouteObjet(I_QUART_COEUR,82*16,64*16,26);
            gpJeu->ajouteEnnemi(6,19*16,27*16);gpJeu->ajouteEnnemi(6,22*16,27*16);
            gpJeu->ajouteEnnemi(6,58*16,41*16);gpJeu->ajouteEnnemi(6,62*16,41*16);
            gpJeu->ajouteEnnemi(6,66*16,77*16);gpJeu->ajouteEnnemi(6,80*16,63*16);
            gpJeu->ajouteEnnemi(6,84*16,63*16);gpJeu->ajouteEnnemi(6,94*16,58*16);
            gpJeu->ajouteEnnemi(6,95*16,45*16);gpJeu->ajouteEnnemi(6,110*16,74*16);
            gpJeu->ajouteEnnemi(6,133*16,78*16-8);gpJeu->ajouteEnnemi(6,148*16,76*16);
            gpJeu->ajouteEnnemi(6,90*16,80*16);
            gpJeu->ajouteEnnemi(5,63*16,54*16);gpJeu->ajouteEnnemi(5,64*16,69*16);
            gpJeu->ajouteEnnemi(5,83*16,52*16);gpJeu->ajouteEnnemi(5,91*16,41*16);
            gpJeu->ajouteEnnemi(5,96*16,53*16);gpJeu->ajouteEnnemi(5,103*16,67*16);
            gpJeu->ajouteEnnemi(5,112*16,79*16);gpJeu->ajouteEnnemi(5,126*16,60*16);
            gpJeu->ajouteEnnemi(5,139*16,70*16);gpJeu->ajouteEnnemi(5,143*16,62*16);
            gpJeu->ajouteEnnemi(5,144*16,79*16);gpJeu->ajouteEnnemi(5,153*16,72*16);
            gpJeu->ajouteEnnemi(44,6*16,57*16);gpJeu->ajouteEnnemi(44,17*16,81*16);
            gpJeu->ajouteEnnemi(44,29*16,72*16);gpJeu->ajouteEnnemi(44,33*16,61*16);
            break;
        case 15 :
            //ennemi
            gpJeu->ajouteEnnemi(18,2*16,17*16); gpJeu->ajouteEnnemi(18,17*16,17*16); gpJeu->ajouteEnnemi(18,186*16,29*16);
            gpJeu->ajouteEnnemi(18,193*16,29*16); gpJeu->ajouteEnnemi(18,166*16,19*16); gpJeu->ajouteEnnemi(18,166*16,24*16);
            gpJeu->ajouteEnnemi(4,17*16,52*16); gpJeu->ajouteEnnemi(4,57*16,7*16); gpJeu->ajouteEnnemi(4,70*16,53*16);
            gpJeu->ajouteEnnemi(4,126*16,32*16); gpJeu->ajouteEnnemi(4,133*16,32*16);
            gpJeu->ajouteEnnemi(1,109*16,23*16); gpJeu->ajouteEnnemi(1,139*16,47*16); gpJeu->ajouteEnnemi(1,152*16,7*16-8);
            gpJeu->ajouteEnnemi(1,190*16-8,22*16);
            //pièges
            gpJeu->ajouteEnnemi(17,38*16,25*16); gpJeu->ajouteEnnemi(17,50*16,25*16); gpJeu->ajouteEnnemi(17,51*16,37*16);
            gpJeu->ajouteEnnemi(17,64*16,36*16); gpJeu->ajouteEnnemi(17,55*16,5*16); gpJeu->ajouteEnnemi(17,132*16,51*16);
            gpJeu->ajouteEnnemi(17,134*16,51*16); gpJeu->ajouteEnnemi(17,136*16,51*16); gpJeu->ajouteEnnemi(17,138*16,51*16);
            gpJeu->ajouteEnnemi(17,138*16,39*16); gpJeu->ajouteEnnemi(17,140*16,40*16); gpJeu->ajouteEnnemi(17,142*16,41*16);
            gpJeu->ajouteEnnemi(17,144*16,42*16); gpJeu->ajouteEnnemi(17,146*16,39*16); gpJeu->ajouteEnnemi(17,148*16,40*16);
            gpJeu->ajouteEnnemi(17,150*16,41*16); gpJeu->ajouteEnnemi(17,152*16,42*16); gpJeu->ajouteEnnemi(17,54*16,2*16);
            //boss
            if(gpJoueur->getCoeur(0)==0) {
                gpJeu->ajouteEnnemi(57,89*16-8,18*16-8);
                gpJeu->ajouteEnnemi(58,81*16,16*16);gpJeu->ajouteEnnemi(58,83*16,16*16);gpJeu->ajouteEnnemi(58,85*16,16*16);
                gpJeu->ajouteEnnemi(59,89*16,24*16);gpJeu->ajouteEnnemi(59,90*16,24*16);
                for (int i = 0; i < 4; i++) mur[89*2+i][24*2]=2;
            }
            //graal
            if (!gpJoueur->hasCristal(0)) gpJeu->ajouteObjet(11,16*89+8,16*6,0,1);
            //objets
            gpJeu->ajouteObjet(4,16*2,16*2-4,0,1); gpJeu->ajouteObjet(4,16*29,16*39-4,0,1); gpJeu->ajouteObjet(4,16*45,16*33-4,0,1);
            gpJeu->ajouteObjet(4,16*56,16*29-4,0,1); gpJeu->ajouteObjet(4,16*64,16*38-4,0,1); gpJeu->ajouteObjet(4,16*82,16*32-4,0,1);
            gpJeu->ajouteObjet(4,16*97,16*32-4,0,1); gpJeu->ajouteObjet(4,16*117,16*57-4,0,1); gpJeu->ajouteObjet(4,16*126,16*19-4,0,1);
            gpJeu->ajouteObjet(4,16*133,16*19-4,0,1); gpJeu->ajouteObjet(4,16*149,16*6-4,0,1); gpJeu->ajouteObjet(4,16*177,16*2-4,0,1);
            gpJeu->ajouteObjet(1,16*17,16*2,0,1); gpJeu->ajouteObjet(1,16*23,16*17,0,1); gpJeu->ajouteObjet(1,16*36,16*20,0,1);
            gpJeu->ajouteObjet(1,16*54,16*29,0,1); gpJeu->ajouteObjet(1,16*74,16*20,0,1); gpJeu->ajouteObjet(1,16*82,16*42,0,1);
            gpJeu->ajouteObjet(1,16*102,16*47,0,1); gpJeu->ajouteObjet(1,16*156,16*32,0,1); gpJeu->ajouteObjet(1,16*177,16*12,0,1);
            gpJeu->ajouteObjet(1,16*182,16*47,0,1);
            break;
        case 16 :
            //caisses
            gpJeu->ajouteCaisse(1,82*16,4*16); gpJeu->ajouteCaisse(1,82*16,5*16); 
            gpJeu->ajouteCaisse(1,83*16,5*16); gpJeu->ajouteCaisse(1,83*16,6*16); 
            gpJeu->ajouteCaisse(1,83*16,7*16); gpJeu->ajouteCaisse(1,83*16,8*16);
            gpJeu->ajouteCaisse(1,83*16,9*16); gpJeu->ajouteCaisse(1,82*16,9*16); 
            gpJeu->ajouteCaisse(1,82*16,10*16);
            //perso
            if (!gpJoueur->getCoffre(1,25)) gpJeu->ajoutePnj(40,174*16,18*16,138);
            if (!gpJoueur->getCoffre(1,26)) gpJeu->ajoutePnj(41,185*16,18*16,138);
            if (!gpJoueur->getCoffre(1,27)) gpJeu->ajoutePnj(42,194*16,61*16,138);
            if (!gpJoueur->hasObjet(O_SAC_BOMBES)) gpJeu->ajoutePnj(43,249*16+8,63*16,143);
            //ennemi
            gpJeu->ajouteEnnemi(8,129*16+8,21*16);gpJeu->ajouteEnnemi(8,289*16+8,66*16);
            gpJeu->ajouteEnnemi(8,149*16+8,19*16);gpJeu->ajouteEnnemi(8,169*16+8,70*16);
            gpJeu->ajouteEnnemi(8,190*16,38*16);gpJeu->ajouteEnnemi(8,129*16+8,81*16);
            gpJeu->ajouteEnnemi(8,270*16,6*16);gpJeu->ajouteEnnemi(8,227*16,97*16);
            gpJeu->ajouteEnnemi(8,272*16,97*16);gpJeu->ajouteEnnemi(8,245*16,49*16);
            gpJeu->ajouteEnnemi(8,254*16,49*16);gpJeu->ajouteEnnemi(8,209*16+8,20*16);
            gpJeu->ajouteEnnemi(8,204*16+8,3*16);
            gpJeu->ajouteEnnemi(3,49*16+2,49*16);gpJeu->ajouteEnnemi(3,149*16+2,81*16);
            gpJeu->ajouteEnnemi(3,70*16+2,56*16);gpJeu->ajouteEnnemi(3,92*16+2,57*16);
            gpJeu->ajouteEnnemi(3,83*16+2,38*16);gpJeu->ajouteEnnemi(3,89*16+2,35*16);
            gpJeu->ajouteEnnemi(3,95*16+2,38*16);gpJeu->ajouteEnnemi(3,109*16+2,45*16);
            gpJeu->ajouteEnnemi(3,149*16+2,41*16);gpJeu->ajouteEnnemi(3,291*16+2,5*16);
            gpJeu->ajouteEnnemi(3,295*16+2,23*16);gpJeu->ajouteEnnemi(3,294*16+2,54*16);
            gpJeu->ajouteEnnemi(3,225*16+2,69*16);gpJeu->ajouteEnnemi(3,225*16+2,75*16);
            gpJeu->ajouteEnnemi(3,225*16+2,81*16);gpJeu->ajouteEnnemi(3,233*16+2,69*16);
            gpJeu->ajouteEnnemi(3,233*16+2,75*16);gpJeu->ajouteEnnemi(3,233*16+2,81*16);
            gpJeu->ajouteEnnemi(3,286*16+2,70*16);gpJeu->ajouteEnnemi(3,292*16+2,70*16);
            gpJeu->ajouteEnnemi(3,206*16+2,49*16);gpJeu->ajouteEnnemi(3,259*16+2,33*16);
            gpJeu->ajouteEnnemi(3,261*16+2,41*16);gpJeu->ajouteEnnemi(3,225*16+2,52*16);
            gpJeu->ajouteEnnemi(18,68*16+8,53*16);gpJeu->ajouteEnnemi(18,77*16,53*16);
            gpJeu->ajouteEnnemi(18,85*16+8,53*16);gpJeu->ajouteEnnemi(18,96*16+8,47*16);
            gpJeu->ajouteEnnemi(18,92*16,7*16);gpJeu->ajouteEnnemi(18,53*16,3*16);
            gpJeu->ajouteEnnemi(18,59*16+8,3*16);gpJeu->ajouteEnnemi(18,66*16,3*16);
            gpJeu->ajouteEnnemi(18,53*16,11*16);gpJeu->ajouteEnnemi(18,59*16+8,11*16);
            gpJeu->ajouteEnnemi(18,66*16,11*16);gpJeu->ajouteEnnemi(18,128*16,40*16);
            gpJeu->ajouteEnnemi(18,136*16,38*16);gpJeu->ajouteEnnemi(18,143*16,45*16);
            gpJeu->ajouteEnnemi(18,146*16,37*16);gpJeu->ajouteEnnemi(18,154*16,40*16);
            gpJeu->ajouteEnnemi(18,154*16,54*16);gpJeu->ajouteEnnemi(18,162*16,51*16);
            gpJeu->ajouteEnnemi(18,169*16,33*16);gpJeu->ajouteEnnemi(18,173*16,47*16);
            gpJeu->ajouteEnnemi(18,128*16,48*16);gpJeu->ajouteEnnemi(18,206*16,40*16);
            gpJeu->ajouteEnnemi(18,216*16,50*16);gpJeu->ajouteEnnemi(18,209*16,7*16);
            gpJeu->ajouteEnnemi(18,249*16+8,97*16);gpJeu->ajouteEnnemi(18,269*16+8,56*16);
            gpJeu->ajouteEnnemi(18,227*16,7*16);gpJeu->ajouteEnnemi(18,223*16,23*16);
            gpJeu->ajouteEnnemi(18,230*16,34*16);gpJeu->ajouteEnnemi(18,240*16,27*16);
            gpJeu->ajouteEnnemi(18,241*16,20*16);gpJeu->ajouteEnnemi(18,243*16,37*16);
            gpJeu->ajouteEnnemi(18,247*16,30*16);gpJeu->ajouteEnnemi(18,255*16,27*16);
            gpJeu->ajouteEnnemi(18,264*16,19*16);gpJeu->ajouteEnnemi(18,264*16,29*16);
            gpJeu->ajouteEnnemi(18,265*16,37*16);gpJeu->ajouteEnnemi(18,273*16,26*16);
            gpJeu->ajouteEnnemi(18,273*16,40*16);gpJeu->ajouteEnnemi(18,249*16+8,7*16);
            gpJeu->ajouteEnnemi(18,286*16,38*16);gpJeu->ajouteEnnemi(18,295*16,37*16);
            //boss
            if(gpJoueur->getCoeur(1)==0) gpJeu->ajouteEnnemi(60,249*16-9,78*16-8);
            //objets
            gpJeu->ajouteObjet(4,16*24,16*2-4,0,1);gpJeu->ajouteObjet(4,16*31,16*12-4,0,1);
            gpJeu->ajouteObjet(4,16*36,16*12-4,0,1);gpJeu->ajouteObjet(4,16*57,16*57-4,0,1);
            gpJeu->ajouteObjet(4,16*150,16*54-4,0,1);gpJeu->ajouteObjet(4,16*150,16*55-4,0,1);
            gpJeu->ajouteObjet(4,16*142,16*17-4,0,1);gpJeu->ajouteObjet(4,16*157,16*17-4,0,1);
            gpJeu->ajouteObjet(4,16*197,16*42-4,0,1);gpJeu->ajouteObjet(4,16*162,16*72-4,0,1);
            gpJeu->ajouteObjet(4,16*202,16*62-4,0,1);gpJeu->ajouteObjet(4,16*242,16*57-4,0,1);
            gpJeu->ajouteObjet(4,16*257,16*57-4,0,1);gpJeu->ajouteObjet(4,16*242,16*77-4,0,1);
            gpJeu->ajouteObjet(4,16*257,16*77-4,0,1);gpJeu->ajouteObjet(4,16*242,16*87-4,0,1);
            gpJeu->ajouteObjet(4,16*257,16*87-4,0,1);gpJeu->ajouteObjet(4,16*222,16*92-4,0,1);
            gpJeu->ajouteObjet(4,16*277,16*92-4,0,1);gpJeu->ajouteObjet(4,16*237,16*40-4,0,1);
            gpJeu->ajouteObjet(4,16*237,16*42-4,0,1);gpJeu->ajouteObjet(1,16*30,16*2,0,1);
            gpJeu->ajouteObjet(1,16*37,16*2,0,1);gpJeu->ajouteObjet(1,16*57,16*47,0,1);
            gpJeu->ajouteObjet(1,16*149,16*54,0,1);gpJeu->ajouteObjet(1,16*149,16*55,0,1);
            gpJeu->ajouteObjet(1,16*182,16*42,0,1);gpJeu->ajouteObjet(1,16*222,16*102,0,1);
            gpJeu->ajouteObjet(1,16*277,16*102,0,1);gpJeu->ajouteObjet(1,16*237,16*41,0,1);
            break;
        case 17 :
            //caisses
            gpJeu->ajouteCaisse(2,207*16,20*16);gpJeu->ajouteCaisse(2,233*16,21*16);
            gpJeu->ajouteCaisse(2,233*16,39*16);
            //pièges
            gpJeu->ajouteEnnemi(61,126*16,32*16);gpJeu->ajouteEnnemi(61,126*16,52*16);
            gpJeu->ajouteEnnemi(61,126*16,72*16);gpJeu->ajouteEnnemi(61,205*16,34*16);
            gpJeu->ajouteEnnemi(61,213*16,34*16);gpJeu->ajouteEnnemi(61,221*16,34*16);
            gpJeu->ajouteEnnemi(61,226*16,62*16);gpJeu->ajouteEnnemi(61,246*16,32*16);
            gpJeu->ajouteEnnemi(17,54*16,94*16);gpJeu->ajouteEnnemi(17,55*16,93*16);
            gpJeu->ajouteEnnemi(17,56*16,92*16);gpJeu->ajouteEnnemi(17,57*16,91*16);
            gpJeu->ajouteEnnemi(17,58*16,91*16);gpJeu->ajouteEnnemi(17,63*16,97*16);
            gpJeu->ajouteEnnemi(17,66*16,97*16);gpJeu->ajouteEnnemi(17,69*16,97*16);
            gpJeu->ajouteEnnemi(17,72*16,97*16);gpJeu->ajouteEnnemi(17,77*16,91*16);
            gpJeu->ajouteEnnemi(17,78*16,94*16);gpJeu->ajouteEnnemi(17,79*16,93*16);
            gpJeu->ajouteEnnemi(17,80*16,92*16);gpJeu->ajouteEnnemi(17,81*16,91*16);
            gpJeu->ajouteEnnemi(17,83*16,94*16);gpJeu->ajouteEnnemi(17,84*16,93*16);
            gpJeu->ajouteEnnemi(17,85*16,92*16);gpJeu->ajouteEnnemi(17,86*16,91*16);
            //ennemi
            gpJeu->ajouteEnnemi(1,4*16,49*16);gpJeu->ajouteEnnemi(1,15*16,49*16);
            gpJeu->ajouteEnnemi(1,43*16,86*16);gpJeu->ajouteEnnemi(1,63*16,79*16);
            gpJeu->ajouteEnnemi(1,50*16,25*16);gpJeu->ajouteEnnemi(1,58*16,38*16);
            gpJeu->ajouteEnnemi(1,62*16,31*16);gpJeu->ajouteEnnemi(1,69*16+8,22*16);
            gpJeu->ajouteEnnemi(1,77*16,31*16);gpJeu->ajouteEnnemi(1,81*16,38*16);
            gpJeu->ajouteEnnemi(1,89*16,25*16);gpJeu->ajouteEnnemi(1,146*16,110*16);
            gpJeu->ajouteEnnemi(1,150*16,114*16);gpJeu->ajouteEnnemi(1,153*16,66*16);
            gpJeu->ajouteEnnemi(1,170*16,77*16);gpJeu->ajouteEnnemi(1,189*16+8,37*16);
            gpJeu->ajouteEnnemi(1,190*16,85*16);gpJeu->ajouteEnnemi(1,211*16,22*16);
            gpJeu->ajouteEnnemi(1,169*16,30*16);
            gpJeu->ajouteEnnemi(18,146*16,31*16);gpJeu->ajouteEnnemi(18,149*16,19*16);
            gpJeu->ajouteEnnemi(18,150*16,39*16);gpJeu->ajouteEnnemi(18,154*16,25*16);
            gpJeu->ajouteEnnemi(18,159*16,29*16);gpJeu->ajouteEnnemi(18,161*16,34*16);
            gpJeu->ajouteEnnemi(18,166*16,23*16);gpJeu->ajouteEnnemi(18,172*16,41*16);
            gpJeu->ajouteEnnemi(18,174*16,18*16);
            gpJeu->ajouteEnnemi(53,44*16,19*16);gpJeu->ajouteEnnemi(53,49*16,35*16);
            gpJeu->ajouteEnnemi(53,90*16,35*16);gpJeu->ajouteEnnemi(53,95*16,19*16);
            gpJeu->ajouteEnnemi(53,43*16,63*16);gpJeu->ajouteEnnemi(53,54*16,98*16);
            gpJeu->ajouteEnnemi(53,84*16,101*16);gpJeu->ajouteEnnemi(53,83*16,108*16);
            gpJeu->ajouteEnnemi(53,96*16,108*16);gpJeu->ajouteEnnemi(53,153*16,109*16);
            gpJeu->ajouteEnnemi(53,169*16,86*16);gpJeu->ajouteEnnemi(53,217*16,19*16);
            gpJeu->ajouteEnnemi(53,217*16,26*16);gpJeu->ajouteEnnemi(53,233*16,109*16);
            gpJeu->ajouteEnnemi(53,233*16,115*16);
            //boss
            if(gpJoueur->getCoeur(2)==0) gpJeu->ajouteEnnemi(62,9*16+4,32*16);
            //graal
            if (!gpJoueur->hasCristal(1)) gpJeu->ajouteObjet(11,16*9+8,16*21,0,1);
            //objet
            gpJeu->ajouteObjet(4,16*2,16*32-4,0,1);gpJeu->ajouteObjet(4,16*17,16*32-4,0,1);
            gpJeu->ajouteObjet(4,16*2,16*47-4,0,1);gpJeu->ajouteObjet(4,16*64,16*24-4,0,1);
            gpJeu->ajouteObjet(4,16*75,16*24-4,0,1);gpJeu->ajouteObjet(4,16*64,16*89-4,0,1);
            gpJeu->ajouteObjet(4,16*71,16*89-4,0,1);gpJeu->ajouteObjet(4,16*142,16*72-4,0,1);
            gpJeu->ajouteObjet(4,16*147,16*72-4,0,1);gpJeu->ajouteObjet(4,16*182,16*107-4,0,1);
            gpJeu->ajouteObjet(4,16*197,16*107-4,0,1);gpJeu->ajouteObjet(4,16*204,16*64-4,0,1);
            gpJeu->ajouteObjet(6,16*2,16*42,0,1);gpJeu->ajouteObjet(6,16*17,16*42,0,1);
            gpJeu->ajouteObjet(6,16*17,16*47,0,1);gpJeu->ajouteObjet(6,16*52,16*39,0,1);
            gpJeu->ajouteObjet(6,16*87,16*39,0,1);gpJeu->ajouteObjet(6,16*197,16*117,0,1);
            gpJeu->ajouteObjet(6,16*42,16*117,0,1);gpJeu->ajouteObjet(6,16*97,16*117,0,1);
            break;
        case 18 :
            //perso
            if (!gpJoueur->hasObjet(O_ARC)) gpJeu->ajoutePnj(44,150*16,46*16,148);
            else gpJeu->ajouteEnnemi(64,150*16-4,47*16);
            if (gpJoueur->getAvancement()<65) gpJeu->ajoutePnj(45,270*16,2*16,152);
            //ennemi
            gpJeu->ajouteEnnemi(12,7*16,50*16);gpJeu->ajouteEnnemi(12,9*16,37*16);
            gpJeu->ajouteEnnemi(12,13*16,24*16);gpJeu->ajouteEnnemi(12,19*16,54*16);
            gpJeu->ajouteEnnemi(12,30*16,29*16);gpJeu->ajouteEnnemi(12,39*16,50*16);
            gpJeu->ajouteEnnemi(12,50*16,38*16);gpJeu->ajouteEnnemi(12,51*16,20*16);
            gpJeu->ajouteEnnemi(12,69*16+8,18*16);gpJeu->ajouteEnnemi(12,69*16+8,37*16);
            gpJeu->ajouteEnnemi(12,74*16,52*16);gpJeu->ajouteEnnemi(12,84*16,52*16);
            gpJeu->ajouteEnnemi(12,109*16+8,34*16);gpJeu->ajouteEnnemi(12,110*16,18*16);
            gpJeu->ajouteEnnemi(12,139*16,3*16);gpJeu->ajouteEnnemi(12,126*16,44*16);
            gpJeu->ajouteEnnemi(12,133*16,44*16);gpJeu->ajouteEnnemi(12,129*16+8,71*16);
            gpJeu->ajouteEnnemi(12,149*16+8,37*16);gpJeu->ajouteEnnemi(12,170*16,24*16);
            gpJeu->ajouteEnnemi(12,189*16,18*16);gpJeu->ajouteEnnemi(12,195*16,4*16);
            gpJeu->ajouteEnnemi(12,209*16,11*16);gpJeu->ajouteEnnemi(12,215*16,3*16);
            gpJeu->ajouteEnnemi(12,215*16,26*16);gpJeu->ajouteEnnemi(12,285*16,18*16);
            gpJeu->ajouteEnnemi(12,295*16,10*16);gpJeu->ajouteEnnemi(12,313*16,14*16);
            gpJeu->ajoutePiege(51,2*16,17*16);gpJeu->ajoutePiege(51,102*16,47*16);
            gpJeu->ajoutePiege(51,122*16,2*16);gpJeu->ajoutePiege(51,262*16,62*16);
            gpJeu->ajoutePiege(51,282*16,2*16);gpJeu->ajoutePiege(51,222*16,32*16);
            gpJeu->ajoutePiege(51,222*16,47*16);gpJeu->ajoutePiege(51,62*16,47*16);
            gpJeu->ajoutePiege(51,62*16,32*16);gpJeu->ajoutePiege(51,102*16,32*16);
            gpJeu->ajoutePiege(51,62*16,17*16);gpJeu->ajoutePiege(51,82*16,17*16);
            gpJeu->ajoutePiege(51,102*16,17*16);gpJeu->ajoutePiege(51,142*16,32*16);
            gpJeu->ajoutePiege(51,182*16,47*16);gpJeu->ajoutePiege(51,122*16,32*16);
            gpJeu->ajoutePiege(51,122*16,62*16);
            gpJeu->ajouteEnnemi(62,189*16+4,39*16);
            gpJeu->ajouteEnnemi(64,169*16-4,6*16);gpJeu->ajouteEnnemi(64,206*16-4,45*16);
            gpJeu->ajouteEnnemi(64,212*16-4,45*16);gpJeu->ajouteEnnemi(64,209*16-4,47*16);
            gpJeu->ajouteEnnemi(64,249*16-4,36*16);gpJeu->ajouteEnnemi(64,269*16-4,37*16);
            gpJeu->ajouteEnnemi(64,249*16-4,51*16);gpJeu->ajouteEnnemi(64,299*16-4,36*16);
            gpJeu->ajouteEnnemi(64,299*16-4,51*16);
            //pieges
            gpJeu->ajouteEnnemi(61,288*16,72*16);gpJeu->ajouteEnnemi(61,296*16,62*16);
            gpJeu->ajouteEnnemi(61,304*16,72*16);gpJeu->ajouteEnnemi(61,236*16,65*16);
            gpJeu->ajouteEnnemi(36,239*16,13*16); setValeur(239*16,13*16,1727,2);
            //boss
            if(gpJoueur->getCoeur(3)==0) gpJeu->ajouteEnnemi(63,268*16-5,17*16);
            //objet
            gpJeu->ajouteObjet(4,16*162,16*2-4,0,1);gpJeu->ajouteObjet(4,16*177,16*2-4,0,1);
            gpJeu->ajouteObjet(4,16*197,16*32-4,0,1);gpJeu->ajouteObjet(4,16*182,16*47-4,0,1);
            gpJeu->ajouteObjet(4,16*197,16*47-4,0,1);gpJeu->ajouteObjet(4,16*242,16*42-4,0,1);
            gpJeu->ajouteObjet(4,16*162,16*47-4,0,1);gpJeu->ajouteObjet(4,16*177,16*47-4,0,1);
            gpJeu->ajouteObjet(4,16*317,16*47-4,0,1);gpJeu->ajouteObjet(6,16*182,16*32,0,1);
            gpJeu->ajouteObjet(6,16*197,16*42,0,1);gpJeu->ajouteObjet(6,16*257,16*42,0,1);
            gpJeu->ajouteObjet(7,16*204,16*11-4,0,1);gpJeu->ajouteObjet(7,16*204,16*12-4,0,1);
            gpJeu->ajouteObjet(7,16*205,16*11-4,0,1);gpJeu->ajouteObjet(7,16*205,16*12-4,0,1);
            break;
        case 19 :
            //caisses
            gpJeu->ajouteCaisse(5,25*16,33*16);gpJeu->ajouteCaisse(5,25*16,34*16);
            gpJeu->ajouteCaisse(5,27*16,52*16);gpJeu->ajouteCaisse(5,28*16,52*16);
            gpJeu->ajouteCaisse(5,113*16,67*16);gpJeu->ajouteCaisse(5,191*16,67*16);
            gpJeu->ajouteCaisse(5,192*16,67*16);gpJeu->ajouteCaisse(5,29*16,28*16);
            gpJeu->ajouteCaisse(5,30*16,28*16);
            //ennemi
            gpJeu->ajouteEnnemi(5,49*16+6,51*16);gpJeu->ajouteEnnemi(5,49*16+6,2*16);
            gpJeu->ajouteEnnemi(5,69*16+6,6*16);gpJeu->ajouteEnnemi(5,17*16+6,19*16);
            gpJeu->ajouteEnnemi(5,9*16+6,27*16);gpJeu->ajouteEnnemi(5,17*16+6,51*16);
            gpJeu->ajouteEnnemi(5,94*16+6,54*16);gpJeu->ajouteEnnemi(5,80*16+6,66*16);
            gpJeu->ajouteEnnemi(5,122*16+6,66*16);gpJeu->ajouteEnnemi(5,153*16+6,47*16);
            gpJeu->ajouteEnnemi(5,130*16+6,6*16);gpJeu->ajouteEnnemi(5,172*16+6,6*16);
            gpJeu->ajouteEnnemi(5,182*16+6,6*16);
            gpJeu->ajouteEnnemi(25,49*16+6,21*16-8);gpJeu->ajouteEnnemi(25,69*16+6,49*16-8);gpJeu->ajouteEnnemi(25,168*16+6,36*16-8);
            gpJeu->ajouteEnnemi(25,169*16+6,49*16-8);gpJeu->ajouteEnnemi(25,109*16+6,47*16-8);gpJeu->ajouteEnnemi(25,129*16+6,42*16-8);
            gpJeu->ajouteEnnemi(25,109*16+6,9*16-8);
            //pieges
            gpJeu->ajouteEnnemi(61,11*16,8*16+8);gpJeu->ajouteEnnemi(61,19*16,8*16+8);
            gpJeu->ajouteEnnemi(61,27*16,8*16+8);gpJeu->ajouteEnnemi(61,16*16,65*16+8);
            gpJeu->ajouteEnnemi(61,24*16,65*16+8);gpJeu->ajouteEnnemi(61,186*16,47*16-8);
            gpJeu->ajouteEnnemi(61,186*16,57*16+8);
            gpJeu->ajouteEnnemi(17,71*16,6*16-8);gpJeu->ajouteEnnemi(17,72*16,8*16+8);
            gpJeu->ajouteEnnemi(17,73*16,6*16-8);gpJeu->ajouteEnnemi(17,74*16,8*16+8);
            gpJeu->ajouteEnnemi(36,42*16,17*16); setValeur(42*16,17*16,1011,2);
            gpJeu->ajouteEnnemi(36,57*16,17*16); setValeur(57*16,17*16,1011,2);
            gpJeu->ajouteEnnemi(36,62*16,57*16); setValeur(62*16,57*16,1011,2);
            gpJeu->ajouteEnnemi(36,77*16,57*16); setValeur(77*16,57*16,1011,2);
            gpJeu->ajouteEnnemi(36,102*16,2*16); setValeur(102*16,2*16,1011,2);
            gpJeu->ajouteEnnemi(36,117*16,2*16); setValeur(117*16,2*16,1011,2);
            gpJeu->ajouteEnnemi(36,102*16,27*16); setValeur(102*16,27*16,1011,2);
            gpJeu->ajouteEnnemi(36,117*16,27*16); setValeur(117*16,27*16,1011,2);
            gpJeu->ajouteEnnemi(36,102*16,32*16); setValeur(102*16,32*16,1011,2);
            gpJeu->ajouteEnnemi(36,137*16,32*16); setValeur(137*16,32*16,1011,2);
            gpJeu->ajouteEnnemi(36,102*16,57*16); setValeur(102*16,57*16,1011,2);
            gpJeu->ajouteEnnemi(36,137*16,57*16); setValeur(137*16,57*16,1011,2);
            gpJeu->ajouteEnnemi(36,162*16,2*16); setValeur(162*16,2*16,1011,2);
            gpJeu->ajouteEnnemi(36,197*16,2*16); setValeur(197*16,2*16,1011,2);
            gpJeu->ajouteEnnemi(36,162*16,12*16); setValeur(162*16,12*16,1011,2);
            gpJeu->ajouteEnnemi(36,197*16,12*16); setValeur(197*16,12*16,1011,2);
            gpJeu->ajouteEnnemi(36,182*16,17*16); setValeur(182*16,17*16,1011,2);
            gpJeu->ajouteEnnemi(36,197*16,17*16); setValeur(197*16,17*16,1011,2);
            gpJeu->ajouteEnnemi(36,182*16,42*16); setValeur(182*16,42*16,1011,2);
            gpJeu->ajouteEnnemi(36,197*16,42*16); setValeur(197*16,42*16,1011,2);
            //boss
            if(gpJoueur->getCoeur(4)==0) gpJeu->ajouteEnnemi(45,80*16-33,30*16-38);
            //graal
            if (!gpJoueur->hasCristal(2)) gpJeu->ajouteObjet(11,16*89+8,16*5,0,1);
            //switch
            gpJeu->ajouteEnnemi(20,23*16,11*16); setValeur(23*16,11*16,1011,2);
            gpJeu->ajouteEnnemi(20,23*16,20*16); setValeur(23*16,20*16,1011,2);
            gpJeu->ajouteEnnemi(20,36*16,28*16); setValeur(36*16,28*16,1011,2);
            gpJeu->ajouteEnnemi(20,56*16,33*16); setValeur(56*16,33*16,1011,2);
            gpJeu->ajouteEnnemi(20,76*16,7*16); setValeur(76*16,7*16,1011,2);
            gpJeu->ajouteEnnemi(20,80*16,69*16); setValeur(80*16,69*16,1011,2);
            gpJeu->ajouteEnnemi(20,109*16,17*16); setValeur(109*16,17*16,1010,2);
            gpJeu->ajouteEnnemi(20,149*16,7*16); setValeur(149*16,7*16,1010,2);
            gpJeu->ajouteEnnemi(20,156*16,22*16); setValeur(156*16,22*16,1010,2);
            gpJeu->ajouteEnnemi(20,169*16,72*16); setValeur(169*16,72*16,1010,2);
            gpJeu->ajouteEnnemi(20,175*16,44*16); setValeur(175*16,44*16,1010,2);
            gpJeu->ajouteEnnemi(20,176*16,26*16); setValeur(176*16,26*16,1010,2);
            //objets
            gpJeu->ajouteObjet(4,16*5,16*2-4,0,1);gpJeu->ajouteObjet(4,16*64,16*12-4,0,1);
            gpJeu->ajouteObjet(4,16*35,16*65-4,0,1);gpJeu->ajouteObjet(4,16*63,16*47-4,0,1);
            gpJeu->ajouteObjet(4,16*71,16*47-4,0,1);gpJeu->ajouteObjet(4,16*82,16*47-4,0,1);
            gpJeu->ajouteObjet(4,16*97,16*47-4,0,1);gpJeu->ajouteObjet(4,16*162,16*21-4,0,1);
            gpJeu->ajouteObjet(5,16*65,16*2,0,1);gpJeu->ajouteObjet(5,16*67,16*2,0,1);
            gpJeu->ajouteObjet(5,16*69,16*47,0,1);gpJeu->ajouteObjet(5,16*83,16*47,0,1);
            gpJeu->ajouteObjet(5,16*96,16*47,0,1);gpJeu->ajouteObjet(5,16*162,16*22,0,1);
            gpJeu->ajouteObjet(6,16*68,16*12,0,1);gpJeu->ajouteObjet(6,16*162,16*23,0,1);
            gpJeu->ajouteObjet(7,16*37,16*65-4,0,1);gpJeu->ajouteObjet(7,16*67,16*47-4,0,1);
            gpJeu->ajouteObjet(7,16*76,16*47-4,0,1);gpJeu->ajouteObjet(7,16*162,16*24-4,0,1);
            break;
        case 20 :
            //caisses
            gpJeu->ajouteCaisse(4,90*16,18*16); gpJeu->ajouteCaisse(4,90*16,26*16);
            gpJeu->ajouteCaisse(4,90*16,33*16); gpJeu->ajouteCaisse(4,90*16,41*16);
            gpJeu->ajouteCaisse(4,96*16,49*16);
            //pièges
            gpJeu->ajouteEnnemi(65,2*16,32*16); setValeur(2*16,32*16,1362,2);
            gpJeu->ajouteEnnemi(65,17*16,32*16); setValeur(17*16,32*16,1362,2);
            gpJeu->ajouteEnnemi(65,130*16,17*16); setValeur(130*16,17*16,135,2);
            gpJeu->ajouteEnnemi(65,132*16,33*16); setValeur(132*16,33*16,135,2);
            gpJeu->ajouteEnnemi(65,157*16,21*16); setValeur(157*16,21*16,135,2);
            gpJeu->ajouteEnnemi(65,157*16,37*16); setValeur(157*16,37*16,135,2);
            gpJeu->ajouteEnnemi(65,142*16,12*16); setValeur(142*16,12*16,135,2);
            gpJeu->ajouteEnnemi(65,157*16,12*16); setValeur(157*16,12*16,135,2);
            gpJeu->ajouteEnnemi(65,149*16,57*16); setValeur(149*16,57*16,135,2);
            gpJeu->ajouteEnnemi(65,150*16,57*16); setValeur(150*16,57*16,135,2);
            gpJeu->ajouteEnnemi(65,162*16,2*16); setValeur(162*16,2*16,1362,2);
            gpJeu->ajouteEnnemi(65,197*16,2*16); setValeur(197*16,2*16,1362,2);
            gpJeu->ajouteEnnemi(65,162*16,27*16); setValeur(162*16,27*16,1362,2);
            gpJeu->ajouteEnnemi(65,197*16,27*16); setValeur(197*16,27*16,1362,2);
            gpJeu->ajouteEnnemi(65,162*16,62*16); setValeur(162*16,62*16,1362,2);
            gpJeu->ajouteEnnemi(65,197*16,62*16); setValeur(197*16,62*16,1362,2);
            gpJeu->ajouteEnnemi(65,162*16,87*16); setValeur(162*16,87*16,1362,2);
            gpJeu->ajouteEnnemi(65,197*16,87*16); setValeur(197*16,87*16,1362,2);
            gpJeu->ajouteEnnemi(65,222*16,2*16); setValeur(222*16,2*16,1362,2);
            gpJeu->ajouteEnnemi(65,257*16,2*16); setValeur(257*16,2*16,1362,2);
            gpJeu->ajouteEnnemi(65,222*16,27*16); setValeur(222*16,27*16,1362,2);
            gpJeu->ajouteEnnemi(65,257*16,27*16); setValeur(257*16,27*16,1362,2);
            gpJeu->ajouteEnnemi(65,222*16,62*16); setValeur(222*16,62*16,1362,2);
            gpJeu->ajouteEnnemi(65,257*16,62*16); setValeur(257*16,62*16,1362,2);
            gpJeu->ajouteEnnemi(65,222*16,87*16); setValeur(222*16,87*16,1362,2);
            gpJeu->ajouteEnnemi(65,257*16,87*16); setValeur(257*16,87*16,1362,2);
            //ennemi
            gpJeu->ajouteEnnemi(55,82*16,78*16);gpJeu->ajouteEnnemi(55,96*16+10,78*16);
            gpJeu->ajouteEnnemi(55,28*16,4*16);gpJeu->ajouteEnnemi(55,26*16,23*16);
            gpJeu->ajouteEnnemi(55,51*16,33*16);gpJeu->ajouteEnnemi(55,46*16,65*16);
            gpJeu->ajouteEnnemi(55,45*16,84*16);gpJeu->ajouteEnnemi(55,133*16,84*16);
            gpJeu->ajouteEnnemi(55,128*16,64*16);gpJeu->ajouteEnnemi(55,149*16,52*16);
            gpJeu->ajouteEnnemi(55,135*16,22*16);gpJeu->ajouteEnnemi(55,141*16,32*16);
            gpJeu->ajouteEnnemi(55,148*16,4*16);
            gpJeu->ajouteEnnemi(25,69*16,48*16);gpJeu->ajouteEnnemi(25,171*16,18*16);
            gpJeu->ajouteEnnemi(25,188*16,18*16);gpJeu->ajouteEnnemi(25,188*16,6*16);
            gpJeu->ajouteEnnemi(37,69*16,63*16);gpJeu->ajouteEnnemi(37,171*16,80*16);
            gpJeu->ajouteEnnemi(37,171*16,66*16);gpJeu->ajouteEnnemi(37,188*16,66*16);
            gpJeu->ajouteEnnemi(41,110*16,63*16);gpJeu->ajouteEnnemi(41,231*16,66*16);
            gpJeu->ajouteEnnemi(41,248*16,66*16);gpJeu->ajouteEnnemi(41,248*16,80*16);
            gpJeu->ajouteEnnemi(1,89*16+8,66*16);
            gpJeu->ajouteEnnemi(15,110*16,48*16);gpJeu->ajouteEnnemi(15,231*16,6*16);
            gpJeu->ajouteEnnemi(15,231*16,18*16);gpJeu->ajouteEnnemi(15,248*16,18*16);
            gpJeu->ajoutePiege(51,23*16,48*16);gpJeu->ajoutePiege(51,2*16,77*16);
            //boss
            if(gpJoueur->getCoeur(5)==0) gpJeu->ajouteEnnemi(40,9*16-11,32*16);
            //objets
            gpJeu->ajouteObjet(4,16*26,16*12-4,0,1);gpJeu->ajouteObjet(4,16*25,16*50-4,0,1);
            gpJeu->ajouteObjet(4,16*31,16*49-4,0,1);gpJeu->ajouteObjet(4,16*7,16*47-4,0,1);
            gpJeu->ajouteObjet(4,16*12,16*47-4,0,1);gpJeu->ajouteObjet(4,16*42,16*87-4,0,1);
            gpJeu->ajouteObjet(4,16*156,16*47-4,0,1);gpJeu->ajouteObjet(4,16*142,16*47-4,0,1);
            gpJeu->ajouteObjet(5,16*34,16*12,0,1);gpJeu->ajouteObjet(5,16*27,16*49,0,1);
            gpJeu->ajouteObjet(5,16*34,16*50,0,1);gpJeu->ajouteObjet(5,16*6,16*47,0,1);
            gpJeu->ajouteObjet(5,16*13,16*47,0,1);gpJeu->ajouteObjet(5,16*57,16*77,0,1);
            gpJeu->ajouteObjet(5,16*144,16*47,0,1);
            gpJeu->ajouteObjet(6,16*28,16*50,0,1);gpJeu->ajouteObjet(6,16*122,16*77,0,1);
            break;
        case 21 :
            //boss
            if(gpJoueur->getCoeur(6)==0) {
                gpJeu->ajouteEnnemi(66,247*16,23*16);
                gpJeu->ajouteEnnemi(67,247*16,23*16);
                gpJeu->ajouteEnnemi(68,244*16,24*16-8);
                gpJeu->ajouteEnnemi(68,243*16,26*16);
                gpJeu->ajouteEnnemi(69,255*16-10,24*16-8);
                gpJeu->ajouteEnnemi(69,256*16-10,26*16);
                gpJeu->ajouteEnnemi(27,249*16+8,24*16);
            }
            //ennemi
            gpJeu->ajouteEnnemi(9,9*16+8,21*16);gpJeu->ajouteEnnemi(9,29*16+8,4*16);
            gpJeu->ajouteEnnemi(9,269*16+8,6*16);
            gpJeu->ajouteEnnemi(10,59*16+8,32*16);gpJeu->ajouteEnnemi(10,39*16+8,32*16);
            gpJeu->ajouteEnnemi(10,49*16+8,27*16);gpJeu->ajouteEnnemi(10,139*16+8,32*16+8);
            gpJeu->ajouteEnnemi(10,162*16+8,32*16+8);
            gpJeu->ajouteEnnemi(22,71*16,52*16-4);gpJeu->ajouteEnnemi(22,65*16,37*16-4);
            gpJeu->ajouteEnnemi(22,33*16,37*16-4);gpJeu->ajouteEnnemi(22,36*16,22*16-4);
            gpJeu->ajouteEnnemi(22,62*16,22*16-4);gpJeu->ajouteEnnemi(22,95*16,7*16-4);
            gpJeu->ajouteEnnemi(22,193*16,33*16-4);gpJeu->ajouteEnnemi(22,106*16,4*16-4);
            gpJeu->ajouteEnnemi(22,174*16,27*16-4);gpJeu->ajouteEnnemi(22,140*16,18*16-4);
            gpJeu->ajouteEnnemi(22,206*16,11*16-4);gpJeu->ajouteEnnemi(22,223*16,6*16-4);
            gpJeu->ajouteEnnemi(22,232*16,14*16-4);gpJeu->ajouteEnnemi(22,295*16,3*16-4);
            gpJeu->ajouteEnnemi(22,295*16,48*16-4);gpJeu->ajouteEnnemi(22,264*16,34*16-4);
            gpJeu->ajouteEnnemi(22,249*16+8,55*16-4);gpJeu->ajouteEnnemi(22,129*16+8,55*16-4);
            gpJeu->ajouteEnnemi(23,89*16+3,36*16);gpJeu->ajouteEnnemi(23,72*16+3,6*16);
            gpJeu->ajouteEnnemi(23,9*16+3,36*16);gpJeu->ajouteEnnemi(23,166*16+3,7*16);
            gpJeu->ajouteEnnemi(23,104*16+3,38*16);gpJeu->ajouteEnnemi(23,169*16+3,52*16);
            gpJeu->ajouteEnnemi(23,224*16+3,37*16);gpJeu->ajouteEnnemi(23,214*16+3,37*16);
            gpJeu->ajouteEnnemi(23,269*16+3,22*16);gpJeu->ajouteEnnemi(23,29*16+3,52*16);
            gpJeu->ajouteEnnemi(27,95*16,48*16);gpJeu->ajouteEnnemi(27,95*16,56*16);
            gpJeu->ajouteEnnemi(27,96*16,22*16);gpJeu->ajouteEnnemi(27,4*16,3*16);
            gpJeu->ajouteEnnemi(27,15*16,3*16);gpJeu->ajouteEnnemi(27,194*16,50*16);
            gpJeu->ajouteEnnemi(27,194*16,54*16);gpJeu->ajouteEnnemi(27,149*16+8,7*16);
            gpJeu->ajouteEnnemi(27,124*16,3*16);gpJeu->ajouteEnnemi(27,129*16+8,3*16);
            gpJeu->ajouteEnnemi(27,135*16,3*16);gpJeu->ajouteEnnemi(27,232*16,52*16);
            //graal
            if (!gpJoueur->hasCristal(3)) gpJeu->ajouteObjet(11,16*249+8,16*5,0,1);
            //switch
            gpJeu->ajouteEnnemi(20,50*16,2*16); setValeur(50*16,2*16,2326,2);
            gpJeu->ajouteEnnemi(20,57*16,56*16); setValeur(57*16,56*16,2326,2);
            gpJeu->ajouteEnnemi(20,63*16,5*16); setValeur(63*16,5*16,2326,2);
            gpJeu->ajouteEnnemi(20,89*16,6*16); setValeur(89*16,6*16,2326,2);
            gpJeu->ajouteEnnemi(20,110*16,22*16); setValeur(110*16,22*16,2520,2);
            gpJeu->ajouteEnnemi(20,137*16,52*16); setValeur(137*16,52*16,2520,2);
            gpJeu->ajouteEnnemi(20,272*16,51*16); setValeur(272*16,51*16,2519,2);
            gpJeu->ajouteEnnemi(20,283*16,3*16); setValeur(283*16,3*16,2519,2);
            //objets
            gpJeu->ajouteObjet(4,16*97,16*32-4,0,1);gpJeu->ajouteObjet(4,16*182,16*49-4,0,1);
            gpJeu->ajouteObjet(4,16*255,16*35-4,0,1);gpJeu->ajouteObjet(4,16*255,16*36-4,0,1);
            gpJeu->ajouteObjet(5,16*144,16*10,0,1);gpJeu->ajouteObjet(5,16*195,16*47,0,1);
            gpJeu->ajouteObjet(5,16*205,16*304,0,1);gpJeu->ajouteObjet(5,16*255,16*38,0,1);
            gpJeu->ajouteObjet(5,16*255,16*39,0,1);
            gpJeu->ajouteObjet(6,16*165,16*4,0,1);gpJeu->ajouteObjet(6,16*255,16*37,0,1);
            gpJeu->ajouteObjet(3,16*194,16*25,0,1);gpJeu->ajouteObjet(3,16*184,16*57,0,1);
            break;
        case 22 :
            //caisses
            gpJeu->ajouteCaisse(5,139*16,22*16);gpJeu->ajouteCaisse(5,140*16,22*16);
            gpJeu->ajouteCaisse(5,190*16,22*16);gpJeu->ajouteCaisse(5,191*16,22*16);
            gpJeu->ajouteCaisse(5,204*16,78*16);gpJeu->ajouteCaisse(5,205*16,78*16);
            //pièges
            gpJeu->ajouteEnnemi(61,25*16,77*16-8);gpJeu->ajouteEnnemi(61,47*16,77*16-8);
            gpJeu->ajouteEnnemi(61,126*16,21*16-8);gpJeu->ajouteEnnemi(61,146*16,21*16-8);
            gpJeu->ajouteEnnemi(61,207*16,17*16-8);gpJeu->ajouteEnnemi(61,215*16,27*16-8);
            gpJeu->ajouteEnnemi(61,216*16,4*16-8);
            //ennemi
            gpJeu->ajouteEnnemi(1,190*16,32*16);gpJeu->ajouteEnnemi(1,210*16,32*16);
            gpJeu->ajouteEnnemi(1,170*16,22*16);gpJeu->ajouteEnnemi(1,156*16,36*16);
            gpJeu->ajouteEnnemi(1,83*16,51*16);gpJeu->ajouteEnnemi(1,39*16,69*16);
            gpJeu->ajouteEnnemi(1,39*16,34*16);gpJeu->ajouteEnnemi(1,75*16,36*16);
            gpJeu->ajouteEnnemi(1,187*16,81*16);gpJeu->ajouteEnnemi(1,196*16+8,82*16);
            gpJeu->ajouteEnnemi(1,214*16+8,77*16);
            gpJeu->ajouteEnnemi(18,236*16,82*16);gpJeu->ajouteEnnemi(18,206*16,53*16);
            gpJeu->ajouteEnnemi(18,204*16,68*16);gpJeu->ajouteEnnemi(18,198*16,56*16);
            gpJeu->ajouteEnnemi(18,185*16,64*16);
            gpJeu->ajouteEnnemi(55,230*16,37*16);gpJeu->ajouteEnnemi(55,108*16+9,81*16);
            gpJeu->ajouteEnnemi(55,128*16+9,81*16);gpJeu->ajouteEnnemi(55,109*16-8,42*16);
            gpJeu->ajouteEnnemi(55,129*16-8,42*16);gpJeu->ajouteEnnemi(55,119*16-8,58*16);
            gpJeu->ajouteEnnemi(55,6*16-8,64*16);gpJeu->ajouteEnnemi(55,13*16-8,64*16);
            gpJeu->ajouteEnnemi(55,68*16-8,58*16);gpJeu->ajouteEnnemi(55,9*16-8,43*16);
            gpJeu->ajouteEnnemi(55,171*16-8,81*16);
            //boss
            if(gpJoueur->getCoeur(7)==0) gpJeu->ajouteEnnemi(28,89*16,17*16);
            //graal
            if (!gpJoueur->hasCristal(4)) gpJeu->ajouteObjet(11,16*89+8,16*5,0,1);
            //objet
            gpJeu->ajouteObjet(4,16*69,16*23-4,0,1);gpJeu->ajouteObjet(4,16*70,16*23-4,0,1);
            gpJeu->ajouteObjet(4,16*175,16*34-4,0,1);gpJeu->ajouteObjet(4,16*217,16*32-4,0,1);
            gpJeu->ajouteObjet(4,16*237,16*50-4,0,1);gpJeu->ajouteObjet(4,16*84,16*34-4,0,1);
            gpJeu->ajouteObjet(5,16*5,16*34,0,1);gpJeu->ajouteObjet(5,16*69,16*25,0,1);
            gpJeu->ajouteObjet(5,16*176,16*34,0,1);gpJeu->ajouteObjet(5,16*222,16*40,0,1);
            gpJeu->ajouteObjet(5,16*237,16*54,0,1);gpJeu->ajouteObjet(5,16*95,16*34,0,1);
            gpJeu->ajouteObjet(6,16*70,16*25,0,1);gpJeu->ajouteObjet(6,16*177,16*34,0,1);
            gpJeu->ajouteObjet(7,16*69,16*24-4,0,1);gpJeu->ajouteObjet(7,16*70,16*24-4,0,1);
            gpJeu->ajouteObjet(7,16*183,16*32-4,0,1);
            break;
        case 23 :
            //caisses
            gpJeu->ajouteCaisse(4,51*16,36*16);gpJeu->ajouteCaisse(4,51*16,37*16);
            gpJeu->ajouteCaisse(4,51*16,38*16);gpJeu->ajouteCaisse(4,53*16,36*16);
            gpJeu->ajouteCaisse(4,53*16,37*16);gpJeu->ajouteCaisse(4,53*16,38*16);
            //pièges
            gpJeu->ajouteEnnemi(17,70*16,51*16-8);gpJeu->ajouteEnnemi(17,70*16,66*16-8);gpJeu->ajouteEnnemi(17,189*16,30*16);
            gpJeu->ajouteEnnemi(17,190*16,31*16);gpJeu->ajouteEnnemi(17,252*16,38*16);gpJeu->ajouteEnnemi(17,252*16,30*16);
            gpJeu->ajouteEnnemi(17,239*16,28*16);gpJeu->ajouteEnnemi(17,226*16,28*16);
            gpJeu->ajouteEnnemi(61,184*16,21*16-8);
            gpJeu->ajouteEnnemi(36,22*16,42*16); setValeur(22*16,42*16,2797,2);
            gpJeu->ajouteEnnemi(36,37*16,32*16); setValeur(37*16,32*16,2797,2);
            gpJeu->ajouteEnnemi(36,42*16,27*16); setValeur(42*16,27*16,2797,2);
            gpJeu->ajouteEnnemi(36,57*16,27*16); setValeur(57*16,27*16,2797,2);
            gpJeu->ajouteEnnemi(36,65*16,67*16); setValeur(65*16,67*16,2797,2);
            gpJeu->ajouteEnnemi(36,68*16,49*16); setValeur(68*16,49*16,2797,2);
            gpJeu->ajouteEnnemi(70,160*16,4*16);
            //ennemi
            gpJeu->ajouteEnnemi(62,149*16+4,3*16);gpJeu->ajouteEnnemi(62,69*16+4,3*16);
            gpJeu->ajouteEnnemi(62,9*16+4,20*16);
            gpJeu->ajoutePiege(51,261*16,1*16);gpJeu->ajoutePiege(51,261*16,61*16);
            gpJeu->ajoutePiege(51,301*16,31*16);gpJeu->ajoutePiege(51,341*16,1*16);
            gpJeu->ajoutePiege(51,341*16,61*16);
            gpJeu->ajouteEnnemi(7,344*16,6*16);gpJeu->ajouteEnnemi(7,355*16,6*16);
            gpJeu->ajouteEnnemi(7,349*16+8,2*16);gpJeu->ajouteEnnemi(7,349*16+8,10*16);
            gpJeu->ajouteEnnemi(7,249*16,52*16);gpJeu->ajouteEnnemi(7,173*16,23*16);
            gpJeu->ajouteEnnemi(7,233*16,28*16);gpJeu->ajouteEnnemi(7,105*16,50*16);
            gpJeu->ajouteEnnemi(7,114*16,50*16);gpJeu->ajouteEnnemi(7,9*16+8,65*16);
            gpJeu->ajouteEnnemi(30,89*16+8,67*16-4);gpJeu->ajouteEnnemi(30,196*16,22*16+4);
            gpJeu->ajouteEnnemi(30,196*16,30*16+4);gpJeu->ajouteEnnemi(30,129*16+8,67*16-4);
            gpJeu->ajouteEnnemi(30,237*16,38*16+4);gpJeu->ajouteEnnemi(30,241*16+8,23*16-4);
            gpJeu->ajouteEnnemi(30,223*16+8,20*16+4);gpJeu->ajouteEnnemi(30,109*16+8,5*16-4);
            gpJeu->ajouteEnnemi(30,49*16+8,37*16-4);gpJeu->ajouteEnnemi(29,86*16,35*16-5);
            gpJeu->ajouteEnnemi(29,69*16+8,22*16-5);gpJeu->ajouteEnnemi(29,109*16+8,36*16-5);
            gpJeu->ajouteEnnemi(29,225*16+8,67*16-5);gpJeu->ajouteEnnemi(29,244*16,8*16-5);
            gpJeu->ajouteEnnemi(29,255*16,8*16-5);gpJeu->ajouteEnnemi(29,93*16,35*16-5);
            gpJeu->ajouteEnnemi(29,105*16,63*16-5);gpJeu->ajouteEnnemi(29,114*16,63*16-5);
            gpJeu->ajouteEnnemi(29,166*16,67*16-5);gpJeu->ajouteEnnemi(29,253*16,67*16-5);
            gpJeu->ajouteEnnemi(29,145*16+8,61*16-5);gpJeu->ajouteEnnemi(29,150*16,68*16-5);
            gpJeu->ajouteEnnemi(29,105*16,20*16-5);gpJeu->ajouteEnnemi(29,114*16,20*16-5);
            gpJeu->ajouteEnnemi(29,149*16+8,29*16-5);gpJeu->ajouteEnnemi(29,9*16+8,36*16-5);
            gpJeu->ajouteEnnemi(29,225*16,49*16-5);gpJeu->ajouteEnnemi(29,234*16,49*16-5);
            //boss
            if(gpJoueur->getCoeur(8)==0) gpJeu->ajouteEnnemi(71,209*16-10,32*16);
            //grande fée
            if (!gpJoueur->getCoffre(8,19)) gpJeu->ajoutePnj(16,209*16,19*16+8,176);
            //objet
            gpJeu->ajouteObjet(3,16*84,16*62,0,1);gpJeu->ajouteObjet(3,16*344,16*2,0,1);
            gpJeu->ajouteObjet(5,16*95,16*32,0,1);gpJeu->ajouteObjet(5,16*204,16*49,0,1);
            gpJeu->ajouteObjet(5,16*215,16*49,0,1);gpJeu->ajouteObjet(5,16*355,16*2,0,1);
            gpJeu->ajouteObjet(6,16*84,16*32,0,1);gpJeu->ajouteObjet(6,16*135,16*62,0,1);
            gpJeu->ajouteObjet(6,16*175,16*17,0,1);gpJeu->ajouteObjet(6,16*344,16*10,0,1);
            gpJeu->ajouteObjet(7,16*49,16*39-4,0,1);gpJeu->ajouteObjet(7,16*117,16*62-4,0,1);
            gpJeu->ajouteObjet(7,16*184,16*47-4,0,1);gpJeu->ajouteObjet(7,16*164,16*70-4,0,1);
            gpJeu->ajouteObjet(7,16*355,16*10-4,0,1);
            break;
        case 24 :
            //pièges
            gpJeu->ajouteEnnemi(17,26*16,8*16);gpJeu->ajouteEnnemi(17,31*16,8*16);
            gpJeu->ajouteEnnemi(17,74*16,49*16);gpJeu->ajouteEnnemi(17,75*16,48*16);
            gpJeu->ajouteEnnemi(17,76*16,47*16);gpJeu->ajouteEnnemi(17,83*16,49*16);
            gpJeu->ajouteEnnemi(17,84*16,48*16);gpJeu->ajouteEnnemi(17,85*16,47*16);
            gpJeu->ajouteEnnemi(61,6*16,13*16);gpJeu->ajouteEnnemi(61,23*16,64*16);
            gpJeu->ajouteEnnemi(61,78*16,5*16);gpJeu->ajouteEnnemi(61,84*16,17*16);
            gpJeu->ajouteEnnemi(61,66*16,32*16);gpJeu->ajouteEnnemi(61,216*16,34*16);
            gpJeu->ajouteEnnemi(61,286*16,2*16-8);gpJeu->ajouteEnnemi(61,286*16,27*16+8);
            gpJeu->ajouteEnnemi(61,220*16,2*16-8);
            gpJeu->ajouteEnnemi(65,275*16,61*16); setValeur(275*16,61*16,27,2);
            gpJeu->ajouteEnnemi(65,277*16,49*16); setValeur(277*16,49*16,27,2);
            gpJeu->ajouteEnnemi(65,293*16,57*16); setValeur(293*16,57*16,27,2);
            //ennemi
            gpJeu->ajouteEnnemi(62,9*16+4,39*16);gpJeu->ajouteEnnemi(62,272*16+4,19*16);
            gpJeu->ajouteEnnemi(2,128*16-2,51*16);gpJeu->ajouteEnnemi(2,133*16-2,52*16);
            gpJeu->ajouteEnnemi(2,169*16-2+8,35*16);gpJeu->ajouteEnnemi(2,193*16-2,21*16);
            gpJeu->ajouteEnnemi(2,183*16-2,51*16);gpJeu->ajouteEnnemi(2,113*16-2,51*16);
            gpJeu->ajouteEnnemi(2,169*16-2+8,70*16);gpJeu->ajouteEnnemi(2,194*16-2,69*16);
            gpJeu->ajouteEnnemi(26,15*16,55*16);gpJeu->ajouteEnnemi(26,27*16,55*16);
            gpJeu->ajouteEnnemi(26,9*16,67*16);gpJeu->ajouteEnnemi(26,91*16,41*16);
            gpJeu->ajouteEnnemi(26,86*16,67*16);gpJeu->ajouteEnnemi(26,222*16,65*16);
            gpJeu->ajouteEnnemi(26,210*16,14*16);gpJeu->ajouteEnnemi(26,216*16,14*16);
            gpJeu->ajouteEnnemi(26,217*16,20*16);gpJeu->ajouteEnnemi(26,228*16,9*16);
            gpJeu->ajouteEnnemi(26,232*16,20*16);gpJeu->ajouteEnnemi(26,235*16,3*16);
            gpJeu->ajouteEnnemi(26,228*16,48*16);
            gpJeu->ajouteEnnemi(41,125*16-1,65*16);gpJeu->ajouteEnnemi(41,127*16-1,70*16);
            gpJeu->ajouteEnnemi(41,246*16-1,21*16);gpJeu->ajouteEnnemi(41,253*16-1,21*16);
            gpJeu->ajouteEnnemi(41,65*16-1,52*16);gpJeu->ajouteEnnemi(41,71*16-1,69*16);
            gpJeu->ajouteEnnemi(41,294*16-1,35*16);gpJeu->ajouteEnnemi(41,285*16-1,39*16);
            gpJeu->ajouteEnnemi(42,90*16-5,7*16);gpJeu->ajouteEnnemi(42,247*16-5,51*16);
            gpJeu->ajouteEnnemi(42,249*16-5,55*16);gpJeu->ajouteEnnemi(42,254*16-5,52*16);
            gpJeu->ajouteEnnemi(42,165*16-5,51*16);gpJeu->ajouteEnnemi(42,174*16-5,51*16);
            gpJeu->ajouteEnnemi(42,229*16-5+8,36*16);gpJeu->ajouteEnnemi(42,205*16-5,37*16);
            gpJeu->ajouteEnnemi(42,104*16-5,18*16);gpJeu->ajouteEnnemi(42,108*16-5,12*16);
            gpJeu->ajouteEnnemi(42,114*16-5,16*16);gpJeu->ajouteEnnemi(42,125*16-5+8,13*16);
            //boss
            if(gpJoueur->getCoeur(9)==0) gpJeu->ajouteEnnemi(43,49*16,17*16);
            //grande fée
            if (!gpJoueur->getCoffre(9,24)) gpJeu->ajoutePnj(16,49*16,4*16+8,176);
            //objet
            gpJeu->ajouteObjet(5,16*144,16*57,0,1);gpJeu->ajouteObjet(5,16*204,16*34,0,1);
            gpJeu->ajouteObjet(5,16*222,16*57,0,1);gpJeu->ajouteObjet(6,16*30,16*63,0,1);
            gpJeu->ajouteObjet(6,16*31,16*63,0,1);gpJeu->ajouteObjet(6,16*225,16*52,0,1);
            gpJeu->ajouteObjet(7,16*114,16*70-4,0,1);gpJeu->ajouteObjet(7,16*235,16*40-4,0,1);
            gpJeu->ajouteObjet(7,16*220,16*62-4,0,1);
            break;
        case 25 :
            //caisses
            gpJeu->ajouteCaisse(1,17*16,4*16); gpJeu->ajouteCaisse(1,17*16,5*16);
            gpJeu->ajouteCaisse(1,16*16,5*16);gpJeu->ajouteCaisse(1,16*16,6*16);
            gpJeu->ajouteCaisse(1,16*16,7*16); gpJeu->ajouteCaisse(1,16*16,8*16);
            gpJeu->ajouteCaisse(1,16*16,9*16); gpJeu->ajouteCaisse(1,17*16,9*16);
            gpJeu->ajouteCaisse(1,17*16,10*16);
            //pièges
            gpJeu->ajouteEnnemi(17,228*16,66*16);gpJeu->ajouteEnnemi(17,229*16,66*16);
            gpJeu->ajouteEnnemi(17,230*16,66*16);gpJeu->ajouteEnnemi(17,231*16,66*16);
            gpJeu->ajouteEnnemi(20,29*16+8,21*16);gpJeu->ajouteEnnemi(20,9*16+8,63*16);
            gpJeu->ajouteEnnemi(20,169*16+8,112*16);
            gpJeu->ajouteEnnemi(20,205*16+8,95*16);mur[205*2+1][95*2]=2;mur[206*2][95*2]=2;mur[205*2+1][95*2+1]=2;mur[206*2][95*2+1]=2;
            gpJeu->ajouteEnnemi(36,26*16,11*16);gpJeu->ajouteEnnemi(36,35*16,6*16);
            gpJeu->ajouteEnnemi(36,44*16+8,8*16);gpJeu->ajouteEnnemi(36,53*16,3*16);
            gpJeu->ajouteEnnemi(36,69*16,37*16);gpJeu->ajouteEnnemi(36,70*16,37*16);
            gpJeu->ajouteEnnemi(61,46*16,49*16-8);gpJeu->ajouteEnnemi(61,56*16,49*16-8);
            gpJeu->ajouteEnnemi(61,66*16,49*16-8);gpJeu->ajouteEnnemi(61,146*16,80*16-8);
            gpJeu->ajouteEnnemi(61,146*16,99*16+8);
            //ennemi
            gpJeu->ajouteEnnemi(62,23*16+4,79*16);gpJeu->ajouteEnnemi(62,35*16+4,79*16);
            gpJeu->ajouteEnnemi(62,154*16+4,64*16);
            gpJeu->ajouteEnnemi(26,29*16+9,37*16);gpJeu->ajouteEnnemi(26,46*16+1,26*16);
            gpJeu->ajouteEnnemi(26,84*16+1,70*16);gpJeu->ajouteEnnemi(26,84*16+1,78*16);
            gpJeu->ajouteEnnemi(26,95*16+1,72*16);gpJeu->ajouteEnnemi(26,95*16+1,80*16);
            gpJeu->ajouteEnnemi(26,111*16+1,67*16);gpJeu->ajouteEnnemi(26,165*16+1,108*16);
            gpJeu->ajouteEnnemi(26,165*16+1,116*16);gpJeu->ajouteEnnemi(26,174*16+1,108*16);
            gpJeu->ajouteEnnemi(26,174*16+1,116*16);gpJeu->ajouteEnnemi(26,165*16+1,70*16);
            gpJeu->ajouteEnnemi(26,165*16+1,76*16);gpJeu->ajouteEnnemi(26,174*16+1,70*16);
            gpJeu->ajouteEnnemi(26,174*16+1,76*16);gpJeu->ajouteEnnemi(26,205*16+9,81*16);
            gpJeu->ajouteEnnemi(26,198*16+9,78*16);gpJeu->ajouteEnnemi(26,209*16+1,88*16+8);
            gpJeu->ajouteEnnemi(26,4*16+1,26*16);gpJeu->ajouteEnnemi(26,4*16+1,38*16);
            gpJeu->ajouteEnnemi(26,15*16+1,20*16);gpJeu->ajouteEnnemi(26,15*16+1,32*16);
            gpJeu->ajouteEnnemi(30,7*16+8,83*16-4);gpJeu->ajouteEnnemi(30,9*16+8,85*16-4);
            gpJeu->ajouteEnnemi(30,11*16+8,87*16-4);gpJeu->ajouteEnnemi(30,37*16,10*16-4);
            gpJeu->ajouteEnnemi(30,46*16,4*16-4);
            gpJeu->ajouteEnnemi(32,26*16,97*16-7);gpJeu->ajouteEnnemi(32,53*16,97*16-7);
            gpJeu->ajouteEnnemi(32,26*16,113*16-7);gpJeu->ajouteEnnemi(32,53*16,113*16-7);
            gpJeu->ajouteEnnemi(32,25*16,33*16-7);gpJeu->ajouteEnnemi(32,34*16,33*16-7);
            gpJeu->ajouteEnnemi(32,71*16,5*16-7);gpJeu->ajouteEnnemi(32,63*16,37*16-7);
            gpJeu->ajouteEnnemi(32,76*16,37*16-7);gpJeu->ajouteEnnemi(32,145*16,37*16-7);
            gpJeu->ajouteEnnemi(32,154*16,37*16-7);gpJeu->ajouteEnnemi(32,129*16+8,23*16-7);
            gpJeu->ajouteEnnemi(32,86*16,5*16-7);gpJeu->ajouteEnnemi(32,93*16,5*16-7);
            gpJeu->ajouteEnnemi(32,105*16,8*16-7);gpJeu->ajouteEnnemi(32,114*16,8*16-7);
            gpJeu->ajouteEnnemi(32,86*16,97*16-7);gpJeu->ajouteEnnemi(32,133*16,67*16-7);
            gpJeu->ajouteEnnemi(32,105*16,67*16-7);gpJeu->ajouteEnnemi(32,146*16,112*16-7);
            gpJeu->ajouteEnnemi(32,4*16,66*16-7);gpJeu->ajouteEnnemi(32,15*16,66*16-7);
            gpJeu->ajouteEnnemi(33,14*16-3,112*16-9);gpJeu->ajouteEnnemi(33,65*16-3,112*16-9);
            gpJeu->ajouteEnnemi(33,66*16-3,82*16-9);gpJeu->ajouteEnnemi(33,73*16-3,82*16-9);
            gpJeu->ajouteEnnemi(33,5*16-3,3*16-9);gpJeu->ajouteEnnemi(33,14*16-3,3*16-9);
            gpJeu->ajouteEnnemi(33,73*16-3,24*16-9);gpJeu->ajouteEnnemi(33,5*16-3,51*16-9);
            gpJeu->ajouteEnnemi(33,14*16-3,51*16-9);gpJeu->ajouteEnnemi(33,112*16-3,52*16-9);
            gpJeu->ajouteEnnemi(33,147*16-3,52*16-9);gpJeu->ajouteEnnemi(33,86*16-3,25*16-9);
            gpJeu->ajouteEnnemi(33,98*16-3,21*16-9);gpJeu->ajouteEnnemi(33,100*16-3,36*16-9);
            gpJeu->ajouteEnnemi(33,113*16-3,34*16-9);gpJeu->ajouteEnnemi(33,105*16-3,92*16-9);
            gpJeu->ajouteEnnemi(33,121*16-3,97*16-9);gpJeu->ajouteEnnemi(33,186*16-3,67*16-9);
            gpJeu->ajouteEnnemi(33,227*16-3,112*16-9);gpJeu->ajouteEnnemi(33,135*16-3,78*16-9);
            gpJeu->ajouteEnnemi(34,69*16-4+8,97*16-9);gpJeu->ajouteEnnemi(34,9*16-4+8,8*16-9);
            gpJeu->ajouteEnnemi(34,29*16-4+8,52*16-9);gpJeu->ajouteEnnemi(34,127*16-4,7*16-9);
            gpJeu->ajouteEnnemi(34,132*16-4,112*16-9);
            switch (gpJoueur->getCoffre(10,20)) {
                case 0 : gpJeu->ajouteEnnemi(72,56*16,36*16); break;
                case 1 : gpJeu->ajouteEnnemi(72,136*16,36*16); break;
                case 2 : gpJeu->ajouteEnnemi(72,39*16,104*16); break;
                case 3 : gpJeu->ajouteEnnemi(72,103*16,115*16); break;
                case 4 : gpJeu->ajouteEnnemi(72,194*16,111*16); break;
            }
            //boss
            if(gpJoueur->getCoeur(10)==0) {
                gpJeu->ajouteEnnemi(73,180*16,-152); gpJeu->ajouteEnnemi(74,200*16-104,-152);}
            //grande fée
            if (!gpJoueur->getCoffre(10,21)) gpJeu->ajoutePnj(16,49*16,64*16+8,176);
            //objet
            gpJeu->ajouteObjet(3,16*62,16*79,0,1);
            gpJeu->ajouteObjet(4,16*46,16*79-4,0,1);gpJeu->ajouteObjet(4,16*130,16*21-4,0,1);
            gpJeu->ajouteObjet(5,16*46,16*85,0,1);
            gpJeu->ajouteObjet(6,16*7,16*100,0,1);gpJeu->ajouteObjet(6,16*53,16*85,0,1);
            gpJeu->ajouteObjet(7,16*53,16*79-4,0,1);
            break;
        case 26 :
            //boss
            if(gpJoueur->hasObjet(O_MASQUE)<2) gpJeu->ajouteEnnemi(75,309*16+6,32*16);
            if(gpJoueur->getCoeur(11)==0 && gpJoueur->hasObjet(O_MASQUE)==2) 
                gpJeu->ajouteObjet(I_COEUR,16*309+8,16*37,0,1);
            if(gpJoueur->getCoffre(11,24)==0) gpJeu->ajouteEnnemi(50,309*16-10,17*16);
            //pièges
            gpJeu->ajoutePiege(51,62*16,47*16);gpJeu->ajoutePiege(51,62*16,32*16);
            gpJeu->ajoutePiege(51,62*16,2*16);gpJeu->ajoutePiege(51,42*16,32*16);
            gpJeu->ajoutePiege(51,22*16,47*16);gpJeu->ajoutePiege(51,102*16,47*16);
            gpJeu->ajouteEnnemi(49,2*16,32*16); setValeur(2*16,32*16,2669,2);
            gpJeu->ajouteEnnemi(49,2*16,42*16); setValeur(2*16,42*16,2669,2);
            gpJeu->ajouteEnnemi(49,39*16,94*16); setValeur(39*16,94*16,2669,2);
            gpJeu->ajouteEnnemi(49,100*16,94*16); setValeur(100*16,94*16,2669,2);
            gpJeu->ajouteEnnemi(49,9*16,67*16); setValeur(9*16,67*16,2669,2);
            gpJeu->ajouteEnnemi(49,82*16,47*16); setValeur(82*16,47*16,2669,2);
            gpJeu->ajouteEnnemi(49,97*16,47*16); setValeur(97*16,47*16,2669,2);
            gpJeu->ajouteEnnemi(49,99*16,20*16); setValeur(99*16,20*16,2669,2);
            //ennemi
            gpJeu->ajouteEnnemi(62,49*16+4,51*16-4);gpJeu->ajouteEnnemi(62,6*16+4,37*16-4);
            gpJeu->ajouteEnnemi(62,129*16+4,22*16-4);gpJeu->ajouteEnnemi(62,129*16+4,37*16-4);
            gpJeu->ajouteEnnemi(16,49*16+8,37*16-4);gpJeu->ajouteEnnemi(16,38*16+8,20*16+4);
            gpJeu->ajouteEnnemi(16,40*16,22*16-4);gpJeu->ajouteEnnemi(16,41*16+8,23*16+4);
            gpJeu->ajouteEnnemi(16,29*16+8,52*16-4);gpJeu->ajouteEnnemi(16,69*16+8,22*16-4);
            gpJeu->ajouteEnnemi(16,109*16+8,52*16-4);gpJeu->ajouteEnnemi(16,89*16+8,52*16-4);
            gpJeu->ajouteEnnemi(16,98*16+8,35*16+4);gpJeu->ajouteEnnemi(16,100*16,37*16-4);
            gpJeu->ajouteEnnemi(16,101*16+8,38*16+4);
            gpJeu->ajouteEnnemi(56,65*16,94*16);gpJeu->ajouteEnnemi(56,74*16,94*16);
            gpJeu->ajouteEnnemi(56,29*16+8,97*16);gpJeu->ajouteEnnemi(56,49*16+8,97*16);
            gpJeu->ajouteEnnemi(56,89*16+8,97*16);gpJeu->ajouteEnnemi(56,109*16+8,97*16);
            gpJeu->ajouteEnnemi(56,30*16,69*16);gpJeu->ajouteEnnemi(56,51*16,69*16);
            gpJeu->ajouteEnnemi(56,40*16,82*16);gpJeu->ajouteEnnemi(56,11*16,81*16);
            gpJeu->ajouteEnnemi(56,7*16,50*16);gpJeu->ajouteEnnemi(56,12*16,50*16);
            gpJeu->ajouteEnnemi(56,65*16,80*16);gpJeu->ajouteEnnemi(56,74*16,80*16);
            gpJeu->ajouteEnnemi(56,11*16,23*16);gpJeu->ajouteEnnemi(56,29*16+8,38*16);
            gpJeu->ajouteEnnemi(56,49*16,6*16);gpJeu->ajouteEnnemi(56,90*16,6*16);
            gpJeu->ajouteEnnemi(56,30*16,7*16);gpJeu->ajouteEnnemi(56,89*16+8,82*16);
            gpJeu->ajouteEnnemi(56,85*16,65*16);gpJeu->ajouteEnnemi(56,94*16,65*16);
            gpJeu->ajouteEnnemi(56,108*16,73*16);gpJeu->ajouteEnnemi(56,115*16,85*16);
            gpJeu->ajouteEnnemi(56,122*16,68*16);gpJeu->ajouteEnnemi(56,130*16,80*16);
            gpJeu->ajouteEnnemi(56,127*16,50*16);gpJeu->ajouteEnnemi(56,132*16,50*16);
            gpJeu->ajouteEnnemi(56,109*16,8*16);gpJeu->ajouteEnnemi(56,90*16,23*16);
            gpJeu->ajouteEnnemi(56,109*16,23*16);gpJeu->ajouteEnnemi(56,289*16,37*16);
            gpJeu->ajouteEnnemi(56,329*16,37*16);
            //objet
            gpJeu->ajouteObjet(3,16*37,16*34,0,1);gpJeu->ajouteObjet(6,16*25,16*34,0,1);
            gpJeu->ajouteObjet(5,16*57,16*71,0,1);gpJeu->ajouteObjet(5,16*135,16*80,0,1);
            break;
        case 27 :
            //boss
            if(!gpJoueur->getCoeur(12)) gpJeu->ajouteEnnemi(76,28*16+2,47*16);
            //pièges
            gpJeu->ajouteEnnemi(49,22*16,47*16); setValeur(22*16,47*16,3728,2);
            gpJeu->ajouteEnnemi(49,37*16,47*16); setValeur(37*16,47*16,3728,2);
            //ennemi
            gpJeu->ajouteEnnemi(44,25*16,124*16-8);gpJeu->ajouteEnnemi(44,34*16,124*16-8);
            gpJeu->ajouteEnnemi(44,13*16,123*16-8);gpJeu->ajouteEnnemi(44,13*16,131*16-8);
            gpJeu->ajouteEnnemi(44,13*16,63*16-8);gpJeu->ajouteEnnemi(44,13*16,71*16-8);
            gpJeu->ajouteEnnemi(44,52*16,130*16-8);gpJeu->ajouteEnnemi(44,47*16,112*16-8);
            gpJeu->ajouteEnnemi(44,70*16,116*16-8);gpJeu->ajouteEnnemi(44,94*16,112*16-8);
            gpJeu->ajouteEnnemi(44,24*16,97*16-8);gpJeu->ajouteEnnemi(44,29*16,101*16-8);
            gpJeu->ajouteEnnemi(44,35*16,97*16-8);gpJeu->ajouteEnnemi(44,36*16-16,5*16-8);
            gpJeu->ajouteEnnemi(44,45*16-16,5*16-8);gpJeu->ajouteEnnemi(44,26*16-16,14*16-8);
            gpJeu->ajouteEnnemi(44,36*16-16,14*16-8);gpJeu->ajouteEnnemi(44,45*16-16,14*16-8);
            gpJeu->ajouteEnnemi(44,55*16-16,14*16-8);gpJeu->ajouteEnnemi(44,36*16-16,24*16-8);
            gpJeu->ajouteEnnemi(44,45*16-16,24*16-8);gpJeu->ajouteEnnemi(44,50*16-16,67*16-8);
            gpJeu->ajouteEnnemi(44,79*16-16,64*16-8);gpJeu->ajouteEnnemi(44,85*16-16,67*16-8);
            gpJeu->ajouteEnnemi(44,79*16-16,70*16-8);gpJeu->ajouteEnnemi(44,101*16-16,97*16-8);
            gpJeu->ajouteEnnemi(44,110*16-8,67*16-8);gpJeu->ajouteEnnemi(44,106*16-16,50*16-8);
            gpJeu->ajouteEnnemi(44,115*16-16,50*16-8);gpJeu->ajouteEnnemi(44,165*16-16,63*16-8);
            gpJeu->ajouteEnnemi(44,165*16-16,71*16-8);gpJeu->ajouteEnnemi(44,135*16-16,67*16-8);
            gpJeu->ajouteEnnemi(44,150*16-16,67*16-8);gpJeu->ajouteEnnemi(44,78*16-16,22*16-8);
            gpJeu->ajouteEnnemi(44,99*16-16,37*16-8);
            break;
        case 28 :
            //boss
            gpJeu->ajouteEnnemi(50,149*16-10,108*16);
            if (gpJoueur->getCoffre(13,10)!=1) gpJeu->ajouteEnnemi(31,49*16,122*16);
            //pièges
            gpJeu->ajouteEnnemi(49,34*16,17*16);gpJeu->ajouteEnnemi(49,65*16,17*16);
            gpJeu->ajouteEnnemi(49,44*16,57*16);gpJeu->ajouteEnnemi(49,55*16,57*16);
            //ennemi
            gpJeu->ajouteEnnemi(46,46*16-18,139*16-19);gpJeu->ajouteEnnemi(46,55*16-18,139*16-19);
            gpJeu->ajouteEnnemi(46,30*16-18,142*16-19);gpJeu->ajouteEnnemi(46,16*16-18,127*16-19);
            gpJeu->ajouteEnnemi(46,25*16-18,127*16-19);gpJeu->ajouteEnnemi(46,7*16-18,104*16-19);
            gpJeu->ajouteEnnemi(46,14*16-18,104*16-19);gpJeu->ajouteEnnemi(46,31*16-18,96*16-19);
            gpJeu->ajouteEnnemi(46,29*16-18,81*16-19);gpJeu->ajouteEnnemi(46,52*16-18,96*16-19);
            gpJeu->ajouteEnnemi(46,52*16-18,82*16-19);gpJeu->ajouteEnnemi(46,67*16-18,90*16-19);
            gpJeu->ajouteEnnemi(46,74*16-18,90*16-19);
            gpJeu->ajouteEnnemi(47,10*16-18+8,107*16-19);gpJeu->ajouteEnnemi(47,70*16-18+8,87*16-19);
            gpJeu->ajouteEnnemi(47,87*16-18,105*16-19);gpJeu->ajouteEnnemi(47,94*16-18,105*16-19);
            gpJeu->ajouteEnnemi(47,80*16-18,127*16-19);gpJeu->ajouteEnnemi(47,66*16-18,142*16-19);
            gpJeu->ajouteEnnemi(47,150*16-18+8,66*16-19);gpJeu->ajouteEnnemi(47,147*16-18+8,50*16-19);
            gpJeu->ajouteEnnemi(47,154*16-18+8,50*16-19);gpJeu->ajouteEnnemi(47,131*16-18+8,51*16-19);
            gpJeu->ajouteEnnemi(47,170*16-18+8,51*16-19);gpJeu->ajouteEnnemi(47,127*16-18+8,28*16-19);
            gpJeu->ajouteEnnemi(47,134*16-18+8,28*16-19);gpJeu->ajouteEnnemi(47,167*16-18+8,31*16-19);
            gpJeu->ajouteEnnemi(47,174*16-18+8,31*16-19);
            gpJeu->ajouteEnnemi(48,150*16-7,21*16-3);gpJeu->ajouteEnnemi(48,130*16-7+8,32*16-3);
            gpJeu->ajouteEnnemi(48,170*16-7+8,27*16-3);gpJeu->ajouteEnnemi(48,105*16-7,37*16-3);
            gpJeu->ajouteEnnemi(48,196*16-7,37*16-3);gpJeu->ajouteEnnemi(48,150*16-7+8,6*16-3);
            gpJeu->ajouteEnnemi(48,50*16-7+8,67*16-3);gpJeu->ajouteEnnemi(48,40*16-7,51*16-3);
            gpJeu->ajouteEnnemi(48,61*16-7,51*16-3);gpJeu->ajouteEnnemi(48,50*16-7+8,22*16-3);
            gpJeu->ajouteEnnemi(48,50*16-7+8,37*16-3);gpJeu->ajouteEnnemi(48,68*16-7,67*16-3);
            gpJeu->ajouteEnnemi(48,7*16-7,37*16-3);gpJeu->ajouteEnnemi(48,14*16-7,37*16-3);
            gpJeu->ajouteEnnemi(48,50*16-7+8,7*16-3);gpJeu->ajouteEnnemi(48,87*16-7,37*16-3);
            gpJeu->ajouteEnnemi(48,94*16-7,37*16-3);
            break;
        case 30 : if (gpJoueur->getMagieMax()==32) gpJeu->ajoutePnj(16,9*16,4*16+8,227); break;
        case 31 :
            if (!gpJoueur->getCoeur(27)) gpJeu->ajouteObjet(I_QUART_COEUR,9*16+8,4*16,27,1);
            break;
        case 39 : gpJeu->ajoutePnj(48,9*16,4*16+8,232); break;
        case 41 :
            gpJeu->ajouteEnnemi(21,67*16,10*16+1);gpJeu->ajouteEnnemi(21,81*16,6*16+1);
            gpJeu->ajouteEnnemi(37,51*16,21*16);gpJeu->ajouteEnnemi(37,5*16,6*16);
            gpJeu->ajouteEnnemi(37,14*16,6*16);gpJeu->ajouteEnnemi(39,49*16+8-4,8*16-5);
            gpJeu->ajouteEnnemi(38,45*16-5,4*16-5);gpJeu->ajouteEnnemi(38,54*16-5,4*16-5);
            break;
        case 43 :
            gpJeu->ajoutePnj(49,12*16,6*16,241);
            gpJeu->ajoutePnj(50,7*16,6*16,242);
            break;
        case 44 :
            gpJeu->ajouteObjet(4,16*4,16*12-4,0,1);gpJeu->ajouteObjet(4,16*5,16*12-4,0,1);
            if (gpJoueur->getAvancement()==1) gpJeu->ajoutePnj(47,9*16+8,6*16,245);
            if (gpJoueur->getAvancement()==2 && !gpJoueur->hasCristal(0)) gpJeu->ajoutePnj(47,9*16+8,6*16,250);
            if (gpJoueur->getAvancement()==2 && gpJoueur->hasCristal(0)) gpJeu->ajoutePnj(47,9*16+8,6*16,251);
            if (gpJoueur->getAvancement()>=3 && gpJoueur->getAvancement()<64) 
                gpJeu->ajoutePnj(47,9*16+8,6*16,250);
            if (gpJoueur->getAvancement()>=64 && gpJoueur->getAvancement()<=66) {
                gpJeu->ajoutePnj(47,7*16+8,5*16,272);
                gpJeu->ajoutePnj(15,11*16+8,5*16,259);
            }
            if (gpJoueur->getAvancement()==67) {
                gpJeu->ajoutePnj(47,7*16+8,5*16,274);
                gpJeu->ajoutePnj(15,11*16+8,5*16,273);
            }
            if (gpJoueur->getAvancement()==68) {
                gpJeu->ajoutePnj(47,7*16+8,5*16,274);
                gpJeu->ajoutePnj(15,11*16+8,5*16,276);
            }
            if (gpJoueur->getAvancement()==69) {
                gpJeu->ajoutePnj(47,7*16+8,5*16,274);
                gpJeu->ajoutePnj(15,11*16+8,5*16,282);
            }
            break;
        case 45 :
            gpJeu->ajoutePnj(32,9*16,5*16,283);
            break;
        case 46 :
            if (gpJoueur->hasObjet(O_SAC_BOMBES)) gpJeu->ajoutePnj(43,12*16,8*16,298);
            break;
        case 47 :
            if(gpJoueur->getCoeur(28)==0) {
                setValeur(6*16,6*16,303,1);
                setValeur(8*16,6*16,303,1);
                setValeur(10*16,6*16,303,1);
                setValeur(6*16,8*16,303,1);
                setValeur(8*16,8*16,303,1);
                setValeur(10*16,8*16,303,1);
                setValeur(6*16,10*16,303,1);
                setValeur(8*16,10*16,303,1);
                setValeur(10*16,10*16,303,1);
                contenu[6][6]=C_PAIE; contenu[8][6]=C_PAIE; contenu[10][6]=C_PAIE;
                contenu[6][8]=C_PAIE; contenu[8][8]=C_PAIE; contenu[10][8]=C_PAIE;
                contenu[6][10]=C_PAIE; contenu[8][10]=C_PAIE; contenu[10][10]=C_PAIE;
                gpJeu->ajoutePnj(20,14*16,11*16,300);
            }else {
                setValeur(6*16,6*16,304,1);
                setValeur(8*16,6*16,304,1);
                setValeur(10*16,6*16,304,1);
                setValeur(6*16,8*16,304,1);
                setValeur(8*16,8*16,304,1);
                setValeur(10*16,8*16,304,1);
                setValeur(6*16,10*16,304,1);
                setValeur(8*16,10*16,304,1);
                setValeur(10*16,10*16,304,1);
                gpJeu->ajoutePnj(20,14*16,11*16,299);
            }
            break;
        case 48 :
            if (gpJoueur->getAvancement()>=6 && !gpJoueur->hasObjet(O_SAC_BOMBES)) 
                gpJeu->ajoutePnj(14,24*16,4*16,306);
            else gpJeu->ajoutePnj(14,24*16,4*16,307);
            gpJeu->ajoutePnj(18,8*16,4*16,0); contenu[8][6]=305;
            gpJeu->ajouteObjet(1,16*20,16*12,0,1); gpJeu->ajouteObjet(1,16*4,16*7,0,1);
            break;
        case 49 :
            if (gpJoueur->getAvancement()==7) gpJeu->ajoutePnj(51,29*16+8,4*16,309);
            else gpJeu->ajoutePnj(51,29*16+8,4*16,308);
            break;
        case 50 :
            gpJeu->ajouteObjet(4,16*5,16*5,0,1); contenu[5][6]=313;
            gpJeu->ajouteObjet(7,16*8,16*5,0,1); contenu[8][6]=316;
            gpJeu->ajouteObjet(8,16*11,16*5,0,1); contenu[11][6]=318;
            gpJeu->ajoutePnj(12,13*16,9*16,320);
            break;
        case 51 :
            gpJeu->ajouteObjet(5,16*7,16*5,0,1); contenu[7][6]=321;
            gpJeu->ajouteObjet(6,16*10,16*5,0,1); contenu[10][6]=323;
            gpJeu->ajoutePnj(10,13*16,9*16,320);
            break;
        case 52 : gpJeu->ajoutePnj(17,9*16+7,7*16,327); break;
        case 53 :
            gpJeu->ajoutePnj(19,13*16,16*16+8,340);
            gpJeu->ajoutePnj(18,8*16,4*16,305); contenu[8][6]=305;
            gpJeu->ajouteObjet(1,16*4,16*27,0,1);
            gpJeu->ajouteObjet(2,16*15,16*2,0,1);
            if (!gpJeu->getJoueur()->getOnilink() && gpJeu->getJoueur()->getAvancement()==64) {
                gpJeu->ajoutePnj(11,5*16,24*16,81);
                gpJeu->ajoutePnj(35,14*16,26*16,84);
                gpJeu->ajoutePnj(36,9*16,9*16,85);
            }
            break;
        case 54 : gpJeu->ajoutePnj(52,9*16,5*16,343); break;
        case 55 :
            if (gpJeu->getJoueur()->getAvancement()==10 
            || gpJeu->getJoueur()->getAvancement()==11 
            || (gpJeu->getJoueur()->getAvancement()>=15
            && gpJeu->getJoueur()->getAvancement()<=64)) 
                gpJeu->ajoutePnj(25,29*16,6*16,50);
            else if (gpJeu->getJoueur()->getAvancement()==12) 
                gpJeu->ajoutePnj(25,29*16,6*16,349);
            else if (gpJeu->getJoueur()->getAvancement()==13) 
                gpJeu->ajoutePnj(25,29*16,6*16,354);
            else if (gpJeu->getJoueur()->getAvancement()==65 && !gpJoueur->hasMelodie(1)) 
                gpJeu->ajoutePnj(25,29*16,6*16,355);
            else gpJeu->ajoutePnj(25,29*16,6*16,358);
            if (gpJeu->getJoueur()->getAvancement()==65) gpJeu->ajoutePnj(45,9*16,6*16,359);
            gpJeu->ajouteObjet(1,16*17,16*12,0,1);gpJeu->ajouteObjet(1,16*22,16*12,0,1);
            break;
        case 56 : gpJeu->ajoutePnj(17,9*16+7,7*16,327); break;
        case 57 :
            gpJeu->ajouteObjet(1,16*14,16*12,0,1); gpJeu->ajouteObjet(4,16*15,16*12-4,0,1);
            if (!gpJeu->getJoueur()->getOnilink() && gpJeu->getJoueur()->getAvancement()==64)
            gpJeu->ajoutePnj(9,12*16+8,8*16,80);
            break;
        case 58 :
            if (!gpJeu->getJoueur()->getOnilink() && gpJeu->getJoueur()->getAvancement()==64) {
            if (gpJoueur->hasObjet(O_OCARINA)) gpJeu->ajoutePnj(34,11*16,8*16-8-4,83);
            else gpJeu->ajoutePnj(34,11*16,8*16-8-4,82);}
            break;
        case 59 :
            gpJeu->ajouteObjet(4,16*5,16*5,0,1); contenu[5][6]=313;
            gpJeu->ajouteObjet(7,16*8,16*5,0,1); contenu[8][6]=316;
            gpJeu->ajouteObjet(8,16*11,16*5,0,1); contenu[11][6]=318;
            gpJeu->ajoutePnj(20,13*16,9*16,320);
            break;
        case 60 :
            gpJeu->ajouteObjet(5,16*7,16*5,0,1); contenu[7][6]=321;
            gpJeu->ajouteObjet(6,16*10,16*5,0,1); contenu[10][6]=323;
            gpJeu->ajoutePnj(7,13*16,9*16,320);
            break;
        case 61 : gpJeu->ajoutePnj(28,9*16,8*16,360); break;
        case 62 : gpJeu->ajoutePnj(29,12*16,7*16,362); break;
        case 63 : gpJeu->ajoutePnj(31,8*16,7*16,365); break;
        case 64 : gpJeu->ajoutePnj(30,7*16,8*16,367); break;
        case 65 :
            gpJeu->ajouteObjet(4,16*5,16*5,0,1); contenu[5][6]=313;
            gpJeu->ajouteObjet(7,16*8,16*5,0,1); contenu[8][6]=316;
            gpJeu->ajouteObjet(8,16*11,16*5,0,1); contenu[11][6]=318;
            gpJeu->ajoutePnj(27,13*16,9*16,320);
            break;
        case 66 : gpJeu->ajoutePnj(53,10*16,6*16,369); break;
        case 67 :
            gpJeu->ajouteObjet(4,16*4,16*12-4,0,1); gpJeu->ajouteObjet(7,16*5,16*12-4,0,1);
            gpJeu->ajoutePnj(21,8*16,5*16,374);
            break;
        case 68 : gpJeu->ajoutePnj(54,9*16,7*16,381); break;
        case 69 :
            gpJeu->ajouteObjet(4,16*5,16*5,0,1); contenu[5][6]=313;
            gpJeu->ajouteObjet(7,16*7,16*5,0,1); contenu[7][6]=316;
            gpJeu->ajouteObjet(8,16*9,16*5,0,1); contenu[9][6]=318;
            gpJeu->ajouteObjet(5,16*11,16*5,0,1); contenu[11][6]=321;
            gpJeu->ajouteObjet(6,16*13,16*5,0,1); contenu[13][6]=323;
            gpJeu->ajoutePnj(7,13*16,9*16,320);
            break;
        case 70 : gpJeu->ajoutePnj(17,9*16+7,7*16,327); break;
        case 71 : gpJeu->ajoutePnj(55,9*16+7,5*16,382); break;
    }
}

void Monde::detRegion(int newZone) {
    region[0]=0; region[1]=0;
    region[2]=largeur;region[3]=hauteur;
    
    Joueur* gpJoueur = gpJeu->getJoueur();
    
    if (!gpJeu->isDehors()) {
        region[0]=((int)gpJoueur->getX()/320)*320; region[1]=((int)gpJoueur->getY()/240)*240;
        region[2]=region[0]+320;region[3]=region[1]+240;
        switch (newZone) {
            case 15 :
                if ((region[0]==20*16 || region[0]==40*16 || region[0]==60*16) 
                && (region[1]==15*16 || region[1]==30*16)) {
                    region[0]=20*16; region[2]=80*16; region[1]=15*16; region[3]=45*16; }
                if (region[1]==45*16 && (region[0]==0 || region[0]==20*16)) {
                    region[0]=0; region[2]=40*16; }
                if ((region[1]==30*16 || region[1]==45*16) 
                && (region[0]==120*16 || region[0]==140*16)) {
                    region[0]=120*16; region[2]=160*16; }
                if (region[0]==140*16 && (region[1]==0 || region[1]==15*16)) {
                    region[1]=0; region[3]=30*16; }
                if (region[0]==180*16 && (region[1]==15*16 || region[1]==30*16)) {
                    region[1]=15*16; region[3]=45*16; }
                if (region[0]==80*16 && region[1]==30*16) gpJeu->getAudio()->stopSpecial();
                break;
            case 16 :
                if (region[1]==0 && (region[0]==40*16 || region[0]==60*16)) {
                    region[0]=40*16; region[2]=80*16; }
                if (region[1]==45*16 && (region[0]==60*16 || region[0]==80*16)) {
                    region[0]=60*16; region[2]=100*16; }
                if (region[0]==100*16 && (region[1]==30*16 || region[1]==45*16)) {
                    region[1]=30*16; region[3]=60*16; }
                if (region[0]==180*16 && (region[1]==45*16 || region[1]==60*16)) {
                    region[1]=45*16; region[3]=75*16; }
                if (region[0]==200*16 && (region[1]==30*16 || region[1]==45*16)) {
                    region[1]=30*16; region[3]=60*16; }
                if (region[0]==220*16 && (region[1]==60*16 || region[1]==75*16)) {
                    region[1]=60*16; region[3]=90*16; }
                if ((region[0]==120*16 || region[0]==140*16 || region[0]==160*16) 
                && (region[1]==30*16 || region[1]==45*16)) {
                    region[0]=120*16; region[2]=180*16; region[1]=30*16; region[3]=60*16; }
                if ((region[0]==220*16 || region[0]==240*16 || region[0]==260*16) 
                && (region[1]==15*16 || region[1]==30*16)) {
                    region[0]=220*16; region[2]=280*16; region[1]=15*16; region[3]=45*16; }
                if ((region[0]==240*16 && region[1]==90*16)
                || (region[0]==260*16 && region[1]==60*16)) gpJeu->getAudio()->stopSpecial();
                break;
            case 17 :
                if ((region[0]==40*16 || region[0]==60*16 || region[0]==80*16) 
                && (region[1]==15*16 || region[1]==30*16)) {
                    region[0]=40*16; region[2]=100*16; region[1]=15*16; region[3]=45*16; }
                if ((region[0]==40*16 || region[0]==60*16 || region[0]==80*16) 
                && (region[1]==75*16 || region[1]==90*16)) {
                    region[0]=40*16; region[2]=100*16; region[1]=75*16; region[3]=105*16; }
                if ((region[0]==140*16 || region[0]==160*16) 
                && (region[1]==15*16 || region[1]==30*16)) {
                    region[0]=140*16; region[2]=180*16; region[1]=15*16; region[3]=45*16; }
                if ((region[0]==200*16 || region[0]==220*16) 
                && (region[1]==15*16 || region[1]==30*16)) {
                    region[0]=200*16; region[2]=240*16; region[1]=15*16; region[3]=45*16; }
                if ((region[0]==120*16 || region[0]==240*16) 
                && (region[1]==30*16 || region[1]==45*16 || region[1]==60*16)) {
                    region[1]=30*16; region[3]=75*16; }
                if (region[0]==0 && region[1]==45*16) gpJeu->getAudio()->stopSpecial();
                break;
            case 18 :
                if ((region[0]==0 || region[0]==20*16|| region[0]==40*16) 
                && (region[1]==15*16 || region[1]==30*16 || region[1]==45*16)) {
                    region[0]=0; region[2]=60*16; region[1]=15*16; region[3]=60*16; }
                if ((region[0]==60*16 || region[0]==80*16) && region[1]==45*16) {
                    region[0]=60*16; region[2]=100*16; }
                if ((region[0]==120*16 || region[0]==140*16) 
                && (region[1]==0 || region[1]==15*16)) {
                    region[0]=120*16; region[2]=160*16; region[1]=0; region[3]=30*16; }
                if ((region[0]==180*16 || region[0]==200*16) 
                && (region[1]==0 || region[1]==15*16)) {
                    region[0]=180*16; region[2]=220*16; region[1]=0; region[3]=30*16; }
                if ((region[0]==120*16 || region[0]==200*16 || region[0]==260*16) 
                && (region[1]==30*16 || region[1]==45*16)) {
                    region[1]=30*16; region[3]=60*16; }
                if ((region[0]==140*16 || region[0]==160*16 || region[0]==180*16) 
                && region[1]==60*16) {
                    region[0]=140*16; region[2]=200*16; }
                if ((region[0]==220*16 || region[0]==240*16) 
                && (region[1]==0 || region[1]==15*16)) {
                    region[0]=220*16; region[2]=260*16; region[1]=0; region[3]=30*16; }
                if ((region[0]==280*16 || region[0]==300*16) 
                && (region[1]==0 || region[1]==15*16)) {
                    region[0]=280*16; region[2]=320*16; region[1]=0; region[3]=30*16; }
                if ((region[0]==220*16 || region[0]==240*16) 
                && region[1]==60*16) {
                    region[0]=220*16; region[2]=260*16; }
                if ((region[0]==280*16 || region[0]==300*16) 
                && (region[1]==30*16 || region[1]==45*16 || region[1]==60*16)) {
                    region[0]=280*16; region[2]=320*16; }
                if (region[0]==260*16 && region[1]==30*16) gpJeu->getAudio()->stopSpecial();
                break;
            case 19 :
                if ((region[0]==60*16 || region[0]==80*16) 
                && (region[1]==15*16 || region[1]==30*16)) {
                    region[0]=60*16; region[2]=100*16; region[1]=15*16; region[3]=45*16; }
                if ((region[0]==0 || region[0]==20*16) 
                && (region[1]==0 || region[1]==45*16 || region[1]==60*16)) {
                    region[0]=0; region[2]=40*16; }
                if ((region[0]==0 || region[0]==20*16) 
                && (region[1]==15*16 || region[1]==30*16)) {
                    region[0]=0; region[2]=40*16; region[1]=15*16; region[3]=45*16; }
                if ((region[0]==60*16 || region[0]==80*16) && region[1]==60*16) {
                    region[0]=60*16; region[2]=100*16; }
                if ((region[0]==100*16 || region[0]==120*16) 
                && (region[1]==0 || region[1]==15*16)) {
                    region[1]=0; region[3]=30*16; }
                if ((region[0]==100*16 || region[0]==120*16) 
                && (region[1]==30*16 || region[1]==45*16)) {
                    region[0]=100*16; region[2]=140*16; region[1]=30*16; region[3]=60*16; }
                if ((region[0]==100*16 || region[0]==120*16) && region[1]==60*16) {
                    region[0]=100*16; region[2]=140*16; }
                if ((region[0]==140*16 || region[0]==160*16) 
                && (region[1]==30*16 || region[1]==45*16)) {
                    region[0]=140*16; region[2]=180*16; region[1]=30*16; region[3]=60*16; }
                if ((region[0]==160*16 || region[0]==180*16) && region[1]==0) {
                    region[0]=160*16; region[2]=200*16; }
                if (region[0]==180*16 && (region[1]==15*16 || region[1]==30*16)) {
                    region[1]=15*16; region[3]=45*16; }
                if (region[0]==80*16 && region[1]==45*16) gpJeu->getAudio()->stopSpecial();
                break;
            case 20 :
                if ((region[0]==160*16 || region[0]==180*16) 
                && (region[1]==0 || region[1]==15*16)) {
                    region[0]=160*16; region[2]=200*16; region[1]=0; region[3]=30*16; }
                if ((region[0]==160*16 || region[0]==180*16) 
                && (region[1]==60*15 || region[1]==75*16)) {
                    region[0]=160*16; region[2]=200*16; region[1]=60*16; region[3]=90*16; }
                if ((region[0]==220*16 || region[0]==240*16) 
                && (region[1]==0 || region[1]==15*16)) {
                    region[0]=220*16; region[2]=260*16; region[1]=0; region[3]=30*16; }
                if ((region[0]==220*16 || region[0]==240*16) 
                && (region[1]==60*15 || region[1]==75*16)) {
                    region[0]=220*16; region[2]=260*16; region[1]=60*16; region[3]=90*16; }
                if ((region[0]==120*16 || region[0]==140*16) 
                && (region[1]==15*16 || region[1]==30*16)) {
                    region[0]=120*16; region[2]=160*16; region[1]=15*16; region[3]=45*16; }
                if (region[0]==80*16 && (region[1]==45*16 || region[1]==60*16)) {
                    region[1]=45*16; region[3]=75*16; }
                if ((region[0]==20*16 || region[0]==40*16) 
                && (region[1]==15*16 || region[1]==30*16)) {
                    region[0]=20*16; region[2]=60*16; region[1]=15*16; region[3]=45*16; }
                if (region[0]==0 && region[1]==45*16) gpJeu->getAudio()->stopSpecial();
                break;
            case 21 :
                if ((region[0]==20*16 || region[0]==40*16 || region[0]==60*16) 
                && (region[1]==15*16 || region[1]==30*16)) {
                    region[0]=20*16; region[2]=80*16; }
                if ((region[0]==120*16 || region[0]==140*16 || region[0]==160*16) 
                && (region[1]==15*16 || region[1]==30*16)) {
                    region[0]=120*16; region[2]=180*16; region[1]=15*16; region[3]=45*16; }
                if ((region[0]==200*16 || region[0]==220*16) 
                && (region[1]==0 || region[1]==15*16)) {
                    region[0]=200*16; region[2]=240*16; region[1]=0; region[3]=30*16; }
                if ((region[0]==200*16 || region[0]==220*16) 
                && (region[1]==30*16 || region[1]==45*16)) {
                    region[0]=200*16; region[2]=240*16; }
                if (region[0]==260*16 && (region[1]==0 || region[1]==15*16)) {
                    region[1]=0; region[3]=30*16; }
                if ((region[0]==260*16 || region[0]==280*16) 
                && (region[1]==30*16 || region[1]==45*16)) {
                    region[0]=260*16; region[2]=300*16; region[1]=30*16; region[3]=60*16; }
                if (region[0]==240*16 && region[1]==30*16) gpJeu->getAudio()->stopSpecial();
                break;
            case 22 :
                if (region[0]==0 && (region[1]==30*16 || region[1]==45*16)) {
                    region[1]=30*16; region[3]=60*16; }
                if ((region[0]==60*16 || region[0]==160*16 || region[0]==220*16) 
                && (region[1]==45*16 || region[1]==60*16)) {
                    region[1]=45*16; region[3]=75*16; }
                if ((region[0]==20*16 || region[0]==40*16) && region[1]==75*16) {
                    region[0]=20*16; region[2]=60*16; }
                if ((region[0]==100*16 || region[0]==120*16) && region[1]==75*16) {
                    region[0]=100*16; region[2]=140*16; }
                if ((region[0]==120*16 || region[0]==140*16) && region[1]==15*16) {
                    region[0]=120*16; region[2]=160*16; }
                if ((region[0]==180*16 || region[0]==200*16) && region[1]==30*16) {
                    region[0]=180*16; region[2]=220*16; }
                if ((region[0]==200*16 || region[0]==220*16) 
                && (region[1]==0 || region[1]==15*16)) {
                    region[0]=200*16; region[2]=240*16; }
                if ((region[0]==20*16 || region[0]==40*16) 
                && (region[1]==30*16 || region[1]==45*16 || region[1]==60*16)) {
                    region[0]=20*16; region[2]=60*16; region[1]=30*16; region[3]=75*16; }
                if ((region[0]==100*16 || region[0]==120*16) 
                && (region[1]==30*16 || region[1]==45*16 || region[1]==60*16)) {
                    region[0]=100*16; region[2]=140*16; region[1]=30*16; region[3]=75*16; }
                if ((region[0]==160*16 || region[0]==180*16) 
                && (region[1]==0 || region[1]==15*16)) {
                    region[0]=160*16; region[2]=200*16; region[1]=0; region[3]=30*16; }
                if ((region[0]==180*16 || region[0]==200*16) 
                && (region[1]==45*16 || region[1]==60*16)) {
                    region[0]=180*16; region[2]=220*16; region[1]=45*16; region[3]=75*16; }
                if (region[0]==80*16 && region[1]==30*16) gpJeu->getAudio()->stopSpecial();
                break;
            case 23 :
                if ((region[0]==60*16 || region[0]==140*16 || region[0]==200*16) 
                && (region[1]==45*16 || region[1]==60*16)) {
                    region[1]=45*16; region[3]=75*16; }
                if ((region[0]==140*16 || region[0]==180*16) 
                && (region[1]==15*16 || region[1]==30*16)) {
                    region[1]=15*16; region[3]=45*16; }
                if ((region[0]==220*16 || region[0]==240*16) 
                && (region[1]==15*16 || region[1]==30*16)) {
                    region[0]=220*16; region[2]=260*16; region[1]=15*16; region[3]=45*16; }
                if (region[0]==200*16 && region[1]==45*16) gpJeu->getAudio()->stopSpecial();
                break;
            case 24 :
                if ((region[0]==0 || region[0]==20*16) && (region[1]==0 || region[1]==15*16)) {
                    region[0]=0; region[1]=0; region[2]=40*16; region[3]=30*16; }
                if ((region[0]==0 || region[0]==20*16) && (region[1]==45*16 || region[1]==60*16)) {
                    region[0]=0; region[1]=45*16; region[2]=40*16; region[3]=75*16; }
                if ((region[0]==60*16 || region[0]==80*16) && (region[1]==0 || region[1]==15*16)) {
                    region[0]=60*16; region[1]=0; region[2]=100*16; region[3]=30*16; }
                if ((region[0]==60*16 || region[0]==80*16) 
                && (region[1]==30*16 || region[1]==45*16)) {
                    region[0]=60*16; region[2]=100*16; }
                if ((region[0]==100*16 || region[0]==120*16 || region[0]==280*16) 
                && (region[1]==0 || region[1]==15*16)) {
                    region[1]=0; region[3]=30*16; }
                if ((region[0]==200*16 || region[0]==220*16) && (region[1]==0 || region[1]==15*16)) {
                    region[0]=200*16; region[1]=0; region[2]=240*16; region[3]=30*16; }
                if ((region[0]==200*16 || region[0]==220*16) 
                && (region[1]==45*16 || region[1]==60*16)) {
                    region[0]=200*16; region[1]=45*16; region[2]=240*16; region[3]=75*16; }
                if ((region[0]==260*16 || region[0]==280*16) 
                && (region[1]==45*16 || region[1]==60*16)) {
                    region[0]=260*16; region[1]=45*16; region[2]=300*16; region[3]=75*16; }
                if ((region[0]==200*16 || region[0]==220*16) && region[1]==30*16) {
                    region[0]=200*16; region[2]=240*16; }
                if (region[0]==40*16 && region[1]==30*16) gpJeu->getAudio()->stopSpecial();
                break;
            case 25 :
                if (region[0]==0 && (region[1]==15*16 || region[1]==30*16)) {
                    region[1]=15*16; region[3]=45*16; }
                if ((region[0]==20*16 || region[0]==40*16) && region[1]==0) {
                    region[0]=20*16; region[2]=60*16; }
                if ((region[0]==40*16 || region[0]==60*16) && region[1]==45*16) {
                    region[0]=40*16; region[2]=80*16; }
                if ((region[0]==80*16 || region[0]==100*16) 
                && (region[1]==15*16 || region[1]==30*16)) {
                    region[0]=80*16; region[1]=15*16; region[2]=120*16; region[3]=45*16; }
                if ((region[0]==0 || region[0]==140*16) 
                && (region[1]==75*16 || region[1]==90*16)) {
                    region[1]=75*16; region[3]=105*16; }
                if ((region[0]==20*16 || region[0]==40*16) 
                && (region[1]==90*16 || region[1]==105*16)) {
                    region[0]=20*16; region[1]=90*16; region[2]=60*16; region[3]=120*16; }
                if ((region[0]==80*16 || region[0]==160*16 || region[0]==220*16) 
                && (region[1]==60*16 || region[1]==75*16)) {
                    region[1]=60*16; region[3]=90*16; }
                if ((region[0]==100*16 || region[0]==120*16) 
                && (region[1]==75*16 || region[1]==90*16)) {
                    region[0]=100*16; region[1]=75*16; region[2]=140*16; region[3]=105*16; }
                if ((region[0]==180*16 || region[0]==200*16) 
                && (region[1]==75*16 || region[1]==90*16)) {
                    region[0]=180*16; region[1]=75*16; region[2]=220*16; region[3]=105*16; }
                if (region[0]==20*16 && region[1]==90*16) gpJeu->getAudio()->stopSpecial();
                break;
            case 26 :
                if ((region[0]==20*16 || region[0]==40*16) 
                && (region[1]==15*16 || region[1]==90*16)) {
                    region[0]=20*16; region[2]=60*16; }
                if ((region[0]==80*16 || region[0]==100*16) 
                && (region[1]==15*16 || region[1]==30*16 || region[1]==90*16)) {
                    region[0]=80*16; region[2]=120*16; }
                if (region[0]==120*16 && (region[1]==15*16 || region[1]==30*16)) {
                    region[1]=15*16; region[3]=45*16; }
                if ((region[0]==20*16 || region[0]==40*16) 
                && (region[1]==60*16 || region[1]==75*16)) {
                    region[0]=20*16; region[1]=60*16; region[2]=60*16; region[3]=90*16; }
                if ((region[0]==100*16 || region[0]==120*16) 
                && (region[1]==60*16 || region[1]==75*16)) {
                    region[0]=100*16; region[1]=60*16; region[2]=140*16; region[3]=90*16; }
                if (region[0]==300*16 && region[1]==45*16) gpJeu->getAudio()->stopSpecial();
                break;
            case 27 :
                if ((region[0]==20*16 || region[0]==40*16) 
                && (region[1]==0 || region[1]==15*16)) {
                    region[0]=20*16; region[1]=0; region[2]=60*16; region[3]=30*16; }
                if ((region[0]==60*16 || region[0]==80*16) 
                && (region[1]==15*16 || region[1]==60*16)) {
                    region[0]=60*16; region[2]=100*16; }
                if ((region[0]==80*16 || region[0]==100*16) 
                && (region[1]==30*16 || region[1]==90*16)) {
                    region[0]=80*16; region[2]=120*16; }
                if ((region[0]==120*16 || region[0]==140*16) && region[1]==60*16) {
                    region[0]=120*16; region[2]=160*16; }
                if (region[0]==20*16 && (region[1]==90*16 || region[1]==105*16)) {
                    region[1]=90*16; region[3]=120*16; }
                if (region[0]==100*16 && (region[1]==60*16 || region[1]==75*16)) {
                    region[1]=60*16; region[3]=90*16; }
                if (region[0]==20*16 && region[1]==60*16) gpJeu->getAudio()->stopSpecial();
                break;
            case 28 :
                if ((region[0]==20*16 || region[0]==40*16 || region[0]==60*16) 
                && (region[1]==15*16 || region[1]==30*16 || region[1]==45*16)) {
                    region[0]=20*16; region[1]=15*16; region[2]=80*16; region[3]=60*16; }
                if ((region[0]==120*16 || region[0]==160*16) 
                && (region[1]==15*16 || region[1]==30*16)) {
                    region[1]=15*16; region[3]=45*16; }
                if ((region[0]==0 || region[0]==80*16) 
                && (region[1]==90*16 || region[1]==105*16)) {
                    region[1]=90*16; region[3]=120*16; }
                if (region[0]==60*16 && (region[1]==75*16 || region[1]==90*16)) {
                    region[1]=75*16; region[3]=105*16; }
                if ((region[0]==0 || region[0]==20*16) && region[1]==120*16) {
                    region[0]=0; region[2]=40*16; }
                if ((region[0]==60*16 || region[0]==80*16) && region[1]==120*16) {
                    region[0]=60*16; region[2]=100*16; }
                if ((region[0]==20*16 || region[0]==40*16) 
                && (region[1]==75*16 || region[1]==90*16)) {
                    region[0]=20*16; region[1]=75*16; region[2]=60*16; region[3]=105*16; }
                break;
            case 41 :
                if ((region[0]==60*16 || region[0]==80*16) 
                && (region[1]==0 || region[1]==15*16)) {
                    region[0]=60*16; region[1]=0; region[2]=100*16; region[3]=30*16; }
                break;
            case 48 : region[0]=0; region[1]=0; region[2]=40*16; region[3]=15*16; break;
            case 53 : region[0]=0; region[1]=0; region[2]=20*16; region[3]=30*16; break;
        }
        if (newZone<29) {
            for (int i = region[0]; i < region[2]; i+=320)
                for (int j = region[1]; j < region[3]; j+=240)
                    gpJoueur->setExplore(newZone-15,i/320, j/240);
            gpJoueur->setExplore(newZone-15,(region[2]-1)/320, (region[3]-1)/240);
        }
        
        if (charge) return;
        gpJeu->replace();
        gpJeu->testAnim();
    }
    
    if (newZone == 1 && gpJoueur->getAvancement() == 0) gpJeu->ecrit(172);
    if (newZone == 29 && gpJoueur->getTutoriel() == 0 && region[1]==240) gpJeu->ecrit(197);
}

//replace crânes
void Monde::replace() {
    switch (gpJeu->getZone()) {
        case 15 :
            setValeur(10*16,21*16,31,1); setValeur(42*16,2*16,31,1); setValeur(42*16,12*16,31,1);
            setValeur(64*16,35*16,31,1); setValeur(64*16,38*16,31,1); setValeur(126*16,19*16,31,1);
            setValeur(127*16,19*16,31,1); setValeur(132*16,19*16,31,1); setValeur(133*16,19*16,31,1);
            setValeur(144*16,14*16,31,1); setValeur(149*16,6*16,31,1); setValeur(155*16,14*16,31,1);
            setValeur(154*16,32*16,31,1); setValeur(155*16,32*16,31,1); setValeur(156*16,32*16,31,1);
            setValeur(157*16,32*16,31,1); setValeur(82*16,17*16,40,0); setValeur(82*16,27*16,40,0);
            setValeur(97*16,17*16,40,0); setValeur(97*16,27*16,1004,0);
            setValeur(2*16,2*16,31,1);setValeur(17*16,2*16,31,1);setValeur(42*16,2*16,31,1);
            setValeur(42*16,12*16,31,1); setValeur(22*16,17*16,31,1); setValeur(23*16,17*16,31,1);
            setValeur(24*16,17*16,31,1); setValeur(26*16,42*16,31,1); setValeur(29*16,39*16,31,1);
            setValeur(36*16,20*16,31,1); setValeur(43*16,20*16,31,1); setValeur(45*16,33*16,31,1);
            setValeur(46*16,33*16,31,1); setValeur(47*16,33*16,31,1); setValeur(54*16,29*16,31,1);
            setValeur(55*16,29*16,31,1); setValeur(56*16,29*16,31,1); setValeur(74*16,20*16,31,1);
            setValeur(82*16,32*16,31,1); setValeur(82*16,42*16,31,1); setValeur(97*16,32*16,31,1);
            setValeur(97*16,42*16,31,1); setValeur(102*16,47*16,31,1); setValeur(102*16,57*16,31,1);
            setValeur(117*16,47*16,31,1); setValeur(117*16,57*16,31,1); setValeur(162*16,2*16,31,1);
            setValeur(162*16,12*16,31,1); setValeur(177*16,2*16,31,1); setValeur(177*16,12*16,31,1);
            setValeur(182*16,47*16,31,1); setValeur(197*16,47*16,31,1);
            break;
        case 16 :
            setValeur(42*16,47*16,30,1);setValeur(57*16,47*16,30,1);setValeur(42*16,57*16,30,1);
            setValeur(57*16,57*16,30,1);setValeur(24*16,2*16,30,1);setValeur(25*16,2*16,30,1);
            for (int j=0; j<5; j++) for (int i=0; i<6; i++) setValeur((87+i)*16,(20+j)*16,30,1);
            setValeur(30*16,2*16,30,1);setValeur(31*16,2*16,30,1);setValeur(36*16,2*16,30,1);
            setValeur(37*16,2*16,30,1);setValeur(24*16,12*16,30,1);setValeur(25*16,12*16,30,1);
            setValeur(30*16,12*16,30,1);setValeur(31*16,12*16,30,1);setValeur(36*16,12*16,30,1);
            setValeur(37*16,12*16,30,1);setValeur(148*16,54*16,30,1);setValeur(149*16,54*16,30,1);
            setValeur(150*16,54*16,30,1);setValeur(148*16,55*16,30,1);setValeur(149*16,55*16,30,1);
            setValeur(150*16,55*16,30,1);setValeur(142*16,17*16,30,1);setValeur(157*16,17*16,30,1);
            setValeur(142*16,27*16,30,1);setValeur(157*16,27*16,30,1);setValeur(162*16,62*16,30,1);
            setValeur(177*16,62*16,30,1);setValeur(162*16,72*16,30,1);setValeur(177*16,72*16,30,1);
            setValeur(182*16,32*16,30,1);setValeur(197*16,32*16,30,1);setValeur(182*16,42*16,30,1);
            setValeur(197*16,42*16,30,1);setValeur(242*16,47*16,30,1);setValeur(257*16,47*16,30,1);
            setValeur(242*16,57*16,30,1);setValeur(257*16,57*16,30,1);setValeur(237*16,40*16,30,1);
            setValeur(237*16,41*16,30,1);setValeur(237*16,42*16,30,1);setValeur(238*16,40*16,30,1);
            setValeur(238*16,41*16,30,1);setValeur(238*16,42*16,30,1);setValeur(222*16,92*16,30,1);
            setValeur(277*16,92*16,30,1);setValeur(222*16,102*16,30,1);setValeur(277*16,102*16,30,1);
            setValeur(242*16,77*16,30,1);setValeur(257*16,77*16,30,1);setValeur(242*16,87*16,30,1);
            setValeur(257*16,87*16,30,1);setValeur(202*16,62*16,30,1);setValeur(217*16,62*16,30,1);
            setValeur(202*16,72*16,30,1);setValeur(217*16,72*16,30,1);
            break;
        case 17 :
            setValeur(2*16,32*16,31,1);setValeur(17*16,32*16,31,1);setValeur(2*16,42*16,31,1);
            setValeur(17*16,42*16,31,1);setValeur(2*16,47*16,31,1);setValeur(17*16,47*16,31,1);
            setValeur(52*16,39*16,31,1);setValeur(64*16,24*16,31,1);setValeur(75*16,24*16,31,1);
            setValeur(87*16,39*16,31,1);setValeur(64*16,89*16,31,1);setValeur(71*16,89*16,31,1);
            setValeur(64*16,94*16,31,1);setValeur(71*16,94*16,31,1);setValeur(142*16,72*16,31,1);
            setValeur(147*16,72*16,31,1);setValeur(182*16,107*16,31,1);setValeur(197*16,107*16,31,1);
            setValeur(182*16,117*16,31,1);setValeur(197*16,117*16,31,1);setValeur(204*16,64*16,31,1);
            setValeur(215*16,64*16,31,1);setValeur(125*16,50*16,31,1);setValeur(125*16,51*16,31,1);
            setValeur(134*16,45*16,31,1);setValeur(134*16,46*16,31,1);setValeur(134*16,55*16,31,1);
            setValeur(134*16,56*16,31,1);setValeur(42*16,117*16,31,1);setValeur(57*16,117*16,31,1);
            setValeur(82*16,117*16,31,1);setValeur(97*16,117*16,31,1);
            break;
        case 18 :
            setValeur(20*16,28*16,30,1);setValeur(21*16,28*16,30,1);setValeur(162*16,2*16,30,1);
            setValeur(177*16,2*16,30,1);setValeur(162*16,12*16,30,1);setValeur(177*16,12*16,30,1);
            setValeur(182*16,32*16,30,1);setValeur(197*16,32*16,30,1);setValeur(182*16,42*16,30,1);
            setValeur(197*16,42*16,30,1);setValeur(182*16,47*16,30,1);setValeur(197*16,47*16,30,1);
            setValeur(182*16,57*16,30,1);setValeur(197*16,57*16,30,1);setValeur(242*16,32*16,30,1);
            setValeur(257*16,32*16,30,1);setValeur(242*16,42*16,30,1);setValeur(257*16,42*16,30,1);
            setValeur(162*16,47*16,30,1);setValeur(177*16,47*16,30,1);setValeur(317*16,47*16,30,1);
            setValeur(317*16,57*16,30,1);setValeur(203*16,11*16,30,1);setValeur(204*16,11*16,30,1);
            setValeur(205*16,11*16,30,1);setValeur(203*16,12*16,30,1);setValeur(204*16,12*16,30,1);
            setValeur(205*16,12*16,30,1);
            break;
        case 19 :
            setValeur(146*16,38*16,2311,1);setValeur(146*16,39*16,2311,1);setValeur(152*16,49*16,2311,1);
            setValeur(152*16,50*16,2311,1);setValeur(169*16,42*16,2311,1);setValeur(170*16,42*16,2311,1);
            setValeur(173*16,17*16,2311,1);setValeur(173*16,18*16,2311,1);setValeur(173*16,19*16,2311,1);
            setValeur(185*16,51*16,2311,1);setValeur(185*16,52*16,2311,1);setValeur(185*16,53*16,2311,1);
            setValeur(5*16,2*16,31,1);setValeur(5*16,8*16,31,1);
            for (int i=0; i<5; i++) {setValeur((64+i)*16,2*16,31,1);setValeur((64+i)*16,12*16,31,1);}
            for (int i=0; i<16; i++) setValeur((62+i)*16,47*16,31,1);
            for (int i=0; i<4; i++) setValeur(162*16,(21+i)*16,31,1);
            for (int i=0; i<3; i++) setValeur((35+i)*16,65*16,31,1);
            for (int i=0; i<2; i++) {setValeur((82+i)*16,47*16,31,1);setValeur((96+i)*16,47*16,31,1);}
            break;
        case 20 :
            setValeur(6*16,47*16,30,1);setValeur(7*16,47*16,30,1);setValeur(12*16,47*16,30,1);
            setValeur(13*16,47*16,30,1);setValeur(42*16,77*16,30,1);setValeur(57*16,77*16,30,1);
            setValeur(42*16,87*16,30,1);setValeur(57*16,87*16,30,1);setValeur(122*16,77*16,30,1);
            setValeur(137*16,77*16,30,1);setValeur(122*16,87*16,30,1);setValeur(137*16,87*16,30,1);
            setValeur(127*16,39*16,30,1);setValeur(132*16,39*16,30,1);
            for (int i=0; i<3; i++) {setValeur((25+i)*16,12*16,30,1);setValeur((32+i)*16,12*16,30,1);}
            for (int j=0; j<2; j++) for (int i=0; i<12; i++) setValeur((24+i)*16,(49+j)*16,30,1);
            for (int i=0; i<3; i++) {setValeur((142+i)*16,47*16,30,1);setValeur((155+i)*16,47*16,30,1);}
            setValeur(91*16,77*16,2324,11);setValeur(82*16,69*16,2324,11);setValeur(97*16,69*16,2324,11);
            setValeur(82*16,54*16,2324,11);setValeur(97*16,54*16,2324,11);setValeur(86*16,48*16,2324,11);
            setValeur(91*16,53*16,2324,11);
            break;
        case 21 :
            for (int i=0; i<5; i++) setValeur(255*16,(35+i)*16,31,1);
            setValeur(97*16,32*16,31,1);setValeur(97*16,42*16,31,1);setValeur(144*16,4*16,31,1);
            setValeur(144*16,10*16,31,1);setValeur(165*16,4*16,31,1);setValeur(165*16,10*16,31,1);
            setValeur(194*16,19*16,31,1);setValeur(194*16,25*16,31,1);setValeur(182*16,49*16,31,1);
            setValeur(184*16,47*16,31,1);setValeur(195*16,47*16,31,1);setValeur(197*16,49*16,31,1);
            setValeur(182*16,55*16,31,1);setValeur(184*16,57*16,31,1);setValeur(195*16,57*16,31,1);
            setValeur(197*16,55*16,31,1);setValeur(205*16,34*16,31,1);setValeur(205*16,40*16,31,1);
            break;
        case 22 :
            setValeur(206*16,21*16,2311,1);setValeur(206*16,22*16,2311,1);setValeur(229*16,55*16,2311,1);
            setValeur(230*16,55*16,2311,1);
            setValeur(5*16,34*16,30,1);setValeur(5*16,55*16,30,1);setValeur(10*16,23*16,30,1);
            for (int j=0; j<3; j++) for (int i=0; i<2; i++) setValeur((69+i)*16,(23+j)*16,30,1);
            setValeur(169*16,55*16,30,1);setValeur(170*16,55*16,30,1);setValeur(175*16,34*16,30,1);
            setValeur(176*16,34*16,30,1);setValeur(177*16,34*16,30,1);setValeur(182*16,32*16,30,1);
            setValeur(183*16,32*16,30,1);setValeur(182*16,42*16,30,1);setValeur(183*16,42*16,30,1);
            setValeur(216*16,32*16,30,1);setValeur(217*16,32*16,30,1);setValeur(216*16,42*16,30,1);
            setValeur(217*16,42*16,30,1);setValeur(222*16,34*16,30,1);setValeur(235*16,34*16,30,1);
            setValeur(222*16,40*16,30,1);setValeur(235*16,40*16,30,1);setValeur(84*16,34*16,30,1);
            setValeur(95*16,34*16,30,1);
            for (int i=0; i<5; i++) setValeur(237*16,(50+i)*16,30,1);
            break;
        case 23 :
            for (int i=0; i<6; i++) setValeur((207+i)*16,59*16,2311,1);
            setValeur(49*16,39*16,31,1);setValeur(50*16,39*16,31,1);setValeur(84*16,32*16,31,1);
            setValeur(95*16,32*16,31,1);setValeur(84*16,62*16,31,1);setValeur(84*16,72*16,31,1);
            setValeur(102*16,62*16,31,1);setValeur(117*16,62*16,31,1);setValeur(135*16,62*16,31,1);
            setValeur(135*16,72*16,31,1);setValeur(164*16,17*16,31,1);setValeur(175*16,17*16,31,1);
            setValeur(164*16,64*16,31,1);setValeur(164*16,70*16,31,1);setValeur(184*16,47*16,31,1);
            setValeur(195*16,47*16,31,1);setValeur(204*16,49*16,31,1);setValeur(215*16,49*16,31,1);
            setValeur(255*16,64*16,31,1);setValeur(255*16,70*16,31,1);setValeur(344*16,2*16,31,1);
            setValeur(355*16,2*16,31,1);setValeur(344*16,10*16,31,1);setValeur(355*16,10*16,31,1);
            break;
        case 24 :
            for (int i=0; i<2; i++) setValeur((109+i)*16,34*16,2311,1);
            for (int i=0; i<2; i++) setValeur((189+i)*16,34*16,2311,1);
            for (int i=0; i<4; i++) setValeur((148+i)*16,49*16,2311,1);
            for (int j=0; j<4; j++) for (int i=0; i<2; i++) setValeur((30+i)*16,(60+j)*16,31,1);
            setValeur(105*16,70*16,31,1);setValeur(114*16,70*16,31,1);setValeur(124*16,47*16,31,1);
            setValeur(124*16,57*16,31,1);setValeur(144*16,57*16,31,1);setValeur(155*16,57*16,31,1);
            setValeur(204*16,34*16,31,1);setValeur(204*16,40*16,31,1);setValeur(235*16,34*16,31,1);
            setValeur(235*16,40*16,31,1);
            for (int j=0; j<3; j++) setValeur(220*16,(61+j)*16,31,1);
            for (int j=0; j<2; j++) setValeur(225*16,(51+j)*16,31,1);
            for (int j=0; j<2; j++) for (int i=0; i<2; i++) setValeur((221+i)*16,(56+j)*16,31,1);
            for (int j=0; j<2; j++) setValeur(285*16,(21+j)*16,31,1);
            for (int j=0; j<2; j++) setValeur(294*16,(13+j)*16,31,1);
            break;
        case 25 :
            for (int i=0; i<2; i++) setValeur(145*16,(84+i)*16,2311,1);
            for (int i=0; i<2; i++) setValeur(154*16,(89+i)*16,2311,1);
            for (int i=0; i<2; i++) setValeur(145*16,(94+i)*16,2311,1);
            setValeur(7*16,100*16,31,1);setValeur(12*16,100*16,31,1);setValeur(31*16,99*16,31,1);
            setValeur(48*16,99*16,31,1);setValeur(48*16,110*16,31,1);setValeur(46*16,79*16,31,1);
            setValeur(53*16,79*16,31,1);setValeur(46*16,85*16,31,1);setValeur(53*16,85*16,31,1);
            setValeur(62*16,34*16,31,1);setValeur(64*16,32*16,31,1);setValeur(75*16,32*16,31,1);
            setValeur(77*16,34*16,31,1);setValeur(62*16,40*16,31,1);setValeur(64*16,42*16,31,1);
            setValeur(75*16,42*16,31,1);setValeur(77*16,40*16,31,1);setValeur(130*16,21*16,31,1);
            setValeur(62*16,79*16,31,1);setValeur(64*16,77*16,31,1);setValeur(75*16,77*16,31,1);
            setValeur(77*16,79*16,31,1);setValeur(62*16,85*16,31,1);setValeur(64*16,87*16,31,1);
            setValeur(75*16,87*16,31,1);setValeur(77*16,85*16,31,1);
            for (int j=0; j<4; j++) setValeur((122+j)*16,39*16,31,1);
            for (int j=0; j<3; j++) setValeur(125*16,(40+j)*16,31,1);
            break;
        case 26 :
            setValeur(22*16,34*16,31,1);setValeur(25*16,34*16,31,1);setValeur(34*16,34*16,31,1);
            setValeur(37*16,34*16,31,1);setValeur(56*16,71*16,31,1);setValeur(57*16,71*16,31,1);
            for (int i=0; i<4; i++) setValeur((134+i)*16,80*16,31,1);
            break;
        case 29 :
            setValeur(9*16,21*16,2324,11);
            setValeur(8*16,27*16,30,1);setValeur(11*16,27*16,30,1);
            for (int i=0; i<4; i++) setValeur((8+i)*16,26*16,30,1);
            break;
    }
}

void Monde::permuteBlocs() {
    int temoin=0;
    switch (gpJeu->getZone()) {
        case 7 :
            if (map[7][42]==1348) {
                //bloc bleu bas
                for (int j=0; j<4; j++) setValeur(7*16,(42+j)*16,1346,0);
                temoin=1;
            }
            else {
                //bloc bleu haut
                for (int j=0; j<4; j++) setValeur(7*16,(42+j)*16,1348,2);
                temoin=0;
            }
            break;
        case 19 :
            if (map[42][65]==1348) {
                //bloc bleu bas
                for (int i=0; i<3; i++) setValeur(7*16,(17+i)*16,1346,0);
                for (int i=0; i<3; i++) setValeur(12*16,(17+i)*16,1346,0);
                for (int i=0; i<4; i++) setValeur((8+i)*16,19*16,1346,0);
                for (int i=0; i<3; i++) setValeur(7*16,(55+i)*16,1346,0);
                for (int i=0; i<3; i++) setValeur(12*16,(55+i)*16,1346,0);
                for (int i=0; i<4; i++) setValeur((8+i)*16,55*16,1346,0);
                for (int i=0; i<3; i++) setValeur(47*16,(10+i)*16,1346,0);
                for (int i=0; i<3; i++) setValeur(52*16,(10+i)*16,1346,0);
                for (int i=0; i<4; i++) setValeur((48+i)*16,10*16,1346,0);
                for (int i=0; i<3; i++) setValeur(47*16,(25+i)*16,1346,0);
                for (int i=0; i<3; i++) setValeur(52*16,(25+i)*16,1346,0);
                for (int i=0; i<4; i++) setValeur((48+i)*16,25*16,1346,0);
                for (int i=0; i<3; i++) setValeur(147*16,(17+i)*16,1346,0);
                for (int i=0; i<3; i++) setValeur((42+i)*16,65*16,1346,0);
                for (int i=0; i<3; i++) setValeur((42+i)*16,69*16,1346,0);
                for (int i=0; i<3; i++) setValeur(44*16,(66+i)*16,1346,0);
                for (int i=0; i<3; i++) setValeur((142+i)*16,20*16,1346,0);
                for (int i=0; i<3; i++) setValeur((142+i)*16,24*16,1346,0);
                for (int i=0; i<3; i++) setValeur(144*16,(21+i)*16,1346,0);
                for (int i=0; i<3; i++) setValeur(21*16,(29+i)*16,1346,0);
                for (int i=0; i<3; i++) setValeur(28*16,(17+i)*16,1346,0);
                for (int i=0; i<3; i++) setValeur((35+i)*16,32*16,1346,0);
                for (int i=0; i<2; i++) setValeur(77*16,(66+i)*16,1346,0);
                for (int i=0; i<2; i++) setValeur((109+i)*16,34*16,1346,0);
                for (int i=0; i<2; i++) setValeur((149+i)*16,64*16,1346,0);
                for (int i=0; i<3; i++) setValeur(144*16,(66+i)*16,1346,0);
                for (int i=0; i<3; i++) setValeur(174*16,(66+i)*16,1346,0);
                for (int i=0; i<7; i++) setValeur(172*16,(34+i)*16,1346,0);
                for (int i=0; i<2; i++) setValeur(173*16,(20+i)*16,1346,0);
                for (int i=0; i<2; i++) setValeur(167*16,(23+i)*16,1346,0);
                for (int i=0; i<4; i++) setValeur((167+i)*16,22*16,1346,0);
                //bloc orange haut
                for (int i=0; i<3; i++) setValeur(19*16,(19+i)*16,1347,2);
                for (int i=0; i<3; i++) setValeur((31+i)*16,22*16,1347,2);
                for (int i=0; i<3; i++) setValeur((35+i)*16,35*16,1347,2);
                for (int i=0; i<3; i++) setValeur((42+i)*16,5*16,1347,2);
                for (int i=0; i<3; i++) setValeur((42+i)*16,9*16,1347,2);
                for (int i=0; i<3; i++) setValeur(44*16,(6+i)*16,1347,2);
                for (int i=0; i<2; i++) setValeur(74*16,(66+i)*16,1347,2);
                for (int i=0; i<3; i++) setValeur(47*16,(47+i)*16,1347,2);
                for (int i=0; i<3; i++) setValeur(52*16,(47+i)*16,1347,2);
                for (int i=0; i<4; i++) setValeur((48+i)*16,49*16,1347,2);
                for (int i=0; i<3; i++) setValeur(124*16,(6+i)*16,1347,2);
                for (int i=0; i<2; i++) setValeur((149+i)*16,10*16,1347,2);
                for (int i=0; i<3; i++) setValeur(147*16,(25+i)*16,1347,2);
                for (int i=0; i<2; i++) setValeur(157*16,(36+i)*16,1347,2);
                for (int i=0; i<3; i++) setValeur(164*16,(6+i)*16,1347,2);
                for (int i=0; i<6; i++) setValeur((167+i)*16,19*16,1347,2);
                for (int i=0; i<2; i++) setValeur((169+i)*16,47*16,1347,2);
                for (int i=0; i<4; i++) setValeur((168+i)*16,54*16,1347,2);
                for (int i=0; i<3; i++) setValeur((168+i)*16,70*16,1347,2);
                for (int i=0; i<5; i++) setValeur((186+i)*16,65*16,1347,2);
                for (int i=0; i<5; i++) setValeur((186+i)*16,69*16,1347,2);
                for (int i=0; i<3; i++) setValeur(186*16,(66+i)*16,1347,2);
                for (int i=0; i<3; i++) setValeur(190*16,(66+i)*16,1347,2);
                for (int i=0; i<3; i++) setValeur(194*16,(51+i)*16,1347,2);
                setValeur(168*16,55*16,1347,2);setValeur(171*16,55*16,1347,2);
                setValeur(187*16,48*16,1347,2);setValeur(192*16,48*16,1347,2);
                setValeur(187*16,56*16,1347,2);setValeur(192*16,56*16,1347,2);
                temoin=1;
            }else {
                //bloc bleu haut
                for (int i=0; i<3; i++) setValeur(7*16,(17+i)*16,1348,2);
                for (int i=0; i<3; i++) setValeur(12*16,(17+i)*16,1348,2);
                for (int i=0; i<4; i++) setValeur((8+i)*16,19*16,1348,2);
                for (int i=0; i<3; i++) setValeur(7*16,(55+i)*16,1348,2);
                for (int i=0; i<3; i++) setValeur(12*16,(55+i)*16,1348,2);
                for (int i=0; i<4; i++) setValeur((8+i)*16,55*16,1348,2);
                for (int i=0; i<3; i++) setValeur(47*16,(10+i)*16,1348,2);
                for (int i=0; i<3; i++) setValeur(52*16,(10+i)*16,1348,2);
                for (int i=0; i<4; i++) setValeur((48+i)*16,10*16,1348,2);
                for (int i=0; i<3; i++) setValeur(47*16,(25+i)*16,1348,2);
                for (int i=0; i<3; i++) setValeur(52*16,(25+i)*16,1348,2);
                for (int i=0; i<4; i++) setValeur((48+i)*16,25*16,1348,2);
                for (int i=0; i<3; i++) setValeur(147*16,(17+i)*16,1348,2);
                for (int i=0; i<3; i++) setValeur((42+i)*16,65*16,1348,2);
                for (int i=0; i<3; i++) setValeur((42+i)*16,69*16,1348,2);
                for (int i=0; i<3; i++) setValeur(44*16,(66+i)*16,1348,2);
                for (int i=0; i<3; i++) setValeur((142+i)*16,20*16,1348,2);
                for (int i=0; i<3; i++) setValeur((142+i)*16,24*16,1348,2);
                for (int i=0; i<3; i++) setValeur(144*16,(21+i)*16,1348,2);
                for (int i=0; i<3; i++) setValeur(21*16,(29+i)*16,1348,2);
                for (int i=0; i<3; i++) setValeur(28*16,(17+i)*16,1348,2);
                for (int i=0; i<3; i++) setValeur((35+i)*16,32*16,1348,2);
                for (int i=0; i<2; i++) setValeur(77*16,(66+i)*16,1348,2);
                for (int i=0; i<2; i++) setValeur((109+i)*16,34*16,1348,2);
                for (int i=0; i<2; i++) setValeur((149+i)*16,64*16,1348,2);
                for (int i=0; i<3; i++) setValeur(144*16,(66+i)*16,1348,2);
                for (int i=0; i<3; i++) setValeur(174*16,(66+i)*16,1348,2);
                for (int i=0; i<7; i++) setValeur(172*16,(34+i)*16,1348,2);
                for (int i=0; i<2; i++) setValeur(173*16,(20+i)*16,1348,2);
                for (int i=0; i<2; i++) setValeur(167*16,(23+i)*16,1348,2);
                for (int i=0; i<4; i++) setValeur((167+i)*16,22*16,1348,2);
                //bloc orange bas
                for (int i=0; i<3; i++) setValeur(19*16,(19+i)*16,1345,0);
                for (int i=0; i<3; i++) setValeur((31+i)*16,22*16,1345,0);
                for (int i=0; i<3; i++) setValeur((35+i)*16,35*16,1345,0);
                for (int i=0; i<3; i++) setValeur((42+i)*16,5*16,1345,0);
                for (int i=0; i<3; i++) setValeur((42+i)*16,9*16,1345,0);
                for (int i=0; i<3; i++) setValeur(44*16,(6+i)*16,1345,0);
                for (int i=0; i<2; i++) setValeur(74*16,(66+i)*16,1345,0);
                for (int i=0; i<3; i++) setValeur(47*16,(47+i)*16,1345,0);
                for (int i=0; i<3; i++) setValeur(52*16,(47+i)*16,1345,0);
                for (int i=0; i<4; i++) setValeur((48+i)*16,49*16,1345,0);
                for (int i=0; i<3; i++) setValeur(124*16,(6+i)*16,1345,0);
                for (int i=0; i<2; i++) setValeur((149+i)*16,10*16,1345,0);
                for (int i=0; i<3; i++) setValeur(147*16,(25+i)*16,1345,0);
                for (int i=0; i<2; i++) setValeur(157*16,(36+i)*16,1345,0);
                for (int i=0; i<3; i++) setValeur(164*16,(6+i)*16,1345,0);
                for (int i=0; i<6; i++) setValeur((167+i)*16,19*16,1345,0);
                for (int i=0; i<2; i++) setValeur((169+i)*16,47*16,1345,0);
                for (int i=0; i<4; i++) setValeur((168+i)*16,54*16,1345,0);
                for (int i=0; i<3; i++) setValeur((168+i)*16,70*16,1345,0);
                for (int i=0; i<5; i++) setValeur((186+i)*16,65*16,1345,0);
                for (int i=0; i<5; i++) setValeur((186+i)*16,69*16,1345,0);
                for (int i=0; i<3; i++) setValeur(186*16,(66+i)*16,1345,0);
                for (int i=0; i<3; i++) setValeur(190*16,(66+i)*16,1345,0);
                for (int i=0; i<3; i++) setValeur(194*16,(51+i)*16,1345,0);
                setValeur(168*16,55*16,1345,0);setValeur(171*16,55*16,1345,0);
                setValeur(187*16,48*16,1345,0);setValeur(192*16,48*16,1345,0);
                setValeur(187*16,56*16,1345,0);setValeur(192*16,56*16,1345,0);
                temoin=0;
            }
            break;
        case 21 :
            if (map[47][49]==1348) {
                //bloc bleu bas
                for (int i=0; i<3; i++) {
                    setValeur(87*16,(25+i)*16,1346,0);setValeur(92*16,(25+i)*16,1346,0);
                    setValeur(87*16,(32+i)*16,1346,0);setValeur(92*16,(32+i)*16,1346,0);
                    setValeur(187*16,(10+i)*16,1346,0);setValeur(192*16,(10+i)*16,1346,0);
                    setValeur(46*16,(36+i)*16,1346,0);setValeur(71*16,(36+i)*16,1346,0);
                    setValeur(167*16,(27+i)*16,1346,0);
                }
                for (int i=0; i<4; i++) {
                    setValeur(47*16,(50+i)*16,1346,0);setValeur(52*16,(50+i)*16,1346,0);
                    setValeur((88+i)*16,25*16,1346,0);setValeur((88+i)*16,34*16,1346,0);
                    setValeur((188+i)*16,10*16,1346,0);
                }
                for (int i=0; i<2; i++) {
                    setValeur(66*16,(11+i)*16,1346,0);setValeur(280*16,(46+i)*16,1346,0);
                    setValeur((49+i)*16,19*16,1346,0);setValeur((149+i)*16,41*16,1346,0);
                }
                for (int i=0; i<5; i++) setValeur((113+i)*16,10*16,1346,0);
                for (int i=0; i<6; i++) {
                    setValeur((102+i)*16,34*16,1346,0);setValeur((47+i)*16,49*16,1346,0);
                    setValeur((47+i)*16,54*16,1346,0);
                }
                //bloc orange haut
                for (int i=0; i<3; i++) {
                    setValeur(7*16,(25+i)*16,1347,2);setValeur(12*16,(25+i)*16,1347,2);
                    setValeur(7*16,(32+i)*16,1347,2);setValeur(12*16,(32+i)*16,1347,2);
                    setValeur((35+i)*16,5*16,1347,2);setValeur((35+i)*16,9*16,1347,2);
                    setValeur(35*16,(6+i)*16,1347,2);setValeur((82+i)*16,5*16,1347,2);
                    setValeur((82+i)*16,9*16,1347,2);setValeur(84*16,(6+i)*16,1347,2);
                    setValeur((82+i)*16,20*16,1347,2);setValeur((82+i)*16,24*16,1347,2);
                    setValeur(84*16,(21+i)*16,1347,2);setValeur((282+i)*16,5*16,1347,2);
                    setValeur((282+i)*16,9*16,1347,2);setValeur(284*16,(6+i)*16,1347,2);
                    setValeur(266*16,(21+i)*16,1347,2);
                }
                for (int i=0; i<4; i++) {
                    setValeur((8+i)*16,25*16,1347,2);setValeur((8+i)*16,34*16,1347,2);
                    setValeur(147*16,(35+i)*16,1347,2);setValeur(152*16,(35+i)*16,1347,2);
                }
                for (int i=0; i<2; i++) {
                    setValeur((49+i)*16,40*16,1347,2);setValeur((69+i)*16,40*16,1347,2);
                    setValeur((73+i)*16,33*16,1347,2);setValeur(279*16,(35+i)*16,1347,2);
                    setValeur((149+i)*16,31*16,1347,2);
                }
                for (int i=0; i<5; i++) setValeur((113+i)*16,34*16,1347,2);
                for (int i=0; i<6; i++) {
                    setValeur((147+i)*16,34*16,1347,2);setValeur((147+i)*16,39*16,1347,2);
                    setValeur((102+i)*16,10*16,1347,2);
                }
                temoin=1;
            }
            else {
                //bloc bleu haut
                for (int i=0; i<3; i++) {
                    setValeur(87*16,(25+i)*16,1348,2);setValeur(92*16,(25+i)*16,1348,2);
                    setValeur(87*16,(32+i)*16,1348,2);setValeur(92*16,(32+i)*16,1348,2);
                    setValeur(187*16,(10+i)*16,1348,2);setValeur(192*16,(10+i)*16,1348,2);
                    setValeur(46*16,(36+i)*16,1348,2);setValeur(71*16,(36+i)*16,1348,2);
                    setValeur(167*16,(27+i)*16,1348,2);
                }
                for (int i=0; i<4; i++) {
                    setValeur(47*16,(50+i)*16,1348,2);setValeur(52*16,(50+i)*16,1348,2);
                    setValeur((88+i)*16,25*16,1348,2);setValeur((88+i)*16,34*16,1348,2);
                    setValeur((188+i)*16,10*16,1348,2);
                }
                for (int i=0; i<2; i++) {
                    setValeur(66*16,(11+i)*16,1348,2);setValeur(280*16,(46+i)*16,1348,2);
                    setValeur((49+i)*16,19*16,1348,2);setValeur((149+i)*16,41*16,1348,2);
                }
                for (int i=0; i<5; i++) setValeur((113+i)*16,10*16,1348,2);
                for (int i=0; i<6; i++) {
                    setValeur((102+i)*16,34*16,1348,2);setValeur((47+i)*16,49*16,1348,2);
                    setValeur((47+i)*16,54*16,1348,2);
                }
                //bloc orange bas
                for (int i=0; i<3; i++) {
                    setValeur(7*16,(25+i)*16,1345,0);setValeur(12*16,(25+i)*16,1345,0);
                    setValeur(7*16,(32+i)*16,1345,0);setValeur(12*16,(32+i)*16,1345,0);
                    setValeur((35+i)*16,5*16,1345,0);setValeur((35+i)*16,9*16,1345,0);
                    setValeur(35*16,(6+i)*16,1345,0);setValeur((82+i)*16,5*16,1345,0);
                    setValeur((82+i)*16,9*16,1345,0);setValeur(84*16,(6+i)*16,1345,0);
                    setValeur((82+i)*16,20*16,1345,0);setValeur((82+i)*16,24*16,1345,0);
                    setValeur(84*16,(21+i)*16,1345,0);setValeur((282+i)*16,5*16,1345,0);
                    setValeur((282+i)*16,9*16,1345,0);setValeur(284*16,(6+i)*16,1345,0);
                    setValeur(266*16,(21+i)*16,1345,0);
                }
                for (int i=0; i<4; i++) {
                    setValeur((8+i)*16,25*16,1345,0);setValeur((8+i)*16,34*16,1345,0);
                    setValeur(147*16,(35+i)*16,1345,0);setValeur(152*16,(35+i)*16,1345,0);
                }
                for (int i=0; i<2; i++) {
                    setValeur((49+i)*16,40*16,1345,0);setValeur((69+i)*16,40*16,1345,0);
                    setValeur((73+i)*16,33*16,1345,0);setValeur(279*16,(35+i)*16,1345,0);
                    setValeur((149+i)*16,31*16,1345,0);
                }
                for (int i=0; i<5; i++) setValeur((113+i)*16,34*16,1345,0);
                for (int i=0; i<6; i++) {
                    setValeur((147+i)*16,34*16,1345,0);setValeur((147+i)*16,39*16,1345,0);
                    setValeur((102+i)*16,10*16,1345,0);
                }
                temoin=0;
            }
            break;
        case 25 :
            if (map[35][34]==1348) {
                //bloc bleu bas
                for (int i=0; i<2; i++) setValeur((189+i)*16,86*16,1346,0);
                for (int i=0; i<3; i++) {setValeur((35+i)*16,34*16,1346,0);setValeur((35+i)*16,40*16,1346,0);
                    setValeur(27*16,(92+i)*16,1346,0);setValeur(32*16,(92+i)*16,1346,0);}
                for (int i=0; i<4; i++) {setValeur((28+i)*16,94*16,1346,0);setValeur((168+i)*16,94*16,1346,0);}
                for (int i=0; i<5; i++) {setValeur(35*16,(35+i)*16,1346,0);setValeur(144*16,(110+i)*16,1346,0);}
                //bloc orange haut
                for (int i=0; i<2; i++) setValeur(194*16,(81+i)*16,1347,2);
                for (int i=0; i<3; i++) {setValeur(27*16,(47+i)*16,1347,2);setValeur(32*16,(47+i)*16,1347,2);
                    setValeur((22+i)*16,109*16,1347,2);setValeur((22+i)*16,115*16,1347,2);
                    setValeur((55+i)*16,109*16,1347,2);setValeur((55+i)*16,115*16,1347,2);
                    setValeur(52*16,(36+i)*16,1347,2);}
                for (int i=0; i<4; i++) {setValeur((28+i)*16,49*16,1347,2);setValeur((148+i)*16,109*16,1347,2);}
                for (int i=0; i<5; i++) {setValeur(24*16,(110+i)*16,1347,2);setValeur(55*16,(110+i)*16,1347,2);
                    setValeur((48+i)*16,35*16,1347,2);setValeur((48+i)*16,39*16,1347,2);
                    setValeur(47*16,(35+i)*16,1347,2);}
                temoin=1;
            }
            else {
                //bloc bleu haut
                for (int i=0; i<2; i++) setValeur((189+i)*16,86*16,1348,2);
                for (int i=0; i<3; i++) {setValeur((35+i)*16,34*16,1348,2);setValeur((35+i)*16,40*16,1348,2);
                    setValeur(27*16,(92+i)*16,1348,2);setValeur(32*16,(92+i)*16,1348,2);}
                for (int i=0; i<4; i++) {setValeur((28+i)*16,94*16,1348,2);setValeur((168+i)*16,94*16,1348,2);}
                for (int i=0; i<5; i++) {setValeur(35*16,(35+i)*16,1348,2);setValeur(144*16,(110+i)*16,1348,2);}
                //bloc orange bas
                for (int i=0; i<2; i++) setValeur(194*16,(81+i)*16,1345,0);
                for (int i=0; i<3; i++) {setValeur(27*16,(47+i)*16,1345,0);setValeur(32*16,(47+i)*16,1345,0);
                    setValeur((22+i)*16,109*16,1345,0);setValeur((22+i)*16,115*16,1345,0);
                    setValeur((55+i)*16,109*16,1345,0);setValeur((55+i)*16,115*16,1345,0);
                    setValeur(52*16,(36+i)*16,1345,0);}
                for (int i=0; i<4; i++) {setValeur((28+i)*16,49*16,1345,0);setValeur((148+i)*16,109*16,1345,0);}
                for (int i=0; i<5; i++) {setValeur(24*16,(110+i)*16,1345,0);setValeur(55*16,(110+i)*16,1345,0);
                    setValeur((48+i)*16,35*16,1345,0);setValeur((48+i)*16,39*16,1345,0);
                    setValeur(47*16,(35+i)*16,1345,0);}
                temoin=0;
            }
            break;
    }
    gpJeu->permuteBloc(temoin);
}

int Monde::etage() {
    int level = 0;
    switch (gpJeu->getZone()-15) {
        case 0 : if (region[0]<100*16) level=0; else level=-1; break;
        case 1 : 
            if (region[0]<100*16) level=0; 
            else if (region[0]<200*16) level=-1;
            else level=-2;  break;
        case 2 : 
            if (region[0]<20*16) level=1; 
            else if (region[0]<120*16) level=0;
            else level=-1;  break;
        case 6 :
            if (region[0]<100*16) level=0;
            if (region[0]>=100*16 && region[0]<200*16) level=-1;
            if (region[0]>=200*16) level=-2; break;
        case 3 : if (region[0]<60*16) level=2;
            if (region[0]>=60*16 && region[0]<120*16) level=1;
            if (region[0]>=120*16 && region[0]<220*16) level=0;
            if (region[0]>=220*16) level=-1; break;
        case 4 : if (region[0]<100*16) level=0; else level=-1; break;
        case 5 : if (region[0]==0) level=1;
            if (region[0]>=20*16 && region[0]<160*16) level=0;
            if (region[0]>=160*16) level=-1; break;
        case 7 : if (region[0]<80*16) level=1;
            if (region[0]>=80*16 && region[0]<160*16) level=0;
            if (region[0]>=160*16) level=-1; break;
        case 8 : if (region[0]==0 && region[1]==60*16) level=2;
            if (region[0]<60*16 && region[1]<60*16) level=1;
            if (region[0]>=60*16 && region[0]<160*16) level=0;
            if (region[0]>=160*16 && region[0]<260*16) level=-1;
            if (region[0]>=260*16) level=-2; break;
        case 9 : if (region[0]<100*16) level=1;
            if (region[0]>=100*16 && region[0]<200*16) level=0;
            if (region[0]>=200*16) level=-1; break;
        case 10 : level=2-(region[0]/(80*16) +  2*(region[1]/(60*16))); break;
        case 11 :
            if (region[0]<140*16) level=0;
            if (region[0]>=140*16 && region[0]<280*16) level=-1;
            if (region[0]>=280*16) level=-2; break;
        case 12 : level=0; break;
        case 13 : level=2-(region[0]/(100*16) +  2*(region[1]/(75*16))); break;
    }
    return level;
}

int Monde::getLargeur() {return largeur;}
int Monde::getHauteur() {return hauteur;}

int Monde::motifValue(int x, int y) {
    return map[(int)(x/CASE)][(int)(y/CASE)];
}

int Monde::motifAirValue(int x, int y) {
    return air[(int)(x/CASE)][(int)(y/CASE)];
}

int Monde::murValue(int x, int y) {
    return mur[(int)(x/(CASE/2))][(int)(y/(CASE/2))];
}

int Monde::contenuValue(int x, int y) {
    return contenu[(int)(x/CASE)][(int)(y/CASE)];
}

int Monde::regionValue(int i) {
    return region[i];
}

void Monde::setValeur(int x, int y, int vsol, int vair, int vmur, int vcontenu) {
    setValeur(x, y, vsol, vmur);
    if (vair>-1) air[(int)(x/CASE)][(int)(y/CASE)] = vair;
    contenu[(int)(x/CASE)][(int)(y/CASE)] = vcontenu;
}

void Monde::setValeur(int x, int y, int vsol, int vmur) {
    map[(int)(x/CASE)][(int)(y/CASE)] = vsol;
    mur[((int)(x/CASE))*2][((int)(y/CASE))*2] = vmur;
    mur[((int)(x/CASE))*2+1][((int)(y/CASE))*2] = vmur;
    mur[((int)(x/CASE))*2][((int)(y/CASE))*2+1] = vmur;
    mur[((int)(x/CASE))*2+1][((int)(y/CASE))*2+1] = vmur;
}

void Monde::setMap(int x, int y, int val) {map[x][y]=val;}

void Monde::setMurs(int x, int y, int val) {mur[x][y]=val;}

void Monde::setContenu(int x, int y, int val) {contenu[x][y]=val;}

void Monde::drawSol(SDL_Surface* screen, int phg[2]) {
    int x=0,y=0,val;
    SDL_Rect src;    
    SDL_Rect dst;
    
    dst.x = 0;
    dst.y = 0;
    
    plafondmax=0;
    
    int zone = gpJeu->getZone();
    
    if (zone==14 || (zone==25 && region[0]>=160*16) 
    || (zone==28 && region[0]==140*16 && region[1]==105*16)) {
        src.x=0;src.y=0;src.w=320;src.h=240;
        if (zone==28) SDL_BlitSurface(imageSpe[2], &src, screen, &dst);
        else if (gpJeu->getJoueur()->hasObjet(O_MASQUE)<2) 
            SDL_BlitSurface(imageSpe[1], &src, screen, &dst);
        else SDL_BlitSurface(imageSpe[0], &src, screen, &dst);
        
        int hasard = 0;
        bool stop = gpJeu->getStop();
        
        //eclairs
        if (zone==28) {
            if (ecl1==0 || ecl2==0) {
                hasard=rand()%50;
                if (hasard==3 && !ecl1) { 
                    xecl1=rand()%288;
                    if (!ecl2 || xecl1+32<xecl2 || xecl1>xecl2+32) ecl1=1;
                }
                if (hasard==7 && !ecl2) {
                    xecl2=rand()%288;
                    if (!ecl1 || xecl2+32<xecl1 || xecl2>xecl1+32) ecl2=1;
                }
            }
            if (ecl1>0) {
                src.x=((ecl1-1)/4)*32;src.y=0;src.w=32;src.h=64;
                dst.x = xecl1;dst.y = 0;
                SDL_BlitSurface(imageSpe[3], &src, screen, &dst);
                if (!stop) ecl1++; if (ecl1>12) ecl1=0;
            }
            if (ecl2>0) {
                src.x=((ecl2-1)/4)*32;src.y=64;src.w=32;src.h=64;
                dst.x = xecl2;dst.y = 0;
                SDL_BlitSurface(imageSpe[3], &src, screen, &dst);
                if (!stop) ecl2++; if (ecl2>12) ecl2=0;
            }
        }
        
        //avion et ovni
        if (avion==0 && ovni==0) {
            hasard=rand()%30000;
            if (hasard>7 && hasard<12 && (rand()%3)==1) avion=1;
            if (hasard==7 && (rand()%5)==1) ovni=1;
        }
        if (avion>0) {
            src.x=((anim%4)/2)*31;src.y=11;src.w=31;src.h=13;
            dst.x = avion-32;dst.y = 32;
            SDL_BlitSurface(imageSpe[7], &src, screen, &dst);
            if (!stop) avion++; if (avion>384) avion=0;
        }
        if (ovni>0) {
            src.x=((anim%8)/2)*19;src.y=0;src.w=19;src.h=11;
            dst.x = ovni-32;dst.y = 32;
            SDL_BlitSurface(imageSpe[7], &src, screen, &dst);
            if (!stop) ovni++; if (ovni>384) ovni=0;
        }
    }
    
    while(x<=320){
        while(y<=240){
            
            if(air[((phg[0]+x)-((phg[0]+x)%CASE))/CASE][((phg[1]+y)-((phg[1]+y)%CASE))/CASE ]>0) {
                plafond[plafondmax][0]=x;
                plafond[plafondmax][1]=y;
                plafondmax++;
            }    
            
            val=map[((phg[0]+x)-((phg[0]+x)%CASE))/CASE][((phg[1]+y)-((phg[1]+y)%CASE))/CASE ];
            
            if (val != 999) { // image vide
            
                if (val==1) {
                    switch (anim%4) {
                        case 0 : val=128; break;
                        case 2 : val=129; break;                    
                    }    
                }
                if ((val >= 425 && val <= 452) || (val >= 457 && val <= 466)
                    || val == 540 || val == 543) {
                    switch (anim%3) {
                        case 0 : val++; break;
                        case 1 : val+=2; break;
                    }    
                }
                if (val==1001 || val==2311) {
                    switch (anim%4) {
                        case 1 : case 3 : val+=1; break;
                        case 2 : val+=2; break;
                    }
                }
                if ((val >= 2257 && val <= 2269) || (val >= 3361 && val <= 3373)) {
                    switch (anim%4) {
                        case 1 : val+=13; break;
                        case 2 : val+=26; break;
                        case 3 : val+=13; break;
                    }
                }
                if (val >= 2450 && val <= 2462) {
                    switch (anim%8) {
                        case 0 :
                        case 1 : val+=13; break;
                        case 2 :
                        case 3 : val+=26; break;
                        case 4 :
                        case 5 : val+=13; break;
                    }
                }
                if (val == 3417) val+=(anim%3);
                if (val >= 3400 && val <= 3415) val+=(anim%3);
                if (val==885 || val==893 || val==895) {
                    switch (anim%3) {
                        case 1 : val=val-885+4116; break;
                        case 2 : val=val-885+4117; break;                   
                    }    
                }
                if (val==888 || val==890 || val==892) {
                    switch (anim%3) {
                        case 1 : val=val-888+4118; break;
                        case 2 : val=val-888+4119; break;                   
                    }
                }
                if (val==896 || val==898) {
                    switch (anim%3) {
                        case 1 : val=val-896+4128; break;
                        case 2 : val=val-896+4129; break;                   
                    }
                }
                if (val==899) {
                    switch (anim%3) {
                        case 1 : val=4132; break;
                        case 2 : val=4133; break;                   
                    }
                }
                if (val==908) {
                    switch (anim%3) {
                        case 1 : val=4134; break;
                        case 2 : val=4135; break;                   
                    }
                }
                if (val==909) {
                    switch (anim%3) {
                        case 1 : val=4136; break;
                        case 2 : val=4137; break;                   
                    }
                }
                
                
            }

            SDL_Surface* img = image[(int)(val/1000)];
            val = val%1000;

            src.x=(val%40)*16; src.y=((int)(val/40))*16;
            if(x==0)src.x+=phg[0]%CASE;
            src.w=CASE-(src.x%CASE);
            if(y==0)src.y+=phg[1]%CASE;
            src.h=CASE-(src.y%CASE);
            dst.x = x; dst.y = y;
            
            SDL_BlitSurface(img, &src, screen, &dst);
            
            y+=src.h;
        }    
        y=0;
        x+=src.w;
    }
    
    bool stop = gpJeu->getStop();
    bool transition = gpJeu->getTransition();
    
    if(SDL_GetTicks() > lastAnimTime + ANIM_REFRESH && (!stop || transition)) {
        lastAnimTime = SDL_GetTicks();
        anim++;            
        if (anim > 59) anim = 0;
        if (gpJeu->getZone()==25) gpJeu->testTapis();
    }
}


void Monde::drawAir(SDL_Surface* screen, int phg[2]) {
    int x=0,y=0,val=0,cpt=0;
    SDL_Rect src;    
    SDL_Rect dst;
    
    dst.x = 0;
    dst.y = 0;    
    while(cpt<plafondmax) {
        x=plafond[cpt][0];
        y=plafond[cpt][1];
        val=air[((phg[0]+x)-((phg[0]+x)%CASE))/CASE][((phg[1]+y)-((phg[1]+y)%CASE))/CASE];
        
        if (val > 0) {
        
            if (val==760) {
                switch (anim%3) {
                    case 0 : val+=3; break;
                    case 1 : val+=5; break;
                }
            }
            if (val==762) {
                switch (anim%3) {
                    case 0 : val+=2; break;
                    case 1 : val+=4; break;
                }
            }
            if (val>=2000 && val<=2005 && anim%2==1) val+=10;
            if ((val==2007 || val==2008) && anim%2==1) val+=9;
        
            SDL_Surface* img = image[(int)(val/1000)];
            val = val%1000;

            src.x=(val%40)*16; src.y=((int)(val/40))*16;
            if(x==0)src.x+=phg[0]%CASE;
            src.w=CASE-(src.x%CASE);
            if(y==0)src.y+=phg[1]%CASE;
            src.h=CASE-(src.y%CASE);
            dst.x = x; dst.y = y;
            
            SDL_BlitSurface(img, &src, screen, &dst);
            
        }

        cpt++;
    }
    
    if (gpJeu->getZone()>=7 && gpJeu->getZone()<=8 && !gpJeu->getJoueur()->getOnilink() 
    && gpJeu->getJoueur()->getAvancement()==64) {
        dst.x=0; dst.y=0; src.x=0; src.y=0; src.w=320; src.h=240;
        SDL_BlitSurface(imageSpe[4+(anim%3)], &src, screen, &dst); 
    }
    
    if (niveau > 0) {
        if(gpJeu->isDehors()) niveau=0;
        else {
            if (!gpJeu->getTransition() && !gpJeu->getStop()) niveau--;
            src.h = 16; src.w = 32;src.x = 0;src.y=32-(etage()*16);
            dst.x = 288; dst.y = 0;
            SDL_BlitSurface(imagelevel, &src, screen, &dst);
        }
    }
    
    if (gpJeu->getTransition() && animtransC>10) {
        dst.x = 0; dst.y = 0;
        SDL_BlitSurface(imagetransit, NULL, screen, &dst);
    }
}



void Monde::chargeMap(int zone) {
    int cpt1=0,cpt2=0,a,b;
    
    utile = 0;
    
    for (int i = 0; i < 4; i++) region[i]=0;
    for (int i = 0; i < 400; i++) 
        for (int j = 0; j < 400; j++) 
            setValeur(i*CASE, j*CASE, 0, 0, 0, 0);
            
    std::ostringstream oss;
    oss << zone;
    std::string result = "app0:/map/carte" + oss.str() + ".map";
    std::ifstream file(result.c_str());
    
    file >> largeur;
    file >> hauteur;

    for (int i=0; i<largeur/16; i++){    
    for (int j=0; j<hauteur/16; j++){
        file >> map[i][j];
        file >> mur[i*2][j*2];
        file >> mur[i*2+1][j*2];
        file >> mur[i*2][j*2+1];
        file >> mur[i*2+1][j*2+1];
    }
    }
    
    file >> cpt1;
    for (int i=0; i<cpt1; i++) {
        file >> a;
        file >> b;
        file >> air[a][b];
    }
    
    file >> cpt2;
    for (int i=0; i<cpt2; i++) {
        file >> a;
        file >> b;
        file >> contenu[a][b];
    }
    corrigeMap(zone);
}

void Monde::transitFull(int nzone, int nx, int ny) {
    gpJeu->setStop(true);
    gpJeu->setTransition(true);
    if (gpJeu->getZone() != nzone) gpJeu->videListes();
    else niveau = 100;
    gpJeu->setNewZone(nzone);
    gpJeu->setNewX(nx);
    gpJeu->setNewY(ny);
    
    animtransC=10;
}

void Monde::sortieVillage() {
    int x=0, y=0, new_zone;
    int zone = gpJeu->getZone();
    new_zone=zone;
    if (zone>=46 && zone<=52) new_zone=3;
    if (zone>=53 && zone<=60) new_zone=7;
    if (zone>=67 && zone<=70) new_zone=13;
    switch (new_zone) {
        case 3 : x=39*16; y=55*16; break;
        case 7 : x=39*16; y=42*16; break;
        case 13 : x=78*16; y=22*16; break;
    }
    gpJeu->getJoueur()->setXDep(x);
    gpJeu->getJoueur()->setYDep(y);
    gpJeu->getJoueur()->setDirDep(gpJeu->getJoueur()->getDirection());
    gpJeu->getAudio()->playSound(5);
    SDL_Delay(1500);
    transitFull(new_zone, x, y);
}

void Monde::transitSalle(Direction dir) {
    Joueur* gpJoueur = gpJeu->getJoueur();
    if (gpJoueur->getTypeAnim()==EMMENE) gpJoueur->setTypeAnim(MARCHE);
    
    //labyrinthe de l'ombre
    if (gpJeu->getZone()==26 && region[0]>=140*16 && region[0]<280*16) {
        int loupe=0;
        switch(dir) {
            case N :
                if (region[1]==0 || (region[1]==15*16 && region[0]==260*16) 
                || (region[1]==30*16 && (region[0]==220*16 || region[0]==240*16)) 
                || (region[1]==45*16 && region[0]<200*16) || (region[1]==60*16 && region[0]>200*16) 
                || (region[1]==75*16 && (region[0]==160*16 || region[0]==180*16 || region[0]==240*16)) 
                || (region[1]==90*16 && (region[0]==140*16 || region[0]==160*16 || region[0]==220*16)) 
                ) loupe=1; break;
            case S :
                if (region[1]==90*16 || (region[1]==0 && region[0]==260*16) 
                || (region[1]==15*16 && (region[0]==220*16 || region[0]==240*16)) 
                || (region[1]==30*16 && region[0]<200*16) || (region[1]==45*16 && region[0]>200*16) 
                || (region[1]==60*16 && (region[0]==160*16 || region[0]==180*16 || region[0]==240*16)) 
                || (region[1]==75*16 && (region[0]==140*16 || region[0]==160*16 || region[0]==220*16)) 
                ) loupe=1; break;
            case O :
                if (region[0]==140*16 
                || (region[0]==160*16 && (region[1]==0 || region[1]==30*16 || region[1]==60*16))
                || (region[0]==180*16 && (region[1]==15*16 || region[1]==45*16))
                || (region[0]==200*16 && region[1]!=0 && region[1]!=45*16)
                || (region[0]==220*16 && region[1]!=45*16 && region[1]!=90*16)
                || (region[0]==240*16 && (region[1]==15*16 || region[1]==45*16))
                || (region[0]==260*16 && (region[1]==30*16 || region[1]>60*16))
                ) loupe=1; break;
            case E :
                if (region[0]==260*16 
                || (region[0]==140*16 && (region[1]==0 || region[1]==30*16 || region[1]==60*16))
                || (region[0]==160*16 && (region[1]==15*16 || region[1]==45*16))
                || (region[0]==180*16 && region[1]!=0 && region[1]!=45*16)
                || (region[0]==200*16 && region[1]!=45*16 && region[1]!=90*16)
                || (region[0]==220*16 && (region[1]==15*16 || region[1]==45*16))
                || (region[0]==240*16 && (region[1]==30*16 || region[1]>60*16))
                ) loupe=1; break;
        }
        if (loupe) {
            gpJeu->getAudio()->playSound(5);
            transitFull(26, gpJoueur->getXDep(), gpJoueur->getYDep());
            return;
        }
    }
    
    gpJeu->setStop(true);
    gpJeu->setTransition(true);
    
    switch(dir){
        case N : animtransY=-248; gpJoueur->setY(gpJoueur->getY()-4); break;
        case S : animtransY=248; gpJoueur->setY(gpJoueur->getY()+20); break;
        case O : animtransX=-328; gpJoueur->setX(gpJoueur->getX()-4); break;
        case E : animtransX=328; gpJoueur->setX(gpJoueur->getX()+20); break;
    }
    
    //changement de sous zone par défaut
    switch (dir) {
        case N : region[0]=((int)(gpJoueur->getX()/320))*320; region[1]-=15*16; break;
        case S : region[0]=((int)(gpJoueur->getX()/320))*320; region[1]=region[3]; break;
        case O : region[0]-=20*16; region[1]=((int)(gpJoueur->getY()/240))*240; break;
        case E : region[0]=region[2]; region[1]=((int)(gpJoueur->getY()/240))*240; break;
    }
    region[3]=region[1]+15*16;
    region[2]=region[0]+20*16;
}

void Monde::transitZone(int nzone, int decx, int decy, Direction dir) {
    Joueur* gpJoueur = gpJeu->getJoueur();
    if (gpJoueur->getTypeAnim()==EMMENE) gpJoueur->setTypeAnim(MARCHE);
    int phg[2];
    phg[0] = gpJeu->getPhg(0);
    phg[1] = gpJeu->getPhg(1);
    
    gpJeu->setStop(true);
    gpJeu->setTransition(true);
    gpJeu->videListes();
    gpJeu->setNewZone(nzone);
    
    animtransX=0;
    animtransY=0;
    
    int nx = gpJoueur->getX()-320*decx;
    int ny = gpJoueur->getY()-240*decy;
    
    int savx=gpJoueur->getX()-phg[0];
    int savy=gpJoueur->getY()-phg[1];
    
    for (int i=0; i<42; i++)
        for (int j=0; j<32; j++){
            transMap[i][j]=0;
            transAir[i][j]=0;
        }
    
    //copie de la partie de la carte dans transMap et transAir
    int cor1=0,cor2=0;
    
    int ind1=21, ind2=16;
    if(phg[0]%16==0)ind1=20;
    if(phg[1]%16==0)ind2=15;
    switch(dir){
        case N:cor2=ind2;break;
        case O:cor1=ind1;break;
        default : break;
    }
    
    for (int i=0; i<ind1; i++){
        for (int j=0; j<ind2; j++){
            transMap[i+cor1][j+cor2]=map[((int)((phg[0])/16))+i][((int)((phg[1])/16))+j];
            transAir[i+cor1][j+cor2]=air[((int)((phg[0])/16))+i][((int)((phg[1])/16))+j];
        }
    }
    
    //changement de zone
    chargeMap(nzone);
    
    phg[0]-=decx*320;
    phg[1]-=decy*240;
    if (dir==N) phg[1]-=240;
    if (dir==S) phg[1]+=240;
    if (dir==O) phg[0]-=320;
    if (dir==E) phg[0]+=320;
    
    //copie de la partie de la nouvelle carte dans transMap et transAir
    cor1=0,cor2=0;
    switch(dir){
        case S:cor2=ind2;break;
        case E:cor1=ind1;break;
        default : break;
    }
    for (int i=0; i<ind1; i++){
        for (int j=0; j<ind2; j++){
            transMap[i+cor1][j+cor2]=map[((int)((phg[0])/16))+i][((int)((phg[1])/16))+j];
            transAir[i+cor1][j+cor2]=air[((int)((phg[0])/16))+i][((int)((phg[1])/16))+j];
        }
    }
    
    //copie de transMap vers map
    for (int i=0; i<42; i++){
        for (int j=0; j<32; j++){
            map[i][j]=transMap[i][j];
            air[i][j]=transAir[i][j];
        }
    }
    
    phg[0]=((phg[0])%16);
    phg[1]=((phg[1])%16);
    
    gpJoueur->setX(savx+phg[0]);
    gpJoueur->setY(savy+phg[1]);
    
    switch(dir){
        case N :
            ny-=(28+8);
            phg[1]+=16*ind2;
            gpJoueur->setX(gpJoueur->getX()%320);
            gpJoueur->setY(gpJoueur->getY()%240+240);
            animtransY=-248;
            break;
        case S :
            ny+=(28+4);
            gpJoueur->setX(gpJoueur->getX()%320);
            gpJoueur->setY(gpJoueur->getY()%240+16);
            animtransY=248;
            break;
        case O :
            nx-=28;
            phg[0]+=16*ind1;
            gpJoueur->setX(gpJoueur->getX()%320+320-4);
            gpJoueur->setY(gpJoueur->getY()%240);
            animtransX=-328;
            break;
        case E :
            nx+=28;
            gpJoueur->setX(gpJoueur->getX()%320+16+4);
            gpJoueur->setY(gpJoueur->getY()%240);
            animtransX=328;
            break;
    }
    
    gpJeu->setNewX(nx);
    gpJeu->setNewY(ny);
    
    gpJeu->setPhg(phg[0],phg[1]);
}

void Monde::defilTransit() {
    int phg[2];
    phg[0] = gpJeu->getPhg(0);
    phg[1] = gpJeu->getPhg(1);
    Joueur* gpJoueur = gpJeu->getJoueur();
    int x = gpJoueur->getX();
    int y = gpJoueur->getY();
        
    if(animtransX>0) {
        phg[0]+=8;
        if(animtransX>=328-4) gpJoueur->setX(x+8);
        animtransX-=8;
        if(animtransX<=0){
            if (gpJeu->isDonjon() || gpJeu->isGrotte()) detRegion(gpJeu->getZone());
            gpJoueur->saveSemiEmpl();
            if (gpJeu->isDehors()) gpJeu->finZone();
            if (!gpJeu->getText()) gpJeu->setStop(false);
            gpJeu->setTransition(false);
            return;
        }
    }
    if(animtransX<0) {
        phg[0]-=8;
        if(animtransX<=-328+20) gpJoueur->setX(x-8);
        animtransX+=8;
        if(animtransX>=0){
            if (gpJeu->isDonjon() || gpJeu->isGrotte()) detRegion(gpJeu->getZone());
            gpJoueur->saveSemiEmpl();
            if (gpJeu->isDehors()) gpJeu->finZone();
            if (!gpJeu->getText()) gpJeu->setStop(false);
            gpJeu->setTransition(false);
            return;
        }
    }
    if(animtransY>0) {
        phg[1]+=8;
        if(animtransY>=248-8) gpJoueur->setY(y+8);
        animtransY-=8;
        if(animtransY<=0){
            if (gpJeu->isDonjon() || gpJeu->isGrotte()) detRegion(gpJeu->getZone());
            gpJoueur->saveSemiEmpl();
            if (gpJeu->isDehors()) gpJeu->finZone();
            if (!gpJeu->getText()) gpJeu->setStop(false);
            gpJeu->setTransition(false);
            return;
        }
    }
    if(animtransY<0) {
        phg[1]-=8;
        if(animtransY<=-248+32-8+8) gpJoueur->setY(y-8);
        animtransY+=8;
        if(animtransY>=0){
            if (gpJeu->isDonjon() || gpJeu->isGrotte()) detRegion(gpJeu->getZone());
            gpJoueur->saveSemiEmpl();
            if (gpJeu->isDehors()) gpJeu->finZone();
            if (!gpJeu->getText()) gpJeu->setStop(false);
            gpJeu->setTransition(false);
            return;
        }
    }
    if(animtransC>0) {
        animtransC+=10;
        SDL_FillRect(imagetransit, NULL, SDL_MapRGB(imagetransit->format, 0, 0, 0));
        int rayon;
        if (animtransC <=320) rayon = 320-animtransC;
        else rayon=animtransC-330;
        if (animtransC < 310 || animtransC >= 350) filledEllipseRGBA(imagetransit,
                        gpJeu->getJoueur()->getX()-phg[0]+8, 
                        gpJeu->getJoueur()->getY()-phg[1]+16,
                        rayon, rayon*2/3,
                        0, 0, 255, 255);
        SDL_SetColorKey(imagetransit,SDL_SRCCOLORKEY, SDL_MapRGB(imagetransit->format,0,0,255));
        if (animtransC==330) { 
            gpJeu->finZone();
            if (gpJoueur->getTypeAnim()!=TOUCHE && gpJoueur->getTypeAnim()!=MORT)
                gpJoueur->setTypeAnim(AUCUNE);
            return;
        }
        if (animtransC>=650) { 
            gpJeu->testAnim();
            if (!gpJeu->getText()) gpJeu->setStop(false);
            gpJeu->setTransition(false);
            animtransC=0;
            gpJeu->testFin();
            return;
        }
    }
    
    gpJeu->setPhg(phg[0],phg[1]);
}

int Monde::getUtile() {return utile;}
void Monde::setUtile(int i) {utile = i;}

void Monde::rempli(int x, int y, int la, int lo) {
    for (int j=0; j<lo; j++)
        for (int i=0; i<la; i++)
            if (mur[(x+i)*2][(y+j)*2]==-1) setValeur((x+i)*16,(y+j)*16,2949,0);
}

void Monde::vide(int x, int y, int la, int lo) {
    for (int j=y; j<y+lo; j++)
        for (int i=x; i<x+la; i++)
            if (map[i][j]==2949) setValeur(i*16,j*16,2949,-1);
    for (int i=y; i<y+lo; i++)
        for (int j=x; j<x+la; j++) {
            
            if (mur[j*2][i*2]==-1) {
            
            if (mur[(j-1)*2][(i-1)*2]==-1 && mur[j*2][(i-1)*2]==-1 && mur[(j+1)*2][(i-1)*2]==-1
            && mur[(j-1)*2][i*2]==-1 && mur[(j+1)*2][i*2]==-1
            && mur[(j-1)*2][(i+1)*2]==-1 && mur[j*2][(i+1)*2]==-1 && mur[(j+1)*2][(i+1)*2]==-1) {
                setValeur(j*16,i*16,2914,-1);
            }
                
            if (mur[j*2][(i-1)*2]!=-1 && mur[(j-1)*2][i*2]==-1 && mur[(j+1)*2][i*2]==-1 && mur[j*2][(i+1)*2]==-1) setValeur(j*16,i*16,2911,-1);
            if (mur[j*2][(i-1)*2]==-1 && mur[(j-1)*2][i*2]==-1 && mur[(j+1)*2][i*2]==-1 && mur[j*2][(i+1)*2]!=-1) setValeur(j*16,i*16,2917,-1);
            if (mur[j*2][(i-1)*2]==-1 && mur[(j-1)*2][i*2]!=-1 && mur[(j+1)*2][i*2]==-1 && mur[j*2][(i+1)*2]==-1) setValeur(j*16,i*16,2913,-1);
            if (mur[j*2][(i-1)*2]==-1 && mur[(j-1)*2][i*2]==-1 && mur[(j+1)*2][i*2]!=-1 && mur[j*2][(i+1)*2]==-1) setValeur(j*16,i*16,2915,-1);
                
            if (mur[j*2][(i-1)*2]!=-1 && mur[(j-1)*2][i*2]!=-1 && mur[(j+1)*2][i*2]==-1 && mur[j*2][(i+1)*2]==-1) setValeur(j*16,i*16,2910,-1);
            if (mur[j*2][(i-1)*2]!=-1 && mur[(j-1)*2][i*2]==-1 && mur[(j+1)*2][i*2]!=-1 && mur[j*2][(i+1)*2]==-1) setValeur(j*16,i*16,2912,-1);
            if (mur[j*2][(i-1)*2]==-1 && mur[(j-1)*2][i*2]!=-1 && mur[(j+1)*2][i*2]==-1 && mur[j*2][(i+1)*2]!=-1) setValeur(j*16,i*16,2916,-1);
            if (mur[j*2][(i-1)*2]==-1 && mur[(j-1)*2][i*2]==-1 && mur[(j+1)*2][i*2]!=-1 && mur[j*2][(i+1)*2]!=-1) setValeur(j*16,i*16,2918,-1);
                
            if (mur[j*2][(i-1)*2]==-1 && mur[(j-1)*2][i*2]==-1 && mur[(j+1)*2][i*2]==-1 && mur[j*2][(i+1)*2]==-1) {
                if (mur[(j-1)*2][(i-1)*2]!=-1) setValeur(j*16,i*16,2922,-1);
                if (mur[(j+1)*2][(i-1)*2]!=-1) setValeur(j*16,i*16,2921,-1);
                if (mur[(j-1)*2][(i+1)*2]!=-1) setValeur(j*16,i*16,2920,-1);
                if (mur[(j+1)*2][(i+1)*2]!=-1) setValeur(j*16,i*16,2919,-1);
            }
            
            if (gpJeu->getZone()==25) map[j][i]+=438;
            
            }
        }    
}

void Monde::passage(int x, int y, int id) {
    if (gpJeu->isDehors()) {
        if (id < 2) {
            map[x][y]=534; mur[x*2+1][y*2+1]=0;
            map[x+1][y]=535; mur[(x+1)*2][y*2+1]=0;
            map[x][y+1]=536; mur[x*2+1][(y+1)*2]=0; mur[x*2+1][(y+1)*2+1]=0;
            map[x+1][y+1]=537; mur[(x+1)*2][(y+1)*2]=0; mur[(x+1)*2][(y+1)*2+1]=0;
            if (id==1) {
                map[x][y+1]=792;
                map[x+1][y+1]=793;
            }
        }
        if (id==2) {
            map[x][y]=971;
            setValeur((x+1)*16, y*16, 972, 999, LIBRE, 0);
            map[x+2][y]=973;
            map[x][y+1]=974;
            setValeur((x+1)*16, (y+1)*16, 975, 999, LIBRE, 0);
            map[x+2][y+1]=976;
            map[x][y+2]=977;
            setValeur((x+1)*16, (y+2)*16, 978, 999, LIBRE, 0);
            map[x+2][y+2]=979;
            mur[x*2][(y+2)*2]=1;
            mur[x*2+5][(y+2)*2]=1;
        }
        if (id==3) {
            for (int a = 2; a<10; a++) mur[x*2+a][y*2]=0;
            for (int a = 0; a<6; a++) air[x+a][y]=0;
            for (int a = 2; a<4; a++) air[x+a][y+1]=0;
            air[x][y]=2018;
            air[x+5][y]=2019;
        }
        if (id==4) {
            air[x][y-1]=0; air[x+1][y-1]=0;
            setValeur(x*16,y*16,158,0); setValeur((x+1)*16,y*16,159,0);
            contenu[x][y]=0; contenu[x+1][y]=0;
        }
        return;
    }
    switch (gpJeu->getZone()) {
        case 15 :
            if (id == 0) {
                map[x][y]=1193; map[x+1][y]=1194;
                map[x][y+1]=1195; map[x+1][y+1]=1196;
                air[x][y]=1220; air[x+1][y]=1221;
                map[x][y-2]=1198; map[x+1][y-2]=1199;
                map[x][y-1]=1200; map[x+1][y-1]=1201;
                air[x][y-1]=1226; air[x+1][y-1]=1227;
                for (int j = y*2-3; j < y*2+4; j++) mur[x*2+1][j]=0;
                for (int j = y*2-3; j < y*2+4; j++) mur[x*2+2][j]=0;
            }
            if (id == 1) {
                map[x][y]=1242; map[x+1][y]=1203;
                map[x][y+1]=1204; map[x+1][y+1]=1205;
                map[x][y+2]=1243; map[x+1][y+2]=1207;
                air[x][y+1]=1234;
                map[x-2][y]=1209; map[x-1][y]=1244;
                map[x-2][y+1]=1211; map[x-1][y+1]=1212;
                map[x-2][y+2]=1213; map[x-1][y+2]=1245;
                air[x-1][y+1]=1241;
                for (int j = x*2-3; j < x*2+4; j++) mur[j][y*2+2]=0;
                for (int j = x*2-3; j < x*2+4; j++) mur[j][y*2+3]=0;
            }
            break;
        case 16 :
        case 20 :
            if (id == 0 || id ==4) {
                map[x][y]=1402; map[x+1][y]=1403;
                map[x][y+1]=1404; map[x+1][y+1]=1405;
                air[x][y]=1406; air[x+1][y]=1407;
                map[x][y-2]=1408; map[x+1][y-2]=1409;
                map[x][y-1]=1410; map[x+1][y-1]=1411;
                air[x][y-1]=1400; air[x+1][y-1]=1401;
                for (int j = y*2-3; j < y*2+4; j++) mur[x*2+1][j]=0;
                for (int j = y*2-3; j < y*2+4; j++) mur[x*2+2][j]=0;
            }
            if (id == 1 || id ==3) {
                map[x][y]=1412; map[x+1][y]=1413;
                map[x][y+1]=1414; map[x+1][y+1]=1415;
                map[x][y+2]=1416; map[x+1][y+2]=1417;
                air[x][y+1]=1418;
                map[x-2][y]=1419; map[x-1][y]=1420;
                map[x-2][y+1]=1421; map[x-1][y+1]=1422;
                map[x-2][y+2]=1423; map[x-1][y+2]=1424;
                air[x-1][y+1]=1425;
                for (int j = x*2-3; j < x*2+4; j++) mur[j][y*2+2]=0;
                for (int j = x*2-3; j < x*2+4; j++) mur[j][y*2+3]=0;
            }
            break;
        case 17 :
            if (id == 0) {
                map[x][y]=1565; map[x+1][y]=1566;
                map[x][y+1]=1567; map[x+1][y+1]=1568;
                air[x][y]=1569; air[x+1][y]=1570;
                map[x][y-2]=1571; map[x+1][y-2]=1572;
                map[x][y-1]=1573; map[x+1][y-1]=1574;
                air[x][y-1]=1575; air[x+1][y-1]=1576;
                for (int j = y*2-3; j < y*2+4; j++) mur[x*2+1][j]=0;
                for (int j = y*2-3; j < y*2+4; j++) mur[x*2+2][j]=0;
            }
            if (id == 1) {
                map[x][y]=1577; map[x+1][y]=1578;
                map[x][y+1]=1579; map[x+1][y+1]=1580;
                map[x][y+2]=1581; map[x+1][y+2]=1582;
                air[x][y+1]=1583;
                map[x-2][y]=1584; map[x-1][y]=1585;
                map[x-2][y+1]=1586; map[x-1][y+1]=1587;
                map[x-2][y+2]=1588; map[x-1][y+2]=1589;
                air[x-1][y+1]=1590;
                for (int j = x*2-3; j < x*2+4; j++) mur[j][y*2+2]=0;
                for (int j = x*2-3; j < x*2+4; j++) mur[j][y*2+3]=0;
            }
            if (id ==4) {
                map[x][y]=1611; map[x+1][y]=1612;
                map[x][y+1]=1613; map[x+1][y+1]=1614;
                air[x][y]=1615; air[x+1][y]=1616;
                map[x][y-2]=1617; map[x+1][y-2]=1618;
                map[x][y-1]=1619; map[x+1][y-1]=1620;
                air[x][y-1]=1621; air[x+1][y-1]=1622;
                for (int j = y*2-3; j < y*2+4; j++) mur[x*2+1][j]=0;
                for (int j = y*2-3; j < y*2+4; j++) mur[x*2+2][j]=0;
            }
            if (id ==3) {
                map[x][y]=1623; map[x+1][y]=1624;
                map[x][y+1]=1625; map[x+1][y+1]=1626;
                map[x][y+2]=1627; map[x+1][y+2]=1628;
                air[x][y+1]=1629;
                map[x-2][y]=1630; map[x-1][y]=1631;
                map[x-2][y+1]=1632; map[x-1][y+1]=1633;
                map[x-2][y+2]=1634; map[x-1][y+2]=1635;
                air[x-1][y+1]=1636;
                for (int j = x*2-3; j < x*2+4; j++) mur[j][y*2+2]=0;
                for (int j = x*2-3; j < x*2+4; j++) mur[j][y*2+3]=0;
            }
            if (id == 8) {
                setValeur(x*16,y*16,1650,-1);
                setValeur((x+1)*16,y*16,1652,-1);
                setValeur(x*16,(y+1)*16,1656,-1);
                setValeur((x+1)*16,(y+1)*16,1658,-1);
            }    
            break;
        case 18 :
            if (id == 0) {
                map[x][y]=1774; map[x+1][y]=1775;
                map[x][y+1]=1776; map[x+1][y+1]=1777;
                air[x][y]=1778; air[x+1][y]=1779;
                map[x][y-2]=1780; map[x+1][y-2]=1781;
                map[x][y-1]=1782; map[x+1][y-1]=1783;
                air[x][y-1]=1784; air[x+1][y-1]=1785;
                for (int j = y*2-3; j < y*2+4; j++) mur[x*2+1][j]=0;
                for (int j = y*2-3; j < y*2+4; j++) mur[x*2+2][j]=0;
            }
            if (id == 1) {
                map[x][y]=1786; map[x+1][y]=1787;
                map[x][y+1]=1788; map[x+1][y+1]=1789;
                map[x][y+2]=1790; map[x+1][y+2]=1791;
                air[x][y+1]=1792;
                map[x-2][y]=1793; map[x-1][y]=1794;
                map[x-2][y+1]=1795; map[x-1][y+1]=1796;
                map[x-2][y+2]=1797; map[x-1][y+2]=1798;
                air[x-1][y+1]=1799;
                for (int j = x*2-3; j < x*2+4; j++) mur[j][y*2+2]=0;
                for (int j = x*2-3; j < x*2+4; j++) mur[j][y*2+3]=0;
            }
            if (id ==4) {
                map[x][y]=1852; map[x+1][y]=1853;
                map[x][y+1]=1854; map[x+1][y+1]=1855;
                air[x][y]=1856; air[x+1][y]=1857;
                map[x][y-2]=1858; map[x+1][y-2]=1859;
                map[x][y-1]=1860; map[x+1][y-1]=1861;
                air[x][y-1]=1862; air[x+1][y-1]=1863;
                for (int j = y*2-3; j < y*2+4; j++) mur[x*2+1][j]=0;
                for (int j = y*2-3; j < y*2+4; j++) mur[x*2+2][j]=0;
            }
            if (id == 8) {
                setValeur(x*16,y*16,1872,-1);
                setValeur((x+1)*16,y*16,1874,-1);
                setValeur(x*16,(y+1)*16,1878,-1);
                setValeur((x+1)*16,(y+1)*16,1880,-1);
            }
            break;
        case 19 :
            if (id == 0) {
                map[x][y]=1949; map[x+1][y]=1950;
                map[x][y+1]=1951; map[x+1][y+1]=1952;
                air[x][y]=1953; air[x+1][y]=1954;
                map[x][y-2]=1955; map[x+1][y-2]=1956;
                map[x][y-1]=1957; map[x+1][y-1]=1958;
                air[x][y-1]=1959; air[x+1][y-1]=1960;
                for (int j = y*2-3; j < y*2+4; j++) mur[x*2+1][j]=0;
                for (int j = y*2-3; j < y*2+4; j++) mur[x*2+2][j]=0;
            }
            if (id == 1) {
                map[x][y]=1961; map[x+1][y]=1962;
                map[x][y+1]=1963; map[x+1][y+1]=1964;
                map[x][y+2]=1965; map[x+1][y+2]=1966;
                air[x][y+1]=1967;
                map[x-2][y]=1968; map[x-1][y]=1969;
                map[x-2][y+1]=1970; map[x-1][y+1]=1971;
                map[x-2][y+2]=1972; map[x-1][y+2]=1973;
                air[x-1][y+1]=1974;
                for (int j = x*2-3; j < x*2+4; j++) mur[j][y*2+2]=0;
                for (int j = x*2-3; j < x*2+4; j++) mur[j][y*2+3]=0;
            }
            if (id ==3) {
                map[x][y]=1995; map[x+1][y]=1996;
                map[x][y+1]=1997; map[x+1][y+1]=1998;
                map[x][y+2]=1999; map[x+1][y+2]=2220;
                air[x][y+1]=2221;
                map[x-2][y]=2222; map[x-1][y]=2223;
                map[x-2][y+1]=2224; map[x-1][y+1]=2225;
                map[x-2][y+2]=2226; map[x-1][y+2]=2227;
                air[x-1][y+1]=2228;
                for (int j = x*2-3; j < x*2+4; j++) mur[j][y*2+2]=0;
                for (int j = x*2-3; j < x*2+4; j++) mur[j][y*2+3]=0;
            }
            break;
        case 21 :
            if (id == 0) {
                map[x][y]=2408; map[x+1][y]=2409;
                map[x][y+1]=2366; map[x+1][y+1]=2367;
                air[x][y]=2410; air[x+1][y]=2411;
                map[x][y-2]=2369; map[x+1][y-2]=2370;
                map[x][y-1]=2412; map[x+1][y-1]=2413;
                air[x][y-1]=2414; air[x+1][y-1]=2415;
                for (int j = y*2-3; j < y*2+4; j++) mur[x*2+1][j]=0;
                for (int j = y*2-3; j < y*2+4; j++) mur[x*2+2][j]=0;
            }
            if (id == 1) {
                map[x][y]=2416; map[x+1][y]=2375;
                map[x][y+1]=2417; map[x+1][y+1]=2377;
                map[x][y+2]=2418; map[x+1][y+2]=2379;
                air[x][y+1]=2419;
                map[x-2][y]=2381; map[x-1][y]=2421;
                map[x-2][y+1]=2383; map[x-1][y+1]=2422;
                map[x-2][y+2]=2385; map[x-1][y+2]=2423;
                air[x-1][y+1]=2424;
                for (int j = x*2-3; j < x*2+4; j++) mur[j][y*2+2]=0;
                for (int j = x*2-3; j < x*2+4; j++) mur[j][y*2+3]=0;
            }
            break;
        case 22 :
            if (id == 0) {
                y-=2;
                if (x<160) {
                for (int j=0; j<8; j++) {mur[x*2+1][y*2+j]=0;mur[x*2+2][y*2+j]=0;}
                for (int i=0; i<4; i++) map[x+(i%2)][y+((int)(i/2))]=2560+i;
                for (int i=0; i<4; i++) map[x+(i%2)][y+2+((int)(i/2))]=2554+i;
                air[x][y+1]=2564; air[x+1][y+1]=2565;
                air[x][y+2]=2558; air[x+1][y+2]=2559;
                }else {
                for (int j=0; j<8; j++) {mur[x*2+1][y*2+j]=0;mur[x*2+2][y*2+j]=0;}
                for (int i=0; i<4; i++) map[x+(i%2)][y+((int)(i/2))]=2692+i;
                for (int i=0; i<4; i++) map[x+(i%2)][y+2+((int)(i/2))]=2686+i;
                air[x][y+1]=2696; air[x+1][y+1]=2697;
                air[x][y+2]=2690; air[x+1][y+2]=2691;
                }
            }
            if (id == 1) {
                x-=2;
                if (x<160) {
                for (int i=0; i<8; i++) {mur[x*2+i][y*2+2]=0;mur[x*2+i][y*2+3]=0;}
                for (int i=0; i<6; i++) map[x+2+(i%2)][y+((int)(i/2))]=2566+i;
                for (int i=0; i<6; i++) map[x+(i%2)][y+((int)(i/2))]=2573+i;
                air[x+2][y+1]=2572;
                air[x+1][y+1]=2579;
                } else {
                for (int i=0; i<8; i++) {mur[x*2+i][y*2+2]=0;mur[x*2+i][y*2+3]=0;}
                for (int i=0; i<6; i++) map[x+2+(i%2)][y+((int)(i/2))]=2698+i;
                for (int i=0; i<6; i++) map[x+(i%2)][y+((int)(i/2))]=2705+i;
                air[x+2][y+1]=2704;
                air[x+1][y+1]=2711;
                }
            }
            if (id ==4) {
                y-=2;
                for (int j=0; j<8; j++) {mur[x*2+1][y*2+j]=0;mur[x*2+2][y*2+j]=0;}
                for (int i=0; i<4; i++) map[x+(i%2)][y+((int)(i/2))]=2648+i;
                for (int i=0; i<4; i++) map[x+(i%2)][y+2+((int)(i/2))]=2642+i;
                air[x][y+1]=2652; air[x+1][y+1]=2653;
                air[x][y+2]=2646; air[x+1][y+2]=2647;
            }
            if (id ==3) {
                x-=2;
                for (int i=0; i<8; i++) {mur[x*2+i][y*2+2]=0;mur[x*2+i][y*2+3]=0;}
                for (int i=0; i<6; i++) map[x+2+(i%2)][y+((int)(i/2))]=2774+i;
                for (int i=0; i<6; i++) map[x+(i%2)][y+((int)(i/2))]=2781+i;
                air[x+2][y+1]=2780;
                air[x+1][y+1]=2787;
            }
            break;
        case 23 :
            if (id == 0) {
                y-=2;
                for (int j=0; j<8; j++) {mur[x*2+1][y*2+j]=0;mur[x*2+2][y*2+j]=0;}
                for (int i=0; i<4; i++) map[x+(i%2)][y+((int)(i/2))]=2838+i;
                for (int i=0; i<4; i++) map[x+(i%2)][y+2+((int)(i/2))]=2832+i;
                air[x][y+1]=2842; air[x+1][y+1]=2843;
                air[x][y+2]=2836; air[x+1][y+2]=2837;
            }
            if (id == 1) {
                x-=2;
                for (int i=0; i<8; i++) {mur[x*2+i][y*2+2]=0;mur[x*2+i][y*2+3]=0;}
                for (int i=0; i<6; i++) map[x+2+(i%2)][y+((int)(i/2))]=2844+i;
                for (int i=0; i<6; i++) map[x+(i%2)][y+((int)(i/2))]=2851+i;
                air[x+2][y+1]=2850;
                air[x+1][y+1]=2857;
            }
            if (id ==3) {
                x-=2;
                for (int i=0; i<8; i++) {mur[x*2+i][y*2+2]=0;mur[x*2+i][y*2+3]=0;}
                for (int i=0; i<6; i++) map[x+2+(i%2)][y+((int)(i/2))]=2935+i;
                for (int i=0; i<6; i++) map[x+(i%2)][y+((int)(i/2))]=2942+i;
                air[x+2][y+1]=2941;
                air[x+1][y+1]=2948;
            }
            if (id == 8) {
                setValeur(x*16,y*16,2910,-1);
                setValeur((x+1)*16,y*16,2912,-1);
                setValeur(x*16,(y+1)*16,2916,-1);
                setValeur((x+1)*16,(y+1)*16,2918,-1);
            }
            break;
        case 24 :
            if (id == 0) {
                y-=2;
                for (int j=0; j<8; j++) {mur[x*2+1][y*2+j]=0;mur[x*2+2][y*2+j]=0;}
                for (int i=0; i<4; i++) map[x+(i%2)][y+((int)(i/2))]=3031+i;
                for (int i=0; i<4; i++) map[x+(i%2)][y+2+((int)(i/2))]=3025+i;
                air[x][y+1]=3023; air[x+1][y+1]=3024;
                air[x][y+2]=3029; air[x+1][y+2]=3030;
            }
            if (id == 1) {
                x-=2;
                for (int i=0; i<8; i++) {mur[x*2+i][y*2+2]=0;mur[x*2+i][y*2+3]=0;}
                for (int i=0; i<6; i++) map[x+2+(i%2)][y+((int)(i/2))]=3035+i;
                for (int i=0; i<6; i++) map[x+(i%2)][y+((int)(i/2))]=3042+i;
                air[x+2][y+1]=3041;
                air[x+1][y+1]=3048;
            }
            if (id == 3) {
                x-=2;
                for (int i=0; i<8; i++) {mur[x*2+i][y*2+2]=0;mur[x*2+i][y*2+3]=0;}
                for (int i=0; i<6; i++) map[x+2+(i%2)][y+((int)(i/2))]=3138+i;
                for (int i=0; i<6; i++) map[x+(i%2)][y+((int)(i/2))]=3145+i;
                air[x+2][y+1]=3144;
                air[x+1][y+1]=3151;
            }
            if (id == 4) {
                y-=2;
                for (int j=0; j<8; j++) {mur[x*2+1][y*2+j]=0;mur[x*2+2][y*2+j]=0;}
                for (int i=0; i<4; i++) map[x+(i%2)][y+((int)(i/2))]=3132+i;
                for (int i=0; i<4; i++) map[x+(i%2)][y+2+((int)(i/2))]=3126+i;
                air[x][y+1]=3136; air[x+1][y+1]=3137;
                air[x][y+2]=3130; air[x+1][y+2]=3131;
            }
            if (id == 8) {
                setValeur(x*16,y*16,3089,-1);
                setValeur((x+1)*16,y*16,3091,-1);
                setValeur(x*16,(y+1)*16,3095,-1);
                setValeur((x+1)*16,(y+1)*16,3097,-1);
            }
            break;
        case 25 :
            if (id == 0) {
                y-=2;
                for (int j=0; j<8; j++) {mur[x*2+1][y*2+j]=0;mur[x*2+2][y*2+j]=0;}
                for (int i=0; i<4; i++) map[x+(i%2)][y+((int)(i/2))]=3209+i;
                for (int i=0; i<4; i++) map[x+(i%2)][y+2+((int)(i/2))]=3203+i;
                air[x][y+1]=3213; air[x+1][y+1]=3214;
                air[x][y+2]=3207; air[x+1][y+2]=3208;
            }
            if (id == 1) {
                x-=2;
                for (int i=0; i<8; i++) {mur[x*2+i][y*2+2]=0;mur[x*2+i][y*2+3]=0;}
                for (int i=0; i<6; i++) map[x+2+(i%2)][y+((int)(i/2))]=3215+i;
                for (int i=0; i<6; i++) map[x+(i%2)][y+((int)(i/2))]=3222+i;
                air[x+2][y+1]=3221;
                air[x+1][y+1]=3228;
            }
            if (id == 3) {
                x-=2;
                for (int i=0; i<8; i++) {mur[x*2+i][y*2+2]=0;mur[x*2+i][y*2+3]=0;}
                for (int i=0; i<6; i++) map[x+2+(i%2)][y+((int)(i/2))]=3289+i;
                for (int i=0; i<6; i++) map[x+(i%2)][y+((int)(i/2))]=3296+i;
                air[x+2][y+1]=3295;
                air[x+1][y+1]=3302;
            }
            if (id == 4) {
                y-=2;
                for (int j=0; j<8; j++) {mur[x*2+1][y*2+j]=0;mur[x*2+2][y*2+j]=0;}
                for (int i=0; i<4; i++) map[x+(i%2)][y+((int)(i/2))]=3283+i;
                for (int i=0; i<4; i++) map[x+(i%2)][y+2+((int)(i/2))]=3277+i;
                air[x][y+1]=3287; air[x+1][y+1]=3288;
                air[x][y+2]=3281; air[x+1][y+2]=3282;
            }
            break;
        case 26 :
            if (id == 0 && x<180) {
                y-=2;
                for (int j=0; j<8; j++) {mur[x*2+1][y*2+j]=0;mur[x*2+2][y*2+j]=0;}
                for (int i=0; i<4; i++) map[x+(i%2)][y+((int)(i/2))]=3506+i;
                for (int i=0; i<4; i++) map[x+(i%2)][y+2+((int)(i/2))]=3500+i;
                air[x][y+1]=3510; air[x+1][y+1]=3511;
                air[x][y+2]=3504; air[x+1][y+2]=3505;
            }
            if (id == 0 && x>=180) {
                y-=2;
                for (int j=0; j<8; j++) {mur[x*2+1][y*2+j]=0;mur[x*2+2][y*2+j]=0;}
                for (int i=0; i<4; i++) map[x+(i%2)][y+((int)(i/2))]=3681+i;
                for (int i=0; i<4; i++) map[x+(i%2)][y+2+((int)(i/2))]=3675+i;
                air[x][y+1]=3685; air[x+1][y+1]=3686;
                air[x][y+2]=3679; air[x+1][y+2]=3680;
            }
            if (id == 1) {
                x-=2;
                for (int i=0; i<8; i++) {mur[x*2+i][y*2+2]=0;mur[x*2+i][y*2+3]=0;}
                for (int i=0; i<6; i++) map[x+2+(i%2)][y+((int)(i/2))]=3512+i;
                for (int i=0; i<6; i++) map[x+(i%2)][y+((int)(i/2))]=3519+i;
                air[x+2][y+1]=3518;
                air[x+1][y+1]=3525;
            }
            if (id == 3) {
                x-=2;
                for (int i=0; i<8; i++) {mur[x*2+i][y*2+2]=0;mur[x*2+i][y*2+3]=0;}
                for (int i=0; i<6; i++) map[x+2+(i%2)][y+((int)(i/2))]=3586+i;
                for (int i=0; i<6; i++) map[x+(i%2)][y+((int)(i/2))]=3593+i;
                air[x+2][y+1]=3592;
                air[x+1][y+1]=3599;
            }
            break;
        case 27 :
            if (id == 0) {
                y-=2;
                for (int j=0; j<8; j++) {mur[x*2+1][y*2+j]=0;mur[x*2+2][y*2+j]=0;}
                for (int i=0; i<4; i++) map[x+(i%2)][y+((int)(i/2))]=3774+i;
                for (int i=0; i<4; i++) map[x+(i%2)][y+2+((int)(i/2))]=3768+i;
                air[x][y+1]=3778; air[x+1][y+1]=3779;
                air[x][y+2]=3772; air[x+1][y+2]=3773;
            }
            if (id == 1) {
                x-=2;
                for (int i=0; i<8; i++) {mur[x*2+i][y*2+2]=0;mur[x*2+i][y*2+3]=0;}
                for (int i=0; i<6; i++) map[x+2+(i%2)][y+((int)(i/2))]=3780+i;
                for (int i=0; i<6; i++) map[x+(i%2)][y+((int)(i/2))]=3787+i;
                air[x+2][y+1]=3786;
                air[x+1][y+1]=3793;
            }
            if (id == 4) {
                y-=2;
                for (int j=0; j<8; j++) {mur[x*2+1][y*2+j]=0;mur[x*2+2][y*2+j]=0;}
                for (int i=0; i<4; i++) map[x+(i%2)][y+((int)(i/2))]=3846+i;
                for (int i=0; i<4; i++) map[x+(i%2)][y+2+((int)(i/2))]=3840+i;
                air[x][y+1]=3850; air[x+1][y+1]=3851;
                air[x][y+2]=3844; air[x+1][y+2]=3845;
            }
            break;
        case 28 :
            if (id == 0) {
                y-=2;
                for (int j=0; j<8; j++) {mur[x*2+1][y*2+j]=0;mur[x*2+2][y*2+j]=0;}
                for (int i=0; i<4; i++) map[x+(i%2)][y+((int)(i/2))]=3905+i;
                for (int i=0; i<4; i++) map[x+(i%2)][y+2+((int)(i/2))]=3899+i;
                air[x][y+1]=3909; air[x+1][y+1]=3910;
                air[x][y+2]=3903; air[x+1][y+2]=3904;
            }
            if (id == 1) {
                x-=2;
                for (int i=0; i<8; i++) {mur[x*2+i][y*2+2]=0;mur[x*2+i][y*2+3]=0;}
                for (int i=0; i<6; i++) map[x+2+(i%2)][y+((int)(i/2))]=3911+i;
                for (int i=0; i<6; i++) map[x+(i%2)][y+((int)(i/2))]=3918+i;
                air[x+2][y+1]=3917;
                air[x+1][y+1]=3924;
            }
            break;
        case 45 :
            map[x][y]=1402; map[x+1][y]=1403;
            map[x][y+1]=1404; map[x+1][y+1]=1405;
            air[x][y]=1406; air[x+1][y]=1407;
            for (int j = y*2; j < y*2+4; j++) mur[x*2+1][j]=0;
            for (int j = y*2; j < y*2+4; j++) mur[x*2+2][j]=0;
            break;
    }
}
