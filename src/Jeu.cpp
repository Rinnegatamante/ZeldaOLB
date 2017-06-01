/*

    Zelda Oni Link Begins

    Copyright (C) 2006-2008  Vincent Jouillat

    Please send bugreports with examples or suggestions to www.zeldaroth.fr

*/

#include "Joueur.h"
#include "Monde.h"
#include "Menu.h"
#include "Texte.h"
#include "Projectile.h"
#include "Jeu.h"
#include "Keyboard.h"

#include <cstdlib>
#include <iostream>

Jeu::Jeu(Audio* audio) : gpAudio(audio), gpGenerique(0), gpKeyboard(0),
zone(1), stop(false), transition(false),
menu(false), text(false), vueHorz(0), vueVert(0) {
    phg[0] = 0;
    phg[1] = 0;
    
    imageObjets = IMG_Load("app0:/images/objet/objets.png");
    SDL_SetColorKey(imageObjets,SDL_SRCCOLORKEY,SDL_MapRGB(imageObjets->format,0,0,255));
    
    gpProjectile = NULL; gpObjet = NULL; gpSnipe = NULL; gpCaisse = NULL; gpEnnemi = NULL;
    gpPiege = NULL; gpPnj = NULL; gpJoueur = NULL; gpMonde = NULL; gpMenu = NULL;
    gpStatut = NULL;
    gpTexte = new Texte(this);
}

Jeu::~Jeu() {
    delete gpMonde;
    delete gpJoueur;
    delete gpMenu;
    delete gpTexte;
    delete gpStatut;
    delete gpProjectile;
    delete gpObjet;
    delete gpSnipe;
    delete gpCaisse;
    delete gpEnnemi;
    delete gpPiege;
    delete gpPnj;
}

void Jeu::init(int save) {
    zone=29;
    gpProjectile = new Projectile(this, 0, N, 0, 0, 0);
    gpObjet = new Objet(this, 0, 0, 0, 0);
    gpSnipe = new Snipe(this, 0, 0, 0, 0, 0);
    gpCaisse = new Caisse(this, 0, 0, 0);
    gpEnnemi = new Ennemi(this, 0, 0, 0, true);
    gpPiege = new Ennemi(this, 0, 0, 0, true);
    gpPnj = new Pnj(this, 0, 0, 0, 0);
    gpJoueur = new Joueur(this, save);
    gpMonde = new Monde(this);
    gpMenu = new Menu(this);
    //gpTexte = new Texte(this);
    gpStatut = new Statut(this);
    //gpAudio->playMusic(zone);
    if (zone != 55 || gpJoueur->getAvancement()<65)gpAudio->playMusic(zone);
        else gpAudio->playMusic(79);
    if (isDonjon()) ecrit(201+zone-15);
}

void Jeu::reinit(int save) {
    zone=29;
    if(gpProjectile) delete gpProjectile;
    gpProjectile = new Projectile(this, 0, N, 0, 0, 0);
	if(gpObjet) delete gpObjet;
    gpObjet = new Objet(this, 0, 0, 0, 0);
    if(gpSnipe) delete gpSnipe;
	gpSnipe = new Snipe(this, 0, 0, 0, 0, 0);
    if(gpCaisse) delete gpCaisse;
	gpCaisse = new Caisse(this, 0, 0, 0);
    if(gpEnnemi) delete gpEnnemi;
	gpEnnemi = new Ennemi(this, 0, 0, 0, true);
    if(gpPiege) delete gpPiege;
	gpPiege = new Ennemi(this, 0, 0, 0, true);
    if(gpPnj) delete gpPnj;
	gpPnj = new Pnj(this, 0, 0, 0, 0);
    if(gpJoueur) delete gpJoueur;
	gpJoueur = new Joueur(this, save);
    if(gpMonde) delete gpMonde;
	gpMonde = new Monde(this);
    if(gpMenu) delete gpMenu;
	gpMenu = new Menu(this);
    //gpTexte = new Texte(this);
    if(gpStatut) delete gpStatut;
	gpStatut = new Statut(this);
    //gpAudio->playMusic(zone);
    if (zone != 55 || gpJoueur->getAvancement()<65)gpAudio->playMusic(zone);
        else gpAudio->playMusic(79);
    if (isDonjon()) ecrit(201+zone-15);
}


void Jeu::setKeyboard(Keyboard* kb) {
    gpKeyboard = kb;
}

void Jeu::setGenerique(Generique* gen) {
    gpGenerique = gen;
}

void Jeu::videListes() {
    delete gpProjectile;
    gpProjectile = new Projectile(this, 0, N, 0, 0, 0);
    delete gpObjet;
    gpObjet = new Objet(this, 0, 0, 0, 0);
    delete gpSnipe;
    gpSnipe = new Snipe(this, 0, 0, 0, 0, 0);
    delete gpCaisse;
    gpCaisse = new Caisse(this, 0, 0, 0);
    delete gpEnnemi;
    gpEnnemi = new Ennemi(this, 0, 0, 0, true);
    delete gpPiege;
    gpPiege = new Ennemi(this, 0, 0, 0, true);
    delete gpPnj;
    gpPnj = new Pnj(this, 0, 0, 0, 0);
}

void Jeu::scrolling() {
    phg[0] = gpJoueur->getX() - 320/2 + 8 + vueHorz;
    phg[1] = gpJoueur->getY() - 240/2 + 16 + vueVert;
    if (phg[0] < gpMonde->regionValue(0)) phg[0] = gpMonde->regionValue(0);
    if (phg[1] < gpMonde->regionValue(1)) phg[1] = gpMonde->regionValue(1);
    if (phg[0] > gpMonde->regionValue(2) - 320) phg[0] = gpMonde->regionValue(2) - 320;
    if (phg[1] > gpMonde->regionValue(3) - 240) phg[1] = gpMonde->regionValue(3) - 240;
}

void Jeu::replace() {
    gpEnnemi->revie();
    gpPiege->revie();
    gpPnj->revie();
    gpCaisse->revie();
    gpObjet->revie();
    delete gpSnipe;
    gpSnipe = new Snipe(this, 0, 0, 0, 0, 0);
    delete gpProjectile;
    gpProjectile = new Projectile(this, 0, N, 0, 0, 0);
    gpMonde->replace();
}

int Jeu::nbEnnemis() {
    int t=0;
    Ennemi* gpEnnemi2 = gpEnnemi->getSuivant();
    while (gpEnnemi2 != NULL) {
        if (gpEnnemi2->getVie()>0 
        && gpEnnemi2->getX()+gpEnnemi2->getW()>gpMonde->regionValue(0) 
        && gpEnnemi2->getX()<gpMonde->regionValue(2) 
        && gpEnnemi2->getY()+gpEnnemi2->getH()>gpMonde->regionValue(1) 
        && gpEnnemi2->getY()<gpMonde->regionValue(3)) t++;
        gpEnnemi2 = gpEnnemi2->getSuivant();
    }
    return t;
}

void Jeu::draw(SDL_Surface* screen) {
    
    if (!transition) scrolling();
    
    gpMonde->drawSol(screen, phg);
    
    gpPiege->drawSpecial(screen);
    
    if (!transition) {
        gpCaisse->draw(screen);
        gpObjet->draw(screen);
        gpPnj->draw(screen);
        gpEnnemi->draw(screen);
        gpSnipe->draw(screen);
        gpProjectile->draw(screen);
    }
    
    gpJoueur->draw(screen);
    gpPiege->draw(screen);
    gpMonde->drawAir(screen, phg);
    
    if (transition) gpMonde->defilTransit();
    
    gpStatut->draw(screen);
    
	if (menu || gpMenu->getVal()) gpMenu->draw(screen);
    if (text && !transition) gpTexte->draw(screen);
}

Menu* Jeu::getMenuPtr() {
    return gpMenu;
}

Joueur* Jeu::getJoueur() {
    return gpJoueur;
}

Monde* Jeu::getMonde() {
    return gpMonde;
}

Audio* Jeu::getAudio() {
    return gpAudio;
}

Texte* Jeu::getTexte() {
    return gpTexte;
}

Statut* Jeu::getStatut() {
    return gpStatut;
}

Generique* Jeu::getGenerique() {
    return gpGenerique;
}

Keyboard* Jeu::getKeyboard() {
    return gpKeyboard;
}

void Jeu::ajouteProjectile(int type, Direction dir, int a, int b, int v) {
    Projectile* proj = new Projectile(this, type, dir, a, b, v);
    if (gpProjectile == NULL) gpProjectile = proj;
    else gpProjectile->ajout(proj);
}

void Jeu::ajouteObjet(int type, int i, int j, int num, int p) {
    Objet* obj = new Objet(this, i, j, type, num, p);
    if (gpObjet == NULL) gpObjet = obj;
    else gpObjet->ajout(obj);
}

void Jeu::ajouteSnipe(int type, double i, double j, double dirx, double diry) {
    Snipe* snp = new Snipe(this, type, i, j, dirx, diry);
    if (gpSnipe == NULL) gpSnipe = snp;
    else gpSnipe->ajout(snp);
}

void Jeu::ajouteCaisse(int type, int i, int j) {
    Caisse* css = new Caisse(this, type, i, j);
    if (gpCaisse == NULL) gpCaisse = css;
    else gpCaisse->ajout(css);
}

void Jeu::ajouteEnnemi(int type, int i, int j, bool p) {
    Ennemi* enn = new Ennemi(this, type, i, j, p);
    if (gpEnnemi == NULL) gpEnnemi = enn;
    else gpEnnemi->ajout(enn);
}

void Jeu::ajoutePiege(int type, int i, int j, bool p) {
    Ennemi* enn = new Ennemi(this, type, i, j, p);
    if (gpPiege == NULL) gpPiege = enn;
    else gpPiege->ajout(enn);
}

void Jeu::ajoutePnj(int type, int i, int j, int m) {
    Pnj* pnj = new Pnj(this, type, i, j, m);
    if (gpPnj == NULL) gpPnj = pnj;
    else gpPnj->ajout(pnj);
}

bool Jeu::isDonjon() {
    return (zone >=15 && zone<=28);
}

bool Jeu::isDehors() {
    return (zone < 15);
}

bool Jeu::isGrotte() {
    return (zone > 28 && zone < 44);
}

void Jeu::poule() {
    Ennemi* gpEnnemi2 = gpEnnemi->getSuivant();
    while (gpEnnemi2 != NULL) {
        if (gpEnnemi2->getEnnId()==52) gpEnnemi2->enerve();
        gpEnnemi2 = gpEnnemi2->getSuivant();
    }
    if (gpJoueur->getAsile()==0) gpJoueur->setAsile(1);
}

void Jeu::lire() {
    int i=0;
    int j=0;
    switch(gpJoueur->getDirection()) {
        case N : i=gpJoueur->getX()+8; j=gpJoueur->getY()+7-2-2; break;
        case S : i=gpJoueur->getX()+8; j=gpJoueur->getY()+24+2; break;
        case O : i=gpJoueur->getX()-1-2; j=gpJoueur->getY()+16; break;
        case E : i=gpJoueur->getX()+16+2; j=gpJoueur->getY()+16; break;
    }
    int id = gpMonde->contenuValue(i, j);
    int val = gpMonde->motifValue(i, j);
    //lit pancarte ou achete
    if (gpMonde->murValue(i, j) && id && gpJoueur->getDirection()==N && 
    (val == 33 || val == 34 || val == 36 || val == 193 || val == 160 || val == 161 
    || val == 560 || val == 561 || val == 2324 || val == 4093 || val == 4095 || val == 4097)) {
        ecrit(id);
        return;
    }
    //ouvre coffre
    if (gpMonde->murValue(i, j) && val==303 && gpJoueur->getDirection()==N) {
        switch (id) {
            case C_RIEN : 
            case C_QUART_COEUR :
                gpMonde->setValeur(6*16,6*16,304,-1,1,0); 
                gpMonde->setValeur(8*16,6*16,304,-1,1,0); 
                gpMonde->setValeur(10*16,6*16,304,-1,1,0);
                gpMonde->setValeur(6*16,8*16,304,-1,1,0); 
                gpMonde->setValeur(8*16,8*16,304,-1,1,0); 
                gpMonde->setValeur(10*16,8*16,304,-1,1,0);
                gpMonde->setValeur(6*16,10*16,304,-1,1,0); 
                gpMonde->setValeur(8*16,10*16,304,-1,1,0); 
                gpMonde->setValeur(10*16,10*16,304,-1,1,0);
                if (id == C_RIEN) {gpPnj->getSuivant()->setMessage(300); ecrit(304);}
                if (id == C_QUART_COEUR) {
                    gpPnj->getSuivant()->setMessage(299);
                    gpJoueur->setCoeur(28); trouve(C_QUART_COEUR);
                }
                break;
            case C_PAIE : ecrit(id); break;
            default : gpMonde->setValeur(i, j, 304, -1, PLEIN, C_RIEN); trouve(id);
        }
        return;
    }
    //parle pnj
    Pnj* gpPnj2 = gpPnj->getSuivant();
    while (gpPnj2 != NULL) {
        int x1 = gpPnj2->getX();
        int y1 = gpPnj2->getY();
        int w1 = x1+gpPnj2->getZoneBase()->getZone()->x+gpPnj2->getZoneBase()->getZone()->w;
        int h1 = y1+gpPnj2->getZoneBase()->getZone()->y+gpPnj2->getZoneBase()->getZone()->h;
        if (i>=x1 && i<w1 && j>=y1 && j<h1 && gpPnj2->getVie()) {
            ecrit(gpPnj2->getMessage());
            return;
        }
        gpPnj2 = gpPnj2->getSuivant();
    }
}

void Jeu::trouve(int id) {
    ecrit(id);
    gpJoueur->setTrouve(id);
    gpAudio->playSound(19,2);
}

bool Jeu::finTexte(int id) {
    if (gpJoueur) gpJoueur->setTrouve(0);
    switch (id) {
        
        //textes olb
        
        case 7 : gpJoueur->trouveObjet(O_BOTTES); break;
        case 16 : case 18 : case 21 : case 25 : case 27 : case 29 : case 32 : case 35 :
            gpJoueur->setTypeAnim(AUCUNE); break;
        case 17 : case 19 : case 22 : case 30 : 
            if (gpJoueur->getAvancement()==67 && zone == 9) {
                ecrit(155);
                gpJoueur->setTypeAnim(AUCUNE);
            }
            else gpJoueur->setMelodie(0,2); 
            return true;
        case 20 : case 23 : case 26 : case 33 :
            gpJoueur->setMelodie(1,2); return true;
        case 24 : case 28 : case 31 : case 34 :
            gpJoueur->setMelodie(2,2); return true;
        case 45 :
            if (zone == 3) {
                //gpJoueur->setBouteille(0,1);
                gpJoueur->setBoostRubis(-100);
                trouve(C_BOUTEILLE);
                return true;
            }
            if (zone == 7) {
                gpJoueur->setBouclier(2);
                gpJoueur->setBoostRubis(-250);
                trouve(C_BOUCLIER_R);
                return true;
            }
            break;
        case 46 : 
            if (zone == 3) gpJoueur->setBouteille(0,1);
            if (zone == 53) gpJoueur->setBouteille(1,1);
            if (zone == 41) gpJoueur->setBouteille(2,1);
            break;
        case 47 : if (gpJoueur->getAvancement()==3) gpJoueur->setAvancement(4); break;
        case 49 : if (gpJoueur->getAvancement()==4) gpJoueur->setAvancement(5); break;
        case 50 : 
            if (gpJoueur->getAvancement()==5) gpJoueur->setAvancement(6);
            if (gpJoueur->getAvancement()==8) gpJoueur->setAvancement(9); 
            if ((gpJoueur->getAvancement()==9 || gpJoueur->getAvancement()==10) 
                && zone == 55) gpJoueur->setAvancement(11); break;
        case 54 : gpJoueur->setBoostRubis(-5); break;
        case 127 : gpJoueur->setRaplaplat(); break;
        case 130 : gpMonde->sortieVillage(); break;
        
        case 131 : gpJoueur->setEpee(1); break;
        case 132 : gpJoueur->setCle(zone-15,0); break;
        case 133 : gpJoueur->setCle(zone-15,1); break;
        case 134 : gpJoueur->setCle(zone-15,2); break;
        
        case 135 : 
            gpJoueur->setCle(zone-15,3,gpJoueur->getCle(zone-15,3)+1);
            switch (zone) {
                case 15 :
                    if (gpMonde->regionValue(0) == 60*16) gpJoueur->setCoffre(0,8);
                    if (gpMonde->regionValue(0) == 160*16) gpJoueur->setCoffre(0,9);
                    if (gpMonde->regionValue(0) == 20*16) gpJoueur->setCoffre(0,10);
                    if (gpMonde->regionValue(0) == 180*16) gpJoueur->setCoffre(0,11);
                    break;
                case 16 :
                    if (gpMonde->regionValue(0) == 160*16) gpJoueur->setCoffre(1,25);
                    if (gpMonde->regionValue(0) == 180*16 && gpMonde->regionValue(1) == 15*16) 
                        gpJoueur->setCoffre(1,26);
                    if (gpMonde->regionValue(0) == 180*16 && gpMonde->regionValue(1) == 45*16) 
                        gpJoueur->setCoffre(1,27);
                    break;
                case 17 :
                    if (gpMonde->regionValue(0) == 40*16) gpJoueur->setCoffre(2,17);
                    if (gpMonde->regionValue(0) == 80*16) gpJoueur->setCoffre(2,18);
                    if (gpMonde->regionValue(0) == 140*16) gpJoueur->setCoffre(2,19);
                    if (gpMonde->regionValue(0) == 180*16) gpJoueur->setCoffre(2,20);
                    break;
                case 18 :
                    if (gpMonde->regionValue(0) == 100*16) gpJoueur->setCoffre(3,24);
                    if (gpMonde->regionValue(0) == 120*16) gpJoueur->setCoffre(3,25);
                    if (gpMonde->regionValue(0) == 160*16) gpJoueur->setCoffre(3,26);
                    if (gpMonde->regionValue(0) == 200*16) gpJoueur->setCoffre(3,27);
                    if (gpMonde->regionValue(0) == 220*16 && gpMonde->regionValue(1) == 45*16) 
                        gpJoueur->setCoffre(3,28);
                    if (gpMonde->regionValue(0) == 280*16 && gpMonde->regionValue(1) == 0) 
                        gpJoueur->setCoffre(3,29);
                    if (gpMonde->regionValue(0) == 280*16 && gpMonde->regionValue(1) == 45*16) 
                        gpJoueur->setCoffre(3,30);
                    if (gpMonde->regionValue(0) == 220*16 && gpMonde->regionValue(1) == 30*16) 
                        gpJoueur->setCoffre(3,31);
                    break;
                case 19 :
                    if (gpMonde->regionValue(0) == 0) gpJoueur->setCoffre(4,14);
                    if (gpMonde->regionValue(0) == 140*16) gpJoueur->setCoffre(4,15);
                    if (gpMonde->regionValue(0) == 160*16) gpJoueur->setCoffre(4,16);
                    if (gpMonde->regionValue(0) == 180*16) gpJoueur->setCoffre(4,17);
                    if (gpMonde->regionValue(0) == 100*16) gpJoueur->setCoffre(4,18);
                    break;
                case 20 :
                    if (gpMonde->regionValue(0) == 0 && gpMonde->regionValue(1) == 75*16)
                        gpJoueur->setCoffre(5,2);
                    break;
                case 21 :
                    if (gpMonde->regionValue(0) == 80*16) gpJoueur->setCoffre(6,18);
                    if (gpMonde->regionValue(0) == 100*16) gpJoueur->setCoffre(6,19);
                    if (gpMonde->regionValue(0) == 120*16 && gpJoueur->getX()<140*16) 
                        gpJoueur->setCoffre(6,20);
                    if (gpMonde->regionValue(0) == 120*16 
                        && gpJoueur->getX()>140*16 && gpJoueur->getY()<30*16) 
                        gpJoueur->setCoffre(6,21);
                    if (gpMonde->regionValue(0) == 120*16 
                        && gpJoueur->getX()>140*16 && gpJoueur->getY()>30*16) 
                        gpJoueur->setCoffre(6,22);
                    if (gpMonde->regionValue(0) == 200*16) gpJoueur->setCoffre(6,23);
                    if (gpMonde->regionValue(0) == 240*16) gpJoueur->setCoffre(6,24);
                    break;
                case 22 :
                    if (gpMonde->regionValue(0) == 0) gpJoueur->setCoffre(7,15);
                    if (gpMonde->regionValue(0) == 80*16) gpJoueur->setCoffre(7,16);
                    if (gpMonde->regionValue(0) == 100*16) gpJoueur->setCoffre(7,17);
                    if (gpMonde->regionValue(0) == 140*16) gpJoueur->setCoffre(7,18);
                    if (gpMonde->regionValue(1) == 75*16) gpJoueur->setCoffre(7,19);
                    if (gpMonde->regionValue(1) == 0) gpJoueur->setCoffre(7,20);
                    break;
                case 23 :
                    if (gpMonde->regionValue(0) == 100*16) gpJoueur->setCoffre(8,15);
                    if (gpMonde->regionValue(0) == 260*16) gpJoueur->setCoffre(8,17);
                    if (gpMonde->regionValue(0) == 340*16) gpJoueur->setCoffre(8,18);
                    break;
                case 24 :
                    if (gpMonde->regionValue(0) == 0) gpJoueur->setCoffre(9,19);
                    if (gpMonde->regionValue(0) == 80*16) gpJoueur->setCoffre(9,20);
                    if (gpMonde->regionValue(0) == 140*16) gpJoueur->setCoffre(9,21);
                    if (gpMonde->regionValue(0) == 160*16) gpJoueur->setCoffre(9,22);
                    if (gpMonde->regionValue(0) == 100*16) gpJoueur->setCoffre(9,23);
                    break;
                case 25 :
                    if (gpMonde->regionValue(0) == 120*16 && gpJoueur->getX()<130*16) 
                        gpJoueur->setCoffre(10,15);
                    if (gpMonde->regionValue(0) == 120*16 && gpJoueur->getX()>130*16) 
                        gpJoueur->setCoffre(10,16);
                    if (gpMonde->regionValue(1) == 60*16) gpJoueur->setCoffre(10,17);
                    if (gpMonde->regionValue(1) == 75*16) gpJoueur->setCoffre(10,18);
                    if (gpMonde->regionValue(0) == 220*16) gpJoueur->setCoffre(10,19);
                    break;
                case 26 :
                    if (gpMonde->regionValue(0) == 0) gpJoueur->setCoffre(11,19);
                    if (gpMonde->regionValue(1) == 0) gpJoueur->setCoffre(11,20);
                    if (gpMonde->regionValue(1) == 75*16) gpJoueur->setCoffre(11,21);
                    if (gpMonde->regionValue(0) == 120*16) gpJoueur->setCoffre(11,22);
                    if (gpMonde->regionValue(0) == 320*16) gpJoueur->setCoffre(11,23);
                    break;
                case 27 :
                    if (gpMonde->regionValue(1) == 60*16) gpJoueur->setCoffre(12,12);
                    if (gpMonde->regionValue(1) == 120*16) gpJoueur->setCoffre(12,13);
                    if (gpMonde->regionValue(0) == 20*16) gpJoueur->setCoffre(12,14);
                    if (gpMonde->regionValue(0) == 140*16) gpJoueur->setCoffre(12,15);
                    break;
                case 28 :
                    if (gpMonde->regionValue(0) == 60*16) gpJoueur->setCoffre(13,7);
                    if (gpMonde->regionValue(1) == 30*16) gpJoueur->setCoffre(13,9);
                    break;
            }
            break;
        
        case 137 : case 146 : case 151 : case 157 : case 177 : case 178 :
            if (id == 157) gpJoueur->trouveObjet(O_GRAPPIN); 
            if (id == 151) {gpJoueur->trouveObjet(O_ARC, 2); 
                gpJoueur->setFleche(gpJoueur->getFlecheMax());}
            if (id == 137 && gpJoueur->nbCristaux() == 5) {
                gpJoueur->setAvancement(16);
                gpJoueur->setDirection(S);
                gpJoueur->setTypeAnim(AUCUNE);
                gpAudio->playSpecial(4);
                gpJoueur->setImmo(true);
            }else {ecrit(199); return true;}
            break;
        
        case 142 : trouve(C_CLE); return true; break;
        case 145 : trouve(C_SAC_BOMBES); gpJoueur->trouveObjet(O_SAC_BOMBES); 
            gpJoueur->setBombe(gpJoueur->getBombeMax());
            return true; break;
        case 147 : gpJoueur->trouveObjet(O_GANTS); break;
        case 150 : trouve(C_ARC); return true; break;
        case 152 : if (gpJoueur->getAvancement()==64) gpJoueur->setAvancement(65); break;
        case 154 : if (gpJoueur->getAvancement()==13) gpJoueur->setAvancement(14); break;
        case 156 : gpJoueur->trouveObjet(O_MARTEAU); break;
        case 165 : gpJoueur->trouveObjet(O_PALMES); break;
        case 166 : 
            if (zone == 26) gpJoueur->setCoffre(11,25);
            else gpJoueur->setCoffre(7,21);
            gpJoueur->setTunique(2); break;
        case 167 : 
            if (zone == 26) gpJoueur->setCoffre(11,25);
            else gpJoueur->setCoffre(7,21);
            gpJoueur->setTunique(3); break;
        case 170 : gpJoueur->setAvancement(63); break;
        case 171 : gpJoueur->setAvancement(64); break;
        case 173 : gpJoueur->setAvancement(1); gpJoueur->setImmo(true); break;
        case 175 : gpJoueur->trouveObjet(O_BFEU); break;
        case 176 : gpJoueur->setEpee(gpJoueur->getEpee()+1);
            switch (zone) {
                case 23 : gpJoueur->setCoffre(8,19); break;
                case 24 : gpJoueur->setCoffre(9,24); break;
                case 25 : gpJoueur->setCoffre(10,21); break;
            }
            trouve(C_EPEE_R); return true; break;
        case 179 : gpJoueur->trouveObjet(O_BGLACE); break;
        case 180 : gpJoueur->setBouclier(3); break;
        case 181 : case 391 : gpJoueur->moveY(4); gpJoueur->setDirection(S); break;
        case 182 : gpStatut->setJauge(true); break;
        case 183 : gpJoueur->trouveObjet(O_MASQUE,2); break;
        case 189 : gpJoueur->setCoffre(11,24); gpAudio->playSound(5);
            SDL_Delay(1500); gpAudio->playSpecial(3); ecrit(199); return true; break;
        case 193 : gpJoueur->setAvancement(67); break;
        case 194 : gpJoueur->trouveObjet(O_ARC,5); ecrit(199); return true; break;
        case 195 : gpAudio->playSpecial(5); gpKeyboard->setMode(11);
            ecrit(223, false, true, 0, 0, 322, 240); break;
        case 197 : gpJoueur->setTutoriel(1); break;
        case 199 : gpJoueur->save(); ecrit(406); return true; break;
        case 216 : gpJoueur->save();
        case 217 : 
            gpAudio->stopSpecial(); gpAudio->stopMusic(); videListes(); 
            gpKeyboard->setMode(2); gpGenerique->initLogo(); return false; break;
        case 224 :
            gpAudio->stopSpecial();
            gpAudio->stopMusic(); 
            gpGenerique->initScore();
            zone = 29;
            gpJoueur->setVie(gpJoueur->getVieMax());
            gpJoueur->setXDep(152);
            gpJoueur->setYDep(72);
            gpJoueur->setDirDep(S);
            gpJoueur->save();
            gpKeyboard->setMode(14); break;
        case 225 : gpJoueur->setBouclier(1); break;
        case 227 : 
            if (gpJoueur->getMagieMax()==32) {
                trouve(C_MIMAGIE); gpJoueur->setMagieMax(64); return true;} break;
        case 229 : gpJoueur->trouveObjet(O_OCARINA); 
        case 230 : gpJoueur->trouveObjet(O_LANTERNE); 
            gpJoueur->setMagie(gpJoueur->getMagieMax()); break;
        case 231 : gpJoueur->trouveObjet(O_MASQUE); break;
        case 237 : gpJoueur->setBoostRubis(-100);
            gpJoueur->setBombeMax(gpJoueur->getBombeMax()+2); 
            gpJoueur->setBombe(gpJoueur->getBombeMax()); break;
        case 238 : gpJoueur->setBoostRubis(-100);
            gpJoueur->setFlecheMax(gpJoueur->getFlecheMax()+5); 
            gpJoueur->setFleche(gpJoueur->getFlecheMax()); break;
        case 239 : gpJoueur->setBoostRubis(-100);
            if (!gpJoueur->hasMelodie(0)) break;
            gpAudio->playSound(5);
            gpMonde->transitFull(9, 16*39+8, 16*80); break;
        case 240 : gpJoueur->trouveObjet(O_GANTS, 2); break;
        case 243 : gpPnj->getSuivant()->setImmo(false); 
            gpPnj->getSuivant()->setMessage(50); break;
        case 244 : gpPnj->getSuivant()->getSuivant()->setImmo(false);
            gpPnj->getSuivant()->getSuivant()->setMessage(50); break;
        case 250 : if (gpJoueur->getAvancement()==1) gpJoueur->setAvancement(2); break;
        case 258 : 
            if (gpJoueur->getAvancement()==2) gpJoueur->setAvancement(3); 
            gpJoueur->trouveObjet(O_CARTE); 
            ecrit(401); gpJoueur->setTrouve(C_CARTE); gpAudio->playSound(19,2);
            return true; break;
        case 259 : trouve(C_EXCALIBUR); return true; break;
        case 260 : gpJoueur->setEpee(2); ecrit(261); return true; break;
        case 282 : if (gpJoueur->getAvancement()==68) gpJoueur->setAvancement(69); break;
        case 289 : 
            if (gpJoueur->getEpee()>1 && gpJoueur->hasObjet(O_ENCYCL)==1) {
                gpJoueur->trouveObjet(O_ENCYCL,2);
                ecrit(294); return true;}
            else if (gpJoueur->getSolde()>0) {ecrit(291); return true;}
            break;
        case 290 : gpJoueur->setEnnemi(75); break;
        case 292 : gpJoueur->paye(); break;
        case 295 : 
            if (gpJoueur->getEnnemi(19) && gpJoueur->getEnnemi(24) && gpJoueur->getEnnemi(35)) {
                gpJoueur->trouveObjet(O_ENCYCL,3);
                ecrit(296); return true;}
            else if (gpJoueur->getSolde()>0) {ecrit(291); return true;}
            break;
        case 296 : 
        case 297 :
            if (gpJoueur->getSolde()>0) {ecrit(291); return true;} break;
        case 298 : gpJoueur->setBombe(gpJoueur->getBombeMax()); break;
        case 300 :
            int hasard;
            hasard=0;
            gpMonde->setValeur(6*16,6*16,303,-1,1,0); 
            gpMonde->setValeur(8*16,6*16,303,-1,1,0); 
            gpMonde->setValeur(10*16,6*16,303,-1,1,0);
            gpMonde->setValeur(6*16,8*16,303,-1,1,0); 
            gpMonde->setValeur(8*16,8*16,303,-1,1,0); 
            gpMonde->setValeur(10*16,8*16,303,-1,1,0);
            gpMonde->setValeur(6*16,10*16,303,-1,1,0); 
            gpMonde->setValeur(8*16,10*16,303,-1,1,0); 
            gpMonde->setValeur(10*16,10*16,303,-1,1,0);
            hasard = rand()%9;
            gpMonde->setContenu(6+(hasard%3)*2,6+(hasard/3)*2,C_QUART_COEUR);
            gpJoueur->setBoostRubis(-10); gpPnj->getSuivant()->setMessage(302);
            ecrit(45); return true; break;
        case 306 : if (gpJoueur->getAvancement()==6) gpJoueur->setAvancement(7); break;
        case 312 : if (gpJoueur->getAvancement()==7) gpJoueur->setAvancement(8);
            gpJoueur->setBoostRubis(-10); break;
        case 313 :
            gpJoueur->setBoostRubis(-5);
            gpJoueur->setBoostVie(gpJoueur->getBoostVie()+2);
            if (gpJoueur->getVie()==gpJoueur->getVieMax()) gpAudio->playSound(15);
            ecrit(45); return true; break;
        case 316 : 
            gpJoueur->setBoostRubis(-15);
            gpJoueur->setBoostMagie(gpJoueur->getBoostMagie()+gpJoueur->getMagieMax()/8);
            gpAudio->playSound(13);
            ecrit(45); return true; break;
        case 318 : 
            gpJoueur->setBoostRubis(-25);
            gpJoueur->setBoostMagie(gpJoueur->getBoostMagie()+gpJoueur->getMagieMax()/4);
            if (gpJoueur->getMagie()<gpJoueur->getMagieMax())
                gpAudio->playSound(37);
            else gpAudio->playSound(13);
            ecrit(45); return true; break;
        case 321 : 
            gpJoueur->setBoostRubis(-20);
            gpJoueur->setFleche(gpJoueur->getFleche()+10); 
            gpAudio->playSound(13);
            ecrit(45); return true; break;
        case 323 : 
            gpJoueur->setBoostRubis(-30);
            gpJoueur->setBombe(gpJoueur->getBombe()+5); 
            gpAudio->playSound(13);
            ecrit(45); return true; break;
        case 331 :
            int i; i=0; while(gpJoueur->hasBouteille(i)!=1) i++;
            gpJoueur->setBouteille(i,2);
            gpJoueur->setBoostRubis(-60);
            trouve(C_POTION_ROUGE);
            return true;
        case 333 :
            i=0; while(gpJoueur->hasBouteille(i)!=1) i++;
            gpJoueur->setBouteille(i,3);
            gpJoueur->setBoostRubis(-60);
            trouve(C_POTION_VERTE);
            return true;
        case 338 : gpJoueur->setAsile(2); break;
        case 346 : if (gpJoueur->getAvancement()==11) gpJoueur->setAvancement(12); break;
        case 354 : if (gpJoueur->getAvancement()==12) gpJoueur->setAvancement(13); break;
        case 356 :
            gpJoueur->setMelodie(1);
            gpAudio->stopMusic();
            SDL_Delay(500);
            gpAudio->playSound(41);
            SDL_Delay(6500); 
            gpAudio->replayMusic();
            gpAudio->playSound(19,2);
            trouve(C_HYMNE_VIDE); return true;
            break;
        case 360 :
            i=0; while(gpJoueur->hasBouteille(i)!=1) i++;
            gpJoueur->setBouteille(i,4);
            gpJoueur->setBoostRubis(-160);
            trouve(C_POTION_BLEUE);
            return true;
        case 369 : gpJoueur->setBoostRubis(-10); ecrit(371); return true; break;
        case 377 :
            gpJoueur->setMelodie(2);
            gpAudio->stopMusic();
            SDL_Delay(500);
            gpAudio->playSound(42);
            SDL_Delay(12000); 
            gpAudio->replayMusic();
            gpAudio->playSound(19,2);
            trouve(C_CHANT_ESPRIT); return true;
            break;
        case 385 : if (gpJoueur->getAvancement()==65) gpJoueur->setAvancement(66); break;
        case 388 :
            gpJoueur->setMelodie(0);
            gpAudio->stopMusic();
            SDL_Delay(500);
            gpAudio->playSound(40);
            SDL_Delay(3500); 
            gpAudio->replayMusic();
            gpAudio->playSound(19,2);
            trouve(C_CHANT_ENVOL); return true;
            break;
        case 392 : gpJoueur->setAvancement(68); break;
        case 395 : gpJoueur->setTutoriel(2); break;
        case 405 : case 222 : return false; break;
        case 411 : gpStatut->setJauge(true); break;
    }
    testAnim();
    return false;
}

