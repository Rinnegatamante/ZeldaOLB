/*

    Zelda Oni Link Begins

    Copyright (C) 2006-2008  Vincent Jouillat

    Please send bugreports with examples or suggestions to www.zeldaroth.fr

*/

#include <SDL/SDL_image.h>
#include <cmath>

#include "Menu.h"
#include "Joueur.h"
#include "Monde.h"
#include "Texte.h"
#include "Projectile.h"
#include "Ennemi.h"
#include "Jeu.h"

Ennemi::Ennemi(Jeu* jeu, int type, int i, int j, bool p) : Personnage(), gpJeu(jeu), 
id(type), vanim(120), vinvin(3), vol(0), nage(0), spectre(0), porte(0), typeIA(IA_RIEN), 
vitesse(1), force(0), recul(16), glace(0), maxglace(60), typeEnn(E_SIMPLE), 
zonesAttaques(NULL), zonesVulnerables(NULL), chargeIm(false), arme(false),
special1(0), special2(0), persistent(p), type_att(ENNEMI), dist(16) {
    if (id) {
        Ennemi* gpEnnemi = gpJeu->existEnnemi(id);
        if (gpEnnemi != NULL) {
            image = gpEnnemi->image;
        }
        else {
            ostringstream im;
            im << id;
            image = IMG_Load(("app0:/images/ennemi/ennemi" + im.str() + ".png").c_str());
            if (id != 70) 
                SDL_SetColorKey(image,SDL_SRCCOLORKEY,SDL_MapRGB(image->format,0,0,255));
            chargeIm = true;
        }
        x=i; y=j; saveEmpl();
        special1=0; // à sortir pour l'ennemi 20
        init();
    }
}

Ennemi::~Ennemi() {

    if (!chargeIm) image = NULL; 
	if(suivant)	{
		Ennemi* temp;
		temp = (Ennemi*)suivant;
		suivant=NULL;
		delete temp;
	}
	
    delete zonesAttaques;
    delete zonesVulnerables;
}

void Ennemi::drawSpecial(SDL_Surface* gpScreen) {
    int phg[2];
    phg[0] = gpJeu->getPhg(0);
    phg[1] = gpJeu->getPhg(1);
    
    Monde* gpMonde = gpJeu->getMonde();
    
    if (id && vie>0 && x+w > gpMonde->regionValue(0) && x < gpMonde->regionValue(2)
    && y+h > gpMonde->regionValue(1) && y < gpMonde->regionValue(3)) {
        
        SDL_Rect src;
        SDL_Rect dst;
        
        if (id == 51) {
            if (anim > 10+10) {
                dst.x = x-phg[0]+2;
                dst.y = y-phg[1]+18;
                src.y=22; src.x=2; src.h=6; src.w=20;
                SDL_BlitSurface(image, &src, gpScreen, &dst);
            }
        }
        
    }
    
    if (suivant != NULL) ((Ennemi*)suivant)->drawSpecial(gpScreen);
}

void Ennemi::draw(SDL_Surface* gpScreen) {
    int phg[2];
    phg[0] = gpJeu->getPhg(0);
    phg[1] = gpJeu->getPhg(1);
    
    Monde* gpMonde = gpJeu->getMonde();
    
    if (id==50 && gpJeu->getZone()==26 && gpJeu->getJoueur()->getCoffre(11,24) && vie>0) vie=0;
    
    if (isBoss() && id != 59 && id != 67 && id != 68 && id != 69 
    && (id != 31 || !gpJeu->getJoueur()->getCoffre(13,10))
    && (id!=50 || gpJeu->getZone()==28)) {
        if (id!=74) {
            gpJeu->getStatut()->setValJauge(vie); gpJeu->getStatut()->setMaxJauge(viemax);}
        else if (y>-152) {
            gpJeu->getStatut()->setValJauge2(vie); gpJeu->getStatut()->setMaxJauge2(viemax);}
    }
    
    if (id && vie>0 && x+w > gpMonde->regionValue(0) && x < gpMonde->regionValue(2)
    && y+h > gpMonde->regionValue(1) && y < gpMonde->regionValue(3)) {
        
        if (!gpJeu->getStop() && (!invincible || isBoss()) && !glace) IA();
        
        if (gpJeu->getTransition()) return;
        
        SDL_Rect src;
        SDL_Rect dst;
        
        int anim2 = anim;
        
        if (id == 15) {
            if (anim < 18) anim2 = anim2%2;
            else anim2 = 2;
        }
        
        if (id == 16 || id == 30) anim2 = anim/5;
        
        if (id == 27) {
            if (anim < 6) anim2 = anim2%2;
            else if (anim < 8) anim2 = 2;
            else anim2 = 3;
        }
        
        if (id == 31 && anim > 3) anim2=3;
        
        if (id == 40) anim2=anim%3;
        
        if (id == 45) anim2 = anim/2;
        
        if (id == 51) {
            if (anim > 30 && special1 == 0) anim2=1;
            else anim2=0;
        }
        
        if (id == 57) {
            if (anim < 36) anim2 = 0;
            else if (anim < 38) anim2 = 1;
            else anim2 = 2;
        }
        
        if (typeIA==IA_STATUE) anim2=0;
        
        switch (typeEnn) {
            case E_SIMPLE : src.x=direction*w; src.y=h*anim2; break;
            case E_PROFIL : src.x=(direction%2)*w; src.y=h*anim2; break;
            case E_MONO_DIR : src.x=w*anim2; src.y=0; break;
        }
        dst.x=x-phg[0]; dst.y=y-phg[1]; src.w=w; src.h=h;
        
        if (id == 20 && special1) src.x+=16;
        
        if(id == 45) {
            if (special1 < 3) {
                SDL_BlitSurface(image, &src, gpScreen, &dst);
                src.y=76; src.x=48*special1; src.w=48; src.h=38;
                dst.x = x+9-phg[0]; dst.y = y+29-phg[1];
            }
            if ((special2 > 10 && special1 > 2) || special2 > 40) {
                SDL_BlitSurface(image, &src, gpScreen, &dst);
                src.y=76; src.x=144; src.w=16; src.h=10;
                dst.x = x+25-phg[0]; dst.y = y+64-phg[1];
            }
        }
        
        if (id == 50) {
            if (glace) {src.y=h*2; effets[FLECHE]=0;}
            else if (gpJeu->getJoueur()->hasObjet(O_ARC)==5 && vie>100) effets[FLECHE]=2;
            if (vie==1 && special1>1 && special1<7) {dst.y-=6; src.h+=6; src.x=52;src.y=113;}
            if (vie==1 && special1>=7) {dst.y-=6; src.h+=6; src.x=0;src.y=113;}
        }
        
        if (id == 51) dst.y-=special1;
        if (id == 52 && special1 >= 3) src.y+=34;
        
        if (id == 58 && special1 > 0) {
            src.x=29; src.y=30; src.w=12; src.h=6; dst.x++; dst.y+=9;
            SDL_BlitSurface(image, &src, gpScreen, &dst);
            src.x=28; src.y=15*anim2; src.w=14; src.h=15; dst.x--; dst.y-=(9+special1);
        }
        
        if (id == 61) {src.y=h*anim2; src.x=0;}
        
        if(id == 62 && special2) {
            src.x=14;src.y=74;src.w=24;src.h=17;dst.y+=20;
            SDL_BlitSurface(image, &src, gpScreen, &dst);
            src.x=0;src.y=74;src.w=14;src.h=16;dst.x+=5;dst.y-=5;
            if (special2<5) dst.y-=15-(special2-1)*5;
            if (special2>46) dst.y-=(special2-46)*5;
        }
        
        if (id == 63 && special2) {
            src.x=0;
            if (special2==1) src.y=68;
            else {
                src.y=128+56*anim2;
            }
        }
        
        if (id == 66 && special1 > 5 && special1 < 134) {
            SDL_BlitSurface(image, &src, gpScreen, &dst);
            src.y=124; src.h = 16; dst.y = y-phg[1]+h-10;
        }
        if(id == 67 && anim>=20) {src.y=0; src.x=w*(anim-20);}
        if(id == 68 || id == 69) {
            src.y=0;
            switch (special2) {
                case 0 : src.x=0; break;
                case 1 : src.x=26; break;
                case 2 : src.x=67; break;
                case 3 : src.x=95; break;
            }
            if (special1>5 && special1<50) {
                SDL_BlitSurface(image, &src, gpScreen, &dst);
                src.x=32*anim; src.w = 32; src.y = 63; src.h = 16;
                dst.y = y-phg[1]+h-15+5; dst.x = x-phg[0]-4;
            }
        }
        
        if (id==70) {
            src.x=144-special1; src.y=0; src.w=25+special1;
            SDL_BlitSurface(image, &src, gpScreen, &dst);
            src.x=169; dst.x+=320-25-special1;
            //pousse Link
            Joueur* gpJoueur = gpJeu->getJoueur();
            int oldx=gpJoueur->getX();
            if (gpJoueur->getVie()>0 && gpJoueur->getTypeAnim()!=CHUTE) {
                if (special1<135-8) {
                    while (gpJoueur->getX()+2<=x+25+special1)
                        gpJoueur->setX(gpJoueur->getX()+2);
                    while (gpJoueur->getX()+17>=x+320-25-special1)
                        gpJoueur->setX(gpJoueur->getX()-2);
                    if (gpJoueur->getX()!=oldx) gpJeu->testFin();
                } else {
                    gpJoueur->setVie(0); gpJoueur->nonvisible();
                    gpJeu->getAudio()->playSound(43);
                    gpJeu->ajouteProjectile(20, direction, gpJoueur->getX()-8, 
                        gpJoueur->getY()-4, 0);
                    //gpJoueur->splash=1;
                    //Mix_PlayChannel(-1,splash,0);
                }
	    }
        }
        
        if (id==75) {
            Joueur* gpJoueur = gpJeu->getJoueur();
            if (!gpJoueur->getVie()) {typeIA=IA_RIEN; special1=0;}
            //immobile - parle
            if (typeIA==IA_RIEN && special1==0 && special2==0) src.y=0;
            //marche
            if (typeIA!=IA_RIEN && special1==0 && special2==0) src.y+=24;
            //mort
            if (special2) {dst.y+=9; src.x=0; src.y=216; src.w=24; src.h=15;}
            //epee
            if (!special1 && !special2 && typeIA!=IA_RIEN)
            switch (direction) {
                case S : 
                    if (gpJoueur->getX()>x-4 && gpJoueur->getX()<x+32 
                    && gpJoueur->getY()>y && gpJoueur->getY()+10<y+43) special1=1; break;
                case N : 
                    if (gpJoueur->getX()>x-12 && gpJoueur->getX()<x+26 
                    && gpJoueur->getY()<y && gpJoueur->getY()+22>y-17) special1=1; break;
                case O : 
                    if (gpJoueur->getX()+14>x-18 && gpJoueur->getX()<x 
                    && gpJoueur->getY()+10<y+32 && gpJoueur->getY()+22>y-1) special1=1; break;
                case E : 
                    if (gpJoueur->getX()+5<x+36 && gpJoueur->getX()>x 
                    && gpJoueur->getY()+10<y+32 && gpJoueur->getY()+22>y-1) special1=1; break;
            }
            
            int aa1=0; int bb1=0; int aa2=w; int bb2=h;
            if (special1>0) {
                switch (direction) {
                    case S : 
                        dst.x-=6; src.x=76;src.y=43*((special1-2)/2); src.w=37; src.h=43;
                        switch (special1) {
                            case 1 : aa1=-6;bb1=15;aa2=5;bb2=23;break;
                            case 2 : aa1=-3;bb1=20;aa2=10;bb2=35;break;
                            case 3 : aa1=3;bb1=22;aa2=13;bb2=37;break;
                            case 4 : aa1=12;bb1=25;aa2=16;bb2=43;break;
                            case 5 : aa1=16;bb1=18;aa2=26;bb2=33;break;
                            case 6 : aa1=18;bb1=16;aa2=31;bb2=28;break;
                        }
                        break;
                case N : 
                    dst.x-=14; dst.y-=20; src.x=113;src.y=44*((special1-2)/2); src.w=40; src.h=44;
                    switch (special1) {
                        case 1 : aa1=15;bb1=7;aa2=26;bb2=13;break;
                        case 2 : aa1=5;bb1=-13;aa2=16;bb2=4;break;
                        case 3 : aa1=3;bb1=-20;aa2=7;bb2=2;break;
                        case 4 : aa1=-15;bb1=-6;aa2=2;bb2=10;break;
                        case 5 : aa1=-14;bb1=3;aa2=0;bb2=15;break;
                    }
                    break;
                case O : 
                    dst.x-=18; dst.y-=1; src.x=153;src.y=36*((special1-2)/2); src.w=39; src.h=36;
                    switch (special1) {
                        case 1 : aa1=-1;bb1=-1;aa2=5;bb2=12;break;
                        case 2 : aa1=-11;bb1=1;aa2=5;bb2=13;break;
                        case 3 : aa1=-19;bb1=12;aa2=1;bb2=18;break;
                        case 4 : aa1=-11;bb1=16;aa2=2;bb2=29;break;
                        case 5 : aa1=-3;bb1=21;aa2=10;bb2=34;break;
                    }
                    break;
                case E : 
                    dst.x-=1; dst.y-=1; src.x=192;src.y=36*((special1-2)/2); src.w=39; src.h=36;
                    switch (special1) {
                        case 1 : aa1=14;bb1=-1;aa2=19;bb2=12;break;
                        case 2 : aa1=14;bb1=1;aa2=29;bb2=13;break;
                        case 3 : aa1=18;bb1=12;aa2=37;bb2=18;break;
                        case 4 : aa1=16;bb1=16;aa2=29;bb2=29;break;
                        case 5 : aa1=8;bb1=21;aa2=21;bb2=34;break;
                    }
                    break;
                }
                zonesAttaques->getZone()->x=aa1; zonesAttaques->getZone()->y=bb1;
                zonesAttaques->getZone()->w=aa2; zonesAttaques->getZone()->h=bb2;
                if (!gpJeu->getStop()) special1++;
                if(special1==4) gpJeu->getAudio()->playSound(22);
                if (special1>=12 && direction!=S) special1=0;
                if (special1>=14) special1=0;
            }
        }
    
    
        
        if (direction==N && arme) drawArme(gpScreen);
        
        if ((id != 51 && id != 67) || anim>=20) SDL_BlitSurface(image, &src, gpScreen, &dst);
        
        if (direction!=N && arme) drawArme(gpScreen);
        
        //if(animmax) {    
            if(SDL_GetTicks() > lastAnimTime + vanim && !gpJeu->getStop()) {
                lastAnimTime = SDL_GetTicks();
                if (!invincible && !glace) {
                    anim++;
                    if (anim > animmax) anim = 0;
                    if (id == 10 && anim == 1) snipe();
                    if (id == 15 && anim == 20) tire();
                    if (id == 16 && anim > 4 && anim%2==1) tire();
                    if (id == 27 && anim == 8) tire();
                    if (id == 30 && anim == 5) tire();
                    if (id == 31 && anim == 8) snipe();
                    if ((id == 36 || id == 49 || id == 65) && anim == 8) snipe();
                    if (id == 40 && anim == 8) snipe();
                    if (id == 45) {
                        if (special1 < 3 && special2 > 40) anim--;
                        special2++;
                        if (special1 < 3) {
                            if (special2 > 50) special2=0;
                            if (special2 == 45) snipe();
                        } else {
                            if (special2>20) special2=0;
                            if (special2==15) snipe();
                            if (special2==17) snipe();
                            if (special2==19) snipe();
                        }
                    }
                    if (id == 50 && special1>1) {
                        special1++;
                        if (special1==8) snipe();
                        if (special1==10) special1=2;
                    }
                    if (id == 51 && anim == 10+10) {
                        x=gpJeu->getJoueur()->getX()-4;y=gpJeu->getJoueur()->getY();}
                    if (id == 51 && anim == 20) {special1=240;gpJeu->getAudio()->playSound(6);}
                    if (id == 51 && anim == 30) if (special1>0) anim--;
                    if (id == 51 && anim == 40) if (special1<240) anim--;
                    if (id == 57 && anim > 30 && getSuivant()->vie 
                        && getSuivant()->getSuivant()->vie
                        && getSuivant()->getSuivant()->getSuivant()->vie) anim=0;
                    if (id == 57 && anim == 38) renfort();
                    if (id == 62) {
                        if (!special2) {
                            special1++;if (special1 >= 10) {special1=0; if (vie<viemax) vie++;}
                        }else {
                            special2++;
                            if (special2>=49) {
                                special1=0;special2=0;recul=4;
                                typeIA=IA_FONCE;force=2;vie=viemax;
                                for (int k = 0; k < 22; k++) effets[k]=2;
                                zonesVulnerables->getZone()->w=w; 
                                zonesVulnerables->getZone()->h=h;
                                zonesVulnerables->getZone()->x=0; 
                                zonesVulnerables->getZone()->y=0; 
                            }
                        }
                    }
                    if (id == 63 && special2>0 && special2==1) {
                        special2=2; w=82; h=56; y+=4; animmax=1;
                        zonesVulnerables->getZone()->h=56;
                        zonesVulnerables->getSuivant()->getZone()->w=82; 
                        zonesVulnerables->getSuivant()->getZone()->h=34;
                        zonesAttaques->getZone()->h=56;
                        zonesAttaques->getSuivant()->getZone()->w=82; 
                        zonesAttaques->getSuivant()->getZone()->h=34;
                    }
                    if (id == 66 && special1>0) {
                        if (special1 < 134) {
                            special1++;
                            if (special1 > 10) {
                                h++;
                                if (special1<134-8) {
                                    zonesAttaques->getZone()->h=h;
                                    zonesVulnerables->getZone()->h=h;
                                    y--;
                                }
                                else {
                                    h=124; special1=134; 
                                    for (int k = 0; k < 22; k++) effets[k]=2;
                                    Ennemi* enn = getSuivant();
                                    enn->vie=1;
                                    for (int z = 0; z < 4; z++) {
                                        enn = enn->getSuivant();
                                        enn->vie=1; enn->special1=1;
                                    }
                                }
                            }
                        }
                    }
                    if (id == 67) {
                        if (anim==0) {
                            force=0;
                            h=0; zonesAttaques->getZone()->h=h;
                            zonesVulnerables->getZone()->h=h;}
                        if (anim==10) {
                            x=gpJeu->getJoueur()->getX()+8;y=gpJeu->getJoueur()->getY()+24;
                            int k=x-x%16; if (x-k>4) x=k+12; else x=k-4;
                            int l=y-y%16; if (y-l>8) y=l+13+7; else y=l-3+7;
                        }
                        if (anim==20) {
                            force=8;
                            h=8; zonesAttaques->getZone()->h=h;
                            zonesVulnerables->getZone()->h=h; y-=7;}
                        if (anim==21) {
                            h=22; zonesAttaques->getZone()->h=h;
                            zonesVulnerables->getZone()->h=h; y-=14;}
                        if (anim==22) {
                            h=47; zonesAttaques->getZone()->h=h;
                            zonesVulnerables->getZone()->h=h; y-=25;}
                        if (anim==23) {
                            h=22; zonesAttaques->getZone()->h=h;
                            zonesVulnerables->getZone()->h=h; y+=25;}
                        if (anim==24) {
                            h=8; zonesAttaques->getZone()->h=h;
                            zonesVulnerables->getZone()->h=h; y+=14;}
                    }
                }
                if ((id == 68 || id == 69) && special1>0) {
                    if (special1 < 50) {
                        special1++;
                        if (special1 > 10) {
                            h++;
                            if (special1 < 42) {
                                zonesAttaques->getZone()->h=h;
                                zonesVulnerables->getZone()->h=h;
                                y--;
                            }
                            else {h=40; special1=50;}
                        }
                    }else {
                        if (id == 68) {
                            if (special2 > 0 
                            || (gpJeu->getJoueur()->getX() + 20 > x 
                            && gpJeu->getJoueur()->getX()< x + 80 
                            && gpJeu->getJoueur()->getY() + 24 > y 
                            && gpJeu->getJoueur()->getY() < y + 63)) {
                                special2++;
                                switch (special2) {
                                    case 1 : w=41; zonesAttaques->getZone()->w=w; break;
                                    case 2 : w=27; zonesAttaques->getZone()->w=w; break;
                                    case 3 : 
                                        w=80; zonesAttaques->getZone()->w=w;
                                        h=63; zonesAttaques->getZone()->h=h;
                                        zonesAttaques->getZone()->y=20; break;
                                    case 4 :
                                        w=26; zonesAttaques->getZone()->w=w;
                                        h=40; zonesAttaques->getZone()->h=h-10;
                                        zonesAttaques->getZone()->y=0; special2 = 0; break;
                                }
                            }
                        }else {
                            if (special2 > 0 
                            || (gpJeu->getJoueur()->getX() + 20 > (x+w)-80 
                            && gpJeu->getJoueur()->getX() < (x+w) 
                            && gpJeu->getJoueur()->getY() + 24 > y 
                            && gpJeu->getJoueur()->getY() < y + 63)) {
                                special2++;
                                switch (special2) {
                                    case 1 : x-=15; w=41; zonesAttaques->getZone()->w=w; break;
                                    case 2 : x+=14; w=27; zonesAttaques->getZone()->w=w; break;
                                    case 3 : x-=53;
                                        w=80; zonesAttaques->getZone()->w=w;
                                        h=63; zonesAttaques->getZone()->h=h;
                                        zonesAttaques->getZone()->y=20; break;
                                    case 4 : x+=54;
                                        w=26; zonesAttaques->getZone()->w=w;
                                        h=40; zonesAttaques->getZone()->h=h-10;
                                        zonesAttaques->getZone()->y=0; special2 = 0; break;
                                }
                            }
                        }
                    }
                }
                if (id == 70 && special1<135-8) special1++;
                if (id == 70 && special1==135-8) special1+=8;
                if (id == 73 && y<0) y++;
                if (id == 73 && y==0 && anim==0) {
                    special2++;
                    if (special2==3) snipe();
                    if (special2==4) special2=0;
                }
                if (id == 74 && y>-152 && y<0) y++;
                if (id == 74 && y==0 && anim==0) {
                    special2++;
                    if (special2==3) snipe();
                    if (special2==5) special2=0;
                }
                if (id == 75 && special2>0) {
                    vie+=2; if (vie>=viemax) {vie=viemax; special2=0; typeIA=IA_FONCE; force=10;}
                }
                if (id == 76 && anim==0 && special1==0) {snipe(); special1=1;}
                if (id == 76 && anim==1 && special1==1) special1=0;
                if (invincible > 0) invincible++;
                if (invincible > vinvin) invincible=0;
                if (glace > 0) glace++;
                if (glace >= (maxglace*40)/vanim) glace=0;
            }
            if (id == 58 && special1>0) {
                special1-=4;
                if (special1==0) {
                    force=1; for (int k = 0; k < 22; k++) effets[k]=2;
                    typeIA = IA_FONCE;
                }
            }
        //}
    }
    if (suivant != NULL) ((Ennemi*)suivant)->draw(gpScreen);
}

