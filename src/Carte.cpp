/*

    Zelda Oni Link Begins

    Copyright (C) 2006-2008  Vincent Jouillat

    Please send bugreports with examples or suggestions to www.zeldaroth.fr

*/

#include "Carte.h"
#include "Keyboard.h"

Carte::Carte(Jeu* jeu) : gpJeu(jeu), levely(0) {
    image = SDL_CreateRGBSurface(SDL_HWSURFACE, 320, 240, 32, 0, 0, 0, 0);
}

Carte::~Carte() {
    SDL_FreeSurface(image);
}

void Carte::init() {
    SDL_Rect src;
    SDL_Rect dst;
    Joueur* gpJoueur = gpJeu->getJoueur();
    int zone = gpJeu->getZone();
    
    if (gpJeu->isDehors() || 
    (zone==25 && gpJeu->getMonde()->regionValue(0)==180*16 
    && gpJeu->getMonde()->regionValue(1)==0) ||
    (zone==28 && gpJeu->getMonde()->regionValue(0)==140*16 
    && gpJeu->getMonde()->regionValue(1)==105*16)) {
        SDL_Surface* tmp = IMG_Load("app0:/images/carte/monde.png");
        src.w = 320; src.h = 240; src.x = 0; src.y = 0; dst.y = 0; dst.x = 0;
        SDL_BlitSurface(tmp, &src, image, &dst);
        SDL_FreeSurface(tmp);
        
        //si en téléportation
        if (gpJeu->getKeyboard()->getMode() == 22) {
            gpJeu->getKeyboard()->setTelep(0); return;}
        
        tmp = IMG_Load("app0:/images/carte/link.png");
        SDL_SetColorKey(tmp,SDL_SRCCOLORKEY,SDL_MapRGB(tmp->format,0,0,255));
        src.w = 14; src.h = 12; src.x = 0; src.y = 0; 
        
        //dst.x = gpJoueur->getX()/18; dst.y = gpJoueur->getY()/18;
        //dst.y+=30;
                    
        switch (zone) {
            case 1 : 
                dst.x = ((gpJoueur->getX()/16)*64)/(20*4); 
                dst.y = ((gpJoueur->getY()/16)*48)/(15*4);
                dst.x+=235; dst.y+=176; break;
            case 2 : 
                dst.x = ((gpJoueur->getX()/16)*92)/(20*6); 
                dst.y = ((gpJoueur->getY()/16)*45)/(15*4);
                dst.x+=204; dst.y+=131; break;
            case 3 : 
                dst.x = ((gpJoueur->getX()/16)*60)/(20*4); 
                dst.y = ((gpJoueur->getY()/16)*48)/(15*4);
                dst.x+=231; dst.y+=83; break;
            case 4 : 
                dst.x = ((gpJoueur->getX()/16)*57)/(20*4); 
                dst.y = ((gpJoueur->getY()/16)*45)/(15*4);
                dst.x+=176; dst.y+=87; break;
            case 5 : 
                dst.x = ((gpJoueur->getX()/16)*140)/(20*10); 
                dst.y = ((gpJoueur->getY()/16)*68)/(15*6);
                dst.x+=148; dst.y+=17; break;
            case 6 : 
                dst.x = ((gpJoueur->getX()/16)*89)/(20*6); 
                dst.y = ((gpJoueur->getY()/16)*46)/(15*4);
                dst.x+=147; dst.y+=176; break;
            case 7 : 
                dst.x = ((gpJoueur->getX()/16)*58)/(20*4); 
                dst.y = ((gpJoueur->getY()/16)*48)/(15*4);
                dst.x+=147; dst.y+=130; break;
            case 8 : 
                dst.x = ((gpJoueur->getX()/16)*31)/(20*2); 
                dst.y = ((gpJoueur->getY()/16)*45)/(15*4);
                dst.x+=146; dst.y+=86; break;
            case 9 : 
                dst.x = ((gpJoueur->getX()/16)*55)/(20*4); 
                dst.y = ((gpJoueur->getY()/16)*91)/(15*8);
                dst.x+=92; dst.y+=85; break;
            case 10: 
                dst.x = ((gpJoueur->getX()/16)*61)/(20*4); 
                dst.y = ((gpJoueur->getY()/16)*46)/(15*4);
                dst.x+=86; dst.y+=176; break;
            case 11 : 
                dst.x = ((gpJoueur->getX()/16)*59)/(20*4); 
                dst.y = ((gpJoueur->getY()/16)*47)/(15*4);
                dst.x+=26; dst.y+=176; break;
            case 12 : 
                dst.x = ((gpJoueur->getX()/16)*58)/(20*4); 
                dst.y = ((gpJoueur->getY()/16)*45)/(15*4);
                dst.x+=31; dst.y+=131; break;
            case 13 : 
                dst.x = ((gpJoueur->getX()/16)*56)/(20*4); 
                dst.y = ((gpJoueur->getY()/16)*43)/(15*4);
                dst.x+=35; dst.y+=87; break;
            case 14 : 
                dst.x = ((gpJoueur->getX()/16)*107)/(20*8); 
                dst.y = ((gpJoueur->getY()/16)*67)/(15*6);
                dst.x+=40; dst.y+=17; break;
            case 25 : dst.x=163; dst.y=83; break;
            case 28 : dst.x=133; dst.y=14; break;
        }
                    
        dst.x-=7; dst.y-=6;
        
        SDL_BlitSurface(tmp, &src, image, &dst);
        SDL_FreeSurface(tmp);
    }
    else if (gpJeu->isDonjon()) {
        levely = gpJeu->getMonde()->etage();
        initDonjon();
    }
}

void Carte::initDonjon() {
    SDL_Rect src;
    SDL_Rect dst;
    Joueur* gpJoueur = gpJeu->getJoueur();
    
    SDL_Surface* tmp = IMG_Load("app0:/images/carte/donjon.png");
    src.w = 320; src.h = 240; src.x = 0; src.y = 0; dst.y = 0; dst.x = 0;
    SDL_BlitSurface(tmp, &src, image, &dst);
    SDL_FreeSurface(tmp);
        
    initName();
    initItems();
    if (gpJoueur->getCle(gpJeu->getZone()-15,0)) initEtage();
}