void Jeu::testRamasse(int x, int y, int w, int h) {
    Objet* gpObjet2 = gpObjet->getSuivant();
    while (gpObjet2 != NULL) {
        if (gpObjet2->getVie()) {
            if (x<gpObjet2->getX()+gpObjet2->getW()-2 && x+w>gpObjet2->getX()+2
            && y<gpObjet2->getY()+gpObjet2->getH()-2 && y+h-4>gpObjet2->getY()+2) {
                gpObjet2->ramasse();
            }
        }
        gpObjet2 = gpObjet2->getSuivant();
    }
}

bool Jeu::testEpee(int x, int y, int w, int h, int type, int force, Direction direction) {
    bool retour = false;
    
    // pour OLB only
    if (type==HYMNE_VIDE) {
        int a=-1;
        for (int i = x; i < x + w; i+=16) {
            for (int j = y; j < y + h; j+=16) {
                if (gpMonde->motifValue(i,j)==160) a=i;
                if (gpMonde->motifValue(i,j)==161) a=i-1;
                if (a != -1) {
                    gpMonde->passage(a/16,j/16,4);
                    gpAudio->playSound(9);
                    gpJoueur->setCoffre(14,10);
                    return true;
                }
            }
        }
        return false;
    }
    if (type==CHANT_ESPRIT) {
        if (zone == 26 && gpJoueur->hasObjet(O_MASQUE)==1 
        && x==300*16 && y==30*16) {
            gpEnnemi->getSuivant()->capture();
            gpAudio->playSpecial(3);
            return true;
        }
        return false;
    }
    
    //if (type==COUP_EPEE || type==EPEE_SPIN) testRamasse(x, y, w, h);
    for (int i = x; i < x + w; i++) {
        for (int j = y; j < y + h; j++) {
            if (i >= 0 && j >=0) {
                retour = (retour || coupeBuisson(i, j, type));
                if (type==COUP_MARTEAU) retour = (retour || applatitBloc(i, j));
                if (type==FEU) retour = (retour || allumeTorche(i, j));
                if (type==COUP_GLACE) retour = (retour || etteintTorche(i, j));
                if (type==EXPLOSION) retour = (retour || exploseMur(i, j));
                if (type==COUP_EPEE || type==EPEE_SPIN) {
                    retour = (retour || casseBarriere(i, j, type));
                    retour = (retour || renvoieProj(i, j, direction));}
                retour = (retour || tueEnnemi(i, j, type, force, direction));
            }
        }
    }
    return retour;
}

bool Jeu::renvoieProj(int i, int j, Direction dir) {
    
    Snipe* gpSnipe2 = gpSnipe->getSuivant();
    while (gpSnipe2 != NULL) {
        if (i >= gpSnipe2->getX() && i < gpSnipe2->getX() + gpSnipe2->getW()
        && j >= gpSnipe2->getY() && j < gpSnipe2->getY() + gpSnipe2->getH()) {
            if (gpSnipe2->renvoit(dir)) {
                gpAudio->playSound(7);
                return true;
            }
        }
        gpSnipe2 = gpSnipe2->getSuivant();
    }
    
    return false;
}

bool Jeu::coupeBuisson(int i, int j, int type) {
    int donne = 0;
    bool retour = false;
    if (type==COUP_EPEE || type==EPEE_SPIN || type==FEU 
    || type==COUP_GLACE || type==EXPLOSION) {
        if (gpMonde->motifValue(i,j)==26) {
            if (gpMonde->contenuValue(i,j)==-1) gpMonde->setValeur(i,j,25,-1);
            else gpMonde->setValeur(i, j, 29, -1, 0, 0);
            donne=1;
            ajouteProjectile(9, N, i-(i%16)-5,j-(j%16)-13, 0);
        }    
        if (gpMonde->motifValue(i,j)==385) {
            gpMonde->setValeur(i, j, 386, -1, 0, 0);
            donne=2;
            ajouteProjectile(10, N, i-(i%16)-5,j-(j%16)-13, 0);
        }
        if (gpMonde->motifValue(i,j)==48) {
            gpMonde->setValeur(i, j, 49, -1, 0, 0);
            donne=3;
            ajouteProjectile(11, N, i-(i%16)-5,j-(j%16)-13, 0);
        }
        if (donne) {
            retour=true;
            //nombreMystere = (std::rand() % (MAX - MIN + 1)) + MIN;
            int ob=0;
            int randomV = (int)((float)std::rand() / RAND_MAX * (15*donne));
            //int randomV = (int) ((std::rand() % (15*donne)) + 1);
            switch (randomV) {                        
                case 1: if (gpJoueur->getRubis()<gpJoueur->getRubisMax()) 
                    ob=I_RUBIS_VERT; break;
                case 2: if (gpJoueur->getVie()<gpJoueur->getVieMax()) 
                    ob=I_PETIT_COEUR; break;
                case 3: if (gpJoueur->getFleche()<gpJoueur->getFlecheMax()
                    && gpJoueur->hasObjet(O_ARC)) 
                    ob=I_FLECHE; break;
                case 4: if (gpJoueur->getBombe()<gpJoueur->getBombeMax()
                    && gpJoueur->hasObjet(O_SAC_BOMBES)) 
                    ob=I_BOMBE; break;
                case 5: if (gpJoueur->getMagie()<gpJoueur->getMagieMax()
                    && gpJoueur->hasObjet(O_LANTERNE)) 
                    ob=I_MAGIE_PEU; break; 
            }
            if(ob>0) ajouteObjet(ob, i-(i%16), j-(j%16));
        }
    }
    return retour;
}

bool Jeu::applatitBloc(int i, int j) {
    int val = gpMonde->motifValue(i,j);
    if (val==126 || val==393 || val==987) {
        gpMonde->setValeur(i, j, val+1, -1, 0, 0);
        return true;
    }
    if (val==2311) {
        gpMonde->setValeur(i,j,2310, -1, 0, 0);
        return true;
    }
    return false;
}

bool Jeu::allumeTorche(int i, int j) {
    int val = gpMonde->motifValue(i,j);
    if (val==1000) {
        gpMonde->setValeur(i, j, 1001, PLEIN);
        testAnim();
        return true;
    }
    return false;
}

bool Jeu::etteintTorche(int i, int j) {
    int val = gpMonde->motifValue(i,j);
    if (val==1001) {
        gpMonde->setValeur(i, j, 1000, PLEIN);
        testAnim();
        return true;
    }
    //gel eau
    if (val==28 || val==463 || val==466 || (val>=2450 && val<=2462))
        if (i>gpMonde->regionValue(0)+31 && j>gpMonde->regionValue(1)+31 
        && i<gpMonde->regionValue(2)+-20 && j<gpMonde->regionValue(3)+-20)
            gpMonde->setValeur(i,j,27,-1,GLACE,C_RIEN);
    if (val==540) {
        gpMonde->setValeur(i,j,980,-1,GLACE,C_RIEN);
        gpMonde->setMurs(((int)(i/16))*2,((int)(j/16))*2,2);
    }
    if (val==543) {
        gpMonde->setValeur(i,j,981,-1,GLACE,C_RIEN);
        gpMonde->setMurs(((int)(i/16))*2+1,((int)(j/16))*2,2);
    }
    return false;
}

bool Jeu::exploseMur(int i, int j) {
    int val = gpMonde->motifValue(i,j);
    
    if (val==532 || val==533 || val ==790 || val==791) {
        switch (val) {
            case 532 : gpMonde->passage(((int)(i/16)),((int)(j/16))-1,0); break;
            case 533 : gpMonde->passage(((int)(i/16))-1,((int)(j/16))-1,0); break;
            case 790 : gpMonde->passage(((int)(i/16)),((int)(j/16))-1,1); break;
            case 791 : gpMonde->passage(((int)(i/16))-1,((int)(j/16))-1,1); break;
        }
        if (zone==2) gpJoueur->setCoffre(14,0);
        if (zone==3) gpJoueur->setCoffre(14,2);
        if (zone==5) gpJoueur->setCoffre(14,3);
        if (zone==6) gpJoueur->setCoffre(14,4);
        if (zone==8) gpJoueur->setCoffre(14,5);
        if (zone==9) gpJoueur->setCoffre(14,6);
        if (zone==10) gpJoueur->setCoffre(14,7);
        if (zone==14 && i<640) gpJoueur->setCoffre(14,8);
        if (zone==14 && i>=640) gpJoueur->setCoffre(14,9);
        gpAudio->playSound(9);
        return true;
    }
    //donjon
    //h et b
    if (val==1488 || val==1489 || val==1490 || val==1491
    || val==1699 || val==1700
    || val==1846 || val==1847 || val==1848 || val==1849
    || val==2640 || val==2641
    || val==3122 || val==3123
    || val==3273 || val==3274
    || val==3838 || val==3839) {
        if (zone == 16) gpJoueur->setCoffre(1,2);
        if (zone == 17 && gpMonde->regionValue(0)==40*16 && gpMonde->regionValue(1)==15*16) 
            gpJoueur->setCoffre(2,1);
        if (zone == 17 && gpMonde->regionValue(0)==40*16 && gpMonde->regionValue(1)==75*16) 
            gpJoueur->setCoffre(2,3);
        if (zone == 17 && gpMonde->regionValue(0)==180*16) gpJoueur->setCoffre(2,6);
        if (zone == 17 && gpMonde->regionValue(0)==220*16) gpJoueur->setCoffre(2,7);
        if (zone == 18 && gpMonde->regionValue(0)==240*16) gpJoueur->setCoffre(3,10);
        if (zone == 18 && gpMonde->regionValue(0)==280*16) gpJoueur->setCoffre(3,11);
        if (zone == 22 && gpMonde->regionValue(0)==0) gpJoueur->setCoffre(7,1);
        if (zone == 22 && gpMonde->regionValue(0)==60*16) gpJoueur->setCoffre(7,2);
        if (zone == 24) gpJoueur->setCoffre(9,4);
        if (zone == 25 && gpMonde->regionValue(0)==40*16) gpJoueur->setCoffre(10,2);
        if (zone == 25 && gpMonde->regionValue(0)==100*16) gpJoueur->setCoffre(10,3);
        if (zone == 25 && gpMonde->regionValue(0)==120*16) gpJoueur->setCoffre(10,4);
        if (zone == 27 && gpMonde->regionValue(0)==100*16) gpJoueur->setCoffre(12,1);
        if (zone == 27 && gpMonde->regionValue(0)==120*16 && j<67*16) gpJoueur->setCoffre(12,2);
        if (zone == 27 && gpMonde->regionValue(0)==120*16 && j>67*16) gpJoueur->setCoffre(12,3);
        
        int x1=0, y1=0;
        switch(val) {
            case 1488 : case 1846 :
                x1=((int)(i/16)); y1=((int)(j/16))-1; break;
            case 1489 : case 1847 :
                x1=((int)(i/16))-1; y1=((int)(j/16))-1; break;
            case 1490 : case 1848 :
                x1=((int)(i/16)); y1=((int)(j/16))+2; break;
            case 1491 : case 1849 :
                x1=((int)(i/16))-1; y1=((int)(j/16))+2; break;
            case 1699 : case 2640 : case 3122 : case 3273 : case 3838 :
                x1=((int)(i/16)); y1=((int)(j/16))-1;
                if (gpMonde->motifValue(i+16,j)!=val) x1--; break;
            case 1700 : case 2641 : case 3123 : case 3274 : case 3839 :
                if (gpMonde->motifValue(i+16,j)==val) {x1=((int)(i/16)); y1=((int)(j/16))+2;}
                if (gpMonde->motifValue(i+16,j)!=val) {x1=((int)(i/16))-1; y1=((int)(j/16))+2;}
                break;
        }
        gpMonde->passage(x1,y1,4);
        gpAudio->playSound(9);
        return true;
    }
    //g et d
    if (val==1495 || val==1496 
    || val==1701 || val==1702
    || val==2308 || val==2309
    || val==2320 || val==2321
    || val==2772 || val==2773
    || val==2932 || val==2933
    || val==3124 || val==3125
    || val==3275 || val==3276
    || val==3572 || val==3573) {
        if (zone == 16 && gpMonde->regionValue(0) < 200*16) gpJoueur->setCoffre(1,3);
        if (zone == 16 && gpMonde->regionValue(0) > 200*16) gpJoueur->setCoffre(1,1);
        if (zone == 17 && gpMonde->regionValue(0) == 60*16) gpJoueur->setCoffre(2,2);
        if (zone == 17 && gpMonde->regionValue(0) == 160*16) gpJoueur->setCoffre(2,5);
        if (zone == 19) gpJoueur->setCoffre(4,1);
        if (zone == 20 && gpMonde->regionValue(0) == 60*16 && gpMonde->regionValue(1) == 45*16) 
            gpJoueur->setCoffre(5,29);
        if (zone == 20 && gpMonde->regionValue(0) == 60*16 && gpMonde->regionValue(1) == 60*16) 
            gpJoueur->setCoffre(5,30);
        if (zone == 20 && gpMonde->regionValue(0) == 100*16 && gpMonde->regionValue(1) == 45*16) 
            gpJoueur->setCoffre(5,31);
        if (zone == 20 && gpMonde->regionValue(0) == 100*16 && gpMonde->regionValue(1) == 60*16) 
            gpJoueur->setCoffre(5,32);
        if (zone == 22) gpJoueur->setCoffre(7,3);
        if (zone == 23 && gpMonde->regionValue(0) == 80*16) gpJoueur->setCoffre(8,1);
        if (zone == 23 && gpMonde->regionValue(0) == 120*16) gpJoueur->setCoffre(8,2);
        if (zone == 23 && gpMonde->regionValue(1) == 45*16) gpJoueur->setCoffre(8,4);
        if (zone == 23 && gpMonde->regionValue(1) == 60*16) gpJoueur->setCoffre(8,5);
        if (zone == 24) gpJoueur->setCoffre(9,3);
        if (zone == 25) gpJoueur->setCoffre(10,1);
        if (zone == 26) gpJoueur->setCoffre(11,1);
        
        int x1, y1;
        x1=((int)(i/16))-1; y1=((int)(j/16))-1;
        switch(val) {
            case 1496 : case 1702 : case 2309 : case 2321 : case 2773 : case 2933 :
            case 3125 : case 3276 : case 3573 :
                x1+=3; break;
        }
        gpMonde->passage(x1,y1,3);
        gpAudio->playSound(9);
        return true;
    }
    //sol fissuré
    if (val==1711 || val==1006 || val==2934 || val==3161) {
        if (zone == 17) gpJoueur->setCoffre(2,4);
        if (zone == 18) gpJoueur->setCoffre(3,9);
        if (zone == 23) gpJoueur->setCoffre(8,3);
        if (zone == 24 && gpMonde->regionValue(1) == 60*16) gpJoueur->setCoffre(9,1);
        if (zone == 24 && gpMonde->regionValue(1) == 0) gpJoueur->setCoffre(9,2);
        int x1, y1;
        x1=((int)(i/16)); y1=((int)(j/16));
        while(gpMonde->motifValue((x1-1)*16,y1*16)==val) x1--;
        while(gpMonde->motifValue(x1*16,(y1-1)*16)==val) y1--;
        gpMonde->passage(x1,y1,8);
        gpAudio->playSound(9);
        return true;
    }
    
    return false;
}

bool Jeu::casseBarriere(int i, int j, int type) {
    if ((type==COUP_EPEE || type==EPEE_SPIN) && gpJoueur->getEpee()>=2 && !gpJoueur->getOni()) {
        if (gpMonde->motifAirValue(i,j)==2002) {
            gpMonde->passage(((int)(i/16))-2,((int)(j/16)),3);
            gpAudio->playSound(9);
            if (zone==2) gpJoueur->setCoffre(14,1);
            return true;
        }
        if (gpMonde->motifAirValue(i,j)==2003) {
            gpMonde->passage(((int)(i/16))-3,((int)(j/16)),3);
            gpAudio->playSound(9);
            if (zone==2) gpJoueur->setCoffre(14,1);
            return true;
        }
    }
    return false;
}

bool Jeu::tueEnnemi(int i, int j, int type, int force, Direction direction) {
    Ennemi* gpEnnemi2 = gpEnnemi->getSuivant();
    ZoneRect* z;
    SDL_Rect* r;
    int degats;
    bool retour = false;
    while (gpEnnemi2 != NULL) {
        if (gpEnnemi2->getVie()) {
            if (type == CAISSE) z = gpEnnemi2->getZoneBase();
            else z = gpEnnemi2->getZonesVulnerables();
            while (z != NULL && gpEnnemi2->getVie()) {
                r = z->getZone();
                if (i >= gpEnnemi2->getX() + r->x && i < gpEnnemi2->getX() + r->x + r->w
                && j >= gpEnnemi2->getY() + r->y && j < gpEnnemi2->getY() + r->y + r->h) {
                    
                    if (gpEnnemi2->getEnnId()==50 && (type==COUP_EPEE || type==EPEE_SPIN)) {
                        if ((gpEnnemi2->getVie()>100 && gpJoueur->getOni())
                        || (gpEnnemi2->getVie()<=100 && !gpJoueur->getOni())) return false;
                    }
                    
                    if (gpEnnemi2->getEffet(type)) {
                        
                        if ((gpEnnemi2->getEffet(type)==2 && !gpEnnemi2->getInvincible()) 
                        || type == CAISSE) {
                            degats = force;
                            if (gpEnnemi2->getEnnId()!=20) {
                                gpEnnemi2->setVie(gpEnnemi2->getVie()-degats);
                                if (type==MORTEL) gpEnnemi2->mort();
                            }
                            else {
                                gpMonde->permuteBlocs();
                                if (!verifZoneJoueur(gpJoueur->getX(), 
                                gpJoueur->getY()+8, 16, 16, 2)) 
                                    gpMonde->permuteBlocs();
                            }
                        }
                        if (gpEnnemi2->getVie()) {
                            if (gpEnnemi2->getEnnId()==50 && type == FLECHE 
                                && gpEnnemi2->getVie()>100) gpEnnemi2->gele();
                            if (type == COUP_GLACE && !gpEnnemi2->getInvincible()
                            && gpEnnemi2->getEffet(type)==2) 
                                gpEnnemi2->gele();
                            if (type == FEU && !gpEnnemi2->getInvincible()) 
                                gpEnnemi2->degele();
                            if (gpEnnemi2->getEffet(type)==2 && !gpEnnemi2->getInvincible()) {
                                gpAudio->playSound(7);
                            }
                            if (gpEnnemi2->getEffet(type)>=1) {
                                if (!gpEnnemi2->isBoss() || !gpEnnemi2->getInvincible()) 
                                    gpEnnemi2->pousse(direction);
                                if (!gpEnnemi2->getInvincible()) gpEnnemi2->setInvincible(1);
                            }
                        }
                        else {
                            gpJoueur->setEnnemi(gpEnnemi2->getEnnId());
                            gpAudio->playSound(8);
                            ajouteProjectile(15, direction, 
                                gpEnnemi2->getX()+(gpEnnemi2->getW()/2)-14, 
                                gpEnnemi2->getY()+(gpEnnemi2->getH()/2)-14, 0);
                            donneObjet(gpEnnemi2);
                            testAnim();
                        }
                        
                    }
                    retour = true;
                }
                z = z->getSuivant();
            }
        }
        gpEnnemi2 = gpEnnemi2->getSuivant();
    }
    return retour;;
}