void Ennemi::drawArme(SDL_Surface* gpScreen) {
    int phg[2];
    phg[0] = gpJeu->getPhg(0);
    phg[1] = gpJeu->getPhg(1);
    
    SDL_Rect src;
    SDL_Rect dst;
    
    dst.x=x-phg[0]+zonesAttaques->getSuivant()->getZone()->x;
    dst.y=y-phg[1]+zonesAttaques->getSuivant()->getZone()->y;
    src.w = zonesAttaques->getSuivant()->getZone()->w;
    src.h = zonesAttaques->getSuivant()->getZone()->h;
    
    switch (id) {
        case 5 :
            switch (direction) {
                case N : src.x=64; src.y=10; break;
                case S : src.x=69; src.y=10; break;
                case O : src.x=64; src.y=0; break;
                case E : src.x=64; src.y=5; break;
            }
            SDL_BlitSurface(image, &src, gpScreen, &dst);
            break;
        case 44 :
        case 55 :
            switch (direction) {
                case N : src.x=88; src.y=14; break;
                case S : src.x=95; src.y=14; break;
                case O : src.x=88; src.y=0; break;
                case E : src.x=88; src.y=7; break;
            }
            SDL_BlitSurface(image, &src, gpScreen, &dst);
            break;
        case 46 :
        case 47 :
        case 48 :
            switch (direction) {
                case N : src.x=88; src.y=10; break;
                case S : src.x=93; src.y=10; break;
                case O : src.x=88; src.y=0; break;
                case E : src.x=88; src.y=5; break;
            }
            SDL_BlitSurface(image, &src, gpScreen, &dst);
            break;
    }
}

void Ennemi::moveX(int i) {
    moveX(i, vol, nage, spectre);
}

void Ennemi::moveY(int i) {
    moveY(i, vol, nage, spectre);
}

void Ennemi::moveX(int i, int v, int n, int s) {
    if (id==21 && !gpJeu->isWater(x + gpZoneBase->getZone()->x + i, 
        y + gpZoneBase->getZone()->y, gpZoneBase->getZone()->w, 
        gpZoneBase->getZone()->h)) return;
    if (gpJeu->isEnnemi(x + gpZoneBase->getZone()->x + i, y + gpZoneBase->getZone()->y, 
        gpZoneBase->getZone()->w, gpZoneBase->getZone()->h, getListId())) return;
    if (gpJeu->isPnj(x + gpZoneBase->getZone()->x + i, y + gpZoneBase->getZone()->y, 
        gpZoneBase->getZone()->w, gpZoneBase->getZone()->h, getListId())) return;
    if (!gpJeu->isNotFull(x + gpZoneBase->getZone()->x + i, y + gpZoneBase->getZone()->y, 
        gpZoneBase->getZone()->w, gpZoneBase->getZone()->h, v, n, s)) return;
    x+=i;
}

void Ennemi::moveY(int i, int v, int n, int s) {
    if (id==21 && !gpJeu->isWater(x + gpZoneBase->getZone()->x, 
        y + gpZoneBase->getZone()->y + i, gpZoneBase->getZone()->w, 
        gpZoneBase->getZone()->h)) return;
    if (gpJeu->isEnnemi(x + gpZoneBase->getZone()->x, y + gpZoneBase->getZone()->y + i, 
        gpZoneBase->getZone()->w, gpZoneBase->getZone()->h, getListId())) return;
    if (gpJeu->isPnj(x + gpZoneBase->getZone()->x, y + gpZoneBase->getZone()->y + i, 
        gpZoneBase->getZone()->w, gpZoneBase->getZone()->h)) return;
    if (!gpJeu->isNotFull(x + gpZoneBase->getZone()->x, y + gpZoneBase->getZone()->y + i, 
        gpZoneBase->getZone()->w, gpZoneBase->getZone()->h, v, n, s)) return;
    y+=i;
}

void Ennemi::reveil() {
    special1 = 1;
}