void Carte::initEtage() {
    SDL_Rect src;
    SDL_Rect dst;
    int zone = gpJeu->getZone();
    
    src.h = 16; src.w = 32;src.x = 0;src.y=32;dst.x = 40; dst.y = 96;
    switch (zone-15) {
        case 0 : case 4 : src.h=32; break;
        case 1 : case 6 : case 11 : src.h=48; break;
        case 2 : case 5 : case 7 : case 9 : src.y=16; dst.y-=16; src.h=48; break;
        case 3 : src.h=64; src.y=0; dst.y-=32; break;
        case 8 : case 10 : src.h=80; src.y=0; dst.y-=32; break;
        case 12 : src.h=16; break;
        case 13 : src.h=48; src.y=0; dst.y-=32; break;
    }
    
    SDL_Surface* tmp = IMG_Load("app0:/images/carte/level.png");
    SDL_BlitSurface(tmp, &src, image, &dst);
    SDL_FreeSurface(tmp);
    
    switch (zone-15) {
        case 0 : case 4 : if (levely<-1) levely=0; if (levely>0) levely=-1; break;
        case 1 : case 6 : case 11 : if (levely<-2) levely=0; if (levely>0) levely=-2; break;
        case 2 : case 5 : case 7 : case 9 : 
            if (levely<-1) levely=1; if (levely>1) levely=-1; break;
        case 3 : if (levely<-1) levely=2; if (levely>2) levely=-1; break;
        case 8 : case 10 : if (levely<-2) levely=2; if (levely>2) levely=-2; break;
        case 12 : levely=0; break;
        case 13 : if (levely<0) levely=2; if (levely>2) levely=0; break;
    }
    src.h = 16; src.w = 32;src.x = 0;src.y=32-levely*16;dst.x = 40; dst.y = 96-levely*16;
    
    tmp = IMG_Load("app0:/images/carte/level2.png");
    SDL_BlitSurface(tmp, &src, image, &dst);
    SDL_FreeSurface(tmp);
    
    //étage de link
    tmp = IMG_Load("app0:/images/carte/link.png");
    SDL_SetColorKey(tmp,SDL_SRCCOLORKEY,SDL_MapRGB(tmp->format,0,0,255));
    src.h = 12; src.w = 14;src.x = 0;src.y=0;
    dst.x = 25; dst.y = 98-gpJeu->getMonde()->etage()*16;
    SDL_BlitSurface(tmp, &src, image, &dst);
    SDL_FreeSurface(tmp);
    
    initCarte();
}

void Carte::initCarte() {
    SDL_Rect src;
    SDL_Rect dst;
    int zone = gpJeu->getZone();
    Joueur* gpJoueur = gpJeu->getJoueur();
    
    int dx=0;
    int dy=0;
    
    ostringstream os;
    ostringstream os2;
    
    switch (zone-15) {
        case 0 : case 4 : case 6 : dx=2; dy=3; break;
        case 1 : dx=2; dy=2; if (levely==-1) dy=3; break;
        case 2 : dx=2; dy=1; if (levely==1) {dx=4; dy=2;} if (levely==-1) {dx=1; dy=2;} break;
        case 3 : dx=2; dy=3; if (levely>0) {dx=3; dy=4;} break;
        case 5 : dx=1; dy=2; if (levely == 1) {dx=4; dy=3;} if (levely == -1) dx=2; break;
        case 7 : dx=3; dy=2; if (levely==1) dy=3; break;
        case 8 : dx=2; dy=3;
            if (levely==2) {dx=4; dy=5;} 
            if (levely==1) {dx=3; dy=4;} 
            break;
        case 9 : dx=2; dy=3; break;
        case 10 : dx=3; dy=3; break;
        case 11 : dx=1; dy=2; if (levely<0) {dx=4; dy=4;} break;
        case 12 : dy=1; break;
        case 13 : dx=2; dy=3; break;
    }
    
    
    if (zone < 23) os << (zone-15+1); else os << (zone-15+2);
    os2 << levely;
    
    SDL_Surface* tmp = IMG_Load(("app0:/images/carte/d"+os.str()+"e"+os2.str()+".png").c_str());
    src.x=0; src.y=0; src.w=tmp->w; src.h=tmp->h; dst.y = 48+dy*16; dst.x = 128+dx*16;
    SDL_BlitSurface(tmp, &src, image, &dst);
    SDL_FreeSurface(tmp);
    
    
    if ((zone-15)!=11 || levely==0) {
    
    tmp = IMG_Load(("app0:/images/carte/nd"+os.str()+"e"+os2.str()+".png").c_str());
    
    int di=0;
    int dj=0;
    
    switch (zone-15) {
        case 0 : if (levely==-1)di=5; break;
        case 1 : if (levely==-1){di=5;dj=1;} if (levely==-2)di=10; break;
        case 2 : if (levely==1)dj=1;if (levely==0)di=1; if (levely==-1){di=6;dj=1;} break;
        case 3 : 
            if (levely==2)dj=1;
            if (levely==1){di=3; dj=1;} 
            if (levely==0) di=6;
            if (levely==-1) di=11; break;
        case 4 : if (levely==-1)di=5; break;
        case 5 : if (levely==1)dj=1; if (levely==0)di=1; if (levely==-1)di=8; break;
        case 6 : if (levely==-1)di=5; if (levely==-2)di=10; break;
        case 7 : if (levely==1)dj=1; if (levely==0)di=4; if (levely==-1)di=8; break;
        case 8 : if (levely==2) dj=4; if (levely==0) di=3; 
            if (levely==-1) di=8; if (levely==-2) di=13; break;
        case 9 : if (levely==0)di=5; if (levely==-1)di=10; break;
        case 10 : if (levely==1) di=4; if (levely==0) dj=4; 
            if (levely==-1) {di=4;dj=4;} if (levely==-2) {di=8;dj=4;} break;
        case 13 : if (levely==1) di=5; if (levely==0) dj=5; break;
    }
    
    for (int i = 0; i < tmp->w/16; i++) 
        for (int j = 0; j < tmp->h/16; j++) 
            if (!gpJoueur->getExplore(zone-15,i+di,j+dj)) {
                src.x=i*16; src.y=j*16; src.w=16; src.h=16; 
                dst.x = 128+(dx+i)*16; dst.y = 48+(dy+j)*16;
                SDL_BlitSurface(tmp, &src, image, &dst);
            }
        
    SDL_FreeSurface(tmp);
    
    if (gpJoueur->getCle(zone-15,1)) initBoussole();
    
    //étage de link
    if (gpJeu->getMonde()->etage() == levely && ((zone-15)!=11 || levely==0)) {
        tmp = IMG_Load("app0:/images/carte/link.png");
        SDL_SetColorKey(tmp,SDL_SRCCOLORKEY,SDL_MapRGB(tmp->format,0,0,255));
        src.h = 12; src.w = 14;src.x = 0;src.y=0;
        dst.x = 128+(dx-di)*16+(gpJoueur->getX()/20)-6; 
        dst.y = 48+(dy-dj)*16+(gpJoueur->getY()/15)-7;
        SDL_BlitSurface(tmp, &src, image, &dst);
        SDL_FreeSurface(tmp);
    }
    
    }
}