void Jeu::donneObjet(Ennemi* ennemi) {
    int obj;
    
    if (ennemi->isBoss()) {
        if (zone < 28) {
            if (zone == 25 && nbEnnemis()>0) return;
            obj = I_COEUR; 
            gpAudio->playSpecial(3);
            gpStatut->setJauge(false);
        }
        else {
            if (zone==28 && ennemi->getEnnId()!=50) {
                gpAudio->stopSpecial();
                gpStatut->setJauge(false);
                gpJoueur->setCoffre(13,10); return;
            }
            if (zone==28) obj = I_TRIFORCE; else obj=0;
            gpAudio->playSpecial(3);
            gpStatut->setJauge(false);
        }
    }else {
    
        int randomValue = (int)(((float)rand() / RAND_MAX) * 12);
        switch (randomValue){
            case 0: case 1: case 2: case 3: obj=ennemi->getObjets(0); break;
            case 4: case 5: case 6: obj=ennemi->getObjets(1); break;
            case 7: case 8: obj=ennemi->getObjets(2); break;                        
            default:obj=0;break;
        }
        if (
        ((obj == I_RUBIS_VERT || obj == I_RUBIS_BLEU || obj == I_RUBIS_ROUGE) 
            && gpJoueur->getRubis() >= gpJoueur->getRubisMax())
        || (obj == I_PETIT_COEUR && gpJoueur->getVie() >= gpJoueur->getVieMax())
        || (obj == I_FLECHE && (!gpJoueur->hasObjet(O_ARC)
            || gpJoueur->getFleche() >= gpJoueur->getFlecheMax()))
        || (obj == I_BOMBE && (!gpJoueur->hasObjet(O_SAC_BOMBES) 
            || gpJoueur->getBombe() >= gpJoueur->getBombeMax()))
        || ((obj == I_MAGIE_PEU || obj == I_MAGIE_BCP) 
            && (gpJoueur->getMagie() >= gpJoueur->getMagieMax() 
            || !gpJoueur->hasObjet(O_LANTERNE)))
        ) obj = 0;
    }
    
    //spécial
    if ((zone==16 && gpMonde->regionValue(0)==120*16 && gpMonde->regionValue(1)==15*16 && gpJoueur->getCoffre(1,28)==0 && ennemi->getEnnId()==8) 
    || (zone==16 && gpMonde->regionValue(0)==280*16 && gpMonde->regionValue(1)==60*16 && gpJoueur->getCoffre(1,29)==0 && ennemi->getEnnId()==8)
    || (zone==20 && gpMonde->regionValue(0)==60*16 && gpMonde->regionValue(1)==45*16 && gpJoueur->getCoffre(5,4)==0 && nbEnnemis()==0)
    || (zone==20 && gpMonde->regionValue(0)==60*16 && gpMonde->regionValue(1)==60*16 && gpJoueur->getCoffre(5,5)==0 && nbEnnemis()==0)
    || (zone==20 && gpMonde->regionValue(0)==80*16 && gpMonde->regionValue(1)==45*16 && gpJoueur->getCoffre(5,6)==0 && nbEnnemis()==0)
    || (zone==20 && gpMonde->regionValue(0)==100*16 && gpMonde->regionValue(1)==45*16 && gpJoueur->getCoffre(5,7)==0 && nbEnnemis()==0)
    || (zone==20 && gpMonde->regionValue(0)==100*16 && gpMonde->regionValue(1)==60*16 && gpJoueur->getCoffre(5,8)==0 && nbEnnemis()==0)
    || (zone==20 && gpMonde->regionValue(0)==160*16 && gpMonde->regionValue(1)==0 && gpJoueur->getCoffre(5,11)==0 && nbEnnemis()==4)
    || (zone==20 && gpMonde->regionValue(0)==160*16 && gpMonde->regionValue(1)==60*16 && gpJoueur->getCoffre(5,12)==0 && nbEnnemis()==4)
    || (zone==20 && gpMonde->regionValue(0)==220*16 && gpMonde->regionValue(1)==0 && gpJoueur->getCoffre(5,13)==0 && nbEnnemis()==4)
    || (zone==20 && gpMonde->regionValue(0)==220*16 && gpMonde->regionValue(1)==60*16 && gpJoueur->getCoffre(5,14)==0 && nbEnnemis()==4)
    || (zone==23 && gpMonde->regionValue(0)==140*16 && gpMonde->regionValue(1)==0 && gpJoueur->getCoffre(8,16)==0 && nbEnnemis()==0)
    || (zone==28 && gpMonde->regionValue(0)==140*16 && gpMonde->regionValue(1)==15*16 && gpJoueur->getCoffre(13,8)==0 && nbEnnemis()==0)
    ) obj=I_PETITE_CLE;
    
    if (zone==16 && gpMonde->regionValue(0)==140*16 
    && gpMonde->regionValue(1)==30*16 && gpJoueur->getCoffre(9,9)==0) obj=I_PETITE_CLE;
    
    if (obj) ajouteObjet(obj, ennemi->getX()+(ennemi->getW()/2)-8, 
        ennemi->getY()+(ennemi->getH()/2)-8);
}

bool Jeu::testDegat(int x, int y, int w, int h, int type, int force, Direction direction, int distance) {
    if (gpJoueur->getInvincible() || !gpJoueur->getVie()) return false;
    int degats=0;
    SDL_Rect* z = gpJoueur->getZoneBase()->getZone();
    
    int a = z->x+z->w+gpJoueur->getX();
    int b = z->x+gpJoueur->getX();
    int c = z->y+z->h+gpJoueur->getY();
    int d = z->y+gpJoueur->getY();
    
    for (int i = x; i < x + w; i++)
        for (int j = y; j < y + h; j++)
            if (i >= 0 && j >=0)
                if (type==FEU) allumeTorche(i, j);
    
    if (gpJoueur->getTypeAnim() == FLOTTE || gpJoueur->getTypeAnim() == NAGE) c-=4;
    
    if (x < a && x + w > b && y < c && y + h > d) {
        
        if (type!=LAPIN && type!=RETOUR) gpAudio->playSound(33);
        degats=force;
        if (!gpJoueur->getDefense()) degats=degats*2;
        else degats=degats/gpJoueur->getDefense();
        if (type==DEMI) degats=(gpJoueur->getVie()+1)/2;
        if (type==VENIN) degats=gpJoueur->getVie()-1;
        if (force > 0 && degats < 1) degats = 1;
        gpJoueur->setVie(gpJoueur->getVie()-degats);
        if (type==MORTEL) gpJoueur->mortel();
        if (type!=RETOUR) gpJoueur->setTypeAnim(TOUCHE);
        if (type==LAPIN) gpJoueur->setLapin();
        if (type == RETOUR) {
            int retX = gpJoueur->getXDep();
            int retY = gpJoueur->getYDep();
            if (zone==20 && gpMonde->regionValue(0)>0) {retX=7*16; retY=83*16;}
            else if (zone==23 && gpMonde->regionValue(0)==300*16) {retX=209*16+6; retY=51*16;}
            gpAudio->playSound(5);
            gpMonde->transitFull(zone, retX, retY);
        }
        if (type == COLERE && gpJoueur->getOnilink()) {
            gpJoueur->setOnijauge(gpJoueur->getOnijauge()+5);}
        
        if (type==EXPLOSION) {
            int dfx = (x+(w/2)) - (gpJoueur->getX()+8);
            int dfy = (y+(h/2)) - (gpJoueur->getY()+16);
            if (std::abs(dfx) > std::abs(dfy)) { 
                if (dfx > 0) direction = O; else direction = E;}
            else { if (dfy > 0) direction = N; else direction = S;}
        }
        
        if (type==COUP_GLACE) gpJoueur->geler();
        else gpJoueur->degeler();
        
        if (type!=LAPIN && type!=RETOUR && type!=COUP_GLACE) {
            for (int i = 0; i < distance; i+=2) {
                switch (direction) {
                    case N : gpJoueur->setDirection(S); gpJoueur->moveY(-2,2);break;
                    case S : gpJoueur->setDirection(N); gpJoueur->moveY(2,2);break;
                    case O : gpJoueur->setDirection(E); gpJoueur->moveX(-2,2);break;
                    case E : gpJoueur->setDirection(O); gpJoueur->moveX(2,2);break;
                }
            }
        }
        testFin();
        return true;
    }
    
    return false;
}

bool Jeu::pousse(int i, int j, Direction dir) {
    Caisse* gpCaisse2 = gpCaisse->getSuivant();
    while (gpCaisse2!=NULL) {
        if (i>=gpCaisse2->getX() && i<gpCaisse2->getX()+16 
        && j>=gpCaisse2->getY() && j<gpCaisse2->getY()+16
        && gpCaisse2->getVie()) {
            return gpCaisse2->pousse(dir);
        }
        gpCaisse2=gpCaisse2->getSuivant();
    }
    return false;
}

void Jeu::ouvrePorteDonjon(int x, int y) {
    //porte boss
    int val = gpMonde->motifValue(x+7,y+8);
    
    if ((val==1248 || val==1249 || val==1468 || val==1469 || val==1601 || val==1602
    || val==1822 || val==1823 || val==1985 || val==1986 || val==2428 || val==2429
    || val==2622 || val==2623 || val==2880 || val==2881 || val==3079 || val==3080
    || val==3251 || val==3252 || val==3548 || val==3549 || val==3836 || val==3837
    || val==3967 || val==3968)
    && gpJoueur->getCle(zone-15, 2)>0) {
        gpJoueur->setCoffre(zone-15, 0);
        gpAudio->playSound(20);
        int x1,y1;
        x1=(int)((x+7)/16); y1=(int)((y+8)/16)-1;
        switch(val) {
            case 1249 : case 1469 : case 1602 : case 1823 : case 1986 : case 2429 : 
            case 2623 : case 2881 : case 3080 : case 3252 : case 3549 : case 3837 :
            case 3968 :
                x1--; break;
        }
        gpMonde->passage(x1,y1,0);
    }
    //porte vérouillée h b
    if ((val==1218 || val==1219 || val==1222 || val==1223 || val==1428 || val==1429 
    || val==1430 || val==1431 || val==1593 || val==1594 || val==1595 || val==1596
    || val==1802 || val==1803 || val==1804 || val==1805 || val==1977 || val==1978 
    || val==1979 || val==1980 || val==2390 || val==2391 || val==2392 || val==2393
    || val==2582 || val==2583 || val==2584 || val==2585 || val==2860 || val==2861 
    || val==2862 || val==2863 || val==3231 || val==3232 || val==3233 || val==3234
    || val==3528 || val==3529 || val==3530 || val==3531 || val==3703 || val==3704 
    || val==3705 || val==3706 || val==3796 || val==3797 || val==3798 || val==3799
    || val==3927 || val==3928 || val==3929 || val==3930)
    && gpJoueur->getCle(zone-15, 3)>0) {
        if (zone==15) gpJoueur->setCoffre(0,2);
        if (zone==16 && gpMonde->regionValue(0)==100*16) gpJoueur->setCoffre(1,4);
        if (zone==16 && gpMonde->regionValue(1)==75*16) gpJoueur->setCoffre(1,6);
        if (zone==16 && gpMonde->regionValue(1)==60*16) gpJoueur->setCoffre(1,7);
        if (zone==16 && gpMonde->regionValue(0)>200*16) gpJoueur->setCoffre(1,8);
        if (zone==17 && gpMonde->regionValue(0)==40*16 && gpMonde->regionValue(1)==15*16) 
            gpJoueur->setCoffre(2,8);
        if (zone==17 && gpMonde->regionValue(0)==60*16 && gpMonde->regionValue(1)==45*16) 
            gpJoueur->setCoffre(2,9);
        if (zone==17 && gpMonde->regionValue(0)==40*16 && gpMonde->regionValue(1)==75*16) 
            gpJoueur->setCoffre(2,10);
        if (zone==17 && gpMonde->regionValue(0)==180*16) gpJoueur->setCoffre(2,11);
        if (zone==18 && gpMonde->regionValue(0)==120*16) gpJoueur->setCoffre(3,3);
        if (zone==18 && gpMonde->regionValue(0)==280*16 && gpJoueur->getX()<300*16) 
            gpJoueur->setCoffre(3,6);
        if (zone==18 && gpMonde->regionValue(0)==280*16 && gpJoueur->getX()>300*16) 
            gpJoueur->setCoffre(3,7);
        if (zone==19 && gpMonde->regionValue(1)==45*16) gpJoueur->setCoffre(4,2);
        if (zone==19 && gpMonde->regionValue(1)==60*16) gpJoueur->setCoffre(4,3);
        if (zone==19 && gpMonde->regionValue(1)==30*16) gpJoueur->setCoffre(4,4);
        if (zone==20 && gpMonde->regionValue(1)==45*16) gpJoueur->setCoffre(5,15);
        if (zone==20 && gpMonde->regionValue(1)==30*16 && gpJoueur->getX()<90*16) 
            gpJoueur->setCoffre(5,18);
        if (zone==20 && gpMonde->regionValue(1)==30*16 && gpJoueur->getX()>90*16) 
            gpJoueur->setCoffre(5,28);
        if (zone==20 && gpMonde->regionValue(1)==15*16 && gpJoueur->getX()<90*16) 
            gpJoueur->setCoffre(5,21);
        if (zone==20 && gpMonde->regionValue(1)==15*16 && gpJoueur->getX()>90*16) 
            gpJoueur->setCoffre(5,25);
        if (zone==20 && gpMonde->regionValue(1)==0) gpJoueur->setCoffre(5,22);
        if (zone==21 && gpMonde->regionValue(0)==20*16 && gpJoueur->getX()<40*16) 
            gpJoueur->setCoffre(6,11);
        if (zone==21 && gpMonde->regionValue(0)==20*16 && gpJoueur->getX()>40*16) 
            gpJoueur->setCoffre(6,12);
        if (zone==21 && gpMonde->regionValue(0)==200*16) gpJoueur->setCoffre(6,16);
        if (zone==22 && gpMonde->regionValue(0)==20*16) gpJoueur->setCoffre(7,4);
        if (zone==22 && gpMonde->regionValue(0)==140*16) gpJoueur->setCoffre(7,9);
        if (zone==23 && gpMonde->regionValue(0)==40*16) gpJoueur->setCoffre(8,11);
        if (zone==23 && gpMonde->regionValue(0)==160*16) gpJoueur->setCoffre(8,14);
        if (zone==25 && gpMonde->regionValue(0)==0) gpJoueur->setCoffre(10,5);
        if (zone==25 && gpMonde->regionValue(1)==90*16) gpJoueur->setCoffre(10,7);
        if (zone==25 && gpMonde->regionValue(0)==20*16 && gpMonde->regionValue(1)==45*16) 
            gpJoueur->setCoffre(10,8);
        if (zone==25 && gpMonde->regionValue(0)==80*16) gpJoueur->setCoffre(10,9);
        if (zone==26 && gpMonde->regionValue(0)==20*16) gpJoueur->setCoffre(11,14);
        if (zone==26 && gpMonde->regionValue(0)==100*16) gpJoueur->setCoffre(11,17);
        if (zone==26 && gpMonde->regionValue(0)==280*16) gpJoueur->setCoffre(11,18);
        if (zone==27 && gpMonde->regionValue(0)==80*16) gpJoueur->setCoffre(12,5);
        if (zone==27 && gpMonde->regionValue(0)==100*16) gpJoueur->setCoffre(12,6);
        if (zone==28) gpJoueur->setCoffre(13,4);
        gpAudio->playSound(20);
        
        int x1,y1;
        x1=(int)((x+7)/16); y1=(int)((y+8)/16)-1;
        switch(val) {
            case 1219 : case 1429 : case 1594 : case 1803 : case 1978 : case 2391 : 
            case 2583 : case 2861 : case 3232 : case 3529 : case 3704 : case 3797 :
            case 3928 :
                x1--; break;
        }
        switch(val) {
            case 1222 : case 1430 : case 1595 : case 1804 : case 1979 : case 2392 : 
            case 2584 : case 2862 : case 3233 : case 3530 : case 3705 : case 3798 :
            case 3929 :
                y1=(int)((y+10+8)/16)+2;; break;
            case 1223 : case 1431 : case 1596 : case 1805 : case 1980 : case 2393 : 
            case 2585 : case 2863 : case 3234 : case 3531 : case 3706 : case 3799 :
            case 3930 :
                x1--; y1=(int)((y+10+8)/16)+2;; break;
        }
        gpMonde->passage(x1,y1,0);
        gpJoueur->setCle(zone-15, 3, gpJoueur->getCle(zone-15, 3)-1);
    }
    //porte vérouillée g d
    if ((val==1231 || val==1237 || val==1437 || val==1442 || val==1811 || val==1816
    || val==2299 || val==2304 || val==2399 || val==2404 || val==2591 || val==2596
    || val==2869 || val==2874 || val==3105 || val==3110 || val==3240 || val==3245
    || val==3537 || val==3542 || val==3805 || val==3810 || val==3936 || val==3941)
    && gpJoueur->getCle(zone-15, 3)>0) {    
        
        if (zone==15 && gpMonde->regionValue(3)==60*16) gpJoueur->setCoffre(0,1);
        if (zone==15 && gpMonde->regionValue(1)==15*16 && gpMonde->regionValue(0)<100*16) 
            gpJoueur->setCoffre(0,3);
        if (zone==15 && gpMonde->regionValue(1)==15*16 && gpMonde->regionValue(0)>100*16) 
            gpJoueur->setCoffre(0,4);
        if (zone==16) gpJoueur->setCoffre(1,5);
        if (zone==18 && gpMonde->regionValue(0)==100*16 && gpMonde->regionValue(1)==15*16) 
            gpJoueur->setCoffre(3,1);
        if (zone==18 && gpMonde->regionValue(0)==100*16 && gpMonde->regionValue(1)==45*16) 
            gpJoueur->setCoffre(3,2);
        if (zone==18 && gpMonde->regionValue(0)==120*16) gpJoueur->setCoffre(3,4);
        if (zone==18 && gpMonde->regionValue(0)==260*16) gpJoueur->setCoffre(3,5);
        if (zone==18 && gpMonde->regionValue(0)==280*16) gpJoueur->setCoffre(3,8);
        if (zone==19 && gpMonde->regionValue(1)==0) gpJoueur->setCoffre(4,5);
        if (zone==19 && gpMonde->regionValue(1)==60*16) gpJoueur->setCoffre(4,6);
        if (zone==20 && gpMonde->regionValue(0)==60*16 && gpMonde->regionValue(1)==15*16) 
            gpJoueur->setCoffre(5,20);
        if (zone==20 && gpMonde->regionValue(0)==60*16 && gpMonde->regionValue(1)==30*16) 
            gpJoueur->setCoffre(5,17);
        if (zone==20 && gpMonde->regionValue(0)==100*16 && gpMonde->regionValue(1)==15*16) 
            gpJoueur->setCoffre(5,24);
        if (zone==20 && gpMonde->regionValue(0)==100*16 && gpMonde->regionValue(1)==30*16) 
            gpJoueur->setCoffre(5,27);
        if (zone==20 && gpMonde->regionValue(0)==80*16 && gpMonde->regionValue(1)==15*16 
            && gpJoueur->getX()<90*16) gpJoueur->setCoffre(5,19);
        if (zone==20 && gpMonde->regionValue(0)==80*16 && gpMonde->regionValue(1)==15*16 
            && gpJoueur->getX()>90*16) gpJoueur->setCoffre(5,23);
        if (zone==20 && gpMonde->regionValue(0)==80*16 && gpMonde->regionValue(1)==30*16 
            && gpJoueur->getX()<90*16) gpJoueur->setCoffre(5,16);
        if (zone==20 && gpMonde->regionValue(0)==80*16 && gpMonde->regionValue(1)==30*16 
        && gpJoueur->getX()>90*16) gpJoueur->setCoffre(5,26);
        if (zone==21 && gpMonde->regionValue(0)==120*16) gpJoueur->setCoffre(6,13);
        if (zone==21 && gpMonde->regionValue(0)==140*16) gpJoueur->setCoffre(6,14);
        if (zone==21 && gpMonde->regionValue(0)==160*16) gpJoueur->setCoffre(6,15);
        if (zone==21 && gpMonde->regionValue(0)==240*16) gpJoueur->setCoffre(6,17);
        if (zone==22 && gpMonde->regionValue(1)==30*16) gpJoueur->setCoffre(7,5);
        if (zone==22 && gpMonde->regionValue(1)==75*16 && gpJoueur->getX()<120*16) 
            gpJoueur->setCoffre(7,6);
        if (zone==22 && gpMonde->regionValue(1)==15*16) gpJoueur->setCoffre(7,7);
        if (zone==22 && gpMonde->regionValue(1)==75*16 && gpJoueur->getX()>120*16) 
            gpJoueur->setCoffre(7,8);
        if (zone==23 && gpMonde->regionValue(1)==60*16) gpJoueur->setCoffre(8,12);
        if (zone==23 && gpMonde->regionValue(1)==30*16) gpJoueur->setCoffre(8,13);
        if (zone==24 && gpMonde->regionValue(0)==20*16) gpJoueur->setCoffre(9,5);
        if (zone==24 && gpMonde->regionValue(0)==0) gpJoueur->setCoffre(9,6);
        if (zone==24 && gpMonde->regionValue(0)==60*16) gpJoueur->setCoffre(9,7);
        if (zone==24 && gpMonde->regionValue(0)==140*16) gpJoueur->setCoffre(9,8);
        if (zone==24 && gpMonde->regionValue(0)==180*16) gpJoueur->setCoffre(9,9);
        if (zone==25) gpJoueur->setCoffre(10,6);
        if (zone==26 && gpJoueur->getX()<70*16) gpJoueur->setCoffre(11,15);
        if (zone==26 && gpJoueur->getX()>70*16) gpJoueur->setCoffre(11,16);
        if (zone==27 && gpMonde->regionValue(0)==20*16) gpJoueur->setCoffre(12,4);
        if (zone==27 && gpMonde->regionValue(0)==120*16) gpJoueur->setCoffre(12,7);
        if (zone==28 && gpMonde->regionValue(1)>60*16) gpJoueur->setCoffre(13,5);
        if (zone==28 && gpMonde->regionValue(1)==60*16) gpJoueur->setCoffre(13,6);
        gpAudio->playSound(20);
        int x1,y1;
        x1=(int)((x+7)/16)-1; y1=(int)((y+8)/16)-1;
        switch(val) {
            case 1237 : case 1442 : case 1816 : case 2304 : case 2404 : 
            case 2596 : case 2874 : case 3110 : case 3245 : case 3542 :
            case 3810 : case 3941 : 
                x1=(int)((x+7+10)/16)+2; break;
        }
        gpMonde->passage(x1,y1,1);
        gpJoueur->setCle(zone-15, 3, gpJoueur->getCle(zone-15, 3)-1);
    }
}

void Jeu::ouvrePorteMaison(int x, int y) {
    int val = gpMonde->motifValue(x,y);
    x=x/16; y=y/16;
    if (val==610 || val==611 || val==705 || val==706 || val==1054 || val==1055) {
        gpAudio->playSound(20);
        switch (val) {
            case 610 :
                gpMonde->setMap(x,y,602);//setValeur(x, y, 602, -1, LIBRE, 0);
                gpMonde->setMap(x+1,y,603);//gpMonde->setValeur(x+16, y, 603, -1, LIBRE, 0);
                break;
            case 611 :
                gpMonde->setMap(x-1,y,602);//gpMonde->setValeur(x-16, y, 602, -1, LIBRE, 0);
                gpMonde->setMap(x,y,603);//gpMonde->setValeur(x, y, 603, -1, LIBRE, 0);
                break;
            case 705 :
                gpMonde->setMap(x,y,711);//gpMonde->setValeur(x, y, 711, -1, LIBRE, 0);
                gpMonde->setMap(x+1,y,712);//gpMonde->setValeur(x+16, y, 712, -1, LIBRE, 0);
                break;
            case 706 :
                gpMonde->setMap(x-1,y,711);//gpMonde->setValeur(x-16, y, 711, -1, LIBRE, 0);
                gpMonde->setMap(x,y,712);//gpMonde->setValeur(x, y, 712, -1, LIBRE, 0);
                break;
            case 1054 :
                gpMonde->setMap(x,y,1066);//gpMonde->setValeur(x, y, 1066, -1, LIBRE, 0);
                gpMonde->setMap(x+1,y,1066);//gpMonde->setValeur(x+16, y, 1066, -1, LIBRE, 0);
                break;
            case 1055 :
                gpMonde->setMap(x-1,y,1066);//gpMonde->setValeur(x-16, y, 1066, -1, LIBRE, 0);
                gpMonde->setMap(x,y,1066);//gpMonde->setValeur(x, y, 1066, -1, LIBRE, 0);
                break;
        }
    }
}


bool Jeu::isDangereux(int x, int y) {
    if (gpJoueur->getInvincible() || !gpJoueur->getVie()) return false;
    
    //barrière magique
    if ((gpMonde->motifAirValue(x+2,y+8-8)>=2001 && gpMonde->motifAirValue(x+2,y+8-8)<=2004) ||
    (gpMonde->motifAirValue(x+10,y+8-8)>=2001 && gpMonde->motifAirValue(x+10,y+8-8)<=2004) ||
    (gpMonde->motifAirValue(x+17,y+8-8)>=2001 && gpMonde->motifAirValue(x+17,y+8-8)<=2004) ||
    (gpMonde->motifAirValue(x+2,y+8)>=2001 && gpMonde->motifAirValue(x+2,y+8)<=2004) ||
    (gpMonde->motifAirValue(x+10,y+8)>=2001 && gpMonde->motifAirValue(x+10,y+8)<=2004) ||
    (gpMonde->motifAirValue(x+17,y+8)>=2001 && gpMonde->motifAirValue(x+17,y+8)<=2004)) {
        
        gpAudio->playSound(33);
        int degats=4;
        if (!gpJoueur->getDefense()) degats=degats*2;
        else degats=degats/gpJoueur->getDefense();
        if (degats < 1) degats = 1;
        gpJoueur->setVie(gpJoueur->getVie()-degats);
        gpJoueur->setTypeAnim(TOUCHE);
        gpJoueur->setInvincible(0);
        for (int i = 0; i < 16; i+=2) {
            gpJoueur->setDirection(N); gpJoueur->moveY(2,2);
        }
        testFin();
        return true;
    }
    
    //motif dangereux
    if (gpMonde->motifValue(x,y)==830 || gpMonde->motifValue(x+15,y)==830
    || gpMonde->motifValue(x,y+15)==830 || gpMonde->motifValue(x+15,y+15)==830
    || gpMonde->motifValue(x,y)==1009 || gpMonde->motifValue(x+15,y)==1009
    || gpMonde->motifValue(x,y+15)==1009 || gpMonde->motifValue(x+15,y+15)==1009) {
    
        gpAudio->playSound(33);
        int degats=2;
        if (!gpJoueur->getDefense()) degats=degats*2;
        else degats=degats/gpJoueur->getDefense();
        if (degats < 1) degats = 1;
        gpJoueur->setVie(gpJoueur->getVie()-degats);
        gpJoueur->setTypeAnim(TOUCHE);
        
        for (int i = 0; i < 16; i+=2) {
            switch (gpJoueur->getDirection()) {
                case S : gpJoueur->setDirection(S); gpJoueur->moveY(-2,2);break;
                case N : gpJoueur->setDirection(N); gpJoueur->moveY(2,2);break;
                case E : gpJoueur->setDirection(E); gpJoueur->moveX(-2,2);break;
                case O : gpJoueur->setDirection(O); gpJoueur->moveX(2,2);break;
            }
        }
        testFin();
        return true;
    }
    
    return false;
}

