
#include <sys/stat.h>
#include <vita2d.h>
#include "os_vita.h"
#include "sound_vita.h"

#define TICKS_PER_MSEC (268123.480)

uint8_t language = 1;
extern vita2d_texture* fbo;

int SDL_Init(int flag) //returns 0 on success, -1 on failure 
{
    vita2d_init();
	fbo = vita2d_create_empty_texture_format_advanced(960,544,SCE_GXM_TEXTURE_FORMAT_A8B8G8R8);
	vita2d_set_vblank_wait(true);
	
	scePowerSetArmClockFrequency(444);
	scePowerSetBusClockFrequency(222);
	scePowerSetGpuClockFrequency(222);
	scePowerSetGpuXbarClockFrequency(166);
	
	// Read the language field from the config savegame.
	//CFGU_GetSystemLanguage(&language);
	
	// In case game dir doesn't exist
    sceIoMkdir("ux0:data/ZeldaOLB", 0777);

    return 0;
}


void SDL_Quit()
{
	vita2d_fini();
}

int SDL_GetTicks()
{
	SceRtcTick tick;
	sceRtcGetCurrentTick(&tick);
    return tick.tick;
}

void SDL_Delay(int ms)
{
	sceKernelDelayThread(ms*1000);
}

// stubbed win functions
void SDL_WM_SetCaption(const char* caption,void* p) {return;}
void SDL_WM_SetIcon(void* i, void* p) {return;}
void SDL_ShowCursor(int s) {return;}

int SDL_PollEvent(int * event)
{
	int ret = 0;
	SceCtrlData pad;
	sceCtrlPeekBufferPositive(0, &pad, 1);
	SceTouchData touch;
	sceTouchPeek(SCE_TOUCH_PORT_FRONT, &touch, 1);
	*event = pad.buttons;
	if (event) ret= SDL_KEYDOWN;
	if (touch.reportNum > 0) ret|= SDL_FINGERDOWN;
	//if(!aptMainLoop()) ret|= SDL_QUIT;
	return ret;
}

int getTouchX(){
	SceTouchData touch;
	sceTouchPeek(SCE_TOUCH_PORT_FRONT, &touch, 1);
	return touch.report[0].x;
}

int getTouchY(){
	SceTouchData touch;
	sceTouchPeek(SCE_TOUCH_PORT_FRONT, &touch, 1);
	return touch.report[0].y;
}

int SDL_GetKeyState(void* p)
{
	SceCtrlData pad;
	sceCtrlPeekBufferPositive(0, &pad, 1);
	return pad.buttons;
}

int WaitKey()
{
	SceCtrlData pad;
	pad.buttons = 0;
	do{sceCtrlPeekBufferPositive(0, &pad, 1);}while(!pad.buttons);
	
	return pad.buttons; 	
}

int getLanguage(void)
{
	return language;
}

void setLanguage(int languageID)
{
	if (languageID>5) language = 1;
	else if (languageID<1) language = 5;
	else language = languageID;
 
}
