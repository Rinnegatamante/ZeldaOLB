/*

    Zelda Oni Link Begins

    Copyright (C) 2006-2008  Vincent Jouillat

    Please send bugreports with examples or suggestions to www.zeldaroth.fr

*/

#include "ZoneRect.h"
#include "vita/gfx_vita.h"

ZoneRect::ZoneRect(SDL_Rect* zn) : Listable(), zone(zn) {
}

ZoneRect::ZoneRect(int x, int y, int w, int h) : Listable() {
    zone = new SDL_Rect;
    zone->x=x; zone->y=y; zone->w=w; zone->h=h;
}

ZoneRect::~ZoneRect() {
    delete zone;
	if(suivant)	{
		ZoneRect* temp;
		temp = (ZoneRect*)suivant;
		suivant=NULL;
		delete temp;
	}
}

void ZoneRect::setZone(SDL_Rect* zn) {
    delete zone;
    zone = zn;
}
        
SDL_Rect* ZoneRect::getZone() {
    return zone;
}

ZoneRect* ZoneRect::getSuivant() {
    return (ZoneRect*)suivant;
}