void Jeu::testInterrupteur(int x, int y) {
    if (gpMonde->motifValue(x,y)==1004) {
        gpMonde->setValeur(x, y, 1005, -1, LIBRE, 0);
        switch (zone) {
            case 15 :
                //boss
                if (gpMonde->regionValue(0)==80*16) {
                    int randomP = (int)((float)rand() / RAND_MAX * (3));
                    switch (randomP) {                        
                        case 0: gpMonde->setValeur(82*16,17*16,31,PLEIN); break;
                        case 1: gpMonde->setValeur(97*16,17*16,31,PLEIN); break;
                        default : gpMonde->setValeur(82*16,27*16,31,PLEIN); break;
                    }
                    gpAudio->playSound(35);
                    break;
                }
                gpAudio->playSound(20);
                //portes
                if (gpMonde->regionValue(0)==0) {gpMonde->passage(9,45,0); gpJoueur->setCoffre(0,5);}
                if (gpMonde->regionValue(0)==20*16 && gpMonde->regionValue(1)==0){ gpMonde->passage(29,15,0); gpJoueur->setCoffre(0,7);}
                if (gpMonde->regionValue(0)==20*16 && gpMonde->regionValue(1)==15*16){ gpMonde->passage(49,45,0); gpJoueur->setCoffre(0,6);}
                break;
            case 16 :
                gpAudio->playSound(20);
                if (gpMonde->regionValue(0)==40*16) {gpMonde->passage(60,51,1); gpJoueur->setCoffre(1,9);gpMonde->setContenu(42,47,51);}
                if (gpMonde->regionValue(0)==80*16) {gpMonde->passage(89,15,0); gpJoueur->setCoffre(1,10);gpMonde->setContenu(89,22,51);}
                if (gpMonde->regionValue(0)==0) {gpMonde->passage(9,15,0); gpJoueur->setCoffre(1,11);}
                if (gpMonde->regionValue(0)==200*16) {gpMonde->passage(204,15,0); gpJoueur->setCoffre(1,12);}
                if (gpMonde->regionValue(0)==220*16 && gpMonde->regionValue(1)==0) {gpMonde->passage(240,2,1); gpJoueur->setCoffre(1,13);}
                if (gpMonde->regionValue(0)==240*16 && gpMonde->regionValue(1)==0) {gpMonde->passage(240,10,1); gpJoueur->setCoffre(1,14);}
                if (gpMonde->regionValue(0)==280*16 && gpMonde->regionValue(1)==0) {gpMonde->passage(280,6,1); gpJoueur->setCoffre(1,15);}
                if (gpMonde->regionValue(0)==280*16 && gpMonde->regionValue(1)==15*16) {gpMonde->passage(289,30,0); gpJoueur->setCoffre(1,16);}
                if (gpMonde->regionValue(0)==280*16 && gpMonde->regionValue(1)==30*16) {gpMonde->passage(280,36,1); gpJoueur->setCoffre(1,17);}
                if (gpMonde->regionValue(0)==220*16 && gpMonde->regionValue(1)==15*16) {gpMonde->passage(269,45,0); gpJoueur->setCoffre(1,18);}
                if (gpMonde->regionValue(0)==220*16 && gpMonde->regionValue(1)==45*16) {gpMonde->passage(229,45,0); gpJoueur->setCoffre(1,19);}
                if (gpMonde->regionValue(0)==240*16 && gpMonde->regionValue(1)==90*16) {gpMonde->passage(240,92,1); gpJoueur->setCoffre(1,20);}
                if (gpMonde->regionValue(0)==180*16 && gpMonde->regionValue(1)==45*16 && x<190*16) {gpMonde->passage(180,66,1); gpJoueur->setCoffre(1,21);}
                if (gpMonde->regionValue(0)==160*16) {gpMonde->setValeur(174*16,22*16,1362,0); gpJoueur->setCoffre(1,22);}
                if (gpMonde->regionValue(0)==180*16 && gpMonde->regionValue(1)==15*16) {gpMonde->setValeur(185*16,22*16,1362,0); gpJoueur->setCoffre(1,23);}
                if (gpMonde->regionValue(0)==180*16 && gpMonde->regionValue(1)==45*16 && x>190*16) {gpMonde->setValeur(194*16,66*16,1362,0); gpJoueur->setCoffre(1,24);}
                break;
            case 17 :
                gpAudio->playSound(20);
                if (gpMonde->regionValue(0)==140*16 && gpMonde->regionValue(1)==60*16) {gpMonde->passage(154,75,0); gpJoueur->setCoffre(2,12);}
                if (gpMonde->regionValue(0)==140*16 && gpMonde->regionValue(1)==15*16) {gpMonde->passage(180,21,1); gpJoueur->setCoffre(2,13);}
                if (gpMonde->regionValue(0)==180*16) {gpMonde->passage(200,81,1); gpJoueur->setCoffre(2,14);}
                if (gpMonde->regionValue(0)==200*16 && gpMonde->regionValue(1)==60*16) {gpMonde->passage(209,75,0); gpJoueur->setCoffre(2,15);}
                if (gpMonde->regionValue(0)==200*16 && gpMonde->regionValue(1)==105*16) {gpMonde->passage(200,111,1); gpJoueur->setCoffre(2,16);}
                break;
            case 18 :
                gpAudio->playSound(20);
                if (gpMonde->regionValue(0)==120*16 && gpMonde->regionValue(1)==0) {gpMonde->passage(160,21,1); gpJoueur->setCoffre(3,12);}
                if (gpMonde->regionValue(0)==140*16 && gpMonde->regionValue(1)==60*16) {gpMonde->passage(189,60,0); gpJoueur->setCoffre(3,13);}
                if (gpMonde->regionValue(0)==160*16 && gpMonde->regionValue(1)==45*16) {gpMonde->passage(169,60,0); gpJoueur->setCoffre(3,14);}
                if (gpMonde->regionValue(0)==140*16 && gpMonde->regionValue(1)==45*16) {gpMonde->passage(149,60,0); gpJoueur->setCoffre(3,15);}
                if (gpMonde->regionValue(0)==200*16 && gpMonde->regionValue(1)==60*16) {gpMonde->passage(200,66,1); gpJoueur->setCoffre(3,16);}
                if (gpMonde->regionValue(0)==240*16 && gpMonde->regionValue(1)==45*16) {gpMonde->passage(260,51,1); gpJoueur->setCoffre(3,17);}
                if (gpMonde->regionValue(0)==220*16 && gpMonde->regionValue(1)==45*16) {gpMonde->passage(240,51,1); gpJoueur->setCoffre(3,18);}
                if (gpMonde->regionValue(0)==220*16 && gpMonde->regionValue(1)==30*16) {gpMonde->passage(240,36,1); gpJoueur->setCoffre(3,19);}
                if (gpMonde->regionValue(0)==280*16 && gpMonde->regionValue(1)==0) {gpMonde->passage(289,30,0); gpJoueur->setCoffre(3,20);}
                if (gpMonde->regionValue(0)==280*16 && gpMonde->regionValue(1)==45*16 && gpJoueur->getX() < 300*16) {gpMonde->passage(280,51,1); gpJoueur->setCoffre(3,21);}
                if (gpMonde->regionValue(0)==280*16 && gpMonde->regionValue(1)==45*16 && gpJoueur->getX() > 300*16) {gpMonde->passage(309,60,0); gpJoueur->setCoffre(3,22);}
                if (gpMonde->regionValue(0)==260*16 && gpMonde->regionValue(1)==0) {gpMonde->setValeur(270*16,8*16,1727,0); gpJoueur->setCoffre(3,23);}
                break;
            case 19 :
                gpAudio->playSound(20);
                if (gpMonde->regionValue(0)==0) {gpMonde->passage(9,45,0); gpJoueur->setCoffre(4,7);}
                if (gpMonde->regionValue(0)==60*16) {gpMonde->passage(80,51,1); gpJoueur->setCoffre(4,8);}
                if (gpMonde->regionValue(0)==120*16) {gpMonde->passage(129,30,0); gpJoueur->setCoffre(4,9);}
                if (gpMonde->regionValue(0)==100*16) {gpMonde->passage(129,60,0); gpJoueur->setCoffre(4,10);}
                if (gpMonde->regionValue(0)==140*16 && gpMonde->regionValue(1)==0) {gpMonde->passage(149,15,0); gpJoueur->setCoffre(4,11);}
                if (gpMonde->regionValue(0)==140*16 && gpMonde->regionValue(1)==60*16) {gpMonde->passage(140,66,1); gpJoueur->setCoffre(4,12);}
                if (gpMonde->regionValue(0)==180*16) {gpMonde->passage(180,36,1); gpJoueur->setCoffre(4,13);}
                break;
            case 21 :
                gpAudio->playSound(20);
                if (gpMonde->regionValue(0)==20*16 && gpMonde->regionValue(1)==15*16) {gpMonde->passage(29,15,0); gpJoueur->setCoffre(6,1);}
                if (gpMonde->regionValue(0)==20*16 && gpMonde->regionValue(1)==45*16) {gpMonde->passage(40,51,1); gpJoueur->setCoffre(6,2);}
                if (gpMonde->regionValue(0)==40*16) {gpMonde->passage(40,6,1); gpJoueur->setCoffre(6,3);}
                if (gpMonde->regionValue(0)==80*16) {gpMonde->passage(89,30,0); gpJoueur->setCoffre(6,4);}
                if (gpMonde->regionValue(0)==120*16) {gpMonde->passage(120,51,1); gpJoueur->setCoffre(6,5);}
                if (gpMonde->regionValue(0)==160*16) {gpMonde->passage(169,15,0); gpJoueur->setCoffre(6,6);}
                if (gpMonde->regionValue(0)==180*16) {gpMonde->passage(189,15,0); gpJoueur->setCoffre(6,7);}
                if (gpMonde->regionValue(0)==200*16 && gpMonde->regionValue(1)==0) {gpMonde->passage(229,30,0); gpJoueur->setCoffre(6,8);}
                if (gpMonde->regionValue(0)==200*16 && gpMonde->regionValue(1)==45*16) {gpMonde->passage(229,45,0); gpJoueur->setCoffre(6,9);}
                if (gpMonde->regionValue(0)==260*16) {gpMonde->passage(280,21,1); gpJoueur->setCoffre(6,10);}
                break;
            case 22 :
                gpAudio->playSound(20);
                if (gpMonde->regionValue(0)==0) {gpMonde->passage(9,75,0); gpJoueur->setCoffre(7,10);}
                if (gpMonde->regionValue(0)==40*16) {gpMonde->passage(40,21,1); gpJoueur->setCoffre(7,11);}
                if (gpMonde->regionValue(0)==160*16) {gpMonde->passage(169,30,0); gpJoueur->setCoffre(7,12);}
                if (gpMonde->regionValue(0)==180*16 && gpMonde->regionValue(1)==75*16) {gpMonde->passage(189,75,0); gpJoueur->setCoffre(7,13);}
                if (gpMonde->regionValue(0)==180*16 && gpMonde->regionValue(1)==30*16) {gpMonde->passage(209,45,0); gpJoueur->setCoffre(7,14);gpMonde->setContenu(217,42,51);}
                break;
            case 23 :
                gpAudio->playSound(20);
                if (gpMonde->regionValue(0)==80*16) {gpMonde->passage(89,30,0); gpJoueur->setCoffre(8,6);}
                if (gpMonde->regionValue(0)==140*16) {gpMonde->passage(140,51,1); gpJoueur->setCoffre(8,7);}
                if (gpMonde->regionValue(0)==160*16) {gpMonde->passage(180,21,1); gpJoueur->setCoffre(8,8);}
                if (gpMonde->regionValue(0)==200*16 && gpJoueur->getX()>210*16) {gpMonde->passage(200,6,1); gpJoueur->setCoffre(8,9);}
                if (gpMonde->regionValue(0)==200*16 && gpJoueur->getX()<210*16) {gpMonde->passage(220,6,1); gpJoueur->setCoffre(8,10);}
                break;
            case 24 :
                gpAudio->playSound(20);
                if (gpMonde->regionValue(0)==40*16) {gpMonde->passage(60,36,1); gpJoueur->setCoffre(9,10);}
                if (gpMonde->regionValue(0)==80*16) {gpMonde->passage(89,60,0); gpJoueur->setCoffre(9,11);}
                if (gpMonde->regionValue(0)==100*16) {gpMonde->passage(109,60,0); gpJoueur->setCoffre(9,12);}
                if (gpMonde->regionValue(0)==120*16 && gpMonde->regionValue(1)==45*16) {gpMonde->passage(120,51,1); gpJoueur->setCoffre(9,13);gpMonde->setContenu(124,47,51);}
                if (gpMonde->regionValue(0)==120*16 && gpMonde->regionValue(1)==30*16) {gpMonde->passage(129,45,0); gpJoueur->setCoffre(9,14);}
                if (gpMonde->regionValue(0)==140*16) {gpMonde->passage(149,15,0); gpJoueur->setCoffre(9,15);}
                if (gpMonde->regionValue(0)==160*16) {gpMonde->passage(160,66,1); gpJoueur->setCoffre(9,16);}
                if (gpMonde->regionValue(0)==180*16) {gpMonde->passage(180,66,1); gpJoueur->setCoffre(9,17);}
                if (gpMonde->regionValue(0)==200*16) {gpMonde->passage(240,6,1); gpJoueur->setCoffre(9,18);}
                break;
            case 25 :
                gpAudio->playSound(20);
                if (gpMonde->regionValue(1)==15*16) {gpMonde->passage(69,30,0); gpJoueur->setCoffre(10,10);}
                if (gpMonde->regionValue(1)==75*16) {gpMonde->passage(69,90,0); gpJoueur->setCoffre(10,11);gpMonde->setContenu(77,85,51);}
                if (gpMonde->regionValue(0)==60*16 && gpMonde->regionValue(1)==90*16) {gpMonde->passage(60,96,1);gpMonde->passage(69,105,0); gpJoueur->setCoffre(10,12);}
                if (gpMonde->regionValue(0)==80*16) {gpMonde->passage(89,90,0); gpJoueur->setCoffre(10,13);}
                if (gpMonde->regionValue(0)==120*16) {gpMonde->passage(129,75,0); gpJoueur->setCoffre(10,14);}
                break;
            case 26 :
                gpAudio->playSound(20);
                if (gpMonde->regionValue(0)==0) {gpMonde->passage(9,60,0); gpJoueur->setCoffre(11,2);}
                if (gpMonde->regionValue(0)==20*16 && gpMonde->regionValue(1)==45*16) {gpMonde->passage(29,60,0); gpJoueur->setCoffre(11,3);}
                if (gpMonde->regionValue(0)==20*16 && gpMonde->regionValue(1)==60*16) {gpMonde->passage(49,90,0); gpJoueur->setCoffre(11,4);}
                if (gpMonde->regionValue(0)==60*16 && gpMonde->regionValue(1)==60*16) {gpMonde->passage(60,66,1); gpJoueur->setCoffre(11,5);}
                if (gpMonde->regionValue(0)==20*16 && gpMonde->regionValue(1)==15*16) {gpMonde->passage(60,21,1); gpJoueur->setCoffre(11,6);}
                if (gpMonde->regionValue(0)==60*16 && gpMonde->regionValue(1)==15*16 && gpJoueur->getX()<69*16) {gpMonde->passage(69,30,0); gpJoueur->setCoffre(11,7);}
                if (gpMonde->regionValue(0)==60*16 && gpMonde->regionValue(1)==15*16 && gpJoueur->getX()>69*16) {gpMonde->passage(69,15,0); gpJoueur->setCoffre(11,8);}
                if (gpMonde->regionValue(0)==80*16 && gpMonde->regionValue(1)==15*16 && gpJoueur->getX()<100*16) {gpMonde->passage(80,21,1); gpJoueur->setCoffre(11,9);}
                if (gpMonde->regionValue(0)==80*16 && gpMonde->regionValue(1)==60*16) {gpMonde->passage(89,60,0); gpJoueur->setCoffre(11,10);}
                if (gpMonde->regionValue(0)==80*16 && gpMonde->regionValue(1)==75*16) {gpMonde->passage(89,90,0); gpJoueur->setCoffre(11,11);}
                if (gpMonde->regionValue(0)==100*16 && gpMonde->regionValue(1)==45*16) {gpMonde->passage(109,60,0); gpJoueur->setCoffre(11,12);}
                if (gpMonde->regionValue(0)==80*16 && gpMonde->regionValue(1)==15*16 && gpJoueur->getX()>100*16) {gpMonde->passage(120,21,1); gpJoueur->setCoffre(11,13);}
                break;
            case 27 :
                gpAudio->playSound(20);
                if (gpMonde->regionValue(0)==20*16 && gpMonde->regionValue(1)==60*16) {gpMonde->passage(29,75,0); gpJoueur->setCoffre(12,8);}
                if (gpMonde->regionValue(0)==20*16 && gpMonde->regionValue(1)==90*16) {gpMonde->passage(29,120,0); gpJoueur->setCoffre(12,9);}
                if (gpMonde->regionValue(0)==40*16 && gpMonde->regionValue(1)==60*16) {gpMonde->passage(40,66,1); gpJoueur->setCoffre(12,10);}
                if (gpMonde->regionValue(0)==40*16 && gpMonde->regionValue(1)==105*16) {gpMonde->passage(40,111,1); gpJoueur->setCoffre(12,11);}
                break;
            case 28 :
                gpAudio->playSound(20);
                if (gpMonde->regionValue(1)==60*16) {gpMonde->passage(60,66,1); gpJoueur->setCoffre(13,1);}
                if (gpMonde->regionValue(1)==135*16) {gpMonde->passage(60,141,1); gpJoueur->setCoffre(13,2);}
                if (gpMonde->regionValue(1)==45*16) {gpMonde->passage(160,51,1); gpJoueur->setCoffre(13,3);}
                break;
        }
    }
}

void Jeu::testInterrupteurEtoile(int x, int y) {
    if (gpMonde->motifValue(x,y)==2951) {
        if (gpMonde->contenuValue(x,y)==70) gpMonde->setContenu(x/16,y/16,71);
        gpAudio->playSound(28);
        switch (zone) {
            case 23 :
                if (gpMonde->regionValue(0)==240*16) {
                    gpMonde->setValeur(89*16,27*16,2951,LIBRE);
                    gpMonde->setValeur(90*16,27*16,2951,LIBRE);
                    gpMonde->setValeur(250*16,3*16,2950,LIBRE);
                    gpMonde->vide(245,4,10,4);gpMonde->vide(102,66,2,3);
                    gpMonde->vide(109,51,2,2);gpMonde->vide(169,66,2,2);
                    gpMonde->vide(186,69,8,4);gpMonde->vide(249,66,2,2);
                    gpMonde->rempli(126,3,8,8);gpMonde->rempli(148,20,4,5);
                    gpMonde->rempli(186,62,8,4);
                } else {
                    gpMonde->setValeur(89*16,27*16,2950,LIBRE);
                    gpMonde->setValeur(90*16,27*16,2950,LIBRE);
                    gpMonde->setValeur(250*16,3*16,2951,LIBRE);
                    gpMonde->rempli(245,4,10,4);gpMonde->rempli(102,66,2,3);
                    gpMonde->rempli(109,51,2,2);gpMonde->rempli(169,66,2,2);
                    gpMonde->rempli(186,69,8,4);gpMonde->rempli(249,66,2,2);
                    gpMonde->vide(126,3,8,8);gpMonde->vide(148,20,4,5);
                    gpMonde->vide(186,62,8,4);
                }
                break;
            case 25 :
                gpMonde->setValeur(31*16,99*16,2950,LIBRE);
                gpMonde->vide(31,108,3,3);
                break;
        }
    }
}

void Jeu::testTeleporteur(int x, int y) {
    if (gpMonde->motifValue(x,y)==3417) {
        gpAudio->playSound(5);
        switch (zone) {
            case 25 : 
                if (gpMonde->regionValue(0)==40*16) {
                    gpMonde->transitFull(25,gpJoueur->getX()+(140*16),gpJoueur->getY()-(75*16));
                }else{
                    gpMonde->transitFull(25,gpJoueur->getX()-(140*16),gpJoueur->getY()+(75*16));
                }
                break;
            case 26 :
                int randomV;
                if (gpJoueur->getCle(11,0) && !gpJoueur->getCle(11,1)) randomV=4;
                else randomV = rand()%4;
                switch (randomV) {
                    case 0 : 
                        gpMonde->transitFull(26,gpJoueur->getX()-(60*16),gpJoueur->getY()-(72*16));
                        break;
                    case 1 : 
                        gpMonde->transitFull(26,gpJoueur->getX()+(60*16),gpJoueur->getY()-(72*16));
                        break;
                    case 2 : 
                        gpMonde->transitFull(26,gpJoueur->getX()-(60*16),gpJoueur->getY()+(18*16));
                        break;
                    case 3 : 
                        gpMonde->transitFull(26,gpJoueur->getX()+(60*16),gpJoueur->getY()+(18*16));
                        break;
                    case 4 : 
                        gpMonde->transitFull(26,gpJoueur->getX(),gpJoueur->getY()-(27*16));
                        break;
                }
                break;
            case 28 :
                gpMonde->transitFull(28,149*16+8,115*16-32+8-8);
                gpAudio->playSpecial(4); //gpStatut->setJauge(true); 
                ecrit(409); break;
        }
        testAnim();
    }
}

bool Jeu::testVillage(int i, int j) {
    if (isVillage(i,j))
    if (((zone == 3 || zone == 7 || zone == 13) && gpJoueur->getOni() ) 
    || (zone == 11 && !gpJoueur->getOni())) {
        if (!getText()) ecrit(129);
        return true;
    }
    return false;
}

bool Jeu::verifZoneJoueur(int x, int y, int w, int h, int nbdir) {
    //ouvre porte
    ouvrePorteMaison(x+8, y);
    
    //portes verrouillées
    ouvrePorteDonjon(x/*+8*/, y/*-2*/);
    
    testInterrupteur(x+8, y+4);
    
    testInterrupteurEtoile(x+8, y+8);
    
    testTeleporteur(x+8,y+8);
    
    if (isDangereux(x,y)) return false;
    
    if (!isNotFull(x,y,w,h)) {
        if ((x-(gpJoueur->getX()+gpJoueur->getZoneBase()->getZone()->x)==0 
        || y-(gpJoueur->getY()+gpJoueur->getZoneBase()->getZone()->y)==0) && nbdir==1) {
        //en fonction de la direction
            if (y-(gpJoueur->getY()+gpJoueur->getZoneBase()->getZone()->y)<0 
            && gpJoueur->getDirection()==N) { 
                if ( isMarchable(x-2,y, 1, 1, 0)
                && isMarchable(x-2,y+h-1, 1, 1, 0)
                && isMarchable(x,y, 1, 1, 0)
                && isMarchable(x+7,y, 1, 1, 0)) gpJoueur->moveX(-2);
                if ( isMarchable(x+8,y, 1, 1, 0)
                && isMarchable(x+w-1,y, 1, 1, 0)
                && isMarchable(x+w+1,y, 1, 1, 0)
                && isMarchable(x+w+1,y+h-1, 1, 1, 0)) gpJoueur->moveX(2);
            }
            if (y-(gpJoueur->getY()+gpJoueur->getZoneBase()->getZone()->y)>0 
            && gpJoueur->getDirection()==S) {
                if ( isMarchable(x-2,y, 1, 1, 0)
                && isMarchable(x-2,y+h-1, 1, 1, 0)
                && isMarchable(x,y+h-1, 1, 1, 0)
                && isMarchable(x+7,y+h-1, 1, 1, 0)) gpJoueur->moveX(-2);
                if ( isMarchable(x+8,y+h-1, 1, 1, 0)
                && isMarchable(x+w-1,y+h-1, 1, 1, 0)
                && isMarchable(x+w+1,y, 1, 1, 0)
                && isMarchable(x+w+1,y+h-1, 1, 1, 0)) gpJoueur->moveX(2);
            }
            if (x-(gpJoueur->getX()+gpJoueur->getZoneBase()->getZone()->x)<0 
            && gpJoueur->getDirection()==O) {
                if ( isMarchable(x,y-2, 1, 1, 0)
                && isMarchable(x+w-1,y-2, 1, 1, 0)
                && isMarchable(x,y, 1, 1, 0)
                && isMarchable(x,y+7, 1, 1, 0)) gpJoueur->moveY(-2);
                if ( isMarchable(x,y+8, 1, 1, 0)
                && isMarchable(x,y+h-1, 1, 1, 0)
                && isMarchable(x,y+h+1, 1, 1, 0)
                && isMarchable(x+w+1,y+h+1, 1, 1, 0)) gpJoueur->moveY(2);
            }
            if (x-(gpJoueur->getX()+gpJoueur->getZoneBase()->getZone()->x)>0 
            && gpJoueur->getDirection()==E) {
                if ( isMarchable(x,y-2, 1, 1, 0)
                && isMarchable(x+w-1,y-2, 1, 1, 0)
                && isMarchable(x+w-1,y, 1, 1, 0)
                && isMarchable(x+w-1,y+7, 1, 1, 0)) gpJoueur->moveY(-2);
                if ( isMarchable(x+w-1,y+8, 1, 1, 0)
                && isMarchable(x+w-1,y+h-1, 1, 1, 0)
                && isMarchable(x,y+h+1, 1, 1, 0)
                && isMarchable(x+w+1,y+h+1, 1, 1, 0)) gpJoueur->moveY(2);
            }
        }
        return false;
    }
    
    //test village
    if (testVillage(x,y-8)) return false;
        
    return true;
}

// renvoit vrai si Link peut y aller
bool Jeu::isNotFull(int x, int y, int w, int h) {
    for (int i = x; i < x + w; i++)
        for (int j = y; j < y + h; j++) {
            if (!isMarchable(i, j, 1, 1, 0)) return false;
        }
    return true;
}

//cas général
bool Jeu::isNotFull(int x, int y, int w, int h, int vol, int nage, int spectre) {
    for (int i = x; i < x + w; i++)
        for (int j = y; j < y + h; j++) {
            if (!isMarchable(i, j, 1, 1, spectre)) return false;
        }
    if (isVide(x,y,w,h) && !vol && !spectre) return false;
    if (isWater(x,y,w,h) && !vol && !nage && !spectre) return false;
    return true;
}

//renvoit vrai si plein d'eau, lol
bool Jeu::isWater(int x, int y, int w, int h) {
    for (int i = x; i < x + w; i++)
        for (int j = y; j < y + h; j++) {
            if (i>0 && j>0 && i<gpMonde->getLargeur() && j<gpMonde->getHauteur()) 
                if (gpMonde->murValue(i,j)!=EAU) return false;
        }
    return true;
}

//renvoit vrai si plein de vide, super lol
bool Jeu::isVide(int x, int y, int w, int h) {
    for (int i = x; i < x + w; i++)
        for (int j = y; j < y + h; j++) {
            if (i>0 && j>0 && i<gpMonde->regionValue(2) && j<gpMonde->regionValue(3)) 
                if (gpMonde->murValue(i,j)!=VIDE) return false;
        }
    return true;
}

//renvoit vrai présence de glace
bool Jeu::isGlace(int x, int y, int w, int h) {
    for (int i = x; i < x + w; i++)
        for (int j = y; j < y + h; j++) {
            if (i>0 && j>0 && i<gpMonde->regionValue(2) && j<gpMonde->regionValue(3)) 
                if (gpMonde->murValue(i,j)==GLACE) return true;
        }
    return false;
}

