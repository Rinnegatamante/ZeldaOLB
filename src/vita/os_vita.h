
#ifndef __OS_VITA_H__
#define __OS_VITA_H__


#include <stdlib.h>
#include <vitasdk.h>

#define SDL_INIT_VIDEO 1
typedef int Uint32;
typedef uint8_t Uint8;

/*
Languages
ID 	Description
0 	JP
1 	EN Supported and default for unsupported
2 	FR Supported
3 	DE Supported
4 	IT 
5 	ES Supported
6 	ZH
7 	KO
8 	NL
9 	PT
10 	RU
11 	TW 
*/

int SDL_Init(int flag); //returns 0 on success, -1 on failure 
void SDL_Quit();

int SDL_GetTicks();
void SDL_Delay(int ms);

// stubbed win functions
void SDL_WM_SetCaption(const char* caption,void* p);
void SDL_WM_SetIcon(void* i, void* p);

#define SDL_Event int

#define SDL_QUIT 1<<1
#define SDL_KEYDOWN 1<<2
#define SDL_FINGERDOWN 1<<3

int SDL_PollEvent(int * event);
int getTouchX();
int getTouchY();

int SDL_GetKeyState(void* p);

#define SDL_DISABLE 0
#define SDL_ENABLE 1
void SDL_ShowCursor(int s);

int getLanguage(void);
void setLanguage(int languageID);


#define SDLK_ESCAPE	SCE_CTRL_SELECT
#define SDLK_F1 	SCE_CTRL_START
#define SDLK_F4 	0
#define SDLK_RETURN	SCE_CTRL_CROSS
#define SDLK_LEFT	SCE_CTRL_LEFT
#define SDLK_RIGHT	SCE_CTRL_RIGHT
#define SDLK_UP		SCE_CTRL_UP
#define SDLK_DOWN	SCE_CTRL_DOWN

#define SDLK_CAPSLOCK 0
#define SDLK_LSHIFT SCE_CTRL_RTRIGGER
#define SDLK_LCTRL	SCE_CTRL_LTRIGGER
#define SDLK_z		SCE_CTRL_CIRCLE
#define SDLK_w		0
#define SDLK_x		SCE_CTRL_SQUARE
#define SDLK_c		SCE_CTRL_TRIANGLE
#define SDLK_SPACE	SCE_CTRL_CROSS
#define SDLK_p 		SCE_CTRL_START
#define SDLK_m 		0
#define SDLK_t 		SCE_CTRL_RTRIGGER
#define SDLK_o 		SCE_CTRL_TRIANGLE
#define SDLK_SEMICOLON 0

#define KMOD_ALT SCE_CTRL_LTRIGGER
#define KMOD_CTRL 0

#define SDLK_0 0
#define SDLK_1 0
#define SDLK_2 0
#define SDLK_3 0
#define SDLK_4 0
#define SDLK_5 0
#define SDLK_6 0
#define SDLK_7 0
#define SDLK_8 0
#define SDLK_9 0

#define SDLK_KP0 0
#define SDLK_KP1 0
#define SDLK_KP2 0
#define SDLK_KP3 0
#define SDLK_KP4 0
#define SDLK_KP5 0
#define SDLK_KP6 0
#define SDLK_KP7 0
#define SDLK_KP8 0
#define SDLK_KP9 0


#endif