void Carte::initBoussole() {
    SDL_Rect src;
    SDL_Rect dst;
    int zone = gpJeu->getZone();
    Joueur* gpJoueur = gpJeu->getJoueur();
    
    SDL_Surface* tmp = IMG_Load("app0:/images/carte/boussole.png");
    SDL_SetColorKey(tmp,SDL_SRCCOLORKEY,SDL_MapRGB(tmp->format,0,0,255));
    
    switch (zone-15) {
        case 0 :
            if (levely==0) {
                if (!gpJoueur->getCoeur(0)) {
                    src.h = 8; src.w = 8;src.x = 2;src.y=0;
                    dst.x = 127+16*6+5; dst.y = 48+16*4+4;
                    SDL_BlitSurface(tmp, &src, image, &dst);
                }
                src.h = 2; src.w = 2;src.x = 0;src.y=0;
                if(!gpJoueur->getCoffre(0,10)) {
                    dst.x = 127+16*3+8; dst.y = 48+16*3+7;
                    SDL_BlitSurface(tmp, &src, image, &dst);
                }
                if (!gpJoueur->hasCristal(0)) {
                    dst.x = 127+16*6+8; dst.y = 48+16*3+7;
                    SDL_BlitSurface(tmp, &src, image, &dst);
                }
            }
            if (levely==-1) {
                src.h = 2; src.w = 2;src.x = 0;src.y=0;
                if(!gpJoueur->getCle(0,2)) {
                    dst.x = 127+16*5+8; dst.y = 48+16*4+7;
                    SDL_BlitSurface(tmp, &src, image, &dst);
                }
                if(!gpJoueur->getCoffre(0,11)) {
                    dst.x = 127+16*6+8; dst.y = 48+16*3+7;
                    SDL_BlitSurface(tmp, &src, image, &dst);
                }
                if (!gpJoueur->getEpee()) {
                    dst.x = 127+16*3+8; dst.y = 48+16*3+7;
                    SDL_BlitSurface(tmp, &src, image, &dst);
                }
            }
            if (!gpJoueur->getCoeur(0)) {
                src.h = 8; src.w = 8;src.x = 2;src.y=0;
                dst.x = 24+48+4; dst.y = 96+4;
                SDL_BlitSurface(tmp, &src, image, &dst);
            }
            break;
        case 1 :
            if (levely==-1) {
                src.h = 2; src.w = 2;src.x = 0;src.y=0;
                if(!gpJoueur->getCoffre(1,25)) {
                    dst.x = 127+16*5+8; dst.y = 48+16*3+7;
                    SDL_BlitSurface(tmp, &src, image, &dst);
                }
                if(!gpJoueur->getCoffre(1,26)) {
                    dst.x = 127+16*6+8; dst.y = 48+16*3+7;
                    SDL_BlitSurface(tmp, &src, image, &dst);
                }
                if(!gpJoueur->getCoffre(1,27)) {
                    dst.x = 127+16*6+8; dst.y = 48+16*6+7;
                    SDL_BlitSurface(tmp, &src, image, &dst);
                }
                if(!gpJoueur->getCoffre(1,28)) {
                    dst.x = 127+16*3+8; dst.y = 48+16*3+7;
                    SDL_BlitSurface(tmp, &src, image, &dst);
                }
            }
            if (levely==-2) {
                if (!gpJoueur->getCoeur(1)) {
                    src.h = 8; src.w = 8;src.x = 2;src.y=0;
                    dst.x = 127+16*4+5; dst.y = 48+16*7+4;
                    SDL_BlitSurface(tmp, &src, image, &dst);
                }
                src.h = 2; src.w = 2;src.x = 0;src.y=0;
                if(!gpJoueur->getCle(1,2)) {
                    dst.x = 127+16*4+8; dst.y = 48+16*4;//+7;
                    SDL_BlitSurface(tmp, &src, image, &dst);
                }
                if (!gpJoueur->hasObjet(O_SAC_BOMBES)) {
                    dst.x = 127+16*4+8; dst.y = 48+16*6+7;
                    SDL_BlitSurface(tmp, &src, image, &dst);
                }
                if(!gpJoueur->getCoffre(1,29)) {
                    dst.x = 127+16*6+8; dst.y = 48+16*6+7;
                    SDL_BlitSurface(tmp, &src, image, &dst);
                }
            }
            if (!gpJoueur->getCoeur(1)) {
                src.h = 8; src.w = 8;src.x = 2;src.y=0;
                dst.x = 24+48+4; dst.y = 96+4+16*2;
                SDL_BlitSurface(tmp, &src, image, &dst);
            }
            break;
        case 2 :
            if (levely==1) {
                if (!gpJoueur->getCoeur(2)) {
                    src.h = 8; src.w = 8;src.x = 2;src.y=0;
                    dst.x = 127+16*4+5; dst.y = 48+16*3+4;
                    SDL_BlitSurface(tmp, &src, image, &dst);
                }
                src.h = 2; src.w = 2;src.x = 0;src.y=0;
                if (!gpJoueur->hasCristal(1)) {
                    dst.x = 127+16*4+8; dst.y = 48+16*2+7;
                    SDL_BlitSurface(tmp, &src, image, &dst);
                }
            }
            if (levely==0) {
                src.h = 2; src.w = 2;src.x = 0;src.y=0;
                if(!gpJoueur->hasObjet(O_GANTS)) {
                    dst.x = 127+16*4+8; dst.y = 48+16+7;
                    SDL_BlitSurface(tmp, &src, image, &dst);
                }
                if(!gpJoueur->getCoffre(2,17)) {
                    dst.x = 127+16*3+8; dst.y = 48+16*4+7;
                    SDL_BlitSurface(tmp, &src, image, &dst);
                }
                if(!gpJoueur->getCoffre(2,18)) {
                    dst.x = 127+16*5+8; dst.y = 48+16*4+7;
                    SDL_BlitSurface(tmp, &src, image, &dst);
                }
                if(!gpJoueur->getCle(2,2)) {
                    dst.x = 127+16*3+8; dst.y = 48+16*5+7;
                    SDL_BlitSurface(tmp, &src, image, &dst);
                }
            }
            if (levely==-1) {
                src.h = 2; src.w = 2;src.x = 0;src.y=0;
                if(!gpJoueur->getCoffre(2,19)) {
                    dst.x = 127+16*2+8; dst.y = 48+16*7+7;
                    SDL_BlitSurface(tmp, &src, image, &dst);
                }
                if(!gpJoueur->getCoffre(2,20)) {
                    dst.x = 127+16*4+8; dst.y = 48+16*4+7;
                    SDL_BlitSurface(tmp, &src, image, &dst);
                }
            }
            if (!gpJoueur->getCoeur(2)) {
                src.h = 8; src.w = 8;src.x = 2;src.y=0;
                dst.x = 24+48+4; dst.y = 96+4-16;
                SDL_BlitSurface(tmp, &src, image, &dst);
            }
            break;
        case 3 :
            if (levely==0) {
                src.h = 2; src.w = 2;src.x = 0;src.y=0;
                if(!gpJoueur->getCoffre(3,27)) {
                    dst.x = 127+16*6+8; dst.y = 48+16*7+7;
                    SDL_BlitSurface(tmp, &src, image, &dst);
                }
            }
            if (levely==-1) {
                if (!gpJoueur->getCoeur(3)) {
                    src.h = 8; src.w = 8;src.x = 2;src.y=0;
                    dst.x = 127+16*4+5; dst.y = 48+16*4+4;
                    SDL_BlitSurface(tmp, &src, image, &dst);
                }
                src.h = 2; src.w = 2;src.x = 0;src.y=0;
                if (gpJoueur->getAvancement()<65) {
                    dst.x = 127+16*4+8; dst.y = 48+16*3+7;
                    SDL_BlitSurface(tmp, &src, image, &dst);
                }
                if(!gpJoueur->getCoffre(3,19)) {
                    dst.x = 127+16*2+8; dst.y = 48+16*5+7;
                    SDL_BlitSurface(tmp, &src, image, &dst);
                }
                if(!gpJoueur->getCoffre(3,28)) {
                    dst.x = 127+16*2+8; dst.y = 48+16*6+7;
                    SDL_BlitSurface(tmp, &src, image, &dst);
                }
                if(!gpJoueur->getCoffre(3,29)) {
                    dst.x = 127+16*5+8; dst.y = 48+16*4+7+4;
                    SDL_BlitSurface(tmp, &src, image, &dst);
                }
                if(!gpJoueur->getCoffre(3,30)) {
                    dst.x = 127+16*5+8; dst.y = 48+16*6+7+4;
                    SDL_BlitSurface(tmp, &src, image, &dst);
                }
                if(!gpJoueur->getCle(3,2)) {
                    dst.x = 127+16*4+8; dst.y = 48+16*7+7;
                    SDL_BlitSurface(tmp, &src, image, &dst);
                }
            }
            if (!gpJoueur->getCoeur(3)) {
                src.h = 8; src.w = 8;src.x = 2;src.y=0;
                dst.x = 24+48+4; dst.y = 96+4+16;
                SDL_BlitSurface(tmp, &src, image, &dst);
            }
            break;
        case 4 :
            if (levely==0) {
                if (!gpJoueur->getCoeur(4)) {
                    src.h = 8; src.w = 8;src.x = 2;src.y=0;
                    dst.x = 127+16*5+5+8; dst.y = 48+16*4+4+8;
                    SDL_BlitSurface(tmp, &src, image, &dst);
                }
                src.h = 2; src.w = 2;src.x = 0;src.y=0;
                if (!gpJoueur->hasCristal(2)) {
                    dst.x = 127+16*6+8; dst.y = 48+16*3+7;
                    SDL_BlitSurface(tmp, &src, image, &dst);
                }
            }
            if (levely==-1) {
                src.h = 2; src.w = 2;src.x = 0;src.y=0;
                if(!gpJoueur->hasObjet(O_MARTEAU)) {
                    dst.x = 127+16*5+8; dst.y = 48+16*4+7-5;
                    SDL_BlitSurface(tmp, &src, image, &dst);
                }
                if(!gpJoueur->getCoffre(4,18)) {
                    dst.x = 127+16*2+8; dst.y = 48+16*7+7;
                    SDL_BlitSurface(tmp, &src, image, &dst);
                }
                if(!gpJoueur->getCoffre(4,16)) {
                    dst.x = 127+16*5+8; dst.y = 48+16*4+7+5;
                    SDL_BlitSurface(tmp, &src, image, &dst);
                }
                if(!gpJoueur->getCoffre(4,17)) {
                    dst.x = 127+16*6+8; dst.y = 48+16*7+7;
                    SDL_BlitSurface(tmp, &src, image, &dst);
                }
                if(!gpJoueur->getCle(4,2)) {
                    dst.x = 127+16*6+8+5; dst.y = 48+16*6+7;
                    SDL_BlitSurface(tmp, &src, image, &dst);
                }
            }
            if (!gpJoueur->getCoeur(4)) {
                src.h = 8; src.w = 8;src.x = 2;src.y=0;
                dst.x = 24+48+4; dst.y = 96+4;
                SDL_BlitSurface(tmp, &src, image, &dst);
            }
            break;
        case 5 :
            if (levely==1) {
                if (!gpJoueur->getCoeur(5)) {
                    src.h = 8; src.w = 8;src.x = 2;src.y=0;
                    dst.x = 127+16*4+5; dst.y = 48+16*4+4;
                    SDL_BlitSurface(tmp, &src, image, &dst);
                }
                src.h = 2; src.w = 2;src.x = 0;src.y=0;
                if (!gpJoueur->hasObjet(O_GRAPPIN)) {
                    dst.x = 127+16*4+8; dst.y = 48+16*3+7;
                    SDL_BlitSurface(tmp, &src, image, &dst);
                }
                if(!gpJoueur->getCoffre(5,1)) {
                    dst.x = 127+16*4+8; dst.y = 48+16*5+7;
                    SDL_BlitSurface(tmp, &src, image, &dst);
                }
                if(!gpJoueur->getCoffre(5,2)) {
                    dst.x = 127+16*4+8; dst.y = 48+16*7+7;
                    SDL_BlitSurface(tmp, &src, image, &dst);
                }
            }
            if (levely==0) {
                src.h = 2; src.w = 2;src.x = 0;src.y=0;
                if(!gpJoueur->getCle(5,2)) {
                    dst.x = 127+16*4+8+5; dst.y = 48+16*5-4+7;
                    SDL_BlitSurface(tmp, &src, image, &dst);
                }
                if(!gpJoueur->getCoffre(5,3)) {
                    dst.x = 127+16*1+8; dst.y = 48+16*2+7;
                    SDL_BlitSurface(tmp, &src, image, &dst);
                }
                if(!gpJoueur->getCoffre(5,4)) {
                    dst.x = 127+16*3+8; dst.y = 48+16*5+7;
                    SDL_BlitSurface(tmp, &src, image, &dst);
                }
                if(!gpJoueur->getCoffre(5,5)) {
                    dst.x = 127+16*3+8; dst.y = 48+16*6+7;
                    SDL_BlitSurface(tmp, &src, image, &dst);
                }
                if(!gpJoueur->getCoffre(5,6)) {
                    dst.x = 127+16*4+8; dst.y = 48+16*6+7;
                    SDL_BlitSurface(tmp, &src, image, &dst);
                }
                if(!gpJoueur->getCoffre(5,7)) {
                    dst.x = 127+16*5+8; dst.y = 48+16*5+7;
                    SDL_BlitSurface(tmp, &src, image, &dst);
                }
                if(!gpJoueur->getCoffre(5,8)) {
                    dst.x = 127+16*5+8; dst.y = 48+16*6+7;
                    SDL_BlitSurface(tmp, &src, image, &dst);
                }
                if(!gpJoueur->getCoffre(5,9)) {
                    dst.x = 127+16*6+8; dst.y = 48+16*4+7;
                    SDL_BlitSurface(tmp, &src, image, &dst);
                }
                if(!gpJoueur->getCoffre(5,10)) {
                    dst.x = 127+16*6+8; dst.y = 48+16*7+7;
                    SDL_BlitSurface(tmp, &src, image, &dst);
                }
            }
            if (levely==-1) {
                src.h = 2; src.w = 2;src.x = 0;src.y=0;
                if(!gpJoueur->getCoffre(5,11)) {
                    dst.x = 127+16*2+8+8; dst.y = 48+16*2+8+7;
                    SDL_BlitSurface(tmp, &src, image, &dst);
                }
                if(!gpJoueur->getCoffre(5,12)) {
                    dst.x = 127+16*2+8+8; dst.y = 48+16*6+8+7;
                    SDL_BlitSurface(tmp, &src, image, &dst);
                }
                if(!gpJoueur->getCoffre(5,13)) {
                    dst.x = 127+16*5+8+8; dst.y = 48+16*2+8+7;
                    SDL_BlitSurface(tmp, &src, image, &dst);
                }
                if(!gpJoueur->getCoffre(5,14)) {
                    dst.x = 127+16*5+8+8; dst.y = 48+16*6+8+7;
                    SDL_BlitSurface(tmp, &src, image, &dst);
                }
            }
            if (!gpJoueur->getCoeur(5)) {
                src.h = 8; src.w = 8;src.x = 2;src.y=0;
                dst.x = 24+48+4; dst.y = 96+4-16;
                SDL_BlitSurface(tmp, &src, image, &dst);
            }
            break;
        case 6 :
            if (levely==0) {
                src.h = 2; src.w = 2;src.x = 0;src.y=0;
                if(!gpJoueur->getCle(6,2)) {
                    dst.x = 127+16*4+8; dst.y = 48+16*3+7;
                    SDL_BlitSurface(tmp, &src, image, &dst);
                }
                if(!gpJoueur->getCoffre(6,18)) {
                    dst.x = 127+16*6+8; dst.y = 48+16*3+7-4;
                    SDL_BlitSurface(tmp, &src, image, &dst);
                }
            }
            if (levely==-1) {
                src.h = 2; src.w = 2;src.x = 0;src.y=0;
                if(!gpJoueur->getCoffre(6,19)) {
                    dst.x = 127+16*2+8-4; dst.y = 48+16*3+7-4;
                    SDL_BlitSurface(tmp, &src, image, &dst);
                }
                if(!gpJoueur->getCoffre(6,21)) {
                    dst.x = 127+16*4+8+8; dst.y = 48+16*4+7;
                    SDL_BlitSurface(tmp, &src, image, &dst);
                }
                if(!gpJoueur->getCoffre(6,22)) {
                    dst.x = 127+16*4+8+8; dst.y = 48+16*5+7;
                    SDL_BlitSurface(tmp, &src, image, &dst);
                }
            }
            if (levely==-2) {
                if (!gpJoueur->getCoeur(6)) {
                    src.h = 8; src.w = 8;src.x = 2;src.y=0;
                    dst.x = 127+16*4+5; dst.y = 48+16*4+4;
                    SDL_BlitSurface(tmp, &src, image, &dst);
                }
                src.h = 2; src.w = 2;src.x = 0;src.y=0;
                if (!gpJoueur->hasCristal(3)) {
                    dst.x = 127+16*4+8; dst.y = 48+16*3+7;
                    SDL_BlitSurface(tmp, &src, image, &dst);
                }
                if (!gpJoueur->hasObjet(O_PALMES)) {
                    dst.x = 127+16*4+8+4; dst.y = 48+16*6+7;
                    SDL_BlitSurface(tmp, &src, image, &dst);
                }
                if(!gpJoueur->getCoffre(6,23)) {
                    dst.x = 127+16*3+8; dst.y = 48+16*4+7;
                    SDL_BlitSurface(tmp, &src, image, &dst);
                }
                if(!gpJoueur->getCoffre(6,24)) {
                    dst.x = 127+16*4+8; dst.y = 48+16*6+7-4;
                    SDL_BlitSurface(tmp, &src, image, &dst);
                }
            }
            if (!gpJoueur->getCoeur(6)) {
                src.h = 8; src.w = 8;src.x = 2;src.y=0;
                dst.x = 24+48+4; dst.y = 96+4+16*2;
                SDL_BlitSurface(tmp, &src, image, &dst);
            }
            break;
        case 7 :
            if (levely==1) {
                src.h = 2; src.w = 2;src.x = 0;src.y=0;
                if(!gpJoueur->getCle(7,2)) {
                    dst.x = 127+16*5+8; dst.y = 48+16*3+7;
                    SDL_BlitSurface(tmp, &src, image, &dst);
                }
                if(!gpJoueur->getCoffre(7,15)) {
                    dst.x = 127+16*3+8; dst.y = 48+16*3+7;
                    SDL_BlitSurface(tmp, &src, image, &dst);
                }
            }
            if (levely==0) {
                if (!gpJoueur->getCoeur(7)) {
                    src.h = 8; src.w = 8;src.x = 2;src.y=0;
                    dst.x = 127+16*3+5; dst.y = 48+16*3+4;
                    SDL_BlitSurface(tmp, &src, image, &dst);
                }
                src.h = 2; src.w = 2;src.x = 0;src.y=0;
                if (!gpJoueur->hasCristal(4)) {
                    dst.x = 127+16*3+8; dst.y = 48+16*2+7;
                    SDL_BlitSurface(tmp, &src, image, &dst);
                }
                if (!gpJoueur->getCoffre(7,21)) {
                    dst.x = 127+16*4+8; dst.y = 48+16*2+7;
                    SDL_BlitSurface(tmp, &src, image, &dst);
                }
                if(!gpJoueur->getCoffre(7,16)) {
                    dst.x = 127+16*3+8-4; dst.y = 48+16*5+7-4;
                    SDL_BlitSurface(tmp, &src, image, &dst);
                }
                if(!gpJoueur->getCoffre(7,17)) {
                    dst.x = 127+16*4+8+8; dst.y = 48+16*4+7-4;
                    SDL_BlitSurface(tmp, &src, image, &dst);
                }
            }
            if (levely==-1) {
                src.h = 2; src.w = 2;src.x = 0;src.y=0;
                if(!gpJoueur->getCoffre(7,19)) {
                    dst.x = 127+16*3+8; dst.y = 48+16*7+7;
                    SDL_BlitSurface(tmp, &src, image, &dst);
                }
            }
            if (!gpJoueur->getCoeur(7)) {
                src.h = 8; src.w = 8;src.x = 2;src.y=0;
                dst.x = 24+48+4; dst.y = 96+4;
                SDL_BlitSurface(tmp, &src, image, &dst);
            }
            break;
        case 8 :
            if (levely==0) {
                src.h = 2; src.w = 2;src.x = 0;src.y=0;
                if(!gpJoueur->getCoffre(8,15)) {
                    dst.x = 127+16*4+8; dst.y = 48+16*4+7;
                    SDL_BlitSurface(tmp, &src, image, &dst);
                }
                if(!gpJoueur->getCoffre(8,16)) {
                    dst.x = 127+16*6+8; dst.y = 48+16*3+7;
                    SDL_BlitSurface(tmp, &src, image, &dst);
                }
            }
            if (levely==-1) {
                if (!gpJoueur->getCoeur(8)) {
                    src.h = 8; src.w = 8;src.x = 2;src.y=0;
                    dst.x = 127+16*4+5; dst.y = 48+16*5+4;
                    SDL_BlitSurface(tmp, &src, image, &dst);
                }
                src.h = 2; src.w = 2;src.x = 0;src.y=0;
                if (!gpJoueur->hasObjet(O_BFEU)) {
                    dst.x = 127+16*2+8; dst.y = 48+16*6+7;
                    SDL_BlitSurface(tmp, &src, image, &dst);
                }
                if(!gpJoueur->getCoffre(8,19)) {
                    dst.x = 127+16*4+8; dst.y = 48+16*4+7;
                    SDL_BlitSurface(tmp, &src, image, &dst);
                }
            }
            if (levely==-2) {
                src.h = 2; src.w = 2;src.x = 0;src.y=0;
                if(!gpJoueur->getCle(8,2)) {
                    dst.x = 127+16*2+8; dst.y = 48+16*7+7;
                    SDL_BlitSurface(tmp, &src, image, &dst);
                }
                if(!gpJoueur->getCoffre(8,18)) {
                    dst.x = 127+16*6+8; dst.y = 48+16*7+7;
                    SDL_BlitSurface(tmp, &src, image, &dst);
                }
            }
            if (!gpJoueur->getCoeur(8)) {
                src.h = 8; src.w = 8;src.x = 2;src.y=0;
                dst.x = 24+48+4; dst.y = 96+4+16;
                SDL_BlitSurface(tmp, &src, image, &dst);
            }
            break;
        case 9 :
            if (levely==1) {
                if (!gpJoueur->getCoeur(9)) {
                    src.h = 8; src.w = 8;src.x = 2;src.y=0;
                    dst.x = 127+16*4+5; dst.y = 48+16*4+4;
                    SDL_BlitSurface(tmp, &src, image, &dst);
                }
                src.h = 2; src.w = 2;src.x = 0;src.y=0;
                if (!gpJoueur->hasObjet(O_BGLACE)) {
                    dst.x = 127+16*4+8; dst.y = 48+16*6+7;
                    SDL_BlitSurface(tmp, &src, image, &dst);
                }
                if(!gpJoueur->getCoffre(9,20)) {
                    dst.x = 127+16*6+8; dst.y = 48+16*7+7;
                    SDL_BlitSurface(tmp, &src, image, &dst);
                }
                if(!gpJoueur->getCoffre(9,24)) {
                    dst.x = 127+16*4+8; dst.y = 48+16*3+7;
                    SDL_BlitSurface(tmp, &src, image, &dst);
                }
            }
            if (levely==0) {
                src.h = 2; src.w = 2;src.x = 0;src.y=0;
                if(!gpJoueur->getCle(9,2)) {
                    dst.x = 127+16*4+8; dst.y = 48+16*3+7;
                    SDL_BlitSurface(tmp, &src, image, &dst);
                }
                if(!gpJoueur->getCoffre(9,22)) {
                    dst.x = 127+16*5+8; dst.y = 48+16*3+7-4;
                    SDL_BlitSurface(tmp, &src, image, &dst);
                }
                if(!gpJoueur->getCoffre(9,23)) {
                    dst.x = 127+16*2+8; dst.y = 48+16*7+7;
                    SDL_BlitSurface(tmp, &src, image, &dst);
                }
            }
            if (!gpJoueur->getCoeur(9)) {
                src.h = 8; src.w = 8;src.x = 2;src.y=0;
                dst.x = 24+48+4; dst.y = 96+4-16;
                SDL_BlitSurface(tmp, &src, image, &dst);
            }
            break;
        case 10 :
            if (levely==1) {
                src.h = 2; src.w = 2;src.x = 0;src.y=0;
                if (gpJoueur->getBouclier()<3) {
                    dst.x = 127+16*5+8; dst.y = 48+16*4+7;
                    SDL_BlitSurface(tmp, &src, image, &dst);
                }
                if(!gpJoueur->getCoffre(10,15)) {
                    dst.x = 127+16*5+8-3; dst.y = 48+16*6+7;
                    SDL_BlitSurface(tmp, &src, image, &dst);
                }
                if(!gpJoueur->getCoffre(10,16)) {
                    dst.x = 127+16*5+8+3; dst.y = 48+16*6+7;
                    SDL_BlitSurface(tmp, &src, image, &dst);
                }
            }
            if (levely==0) {
                if (!gpJoueur->getCoeur(10)) {
                    src.h = 8; src.w = 8;src.x = 2;src.y=0;
                    dst.x = 127+16*5+5; dst.y = 48+16*4+4;
                    SDL_BlitSurface(tmp, &src, image, &dst);
                }
            }
            if (levely==-2) {
                src.h = 2; src.w = 2;src.x = 0;src.y=0;
                if(!gpJoueur->getCle(10,2)) {
                    dst.x = 127+16*5+8; dst.y = 48+16*6+7;
                    SDL_BlitSurface(tmp, &src, image, &dst);
                }
                if(!gpJoueur->getCoffre(10,18)) {
                    dst.x = 127+16*4+8; dst.y = 48+16*5+7;
                    SDL_BlitSurface(tmp, &src, image, &dst);
                }
            }
            if (!gpJoueur->getCoeur(10)) {
                src.h = 8; src.w = 8;src.x = 2;src.y=0;
                dst.x = 24+48+4; dst.y = 96+4;
                SDL_BlitSurface(tmp, &src, image, &dst);
            }
            break;
        case 11 :
            if (levely==0) {
                src.h = 2; src.w = 2;src.x = 0;src.y=0;
                if(!gpJoueur->getCoffre(11,25)) {
                    dst.x = 127+16*4+8; dst.y = 48+16*6+7;
                    SDL_BlitSurface(tmp, &src, image, &dst);
                }
            }
            if (!gpJoueur->getCoeur(11)) {
                src.h = 8; src.w = 8;src.x = 2;src.y=0;
                dst.x = 24+48+4; dst.y = 96+4+32;
                SDL_BlitSurface(tmp, &src, image, &dst);
            }
            break;
        case 12 :
            if (!gpJoueur->getCoeur(12)) {
                src.h = 8; src.w = 8;src.x = 2;src.y=0;
                dst.x = 127+16*1+5; dst.y = 48+16*4+4;
                SDL_BlitSurface(tmp, &src, image, &dst);
            }
            src.h = 2; src.w = 2;src.x = 0;src.y=0;
            if (gpJoueur->hasObjet(O_ARC)<5) {
                dst.x = 127+16*1+8; dst.y = 48+16*3+7;
                SDL_BlitSurface(tmp, &src, image, &dst);
            }
            if(!gpJoueur->getCle(12,2)) {
                dst.x = 127+8; dst.y = 48+16*1+7;
                SDL_BlitSurface(tmp, &src, image, &dst);
            }
            if(!gpJoueur->getCoffre(12,15)) {
                dst.x = 127+16*7+8; dst.y = 48+16*6+7;
                SDL_BlitSurface(tmp, &src, image, &dst);
            }
            if (!gpJoueur->getCoeur(12)) {
                src.h = 8; src.w = 8;src.x = 2;src.y=0;
                dst.x = 24+48+4; dst.y = 96+4;
                SDL_BlitSurface(tmp, &src, image, &dst);
            }
            break;
        case 13 :
            if (levely==1) {
                src.h = 2; src.w = 2;src.x = 0;src.y=0;
                if(!gpJoueur->getCle(13,2)) {
                    dst.x = 127+16*4+8; dst.y = 48+16*5+7;
                    SDL_BlitSurface(tmp, &src, image, &dst);
                }
                if(!gpJoueur->getCoffre(13,9)) {
                    dst.x = 127+16*6+8; dst.y = 48+16*5+7;
                    SDL_BlitSurface(tmp, &src, image, &dst);
                }
            }
            if (levely==0) {
                src.h = 8; src.w = 8;src.x = 2;src.y=0;
                dst.x = 127+16*4+5; dst.y = 48+16*5+4;
                SDL_BlitSurface(tmp, &src, image, &dst);
            }
            src.h = 8; src.w = 8;src.x = 2;src.y=0;
            dst.x = 24+48+4; dst.y = 96+4;
            SDL_BlitSurface(tmp, &src, image, &dst);
            break;
    }
    SDL_FreeSurface(tmp);
}

