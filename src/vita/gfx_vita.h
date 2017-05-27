
#ifndef __GFX_VITA_H__
#define __GFX_VITA_H__

#include <vita2d.h>
#include <vitasdk.h>
#include "gfx_vita.h"


typedef struct SDL_Surface {
        uint32_t flags;                           /* Read-only */
//        SDL_PixelFormat *format;                /* Read-only */
        int format;                /* Read-only */
        int w, h;                               /* Read-only */
//        Uint16 pitch;                           /* Read-only */
//        void *pixels;                           /* Read-write */
		vita2d_texture* texture;
//		int colorKey;

//        SDL_Rect clip_rect;                     /* Read-only */
//        int refcount;                           /* Read-mostly */
} SDL_Surface;

typedef struct{
  int x, y;
  int w, h;
} SDL_Rect;

typedef struct{
  int x, y;
} SDL_Point;


#define SDL_SRCCOLORKEY 1
#define SDL_FULLSCREEN 1
#define SDL_DOUBLEBUF 2
#define SDL_HWSURFACE 4

SDL_Surface* SDL_SetVideoMode(int width, int height, int bpp, int flags);

SDL_Surface* IMG_Load(const char* f);
SDL_Surface* SDL_CreateRGBSurface(int type, int width, int height, int bpp, int a, int b, int c, int d);

void SDL_FreeSurface(SDL_Surface* s);

void SDL_SetColorKey(SDL_Surface* s, int flag, uint32_t color);

uint32_t SDL_MapRGB(int flag , uint8_t r, uint8_t g, uint8_t b);


void SDL_FillRect(SDL_Surface* s, SDL_Rect* rect, uint32_t color);
void filledEllipseRGBA(SDL_Surface* s, int x, int y, int rx, int ry, uint8_t r, uint8_t g, uint8_t b, uint8_t a);

void SDL_BlitSurface(SDL_Surface* s, SDL_Rect * src, SDL_Surface* d, SDL_Rect * dst);

void SDL_Flip(void * p);

#endif