//renvoit vrai présence d'un village
bool Jeu::isVillage(int x, int y) {
    int x1=0,x2=0,y1=0,y2=0;
    if (zone>=46 && zone<=70) return true; //maisons des villages
    if (zone != 3 && zone != 7 && zone != 11 && zone != 13) return false;
    switch (zone) {
        case 3 : x1=7*16;y1=14*16;x2=78*16;y2=54*16; 
            if (x+17>=x1 && y+22>=y1 && x+2<x2 && y+8<y2) return true;
            x1=16*16;y1=10*16;x2=78*16;y2=30*16; break;
        case 7 : x1=11*16;y1=13*16;x2=74*16;y2=41*16; 
            if (x+17>=x1 && y+22>=y1 && x+2<x2 && y+8<y2) return true;
            x1=32*16;y1=2*16;x2=64*16;y2=14*16; break;
        case 11 : x1=8*16;y1=9*16;x2=72*16;y2=43*16; break;
        case 13 : x1=24*16;y1=21*16;x2=64*16;y2=42*16; break;
    }
    if (x+17>=x1 && y+22>=y1 && x+2<x2 && y+8<y2) return true;
    return false;   
}

//renvoit vrai si ramassable
bool Jeu::isRamassable(int x, int y, int g) {
    int val = gpMonde->motifValue(x,y);
    if ((val==38 || val==39 || val==192) && g==2) return true;
    if ((val==32 || val==35 || val==191) && g>=1) return true;
    if (((val>=30 && val <=37 && val!=32 && val!=35) || val==193 || val==2324) && g>=0) 
        return true;
    return false;
}

bool Jeu::isMarchable(int i, int j, int vol, int nage, int spectre) {
    if (i < gpMonde->regionValue(0) || i>= gpMonde->regionValue(2) 
    || j < gpMonde->regionValue(1) || j>= gpMonde->regionValue(3)) return false;
    if (spectre) return true;
    //vérif caisses
    Caisse* gpCaisse2 = gpCaisse->getSuivant();
    while (gpCaisse2!=NULL) {
        if (abs(i-gpCaisse2->getX()<16) || abs(j-gpCaisse2->getY()<16)) {
            if (i >= gpCaisse2->getX() && i < gpCaisse2->getX()+16
                && j >= gpCaisse2->getY() && j < gpCaisse2->getY()+16
                && gpCaisse2->getVie()) return false;
        }
        gpCaisse2 = gpCaisse2->getSuivant();
    }
    int val = gpMonde->murValue(i, j);
    if (val == LIBRE || val == GLACE) return true;
    if (val == VIDE && vol) return true;
    if (val == EAU && (vol || nage)) return true;
    return false;
}

bool Jeu::isEnnemi(int x, int y, int w, int h, int id, int type) {
    Ennemi* gpEnnemi2 = gpEnnemi->getSuivant();
    SDL_Rect* r;
    int x1;
    int y1;
    while (gpEnnemi2 != NULL) {
        if (gpEnnemi2->getListId() != id && gpEnnemi2->getVie()>0 
        && (gpEnnemi2->getEnnId()==type || type ==0)) {
            r = gpEnnemi2->getZoneBase()->getZone();
            x1 = gpEnnemi2->getX();
            y1 = gpEnnemi2->getY();
            if ((x1 + r->x + r->w > x && x1 + r->x < x+w)
            && (y1 + r->y + r->h > y && y1 + r->y < y+h)) 
                return true;
        }
        gpEnnemi2 = gpEnnemi2->getSuivant();
    }
    return false;
}

bool Jeu::isPnj(int x, int y, int w, int h, int id) {
    Pnj* gpPnj2 = gpPnj->getSuivant();
    SDL_Rect* r;
    int x1;
    int y1;
    while (gpPnj2 != NULL) {
        if (gpPnj2->getListId() != id && gpPnj2->getVie()>0) {
            r = gpPnj2->getZoneBase()->getZone();
            x1 = gpPnj2->getX();
            y1 = gpPnj2->getY();
            if ((x1 + r->x + r->w > x && x1 + r->x < x+w)
            && (y1 + r->y + r->h > y && y1 + r->y < y+h)) 
                return true;
        }
        gpPnj2 = gpPnj2->getSuivant();
    }
    return false;
}

bool Jeu::isLink(int x, int y, int w, int h) {
    SDL_Rect* r = gpJoueur->getZoneBase()->getZone();
    int x1 = gpJoueur->getX();
    int y1 = gpJoueur->getY();
    
    if (gpJoueur->getVie()>0) {
        if ((x1 + r->x + r->w > x && x1 + r->x < x+w)
        && (y1 + r->y + r->h > y && y1 + r->y < y+h)) 
            return true;
    }
    return false;
}

//ramasse objet rammassable (pancarte, pierre, etc.)
void Jeu::ramasse() {
    int i=0;
    int j=0;
    int g = gpJoueur->hasObjet(O_GANTS);
    switch(gpJoueur->getDirection()) {
        case N : i=gpJoueur->getX()+8; j=gpJoueur->getY()+7-2; break;
        case S : i=gpJoueur->getX()+8; j=gpJoueur->getY()+24+2; break;
        case O : i=gpJoueur->getX()-1-2; j=gpJoueur->getY()+16; break;
        case E : i=gpJoueur->getX()+16+2; j=gpJoueur->getY()+16; break;
    }
    if (isRamassable(i,j,g)) {gpJoueur->setTypeAnim(SOULEVE); return;}
    if (g < 2) if (isRamassable(i,j,2)) gpJoueur->setTypeAnim(LOURD);
}

int Jeu::enleve() {
    int i=0;
    int j=0;
    int g = gpJoueur->hasObjet(O_GANTS);
    switch(gpJoueur->getDirection()) {
        case N : i=gpJoueur->getX()+8; j=gpJoueur->getY()+7-2; break;
        case S : i=gpJoueur->getX()+8; j=gpJoueur->getY()+24+2; break;
        case O : i=gpJoueur->getX()-1-2; j=gpJoueur->getY()+16; break;
        case E : i=gpJoueur->getX()+16+2; j=gpJoueur->getY()+16; break;
    }
    if (!isRamassable(i,j,g)) return 0;
    
    //olb only
    if (zone==15) if (gpMonde->regionValue(0)==80*16) gpMonde->setValeur(97*16,27*16,1004,LIBRE);
    
    int val = gpMonde->motifValue(i,j);
    int new_val = val+9;
    int obj=0;
    
    switch(val) {
        case 30 : obj = 1; new_val = 40; break;
        case 31 : obj = 2; new_val = 40; break;
        case 32 : obj = 3; new_val = 41; break;
        case 33 : obj = 4; new_val = 42; break;
        case 34 : obj = 4; new_val = 43; break;
        case 35 : obj = 3; new_val = 44; break;
        case 36 : obj = 4; new_val = 45; break;
        case 37 : obj = 5; new_val = 44; break;
        case 38 : obj = 6; new_val = 41; break;
        case 39 : obj = 6; new_val = 44; break;
        case 191 : obj = 3; new_val = 46; break;
        case 192 : obj = 6; new_val = 46; break;
        case 193 : obj = 4; new_val = 47; break;
        case 2324 : obj = 4; new_val = 2325; break;
    }
    
    //pierres désert
    if (zone == 5) {
        int a=((int)(i/16));
        int b=((int)(j/16));
        switch (a) {
            case 128 : if (b == 20) gpMonde->setUtile(1); break;
            case 127 : if (b == 25 && gpMonde->getUtile() == 1) gpMonde->setUtile(2); break;
            case 126 : if (b == 30 && gpMonde->getUtile() == 2) gpMonde->setUtile(3); break;
            case 129 : if (b == 40 && gpMonde->getUtile() == 3) gpMonde->setUtile(4); break;
            case 125 : if (b == 44 && gpMonde->getUtile() == 4) gpMonde->setUtile(5); break;
            case 115 : if (b == 41 && gpMonde->getUtile() == 5) gpMonde->setUtile(6); break;
            case 110 : if (b == 42 && gpMonde->getUtile() == 6) gpMonde->setUtile(7); break;
            case 105 : if (b == 43 && gpMonde->getUtile() == 7) gpMonde->setUtile(8); break;
            case 109 : if (b == 22 && gpMonde->getUtile() == 8) {
                gpAudio->playSound(9); gpMonde->setValeur(i,j,194,VIDE);
                for (int z = 0; z < 4; z++) {
                    gpMonde->setMurs(a*2-1,b*2-1+z,VIDE);
                    gpMonde->setMurs(a*2+2,b*2-1+z,VIDE);
                    gpMonde->setMurs(a*2-1+z,b*2-1,VIDE);
                    gpMonde->setMurs(a*2-1+z,b*2+2,VIDE);
                }
                gpAudio->playSound(34); return obj;} break;
        }
    }
    
    gpMonde->setValeur(i,j,new_val,0);
    if (isDonjon()) {
        if(gpMonde->contenuValue(i, j)==51) gpMonde->setValeur(i,j,1005,-1,0,51);
        if(gpMonde->contenuValue(i, j)==50) gpMonde->setValeur(i,j,1004,-1,0,50);
        if(gpMonde->contenuValue(i, j)==71) gpMonde->setValeur(i,j,2950,-1,0,71);
        if(gpMonde->contenuValue(i, j)==70) gpMonde->setValeur(i,j,2951,-1,0,70);
    }
    
    gpAudio->playSound(34);
    
    return obj;
}

void Jeu::setMenu(bool b) {
    menu = b;
	if (!menu) gpMenu->menuOut();
    else {stop = true; gpMenu->menuIn();}
}

void Jeu::ecrit(int id, bool anim, bool cadre, int x, int y, int w, int h) {
    setText(true);
    gpTexte->setTexte(id, x, y, w, h, anim, cadre);
}

void Jeu::affiche(SDL_Surface* gpScreen, std::string s, int x, int y) {
    gpTexte->affiche(gpScreen, s, x, y);
}

Ennemi* Jeu::existEnnemi(int type) {
    Ennemi* gpEnnemi2 = gpEnnemi->getSuivant();
    while (gpEnnemi2 != NULL) {
        if (gpEnnemi2->getEnnId() == type) return gpEnnemi2;
        gpEnnemi2 = gpEnnemi2->getSuivant();
    }
    return NULL;
}

void Jeu::setStop(bool b) { stop = b;}

void Jeu::setTransition(bool b) { transition = b;}

void Jeu::setText(bool b) { text = b; stop = text;}

void Jeu::setPhg(int i, int j) {phg[0]=i;phg[1]=j;}

void Jeu::setZone(int i) {zone=i;}

bool Jeu::getStop() { return stop;}
bool Jeu::getTransition() { return transition;}
int Jeu::getZone() { return zone;}
int Jeu::getPhg(int i) { return phg[i];}
bool Jeu::getMenu() { return menu;}
bool Jeu::getText() { return text;}
SDL_Surface* Jeu::getImageObjets() { return imageObjets;}

int Jeu::getVueHorz() {return vueHorz;}
int Jeu::getVueVert() {return vueVert;}
void Jeu::setVueHorz(int i) {vueHorz = i;}
void Jeu::setVueVert(int i) {vueVert = i;}

void Jeu::setNewZone(int nz) { newzone = nz;}
void Jeu::setNewX(int nx) { newx = nx;}
void Jeu::setNewY(int ny) { newy = ny;}

void Jeu::permuteBloc(int val) {
    gpEnnemi->permute(val);
}