void Carte::initItems() {
    SDL_Rect src;
    SDL_Rect dst;
    Joueur* gpJoueur = gpJeu->getJoueur();
    if (gpJoueur->getCle(gpJeu->getZone()-15,0)) { //carte
        src.h = 16; src.w = 16;src.x = 48;src.y=0;dst.x = 28; dst.y = 192;
        SDL_BlitSurface(gpJeu->getImageObjets(), &src, image, &dst);
    }
    if (gpJoueur->getCle(gpJeu->getZone()-15,1)) { //boussole
        src.h = 16; src.w = 16;src.x = 96;src.y=0;dst.x = 48; dst.y = 192;
        SDL_BlitSurface(gpJeu->getImageObjets(), &src, image, &dst);
    }
    if (gpJoueur->getCle(gpJeu->getZone()-15,2)) { //cle boss
        src.h = 16; src.w = 16;src.x = 16;src.y=0;dst.x = 68; dst.y = 192;
        SDL_BlitSurface(gpJeu->getImageObjets(), &src, image, &dst);
    }
}

void Carte::initName() {
    int zone = gpJeu->getZone();
    //message
    switch (zone-15) {
        case 0 : gpJeu->affiche(image, "Faron Temple",128,25); break;
        case 1 : gpJeu->affiche(image, "Ruto Mine",128,25); break;
        case 2 : gpJeu->affiche(image, "Ordona Temple",128,25); break;
        case 3 : gpJeu->affiche(image, "Haunted Mansion",128,25); break;
        case 4 : gpJeu->affiche(image, "Eldin Temple",128,25); break;
        case 5 : gpJeu->affiche(image, "Monsters' Colosseum",128,25); break;
        case 6 : gpJeu->affiche(image, "Lanayru Temple",128,25); break;
        case 7 : gpJeu->affiche(image, "Zelda Temple",128,25); break;
        case 8 : gpJeu->affiche(image, "Din Temple",128,25); break;
        case 9 : gpJeu->affiche(image, "Nayru Temple",128,25); break;
        case 10 : gpJeu->affiche(image, "Farore Temple",128,25); break;
        case 11 : gpJeu->affiche(image, "Xanto Temple",128,25); break;
        case 12 : gpJeu->affiche(image, "Secret Temple",128,25); break;
        case 13 : gpJeu->affiche(image, "Destiny Tower",128,25); break;
    }
}

