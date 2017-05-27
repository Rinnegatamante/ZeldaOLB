/*

    Zelda Oni Link Begins

    Copyright (C) 2006-2008  Vincent Jouillat

    Please send bugreports with examples or suggestions to www.zeldaroth.fr

*/

#include <iostream>

#include "vita/os_vita.h"

#include "Keyboard.h"

Keyboard::Keyboard(Jeu* jeu, Carte* carte, Encyclopedie* encycl, SDL_Surface* screen, int m) : 
    gpJeu(jeu), gpCarte(carte), gpEncyclopedie(encycl), mode(m), gFullScreen(1), 
    gpScreen(screen), tmp(0), tmpx(0), tmpc(0), tmpw(0), tmpt(0), tmpp(0), tmpm(0), tmpo(0), 
    tmptp(0), ligne(0), colonne(0), ligneOption(3), volume(64), volson(64), ligneRecord(3), 
    colonneRecord(0), temps(0), ligneVal(0), intro(0), telep(0) {
    for (int i = 0; i < 3; i++) save[i]=0;
    for (int i = 0; i < 6; i++) rang[i]=0;
    loadP();
    loadOldP();
    gpJeu->getAudio()->setVolume(volume);
    gpJeu->getAudio()->setVolson(volson);
}

void Keyboard::saveP() {
	int languageID = getLanguage();
    ofstream f("ux0:/data/ZeldaOLB/system.dat",ios::out | ios::binary);
    f.write((char *)&volume,sizeof(int));
    f.write((char *)&volson,sizeof(int));
    f.write((char *)&temps,sizeof(int));
    for (int i = 0; i < 3; i++) f.write((char *)&rang[3+i],sizeof(int));
    f.write((char *)&languageID,sizeof(int));
    f.close();
}

void Keyboard::loadP() {
	int languageID = 0;
    ifstream f("ux0:/data/ZeldaOLB/system.dat",ios::in | ios::binary);
    if(!f.is_open()) return;
    f.read((char *)&volume,sizeof(int));
    f.read((char *)&volson,sizeof(int));
    f.read((char *)&temps,sizeof(int));
    for (int i = 0; i < 3; i++) f.read((char *)&rang[3+i],sizeof(int));
    if(!f.eof()){ //for retro compatibility
		 f.read((char *)&languageID,sizeof(int));
		 setLanguage(languageID);  
 	}
    f.close();
}

void Keyboard::loadOldP() {
    ifstream f("ux0:/data/ZeldaROTH/system.dat",ios::in | ios::binary);
    if(!f.is_open()) return;
    int i;
    f.read((char *)&i,sizeof(int));
    f.read((char *)&i,sizeof(int));
    f.read((char *)&i,sizeof(int));
    for (int i = 0; i < 3; i++) f.read((char *)&rang[i],sizeof(int));
    f.close();
}

int Keyboard::getLigne() {return ligne;}
int Keyboard::getColonne() {return colonne;}
int Keyboard::getSave(int i) {return save[i];}
void Keyboard::setSave(int i, int val) {save[i]=val;}
int Keyboard::getLigneOption() {return ligneOption;}
int Keyboard::getVolume() {return volume;}
int Keyboard::getVolson() {return volson;}
int Keyboard::getLigneRecord() {return ligneRecord;}
int Keyboard::getColonneRecord() {return colonneRecord;}
int Keyboard::getTemps() {return temps;}
void Keyboard::setTemps(int t) {temps=t;}
int Keyboard::getRang(int i) {return rang[i];}
void Keyboard::setRang(int i) {rang[i]=1;}
int Keyboard::getLigneVal() {return ligneVal;}
int Keyboard::getIntro() {return intro;}
int Keyboard::getMode() {return mode;}
void Keyboard::setMode(int i) {mode=i;}

int Keyboard::gererClavier() {
 	int ev = SDL_PollEvent(&event);
	if(ev & SDL_QUIT) return -1;
	if(ev & SDL_KEYDOWN) {
		keys = SDL_GetKeyState(NULL);
		if(pollKeys(keys)==-1) return -1;
    }
	if(ev & SDL_FINGERDOWN) {
		touchLocation.x = getTouchX();
		touchLocation.y = getTouchY();
	} else {
		touchLocation.x = -1;
		touchLocation.y = -1;
	}
	return 0;
}

void Keyboard::toggleFullScreen() {
    gFullScreen = (gFullScreen ? 0 : SDL_FULLSCREEN);
    gFullScreen ? SDL_ShowCursor(SDL_DISABLE) : SDL_ShowCursor(SDL_ENABLE);
    gpScreen = SDL_SetVideoMode(640, 480, 32, SDL_HWSURFACE
                                |SDL_DOUBLEBUF|gFullScreen);   
}