void Jeu::testAnim() {
    switch (zone) {
        case 9 :
            if (gpJoueur->getAvancement() == 67 && nbEnnemis() == 0) {
                gpAudio->stopSpecial();
                gpStatut->setJauge(false);
                ecrit(392);
            }
            break;
        case 15 :
            if (gpMonde->regionValue(0)==80*16 && gpMonde->regionValue(1)==15*16 
            && gpJoueur->hasCristal(0)) gpAudio->playSpecial(3);
            if (gpMonde->regionValue(0)==80*16 && gpMonde->regionValue(1)==15*16 
            && gpJoueur->hasCristal(0) && gpMonde->motifValue(89*16,28*16)==1254) {
                gpAudio->playSound(20); gpMonde->passage(89,30,0); break;
            }
            if (gpMonde->regionValue(0)==80*16 && gpMonde->regionValue(1)==15*16 
            && !gpJoueur->hasCristal(0) && gpMonde->motifValue(89*16,28*16)==1198) {
                gpAudio->playSound(20);
                if (!gpJoueur->getCoeur(0)) {gpAudio->playSpecial(1); gpStatut->setJauge(true);}
                else gpAudio->playSpecial(3);
                gpJoueur->moveY(-16);
                gpMonde->setValeur(89*16, 28*16, 1254, -1, LIBRE, 0);
                gpMonde->setValeur(90*16, 28*16, 1255, -1, LIBRE, 0);
                gpMonde->setValeur(89*16, 29*16, 1256, 0, LIBRE, 0);
                gpMonde->setValeur(90*16, 29*16, 1257, 0, LIBRE, 0);
                for (int i = 0; i < 4; i++) gpMonde->setMurs(89*2+i,28*2+1,PLEIN);
                for (int i = 0; i < 4; i++) gpMonde->setMurs(89*2+i,28*2+2,HAUT);
                for (int i = 0; i < 4; i++) gpMonde->setMurs(89*2+i,28*2+3,SEMI_PLEIN);
            }
            if (gpMonde->regionValue(0)==0 && gpMonde->regionValue(1)==15*16 && nbEnnemis()==0 
            && gpMonde->motifValue(9*16,15*16)==1250) {
                gpAudio->playSound(20); gpMonde->passage(9,15,0);}
            if (gpMonde->regionValue(0)==40*16 && gpMonde->regionValue(1)==0 && nbEnnemis()==2 
            && gpMonde->motifValue(40*16,6*16)==1258) {
                gpAudio->playSound(20); gpMonde->passage(40,6,1);}
            if (gpMonde->regionValue(0)==60*16 && gpMonde->regionValue(1)==45*16 
            && nbEnnemis()==0 && gpMonde->motifValue(80*16,51*16)==1258) {
                gpAudio->playSound(20); gpMonde->passage(80,51,1);}
            if (gpMonde->regionValue(0)==80*16 && gpMonde->regionValue(1)==15*16 
            && nbEnnemis()==0 && gpMonde->motifValue(89*16,15*16)==1250) {
                gpAudio->playSound(20); gpMonde->passage(89,15,0);}
            if (gpMonde->regionValue(0)==100*16 && gpMonde->regionValue(1)==15*16 
            && nbEnnemis()==0 && gpMonde->motifValue(109*16,30*16)==1250) {
                gpAudio->playSound(20); gpMonde->passage(109,30,0);}
            if (gpMonde->regionValue(0)==120*16 && gpMonde->regionValue(1)==30*16 
            && nbEnnemis()==8 && gpMonde->motifValue(129*16,30*16)==1250) {
                gpAudio->playSound(20); gpMonde->passage(129,30,0);}
            if (gpMonde->regionValue(0)==140*16 && gpMonde->regionValue(1)==0 
            && nbEnnemis()==0 && gpMonde->motifValue(160*16,6*16)==1258) {
                gpAudio->playSound(20); gpMonde->passage(160,6,1);}
            break;
        case 16 :
            if (gpMonde->regionValue(0)==240*16 && 
            (gpMonde->regionValue(1)==60*16 || gpMonde->regionValue(1)==75*16)
            && gpJoueur->hasObjet(O_SAC_BOMBES)) gpAudio->playSpecial(3);
            if (gpMonde->regionValue(0)==240*16 && gpMonde->regionValue(1)==75*16 
            && gpJoueur->hasObjet(O_SAC_BOMBES) && gpMonde->motifValue(249*16,88*16)==1450) {
                gpAudio->playSound(20); gpMonde->passage(249,90,0); break;}
            if (gpMonde->regionValue(0)==240*16 && gpMonde->regionValue(1)==75*16 
            && !gpJoueur->hasObjet(O_SAC_BOMBES) && gpMonde->motifValue(249*16,88*16)==1408) {
                gpAudio->playSound(20); 
                if (!gpJoueur->getCoeur(1)) {gpAudio->playSpecial(1); gpStatut->setJauge(true);}
                else gpAudio->playSpecial(3);
                gpJoueur->moveY(-16);
                gpMonde->setValeur(249*16, 88*16, 1450, -1, LIBRE, 0);
                gpMonde->setValeur(250*16, 88*16, 1451, -1, LIBRE, 0);
                gpMonde->setValeur(249*16, 89*16, 1452, 0, LIBRE, 0);
                gpMonde->setValeur(250*16, 89*16, 1453, 0, LIBRE, 0);
                for (int i = 0; i < 4; i++) gpMonde->setMurs(249*2+i,88*2+1,PLEIN);
                for (int i = 0; i < 4; i++) gpMonde->setMurs(249*2+i,88*2+2,HAUT);
                for (int i = 0; i < 4; i++) gpMonde->setMurs(249*2+i,88*2+3,SEMI_PLEIN);
            }
            if (gpMonde->regionValue(0)==240*16 && gpMonde->regionValue(1)==60*16 
            && gpJoueur->hasObjet(O_SAC_BOMBES) && gpMonde->motifValue(249*16,73*16)==1450) {
                gpAudio->playSound(20); gpMonde->passage(249,75,0); break;}
            if (gpMonde->regionValue(0)==80*16 && gpMonde->regionValue(1)==30*16 
            && nbEnnemis()==0 && gpMonde->motifValue(89*16,30*16)==1446) {
                gpAudio->playSound(20); gpMonde->passage(89,30,0);}
            if (gpMonde->regionValue(0)==140*16 && gpMonde->regionValue(1)==15*16 
            && nbEnnemis()==0 && gpMonde->motifValue(160*16,21*16)==1454) {
                gpAudio->playSound(20); gpMonde->passage(160,21,1);}
            if (gpMonde->regionValue(0)==140*16 && gpMonde->regionValue(1)==75*16 
            && nbEnnemis()==0 && gpMonde->motifValue(149*16,75*16)==1446) {
                gpAudio->playSound(20); gpMonde->passage(149,75,0);}
            if (gpMonde->regionValue(0)==180*16 && gpMonde->regionValue(1)==30*16 
            && nbEnnemis()==0 && gpMonde->motifValue(189*16,30*16)==1446) {
                gpAudio->playSound(20); gpMonde->passage(189,30,0);}
            if (gpMonde->regionValue(0)==240*16 && gpMonde->regionValue(1)==45*16 
            && nbEnnemis()==0 && gpMonde->motifValue(240*16,51*16)==1454) {
                gpAudio->playSound(20); gpMonde->passage(240,51,1);}
            if (gpMonde->regionValue(0)==240*16 && gpMonde->regionValue(1)==75*16 
            && nbEnnemis()==0 && gpMonde->motifValue(249*16,75*16)==1446) {
                gpAudio->playSound(20); gpMonde->passage(249,75,0);}
            break;
        case 17 :
            if (gpMonde->regionValue(0)==0 && gpMonde->regionValue(1)==30*16 
            && gpJoueur->hasCristal(1)) gpAudio->playSpecial(3);
            if (gpMonde->regionValue(0)==0 && gpMonde->regionValue(1)==30*16 
            && gpJoueur->hasCristal(1) && gpMonde->motifValue(9*16,43*16)==1607) {
                gpAudio->playSound(20); gpMonde->passage(9,45,0); break;}
            if (gpMonde->regionValue(0)==0 && gpMonde->regionValue(1)==30*16 
            && !gpJoueur->hasCristal(1) && gpMonde->motifValue(9*16,43*16)==1571) {
                gpAudio->playSound(20); 
                if (!gpJoueur->getCoeur(2)) {gpAudio->playSpecial(1); gpStatut->setJauge(true);}
                else gpAudio->playSpecial(3);
                gpJoueur->moveY(-16);
                gpMonde->setValeur(9*16, 43*16, 1607, -1, LIBRE, 0);
                gpMonde->setValeur(10*16, 43*16, 1608, -1, LIBRE, 0);
                gpMonde->setValeur(9*16, 44*16, 1609, 0, LIBRE, 0);
                gpMonde->setValeur(10*16, 44*16, 1610, 0, LIBRE, 0);
                for (int i = 0; i < 4; i++) gpMonde->setMurs(9*2+i,43*2+1,PLEIN);
                for (int i = 0; i < 4; i++) gpMonde->setMurs(9*2+i,43*2+2,HAUT);
                for (int i = 0; i < 4; i++) gpMonde->setMurs(9*2+i,43*2+3,SEMI_PLEIN);
            }
            if (gpMonde->regionValue(0)==0 && gpMonde->regionValue(1)==30*16 
            && gpJoueur->getCoeur(2) && gpMonde->motifValue(9*16,30*16)==1603) {
                gpAudio->playSound(20); gpMonde->passage(9,30,0);}
            if (gpMonde->regionValue(0)==140*16 && gpMonde->regionValue(1)==105*16 
            && nbEnnemis()==0 && gpMonde->motifValue(149*16,105*16)==1603) {
                gpAudio->playSound(20); gpMonde->passage(149,105,0);}
            break;
        case 18 :
            if (gpMonde->regionValue(0)==260*16 && gpMonde->regionValue(1)==15*16 
            && gpJoueur->getAvancement()==65) gpAudio->playSpecial(3);
            if (gpMonde->regionValue(0)==260*16 && gpMonde->regionValue(1)==15*16 
            && gpJoueur->getAvancement()==65 && gpMonde->motifValue(269*16,28*16)==1828) {
                gpAudio->playSound(20); gpMonde->passage(269,30,0); break;}
            if (gpMonde->regionValue(0)==260*16 && gpMonde->regionValue(1)==15*16 
            && gpJoueur->getAvancement()<65 && gpMonde->motifValue(269*16,28*16)==1780) {
                gpAudio->playSound(20); 
                if (!gpJoueur->getCoeur(3)) {gpAudio->playSpecial(6); gpStatut->setJauge(true);}
                else gpAudio->playSpecial(3);
                gpJoueur->moveY(-16);
                gpMonde->setValeur(269*16, 28*16, 1828, -1, LIBRE, 0);
                gpMonde->setValeur(270*16, 28*16, 1829, -1, LIBRE, 0);
                gpMonde->setValeur(269*16, 29*16, 1830, 0, LIBRE, 0);
                gpMonde->setValeur(270*16, 29*16, 1831, 0, LIBRE, 0);
                for (int i = 0; i < 4; i++) gpMonde->setMurs(269*2+i,28*2+1,PLEIN);
                for (int i = 0; i < 4; i++) gpMonde->setMurs(269*2+i,28*2+2,HAUT);
                for (int i = 0; i < 4; i++) gpMonde->setMurs(269*2+i,28*2+3,SEMI_PLEIN);
            }
            if (gpMonde->regionValue(0)==260*16 && gpMonde->regionValue(1)==15*16 
            && gpJoueur->getCoeur(3) && gpMonde->motifValue(269*16,15*16)==1824) {
                gpAudio->playSound(20); gpMonde->passage(269,15,0);}
            if (gpMonde->regionValue(0)==100*16 && gpMonde->regionValue(1)==30*16 
            && nbEnnemis()==0 && gpMonde->motifValue(109*16,30*16)==1824) {
                gpAudio->playSound(20); gpMonde->passage(109,30,0);}
            if (gpMonde->regionValue(0)==160*16 && gpMonde->regionValue(1)==15*16 
            && nbEnnemis()==0 && gpMonde->motifValue(169*16,15*16)==1824) {
                gpAudio->playSound(20); gpMonde->passage(169,15,0);}
            if (gpMonde->regionValue(0)==180*16 && gpMonde->regionValue(1)==30*16 
            && nbEnnemis()==0 && gpMonde->motifValue(200*16,36*16)==1832) {
                gpAudio->playSound(20); gpMonde->passage(200,36,1);}
            if (gpMonde->motifValue(185*16,17*16)==1001 && gpMonde->motifValue(189*16,7*16)==1001 
            && gpMonde->motifValue(193*16,18*16)==1001 && gpMonde->motifValue(196*16,25*16)==1001
            && gpMonde->motifValue(200*16,16*16)==1001 && gpMonde->motifValue(203*16,4*16)==1001 
            && gpMonde->motifValue(208*16,16*16)==1001 && gpMonde->motifValue(212*16,23*16)==1001
            && gpMonde->motifValue(213*16,11*16)==1001 && gpMonde->motifValue(189*16,30*16)==1824) {
                gpAudio->playSound(20); gpMonde->passage(189,30,0);}
            if (gpMonde->motifValue(226*16,13*16)==1001 && gpMonde->motifValue(239*16,5*16)==1001 
            && gpMonde->motifValue(239*16,21*16)==1001 && gpMonde->motifValue(253*16,13*16)==1001 
            && gpMonde->motifValue(229*16,30*16)==1824) {
                gpAudio->playSound(20); gpMonde->passage(229,30,0);}
            break;
        case 19 :
            if (gpMonde->regionValue(0)==60*16 && gpMonde->regionValue(1)==15*16 
            && gpJoueur->hasCristal(2)) gpAudio->playSpecial(3);
            if (gpMonde->regionValue(0)==60*16 && gpMonde->regionValue(1)==15*16 
            && gpJoueur->hasCristal(2) && gpMonde->motifValue(89*16,43*16)==1991) {
                gpAudio->playSound(20); gpMonde->passage(89,45,0); break;}
            if (gpMonde->regionValue(0)==60*16 && gpMonde->regionValue(1)==15*16 
            && !gpJoueur->hasCristal(2) && gpMonde->motifValue(89*16,43*16)==1955) {
                gpAudio->playSound(20); gpJoueur->moveY(-16);
                if (!gpJoueur->getCoeur(4)) {gpAudio->playSpecial(1); gpStatut->setJauge(true);}
                else gpAudio->playSpecial(3);
                gpMonde->setValeur(89*16, 43*16, 1991, -1, LIBRE, 0);
                gpMonde->setValeur(90*16, 43*16, 1992, -1, LIBRE, 0);
                gpMonde->setValeur(89*16, 44*16, 1993, 0, LIBRE, 0);
                gpMonde->setValeur(90*16, 44*16, 1994, 0, LIBRE, 0);
                for (int i = 0; i < 4; i++) gpMonde->setMurs(89*2+i,43*2+1,PLEIN);
                for (int i = 0; i < 4; i++) gpMonde->setMurs(89*2+i,43*2+2,HAUT);
                for (int i = 0; i < 4; i++) gpMonde->setMurs(89*2+i,43*2+3,SEMI_PLEIN);
            }
            if (gpMonde->regionValue(0)==60*16 && gpMonde->regionValue(1)==15*16 
            && gpJoueur->getCoeur(4) && gpMonde->motifValue(89*16,15*16)==1987) {
                gpAudio->playSound(20); gpMonde->passage(89,15,0);}
            if (gpMonde->regionValue(0)==40*16 && gpMonde->regionValue(1)==15*16 
            && nbEnnemis()==2 && gpMonde->motifValue(49*16,15*16)==1987) {
                gpAudio->playSound(20); gpMonde->passage(49,15,0);}
            if (gpMonde->motifValue(43*16,49*16)==1001 && gpMonde->motifValue(43*16,55*16)==1001 
            && gpMonde->motifValue(40*16,51*16)==2237) {
                gpAudio->playSound(20); gpMonde->passage(40,51,1);}
            if (gpMonde->motifValue(56*16,49*16)==1001 && gpMonde->motifValue(56*16,55*16)==1001 
            && gpMonde->motifValue(60*16,51*16)==2237) {
                gpAudio->playSound(20); gpMonde->passage(60,51,1);}
            if (gpMonde->motifValue(165*16,35*16)==1001 && gpMonde->motifValue(174*16,35*16)==1001 
            && gpMonde->motifValue(165*16,39*16)==1001 && gpMonde->motifValue(174*16,39*16)==1001 
            && gpMonde->motifValue(165*16,50*16)==1001 && gpMonde->motifValue(174*16,50*16)==1001 
            && gpMonde->motifValue(165*16,54*16)==1001 && gpMonde->motifValue(174*16,54*16)==1001 
            && gpMonde->motifValue(180*16,51*16)==2237) {
                gpAudio->playSound(20); gpMonde->passage(180,51,1);}
            break;
        case 20 :
            if (gpMonde->regionValue(0)==0 && gpMonde->regionValue(1)==30*16 
            && gpJoueur->hasObjet(O_GRAPPIN)) gpAudio->playSpecial(3);
            if (gpMonde->regionValue(0)==0 && gpMonde->regionValue(1)==30*16 
            && gpJoueur->hasObjet(O_GRAPPIN) && gpMonde->motifValue(9*16,43*16)==1450) {
                gpAudio->playSound(20); gpMonde->passage(9,45,0); break;}
            if (gpMonde->regionValue(0)==0 && gpMonde->regionValue(1)==30*16 
            && !gpJoueur->hasObjet(O_GRAPPIN) && gpMonde->motifValue(9*16,43*16)==1408) {
                gpAudio->playSound(20); gpJoueur->moveY(-16);
                if (!gpJoueur->getCoeur(5)) {gpAudio->playSpecial(1); gpStatut->setJauge(true);}
                else gpAudio->playSpecial(3);
                gpMonde->setValeur(9*16, 43*16, 1450, -1, LIBRE, 0);
                gpMonde->setValeur(10*16, 43*16, 1451, -1, LIBRE, 0);
                gpMonde->setValeur(9*16, 44*16, 1452, 0, LIBRE, 0);
                gpMonde->setValeur(10*16, 44*16, 1453, 0, LIBRE, 0);
                for (int i = 0; i < 4; i++) gpMonde->setMurs(9*2+i,43*2+1,PLEIN);
                for (int i = 0; i < 4; i++) gpMonde->setMurs(9*2+i,43*2+2,HAUT);
                for (int i = 0; i < 4; i++) gpMonde->setMurs(9*2+i,43*2+3,SEMI_PLEIN);
            }
            if (gpMonde->regionValue(0)==0 && gpMonde->regionValue(1)==30*16 
            && gpJoueur->getCoeur(5) && gpMonde->motifValue(9*16,30*16)==1446) {
                gpAudio->playSound(20); gpMonde->passage(9,30,0);}
            if (gpMonde->regionValue(0)==80*16 && gpMonde->regionValue(1)==75*16 
            && nbEnnemis()==0 && gpMonde->motifValue(89*16,75*16)==1446) {
                gpAudio->playSound(20); gpMonde->passage(89,75,0);}
            if (gpMonde->regionValue(0)==80*16 && gpMonde->regionValue(1)==45*16 
            && gpJoueur->getCoffre(5,6) && gpMonde->motifValue(80*16,66*16)==1454) {
                gpAudio->playSound(20); gpMonde->passage(80,66,1);}
            if (gpMonde->regionValue(0)==80*16 && gpMonde->regionValue(1)==45*16 
            && gpJoueur->getCoffre(5,5) && gpMonde->motifValue(100*16,66*16)==1454) {
                gpAudio->playSound(20); gpMonde->passage(100,66,1);}
            if (gpMonde->regionValue(0)==80*16 && gpMonde->regionValue(1)==45*16 
            && gpJoueur->getCoffre(5,8) && gpMonde->motifValue(80*16,51*16)==1454) {
                gpAudio->playSound(20); gpMonde->passage(80,51,1);}
            if (gpMonde->regionValue(0)==80*16 && gpMonde->regionValue(1)==45*16 
            && gpJoueur->getCoffre(5,4) && gpMonde->motifValue(100*16,51*16)==1454) {
                gpAudio->playSound(20); gpMonde->passage(100,51,1);}
            break;
        case 21 :
            if (gpMonde->regionValue(0)==240*16 && gpMonde->regionValue(1)==15*16 
            && gpJoueur->hasCristal(3)) gpAudio->playSpecial(3);
            if (gpMonde->regionValue(0)==240*16 && gpMonde->regionValue(1)==15*16 
            && gpJoueur->hasCristal(3) && gpMonde->motifValue(249*16,28*16)==2434) {
                gpAudio->playSound(20); gpMonde->passage(249,30,0); break;}
            if (gpMonde->regionValue(0)==240*16 && gpMonde->regionValue(1)==15*16 
            && !gpJoueur->hasCristal(3) && gpMonde->motifValue(249*16,28*16)==2369
            && nbEnnemis()==1) {
                gpEnnemi->getSuivant()->reveil();
                gpAudio->playSound(20); gpJoueur->moveY(-16);
                if (!gpJoueur->getCoeur(6)) {gpAudio->playSpecial(1); gpStatut->setJauge(true);}
                else gpAudio->playSpecial(3);
                gpMonde->setValeur(249*16, 28*16, 2434, -1, LIBRE, 0);
                gpMonde->setValeur(250*16, 28*16, 2435, -1, LIBRE, 0);
                gpMonde->setValeur(249*16, 29*16, 2436, 0, LIBRE, 0);
                gpMonde->setValeur(250*16, 29*16, 2437, 0, LIBRE, 0);
                for (int i = 0; i < 4; i++) gpMonde->setMurs(249*2+i,28*2+1,PLEIN);
                for (int i = 0; i < 4; i++) gpMonde->setMurs(249*2+i,28*2+2,HAUT);
                for (int i = 0; i < 4; i++) gpMonde->setMurs(249*2+i,28*2+3,SEMI_PLEIN);
            }
            if (gpMonde->regionValue(0)==240*16 && gpMonde->regionValue(1)==15*16 
            && gpJoueur->getCoeur(6) && gpMonde->motifValue(249*16,15*16)==2430) {
                gpAudio->playSound(20); gpMonde->passage(249,15,0);}
            if (gpMonde->regionValue(0)==0 && gpMonde->regionValue(1)==0 
            && nbEnnemis()==0 && gpMonde->motifValue(20*16,6*16)==2438) {
                gpAudio->playSound(20); gpMonde->passage(20,6,1);}
            if (gpMonde->regionValue(0)==80*16 && gpMonde->regionValue(1)==45*16 
            && nbEnnemis()==0 && gpMonde->motifValue(89*16,45*16)==2430) {
                gpAudio->playSound(20); gpMonde->passage(89,45,0);}
            if (gpMonde->regionValue(0)==120*16 && gpMonde->regionValue(1)==0 
            && nbEnnemis()==0 && gpMonde->motifValue(120*16,6*16)==2438) {
                gpAudio->playSound(20); gpMonde->passage(120,6,1);}
            if (gpMonde->regionValue(0)==180*16 && gpMonde->regionValue(1)==45*16 
            && nbEnnemis()==0 && gpMonde->motifValue(189*16,45*16)==2430) {
                gpAudio->playSound(20); gpMonde->passage(189,45,0);}
            break;
        case 22 :
            if (gpMonde->regionValue(0)==80*16 && gpMonde->regionValue(1)==15*16 
            && gpJoueur->hasCristal(4)) gpAudio->playSpecial(3);
            if (gpMonde->regionValue(0)==80*16 && gpMonde->regionValue(1)==15*16 
            && gpJoueur->hasCristal(4) && gpMonde->motifValue(89*16,28*16)==2604) {
                gpAudio->playSound(20); gpMonde->passage(89,30,0); break;
            }
            if (gpMonde->regionValue(0)==80*16 && gpMonde->regionValue(1)==15*16 
            && !gpJoueur->hasCristal(4) && gpMonde->motifValue(89*16,28*16)==2560) {
                gpAudio->playSound(20); gpJoueur->moveY(-16);
                if (!gpJoueur->getCoeur(7)) {gpAudio->playSpecial(1); gpStatut->setJauge(true);}
                else gpAudio->playSpecial(3);
                gpMonde->setValeur(89*16, 28*16, 2604, -1, LIBRE, 0);
                gpMonde->setValeur(90*16, 28*16, 2605, -1, LIBRE, 0);
                gpMonde->setValeur(89*16, 29*16, 2606, 0, LIBRE, 0);
                gpMonde->setValeur(90*16, 29*16, 2607, 0, LIBRE, 0);
                for (int i = 0; i < 4; i++) gpMonde->setMurs(89*2+i,28*2+1,PLEIN);
                for (int i = 0; i < 4; i++) gpMonde->setMurs(89*2+i,28*2+2,HAUT);
                for (int i = 0; i < 4; i++) gpMonde->setMurs(89*2+i,28*2+3,SEMI_PLEIN);
            }
            if (gpMonde->regionValue(0)==80*16 && gpMonde->regionValue(1)==15*16 
            && gpJoueur->getCoeur(7) && gpMonde->motifValue(89*16,15*16)==2600) {
                gpAudio->playSound(20); gpMonde->passage(89,15,0);}
            if (gpMonde->regionValue(0)==0 && gpMonde->regionValue(1)==60*16 
            && nbEnnemis()==0 && gpMonde->motifValue(20*16,66*16)==2608) {
                gpAudio->playSound(20); gpMonde->passage(20,66,1);}
            if (gpMonde->motifValue(62*16,49*16)==1001 && gpMonde->motifValue(75*16,49*16)==1001 
            && gpMonde->motifValue(62*16,55*16)==1001 && gpMonde->motifValue(75*16,55*16)==1001 
            && gpMonde->motifValue(62*16,64*16)==1001 && gpMonde->motifValue(75*16,64*16)==1001 
            && gpMonde->motifValue(62*16,70*16)==1001 && gpMonde->motifValue(75*16,70*16)==1001 
            && gpMonde->motifValue(60*16,51*16)==2608) {
                gpAudio->playSound(20); gpMonde->passage(60,51,1);}
            if (gpMonde->regionValue(0)==220*16 && gpMonde->regionValue(1)==30*16 
            && nbEnnemis()==0 && gpMonde->motifValue(229*16,30*16)==2712) {
                gpAudio->playSound(20); gpMonde->passage(229,30,0);}
            break;
        case 23 :
            if (gpMonde->regionValue(0)==200*16 && gpMonde->regionValue(1)==30*16 
            && gpJoueur->getCoffre(8,19)) gpAudio->playSpecial(3);
            if (gpMonde->regionValue(0)==200*16 && gpMonde->regionValue(1)==30*16 
            && gpJoueur->getCoffre(8,19) && gpMonde->motifValue(209*16,43*16)==2886) {
                gpAudio->playSound(20); gpMonde->passage(209,45,0); break;}
            if (gpMonde->regionValue(0)==200*16 && gpMonde->regionValue(1)==30*16 
            && !gpJoueur->getCoffre(8,19) && gpMonde->motifValue(209*16,43*16)==2838) {
                gpAudio->playSound(20); gpJoueur->moveY(-16);
                if (!gpJoueur->getCoeur(8)) {gpAudio->playSpecial(1); gpStatut->setJauge(true);}
                else gpAudio->playSpecial(3);
                gpMonde->setValeur(209*16, 43*16, 2886, -1, LIBRE, 0);
                gpMonde->setValeur(210*16, 43*16, 2887, -1, LIBRE, 0);
                gpMonde->setValeur(209*16, 44*16, 2888, 0, LIBRE, 0);
                gpMonde->setValeur(210*16, 44*16, 2889, 0, LIBRE, 0);
                for (int i = 0; i < 4; i++) gpMonde->setMurs(209*2+i,43*2+1,PLEIN);
                for (int i = 0; i < 4; i++) gpMonde->setMurs(209*2+i,43*2+2,HAUT);
                for (int i = 0; i < 4; i++) gpMonde->setMurs(209*2+i,43*2+3,SEMI_PLEIN);
            }
            if (gpMonde->regionValue(0)==200*16 && gpMonde->regionValue(1)==30*16 
            && gpJoueur->getCoeur(8) && gpMonde->motifValue(209*16,30*16)==2882) {
                gpAudio->playSound(20); gpMonde->passage(209,30,0);}
            if (gpMonde->regionValue(0)==0 && gpMonde->regionValue(1)==15*16 
            && nbEnnemis()==0 && gpMonde->motifValue(9*16,15*16)==2882) {
                gpAudio->playSound(20); gpMonde->passage(9,15,0);}
            if (gpMonde->motifValue(22*16,32*16)==1001 && gpMonde->motifValue(37*16,42*16)==1001 
            && gpMonde->motifValue(20*16,36*16)==2890) {
                gpAudio->playSound(20); gpMonde->passage(20,36,1);}
            if (gpMonde->motifValue(49*16,40*16)==1001 && gpMonde->motifValue(50*16,40*16)==1001 
            && gpMonde->motifValue(56*16,37*16)==1001 && gpMonde->motifValue(40*16,36*16)==2890) {
                gpAudio->playSound(20); gpMonde->passage(40,36,1);}
            if (gpMonde->motifValue(49*16,22*16)==1001 && gpMonde->motifValue(50*16,22*16)==1001 
            && gpMonde->motifValue(49*16,30*16)==2882) {
                gpAudio->playSound(20); gpMonde->passage(49,30,0);}
            if (gpMonde->regionValue(0)==60*16 && gpMonde->regionValue(1)==0 
            && nbEnnemis()==0 && gpMonde->motifValue(69*16,15*16)==2882) {
                gpAudio->playSound(20); gpMonde->passage(69,15,0);}
            if (gpMonde->regionValue(0)==80*16 && gpMonde->regionValue(1)==60*16 
            && nbEnnemis()==0 && gpMonde->motifValue(80*16,66*16)==2890) {
                gpAudio->playSound(20); gpMonde->passage(80,66,1);}
            if (gpMonde->motifValue(103*16,41*16)==1001 && gpMonde->motifValue(116*16,41*16)==1001 
            && gpMonde->motifValue(109*16,45*16)==2882) {
                gpAudio->playSound(20); gpMonde->passage(109,45,0);}
            if (gpMonde->motifValue(189*16,72*16)==1001 && gpMonde->motifValue(190*16,72*16)==1001 
            && gpMonde->motifValue(180*16,66*16)==2890) {
                gpAudio->playSound(20); gpMonde->passage(180,66,1);}
            if (gpMonde->regionValue(0)==160*16 && gpMonde->regionValue(1)==0 
            && gpMonde->motifValue(169*16,13*16)==2838) {
                gpAudio->playSound(20); gpAudio->playSpecial(4); gpJoueur->moveY(-16);
                gpMonde->setValeur(169*16, 13*16, 2886, -1, LIBRE, 0);
                gpMonde->setValeur(170*16, 13*16, 2887, -1, LIBRE, 0);
                gpMonde->setValeur(169*16, 14*16, 2888, 0, LIBRE, 0);
                gpMonde->setValeur(170*16, 14*16, 2889, 0, LIBRE, 0);
                for (int i = 0; i < 4; i++) gpMonde->setMurs(169*2+i,13*2+1,PLEIN);
                for (int i = 0; i < 4; i++) gpMonde->setMurs(169*2+i,13*2+2,HAUT);
                for (int i = 0; i < 4; i++) gpMonde->setMurs(169*2+i,13*2+3,SEMI_PLEIN);
            }
            break;
        case 24 :
            if (gpMonde->regionValue(0)==40*16 && gpMonde->regionValue(1)==15*16 
            && gpJoueur->getCoffre(9,24)) gpAudio->playSpecial(3);
            if (gpMonde->regionValue(0)==40*16 && gpMonde->regionValue(1)==15*16 
            && gpJoueur->getCoffre(9,24) && gpMonde->motifValue(49*16,28*16)==3061) {
                gpAudio->playSound(20); gpMonde->passage(49,30,0); break;}
            if (gpMonde->regionValue(0)==40*16 && gpMonde->regionValue(1)==15*16 
            && !gpJoueur->getCoffre(9,24) && gpMonde->motifValue(49*16,28*16)==3031) {
                gpAudio->playSound(20); gpJoueur->moveY(-16);
                if (!gpJoueur->getCoeur(9)) {gpAudio->playSpecial(1); gpStatut->setJauge(true);}
                else gpAudio->playSpecial(3);
                gpMonde->setValeur(49*16, 28*16, 3061, -1, LIBRE, 0);
                gpMonde->setValeur(50*16, 28*16, 3062, -1, LIBRE, 0);
                gpMonde->setValeur(49*16, 29*16, 3063, 0, LIBRE, 0);
                gpMonde->setValeur(50*16, 29*16, 3064, 0, LIBRE, 0);
                for (int i = 0; i < 4; i++) gpMonde->setMurs(49*2+i,28*2+1,PLEIN);
                for (int i = 0; i < 4; i++) gpMonde->setMurs(49*2+i,28*2+2,HAUT);
                for (int i = 0; i < 4; i++) gpMonde->setMurs(49*2+i,28*2+3,SEMI_PLEIN);
            }
            if (gpMonde->regionValue(0)==40*16 && gpMonde->regionValue(1)==15*16 
            && gpJoueur->getCoeur(9) && gpMonde->motifValue(49*16,15*16)==3057) {
                gpAudio->playSound(20); gpMonde->passage(49,15,0);}
            if (gpMonde->regionValue(0)==0 && gpMonde->regionValue(1)==30*16 
            && nbEnnemis()==0 && gpMonde->motifValue(9*16,45*16)==3057) {
                gpAudio->playSound(20); gpMonde->passage(9,45,0);}
            if (gpMonde->motifValue(27*16,32*16)==1001 && gpMonde->motifValue(32*16,32*16)==1001 
            && gpMonde->motifValue(29*16,30*16)==3057) {
                gpAudio->playSound(20); gpMonde->passage(29,30,0);}
            if (gpMonde->regionValue(0)==120*16 && gpMonde->regionValue(1)==60*16 
            && nbEnnemis()==0 && gpMonde->motifValue(129*16,60*16)==3057) {
                gpAudio->playSound(20); gpMonde->passage(129,60,0);}
            if (gpMonde->regionValue(0)==240*16 && gpMonde->regionValue(1)==45*16 
            && nbEnnemis()==0 && gpMonde->motifValue(249*16,45*16)==3057) {
                gpAudio->playSound(20); gpMonde->passage(249,45,0);}
            if (gpMonde->regionValue(0)==260*16 && gpMonde->regionValue(1)==15*16 
            && nbEnnemis()==0 && gpMonde->motifValue(269*16,30*16)==3057) {
                gpAudio->playSound(20); gpMonde->passage(269,30,0);}
            if (gpMonde->motifValue(247*16,32*16)==1001 && gpMonde->motifValue(252*16,32*16)==1001 
            && gpMonde->motifValue(249*16,30*16)==3057) {
                gpAudio->playSound(20);
                gpMonde->passage(249,30,0);
                for (int i=0; i<6; i++) 
                    gpMonde->setValeur((240+(i%2))*16, (36+((int)(i/2)))*16, 3065+i, 0, LIBRE, 0);
                for (int i=0; i<6; i++) 
                    gpMonde->setValeur((238+(i%2))*16, (36+((int)(i/2)))*16, 3071+i, 0, LIBRE, 0);
                for (int i = 0; i < 6; i++) for (int j = 0; j < 6; j++) 
                    gpMonde->setMurs(238*2+i+1,36*2+j,PLEIN);
            }
            if (gpMonde->motifValue(247*16,32*16)==1000 && gpMonde->motifValue(252*16,32*16)==1000 
            && gpMonde->motifValue(240*16,36*16)==3065) {
                gpAudio->playSound(20); gpMonde->passage(240,36,1);
                gpMonde->setValeur(249*16, 28*16, 3061, -1, LIBRE, 0);
                gpMonde->setValeur(250*16, 28*16, 3062, -1, LIBRE, 0);
                gpMonde->setValeur(249*16, 29*16, 3063, 0, LIBRE, 0);
                gpMonde->setValeur(250*16, 29*16, 3064, 0, LIBRE, 0);
                gpMonde->setValeur(249*16, 30*16, 3057, 0, LIBRE, 0);
                gpMonde->setValeur(250*16, 30*16, 3058, 0, LIBRE, 0);
                gpMonde->setValeur(249*16, 31*16, 3059, -1, LIBRE, 0);
                gpMonde->setValeur(250*16, 31*16, 3060, -1, LIBRE, 0);
                for (int i = 0; i < 4; i++) gpMonde->setMurs(49*2+i,28*2+1,PLEIN);
                for (int i = 0; i < 4; i++) gpMonde->setMurs(49*2+i,28*2+2,HAUT);
                for (int i = 0; i < 4; i++) gpMonde->setMurs(49*2+i,28*2+3,SEMI_PLEIN);
                for (int i = 0; i < 4; i++) gpMonde->setMurs(49*2+i,28*2+4,SEMI_PLEIN);
                for (int i = 0; i < 4; i++) gpMonde->setMurs(49*2+i,28*2+5,BAS);
                for (int i = 0; i < 4; i++) gpMonde->setMurs(49*2+i,28*2+6,PLEIN);
            }
            break;
        case 25 :
            if (gpMonde->regionValue(0)==180*16 && gpMonde->regionValue(1)==0
            && !gpJoueur->getCoeur(10) && nbEnnemis() && !gpStatut->getJauge()) {
                gpAudio->playSpecial(7); gpStatut->setJauge(true);}
            if (gpMonde->regionValue(0)==40*16 && gpMonde->regionValue(1)==75*16 
            && gpJoueur->getCoffre(10,21)) gpAudio->playSpecial(3);
            if (gpMonde->regionValue(0)==40*16 && gpMonde->regionValue(1)==75*16 
            && gpJoueur->getCoffre(10,21) && gpMonde->motifValue(49*16,88*16)==3257) {
                gpAudio->playSound(20); gpMonde->passage(49,90,0); break;}
            if (gpMonde->regionValue(0)==40*16 && gpMonde->regionValue(1)==75*16 
            && !gpJoueur->getCoffre(10,21) && gpMonde->motifValue(49*16,88*16)==3209) {
                gpAudio->playSound(20); gpJoueur->moveY(-16);
                if (gpJoueur->getCoeur(10)) gpAudio->playSpecial(3);
                gpMonde->setValeur(49*16, 88*16, 3257, -1, LIBRE, 0);
                gpMonde->setValeur(50*16, 88*16, 3258, -1, LIBRE, 0);
                gpMonde->setValeur(49*16, 89*16, 3259, 0, LIBRE, 0);
                gpMonde->setValeur(50*16, 89*16, 3260, 0, LIBRE, 0);
                for (int i = 0; i < 4; i++) gpMonde->setMurs(49*2+i,88*2+1,PLEIN);
                for (int i = 0; i < 4; i++) gpMonde->setMurs(49*2+i,88*2+2,HAUT);
                for (int i = 0; i < 4; i++) gpMonde->setMurs(49*2+i,88*2+3,SEMI_PLEIN);
            }
            if (gpMonde->regionValue(0)==40*16 && gpMonde->regionValue(1)==75*16 
            && gpJoueur->getCoeur(10) && gpMonde->motifValue(49*16,75*16)==3253) {
                gpAudio->playSound(20); gpMonde->passage(49,75,0);}
            if (gpMonde->regionValue(0)==180*16 && gpMonde->regionValue(1)==0 
            && gpJoueur->getCoeur(10) && gpMonde->motifValue(191*16,7*16)==3416)
                gpMonde->setValeur(191*16, 7*16, 3417, LIBRE);
            if (gpMonde->regionValue(0)==20*16 && gpMonde->regionValue(1)==75*16 
            && nbEnnemis()==0 && gpMonde->motifValue(29*16,75*16)==3253) {
                gpAudio->playSound(20); gpMonde->passage(29,75,0);}
            if (gpMonde->regionValue(0)==140*16 && gpMonde->regionValue(1)==60*16 
            && nbEnnemis()==0 && gpMonde->motifValue(140*16,66*16)==3261) {
                gpAudio->playSound(20); gpMonde->passage(140,66,1);}
            if (gpMonde->regionValue(0)==180*16 && gpMonde->regionValue(1)==105*16 && nbEnnemis()==0 
            && gpJoueur->getCoffre(10,20)>3 && gpMonde->motifValue(200*16,111*16)==3261) {
                gpAudio->playSound(20); gpMonde->passage(200,111,1);}
            if (gpMonde->motifValue(163*16,74*16)==1001 && gpMonde->motifValue(163*16,79*16)==1001 
            && gpMonde->motifValue(177*16,74*16)==1001 && gpMonde->motifValue(177*16,79*16)==1001 
            && gpMonde->motifValue(180*16,66*16)==3261) {
                gpAudio->playSound(20); gpMonde->passage(180,66,1);}
            break;
        case 26 :
            if (gpMonde->regionValue(0)==300*16 && gpMonde->regionValue(1)==15*16 
            && !gpJoueur->getCoffre(11,24)) {
                ecrit(190); gpAudio->playSpecial(4);
            }
            if (gpMonde->regionValue(0)==300*16 && gpMonde->regionValue(1)==30*16 
            && gpJoueur->getCoffre(11,24)) gpAudio->playSpecial(3);
            if (gpMonde->regionValue(0)==300*16 && gpMonde->regionValue(1)==30*16 
            && gpJoueur->getCoffre(11,24) && gpMonde->motifValue(309*16,43*16)==3713) {
                gpAudio->playSound(20); gpMonde->passage(309,45,0); break;}
            if (gpMonde->regionValue(0)==300*16 && gpMonde->regionValue(1)==30*16 
            && !gpJoueur->getCoffre(11,24) && gpMonde->motifValue(309*16,43*16)==3681) {
                gpAudio->playSound(20); gpJoueur->moveY(-16);
                if (gpJoueur->hasObjet(O_MASQUE)<2) {ecrit(182); gpAudio->playSpecial(8);}
                else gpAudio->playSpecial(3);
                gpMonde->setValeur(309*16, 43*16, 3713, -1, LIBRE, 0);
                gpMonde->setValeur(310*16, 43*16, 3714, -1, LIBRE, 0);
                gpMonde->setValeur(309*16, 44*16, 3715, 0, LIBRE, 0);
                gpMonde->setValeur(310*16, 44*16, 3716, 0, LIBRE, 0);
                for (int i = 0; i < 4; i++) gpMonde->setMurs(309*2+i,43*2+1,PLEIN);
                for (int i = 0; i < 4; i++) gpMonde->setMurs(309*2+i,43*2+2,HAUT);
                for (int i = 0; i < 4; i++) gpMonde->setMurs(309*2+i,43*2+3,SEMI_PLEIN);
            }
            if (gpMonde->regionValue(0)==300*16 && gpMonde->regionValue(1)==30*16 
            && gpJoueur->getCoeur(11) && gpMonde->motifValue(309*16,30*16)==3709) {
                gpAudio->playSound(20); gpMonde->passage(309,30,0);}
            if (gpMonde->regionValue(0)==40*16 && gpMonde->regionValue(1)==45*16 
            && nbEnnemis()==0 && gpMonde->motifValue(40*16,51*16)==3558) {
                gpAudio->playSound(20); gpMonde->passage(40,51,1);}
            if (gpMonde->regionValue(0)==0 && gpMonde->regionValue(1)==30*16 
            && nbEnnemis()==2 && gpMonde->motifValue(9*16,30*16)==3550) {
                gpAudio->playSound(20); gpMonde->passage(9,30,0);}
            break;
        case 27 :
            if (gpMonde->regionValue(0)==20*16 && gpMonde->regionValue(1)==45*16 
            && gpJoueur->hasObjet(O_ARC)==5) gpAudio->playSpecial(3);
            if (gpMonde->regionValue(0)==20*16 && gpMonde->regionValue(1)==45*16 
            && gpJoueur->hasObjet(O_ARC)==5 && gpMonde->motifValue(29*16,58*16)==3818) {
                gpAudio->playSound(20); gpMonde->passage(29,60,0); break;}
            if (gpMonde->regionValue(0)==20*16 && gpMonde->regionValue(1)==45*16 
            && gpJoueur->hasObjet(O_ARC)!=5 && gpMonde->motifValue(29*16,58*16)==3774) {
                gpAudio->playSound(20); gpJoueur->moveY(-16);
                if (!gpJoueur->getCoeur(12)) {gpAudio->playSpecial(1); gpStatut->setJauge(true);}
                else gpAudio->playSpecial(3);
                gpMonde->setValeur(29*16, 58*16, 3818, -1, LIBRE, 0);
                gpMonde->setValeur(30*16, 58*16, 3819, -1, LIBRE, 0);
                gpMonde->setValeur(29*16, 59*16, 3820, 0, LIBRE, 0);
                gpMonde->setValeur(30*16, 59*16, 3821, 0, LIBRE, 0);
                for (int i = 0; i < 4; i++) gpMonde->setMurs(29*2+i,58*2+1,PLEIN);
                for (int i = 0; i < 4; i++) gpMonde->setMurs(29*2+i,58*2+2,HAUT);
                for (int i = 0; i < 4; i++) gpMonde->setMurs(29*2+i,58*2+3,SEMI_PLEIN);
            }
            if (gpMonde->regionValue(0)==20*16 && gpMonde->regionValue(1)==45*16 
            && gpJoueur->getCoeur(12) && gpMonde->motifValue(29*16,45*16)==3814) {
                gpAudio->playSound(20); gpMonde->passage(29,45,0);}
            if (gpMonde->regionValue(0)==20*16 && gpMonde->regionValue(1)==0 
            && nbEnnemis()==0 && gpMonde->motifValue(20*16,6*16)==3822) {
                gpAudio->playSound(20); gpMonde->passage(20,6,1);}
            break;
        case 28 :
            if (gpMonde->regionValue(0)==40*16 && gpMonde->regionValue(1)==120*16 
            && !gpJoueur->getCoffre(13,10) && gpMonde->motifValue(49*16,133*16)==3905) {
                gpAudio->playSound(20); gpAudio->playSpecial(1);
                gpStatut->setJauge(true); gpJoueur->moveY(-16);
                gpMonde->setValeur(49*16, 133*16, 3949, -1, LIBRE, 0);
                gpMonde->setValeur(50*16, 133*16, 3950, -1, LIBRE, 0);
                gpMonde->setValeur(49*16, 134*16, 3951, 0, LIBRE, 0);
                gpMonde->setValeur(50*16, 134*16, 3952, 0, LIBRE, 0);
                for (int i = 0; i < 4; i++) gpMonde->setMurs(49*2+i,133*2+1,PLEIN);
                for (int i = 0; i < 4; i++) gpMonde->setMurs(49*2+i,133*2+2,HAUT);
                for (int i = 0; i < 4; i++) gpMonde->setMurs(49*2+i,133*2+3,SEMI_PLEIN);
            }
            if (gpMonde->regionValue(0)==40*16 && gpMonde->regionValue(1)==120*16 
            && gpJoueur->getCoffre(13,10) && gpMonde->motifValue(49*16,120*16)==3945) {
                gpAudio->playSound(20); gpMonde->passage(49,120,0); gpMonde->passage(49,135,0);}
            if (gpMonde->regionValue(0)==100*16 && gpMonde->regionValue(1)==30*16 
            && nbEnnemis()==0 && gpMonde->motifValue(120*16,36*16)==3953) {
                gpAudio->playSound(20); gpMonde->passage(120,36,1);}
            if (gpMonde->regionValue(0)==140*16 && gpMonde->regionValue(1)==0 
            && nbEnnemis()==0 && gpMonde->motifValue(149*16,15*16)==3945) {
                gpAudio->playSound(20); gpMonde->passage(149,15,0);}
            if (gpMonde->regionValue(0)==180*16 && gpMonde->regionValue(1)==30*16 
            && nbEnnemis()==0 && gpMonde->motifValue(180*16,36*16)==3953) {
                gpAudio->playSound(20); gpMonde->passage(180,36,1);}
            break;
    }
}

