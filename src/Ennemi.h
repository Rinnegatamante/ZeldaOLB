/*

    Zelda Oni Link Begins

    Copyright (C) 2006-2008  Vincent Jouillat

    Please send bugreports with examples or suggestions to www.zeldaroth.fr

*/

#ifndef __ENNEMI_H__
#define __ENNEMI_H__

#include "Personnage.h"
#include "ZoneRect.h"

/*struct noeud{
    int cout_g, cout_h, cout_f;
    std::pair<int,int> coord;
    noeud* parent;
    bool operator<(const noeud n2) const {
        return (cout_f) <= (n2.cout_f);
    }
};

typedef std::list<noeud> l_noeud;
*/

class Jeu;

class Ennemi : public Personnage {
    public :
        Ennemi(Jeu* jeu, int type, int i, int j, bool p);
        ~Ennemi();
        void draw(SDL_Surface* gpScreen);
        void drawSpecial(SDL_Surface* gpScreen);
        void moveX(int i);
        void moveY(int i);
        void moveX(int i, int v, int n, int s);
        void moveY(int i, int v, int n, int s);
        void revie();
        Ennemi* getSuivant();
        int getEffet(int i);
        ZoneRect* getZonesVulnerables();
        void setInvincible(int i);
        void setVie(int i);
        void pousse(Direction dir);
        int getObjets(int i);
        void gele();
        void degele();
        int getEnnId();
        void enerve();
        void permute(int val);
        bool isBoss();
        void reveil();
        void testTapis();
        void capture();
        void mort();
    private :
        void init();
        void IA();
        void bouge();
        void attend();
        void deplace();
        void hautbas();
        void regard();
        bool distance();
        void snipe();
        void tire();
        void special();
        void drawArme(SDL_Surface* gpScreen);
        void ajuste();
        void renfort();
        
        Jeu* gpJeu;
        int id;
        int vanim;
        int vinvin;
        int vol;
        int nage;
        int spectre;
        int porte;
        int typeIA;
        int vitesse;
        int force;
        int recul;
        int glace;
        int maxglace;
        int typeEnn;
        ZoneRect* zonesAttaques;
        ZoneRect* zonesVulnerables;
        bool chargeIm;
        bool arme;
        int special1;
        int special2;
        bool persistent;
        int type_att;
        int dist;
        int effets[22]; //0 rien, 1 repousse, 2 dégats
        int objets[3];  //objets laissés après vaincu
};

#endif  // Ennemi.h