void Ennemi::pousse(Direction dir) {
    switch (dir) {
        case N : for (int i = 0; i < recul; i++) moveY(-1, 1, 1, spectre); break;
        case S : for (int i = 0; i < recul; i++) moveY(1, 1, 1, spectre); break;
        case O : for (int i = 0; i < recul; i++) moveX(-1, 1, 1, spectre); break;
        case E : for (int i = 0; i < recul; i++) moveX(1, 1, 1, spectre); break;
    }
    //rajouter vérif eau/vide
    if (gpJeu->isVide(x+gpZoneBase->getZone()->x,y+gpZoneBase->getZone()->y,
    gpZoneBase->getZone()->w,gpZoneBase->getZone()->h) && !vol && !spectre) {
        vie = 0; gpJeu->getAudio()->playSound(6);
        if (id==72) {
            Monde* gpMonde = gpJeu->getMonde();
            Joueur* gpJoueur = gpJeu->getJoueur();
            if (gpMonde->regionValue(0)==40*16) {
                gpJoueur->setCoffre(10,20,1);vie=1;x=136*16;y=36*16;xdep=x;ydep=y;}
            if (gpMonde->regionValue(0)==120*16) {
                gpJoueur->setCoffre(10,20,2);vie=1;x=39*16;y=104*16;xdep=x;ydep=y;}
            if (gpMonde->regionValue(0)==20*16) {
                gpJoueur->setCoffre(10,20,3);vie=1;x=103*16;y=115*16;xdep=x;ydep=y;}
            if (gpMonde->regionValue(0)==100*16) {
                gpJoueur->setCoffre(10,20,4);vie=1;x=194*16;y=111*16;xdep=x;ydep=y;}
            if (gpMonde->regionValue(0)==180*16) {
                gpJoueur->setCoffre(10,20,5);gpJoueur->setEnnemi(id);
                gpJeu->ajouteProjectile(15, direction, x+(w/2)-14, y+(h/2)-14, 0);}
        }else gpJeu->ajouteProjectile(15, direction, x+(w/2)-14, y+(h/2)-14, 0);
        gpJeu->testAnim();
        //return;
    }
    if (gpJeu->isWater(x+gpZoneBase->getZone()->x,y+gpZoneBase->getZone()->y,
    gpZoneBase->getZone()->w,gpZoneBase->getZone()->h) && !vol && !nage && !spectre) {
        vie = 0; gpJeu->ajouteProjectile(8, direction, x, y, 0);
        gpJeu->testAnim();
    }
}

void Ennemi::revie() {
    if (((!isBoss() && id!=72) || vie > 0) 
    && (id != 27 || xdep != 249*16+8 || ydep != 24*16)) init();
    //else if ((gpJeu->getZone()==23 || gpJeu->getZone()==24) && vie > 0) {
    //    x=xdep; y=ydep; vie=viemax; direction=S;}
    if (suivant != NULL) getSuivant()->revie();
}

void Ennemi::permute(int val) {
    if (id == 20) {
        special1=val;
    }
    if (suivant != NULL) getSuivant()->permute(val);
}

int Ennemi::getEffet(int i) {return effets[i];}

int Ennemi::getObjets(int i) {return objets[i];}

ZoneRect* Ennemi::getZonesVulnerables() {return zonesVulnerables;}

Ennemi* Ennemi::getSuivant() {return (Ennemi*)suivant;}

void Ennemi::setInvincible(int i) {invincible=i;}

void Ennemi::gele() {glace=1;}

void Ennemi::degele() {glace=0;}

void Ennemi::mort() {vie=0;}

void Ennemi::enerve() {
    if (id == 52) {
        for (int k = 0; k < 22; k++) effets[k]=0;
        typeIA = IA_FONCE;
        special1=3;
        vitesse=2;
        vanim = 120;
    }
}

void Ennemi::setVie(int i) {
    if (id == 52 && special1 < 3) {
        special1++;
        if (special1 == 3) gpJeu->poule();
    }
    if (id == 31) {
        special2++;
        special1 = 1 + special2;
    }
    if (id == 45 && special1 < 3) {
        special1++; 
        if (special1 == 3) {
            zonesVulnerables->getZone()->w=12; zonesVulnerables->getZone()->h=17;
            zonesVulnerables->getZone()->x=27; zonesVulnerables->getZone()->y=39; 
            for (int k = 0; k < 22; k++) effets[k]=2;
        }
        return;
    }
    if (id == 50) { 
        if (effets[FLECHE]==2 && i < 100) effets[FLECHE]=0;
        if (i<=1) {i=1; special1=1; for (int k = 0; k < 22; k++) effets[k]=0; effets[MORTEL]=2;}
    }
    if (id == 57) {
        Ennemi* enn = getSuivant();
        if (i <= 0) {
            for (int j=0; j <3; j++) {
                if (enn->vie) {
                    gpJeu->ajouteProjectile(15, direction, 
                        enn->x+(enn->w/2)-14, enn->y+(enn->h/2)-14, 0);
                    enn->init();
                }
                enn=enn->getSuivant();
            }
        }
        enn = getSuivant()->getSuivant()->getSuivant()->getSuivant();
        if (i <= 2 && enn->vie) {
            enn->vie=0;
            gpJeu->ajouteProjectile(15, direction, 
                enn->x+(enn->w/2)-14, enn->y+(enn->h/2)-14, 0);
            enn = enn->getSuivant();
            enn->vie=0;
            gpJeu->ajouteProjectile(15, direction, 
                enn->x+(enn->w/2)-14, enn->y+(enn->h/2)-14, 0);
            for (int i = 0; i < 4; i++) gpJeu->getMonde()->setMurs(89*2+i,24*2,0);
        }
    }
    if (id == 62 && special2==0) {
        if (i > 0 && i <= 5) {
            special1=0;special2=1;recul=0;typeIA=IA_RIEN;force=0;
            for (int k = 0; k < 22; k++) effets[k]=0; effets[EXPLOSION]=2;
            zonesVulnerables->getZone()->w=w; zonesVulnerables->getZone()->h=16;
            zonesVulnerables->getZone()->x=0; zonesVulnerables->getZone()->y=21; 
        }
    }
    if(id == 63 && special2==0 && vie < 11  && vie > 0) {
        special2=1; x-=12; w=71; h=60; y-=8; force=12;
        if (x < gpJeu->getMonde()->regionValue(0)) x = gpJeu->getMonde()->regionValue(0);
        zonesVulnerables->getZone()->x=25; zonesVulnerables->getZone()->h=60;
        zonesVulnerables->getSuivant()->getZone()->w=71; 
        zonesVulnerables->getSuivant()->getZone()->h=40;
        zonesVulnerables->getSuivant()->getZone()->x=0; 
        zonesVulnerables->getSuivant()->getZone()->y=1; 
        zonesAttaques->getZone()->x=25; zonesAttaques->getZone()->h=60;
        zonesAttaques->getSuivant()->getZone()->w=71; 
        zonesAttaques->getSuivant()->getZone()->h=40;
        zonesAttaques->getSuivant()->getZone()->x=0; 
        zonesAttaques->getSuivant()->getZone()->y=1; 
    }
    if (id == 66 && i <= 0) {
        Ennemi* enn = getSuivant();
        enn->vie=0;
        for (int z = 0; z < 4; z++) {
            enn = enn->getSuivant();
            enn->vie=0;
        }
    }
    if (id == 73 && getSuivant()->y==-152 && vie < 50) getSuivant()->y++;
    if (id == 75) {
        if (i<=0) {
            i=1;
            special2=1;
            special1=0;
            typeIA=IA_RIEN;
            force=0;
            effets[COUP_EPEE]=0; effets[EPEE_SPIN]=0;
        }
        //gpJeu->ecrit(183);
        if (i<=75) vitesse=2; else vitesse=1;
    }
    
    vie=i;
    if (vie < 0) vie = 0;
    if (vie > viemax) vie = viemax;
}

void Ennemi::capture() {
    Joueur* j = gpJeu->getJoueur();
    if (id==75 && j->hasObjet(O_MASQUE)==1 && special2) {
        gpJeu->trouve(C_MASQUE_ONI);
        vie=0;
        gpJeu->getAudio()->playSound(8);
        gpJeu->ajouteProjectile(15, direction, x+(w/2)-14, y+(h/2)-14, 0);
        gpJeu->donneObjet(this);
    }
}

void Ennemi::ajuste() {
    switch(id) {
        case 2 :
            if (direction==N || direction==S) {
                zonesAttaques->getZone()->x=0;
                zonesAttaques->getZone()->w=21;
            }
            else {
                zonesAttaques->getZone()->x=3;
                zonesAttaques->getZone()->w=16;
            }
            break;
        case 5 :
            switch(direction) {
                case N : 
                    zonesAttaques->getSuivant()->getZone()->w=5;
                    zonesAttaques->getSuivant()->getZone()->h=13;
                    zonesAttaques->getSuivant()->getZone()->x=12;
                    zonesAttaques->getSuivant()->getZone()->y=4-3*anim;
                    break;
                case S : 
                    zonesAttaques->getSuivant()->getZone()->w=5;
                    zonesAttaques->getSuivant()->getZone()->h=13;
                    zonesAttaques->getSuivant()->getZone()->x=0;
                    zonesAttaques->getSuivant()->getZone()->y=15+4*anim;
                    break;
                case O : 
                    zonesAttaques->getSuivant()->getZone()->w=13;
                    zonesAttaques->getSuivant()->getZone()->h=5;
                    zonesAttaques->getSuivant()->getZone()->x=-4-5*anim;
                    zonesAttaques->getSuivant()->getZone()->y=19;
                    break;
                case E : 
                    zonesAttaques->getSuivant()->getZone()->w=13;
                    zonesAttaques->getSuivant()->getZone()->h=5;
                    zonesAttaques->getSuivant()->getZone()->x=7+5*anim;
                    zonesAttaques->getSuivant()->getZone()->y=19;
                    break;
            }
            break;
        case 13 :
            switch (anim) {
                case 0 : zonesAttaques->getZone()->w=24; zonesAttaques->getZone()->x=4; break;
                case 1 : zonesAttaques->getZone()->w=32; zonesAttaques->getZone()->x=0; break;
                case 2 : zonesAttaques->getZone()->w=18; zonesAttaques->getZone()->x=7; break;
            }
            break;
        case 15 :
            if (direction!=N) {zonesAttaques->getZone()->y=5; zonesAttaques->getZone()->h=26;}
            else {zonesAttaques->getZone()->y=0; zonesAttaques->getZone()->h=31;}
            break;
        case 19 :
            switch (anim) {
                case 0 : gpZoneBase->getZone()->w=64; gpZoneBase->getZone()->x=0; break;
                case 1 : gpZoneBase->getZone()->w=44; gpZoneBase->getZone()->x=10; break;
                case 2 : gpZoneBase->getZone()->w=22; gpZoneBase->getZone()->x=21; break;
            }
            break;
        case 25 :
            if (direction==N || direction==S) {
                zonesAttaques->getZone()->x=0; zonesAttaques->getZone()->w=28;}
            else {zonesAttaques->getZone()->x=6; zonesAttaques->getZone()->w=16;}
            break;
        case 33 :
        case 38 :
            if (direction==N || direction==S) {
                zonesAttaques->getZone()->x=0; zonesAttaques->getZone()->w=22;}
            else {zonesAttaques->getZone()->x=2; zonesAttaques->getZone()->w=18;}
            break;
        case 34 :
        case 39 :
            if (direction==N || direction==S) {
                zonesAttaques->getZone()->x=0; zonesAttaques->getZone()->w=24;}
            else {zonesAttaques->getZone()->x=4; zonesAttaques->getZone()->w=16;}
            break;
        case 44 :
        case 55 :
            if (direction==N || direction==S) {
                zonesAttaques->getZone()->x=0; zonesAttaques->getZone()->w=22;}
            else {zonesAttaques->getZone()->x=3; zonesAttaques->getZone()->w=16;}
            switch(direction) {
                case N : 
                    zonesAttaques->getSuivant()->getZone()->w=7;
                    zonesAttaques->getSuivant()->getZone()->h=15;
                    zonesAttaques->getSuivant()->getZone()->x=15;
                    zonesAttaques->getSuivant()->getZone()->y=-6+7*anim;
                    break;
                case S : 
                    zonesAttaques->getSuivant()->getZone()->w=7;
                    zonesAttaques->getSuivant()->getZone()->h=15;
                    zonesAttaques->getSuivant()->getZone()->x=0;
                    zonesAttaques->getSuivant()->getZone()->y=17+3*anim;
                    break;
                case O : 
                    zonesAttaques->getSuivant()->getZone()->w=15;
                    zonesAttaques->getSuivant()->getZone()->h=7;
                    zonesAttaques->getSuivant()->getZone()->x=-12+5*anim;
                    zonesAttaques->getSuivant()->getZone()->y=14+2*anim;
                    break;
                case E : 
                    zonesAttaques->getSuivant()->getZone()->w=15;
                    zonesAttaques->getSuivant()->getZone()->h=7;
                    zonesAttaques->getSuivant()->getZone()->x=19-5*anim;
                    zonesAttaques->getSuivant()->getZone()->y=14+2*anim;
                    break;
            }
            break;
        case 45 :
            switch(anim%2) {
                case 0 : 
                    zonesAttaques->getSuivant()->getZone()->y=67;
                    zonesAttaques->getSuivant()->getSuivant()->getZone()->y=59;
                    break;
                case 1 : 
                    zonesAttaques->getSuivant()->getZone()->y=59;
                    zonesAttaques->getSuivant()->getSuivant()->getZone()->y=67;
                    break;
            }
            break;
        case 46 :
        case 47 :
        case 48 :
            if (direction==N || direction==S) {
                zonesAttaques->getZone()->x=0; zonesAttaques->getZone()->w=22;}
            else {zonesAttaques->getZone()->x=2; zonesAttaques->getZone()->w=18;}
            switch(direction) {
                case N : 
                    zonesAttaques->getSuivant()->getZone()->w=5;
                    zonesAttaques->getSuivant()->getZone()->h=16+((int)(id/48));
                    zonesAttaques->getSuivant()->getZone()->x=16;
                    zonesAttaques->getSuivant()->getZone()->y=-6+7*anim;
                    break;
                case S : 
                    zonesAttaques->getSuivant()->getZone()->w=5;
                    zonesAttaques->getSuivant()->getZone()->h=16+((int)(id/48));
                    zonesAttaques->getSuivant()->getZone()->x=0;
                    zonesAttaques->getSuivant()->getZone()->y=21+anim-((int)(id/48));
                    break;
                case O : 
                    zonesAttaques->getSuivant()->getZone()->w=16+((int)(id/48));
                    zonesAttaques->getSuivant()->getZone()->h=5;
                    zonesAttaques->getSuivant()->getZone()->x=-12+3*anim-((int)(id/48));
                    zonesAttaques->getSuivant()->getZone()->y=16+2*anim;
                    break;
                case E : 
                    zonesAttaques->getSuivant()->getZone()->w=16+((int)(id/48));
                    zonesAttaques->getSuivant()->getZone()->h=5;
                    zonesAttaques->getSuivant()->getZone()->x=18-3*anim;
                    zonesAttaques->getSuivant()->getZone()->y=16+2*anim;
                    break;
            }
            break;
        case 50 :
            switch(direction) {
                case N : zonesAttaques->getZone()->x=7; zonesAttaques->getZone()->w=38; break;
                case S : zonesAttaques->getZone()->x=4*anim; zonesAttaques->getZone()->w=48; 
                    break;
                default : break;
            }
            break;
        case 60 :
            switch(direction) {
                case N : 
                    zonesAttaques->getZone()->x=0; zonesAttaques->getZone()->y=11;
                    zonesAttaques->getZone()->w=36; zonesAttaques->getZone()->h=28;
                    zonesAttaques->getSuivant()->getZone()->x=9;
                    zonesAttaques->getSuivant()->getZone()->y=39;
                    zonesAttaques->getSuivant()->getZone()->w=32;
                    zonesAttaques->getSuivant()->getZone()->h=24;
                    zonesVulnerables->getZone()->x=9; zonesVulnerables->getZone()->y=39;
                    zonesVulnerables->getZone()->w=32; zonesVulnerables->getZone()->h=24;
                    break;
                case S : 
                    zonesAttaques->getZone()->x=0; zonesAttaques->getZone()->y=0;
                    zonesAttaques->getZone()->w=50; zonesAttaques->getZone()->h=36;
                    zonesAttaques->getSuivant()->getZone()->x=9;
                    zonesAttaques->getSuivant()->getZone()->y=36;
                    zonesAttaques->getSuivant()->getZone()->w=32;
                    zonesAttaques->getSuivant()->getZone()->h=26;
                    zonesVulnerables->getZone()->x=9; zonesVulnerables->getZone()->y=36;
                    zonesVulnerables->getZone()->w=32; zonesVulnerables->getZone()->h=26;
                    break;
                case O : 
                    zonesAttaques->getZone()->x=4; zonesAttaques->getZone()->y=0;
                    zonesAttaques->getZone()->w=26; zonesAttaques->getZone()->h=30;
                    zonesAttaques->getSuivant()->getZone()->x=6;
                    zonesAttaques->getSuivant()->getZone()->y=30;
                    zonesAttaques->getSuivant()->getZone()->w=39;
                    zonesAttaques->getSuivant()->getZone()->h=32;
                    zonesVulnerables->getZone()->x=6; zonesVulnerables->getZone()->y=30;
                    zonesVulnerables->getZone()->w=39; zonesVulnerables->getZone()->h=32;
                    break;
                case E : 
                    zonesAttaques->getZone()->x=20; zonesAttaques->getZone()->y=0;
                    zonesAttaques->getZone()->w=26; zonesAttaques->getZone()->h=30;
                    zonesAttaques->getSuivant()->getZone()->x=5;
                    zonesAttaques->getSuivant()->getZone()->y=30;
                    zonesAttaques->getSuivant()->getZone()->w=39;
                    zonesAttaques->getSuivant()->getZone()->h=32;
                    zonesVulnerables->getZone()->x=5; zonesVulnerables->getZone()->y=30;
                    zonesVulnerables->getZone()->w=39; zonesVulnerables->getZone()->h=32;
                    break;
            }
            break;
    }
}