void Jeu::testTapis() {
    gpJoueur->testTapis();
    gpEnnemi->testTapis();
}

void Jeu::teleporte(int tel) {
    switch (tel) {
        case 0 : newx=16*13+8; newy=16*32; newzone=14; break;
        case 1 : newx=16*21+8; newy=16*14; newzone=5; break;    
        case 2 : newx=16*23+8; newy=16*72; newzone=5; break;    
        case 3 : newx=16*41+8; newy=16*38; newzone=13; break;
        case 4 : newx=16*39; newy=16*42; newzone=7; break;
        case 5 : newx=16*39; newy=16*55; newzone=3; break;
        case 6 : newx=16*65; newy=16*44; newzone=11; break;
        case 7 : newx=16*24+8; newy=16*44; newzone=10; break;
        case 8 : newx=16*41+8; newy=16*13; newzone=1; break;
        default : return;
    }
    gpJoueur->setDirection(S);
    finZone();
}

void Jeu::finZone() {
    gpJoueur->setX(newx);
    gpJoueur->setY(newy);
    gpJoueur->saveSemiEmpl();
    
    if (zone != newzone) {
        gpJoueur->saveEmpl();
        videListes();
        zone = newzone;
        if (isDonjon() && !gpJoueur->getCharge()) 
            gpJoueur->setDirection(N);
        if (zone==2 && newx==16*14+8 && newy==16*25) gpJoueur->setDirection(S);
        if (zone==5 && newx==16*42+8 && newy==16*73) gpJoueur->setDirection(S);
        if (zone==5 && newx==16*53+8 && newy==16*72) gpJoueur->setDirection(S);
        if (zone==6 && newx==16*89+8 && newy==16*26) gpJoueur->setDirection(S);
        if (zone==8 && newx==16*26+8 && newy==16*17) gpJoueur->setDirection(S);
        if (zone==14 && newx==16*13+8 && newy==16*51) gpJoueur->setDirection(S);
        if (zone==33 && newy==120) gpJoueur->setDirection(S);
        if (zone==37 && newy==136) gpJoueur->setDirection(S);
        if (zone==38 && newy==136) gpJoueur->setDirection(S);
        if (zone==40) gpJoueur->setDirection(S);
        if (zone==43 && newy==88) gpJoueur->setDirection(S);
        
        if (isDehors() && gpJoueur->getAvancement()<67 && gpJoueur->hasObjet(O_MASQUE)==2) 
            ecrit(192);
        gpMonde->changeZone(zone);
        if (isDonjon()) ecrit(201+zone-15);
        if (gpJoueur->getAvancement() == 63 && isDehors()) ecrit(171);
        if (gpJoueur->getAsile()==1) {if (zone == 53) ecrit(338); else gpJoueur->setAsile(0);}
        if (gpJoueur->getAsile()==2) gpJoueur->setAsile(3);
        if (zone != 55 || gpJoueur->getAvancement()<65)gpAudio->playMusic(zone);
        else gpAudio->playMusic(79);
    } else gpMonde->detRegion(zone);
    
    if (zone==28 && newx==16*149+8 && newy==16*113) gpJoueur->setDirection(N);
    
    if (gpJoueur->getTypeAnim()==EMMENE) gpJoueur->setTypeAnim(MARCHE);
    gpJoueur->visible();
    scrolling();
}