int Keyboard::pollKeys(int keys) {
    Joueur* gpJoueur;
    int vit;
    int avance;
    int nbdir;

	if (keys & SDLK_ESCAPE) {
             if (mode==0) { if (!gpJeu->getStop() && !gpJeu->getJoueur()->getImmo() 
            && gpJeu->getJoueur()->getVie()>0) 
                gpJeu->ecrit(215);
            } else if (mode<8 || mode==9 || mode==15 || mode==16 
            || (mode >18 && mode !=22)) return -1;
    }

    if (keys & KMOD_ALT) {

		if ( (keys &  SDLK_x) && mode == 0) {
			mode = 17; 
			gpJeu->getGenerique()->initAide1(); 
			gpJeu->setStop(true);
			gpJeu->getAudio()->playSound(1);
		}
        if (keys & SDLK_F4) return -1;
		if ((keys & SDLK_RETURN ) && tmpa==0) {
			toggleFullScreen();
			tmpa=1;
		}
    }

    if (!((keys&SDLK_RETURN) && (keys & KMOD_ALT))) tmpa=0; 

    switch (mode) {
        case 0 :
            gpJoueur = gpJeu->getJoueur();

            if ((keys & KMOD_ALT) && (keys & SDLK_F1) && tmp == 0) { 
                if (!gpJeu->getStop() && !gpJeu->getMenu()) gpJeu->setMenu(true);
                else if (gpJeu->getMenu()) gpJeu->setMenu(false);
                tmp = 1;
            }
 
            if ((!(keys&SDLK_F1) && !(keys&SDLK_RETURN) && !gpJeu->getMenu()) || (gpJeu->getMenu() &&
            !(keys&SDLK_F1) && !(keys&SDLK_RETURN) && !(keys&SDLK_LEFT) && !(keys&SDLK_RIGHT) 
            && !(keys&SDLK_UP) && !(keys&SDLK_DOWN))) tmp=0;
            
            if (keys&SDLK_SPACE && !gpJeu->getStop() && gpJoueur->getVie() && 
            (gpJoueur->getTypeAnim()==AUCUNE || gpJoueur->getTypeAnim()==MARCHE) 
             && !gpJoueur->getImmo()  && tmp == 0) {
                gpJeu->lire();
                tmp = 1;
            } else if (!(keys & KMOD_ALT) && (keys & SDLK_RETURN) && tmp == 0) { 
               if (gpJoueur->getTypeAnim() == MORT) gpJoueur->revie();
               else if (gpJeu->getMenu()) gpJeu->setMenu(false);
               else if (gpJeu->getText()) gpJeu->setText(gpJeu->getTexte()->suite());
               tmp = 1;
            }

            if (gpJeu->getText() && gpJeu->getTexte()->isFinished()) {
                if(tmpt==0){
                    tmpt=1;
                    switch (gpJeu->getTexte()->getId()) {
                        case 41 : case 76 : case 233 : case 292 : case 300 : 
                        case 310 : case 313 : case 316 : case 318 : case 321 : case 323 : 
                        case 331 : case 333 : case 360 : case 369 :
                            if ((keys&SDLK_LEFT) || (keys&SDLK_RIGHT)) {
                                gpJeu->getAudio()->playSound(3);
                                gpJeu->getTexte()->changeId(gpJeu->getTexte()->getId()+1);
                            }
                            break;
                        case 42 : case 77 : case 234 : case 293 : case 301 : 
                        case 311 : case 314 : case 317 : case 319 : case 322 : case 324 : 
                        case 332 : case 334 : case 361 : case 370 :
                            if ((keys&SDLK_LEFT) || (keys&SDLK_RIGHT)) {
                                gpJeu->getAudio()->playSound(3);
                                gpJeu->getTexte()->changeId(gpJeu->getTexte()->getId()-1);
                            }
                            break;
                        case 17 : case 18 : case 19 : case 20 : case 21 : case 22 :
                        case 23 : case 24 : case 25 : case 26 : case 27 : case 28 :
                        case 29 : case 30 : case 31 : case 32 : case 33 : case 34 : 
                        case 35 : case 215 : case 216 : case 217 : case 328 : case 329 : 
                        case 330 : case 199 : case 200 : 
                            if (gpJeu->getTexte()->getId()!=217 && gpJeu->getTexte()->getId()!=18 
                            && gpJeu->getTexte()->getId()!=21 && gpJeu->getTexte()->getId()!=25 
                            && gpJeu->getTexte()->getId()!=27 && gpJeu->getTexte()->getId()!=29
                            && gpJeu->getTexte()->getId()!=32 && gpJeu->getTexte()->getId()!=35 
                            && gpJeu->getTexte()->getId()!=330 
                            && gpJeu->getTexte()->getId()!=200) {
                                if (keys&SDLK_DOWN) {
                                    gpJeu->getAudio()->playSound(3);
                                    gpJeu->getTexte()->changeId(gpJeu->getTexte()->getId()+1);
                                }
                            }
                            if (gpJeu->getTexte()->getId()!=215 && gpJeu->getTexte()->getId()!=17 
                            && gpJeu->getTexte()->getId()!=19 && gpJeu->getTexte()->getId()!=22 
                            && gpJeu->getTexte()->getId()!=26 && gpJeu->getTexte()->getId()!=28
                            && gpJeu->getTexte()->getId()!=30 && gpJeu->getTexte()->getId()!=33 
                            && gpJeu->getTexte()->getId()!=328
                            && gpJeu->getTexte()->getId()!=199) {
                                if (keys&SDLK_UP) {
                                    gpJeu->getAudio()->playSound(3);
                                    gpJeu->getTexte()->changeId(gpJeu->getTexte()->getId()-1);
                                }
                            }
                            break;
                    }
                }
                if (!(keys&SDLK_RETURN) && !(keys&SDLK_RETURN) && !(keys&SDLK_LEFT) 
                && !(keys&SDLK_RIGHT) && !(keys&SDLK_UP) && !(keys&SDLK_DOWN)) tmpt=0;
            }
            
            nbdir=0;
            if (keys&SDLK_UP) nbdir++;
            if (keys&SDLK_DOWN) nbdir++;
            if (keys&SDLK_LEFT) nbdir++;
            if (keys&SDLK_RIGHT) nbdir++;
           
            int vitesse;
            if (((keys&SDLK_CAPSLOCK) || (keys&SDLK_LSHIFT)) && !gpJeu->getStop() 
            && gpJoueur->hasObjet(O_BOTTES)) vitesse=4; 
            else vitesse=2;
    
            avance=0;
            
            //marche
            if (!(keys&SDLK_LCTRL) && (
            gpJoueur->getTypeAnim()==AUCUNE || gpJoueur->getTypeAnim()==MARCHE 
            || gpJoueur->getTypeAnim()==PORTE || gpJoueur->getTypeAnim()==EMMENE
            || gpJoueur->getTypeAnim()==NAGE || gpJoueur->getTypeAnim()==FLOTTE) 
            && !gpJeu->getStop() && !gpJeu->getTransition() && !gpJoueur->getImmo()) {
                gpJoueur->savePrec();
                if (keys&SDLK_LEFT) {
                    if(!gpJoueur->getCharge() 
                    && (!(keys&SDLK_UP) || gpJoueur->getDirection()!=N) 
                    && (!(keys&SDLK_DOWN) || gpJoueur->getDirection()!=S))
                        gpJoueur->setDirection(O);
                    if (gpJoueur->getX()%4==2) vit=2; else vit = vitesse;
                    //gpJeu->moveJoueurX(-vit, nbdir);
                    gpJoueur->moveX(-vit, nbdir);
                    avance = 1;
                }
                if (keys&SDLK_RIGHT) {
                    if(!gpJoueur->getCharge() 
                    && (!(keys&SDLK_UP) || gpJoueur->getDirection()!=N) 
                    && (!(keys&SDLK_DOWN) || gpJoueur->getDirection()!=S))
                        gpJoueur->setDirection(E);
                    if (gpJoueur->getX()%4==2) vit=2; else vit = vitesse;
                    //gpJeu->moveJoueurX(vit, nbdir);
                    gpJoueur->moveX(vit, nbdir);
                    avance=1;
                }
                if (keys&SDLK_UP) {
                    if(!gpJoueur->getCharge()
                    && (!(keys&SDLK_LEFT) || gpJoueur->getDirection()!=O) 
                    && (!(keys&SDLK_RIGHT) || gpJoueur->getDirection()!=E))
                        gpJoueur->setDirection(N);
                    if (gpJoueur->getY()%4!=0) vit=2; else vit = vitesse;
                    //gpJeu->moveJoueurY(-vit, nbdir);
                    gpJoueur->moveY(-vit, nbdir);
                    avance=1;
                }
                if (keys&SDLK_DOWN) {
                    if(!gpJoueur->getCharge()
                    && (!(keys&SDLK_LEFT) || gpJoueur->getDirection()!=O) 
                    && (!(keys&SDLK_RIGHT) || gpJoueur->getDirection()!=E))
                        gpJoueur->setDirection(S);
                    if (gpJoueur->getY()%4!=0) vit=2; else vit = vitesse;
                    //gpJeu->moveJoueurY(vit, nbdir);
                    gpJoueur->moveY(vit, nbdir);
                    avance=1;
                }
                if (gpJeu->getVueHorz()>0) {
                    if (gpJeu->getVueHorz()>=4) gpJeu->setVueHorz(gpJeu->getVueHorz()-4);
                    else gpJeu->setVueHorz(0);
                }
                if (gpJeu->getVueHorz()<0) {
                    if (gpJeu->getVueHorz()<=-4) gpJeu->setVueHorz(gpJeu->getVueHorz()+4);
                    else gpJeu->setVueHorz(0);
                }
                if (gpJeu->getVueVert()>0) {
                    if (gpJeu->getVueVert()>=4) gpJeu->setVueVert(gpJeu->getVueVert()-4);
                    else gpJeu->setVueVert(0);
                }
                if (gpJeu->getVueVert()<0) {
                    if (gpJeu->getVueVert()<=-4) gpJeu->setVueVert(gpJeu->getVueVert()+4);
                    else gpJeu->setVueVert(0);
                }
            }
            if ((keys&SDLK_LCTRL) && !gpJoueur->getImmo()) {
                if ((keys&SDLK_LEFT) && gpJeu->getVueHorz()>-64)
                    gpJeu->setVueHorz(gpJeu->getVueHorz()-2);
                if ((keys&SDLK_RIGHT) && gpJeu->getVueHorz()<64)
                    gpJeu->setVueHorz(gpJeu->getVueHorz()+2);
                if ((keys&SDLK_UP) && gpJeu->getVueVert()>-64)
                    gpJeu->setVueVert(gpJeu->getVueVert()-2);
                if ((keys&SDLK_DOWN) && gpJeu->getVueVert()<64)
                    gpJeu->setVueVert(gpJeu->getVueVert()+2);
            }
            
            if (avance == 1) {
                if (gpJoueur->getTypeAnim()==AUCUNE) gpJoueur->setTypeAnim(MARCHE);
                if (gpJoueur->getTypeAnim()==FLOTTE) gpJoueur->setTypeAnim(NAGE);
                if (gpJoueur->getTypeAnim()==PORTE) gpJoueur->setTypeAnim(EMMENE);
                gpJeu->testFin();
            }
            else {
                if (gpJoueur->getTypeAnim()==MARCHE && !gpJeu->getStop()) 
                    gpJoueur->setTypeAnim(AUCUNE);
                if (gpJoueur->getTypeAnim()==NAGE && !gpJeu->getStop()) 
                    gpJoueur->setTypeAnim(FLOTTE);
                if (gpJoueur->getTypeAnim()==EMMENE && !gpJeu->getStop()) 
                    gpJoueur->setTypeAnim(PORTE);
            }
            
            
            //épée
            if ((gpJoueur->getTypeAnim()==AUCUNE || gpJoueur->getTypeAnim()==MARCHE) 
            && !gpJoueur->getCharge() && gpJoueur->getEpee() && !gpJeu->getStop()
            && !gpJoueur->isLapin() && !gpJoueur->getImmo()) {
                if (!(keys & KMOD_ALT) && ((keys&SDLK_z) || (keys&SDLK_w)) && !tmpw) {
                    gpJoueur->setTypeAnim(EPEE);
                    gpJoueur->chargeSpin();
                    tmpw=1;
                }
                if (!(keys&SDLK_z) && !(keys&SDLK_w) && tmpw) tmpw=0;
            }
            if (!(keys&SDLK_z) && !(keys&SDLK_w) && gpJoueur->getCharge() && !gpJeu->getStop() 
            && !gpJoueur->isLapin()) {
                if (gpJoueur->getCharge() >= 20) gpJoueur->setTypeAnim(SPIN);
                gpJoueur->dechargeSpin();
            }
            
            //touchscreeen object selection
			
			if (gpJeu->getMenu() || !gpJeu->getStop()) {
				int sx=-1;
				int sy=-1;
				if (touchLocation.x>=32 && touchLocation.x<128) {
					if (touchLocation.x>=96) sx=2;
					else if (touchLocation.x>=64) sx=1;
					else sx=0;
				}
				if (touchLocation.y>=32 && touchLocation.y<160) {
					if (touchLocation.y>=128) sy=3;
					else if (touchLocation.y>=96) sy=2;
					else if (touchLocation.y>=64) sy=1;
					else sy=0;
				}
				if (sx>-1 && sy>-1) gpJoueur->setObjet(sx+sy*3);
			}
			
            //curseur menu
            if (gpJeu->getMenu() && !gpJoueur->getOni() &&
            (gpJoueur->getTypeAnim()<4 || gpJoueur->getTypeAnim()>20) && tmp==0) {
                if (keys&SDLK_LEFT) {
                    int obj = gpJoueur->getObjet();
                    if (obj%3 == 0) gpJoueur->setObjet(obj+2);
                    else gpJoueur->setObjet(obj-1);
                    gpJeu->getAudio()->playSound(3);
                    tmp=1;
                }
                if (keys&SDLK_RIGHT) {
                    int obj = gpJoueur->getObjet();
                    if (obj%3 == 2) gpJoueur->setObjet(obj-2);
                    else gpJoueur->setObjet(obj+1);
                    gpJeu->getAudio()->playSound(3);
                    tmp=1;
                }
                if (keys&SDLK_UP) {
                    int obj = gpJoueur->getObjet();
                    if (obj/3 == 0) gpJoueur->setObjet(obj+9);
                    else gpJoueur->setObjet(obj-3);
                    gpJeu->getAudio()->playSound(3);
                    tmp=1;
                }
                if (keys&SDLK_DOWN) {
                    int obj = gpJoueur->getObjet();
                    if (obj/3 == 3) gpJoueur->setObjet(obj-9);
                    else gpJoueur->setObjet(obj+3);
                    gpJeu->getAudio()->playSound(3);
                    tmp=1;
                }
            }
            
            if (!(keys & KMOD_ALT) &&(keys&SDLK_x) && (
            gpJoueur->getTypeAnim()==AUCUNE || gpJoueur->getTypeAnim()==MARCHE || 
            (gpJoueur->getObjet()==8 && 
            (gpJoueur->getTypeAnim()==PORTE || gpJoueur->getTypeAnim()==EMMENE))) 
            && !gpJeu->getStop() && !gpJoueur->getImmo()
            && gpJoueur->getVie()>0 && !gpJoueur->getCharge() && !gpJoueur->isLapin() 
            && (!gpJoueur->getOni() || gpJoueur->getObjet()==8)
            && !tmpx) {
                tmpx=1;
                switch(gpJoueur->getObjet()) {
                    case 0 : //arc
                        if (gpJoueur->hasObjet(O_ARC)) gpJoueur->setTypeAnim(ARC);
                        break;
                    case 1 : //grappin
                        if (gpJoueur->hasObjet(O_GRAPPIN)) gpJoueur->setTypeAnim(GRAPPIN);
                        break;
                    case 2 : //bombe
                        if (gpJoueur->hasObjet(O_SAC_BOMBES) && gpJoueur->getBombe()) 
                            gpJoueur->setTypeAnim(BOMBE);
                        break;
                    case 3 : //baguette de feu
                        if (gpJoueur->hasObjet(O_BFEU)) gpJoueur->setTypeAnim(BFEU);
                        break;
                    case 4 : //baguette de glace
                        if (gpJoueur->hasObjet(O_BGLACE)) gpJoueur->setTypeAnim(BGLACE);
                        break;
                    case 5 : //lanterne
                        if (gpJoueur->hasObjet(O_LANTERNE) && gpJoueur->getMagie()) 
                            gpJoueur->setTypeAnim(LANTERNE);
                        break;
                    case 6 : //marteau
                        if (gpJoueur->hasObjet(O_MARTEAU)) gpJoueur->setTypeAnim(MARTEAU);
                        break;
                    case 7 : //ocarina
                        if (gpJoueur->hasObjet(O_OCARINA)) {
                            gpJoueur->setTypeAnim(JOUE);
                            //gpJeu->setStop(true);}
                            gpJeu->ecrit(16);}
                        break;
                    case 8 : //masque
                        if (gpJoueur->hasObjet(O_MASQUE)<2) break;
                        if (gpJeu->isVillage(gpJoueur->getX(),gpJoueur->getY())) {
                            gpJeu->ecrit(128); break;}
                        if (gpJoueur->getOni()) {
                            gpJeu->getAudio()->playSound(2); gpJoueur->setOni(false);
                        }else {gpJeu->getAudio()->playSound(1); gpJoueur->setOni(true);}
                        break;
                    case 9 : // flacon
                    case 10 :
                    case 11 :
                        if (gpJoueur->hasBouteille(gpJoueur->getObjet()-9)) 
                            gpJoueur->setTypeAnim(BOIRE);
                        break;
                }    
            }
            
            if (!(keys&SDLK_x) && tmpx) tmpx=0;
            
            if (!(keys & KMOD_ALT) && (keys&SDLK_c) && !tmpc && !gpJoueur->getCharge() && gpJoueur->getVie()>0
            && !gpJoueur->isLapin() && !gpJeu->getStop() && !gpJoueur->getImmo()) {
                if (gpJoueur->getTypeAnim()==AUCUNE || gpJoueur->getTypeAnim()==MARCHE)
                    gpJeu->ramasse();
                else if (gpJoueur->getTypeAnim()==PORTE || gpJoueur->getTypeAnim()==EMMENE)
                    gpJoueur->setTypeAnim(LANCE);
            }
            
            if (!(keys&SDLK_c) && tmpc) tmpc=0;
            
            if (!(keys & KMOD_ALT) && keys&SDLK_p && (gpJoueur->hasObjet(O_CARTE) || gpJeu->isDonjon()) && 
            (gpJeu->isDehors() || gpJeu->isDonjon())
            && !gpJeu->getStop() && gpJoueur->getVie()>0 && !tmpp) {
                mode = 12;
 				gpJeu->setStop(true);
                gpJeu->getAudio()->playSound(1);
                gpCarte->init();
                tmpp=1;
            }
            
            if (!((keys&SDLK_p)) && tmpp) tmpp=0;
            
 			//open monsters screen
            if ((keys & KMOD_ALT) && (keys&SDLK_z) && gpJoueur->hasObjet(O_ENCYCL) 
            && !gpJeu->getStop() && gpJoueur->getVie()>0 && !tmpm) {
                mode = 13;
				gpJeu->setStop(true);
                gpJeu->getAudio()->playSound(1);
                gpEncyclopedie->setPage(0);
                gpEncyclopedie->init();
                tmpm=1;
            }
            
            // monsters screen related
            if (!((keys&SDLK_z)) && tmpm) tmpm=0;
            
            if ((keys & KMOD_ALT) && keys&SDLK_o && gpJoueur->hasObjet(O_MASQUE)==2 && !gpJoueur->isLapin() && !tmpo
            && !gpJeu->getStop() && !gpJoueur->getImmo() && gpJoueur->getVie()>0) {
                if (gpJeu->isVillage(gpJoueur->getX(),gpJoueur->getY())) {
                    gpJeu->ecrit(128); }
                else {
                    gpJoueur->setObjet(8);
                    if (gpJoueur->getOni()) {
                        gpJeu->getAudio()->playSound(2);gpJoueur->setOni(false);}
                    else {gpJeu->getAudio()->playSound(1); gpJoueur->setOni(true);}
                }
                tmpo=1;
            }
            
            if (!(keys&SDLK_o) && tmpo) tmpo=0;
            
            
            if ((keys & KMOD_ALT) && keys&SDLK_t && gpJoueur->getOni() && !gpJoueur->isLapin()
            && gpJeu->isDonjon() && !tmptp
            && !gpJeu->getStop() && !gpJoueur->getImmo() && gpJoueur->getVie()>0) {
                gpJeu->getAudio()->playSound(5);
                SDL_Delay(1000);
                gpJeu->getAudio()->stopSpecial();
                gpJoueur->restoreEmpl();
                tmptp=1;
            }
            
            if (!(keys&SDLK_t) && tmptp) tmptp=0;
            break;
        case 1 :
            if (!(keys & KMOD_ALT) && (keys&SDLK_RETURN) && tmp == 0) {
                mode = 2;
                gpJeu->getGenerique()->initLogo();
                gpJeu->getAudio()->playSound(1);
                tmp = 1;
            }
            if (!(keys&SDLK_RETURN)) tmp=0;
            break;
        case 2 :
             if (!(keys & KMOD_ALT) && (keys&SDLK_RETURN) && tmp == 0) {
               mode = 3;
                gpJeu->getGenerique()->initTitre();
                gpJeu->getAudio()->playSound(1);
                gpJeu->getAudio()->playMusic(80);
                tmp = 1;
            }
            if (!(keys&SDLK_RETURN)) tmp=0;
            break;
        case 3 :
            if (!(keys & KMOD_ALT) && (keys&SDLK_RETURN) && tmp == 0) {
                mode = 4; colonne=0;
                gpJeu->getGenerique()->initSelection();
                gpJeu->getAudio()->playSound(1);
                gpJeu->getAudio()->playMusic(90);
                tmp = 1;
            }
            if (!(keys&SDLK_RETURN)) tmp=0;
            break;
        case 4 :
            if ((keys&SDLK_RETURN) && tmp == 0) {
                if (ligne < 3) {
                    if (save[ligne]) {
                        mode = 7; ligneVal=0;
                        gpJeu->getGenerique()->initCharger();
                    }
                    else {
                        mode = 8; intro = 0; gpJeu->getGenerique()->initIntro();
                        gpJeu->getAudio()->playMusic(100);
                    }
                }
                if (ligne == 3 && colonne == 0) {
                    mode = 6; ligneOption=2;
                    gpJeu->getGenerique()->initOption();
                }
                if (ligne == 3 && colonne == 1) {
                    mode = 15; ligneRecord=3; colonneRecord=0;
                    gpJeu->getGenerique()->initRecord();
                }
                tmp=1; gpJeu->getAudio()->playSound(1);
            }
            
            if ((keys&SDLK_UP) && !tmp) {
                ligne--; if (ligne<0) ligne=3; tmp=1; 
                if (ligne!=3) colonne=0;
                gpJeu->getAudio()->playSound(3);}
            if ((keys&SDLK_DOWN) && !tmp) {
                ligne++; if (ligne>3) ligne=0; tmp=1;
                if (ligne!=3) colonne=0;
                gpJeu->getAudio()->playSound(3);}
            if ((keys&SDLK_LEFT) && !tmp && ligne==3) {
                colonne--; if (colonne<0) colonne=1; tmp=1;
                gpJeu->getAudio()->playSound(3);}
            if ((keys&SDLK_RIGHT) && !tmp && ligne==3) {
                colonne++; if (colonne>1) colonne=0; tmp=1;
                gpJeu->getAudio()->playSound(3);}
            
            if (!(keys&SDLK_RETURN) && !(keys&SDLK_UP) && !(keys&SDLK_DOWN) 
                && !(keys&SDLK_LEFT) && !(keys&SDLK_RIGHT) && tmp) tmp=0;
            break;
        case 6 :
            if (!(keys & KMOD_ALT) && (keys&SDLK_RETURN) && tmp == 0 && ligneOption == 3) {
                mode = 4;
                gpJeu->getGenerique()->initSelection();
                gpJeu->getAudio()->playSound(2);
                tmp = 1;
                saveP();
            }
            
            if ((keys&SDLK_UP) && !tmp) {
                ligneOption--; if (ligneOption<0) ligneOption=3; tmp=1; 
                gpJeu->getAudio()->playSound(3);}
            if ((keys&SDLK_DOWN) && !tmp) {
                ligneOption++; if (ligneOption>3) ligneOption=0; tmp=1;
                gpJeu->getAudio()->playSound(3);}
            if ((keys&SDLK_LEFT) && !tmp) {
                if (ligneOption == 0) {
                    volume-=64; if (volume < 0) volume = 0; tmp=1;
                    gpJeu->getAudio()->setVolume(volume); 
					gpJeu->getAudio()->playSound(3);
					if (volume==0) gpJeu->getAudio()->stopMusic();}
                if (ligneOption == 1) {
                    volson-=64; if (volson < 0) volson = 0; tmp=1;
                    gpJeu->getAudio()->setVolson(volson); 
					gpJeu->getAudio()->playSound(3);}
                if (ligneOption == 2) {
                    if (getLanguage()==5) setLanguage(2); 
					else setLanguage(getLanguage()-1);
					tmp=1;
                    gpJeu->getAudio()->playSound(3);}}
            if ((keys&SDLK_RIGHT) && !tmp) {
                if (ligneOption == 0) {
					int vtemp = volume;
                    volume+=64; if (volume > 64) volume = 64; tmp=1;
                    gpJeu->getAudio()->setVolume(volume);
					gpJeu->getAudio()->playSound(3);
					if (vtemp==0) 
						gpJeu->getAudio()->replayMusic();}
                if (ligneOption == 1) {
                    volson+=64; if (volson > 64) volson = 64; tmp=1;
                    gpJeu->getAudio()->setVolson(volson);gpJeu->getAudio()->playSound(3);}
                if (ligneOption == 2) {
					if (getLanguage()==2) setLanguage(5); 
					else setLanguage(getLanguage()+1);
					tmp=1;
                    gpJeu->getAudio()->playSound(3);}}

            if (!(keys&SDLK_RETURN) && !(keys&SDLK_UP) && !(keys&SDLK_DOWN) 
                && !(keys&SDLK_LEFT) && !(keys&SDLK_RIGHT) && tmp) tmp=0;
            break;
        case 7 :
            if (!(keys & KMOD_ALT) && (keys&SDLK_RETURN) && tmp == 0) {
                if (ligneVal==0) {
                    mode = 0;
                    gpJeu->reinit(ligne+1);
                }
                if (ligneVal==1) {
                    mode = 9; ligneVal=1;
                    gpJeu->getGenerique()->initEffacer();
                    gpJeu->getAudio()->playSound(1);
                }
                if (ligneVal==2) {
                    mode = 4; gpJeu->getGenerique()->initSelection();
                    gpJeu->getAudio()->playSound(2);
                }    
                tmp = 1;
            }
            
            if ((keys&SDLK_UP) && !tmp) {
                if (--ligneVal<0) ligneVal=2; tmp=1; 
                gpJeu->getAudio()->playSound(3);}
            if ((keys&SDLK_DOWN) && !tmp) {
                if (++ligneVal>2) ligneVal=0; tmp=1;
                gpJeu->getAudio()->playSound(3);}
             
            if (!(keys&SDLK_RETURN) && !(keys&SDLK_UP) && !(keys&SDLK_DOWN) && tmp) tmp=0;
            break;
        case 8 :
            if (!(keys & KMOD_ALT) && (keys&SDLK_RETURN) && tmp == 0) {
                int old;
                old = gpJeu->getTexte()->getId();
                if (!gpJeu->getTexte()->suite()) {
                    mode = 10; gpJeu->getGenerique()->initNuit();
                    gpJeu->getAudio()->stopMusic();
                }
                else if (old != gpJeu->getTexte()->getId()) intro++;
                tmp = 1;
            }
            if (!(keys&SDLK_RETURN) && tmp) tmp=0;
            break;
        case 9 :
            if (!(keys & KMOD_ALT) && (keys&SDLK_RETURN) && tmp == 0) {
                if (ligneVal==0) {
                    ostringstream oss; oss << (ligne+1);
                    remove(("ux0:/data/ZeldaOLB/olb" + oss.str() + ".dat").c_str());
                    mode = 4; gpJeu->getGenerique()->initSelection();
                    gpJeu->getAudio()->playSound(2);
                }
                else {
                    mode = 7; gpJeu->getGenerique()->initCharger();
                    gpJeu->getAudio()->playSound(1);
                }
                tmp = 1;
            }
            
            if (((keys&SDLK_UP) || (keys&SDLK_DOWN)) && !tmp) {
                ligneVal ? ligneVal=0 : ligneVal = 1; tmp=1;
                gpJeu->getAudio()->playSound(3);}
            
            if (!(keys&SDLK_RETURN) && !(keys&SDLK_UP) && !(keys&SDLK_DOWN) && tmp) tmp=0;
            break;
        case 10 :
            if (!(keys & KMOD_ALT) && (keys&SDLK_RETURN) && tmp == 0) {
                if (gpJeu->getText()) gpJeu->setText(gpJeu->getTexte()->suite());
                if (!gpJeu->getText()) {
                    gpJeu->getAudio()->playSound(5);
                    SDL_Delay(2500);
                    mode = 0; gpJeu->reinit(ligne+1);
                    gpJeu->ecrit(408);
                }
                tmp = 1;
            }
            if (!(keys&SDLK_RETURN) && tmp) tmp=0;
            break;
        case 11 :
            if ((keys&SDLK_RETURN) && tmp == 0) {
                gpJeu->setText(gpJeu->getTexte()->suite());
                tmp = 1;
            }
            if (!(keys&SDLK_RETURN)) tmp=0;
            break;
        case 12 :
            if (!(keys & KMOD_ALT) && (keys&SDLK_RETURN) && !tmp) {
                mode = 0;
				gpJeu->setStop(false);
                gpJeu->getAudio()->playSound(2);
                tmp=1;
            }
            
            if ((keys&SDLK_UP) && !tmp) {
                gpCarte->moveLevel(N);
                tmp=1;
            }
            if ((keys&SDLK_DOWN) && !tmp) {
                gpCarte->moveLevel(S);
                tmp=1;
            }
            
            if (!(keys&SDLK_RETURN) && !(keys&SDLK_UP) && !(keys&SDLK_DOWN) && tmp) tmp=0;
            break;
        case 13 :
            if (!(keys & KMOD_ALT) && (keys&SDLK_RETURN) && !tmp) {
                mode = 0;
				gpJeu->setStop(false);
                gpJeu->getAudio()->playSound(2);
                tmp=1;
            }
            
            if ((keys&SDLK_LEFT) && !tmp) {
                gpJeu->getAudio()->playSound(3);
                gpEncyclopedie->moveL();
                tmp=1;
            }
            if ((keys&SDLK_RIGHT) && !tmp) {
                gpJeu->getAudio()->playSound(3);
                gpEncyclopedie->moveR();
                tmp=1;
            }
            
            if (!(keys&SDLK_RETURN) && !(keys&SDLK_LEFT) && !(keys&SDLK_RIGHT) && tmp) tmp=0;
            break;
        case 14 :
            if ((keys&SDLK_RETURN) && tmp == 0) {
                mode = 2; gpJeu->getAudio()->playSound(1);
                gpJeu->getGenerique()->initLogo();
                tmp = 1;
            }
            if (!(keys&SDLK_RETURN) && tmp) tmp=0;
            break;
        case 15 :
            if (!(keys & KMOD_ALT) && (keys&SDLK_RETURN) && tmp == 0) {
                if (ligneRecord==3) {
                    if (colonneRecord == 0) {
                        mode = 4;
                        gpJeu->getGenerique()->initSelection();
                        gpJeu->getAudio()->playSound(2);
                    }
                    else {
                        mode = 16; ligneVal=1;
                        gpJeu->getGenerique()->initEffacer();
                        gpJeu->getAudio()->playSound(1);
                    }
                }
                else {
                    mode = 19 + ligneRecord;
                    gpJeu->getGenerique()->initRang(ligneRecord);
                    gpJeu->getAudio()->playSound(1);
                }
                tmp = 1;
            }
            
            if ((keys&SDLK_UP) && !tmp) {
               ligneRecord--; if (ligneRecord<0) ligneRecord=3; tmp=1; 
                colonneRecord=0; gpJeu->getAudio()->playSound(3);}
            if ((keys&SDLK_DOWN) && !tmp) {
                ligneRecord++; if (ligneRecord>3) ligneRecord=0; tmp=1;
                colonneRecord=0; gpJeu->getAudio()->playSound(3);}
            
            if (((keys&SDLK_LEFT) || (keys&SDLK_RIGHT)) && ligneRecord==3 && !tmp) {
                if (colonneRecord == 0) colonneRecord=1;
                else colonneRecord=0; tmp=1;
                gpJeu->getAudio()->playSound(3);}
            
            if (!(keys&SDLK_RETURN) && !(keys&SDLK_UP) && !(keys&SDLK_DOWN) 
                && !(keys&SDLK_LEFT) && !(keys&SDLK_RIGHT) && tmp) tmp=0;
            break;
        case 16 :
            if (!(keys & KMOD_ALT) && (keys&SDLK_RETURN) && tmp == 0) {
                mode = 15;
                if (ligneVal==0) {
                    gpJeu->getAudio()->playSound(2);
                    temps=0; for (int i = 0; i < 3; i++) rang[3+i]=0; saveP();
                }
                else gpJeu->getAudio()->playSound(1);
                gpJeu->getGenerique()->initRecord();
                tmp = 1;
            }
            
            if (((keys&SDLK_UP) || (keys&SDLK_DOWN)) && !tmp) {
                ligneVal ? ligneVal=0 : ligneVal = 1; tmp=1;
                gpJeu->getAudio()->playSound(3);}
            
             if (!(keys&SDLK_RETURN) && !(keys&SDLK_UP) && !(keys&SDLK_DOWN) && tmp) tmp=0;
            break;
        case 17 :
            if (!(keys & KMOD_ALT) && (keys&SDLK_RETURN) && !tmp) {
                mode = 0;
				gpJeu->setStop(false);
                gpJeu->getAudio()->playSound(2);
                tmp=1;
            }
            if ((keys&SDLK_RIGHT) && !tmp) {
                mode = 18; gpJeu->getGenerique()->initAide2();
                gpJeu->getAudio()->playSound(3); tmp=1;}
            
           if (!(keys&SDLK_RETURN) && !(keys&SDLK_RIGHT) && tmp) tmp=0;
             break;
        case 18 :
            if ((keys&SDLK_RETURN) && !tmp) {
                mode = 0;
				gpJeu->setStop(false);
                gpJeu->getAudio()->playSound(2);
                tmp=1;
            }
            if ((keys&SDLK_LEFT) && !tmp) {
                mode = 17; gpJeu->getGenerique()->initAide1();
                gpJeu->getAudio()->playSound(3); tmp=1;}
            
            if (!(keys&SDLK_RETURN) && !(keys&SDLK_LEFT) && tmp) tmp=0;
            break;
        case 19 :
        case 20 :
        case 21 :
            if (!(keys & KMOD_ALT) && (keys&SDLK_RETURN) && !tmp) {
                mode = 15; gpJeu->getGenerique()->initRecord();
                gpJeu->getAudio()->playSound(2);
                tmp=1;
            }
            if (!(keys&SDLK_RETURN) && tmp) tmp=0;
            break;
        case 22 :
            if ((keys&SDLK_RETURN) && !tmp) {
                mode = 0;
                gpJeu->getAudio()->playSound(5);
                SDL_Delay(2500);
                gpJeu->teleporte(telep);
                tmp=1;
            }
            if ((keys & SDLK_LEFT) && !tmp) {
                telep--; if (telep<0) telep=8;
                gpJeu->getAudio()->playSound(3); tmp=1;}
            if ((keys&SDLK_RIGHT) && !tmp) {
                telep++; if (telep>8) telep=0;
                gpJeu->getAudio()->playSound(3); tmp=1;}
            if (((keys&SDLK_1) || (keys&SDLK_KP1)
            || (keys&SDLK_2) || (keys&SDLK_KP2)
            || (keys&SDLK_3) || (keys&SDLK_KP3)
            || (keys&SDLK_4) || (keys&SDLK_KP4)
            || (keys&SDLK_5) || (keys&SDLK_KP5)
            || (keys&SDLK_6) || (keys&SDLK_KP6)
            || (keys&SDLK_7) || (keys&SDLK_KP7)
            || (keys&SDLK_8) || (keys&SDLK_KP8)
            || (keys&SDLK_9) || (keys&SDLK_KP9)
            ) && !tmp) {
                if ((keys&SDLK_1) || (keys&SDLK_KP1))telep=0;
                if ((keys&SDLK_2) || (keys&SDLK_KP2))telep=1;
                if ((keys&SDLK_3) || (keys&SDLK_KP3))telep=2;
                if ((keys&SDLK_4) || (keys&SDLK_KP4))telep=3;
                if ((keys&SDLK_5) || (keys&SDLK_KP5))telep=4;
                if ((keys&SDLK_6) || (keys&SDLK_KP6))telep=5;
                if ((keys&SDLK_7) || (keys&SDLK_KP7))telep=6;
                if ((keys&SDLK_8) || (keys&SDLK_KP8))telep=7;
                if ((keys&SDLK_9) || (keys&SDLK_KP9))telep=8;
                gpJeu->getAudio()->playSound(3); tmp=1;
            }
            
            if (!(keys&SDLK_RETURN) && !(keys&SDLK_LEFT) && !(keys&SDLK_RIGHT) 
            && !(keys&SDLK_1) && !(keys&SDLK_2) && !(keys&SDLK_3)
            && !(keys&SDLK_4) && !(keys&SDLK_5) && !(keys&SDLK_6)
            && !(keys&SDLK_7) && !(keys&SDLK_8) && !(keys&SDLK_9)
            && !(keys&SDLK_KP1) && !(keys&SDLK_KP2) && !(keys&SDLK_KP3)
            && !(keys&SDLK_KP4) && !(keys&SDLK_KP5) && !(keys&SDLK_KP6)
            && !(keys&SDLK_KP7) && !(keys&SDLK_KP8) && !(keys&SDLK_KP9)
            && tmp) tmp=0;
            break;
    }
    return 0;
}

Carte* Keyboard::getCarte() {return gpCarte;}
int Keyboard::getTelep() {return telep;}
void Keyboard::setTelep(int i) {telep=i;}