void Ennemi::regard() {
    //if (typeEnn == E_MONO_DIR) return;
    Joueur* gpJoueur = gpJeu->getJoueur();
    int ennCX = x + gpZoneBase->getZone()->x + (gpZoneBase->getZone()->w/2);
    int ennCY = y + gpZoneBase->getZone()->y + (gpZoneBase->getZone()->h);
    int linkCX = gpJoueur->getX() + 8;
    int linkCY = gpJoueur->getY() + 24;
    int difx = ennCX - linkCX;
    int dify = ennCY - linkCY;
    
    if (abs(difx) > abs(dify) || typeEnn == E_PROFIL) {
        if (difx>0) direction=O;
        else direction=E;
    }
    else {
        if (dify>0) direction=N;
        else direction=S;
    }
    
    if (id == 50) {if (dify>0) direction = N; else direction = S;}
    
    ajuste();
}

void Ennemi::hautbas() {
    int phg[2];
    phg[0] = gpJeu->getPhg(0);
    phg[1] = gpJeu->getPhg(1);
    
    int old = y;
    switch (direction) {
        case N :
            moveY(-1);
            if (old == y) {
                direction = S;
                if (x+w > phg[0] && x < phg[0] + 320 && y+h > phg[1] && y < phg[1] + 240)
                    gpJeu->getAudio()->playSound(21);
            }
            break;
        case S :
            moveY(1);
            if (old == y) {
                direction = N;
                if (x+w > phg[0] && x < phg[0] + 320 && y+h > phg[1] && y < phg[1] + 240)
                    gpJeu->getAudio()->playSound(21);
            }
            break;
        default : break;
    }
}

void Ennemi::special() {
    if (id==51) {
        if (special1>0) special1-=4;
        if (anim>31 && anim <36)
        gpJeu->testDegat(x + zonesAttaques->getZone()->x, 
            y + zonesAttaques->getZone()->y , 
            zonesAttaques->getZone()->w, 
            zonesAttaques->getZone()->h, 
            RETOUR, force, direction);
        if (anim>35) special1+=8;
    }
}

void Ennemi::IA() {
    if (id!=75 || !special1)
    switch (typeIA) {
        case IA_FONCE : for (int i = 0; i < vitesse; i++) bouge(); break;
        case IA_ATTEND : attend(); break;
        case IA_RANDOM : deplace(); break;
        case IA_SNIPE : regard(); break;
        case IA_HAUT_BAS : for (int i = 0; i < vitesse; i++) hautbas(); break;
        case IA_SPECIAL : special(); break;
        default : break;
    }
    
    if ((id == 52 && special1 < 3) || id == 20 || id == 51 || force==0) return;
    
    ZoneRect* zonesAttaques2 = zonesAttaques;
    while (zonesAttaques2 != NULL) {
        if (gpJeu->testDegat(x + zonesAttaques2->getZone()->x, 
            y + zonesAttaques2->getZone()->y , 
            zonesAttaques2->getZone()->w, 
            zonesAttaques2->getZone()->h, 
            type_att, force, direction, dist) && type_att == ASPIRE) {
            
            int degats=force;
            if (!gpJeu->getJoueur()->getDefense()) degats=degats*2;
            else degats=degats/gpJeu->getJoueur()->getDefense();
            if (force > 0 && degats < 1) degats = 1;
            setVie(vie + degats);
            
        }
        zonesAttaques2 = zonesAttaques2->getSuivant();
    }
}

void Ennemi::bouge() {
    if (!distance()) {
        if (id == 13 || id == 18) typeIA=IA_ATTEND;
        return;
    }
    if (id == 15 && anim>=18) return;
    if (id == 27 && anim>=6) return;
    if (id == 45 && special1 < 3 && special2 > 40) return;
    if (id==50 && special1) {
        direction=N;
        if (x < 149*16-10) x++;
        if (x > 149*16-10) x--;
        if (y < 108*16) y++;
        if (y > 108*16) y--;
        if (x == 149*16-10 && y == 108*16 && special1==1) special1=2;
        return;
    }
    
    Joueur* gpJoueur = gpJeu->getJoueur();
    
    int x1 = x + gpZoneBase->getZone()->x + (gpZoneBase->getZone()->w/2);
    int y1 = y + gpZoneBase->getZone()->y + gpZoneBase->getZone()->h;
    
    int x2 = gpJoueur->getX()+8;
    int y2 = gpJoueur->getY()+24-vol*7;
    
    
    if (x1 < x2) moveX(1);
    else if (x1 > x2) moveX(-1);
    
    if (y1 < y2) moveY(1);
    else if (y1 > y2) moveY(-1);
    
    regard();
}

void Ennemi::attend() {
    anim=animmax+1;
    if (!distance()) return;
    typeIA = IA_FONCE; 
    if (id != 13 && id !=18 && id != 54) porte+=130;
    for (int k = 0; k < 22; k++) if (effets[k]>=1) effets[k]=2; else effets[k]=1;
    effets[COUP_MARTEAU]=2;
    anim=0;
}

void Ennemi::deplace() {
    int randomValue = (int)((float)rand() / RAND_MAX * (100));    
    switch (randomValue) {
        case 1 : moveX(-1);direction=O; break;
        case 2 : moveX(1); direction=E; break;
        case 3 : moveY(-1);direction=N; break;
        case 4 : moveY(1); direction=S; break;
        case 5 :
        case 6 :
        case 7 :
        case 8 : break;
        default :
            switch (direction) {
                case N : moveY(-1); break;
                case S : moveY(1); break;
                case O : moveX(-1); break;
                case E : moveX(1); break;
            }
            break;    
    }
}

bool Ennemi::distance() {
    Joueur* gpJoueur = gpJeu->getJoueur();
    /*int x1 = x + gpZoneBase->getZone()->x + (gpZoneBase->getZone()->w/2);
    int y1 = y + gpZoneBase->getZone()->y + gpZoneBase->getZone()->h;
    if (typeIA==IA_ATTEND) y1 -= gpZoneBase->getZone()->h/2;
    int tmp1 = abs(x1-(gpJoueur->getX()+8));
    int tmp2 = abs(y1-(gpJoueur->getY()+24-vol*7));
    if (tmp1 > tmp2) return tmp1;
    else return tmp2;*/
    
    int gauche = x+gpZoneBase->getZone()->x;
    int droite = gauche + gpZoneBase->getZone()->w;
    int haut = y+gpZoneBase->getZone()->y;
    int bas = haut + gpZoneBase->getZone()->h;
    
    if (  ((gpJoueur->getX()<gauche-porte && gpJoueur->getX()+16>gauche-porte)
    || (gpJoueur->getX()>=gauche-porte && gpJoueur->getX()+16<=droite+porte)
    || (gpJoueur->getX()<droite+porte && gpJoueur->getX()+16>droite+porte))
    &&
    ((gpJoueur->getY()+8<haut-porte && gpJoueur->getY()+24>haut-porte)
    || (gpJoueur->getY()+8>=haut-porte && gpJoueur->getY()+24<=bas+porte)
    || (gpJoueur->getY()+8<bas+porte && gpJoueur->getY()+24>bas+porte))) {
        return true;
    }
    return false;
}

void Ennemi::snipe() {
    if(!distance()) return;
    
    Joueur* gpJoueur = gpJeu->getJoueur();
    
    int high= 16;
    /*if (gpJoueur->getTypeAnim()==FLOTTE || gpJoueur->getTypeAnim()==NAGE) high=10;
    else high = 16;*/
    
    double anglx = 0;
    double angly = 0;
    int origx = x + (w/2);
    int origy = y + (h/2);
    int destx = gpJoueur->getX() + 8;
    int desty = gpJoueur->getY() + high;;
    
    if (id == 45) origy=y+69;
    if (id == 50) origy-=5;
    if (id == 73) {origx=x+75;origy=y+75;}
    if (id == 74) {origx=x+25;origy=y+75;}
    if (id == 76) {origx=x+23;origy=y+42;}
    if (id == 76 && (direction == N || direction == O)) origx=x+36;
    
    double coef1 = 0;
    double coef2 = 0;
    
    if ((destx-origx) == 0) {anglx=0; angly=12;}
    else if ((desty-origy) == 0) {anglx=12; angly=0;}
    else { 
        coef1=((double)(desty-origy))/((double)(destx-origx));
        coef2=((double)(destx-origx))/((double)(desty-origy));
        anglx=(sqrt(12/(1+(coef1*coef1))));
        angly=(sqrt(12/(1+(coef2*coef2))));
    }
    if (destx - origx < 0) anglx = -anglx;
    if (desty - origy < 0) angly = -angly;
    
    int type = 0;
    switch(id) {
        case 31 : 
            origy+=16;
            if (special1==0) {type = 5;gpJeu->getAudio()->playSound(35);}
            else if (special1 == 1 + special2) {
                type=7; special1--;gpJeu->getAudio()->playSound(28);}
            else {
                type=6; special1--; gpJeu->getAudio()->playSound(28);
            }
            break;
        case 49 : type = 8; gpJeu->getAudio()->playSound(35); break;
        case 50 : type = 12; gpJeu->getAudio()->playSound(35); break;
        case 65 : type = 9; gpJeu->getAudio()->playSound(35); break;
        case 73 : type = 10; gpJeu->getAudio()->playSound(35); break;
        case 74 : case 76 : type = 11; gpJeu->getAudio()->playSound(35); break;
        default : type = 1; gpJeu->getAudio()->playSound(35); break;
    }
    
    if (anglx>4) anglx=4;
    if (angly>4) angly=4;
    if (anglx<-4) anglx=-4;
    if (angly<-4) angly=-4;
    
    gpJeu->ajouteSnipe(type, origx, origy, anglx, angly);
}