void Carte::moveLevel(Direction dir) {
    if (!gpJeu->isDonjon()) return;
    if (!gpJeu->getJoueur()->getCle(gpJeu->getZone()-15,0)) return;
    if (gpJeu->getZone()==27) return;
    int old = levely;
    if (dir == N) levely++;
    if (dir == S) levely--;
    initDonjon();
    if (old != levely) gpJeu->getAudio()->playSound(3);
}

void Carte::draw(SDL_Surface* gpScreen) {
    SDL_Rect src;
    SDL_Rect dst;
    
    src.w = 320; src.h = 240; src.x = 0; src.y = 0; dst.y = 0; dst.x = 0;
    
    SDL_BlitSurface(image, &src, gpScreen, &dst);
    
    if (gpJeu->getKeyboard()->getMode() == 22) { //si en téléportation
        
        
        
        SDL_Surface* tmp = IMG_Load("app0:/images/carte/link.png");
        SDL_SetColorKey(tmp,SDL_SRCCOLORKEY,SDL_MapRGB(tmp->format,0,0,255));
        src.w = 14; src.h = 12; src.x = 0; src.y = 0; 
        
        int tel = gpJeu->getKeyboard()->getTelep();
        
        if (tel != 0) gpJeu->getTexte()->affiche(gpScreen, "1", 45, 32);
        else {dst.x=41; dst.y=33;}
        if (tel != 1) gpJeu->getTexte()->affiche(gpScreen, "2", 159, 20);
        else {dst.x=155; dst.y=21;}
        if (tel != 2) gpJeu->getTexte()->affiche(gpScreen, "3", 161, 64);
        else {dst.x=157; dst.y=65;}
        if (tel != 3) gpJeu->getTexte()->affiche(gpScreen, "4", 62, 109);
        else {dst.x=58; dst.y=110;}
        if (tel != 4) gpJeu->getTexte()->affiche(gpScreen, "5", 172, 155);
        else {dst.x=168; dst.y=156;}
        if (tel != 5) gpJeu->getTexte()->affiche(gpScreen, "6", 259, 119);
        else {dst.x=255; dst.y=120;}
        if (tel != 6) gpJeu->getTexte()->affiche(gpScreen, "7", 72, 201);
        else {dst.x=68; dst.y=202;}
        if (tel != 7) gpJeu->getTexte()->affiche(gpScreen, "8", 102, 201);
        else {dst.x=98; dst.y=202;}
        if (tel != 8) gpJeu->getTexte()->affiche(gpScreen, "9", 264, 179);
        else {dst.x=260; dst.y=180;}
        
        SDL_BlitSurface(tmp, &src, gpScreen, &dst);
        SDL_FreeSurface(tmp);
    
    }
}
