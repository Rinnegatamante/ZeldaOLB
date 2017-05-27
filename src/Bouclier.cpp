/*

    Zelda Oni Link Begins

    Copyright (C) 2006-2008  Vincent Jouillat

    Please send bugreports with examples or suggestions to www.zeldaroth.fr

*/

#include "Bouclier.h"
#include "Common.h"
#include "vita/gfx_vita.h"

Bouclier::Bouclier(SDL_Surface* img, int id) : image(img), type(id) {
    SDL_SetColorKey(image,SDL_SRCCOLORKEY,SDL_MapRGB(image->format,0,0,255));
    zone = NULL;
}

Bouclier::~Bouclier() {
    SDL_FreeSurface(image);
    if(zone) delete zone;
}

void Bouclier::draw(SDL_Surface* screen, int x, int y, ZoneRect* z, int direction) {
    setZone(z);
    
    if (type == 2) {
        switch (direction) {
            case N :
                zone->getZone()->x--; zone->getZone()->y-=2;
                zone->getZone()->w+=3; zone->getZone()->h+=2;
                break;
            case S :
                zone->getZone()->x-=2; zone->getZone()->y-=2;
                zone->getZone()->w+=3; zone->getZone()->h+=2;
                x+=3;
                break;
            case O :
            case E :
                x+=6;
                zone->getZone()->y--; zone->getZone()->h+=2;
                break;
        }
    }
    if (type == 3) {
        switch (direction) {
            case N :
                zone->getZone()->x-=2; zone->getZone()->y-=4;
                zone->getZone()->w+=5; zone->getZone()->h+=5;
                break;
            case S :
                zone->getZone()->x-=3; zone->getZone()->y-=4;
                zone->getZone()->w+=5; zone->getZone()->h+=5;
                x+=5;
                break;
            case O :
            case E :
                x+=10;
                zone->getZone()->y--; zone->getZone()->h+=4;
                break;
        }
    }
    if (type == 10) {
        switch (direction) {
            case N :
                zone->getZone()->x--;
                zone->getZone()->w+=2; zone->getZone()->h++;
                break;
            case S :
                zone->getZone()->x--; zone->getZone()->y--;
                zone->getZone()->w+=2; zone->getZone()->h++;
                x+=2;
                break;
            case O :
            case E :
                x+=4;
                zone->getZone()->h++;
                break;
        }
    }
    
    SDL_Rect src;
    src.x = x;
    src.y = y;
    src.w = zone->getZone()->w;
    src.h = zone->getZone()->h;
    
    SDL_Rect dst;
    dst.x = zone->getZone()->x;
    dst.y = zone->getZone()->y;
    
    SDL_BlitSurface(image, &src, screen, &dst);
    
}

void Bouclier::setZone(ZoneRect* z) {
    if(zone) delete zone;
    zone = z;
}