void Ennemi::tire() {
    if(!distance()) return;
    
    double anglx = 0;
    double angly = 0;
    int origx = x + (w/2);
    int origy = y + (h/2);
    
    int type = 0;
    switch(id) {
        case 15 : type = 2; gpJeu->getAudio()->playSound(29); break;
        case 16 : case 30 : type = 3; gpJeu->getAudio()->playSound(28); origy+=4; break;
        case 27 : type = 4; gpJeu->getAudio()->playSound(35); break;
        default : type = 2; break;
    }
    
    if (direction==E) anglx=4;
    if (direction==S) angly=4;
    if (direction==O) anglx=-4;
    if (direction==N) angly=-4;
    
    gpJeu->ajouteSnipe(type, origx, origy, anglx, angly);
}

void Ennemi::renfort() {
    Ennemi* enn = getSuivant();
    
    for (int i=0; i<3; i++) {
        if (enn->vie<=0) {
            enn->init();
            enn->x = gpJeu->getJoueur()->getX()+2;
            enn->y = gpJeu->getJoueur()->getY()+9;
            enn->viemax=1;
            enn->vie=1;
            enn->special1=240;
            gpJeu->getAudio()->playSound(6);
            return;
        }
        enn=enn->getSuivant();
    }
}

int Ennemi::getEnnId() {return id;}

void Ennemi::testTapis() {
    if (id==17 || id==61 || typeIA != IA_FONCE || vol || spectre || vie<=0) return;
    
    int a=x+gpZoneBase->getZone()->x+(gpZoneBase->getZone()->w/2);
    int b=y+gpZoneBase->getZone()->y+(gpZoneBase->getZone()->h/2);
    
    Monde* gpMonde = gpJeu->getMonde();
    
    if (gpMonde->motifValue(a,b)>=3400 && gpMonde->motifValue(a,b)<=3415)
    switch (gpMonde->motifValue(a,b)) {
        case 3400 : moveY(-2); break;
        case 3404 : moveY(2); break;
        case 3408 : moveX(-2); break;
        case 3412 : moveX(2); break;
    }
}    

bool Ennemi::isBoss() {
    return (id == 28 || id == 31 || id == 40 || id == 71 || id == 73 || id == 74 || id == 75
    || id == 43 || id == 45 || id == 50 || id == 57 || id == 59 || id == 60 || id == 76
    || (id == 62 && gpJeu->getZone() == 17) || id == 63 || (id >= 66 && id <= 69));
}