void Jeu::testFin() {
    if (transition) return;
    //si peut ramasser un objet
    testRamasse(gpJoueur->getX(), gpJoueur->getY()+8, 16, 16);
    //si dans l'eau
    if (isWater(gpJoueur->getX(), gpJoueur->getY()+8, 16,16)) {
        gpJoueur->bigPlouf();
    }else {
        gpJoueur->antiPlouf();
        if (gpJoueur->getTypeAnim()==FLOTTE || gpJoueur->getTypeAnim()==NAGE) 
            gpJoueur->setTypeAnim(AUCUNE);
    }
    //si tombe dans le vide
    if (isVide(gpJoueur->getX(), gpJoueur->getY()+8, 16,16)) {
        gpAudio->playSound(12);
        gpJoueur->setTypeAnim(CHUTE);
    }
    switch(zone) {
        case 1 :
            if (gpJoueur->getY()<6) {
                if (gpJoueur->getAvancement() >= 3)
                    gpMonde->transitZone(2, -2, -4, N);
                else {ecrit(174); gpJoueur->setY(gpJoueur->getY()+4); gpJoueur->setDirection(S);}
            }
            if (gpJoueur->getY()>19*16-4 && gpJoueur->getAvancement() < 2) {
                ecrit(226); gpJoueur->setY(gpJoueur->getY()-4); gpJoueur->setDirection(N);
            }
            if (gpJoueur->getX()<6) gpMonde->transitZone(6, -6, 0, O);
            if (((int)(gpJoueur->getX()/16))==69 && ((int)(gpJoueur->getY()/16))==35)
                gpMonde->transitFull(15, 792, 920);
            if (((int)(gpJoueur->getX()/16))==29 && ((int)((gpJoueur->getY()-8)/16))==11)
                gpMonde->transitFull(29, 152, 680);
            if (((int)(gpJoueur->getX()/16))==9 && ((int)((gpJoueur->getY()-8)/16))==6)
                gpMonde->transitFull(30, 152, 200);
            if (((int)(gpJoueur->getX()/16))==53 && ((int)((gpJoueur->getY()-8)/16))==37)
                gpMonde->transitFull(31, 152, 200);
            if (((int)(gpJoueur->getX()/16))==41 && ((int)((gpJoueur->getY()-8)/16))==10)
                gpMonde->transitFull(44, 152, 200);
            break;
        case 2 :
            if (gpJoueur->getY()<6 && gpJoueur->getX()<320*3) 
                gpMonde->transitZone(4, -2, -4, N);
            if (gpJoueur->getY()<6 && gpJoueur->getX()>=320*3) 
                gpMonde->transitZone(3, 2, -4, N);
            if (gpJoueur->getY()>240*3+16*13+8-4 && gpJoueur->getX()<320*3) 
                gpMonde->transitZone(6, -4, 4, S);
            if (gpJoueur->getY()>240*3+16*13+8-4 && gpJoueur->getX()>=320*3) 
                gpMonde->transitZone(1, 2, 4, S);
            if (gpJoueur->getX()<6) 
                gpMonde->transitZone(7, -4, 0, O);
            if (((int)(gpJoueur->getX()/16))==89 && ((int)((gpJoueur->getY()-8)/16))==28)
                gpMonde->transitFull(32, 152, 200);
            if (((int)(gpJoueur->getX()/16))==14 && ((int)((gpJoueur->getY()-8)/16))==28)
                gpMonde->transitFull(33, 152, 200);
            if (((int)(gpJoueur->getX()/16))==14 && ((int)((gpJoueur->getY()-8)/16))==23) {
                gpAudio->playSound(11);
                gpMonde->transitFull(33, 152, 120);}
            if (((int)(gpJoueur->getX()/16))==57 && ((int)((gpJoueur->getY()-8)/16))==36)
                gpMonde->transitFull(45, 152, 200);
            break;
        case 3 :
            if (gpJoueur->getY()<6) 
                gpMonde->transitZone(5, -6, -6, N);
            if (gpJoueur->getY()>240*3+16*13+8-4) 
                gpMonde->transitZone(2, -2, 4, S);
            if (gpJoueur->getX()<6) 
                gpMonde->transitZone(4, -4, 0, O);    
            if (((int)(gpJoueur->getX()/16))==72 && ((int)((gpJoueur->getY()-8)/16))==13)
                gpMonde->transitFull(16, 792, 1640);
            if (((int)(gpJoueur->getX()/16))==3 && ((int)((gpJoueur->getY()-8)/16))==11)
                gpMonde->transitFull(34, 152, 200);
            if (((int)(gpJoueur->getX()/16))==10 && ((int)((gpJoueur->getY()-8)/16))==11)
                gpMonde->transitFull(34, 472, 200);
            if (((int)(gpJoueur->getX()/16))==8 && ((int)((gpJoueur->getY()-8)/16))==3)
                gpMonde->transitFull(34, 792, 200);
            if (((int)(gpJoueur->getX()/16))==66 && ((int)((gpJoueur->getY()-8)/16))==24)
                gpMonde->transitFull(46, 152, 200);
            if (((int)(gpJoueur->getX()/16))==47 && ((int)((gpJoueur->getY()-8)/16))==18)
                gpMonde->transitFull(47, 152, 200);
            if (((int)(gpJoueur->getX()/16))==32 && ((int)((gpJoueur->getY()-8)/16))==20)
                gpMonde->transitFull(48, 152, 200);
            if (((int)(gpJoueur->getX()/16))==38 && ((int)((gpJoueur->getY()-8)/16))==20)
                gpMonde->transitFull(48, 472, 200);
            if (((int)(gpJoueur->getX()/16))==18 && ((int)((gpJoueur->getY()-8)/16))==42)
                gpMonde->transitFull(49, 152, 200);
            if (((int)(gpJoueur->getX()/16))==24 && ((int)((gpJoueur->getY()-8)/16))==42)
                gpMonde->transitFull(49, 472, 200);
            if (((int)(gpJoueur->getX()/16))==45 && ((int)((gpJoueur->getY()-8)/16))==41)
                gpMonde->transitFull(50, 152, 200);
            if (((int)(gpJoueur->getX()/16))==55 && ((int)((gpJoueur->getY()-8)/16))==41)
                gpMonde->transitFull(51, 152, 200);
            if (((int)(gpJoueur->getX()/16))==65 && ((int)((gpJoueur->getY()-8)/16))==41)
                gpMonde->transitFull(52, 152, 200);
            break;
        case 4 :
            if (gpJoueur->getY()>240*3+16*13+8-4) 
                gpMonde->transitZone(2, 2, 4, S);
            if (gpJoueur->getX()>320*3+16*18+8) 
                gpMonde->transitZone(3, 4, 0, E);
            if (gpJoueur->getX()<6) 
                gpMonde->transitZone(8, -2, 0, O);
            if (((int)(gpJoueur->getX()/16))==35 && ((int)((gpJoueur->getY()-8)/16))==18)
                gpMonde->transitFull(17, 152, 920);
            if (((int)(gpJoueur->getX()/16))==35 && ((int)(gpJoueur->getY()/16))==28)
                gpMonde->transitFull(17, 1112, 1880);
            if (((int)(gpJoueur->getX()/16))==32 && ((int)((gpJoueur->getY()-8)/16))==22)
                gpMonde->transitFull(17, 472, 1400);
            if (((int)(gpJoueur->getX()/16))==39 && ((int)((gpJoueur->getY()-8)/16))==22)
                gpMonde->transitFull(17, 1752, 1400);
            break;
        case 5 :
            if (gpJoueur->getY()>240*5+16*13+8-4) 
                gpMonde->transitZone(3, 6, 6, S);
            if ((((int)(gpJoueur->getX()/16))==21 || ((int)(gpJoueur->getX()/16))==22) 
            && ((int)(gpJoueur->getY()/16))==11)
                gpMonde->transitFull(23, 152, 1160);
            if ((((int)(gpJoueur->getX()/16))==21 || ((int)(gpJoueur->getX()/16))==22) 
            && ((int)(gpJoueur->getY()/16))==17)
                gpMonde->transitFull(23, 1752, 1160);
            if (((int)(gpJoueur->getX()/16))==23 && ((int)((gpJoueur->getY()-8)/16))==69)
                gpMonde->transitFull(39, 152, 200);
            if (((int)(gpJoueur->getX()/16))==42 && ((int)((gpJoueur->getY()-8)/16))==71) {
                gpAudio->playSound(11); gpMonde->transitFull(40, 152, 120);}
            if (((int)(gpJoueur->getX()/16))==53 && ((int)((gpJoueur->getY()-8)/16))==70) {
                gpAudio->playSound(11); gpMonde->transitFull(40, 744, 120);}
            if (((int)(gpJoueur->getX()/16))==90 && ((int)((gpJoueur->getY()-8)/16))==11)
                gpMonde->transitFull(41, 152, 440);
            if (((int)(gpJoueur->getX()/16))==176 && ((int)((gpJoueur->getY()-8)/16))==71)
                gpMonde->transitFull(71, 152, 200);
            break;
        case 6 :
            if (gpJoueur->getY()<6 && gpJoueur->getX()<320*4) 
                gpMonde->transitZone(7, 0, -4, N);
            if (gpJoueur->getY()<6 && gpJoueur->getX()>=320*4) 
                gpMonde->transitZone(2, 4, -4, N);
            if (gpJoueur->getX()<6) 
                gpMonde->transitZone(10, -4, 0, O);
            if (gpJoueur->getX()>320*5+16*18+8) 
                gpMonde->transitZone(1, 6, 0, E);
            if (((int)(gpJoueur->getX()/16))==46 && ((int)(gpJoueur->getY()/16))==28)
                gpMonde->transitFull(21, 792, 920);
            if (((int)(gpJoueur->getX()/16))==93 && ((int)((gpJoueur->getY()-8)/16))==30)
                gpMonde->transitFull(37, 152, 200);
            if (((int)(gpJoueur->getX()/16))==89 && ((int)((gpJoueur->getY()-8)/16))==24) {
                gpAudio->playSound(11);
                gpMonde->transitFull(37, 88, 136);}
            break;
        case 7 :
            if (gpJoueur->getY()<6) 
                gpMonde->transitZone(8, 0, -4, N);
            if (gpJoueur->getY()>240*3+16*13+8-4) 
                gpMonde->transitZone(6, 0, 4, S);
            if (gpJoueur->getX()<6) 
                gpMonde->transitZone(9, -4, -4, O);
            if (gpJoueur->getX()>320*3+16*18+8) 
                gpMonde->transitZone(2, 4, 0, E);
            if (((int)(gpJoueur->getX()/16))==19 && ((int)((gpJoueur->getY()-8)/16))==23)
                gpMonde->transitFull(53, 152, 440);
            if (((int)(gpJoueur->getX()/16))==19 && ((int)((gpJoueur->getY()-8)/16))==18)
                gpMonde->transitFull(53, 152, 8);
            if (((int)(gpJoueur->getX()/16))==34 && ((int)((gpJoueur->getY()-8)/16))==20)
                gpMonde->transitFull(54, 152, 200);
            if (((int)(gpJoueur->getX()/16))==38 && ((int)((gpJoueur->getY()-8)/16))==5)
                gpMonde->transitFull(55, 152, 200);
            if (((int)(gpJoueur->getX()/16))==44 && ((int)((gpJoueur->getY()-8)/16))==5)
                gpMonde->transitFull(55, 472, 200);
            if (((int)(gpJoueur->getX()/16))==57 && ((int)((gpJoueur->getY()-8)/16))==5)
                gpMonde->transitFull(56, 152, 200);
            if (((int)(gpJoueur->getX()/16))==64 && ((int)((gpJoueur->getY()-8)/16))==20)
                gpMonde->transitFull(57, 152, 200);
            if (((int)(gpJoueur->getX()/16))==49 && ((int)((gpJoueur->getY()-8)/16))==32)
                gpMonde->transitFull(58, 152, 200);
            if (((int)(gpJoueur->getX()/16))==49 && ((int)((gpJoueur->getY()-8)/16))==20)
                gpMonde->transitFull(59, 152, 200);
            if (((int)(gpJoueur->getX()/16))==64 && ((int)((gpJoueur->getY()-8)/16))==32)
                gpMonde->transitFull(60, 152, 200);
            break;
        case 8 :
            if (gpJoueur->getY()>240*3+16*13+8-4) 
                gpMonde->transitZone(7, 0, 4, S);
            if (gpJoueur->getX()<6) {gpMonde->transitZone(9, -4, 0, O); break;}
            if (gpJoueur->getX()>320*1+16*18+8) 
                gpMonde->transitZone(4, 2, 0, E);
            if ((((int)(gpJoueur->getX()/16))==28 || ((int)(gpJoueur->getX()/16))==29) 
            && ((int)((gpJoueur->getY()-8)/16))==38) gpMonde->transitFull(18, 2712, 1160);
            if (((int)(gpJoueur->getX()/16))==21 && ((int)(gpJoueur->getY()/16))==6)
                gpMonde->transitFull(25, 632, 1880);
            if (((int)(gpJoueur->getX()/16))==21 && ((int)((gpJoueur->getY()-8)/16))==19)
                gpMonde->transitFull(38, 152, 200);
            if (((int)(gpJoueur->getX()/16))==26 && ((int)((gpJoueur->getY()-8)/16))==15) {
                gpAudio->playSound(11);
                gpMonde->transitFull(38, 216, 136);}
            break;
        case 9 :
            if (gpJoueur->getY()<6) {
                if (gpJoueur->getAvancement() == 67) {
                    gpJoueur->moveY(4); gpJoueur->setDirection(S); ecrit(155);
                }
                else gpMonde->transitZone(14, -4, -6, N);}
            if (gpJoueur->getY()>240*7+16*13+8-4) {
                if (gpJoueur->getAvancement() == 67) {
                    gpJoueur->moveY(-4); gpJoueur->setDirection(N); ecrit(155);
                }
                else gpMonde->transitZone(10, 0, 8, S);}
            if (gpJoueur->getX()<6 && gpJoueur->getY()<240*4) {
                if (gpJoueur->getAvancement() == 67) {
                    gpJoueur->moveX(4); gpJoueur->setDirection(E); ecrit(155);
                }
                else gpMonde->transitZone(13, -4, 0, O);}
            if (gpJoueur->getX()<6 && gpJoueur->getY()>=240*4) {
                if (gpJoueur->getAvancement() == 67) {
                    gpJoueur->moveX(4); gpJoueur->setDirection(E); ecrit(155);
                }
                else gpMonde->transitZone(12, -4, 4, O);}
            if (gpJoueur->getX()>320*3+16*18+8 && gpJoueur->getY()<240*4) {
                if (gpJoueur->getAvancement() == 67) {
                    gpJoueur->moveX(-4); gpJoueur->setDirection(O); ecrit(155);
                }
                else gpMonde->transitZone(8, 4, 0, E);}
            if (gpJoueur->getX()>320*3+16*18+8 && gpJoueur->getY()>=240*4) {
                if (gpJoueur->getAvancement() == 67) {
                    gpJoueur->moveX(-4); gpJoueur->setDirection(O); ecrit(155);
                }
                else gpMonde->transitZone(7, 4, 4, E);}
            if (((int)(gpJoueur->getX()/16))==39 && ((int)(gpJoueur->getY()/16))==25) {
                if (gpJoueur->getAvancement() == 67) {
                    gpJoueur->moveY(4); gpJoueur->setDirection(S); ecrit(155);
                }
                else gpMonde->transitFull(22, 1912, 1400);}
            if (((int)(gpJoueur->getX()/16))==39 && ((int)((gpJoueur->getY()-8)/16))==85) {
                if (gpJoueur->getAvancement() == 67) {
                    gpJoueur->moveY(4); gpJoueur->setDirection(S); ecrit(155);
                }
                else gpMonde->transitFull(35, 152, 200);}
            break;
        case 10 :
            if (gpJoueur->getY()<6) 
                gpMonde->transitZone(9, 0, -8, N);
            if (gpJoueur->getX()<6) 
                gpMonde->transitZone(11, -4, 0, O);
            if (gpJoueur->getX()>320*3+16*18+8) 
                gpMonde->transitZone(6, 4, 0, E);
            if (((int)(gpJoueur->getX()/16))==24 && ((int)((gpJoueur->getY()-8)/16))==37)
                gpMonde->transitFull(36, 152, 200);
            if (((int)(gpJoueur->getX()/16))==45 && ((int)(gpJoueur->getY()/16))==50)
                gpMonde->transitFull(61, 152, 200);
            break;
        case 11 :
            if (gpJoueur->getY()<6) 
                gpMonde->transitZone(12, 0, -4, N);
            if (gpJoueur->getX()>320*3+16*18+8) 
                gpMonde->transitZone(10, 4, 0, E);
            if (((int)(gpJoueur->getX()/16))==8 && ((int)((gpJoueur->getY()-8)/16))==48)
                gpMonde->transitFull(20, 1432, 1400);
            if (((int)(gpJoueur->getX()/16))==40 && ((int)(gpJoueur->getY()/16))==16)
                gpMonde->transitFull(62, 152, 200);
            if (((int)(gpJoueur->getX()/16))==51 && ((int)(gpJoueur->getY()/16))==30)
                gpMonde->transitFull(63, 152, 200);
            if (((int)(gpJoueur->getX()/16))==29 && ((int)(gpJoueur->getY()/16))==29)
                gpMonde->transitFull(64, 152, 200);
            if (((int)(gpJoueur->getX()/16))==65 && ((int)(gpJoueur->getY()/16))==20)
                gpMonde->transitFull(65, 152, 200);
            if (((int)(gpJoueur->getX()/16))==13 && ((int)(gpJoueur->getY()/16))==20)
                gpMonde->transitFull(66, 152, 200);
            break;
        case 12 :
            if (gpJoueur->getY()>240*3+16*13+8-4) 
                gpMonde->transitZone(11, 0, 4, S);
            if (gpJoueur->getX()>320*3+16*18+8) 
                gpMonde->transitZone(9, 4, -4, E);
            if (((int)(gpJoueur->getX()/16))==39 && ((int)(gpJoueur->getY()/16))==13) {
                if (!gpJoueur->getEpee()==5 || !gpJoueur->hasObjet(O_MASQUE)
                || !gpJoueur->hasMelodie(2)) ecrit(181);
            }
            if (((int)(gpJoueur->getX()/16))==39 && ((int)(gpJoueur->getY()/16))==12) {
                if (gpJoueur->getEpee()==5 && gpJoueur->hasObjet(O_MASQUE)
                && gpJoueur->hasMelodie(2)) gpMonde->transitFull(26, 1112, 1640);
            }
            break;
        case 13 :
            if (gpJoueur->getY()<6) 
                gpMonde->transitZone(14, 0, -6, N);
            if (gpJoueur->getX()>320*3+16*18+8) 
                gpMonde->transitZone(9, 4, 0, E);
            if (((int)(gpJoueur->getX()/16))==26 && ((int)(gpJoueur->getY()/16))==22)
                gpMonde->transitFull(67, 152, 200);
            if (((int)(gpJoueur->getX()/16))==38 && ((int)(gpJoueur->getY()/16))==27)
                gpMonde->transitFull(68, 152, 200);
            if (((int)(gpJoueur->getX()/16))==55 && ((int)(gpJoueur->getY()/16))==25)
                gpMonde->transitFull(69, 152, 200);
            if (((int)(gpJoueur->getX()/16))==48 && ((int)(gpJoueur->getY()/16))==32)
                gpMonde->transitFull(70, 152, 200);
            break;
        case 14 :
            if (gpJoueur->getY()>240*5+16*13+8-4 && gpJoueur->getX()<320*4) 
                gpMonde->transitZone(13, 0, 6, S);
            if (gpJoueur->getY()>240*5+16*13+8-4 && gpJoueur->getX()>=320*4) 
                gpMonde->transitZone(9, 4, 6, S);
            if (((int)(gpJoueur->getX()/16))==60 && ((int)(gpJoueur->getY()/16))==32)
                gpMonde->transitFull(19, 792, 1160);
            if (((int)(gpJoueur->getX()/16))==20 && ((int)(gpJoueur->getY()/16))==17)
                gpMonde->transitFull(24, 2392, 1160);
            if (((int)(gpJoueur->getX()/16))==136 && ((int)((gpJoueur->getY()-8)/16))==2)
                gpMonde->transitFull(28, 792, 2360);
            if (((int)(gpJoueur->getX()/16))==94 && ((int)((gpJoueur->getY()-8)/16))==33)
                gpMonde->transitFull(42, 152, 200);
            if (((int)(gpJoueur->getX()/16))==16 && ((int)((gpJoueur->getY()-8)/16))==54) {
                if (gpJoueur->getAvancement() == 67) ecrit(391);
                else gpMonde->transitFull(43, 152, 200);
            }
            if (((int)(gpJoueur->getX()/16))==13 && ((int)((gpJoueur->getY()-8)/16))==49) {
                gpAudio->playSound(11);
                gpMonde->transitFull(43, 72, 88);}
            break;
        case 15 :
            if (gpJoueur->getY()>gpMonde->regionValue(3)-28 
            && gpMonde->regionValue(0)==40*16 && gpMonde->regionValue(1)==45*16)
                gpMonde->transitFull(1, 16*69+8, 16*37);
            //escaliers vers h
            if (gpMonde->motifValue(gpJoueur->getX(),gpJoueur->getY()+15)==1270) {
                gpAudio->playSound(10);
                gpMonde->transitFull(15, gpJoueur->getX()-(16*100), gpJoueur->getY()-16);
                break;
            }
            //escalier vers b
            if (gpMonde->motifValue(gpJoueur->getX(),gpJoueur->getY()+16)==1276) {
                gpAudio->playSound(11);
                gpMonde->transitFull(15, gpJoueur->getX()+(16*100), gpJoueur->getY()+16);
                break;
            }
            break;
        case 16 :
            if (gpJoueur->getY()>gpMonde->regionValue(3)-28 && gpMonde->regionValue(0)==40*16
            && gpMonde->regionValue(1)==90*16) {
                if (gpJoueur->getOni()) {
                    ecrit(129); gpJoueur->setY(gpJoueur->getY()-4); gpJoueur->setDirection(N);}
                else gpMonde->transitFull(3, 16*72, 16*15+8);}
            //escaliers vers h
            if (gpMonde->motifValue(gpJoueur->getX(),gpJoueur->getY()+15)==1470) {
                gpAudio->playSound(10);
                gpMonde->transitFull(16, gpJoueur->getX()-(16*100), gpJoueur->getY()-16);
                break;
            }
            //escalier vers b
            if (gpMonde->motifValue(gpJoueur->getX(),gpJoueur->getY()+16)==1476) {
                gpAudio->playSound(11);
                gpMonde->transitFull(16, gpJoueur->getX()+(16*100), gpJoueur->getY()+16);
                break;
            }
            break;
        case 17 :
            if (gpJoueur->getY()>gpMonde->regionValue(3)-28 
            && gpMonde->regionValue(0)==960 && gpMonde->regionValue(1)==105*16)
                gpMonde->transitFull(4, 16*35+8, 16*29+8);
            if (gpJoueur->getY()>gpMonde->regionValue(3)-28 
            && gpMonde->regionValue(0)==0 && gpMonde->regionValue(1)==45*16)
                gpMonde->transitFull(4, 16*35+8, 16*20);
            if (gpJoueur->getY()>gpMonde->regionValue(3)-28 
            && gpMonde->regionValue(0)==320 && gpMonde->regionValue(3)==1440)
                gpMonde->transitFull(4, 16*32, 16*24+8);
            if (gpJoueur->getY()>gpMonde->regionValue(3)-28 
            && gpMonde->regionValue(0)==1600 && gpMonde->regionValue(3)==1440)
                gpMonde->transitFull(4, 16*39, 16*24+8);
            //escaliers vers h
            if (gpMonde->motifValue(gpJoueur->getX(),gpJoueur->getY()+15)==1637) {
                gpAudio->playSound(10);
                int dec = 120*16; if (gpMonde->regionValue(0)<120*16) dec = 60*16;
                gpMonde->transitFull(17, gpJoueur->getX()-dec, gpJoueur->getY()-16);
                break;
            }
            //escalier vers b
            if (gpMonde->motifValue(gpJoueur->getX(),gpJoueur->getY()+16)==1643) {
                gpAudio->playSound(11);
                int dec = 120*16; if (gpMonde->regionValue(0)==0) dec = 60*16;
                gpMonde->transitFull(17, gpJoueur->getX()+dec, gpJoueur->getY()+16);
                break;
            }
            break;
        case 18 :
            if (gpJoueur->getY()>gpMonde->regionValue(3)-28 
            && gpMonde->regionValue(0)==320*7 && gpMonde->regionValue(1)==240*4)
                gpMonde->transitFull(8, 16*28+8, 16*41);
            //escaliers vers h
            if (gpMonde->motifValue(gpJoueur->getX(),gpJoueur->getY()+15)==1864) {
                gpAudio->playSound(10);
                int dec = 100*16;
                if (gpMonde->regionValue(0)<120*16) dec=60*16;
                else if (gpMonde->regionValue(0)<220*16) dec=80*16;
                gpMonde->transitFull(18, gpJoueur->getX()-dec, gpJoueur->getY()-16);
                break;
            }
            //escalier vers b
            if (gpMonde->motifValue(gpJoueur->getX(),gpJoueur->getY()+16)==1870) {
                gpAudio->playSound(11);
                int dec = 100*16;
                if (gpMonde->regionValue(0)<60*16) dec=60*16;
                else if (gpMonde->regionValue(0)<120*16) dec=80*16;
                gpMonde->transitFull(18, gpJoueur->getX()+dec, gpJoueur->getY()+16);
                break;
            }
            break;
        case 19 :
            if (gpJoueur->getY()>gpMonde->regionValue(3)-28 
            && gpMonde->regionValue(0)==320*2 && gpMonde->regionValue(1)==240*4)
                gpMonde->transitFull(14, 16*60, 16*34);
            //escaliers vers h
            if (gpMonde->motifValue(gpJoueur->getX(),gpJoueur->getY()+15)==2229) {
                gpAudio->playSound(10);
                gpMonde->transitFull(19, gpJoueur->getX()-(100*16), gpJoueur->getY()-16);
                break;
            }
            //escalier vers b
            if (gpMonde->motifValue(gpJoueur->getX(),gpJoueur->getY()+16)==2235) {
                gpAudio->playSound(11);
                gpMonde->transitFull(19, gpJoueur->getX()+(100*16), gpJoueur->getY()+16);
                break;
            }
            break;
        case 20 :
            if (gpJoueur->getY()>gpMonde->regionValue(3)-28 
            && gpMonde->regionValue(0)==320*4 && gpMonde->regionValue(1)==240*5)
                gpMonde->transitFull(11, 16*8, 16*50);
            //escaliers vers h
            if (gpMonde->motifValue(gpJoueur->getX(),gpJoueur->getY()+15)==1470) {
                gpAudio->playSound(10);
                int dec = 120*16;
                if (gpMonde->regionValue(0)<160*16) dec=80*16;
                gpMonde->transitFull(20, gpJoueur->getX()-dec, gpJoueur->getY()-16);
                break;
            }
            //escalier vers b
            if (gpMonde->motifValue(gpJoueur->getX(),gpJoueur->getY()+16)==1476) {
                gpAudio->playSound(11);
                int dec = 120*16;
                if (gpMonde->regionValue(0)<20*16) dec=80*16;
                gpMonde->transitFull(20, gpJoueur->getX()+dec, gpJoueur->getY()+16);
                break;
            }
            break;
        case 21 :
            if (gpJoueur->getY()>gpMonde->regionValue(3)-28 
            && gpMonde->regionValue(0)==320*2 && gpMonde->regionValue(1)==240*3)
                gpMonde->transitFull(6, 16*46+8, 16*30);
            //escaliers vers h
            if (gpMonde->motifValue(gpJoueur->getX(),gpJoueur->getY()+15)==2510) {
                gpAudio->playSound(10);
                gpMonde->transitFull(21, gpJoueur->getX()-(100*16), gpJoueur->getY()-16);
                break;
            }
            //escalier vers b
            if (gpMonde->motifValue(gpJoueur->getX(),gpJoueur->getY()+16)==2516) {
                gpAudio->playSound(11);
                gpMonde->transitFull(21, gpJoueur->getX()+(100*16), gpJoueur->getY()+16);
                break;
            }
            break;
        case 22 :
            if (gpJoueur->getY()>gpMonde->regionValue(3)-28 
            && gpMonde->regionValue(0)==320*5 && gpMonde->regionValue(1)==240*5)
                gpMonde->transitFull(9, 16*39+8, 16*27);
            //escaliers vers h
            if (gpMonde->motifValue(gpJoueur->getX(),gpJoueur->getY()+15)==2624
            || gpMonde->motifValue(gpJoueur->getX(),gpJoueur->getY()+15)==2720) {
                gpAudio->playSound(10);
                gpMonde->transitFull(22, gpJoueur->getX()-(80*16), gpJoueur->getY()-16);
                break;
            }
            //escalier vers b
            if (gpMonde->motifValue(gpJoueur->getX(),gpJoueur->getY()+16)==2630) {
                gpAudio->playSound(11);
                gpMonde->transitFull(22, gpJoueur->getX()+(80*16), gpJoueur->getY()+16);
                break;
            }
            break;
        case 23 :
            if (gpJoueur->getY()>gpMonde->regionValue(3)-28 
            && gpMonde->regionValue(0)==0 && gpMonde->regionValue(1)==240*4)
                gpMonde->transitFull(5, 16*21+8, 16*13);
            if (gpJoueur->getY()>gpMonde->regionValue(3)-28 
            && gpMonde->regionValue(0)==320*5 && gpMonde->regionValue(1)==240*4)
                gpMonde->transitFull(5, 16*21+8, 16*19);
            //escaliers vers h
            if (gpMonde->motifValue(gpJoueur->getX(),gpJoueur->getY()+15)==2902) {
                gpAudio->playSound(10);
                int dec = 100*16; if (gpMonde->regionValue(0)<160*16) dec=80*16;
                int dec2 = 0; if (dec == 80*16) dec2=15*16;
                gpMonde->transitFull(23, gpJoueur->getX()-dec, gpJoueur->getY()-16-dec2);
                break;
            }
            //escalier vers b
            if (gpMonde->motifValue(gpJoueur->getX(),gpJoueur->getY()+16)==2908) {
                gpAudio->playSound(11);
                int dec = 100*16; if (gpMonde->regionValue(0)<60*16) dec=80*16;
                int dec2 = 0; if (dec == 80*16) dec2=15*16;
                gpMonde->transitFull(23, gpJoueur->getX()+dec, gpJoueur->getY()+16+dec2);
                break;
            }
            break;
        case 24 :
            if (gpJoueur->getY()>gpMonde->regionValue(3)-28 
            && gpMonde->regionValue(0)==320*7 && gpMonde->regionValue(1)==240*4)
                gpMonde->transitFull(14, 16*20+8, 16*18+8);
            //escaliers vers h
            if (gpMonde->motifValue(gpJoueur->getX(),gpJoueur->getY()+15)==3081) {
                gpAudio->playSound(10);
                gpMonde->transitFull(24, gpJoueur->getX()-(100*16), gpJoueur->getY()-16);
                break;
            }
            //escalier vers b
            if (gpMonde->motifValue(gpJoueur->getX(),gpJoueur->getY()+16)==3087) {
                gpAudio->playSound(11);
                gpMonde->transitFull(24, gpJoueur->getX()+(100*16), gpJoueur->getY()+16);
                break;
            }
            break;
        case 25 :
            if (gpJoueur->getY()>gpMonde->regionValue(3)-28 
            && gpMonde->regionValue(0)==320 && gpMonde->regionValue(1)==240*6)
                gpMonde->transitFull(8, 16*21, 16*8);
            //escaliers vers h
            if (gpMonde->motifValue(gpJoueur->getX(),gpJoueur->getY()+15)==3303) {
                gpAudio->playSound(10);
                int dec = gpJoueur->getX()-(80*16); int dec2 = gpJoueur->getY()-16;
                if (dec < 0) {dec +=160*16; dec2-=60*16;}
                gpMonde->transitFull(25, dec, dec2);
                break;
            }
            //escalier vers b
            if (gpMonde->motifValue(gpJoueur->getX(),gpJoueur->getY()+16)==3309) {
                gpAudio->playSound(11);
                int dec = gpJoueur->getX()+(80*16); int dec2 = gpJoueur->getY()+16;
                if (dec > 160*16 && dec2 < 60*16) {dec -=160*16; dec2+=60*16;}
                gpMonde->transitFull(25, dec, dec2);
                break;
            }
            break;
        case 26 :
            if (gpJoueur->getY()>gpMonde->regionValue(3)-28 
            && gpMonde->regionValue(0)==320*3 && gpMonde->regionValue(1)==240*6)
                gpMonde->transitFull(12, 16*39+8, 16*14+8);
            //escaliers vers h
            if (gpMonde->motifValue(gpJoueur->getX(),gpJoueur->getY()+15)==3608) {
                gpAudio->playSound(10);
                gpMonde->transitFull(26, gpJoueur->getX()-(140*16), gpJoueur->getY()-16);
                break;
            }
            //escalier vers b
            if (gpMonde->motifValue(gpJoueur->getX(),gpJoueur->getY()+16)==3614) {
                gpAudio->playSound(11);
                gpMonde->transitFull(26, gpJoueur->getX()+(140*16), gpJoueur->getY()+16);
                break;
            }
            break;
        case 27 :
            if (gpJoueur->getY()>gpMonde->regionValue(3)-28 
            && gpMonde->regionValue(0)==320*1 && gpMonde->regionValue(1)==240*8)
                gpMonde->transitFull(45, 16*29+8, 8);
            break;
        case 28 :
            if (gpJoueur->getY()>gpMonde->regionValue(3)-28 
            && gpMonde->regionValue(0)==320*2 && gpMonde->regionValue(1)==240*9)
                gpMonde->transitFull(14, 16*136+8, 16*4);
            //escaliers vers h
            if (gpMonde->motifValue(gpJoueur->getX(),gpJoueur->getY()+15)==2624) {
                gpAudio->playSound(10);
                int newx = gpJoueur->getX()-(100*16);
                int newy = gpJoueur->getY()-16;
                if (newx<0) {newx+=16*200; newy-=16*75;}
                gpMonde->transitFull(28, newx, newy);
                break;
            }
            //escalier vers b
            if (gpMonde->motifValue(gpJoueur->getX(),gpJoueur->getY()+16)==2630) {
                gpAudio->playSound(11);
                int newx = gpJoueur->getX()+(100*16);
                int newy = gpJoueur->getY()+16;
                if (newx>200*16) {newx-=16*200; newy+=16*75;}
                gpMonde->transitFull(28, newx, newy);
                break;
            }
            break;
        case 29 :
            if (gpJoueur->getY()>gpMonde->regionValue(3)-28 && gpMonde->regionValue(1)==240*2)
                gpMonde->transitFull(1, 16*29+8, 16*13);
            break;
        case 30 :
            if (gpJoueur->getY()>gpMonde->regionValue(3)-28)
                gpMonde->transitFull(1, 16*9+8, 16*8);
            break;
        case 31 :
            if (gpJoueur->getY()>gpMonde->regionValue(3)-28)
                gpMonde->transitFull(1, 16*53+8, 16*39);
            break;
        case 32 :
            if (gpJoueur->getY()>gpMonde->regionValue(3)-28)
                gpMonde->transitFull(2, 16*89+8, 16*30);
            break;
        case 33 :
            if (gpJoueur->getY()>gpMonde->regionValue(3)-28)
                gpMonde->transitFull(2, 16*14+8, 16*30);
            if (gpMonde->motifValue(gpJoueur->getX(),gpJoueur->getY()+15)==1470) {
                gpAudio->playSound(10);
                gpMonde->transitFull(2, 16*14+8, 16*25);
                break;
            }
            break;
        case 34 :
            if (gpJoueur->getY()>gpMonde->regionValue(3)-28 && gpMonde->regionValue(0)==20*16) {
                if (gpJoueur->getOni()) {
                    ecrit(129); gpJoueur->setY(gpJoueur->getY()-4); gpJoueur->setDirection(N);}
                else gpMonde->transitFull(3, 16*10+8, 16*13);}
            if (gpJoueur->getY()>gpMonde->regionValue(3)-28 && gpMonde->regionValue(0)==0)
                gpMonde->transitFull(3, 16*3+8, 16*13);
            if (gpJoueur->getY()>gpMonde->regionValue(3)-28 && gpMonde->regionValue(0)==40*16)
                gpMonde->transitFull(3, 16*8+8, 16*5);
            //escaliers vers h
            if (gpMonde->motifValue(gpJoueur->getX(),gpJoueur->getY()+15)==1470) {
                gpAudio->playSound(10);
                gpMonde->transitFull(34, gpJoueur->getX()+(16*20), gpJoueur->getY()-16);
                break;
            }
            //escalier vers b
            if (gpMonde->motifValue(gpJoueur->getX(),gpJoueur->getY()+16)==1476) {
                gpAudio->playSound(11);
                gpMonde->transitFull(34, gpJoueur->getX()-(16*20), gpJoueur->getY()+16);
                break;
            }
            break;
        case 35 :
            if (gpJoueur->getY()>gpMonde->regionValue(3)-28)
                gpMonde->transitFull(9, 16*39+8, 16*87+8);
            break;
        case 36 :
            if (gpJoueur->getY()>gpMonde->regionValue(3)-28)
                gpMonde->transitFull(10, 16*24+8, 16*39);
            break;
        case 37 :
            if (gpJoueur->getY()>gpMonde->regionValue(3)-28)
                gpMonde->transitFull(6, 16*93+8, 16*32);
            //escaliers vers h
            if (gpMonde->motifValue(gpJoueur->getX(),gpJoueur->getY()+15)==1470) {
                gpAudio->playSound(10);
                gpMonde->transitFull(6, 16*89+8, 16*26);
                break;
            }
            break;
        case 38 :
            if (gpJoueur->getY()>gpMonde->regionValue(3)-28)
                gpMonde->transitFull(8, 16*21+8, 16*21);
            //escaliers vers h
            if (gpMonde->motifValue(gpJoueur->getX(),gpJoueur->getY()+15)==1470) {
                gpAudio->playSound(10);
                gpMonde->transitFull(8, 16*26+8, 16*17);
                break;
            }
            break;
        case 39 :
            if (gpJoueur->getY()>gpMonde->regionValue(3)-28)
                gpMonde->transitFull(5, 16*23+8, 16*71);
            break;
        case 40 :
            //escaliers vers h
            if (gpMonde->motifValue(gpJoueur->getX(),gpJoueur->getY()+15)==1470) {
                gpAudio->playSound(10);
                if (gpMonde->regionValue(0)==0) 
                    gpMonde->transitFull(5, 16*42+8, 16*73);
                if (gpMonde->regionValue(0)==20*16) 
                    gpMonde->transitFull(40, gpJoueur->getX()+(16*20), gpJoueur->getY()-16);
                if (gpMonde->regionValue(0)==40*16) 
                    gpMonde->transitFull(5, 16*53+8, 16*72);
                break;
            }
            //escalier vers b
            if (gpMonde->motifValue(gpJoueur->getX(),gpJoueur->getY()+16)==1476) {
                gpAudio->playSound(11);
                gpMonde->transitFull(40, gpJoueur->getX()-(16*20), gpJoueur->getY()+16);
                break;
            }
            break;
        case 41 :
            if (gpJoueur->getY()>gpMonde->regionValue(3)-28 && gpMonde->regionValue(1)==15*16)
                gpMonde->transitFull(5, 16*90+8, 16*13);
            //escaliers vers h
            if (gpMonde->motifValue(gpJoueur->getX(),gpJoueur->getY()+15)==1470) {
                gpAudio->playSound(10);
                gpMonde->transitFull(41, gpJoueur->getX()-(16*20), gpJoueur->getY()-16);
                break;
            }
            //escalier vers b
            if (gpMonde->motifValue(gpJoueur->getX(),gpJoueur->getY()+16)==1476) {
                gpAudio->playSound(11);
                gpMonde->transitFull(41, gpJoueur->getX()+(16*20), gpJoueur->getY()+16);
                break;
            }
            break;
        case 42 :
            if (gpJoueur->getY()>gpMonde->regionValue(3)-28)
                gpMonde->transitFull(14, 16*94+8, 16*35);
            break;
        case 43 :
            if (gpJoueur->getY()>gpMonde->regionValue(3)-28)
                gpMonde->transitFull(14, 16*16+8, 16*56);
            //escaliers vers h
            if (gpMonde->motifValue(gpJoueur->getX(),gpJoueur->getY()+15)==1470) {
                gpAudio->playSound(10);
                gpMonde->transitFull(14, 16*13+8, 16*51);
                break;
            }
            break;
        case 44 :
            if (gpJoueur->getY()>gpMonde->regionValue(3)-28)
                gpMonde->transitFull(1, 16*41+8, 16*12+8);
            break;
        case 45 :
            if (gpJoueur->getY()>gpMonde->regionValue(3)-28)
                gpMonde->transitFull(2, 16*57+8, 16*38+8);
            //escaliers vers h
            if (gpMonde->motifValue(gpJoueur->getX(),gpJoueur->getY()+15)==1470) {
                gpAudio->playSound(10);
                gpMonde->transitFull(45, gpJoueur->getX()-(16*20), gpJoueur->getY()-16);
                break;
            }
            //escalier vers b
            if (gpMonde->motifValue(gpJoueur->getX(),gpJoueur->getY()+16)==1476) {
                gpAudio->playSound(11);
                gpMonde->transitFull(45, gpJoueur->getX()+(16*20), gpJoueur->getY()+16);
                break;
            }
            if (gpJoueur->getY()<6)
                gpMonde->transitFull(27, 16*29+8, 16*132+8);
            break;
        case 46 :
            if (gpJoueur->getY()>gpMonde->regionValue(3)-28)
                gpMonde->transitFull(3, 16*66+8, 16*26+8);
            break;
        case 47 :
            if (gpJoueur->getY()>gpMonde->regionValue(3)-28)
                gpMonde->transitFull(3, 16*47+8, 16*20+8);
            break;
        case 48 :
            if (gpJoueur->getY()>gpMonde->regionValue(3)-28 && gpJoueur->getX()<20*16)
                gpMonde->transitFull(3, 16*32+8, 16*22+8);
            if (gpJoueur->getY()>gpMonde->regionValue(3)-28 && gpJoueur->getX()>=20*16)
                gpMonde->transitFull(3, 16*38+8, 16*22+8);
            break;
        case 49 :
            if (gpJoueur->getY()>gpMonde->regionValue(3)-28 && gpJoueur->getX()<20*16)
                gpMonde->transitFull(3, 16*18+8, 16*44+8);
            if (gpJoueur->getY()>gpMonde->regionValue(3)-28 && gpJoueur->getX()>=20*16)
                gpMonde->transitFull(3, 16*24+8, 16*44+8);
            break;
        case 50 :
            if (gpJoueur->getY()>gpMonde->regionValue(3)-28)
                gpMonde->transitFull(3, 16*45+8, 16*43+8);
            break;
        case 51 :
            if (gpJoueur->getY()>gpMonde->regionValue(3)-28)
                gpMonde->transitFull(3, 16*55+8, 16*43+8);
            break;
        case 52 :
            if (gpJoueur->getY()>gpMonde->regionValue(3)-28)
                gpMonde->transitFull(3, 16*65+8, 16*43+8);
            break;
        case 53 : 
            if (gpJoueur->getY()<6)
                gpMonde->transitFull(7, 16*19+8, 16*16+8);
            if (gpJoueur->getY()>gpMonde->regionValue(3)-28)
                gpMonde->transitFull(7, 16*19+8, 16*25+8);
            break;
        case 54 :
            if (gpJoueur->getY()>gpMonde->regionValue(3)-28)
                gpMonde->transitFull(7, 16*34+8, 16*22+8);
            break;
        case 55 :
            if (gpJoueur->getY()>gpMonde->regionValue(3)-28 && gpJoueur->getX()<20*16)
                gpMonde->transitFull(7, 16*38+8, 16*7+8);
            if (gpJoueur->getY()>gpMonde->regionValue(3)-28 && gpJoueur->getX()>=20*16)
                gpMonde->transitFull(7, 16*44+8, 16*7+8);
            break;
        case 56 :
            if (gpJoueur->getY()>gpMonde->regionValue(3)-28)
                gpMonde->transitFull(7, 16*57+8, 16*7+8);
            break;
        case 57 :
            if (gpJoueur->getY()>gpMonde->regionValue(3)-28)
                gpMonde->transitFull(7, 16*64+8, 16*22+8);
            break;
        case 58 :
            if (gpJoueur->getY()>gpMonde->regionValue(3)-28)
                gpMonde->transitFull(7, 16*49+8, 16*34+8);
            break;
        case 59 :
            if (gpJoueur->getY()>gpMonde->regionValue(3)-28)
                gpMonde->transitFull(7, 16*49+8, 16*22+8);
            break;
        case 60 :
            if (gpJoueur->getY()>gpMonde->regionValue(3)-28)
                gpMonde->transitFull(7, 16*64+8, 16*34+8);
            break;
        case 61 :
            if (gpJoueur->getY()>gpMonde->regionValue(3)-28)
                gpMonde->transitFull(10, 16*45+8, 16*52+8);
            break;
        case 62 :
            if (gpJoueur->getY()>gpMonde->regionValue(3)-28)
                gpMonde->transitFull(11, 16*40+8, 16*18+8);
            break;
        case 63 :
            if (gpJoueur->getY()>gpMonde->regionValue(3)-28)
                gpMonde->transitFull(11, 16*51+8, 16*32+8);
            break;
        case 64 :
            if (gpJoueur->getY()>gpMonde->regionValue(3)-28)
                gpMonde->transitFull(11, 16*29+8, 16*31+8);
            break;
        case 65 :
            if (gpJoueur->getY()>gpMonde->regionValue(3)-28)
                gpMonde->transitFull(11, 16*65+8, 16*22+8);
            break;
        case 66 :
            if (gpJoueur->getY()>gpMonde->regionValue(3)-28)
                gpMonde->transitFull(11, 16*13+8, 16*22+8);
            break;
        case 67 :
            if (gpJoueur->getY()>gpMonde->regionValue(3)-28)
                gpMonde->transitFull(13, 16*26+8, 16*24+8);
            break;
        case 68 :
            if (gpJoueur->getY()>gpMonde->regionValue(3)-28)
                gpMonde->transitFull(13, 16*38+8, 16*29+8);
            break;
        case 69 :
            if (gpJoueur->getY()>gpMonde->regionValue(3)-28)
                gpMonde->transitFull(13, 16*55+8, 16*27+8);
            break;
        case 70 :
            if (gpJoueur->getY()>gpMonde->regionValue(3)-28)
                gpMonde->transitFull(13, 16*48+8, 16*34+8);
            break;
        case 71 :
            if (gpJoueur->getY()>gpMonde->regionValue(3)-28)
                gpMonde->transitFull(5, 16*176+8, 16*73+8);
            break;
    }
    
    if (!isDehors() && !transition) {
        
        int dir=-1;
        
        if (gpJoueur->getY()<gpMonde->regionValue(1)+6 && !transition) {
            gpMonde->transitSalle(N);dir=N;}
        if (gpJoueur->getY()>gpMonde->regionValue(3)-28 && !transition) {
            gpMonde->transitSalle(S);dir=S;}
        if (gpJoueur->getX()<gpMonde->regionValue(0)+6 && !transition) {
            gpMonde->transitSalle(O);dir=O;}
        if (gpJoueur->getX()>gpMonde->regionValue(2)-24 && !transition) {
            gpMonde->transitSalle(E);dir=E;}
        
    }
}