void Ennemi::init() {
    x=xdep; y=ydep; direction=S; anim=0; if (id!=20) special1=0; special2=0;
    vanim=240; vol=0; nage=0; spectre=0; porte=0; typeIA=IA_RIEN; 
    vitesse=1; force=0; recul=16; glace=0; maxglace=60; typeEnn=E_SIMPLE;
    for (int k = 0; k < 22; k++) effets[k]=2;
    for (int k = 0; k < 3; k++) objets[k]=0;
    switch (id) {
        case 1 :
            viemax=3; w=16; h=26; animmax=1; typeIA=IA_FONCE; porte=160; force = 2;
            objets[0]=I_RUBIS_VERT;objets[1]=I_BOMBE;objets[2]=I_RUBIS_BLEU;
            gpZoneBase->getZone()->w=16; gpZoneBase->getZone()->h=16;
            gpZoneBase->getZone()->x=0; gpZoneBase->getZone()->y=10; 
            zonesAttaques = new ZoneRect(0,0,w,h);
            zonesVulnerables = new ZoneRect(0,0,w,h);
            break;
        case 2 :
            viemax=12; w=21; h=26; animmax=1; typeIA=IA_FONCE; porte=300; force = 8;
            objets[0]=I_BOMBE;objets[1]=I_MAGIE_PEU;objets[2]=I_PETIT_COEUR;
            gpZoneBase->getZone()->w=16; gpZoneBase->getZone()->h=16;
            gpZoneBase->getZone()->x=2; gpZoneBase->getZone()->y=10; 
            zonesAttaques = new ZoneRect(0,0,w,h);
            zonesVulnerables = new ZoneRect(0,0,w,h);
            recul = 8; maxglace=30;
            break;
        case 3 :
            viemax=2; w=28; h=17; animmax=2; typeIA=IA_FONCE; porte=200; force = 1;
            objets[0]=I_PETIT_COEUR;objets[1]=I_RUBIS_VERT;objets[2]=I_RUBIS_BLEU;
            gpZoneBase->getZone()->w=28; gpZoneBase->getZone()->h=16;
            gpZoneBase->getZone()->x=0; gpZoneBase->getZone()->y=1; 
            zonesAttaques = new ZoneRect(0,0,w,h);
            zonesVulnerables = new ZoneRect(0,0,w,h);
            typeEnn=E_MONO_DIR;
            break;
        case 4 :
            viemax=1; w=16; h=17; animmax=2; typeIA=IA_FONCE; porte=100; force = 1;
            objets[0]=I_PETIT_COEUR;objets[1]=I_RUBIS_VERT;objets[2]=I_RUBIS_BLEU;
            gpZoneBase->getZone()->w=16; gpZoneBase->getZone()->h=16;
            gpZoneBase->getZone()->x=0; gpZoneBase->getZone()->y=1; 
            zonesAttaques = new ZoneRect(0,0,w,h);
            zonesVulnerables = new ZoneRect(0,0,w,h);
            typeEnn=E_MONO_DIR;
            break;
        case 5 :
            viemax=3; w=16; h=26; animmax=1; typeIA=IA_FONCE; porte=200; force = 3;
            objets[0]=I_FLECHE;objets[1]=I_RUBIS_BLEU;objets[2]=I_PETIT_COEUR;
            gpZoneBase->getZone()->w=16; gpZoneBase->getZone()->h=16;
            gpZoneBase->getZone()->x=0; gpZoneBase->getZone()->y=10; 
            zonesAttaques = new ZoneRect(0,0,w,h);
            zonesAttaques->ajout(new ZoneRect(0,15,5,13));
            zonesVulnerables = new ZoneRect(0,0,w,h); arme = true;
            break;
        case 6 :
            viemax=5; w=16; h=37; animmax=9; typeIA=IA_ATTEND; porte=30; force = 2;
            objets[0]=I_FLECHE;objets[1]=I_RUBIS_BLEU;objets[2]=I_PETIT_COEUR;
            gpZoneBase->getZone()->w=16; gpZoneBase->getZone()->h=16;
            gpZoneBase->getZone()->x=0; gpZoneBase->getZone()->y=21; 
            zonesAttaques = new ZoneRect(0,5,w,32);
            zonesVulnerables = new ZoneRect(0,5,w,32); recul = 8; vanim = 30; typeEnn=E_MONO_DIR;
            for (int k = 0; k < 22; k++) effets[k]=1;
            break;
        case 7 :
            viemax=1; w=16; h=16; animmax=1; typeIA=IA_FONCE; porte=100; force = 1;
            objets[0]=I_RIEN;objets[1]=I_RUBIS_VERT;objets[2]=I_RUBIS_BLEU;
            gpZoneBase->getZone()->w=16; gpZoneBase->getZone()->h=16;
            gpZoneBase->getZone()->x=0; gpZoneBase->getZone()->y=1; 
            zonesAttaques = new ZoneRect(0,0,w,h);
            zonesVulnerables = new ZoneRect(0,0,w,h);
            type_att = VENIN;
            break;
        case 8 :
            viemax=4; w=16; h=24; animmax=1; typeIA=IA_FONCE; porte=160; force = 2;
            objets[0]=I_RUBIS_BLEU;objets[1]=I_RUBIS_BLEU;objets[2]=I_RUBIS_ROUGE;
            gpZoneBase->getZone()->w=16; gpZoneBase->getZone()->h=16;
            gpZoneBase->getZone()->x=0; gpZoneBase->getZone()->y=8; 
            zonesAttaques = new ZoneRect(0,0,w,h);
            zonesVulnerables = new ZoneRect(0,0,w,h);
            maxglace=100;
            break;
        case 9 :
            viemax=3; w=16; h=24; animmax=1; typeIA=IA_FONCE; porte=160; force = 2;
            objets[0]=I_FLECHE;objets[1]=I_PETIT_COEUR;objets[2]=I_RUBIS_ROUGE;
            gpZoneBase->getZone()->w=16; gpZoneBase->getZone()->h=16;
            gpZoneBase->getZone()->x=0; gpZoneBase->getZone()->y=8; 
            zonesAttaques = new ZoneRect(0,0,w,h);
            zonesVulnerables = new ZoneRect(0,0,w,h);
            maxglace=100;
            break;
        case 10 :
            viemax=1; w=16; h=16; animmax=1; typeIA=IA_SNIPE; porte=200; force = 2;
            objets[0]=I_FLECHE;objets[1]=I_PETIT_COEUR;objets[2]=I_RUBIS_ROUGE;
            gpZoneBase->getZone()->w=16; gpZoneBase->getZone()->h=16;
            gpZoneBase->getZone()->x=0; gpZoneBase->getZone()->y=0; 
            zonesAttaques = new ZoneRect(0,0,w,h);
            zonesVulnerables = new ZoneRect(0,0,w,h);
            maxglace=100; recul=0; vanim=1240; direction=N; typeEnn=E_MONO_DIR;
            break;
        case 11 :
            viemax=1; w=16; h=21; animmax=2; typeIA=IA_ATTEND; porte=60; force = 8;
            objets[0]=I_BOMBE;objets[1]=I_RUBIS_BLEU;objets[2]=I_PETIT_COEUR;
            gpZoneBase->getZone()->w=16; gpZoneBase->getZone()->h=16;
            gpZoneBase->getZone()->x=0; gpZoneBase->getZone()->y=0; 
            zonesAttaques = new ZoneRect(0,0,w,16);
            zonesVulnerables = new ZoneRect(0,0,w,16); recul = 8; typeEnn=E_MONO_DIR;
            for (int k = 0; k < 22; k++) effets[k]=0;
            break;
        case 12 :
            viemax=4; w=17; h=17; animmax=2; typeIA=IA_FONCE; porte=150; force = 2;
            objets[0]=I_RUBIS_BLEU;objets[1]=I_MAGIE_PEU;objets[2]=I_PETIT_COEUR;
            gpZoneBase->getZone()->w=17; gpZoneBase->getZone()->h=17;
            gpZoneBase->getZone()->x=0; gpZoneBase->getZone()->y=0; 
            zonesAttaques = new ZoneRect(0,0,w,h);
            zonesVulnerables = new ZoneRect(0,0,w,h);
            maxglace=30; typeEnn=E_PROFIL; spectre=1; direction=E;
            for (int k = 0; k < 22; k++) effets[k]=1;
            if (gpJeu->getJoueur()->getEpee()>=2) {effets[COUP_EPEE]=2; effets[EPEE_SPIN]=2;}
            if (gpJeu->getJoueur()->hasObjet(O_ARC)==5) effets[FLECHE]=2; 
            break;
        case 13 :
            viemax=5; w=32; h=15; animmax=1; typeIA=IA_ATTEND; porte=100; force = 6;
            objets[0]=I_RUBIS_VERT;objets[1]=I_RUBIS_BLEU;objets[2]=I_FLECHE;
            gpZoneBase->getZone()->w=18; gpZoneBase->getZone()->h=15;
            gpZoneBase->getZone()->x=7; gpZoneBase->getZone()->y=0; 
            zonesAttaques = new ZoneRect(7,0,18,15);
            zonesVulnerables = new ZoneRect(7,0,18,15);
            maxglace=100; direction=N; typeEnn=E_MONO_DIR; spectre=1; vol=1; vanim=150;
            break;
        case 14 :
            viemax=5; w=41; h=16; animmax=1; typeIA=IA_FONCE; porte=160; force = 18;
            objets[0]=I_BOMBE;objets[1]=I_FLECHE;objets[2]=I_RUBIS_ROUGE;
            gpZoneBase->getZone()->w=41; gpZoneBase->getZone()->h=16;
            gpZoneBase->getZone()->x=0; gpZoneBase->getZone()->y=0; 
            zonesAttaques = new ZoneRect(0,0,w,h);
            zonesVulnerables = new ZoneRect(0,0,w,h);
            maxglace=100; direction=N; typeEnn=E_MONO_DIR;
            break;
        case 15 :
            viemax=10; w=24; h=31; animmax=20; typeIA=IA_FONCE; porte=200; force = 12;
            objets[0]=I_RUBIS_BLEU;objets[1]=I_RUBIS_ROUGE;objets[2]=I_BOMBE;
            gpZoneBase->getZone()->w=16; gpZoneBase->getZone()->h=16;
            gpZoneBase->getZone()->x=4; gpZoneBase->getZone()->y=15; 
            zonesAttaques = new ZoneRect(0,0,w,h);
            zonesVulnerables = new ZoneRect(0,0,w,h); vanim=120;
            break;
        case 16 :
            viemax=6; w=16; h=24; animmax=9; typeIA=IA_SNIPE; porte=200; force = 10;
            objets[0]=I_BOMBE;objets[1]=I_FLECHE;objets[2]=I_PETIT_COEUR;
            gpZoneBase->getZone()->w=16; gpZoneBase->getZone()->h=16;
            gpZoneBase->getZone()->x=0; gpZoneBase->getZone()->y=8; 
            zonesAttaques = new ZoneRect(0,0,w,h);
            zonesVulnerables = new ZoneRect(0,0,w,h);
            maxglace=40; recul=0;
            for (int k = 0; k < 22; k++) effets[k]=1;
            effets[ENNEMI]=2;
            break;
        case 17 :
            viemax=1; w=16; h=16; animmax=0; typeIA=IA_HAUT_BAS; porte=0; force = 1;
            objets[0]=I_RIEN;objets[1]=I_RIEN;objets[2]=I_RIEN;
            gpZoneBase->getZone()->w=16; gpZoneBase->getZone()->h=16;
            gpZoneBase->getZone()->x=0; gpZoneBase->getZone()->y=0; 
            zonesAttaques = new ZoneRect(2,2,12,12);
            zonesVulnerables = new ZoneRect(0,0,w,h);
            maxglace=0; recul=0; vol=1; vitesse=2; typeEnn=E_MONO_DIR;
            for (int k = 0; k < 22; k++) effets[k]=0;
            break;
        case 18 :
            viemax=1; w=16; h=14; animmax=1; typeIA=IA_ATTEND; porte=70; force = 1;
            objets[0]=I_PETIT_COEUR;objets[1]=I_RUBIS_VERT;objets[2]=I_RUBIS_BLEU;
            gpZoneBase->getZone()->w=16; gpZoneBase->getZone()->h=14;
            gpZoneBase->getZone()->x=0; gpZoneBase->getZone()->y=0; 
            zonesAttaques = new ZoneRect(0,0,w,h);
            zonesVulnerables = new ZoneRect(0,0,w,h);
            maxglace=60; direction=N; typeEnn=E_MONO_DIR; spectre=1; vol=1;
            break;
        case 19 :
            viemax=8; w=64; h=32; animmax=2; typeIA=IA_FONCE; porte=300; force = 2;
            objets[0]=I_RIEN;objets[1]=I_RIEN;objets[2]=I_RIEN;
            gpZoneBase->getZone()->w=w; gpZoneBase->getZone()->h=h;
            gpZoneBase->getZone()->x=0; gpZoneBase->getZone()->y=0; 
            zonesAttaques = new ZoneRect(0,0,w,h);
            zonesVulnerables = new ZoneRect(0,0,w,h);
            typeEnn=E_MONO_DIR; recul=4; spectre=1;
            break;
        case 20 :
            viemax=1; w=16; h=16; animmax=0; typeIA=IA_RIEN; porte=200; force = 0;
            objets[0]=I_RIEN;objets[1]=I_RIEN;objets[2]=I_RIEN;
            gpZoneBase->getZone()->w=w; gpZoneBase->getZone()->h=h;
            gpZoneBase->getZone()->x=0; gpZoneBase->getZone()->y=0; 
            zonesAttaques = new ZoneRect(0,0,w,h);
            zonesVulnerables = new ZoneRect(0,0,w,h);
            typeEnn=E_MONO_DIR; recul=0; maxglace=0;
            break;
        case 21 :
            viemax=2; w=16; h=15; animmax=1; typeIA=IA_FONCE; porte=200; force = 2;
            objets[0]=I_FLECHE;objets[1]=I_RUBIS_BLEU;objets[2]=I_PETIT_COEUR;
            gpZoneBase->getZone()->w=16; gpZoneBase->getZone()->h=15;
            gpZoneBase->getZone()->x=0; gpZoneBase->getZone()->y=0; 
            zonesAttaques = new ZoneRect(0,0,w,h);
            zonesVulnerables = new ZoneRect(0,0,w,h);
            maxglace=30; typeEnn=E_PROFIL; direction=E; nage=1;
            break;
        case 22 :
            viemax=2; w=16; h=23; animmax=1; typeIA=IA_FONCE; porte=200; force = 2;
            objets[0]=I_FLECHE;objets[1]=I_RUBIS_BLEU;objets[2]=I_PETIT_COEUR;
            gpZoneBase->getZone()->w=16; gpZoneBase->getZone()->h=23;
            gpZoneBase->getZone()->x=0; gpZoneBase->getZone()->y=0; 
            zonesAttaques = new ZoneRect(0,0,w,h);
            zonesVulnerables = new ZoneRect(0,0,w,h);
            maxglace=30; direction=N; typeEnn=E_MONO_DIR; spectre=1;
            break;
        case 23 :
            viemax=2; w=26; h=17; animmax=1; typeIA=IA_FONCE; porte=200; force = 4;
            objets[0]=I_FLECHE;objets[1]=I_RUBIS_BLEU;objets[2]=I_PETIT_COEUR;
            gpZoneBase->getZone()->w=26; gpZoneBase->getZone()->h=16;
            gpZoneBase->getZone()->x=0; gpZoneBase->getZone()->y=1; 
            zonesAttaques = new ZoneRect(0,0,w,h);
            zonesVulnerables = new ZoneRect(0,0,w,h);
            maxglace=30; direction=N; typeEnn=E_MONO_DIR;
            break;
        case 24 :
            viemax=16; w=32; h=53; animmax=1; typeIA=IA_FONCE; porte=300; force = 2;
            objets[0]=I_RIEN;objets[1]=I_RIEN;objets[2]=I_RIEN;
            gpZoneBase->getZone()->w=w; gpZoneBase->getZone()->h=h;
            gpZoneBase->getZone()->x=0; gpZoneBase->getZone()->y=0; 
            zonesAttaques = new ZoneRect(0,0,w,h);
            zonesVulnerables = new ZoneRect(0,0,w,h);
            typeEnn=E_MONO_DIR; recul=4; spectre=1;
            break;
        case 25 :
            viemax=7; w=28; h=30; animmax=1; typeIA=IA_FONCE; porte=160; force = 4;
            objets[0]=I_BOMBE;objets[1]=I_RUBIS_ROUGE;objets[2]=I_PETIT_COEUR;
            gpZoneBase->getZone()->w=16; gpZoneBase->getZone()->h=16;
            gpZoneBase->getZone()->x=6; gpZoneBase->getZone()->y=14; 
            zonesAttaques = new ZoneRect(0,0,w,h);
            zonesVulnerables = new ZoneRect(0,0,w,h);
            recul=8; maxglace=100;
            break;
        case 26 :
            viemax=2; w=14; h=16; animmax=1; typeIA=IA_ATTEND; porte=100; force = 6;
            objets[0]=I_RUBIS_ROUGE;objets[1]=I_MAGIE_PEU;objets[2]=I_PETIT_COEUR;
            gpZoneBase->getZone()->w=w; gpZoneBase->getZone()->h=h;
            gpZoneBase->getZone()->x=0; gpZoneBase->getZone()->y=0; 
            zonesAttaques = new ZoneRect(0,0,w,h);
            zonesVulnerables = new ZoneRect(0,0,w,h);
            maxglace=60; direction=N; typeEnn=E_MONO_DIR; spectre=1; vol=1;
            break;
        case 27 :
            viemax=3; w=16; h=17; animmax=10; typeIA=IA_FONCE; porte=300; force = 2;
            objets[0]=I_FLECHE;objets[1]=I_MAGIE_PEU;objets[2]=I_BOMBE;
            gpZoneBase->getZone()->w=16; gpZoneBase->getZone()->h=16;
            gpZoneBase->getZone()->x=0; gpZoneBase->getZone()->y=1; 
            zonesAttaques = new ZoneRect(0,0,w,h);
            zonesVulnerables = new ZoneRect(0,0,w,h);
            break;
        case 28 :
            viemax=16; w=32; h=38; animmax=1; typeIA=IA_FONCE; porte=300; force = 7;
            objets[0]=I_RIEN;objets[1]=I_RIEN;objets[2]=I_RIEN;
            gpZoneBase->getZone()->w=16; gpZoneBase->getZone()->h=16;
            gpZoneBase->getZone()->x=8; gpZoneBase->getZone()->y=22; 
            zonesAttaques = new ZoneRect(0,0,w,h);
            zonesVulnerables = new ZoneRect(0,0,w,h);
            typeEnn=E_MONO_DIR; recul=4;
            break;
        case 29 :
            viemax=1; w=16; h=26; animmax=1; typeIA=IA_FONCE; porte=300; force = 5;
            objets[0]=I_RUBIS_BLEU;objets[1]=I_MAGIE_PEU;objets[2]=I_MAGIE_BCP;
            gpZoneBase->getZone()->w=16; gpZoneBase->getZone()->h=16;
            gpZoneBase->getZone()->x=0; gpZoneBase->getZone()->y=10; 
            zonesAttaques = new ZoneRect(0,0,w,h);
            zonesVulnerables = new ZoneRect(0,0,w,h);
            for (int k = 0; k < 22; k++) effets[k]=1; effets[FEU]=2;
            maxglace=100;
            break;
        case 30 :
            viemax=3; w=16; h=24; animmax=9; typeIA=IA_SNIPE; porte=200; force = 5;
            objets[0]=I_RUBIS_BLEU;objets[1]=I_PETIT_COEUR;objets[2]=I_MAGIE_PEU;
            gpZoneBase->getZone()->w=16; gpZoneBase->getZone()->h=16;
            gpZoneBase->getZone()->x=0; gpZoneBase->getZone()->y=8; 
            zonesAttaques = new ZoneRect(0,0,w,h);
            zonesVulnerables = new ZoneRect(0,0,w,h);
            maxglace=40; recul=0;
            for (int k = 0; k < 22; k++) effets[k]=1;
            effets[ENNEMI]=2;
            break;
        case 31 :
            viemax=60; w=32; h=32; animmax=9; typeIA=IA_SNIPE; porte=300; force = 20;
            objets[0]=I_RIEN;objets[1]=I_RIEN;objets[2]=I_RIEN;
            gpZoneBase->getZone()->w=w; gpZoneBase->getZone()->h=h;
            gpZoneBase->getZone()->x=0; gpZoneBase->getZone()->y=0; 
            zonesAttaques = new ZoneRect(0,0,w,h);
            zonesVulnerables = new ZoneRect(0,0,w,h);
            maxglace=40; recul=0; typeEnn=E_MONO_DIR; vanim=120;
            for (int k = 0; k < 22; k++) effets[k]=1; effets[ENNEMI]=2; 
            break;
        case 32 :
            viemax=5; w=16; h=23; animmax=2; typeIA=IA_FONCE; porte=200; force = 5;
            objets[0]=I_MAGIE_PEU;objets[1]=I_RUBIS_BLEU;objets[2]=I_RUBIS_ROUGE;
            gpZoneBase->getZone()->w=16; gpZoneBase->getZone()->h=16;
            gpZoneBase->getZone()->x=0; gpZoneBase->getZone()->y=7; 
            zonesAttaques = new ZoneRect(0,0,w,h);
            zonesVulnerables = new ZoneRect(0,0,w,h);
            typeEnn=E_MONO_DIR; maxglace=100;
            break;
        case 33 :
            viemax=10; w=22; h=25; animmax=1; typeIA=IA_FONCE; porte=300; force = 6;
            objets[0]=I_RUBIS_BLEU;objets[1]=I_MAGIE_PEU;objets[2]=I_FLECHE;
            gpZoneBase->getZone()->w=16; gpZoneBase->getZone()->h=16;
            gpZoneBase->getZone()->x=3; gpZoneBase->getZone()->y=9; 
            zonesAttaques = new ZoneRect(0,0,w,h);
            zonesVulnerables = new ZoneRect(0,0,w,h);
            maxglace=100;
            break;
        case 34 :
            viemax=5; w=24; h=25; animmax=1; typeIA=IA_FONCE; porte=300; force = 9;
            objets[0]=I_FLECHE;objets[1]=I_RUBIS_ROUGE;objets[2]=I_RUBIS_ROUGE;
            gpZoneBase->getZone()->w=16; gpZoneBase->getZone()->h=16;
            gpZoneBase->getZone()->x=4; gpZoneBase->getZone()->y=9; 
            zonesAttaques = new ZoneRect(0,0,w,h);
            zonesVulnerables = new ZoneRect(0,0,w,h);
            recul=8; maxglace=100;
            for (int k = 0; k < 22; k++) effets[k]=1; effets[FLECHE]=2;
            break;
        case 35 :
            viemax=24; w=32; h=32; animmax=2; typeIA=IA_FONCE; porte=300; force = 8;
            objets[0]=I_RIEN;objets[1]=I_RIEN;objets[2]=I_RIEN;
            gpZoneBase->getZone()->w=w; gpZoneBase->getZone()->h=h;
            gpZoneBase->getZone()->x=0; gpZoneBase->getZone()->y=0; 
            zonesAttaques = new ZoneRect(0,0,w,h);
            zonesVulnerables = new ZoneRect(0,0,w,h);
            typeEnn=E_MONO_DIR; recul=4; spectre=1; maxglace=40;
            break;
        case 36 :
            viemax=1; w=16; h=16; animmax=9; typeIA=IA_STATUE; porte=300; force = 0;
            objets[0]=I_RIEN;objets[1]=I_RIEN;objets[2]=I_RIEN;
            gpZoneBase->getZone()->w=16; gpZoneBase->getZone()->h=16;
            gpZoneBase->getZone()->x=0; gpZoneBase->getZone()->y=0; 
            zonesAttaques = new ZoneRect(0,0,w,h);
            zonesVulnerables = new ZoneRect(0,0,w,h);
            maxglace=0; recul=0; vanim=120; direction=N; typeEnn=E_MONO_DIR;
            for (int k = 0; k < 22; k++) effets[k]=0;
            break;
        case 37 :
            viemax=9; w=16; h=26; animmax=1; typeIA=IA_FONCE; porte=160; force = 6;
            objets[0]=I_BOMBE;objets[1]=I_FLECHE;objets[2]=I_RUBIS_ROUGE;
            gpZoneBase->getZone()->w=16; gpZoneBase->getZone()->h=16;
            gpZoneBase->getZone()->x=0; gpZoneBase->getZone()->y=10; 
            zonesAttaques = new ZoneRect(0,0,w,h);
            zonesVulnerables = new ZoneRect(0,0,w,h);
            break;
        case 38 :
            viemax=15; w=22; h=25; animmax=1; typeIA=IA_FONCE; porte=300; force = 8;
            objets[0]=I_RUBIS_ROUGE;objets[1]=I_MAGIE_PEU;objets[2]=I_MAGIE_BCP;
            gpZoneBase->getZone()->w=16; gpZoneBase->getZone()->h=16;
            gpZoneBase->getZone()->x=3; gpZoneBase->getZone()->y=9; 
            zonesAttaques = new ZoneRect(0,0,w,h);
            zonesVulnerables = new ZoneRect(0,0,w,h);
            maxglace=100;
            break;
        case 39 :
            viemax=10; w=24; h=25; animmax=1; typeIA=IA_FONCE; porte=300; force = 10;
            objets[0]=I_RUBIS_ROUGE;objets[1]=I_RUBIS_ROUGE;objets[2]=I_RUBIS_ROUGE;
            gpZoneBase->getZone()->w=16; gpZoneBase->getZone()->h=16;
            gpZoneBase->getZone()->x=4; gpZoneBase->getZone()->y=9; 
            zonesAttaques = new ZoneRect(0,0,w,h);
            zonesVulnerables = new ZoneRect(0,0,w,h);
            for (int k = 0; k < 22; k++) effets[k]=1; effets[FLECHE]=2; recul=8; maxglace=100;
            break;
        case 40 :
            viemax=20; w=54; h=42; animmax=8; typeIA=IA_FONCE; porte=300; force = 4;
            objets[0]=I_RIEN;objets[1]=I_RIEN;objets[2]=I_RIEN;
            gpZoneBase->getZone()->w=w; gpZoneBase->getZone()->h=h;
            gpZoneBase->getZone()->x=0; gpZoneBase->getZone()->y=0; 
            zonesAttaques = new ZoneRect(0,0,w,h);
            zonesVulnerables = new ZoneRect(0,0,w,h);
            typeEnn=E_MONO_DIR; recul=4; spectre=1; maxglace=100; vanim=120;
            break;
        case 41 :
            viemax=9; w=16; h=24; animmax=1; typeIA=IA_FONCE; porte=300; force = 6;
            objets[0]=I_RUBIS_BLEU;objets[1]=I_FLECHE;objets[2]=I_MAGIE_PEU;
            gpZoneBase->getZone()->w=16; gpZoneBase->getZone()->h=16;
            gpZoneBase->getZone()->x=0; gpZoneBase->getZone()->y=8; 
            zonesAttaques = new ZoneRect(0,0,w,h);
            zonesVulnerables = new ZoneRect(0,0,w,h);
            maxglace=30;
            break;
        case 42 :
            viemax=3; w=26; h=19; animmax=1; typeIA=IA_FONCE; porte=300; force = 10;
            objets[0]=I_MAGIE_PEU;objets[1]=I_MAGIE_PEU;objets[2]=I_MAGIE_BCP;
            gpZoneBase->getZone()->w=26; gpZoneBase->getZone()->h=19;
            gpZoneBase->getZone()->x=0; gpZoneBase->getZone()->y=0; 
            zonesAttaques = new ZoneRect(0,0,w,h);
            zonesVulnerables = new ZoneRect(0,0,w,h);
            maxglace=10; direction=N; typeEnn=E_MONO_DIR;
            for (int k = 0; k < 22; k++) effets[k]=1; effets[FEU]=2;
            break;
        case 43 :
            viemax=30; w=32; h=32; animmax=2; typeIA=IA_FONCE; porte=300; force = 10;
            gpZoneBase->getZone()->w=w; gpZoneBase->getZone()->h=h;
            gpZoneBase->getZone()->x=0; gpZoneBase->getZone()->y=0; 
            zonesAttaques = new ZoneRect(0,0,w,h);
            zonesVulnerables = new ZoneRect(0,0,w,h);
            recul=4; spectre=1; maxglace=30;
            break;
        case 44 :
            viemax=9; w=22; h=26; animmax=1; typeIA=IA_FONCE; porte=300; force = 8;
            objets[0]=I_RUBIS_BLEU;objets[1]=I_FLECHE;objets[2]=I_PETIT_COEUR;
            gpZoneBase->getZone()->w=16; gpZoneBase->getZone()->h=16;
            gpZoneBase->getZone()->x=3; gpZoneBase->getZone()->y=10; 
            zonesAttaques = new ZoneRect(0,0,w,h);
            zonesAttaques->ajout(new ZoneRect(0,20,7,15));
            zonesVulnerables = new ZoneRect(0,0,w,h); arme = true;
            break;
        case 45 :
            viemax=20; w=66; h=76; animmax=6; typeIA=IA_FONCE; porte=300; force = 6;
            objets[0]=I_RIEN;objets[1]=I_RIEN;objets[2]=I_RIEN;
            gpZoneBase->getZone()->w=w; gpZoneBase->getZone()->h=h;
            gpZoneBase->getZone()->x=0; gpZoneBase->getZone()->y=0; 
            zonesAttaques = new ZoneRect(0,0,66,64);
            zonesAttaques->ajout(new ZoneRect(0,67,16,9));
            zonesAttaques->ajout(new ZoneRect(50,59,16,9));
            zonesVulnerables = new ZoneRect(9,29,48,38);
            for (int k = 0; k < 22; k++) effets[k]=0; effets[COUP_MARTEAU]=2;
            typeEnn=E_MONO_DIR; recul=4; spectre=1; maxglace=40; vanim = 120;
            break;
        case 46 :
            viemax=6; w=22; h=27; animmax=1; typeIA=IA_FONCE; porte=300; force = 10;
            objets[0]=I_RUBIS_VERT;objets[1]=I_MAGIE_PEU;objets[2]=I_MAGIE_BCP;
            gpZoneBase->getZone()->w=16; gpZoneBase->getZone()->h=16;
            gpZoneBase->getZone()->x=3; gpZoneBase->getZone()->y=11; 
            zonesAttaques = new ZoneRect(0,0,w,h);
            zonesAttaques->ajout(new ZoneRect(0,21,5,16));
            zonesVulnerables = new ZoneRect(0,0,w,h); arme = true;
            break;
        case 47 :
            viemax=12; w=22; h=27; animmax=1; typeIA=IA_FONCE; porte=300; force = 20;
            objets[0]=I_RUBIS_BLEU;objets[1]=I_FLECHE;objets[2]=I_BOMBE;
            gpZoneBase->getZone()->w=16; gpZoneBase->getZone()->h=16;
            gpZoneBase->getZone()->x=3; gpZoneBase->getZone()->y=11; 
            zonesAttaques = new ZoneRect(0,0,w,h);
            zonesAttaques->ajout(new ZoneRect(0,21,5,16));
            zonesVulnerables = new ZoneRect(0,0,w,h); arme = true;
            break;
        case 48 :
            viemax=18; w=22; h=27; animmax=1; typeIA=IA_FONCE; porte=300; force = 30;
            objets[0]=I_RUBIS_ROUGE;objets[1]=I_PETIT_COEUR;objets[2]=I_PETIT_COEUR;
            gpZoneBase->getZone()->w=16; gpZoneBase->getZone()->h=16;
            gpZoneBase->getZone()->x=3; gpZoneBase->getZone()->y=11; 
            zonesAttaques = new ZoneRect(0,0,w,h);
            zonesAttaques->ajout(new ZoneRect(0,21,5,17));
            zonesVulnerables = new ZoneRect(0,0,w,h); arme = true;
            break;
        case 49 :
            viemax=1; w=16; h=16; animmax=9; typeIA=IA_STATUE; porte=300; force = 0;
            objets[0]=I_RIEN;objets[1]=I_RIEN;objets[2]=I_RIEN;
            gpZoneBase->getZone()->w=16; gpZoneBase->getZone()->h=16;
            gpZoneBase->getZone()->x=0; gpZoneBase->getZone()->y=0; 
            zonesAttaques = new ZoneRect(0,0,w,h);
            zonesVulnerables = new ZoneRect(0,0,w,h);
            maxglace=0; recul=0; vanim=120; direction=N; typeEnn=E_MONO_DIR;
            for (int k = 0; k < 22; k++) effets[k]=0;
            break;
        case 50 :
            viemax=200; w=52; h=36; animmax=1; typeIA=IA_FONCE; porte=300; force = 50;
            objets[0]=I_RIEN;objets[1]=I_RIEN;objets[2]=I_RIEN;
            gpZoneBase->getZone()->w=w; gpZoneBase->getZone()->h=h;
            gpZoneBase->getZone()->x=0; gpZoneBase->getZone()->y=0; 
            zonesAttaques = new ZoneRect(0,0,w,h);
            zonesVulnerables = new ZoneRect(0,0,w,h);
            typeEnn=E_SIMPLE; recul=4; spectre=1; maxglace=60; vanim=120; vinvin=5;
            for (int k = 0; k < 22; k++) effets[k]=1;
            if (gpJeu->getJoueur()->getEpee()==5) {effets[COUP_EPEE]=2; effets[EPEE_SPIN]=2;}
            if (gpJeu->getJoueur()->hasObjet(O_ARC)==5) effets[FLECHE]=2; 
            break;
        case 51 :
            viemax=1; w=24; h=22; animmax=40; typeIA=IA_SPECIAL; porte=300; force = 0;
            objets[0]=I_RIEN;objets[1]=I_RIEN;objets[2]=I_RIEN;
            gpZoneBase->getZone()->w=w; gpZoneBase->getZone()->h=h;
            gpZoneBase->getZone()->x=0; gpZoneBase->getZone()->y=0; 
            zonesAttaques = new ZoneRect(0,0,w,h);
            zonesVulnerables = new ZoneRect(0,0,w,h);
            typeEnn=E_MONO_DIR; recul=0; spectre=1; maxglace=0; vanim=120;
            for (int k = 0; k < 22; k++) effets[k]=0;
            break;
        case 52 :
            viemax=100; w=16; h=17; animmax=1; typeIA=IA_RANDOM; porte=1000; force = 2;
            objets[0]=I_RIEN;objets[1]=I_RIEN;objets[2]=I_RIEN;
            gpZoneBase->getZone()->w=16; gpZoneBase->getZone()->h=16;
            gpZoneBase->getZone()->x=0; gpZoneBase->getZone()->y=1; 
            zonesAttaques = new ZoneRect(0,0,w,h);
            zonesVulnerables = new ZoneRect(0,0,w,h);
            maxglace=0; typeEnn=E_PROFIL;
            break;
        case 53 :
            viemax=1; w=14; h=15; animmax=1; typeIA=IA_FONCE; porte=100; force = 1;
            objets[0]=I_RUBIS_VERT;objets[1]=I_RUBIS_VERT;objets[2]=I_MAGIE_PEU;
            gpZoneBase->getZone()->w=14; gpZoneBase->getZone()->h=15;
            gpZoneBase->getZone()->x=0; gpZoneBase->getZone()->y=0; 
            zonesAttaques = new ZoneRect(0,0,w,h);
            zonesVulnerables = new ZoneRect(0,0,w,h);
            maxglace=60; typeEnn=E_PROFIL; direction=E;
            break;
        case 54 :
            viemax=2; w=16; h=17; animmax=1; typeIA=IA_ATTEND; porte=100; force = 2;
            objets[0]=I_PETIT_COEUR;objets[1]=I_PETIT_COEUR;objets[2]=I_RUBIS_BLEU;
            gpZoneBase->getZone()->w=17; gpZoneBase->getZone()->h=17;
            gpZoneBase->getZone()->x=0; gpZoneBase->getZone()->y=0; 
            zonesAttaques = new ZoneRect(0,0,w,h);
            zonesVulnerables = new ZoneRect(0,0,w,h);
            maxglace=60; typeEnn=E_PROFIL; spectre=1; vol=1; direction=E;
            break;
        case 55 :
            viemax=3; w=22; h=26; animmax=1; typeIA=IA_FONCE; porte=200; force = 4;
            objets[0]=I_PETIT_COEUR;objets[1]=I_RUBIS_BLEU;objets[2]=I_FLECHE;
            gpZoneBase->getZone()->w=16; gpZoneBase->getZone()->h=16;
            gpZoneBase->getZone()->x=3; gpZoneBase->getZone()->y=10; 
            zonesAttaques = new ZoneRect(0,0,w,h);
            zonesAttaques->ajout(new ZoneRect(0,20,7,15));
            zonesVulnerables = new ZoneRect(0,0,w,h); arme = true;
            break;
        case 56 :
            viemax=10; w=17; h=17; animmax=2; typeIA=IA_FONCE; porte=150; force = 12;
            objets[0]=I_RUBIS_BLEU;objets[1]=I_MAGIE_PEU;objets[2]=I_PETIT_COEUR;
            gpZoneBase->getZone()->w=17; gpZoneBase->getZone()->h=17;
            gpZoneBase->getZone()->x=0; gpZoneBase->getZone()->y=0; 
            zonesAttaques = new ZoneRect(0,0,w,h);
            zonesVulnerables = new ZoneRect(0,0,w,h);
            maxglace=30; typeEnn=E_PROFIL; spectre=1; direction=E;
            for (int k = 0; k < 22; k++) effets[k]=1;
            if (gpJeu->getJoueur()->getEpee()>=2) {effets[COUP_EPEE]=2; effets[EPEE_SPIN]=2;}
            if (gpJeu->getJoueur()->hasObjet(O_ARC)==5) effets[FLECHE]=2; 
            break;
        case 57 :
            viemax=8; w=48; h=65; animmax=40; typeIA=IA_SNIPE; porte=300; force = 1;
            objets[0]=I_RIEN;objets[1]=I_RIEN;objets[2]=I_RIEN;
            gpZoneBase->getZone()->w=w; gpZoneBase->getZone()->h=h;
            gpZoneBase->getZone()->x=0; gpZoneBase->getZone()->y=0; 
            zonesAttaques = new ZoneRect(12,0,24,h);
            zonesAttaques->ajout(new ZoneRect(0,41,w,24));
            zonesVulnerables = new ZoneRect(0,0,w,h); recul=0; typeEnn=E_PROFIL;
            break;
        case 58 :
            viemax=0; w=14; h=15; animmax=1; typeIA=IA_RIEN; porte=100; force = 0;
            objets[0]=I_RUBIS_VERT;objets[1]=I_RUBIS_VERT;objets[2]=I_MAGIE_PEU;
            gpZoneBase->getZone()->w=14; gpZoneBase->getZone()->h=15;
            gpZoneBase->getZone()->x=0; gpZoneBase->getZone()->y=0; 
            zonesAttaques = new ZoneRect(0,0,w,h);
            zonesVulnerables = new ZoneRect(0,0,w,h);
            maxglace=60; typeEnn=E_PROFIL; direction=E;
            for (int k = 0; k < 22; k++) effets[k]=0;
            break;
        case 59 :
            viemax=999; w=16; h=16; animmax=0; typeIA=IA_RIEN; porte=300; force = 1;
            objets[0]=I_RIEN;objets[1]=I_RIEN;objets[2]=I_RIEN;
            gpZoneBase->getZone()->w=w; gpZoneBase->getZone()->h=h;
            gpZoneBase->getZone()->x=0; gpZoneBase->getZone()->y=0; 
            zonesAttaques = new ZoneRect(0,0,w,h);
            zonesVulnerables = new ZoneRect(0,0,w,h); recul=0; typeEnn=E_MONO_DIR;
            for (int k = 0; k < 22; k++) effets[k]=0;
            break;
        case 60 :
            viemax=12; w=50; h=66; animmax=3; typeIA=IA_FONCE; porte=300; force = 1;
            objets[0]=I_RIEN;objets[1]=I_RIEN;objets[2]=I_RIEN;
            gpZoneBase->getZone()->w=32; gpZoneBase->getZone()->h=24;
            gpZoneBase->getZone()->x=9; gpZoneBase->getZone()->y=42; 
            zonesAttaques = new ZoneRect(0,0,50,36);
            zonesAttaques->ajout(new ZoneRect(9,36,32,26));
            zonesVulnerables = new ZoneRect(9,36,32,26); recul=4;
            break;
        case 61 :
            viemax=1; w=128; h=16; animmax=3; typeIA=IA_HAUT_BAS; porte=200; force = 1;
            objets[0]=I_RIEN;objets[1]=I_RIEN;objets[2]=I_RIEN;
            gpZoneBase->getZone()->w=w; gpZoneBase->getZone()->h=h;
            gpZoneBase->getZone()->x=0; gpZoneBase->getZone()->y=0; 
            zonesAttaques = new ZoneRect(0,0,w,h);
            zonesVulnerables = new ZoneRect(0,0,w,h);
            maxglace=0; recul=0; vol=1; vitesse=2; typeEnn=E_MONO_DIR;
            for (int k = 0; k < 22; k++) effets[k]=0;
            break;
        case 62 :
            viemax=10; w=24; h=37; animmax=1; typeIA=IA_FONCE; porte=300; force = 2;
            objets[0]=I_RIEN;objets[1]=I_RIEN;objets[2]=I_RIEN;
            gpZoneBase->getZone()->w=w; gpZoneBase->getZone()->h=16;
            gpZoneBase->getZone()->x=0; gpZoneBase->getZone()->y=21; 
            zonesAttaques = new ZoneRect(0,0,w,h);
            zonesVulnerables = new ZoneRect(0,0,w,h); recul=4; vanim=120;
            break;
        case 63 :
            viemax=30; w=58; h=68; animmax=0; typeIA=IA_FONCE; porte=300; force = 6;
            objets[0]=I_RIEN;objets[1]=I_RIEN;objets[2]=I_RIEN;
            gpZoneBase->getZone()->w=w; gpZoneBase->getZone()->h=24;
            gpZoneBase->getZone()->x=0; gpZoneBase->getZone()->y=42; 
            zonesAttaques = new ZoneRect(13,0,32,68); vanim=120;
            zonesAttaques->ajout(new ZoneRect(5,28,48,20)); type_att = ASPIRE;
            zonesVulnerables = new ZoneRect(13,0,32,68); direction=S; typeEnn=E_MONO_DIR;
            zonesVulnerables->ajout(new ZoneRect(5,28,48,20)); recul=4; spectre=1;
            for (int k = 0; k < 22; k++) effets[k]=1;
            if (gpJeu->getJoueur()->getEpee()>=2) {effets[COUP_EPEE]=2; effets[EPEE_SPIN]=2;}
            if (gpJeu->getJoueur()->hasObjet(O_ARC)==5) effets[FLECHE]=2; 
            break;
        case 64 :
            viemax=10; w=40; h=36; animmax=3; typeIA=IA_FONCE; porte=300; force = 12;
            objets[0]=I_BOMBE;objets[1]=I_RUBIS_BLEU;objets[2]=I_RUBIS_ROUGE;
            gpZoneBase->getZone()->w=20; gpZoneBase->getZone()->h=16;
            gpZoneBase->getZone()->x=9; gpZoneBase->getZone()->y=19; 
            zonesAttaques = new ZoneRect(9,0,20,h);
            zonesVulnerables = new ZoneRect(9,0,20,h); recul=4; vanim=120; maxglace=100;
            break;
        case 65 :
            viemax=1; w=16; h=16; animmax=9; typeIA=IA_STATUE; porte=300; force = 0;
            objets[0]=I_RIEN;objets[1]=I_RIEN;objets[2]=I_RIEN;
            gpZoneBase->getZone()->w=16; gpZoneBase->getZone()->h=16;
            gpZoneBase->getZone()->x=0; gpZoneBase->getZone()->y=0; 
            zonesAttaques = new ZoneRect(0,0,w,h);
            zonesVulnerables = new ZoneRect(0,0,w,h);
            maxglace=0; recul=0; vanim=120; direction=N; typeEnn=E_MONO_DIR;
            for (int k = 0; k < 22; k++) effets[k]=0;
            break;
        case 66 :
            viemax=50; w=96; h=0; animmax=3; typeIA=IA_RIEN; porte=300; force = 8;
            objets[0]=I_RIEN;objets[1]=I_RIEN;objets[2]=I_RIEN;
            gpZoneBase->getZone()->w=w; gpZoneBase->getZone()->h=0;
            gpZoneBase->getZone()->x=0; gpZoneBase->getZone()->y=0; 
            zonesAttaques = new ZoneRect(0,0,w,0);
            zonesVulnerables = new ZoneRect(0,0,w,0);
            maxglace=60; recul=0; vanim=120; direction=S; typeEnn=E_MONO_DIR;
            for (int k = 0; k < 22; k++) effets[k]=0; vol=1;
            break;
        case 67 :
            viemax=0; w=8; h=0; animmax=24; typeIA=IA_SPECIAL; porte=300; force = 0;
            objets[0]=I_RIEN;objets[1]=I_RIEN;objets[2]=I_RIEN;
            gpZoneBase->getZone()->w=w; gpZoneBase->getZone()->h=h;
            gpZoneBase->getZone()->x=0; gpZoneBase->getZone()->y=0; 
            zonesAttaques = new ZoneRect(0,0,w,h);
            zonesVulnerables = new ZoneRect(0,0,w,h);
            maxglace=60; recul=0; vanim=120; direction=N; typeEnn=E_MONO_DIR;
            for (int k = 0; k < 22; k++) effets[k]=0; vol=1;
            break;
        case 68 :
            viemax=0; w=26; h=0; animmax=3; typeIA=IA_SPECIAL; porte=300; force = 8;
            objets[0]=I_RIEN;objets[1]=I_RIEN;objets[2]=I_RIEN;
            gpZoneBase->getZone()->w=w; gpZoneBase->getZone()->h=h;
            gpZoneBase->getZone()->x=0; gpZoneBase->getZone()->y=0; 
            zonesAttaques = new ZoneRect(0,0,w,h);
            zonesVulnerables = new ZoneRect(0,0,w,h);
            maxglace=60; recul=0; vanim=120; direction=E; typeEnn=E_MONO_DIR;
            for (int k = 0; k < 22; k++) effets[k]=0; vol=1;
            break;
        case 69 :
            viemax=0; w=26; h=0; animmax=3; typeIA=IA_SPECIAL; porte=300; force = 8;
            objets[0]=I_RIEN;objets[1]=I_RIEN;objets[2]=I_RIEN;
            gpZoneBase->getZone()->w=w; gpZoneBase->getZone()->h=h;
            gpZoneBase->getZone()->x=0; gpZoneBase->getZone()->y=0; 
            zonesAttaques = new ZoneRect(0,0,w,h);
            zonesVulnerables = new ZoneRect(0,0,w,h);
            maxglace=60; recul=0; vanim=120; direction=O; typeEnn=E_MONO_DIR;
            for (int k = 0; k < 22; k++) effets[k]=0; vol=1;
            break;
        case 70 :
            viemax=1; w=1; h=144; animmax=0; typeIA=IA_RIEN; porte=300; force = 0;
            objets[0]=I_RIEN;objets[1]=I_RIEN;objets[2]=I_RIEN;
            gpZoneBase->getZone()->w=w; gpZoneBase->getZone()->h=h;
            gpZoneBase->getZone()->x=0; gpZoneBase->getZone()->y=0; 
            zonesAttaques = new ZoneRect(0,0,w,h);
            zonesVulnerables = new ZoneRect(0,0,w,h);
            maxglace=0; recul=0; vanim=120; direction=O; typeEnn=E_MONO_DIR;
            for (int k = 0; k < 22; k++) effets[k]=0; spectre=1;
            break;
        case 71 :
            viemax=30; w=52; h=55; animmax=3; typeIA=IA_FONCE; porte=300; force = 12;
            objets[0]=I_RIEN;objets[1]=I_RIEN;objets[2]=I_RIEN;
            gpZoneBase->getZone()->w=32; gpZoneBase->getZone()->h=16;
            gpZoneBase->getZone()->x=10; gpZoneBase->getZone()->y=34; 
            zonesAttaques = new ZoneRect(10,0,32,h);
            zonesVulnerables = new ZoneRect(10,0,32,h);
            maxglace=0; recul=0; vanim=120; vol=1; dist=32;
            break;
        case 72 :
            viemax=1; w=16; h=26; animmax=1; typeIA=IA_FONCE; porte=200; force = 12;
            objets[0]=I_RIEN;objets[1]=I_RIEN;objets[2]=I_RIEN;
            gpZoneBase->getZone()->w=16; gpZoneBase->getZone()->h=16;
            gpZoneBase->getZone()->x=0; gpZoneBase->getZone()->y=10; 
            zonesAttaques = new ZoneRect(0,0,w,h);
            zonesVulnerables = new ZoneRect(0,0,w,h); maxglace=15;
            for (int k = 0; k < 22; k++) effets[k]=1;
            break;
        case 73 :
            viemax=60; w=104; h=153; animmax=3; typeIA=IA_RIEN; porte=300; force = 24;
            objets[0]=I_RIEN;objets[1]=I_RIEN;objets[2]=I_RIEN; vanim=120;
            gpZoneBase->getZone()->w=w; gpZoneBase->getZone()->h=h;
            gpZoneBase->getZone()->x=0; gpZoneBase->getZone()->y=0; 
            zonesAttaques = new ZoneRect(30,0,14,h); direction=E; typeEnn=E_MONO_DIR;
            zonesAttaques->ajout(new ZoneRect(30,30,40,74));
            zonesVulnerables = new ZoneRect(30,30,40,74); maxglace=0; recul=0;
            for (int k = 0; k < 22; k++) effets[k]=0; effets[COUP_GLACE]=2; spectre=1;
            break;
        case 74 :
            viemax=90; w=104; h=153; animmax=3; typeIA=IA_RIEN; porte=300; force = 24;
            objets[0]=I_RIEN;objets[1]=I_RIEN;objets[2]=I_RIEN; vanim=120;
            gpZoneBase->getZone()->w=w; gpZoneBase->getZone()->h=h;
            gpZoneBase->getZone()->x=0; gpZoneBase->getZone()->y=0; 
            zonesAttaques = new ZoneRect(60,0,14,h); direction=O; typeEnn=E_MONO_DIR;
            zonesAttaques->ajout(new ZoneRect(24,30,40,74));
            zonesVulnerables = new ZoneRect(24,30,40,74); maxglace=0; recul=0;
            for (int k = 0; k < 22; k++) effets[k]=0; effets[FEU]=2; spectre=1;
            break;
        case 75 :
            viemax=100; w=19; h=24; animmax=7; typeIA=IA_FONCE; porte=300; force = 1;
            objets[0]=I_RIEN;objets[1]=I_RIEN;objets[2]=I_RIEN; vanim=120; vinvin=5;
            gpZoneBase->getZone()->w=16; gpZoneBase->getZone()->h=16;
            gpZoneBase->getZone()->x=0; gpZoneBase->getZone()->y=8; 
            zonesAttaques = new ZoneRect(0,0,w,h); type_att=DEMI;
            zonesAttaques->ajout(new ZoneRect(0,0,w,h));
            zonesVulnerables = new ZoneRect(0,0,w,h); maxglace=0;
            for (int k = 0; k < 22; k++) effets[k]=0; effets[COUP_EPEE]=2; effets[EPEE_SPIN]=2;
            break;
        case 76 :
            viemax=50; w=60; h=51; animmax=5; typeIA=IA_FONCE; porte=300; force = 20;
            objets[0]=I_RIEN;objets[1]=I_RIEN;objets[2]=I_RIEN; vanim=120; vinvin=5;
            gpZoneBase->getZone()->w=32; gpZoneBase->getZone()->h=24;
            gpZoneBase->getZone()->x=11; gpZoneBase->getZone()->y=18; 
            zonesAttaques = new ZoneRect(0,0,w,43);
            zonesVulnerables = new ZoneRect(0,0,w,43); recul=4; typeEnn=E_PROFIL;
            break;
    }
    vie=viemax;
    lastAnimTime = SDL_GetTicks();
}
