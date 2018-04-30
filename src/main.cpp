/*

    Zelda Oni Link Begins

    Copyright (C) 2006-2008  Vincent Jouillat

    Please send bugreports with examples or suggestions to www.zeldaroth.fr

*/

#include <SDL/SDL.h>
#include "vita/os_vita.h"
#include <iostream>
#include <cstdlib>
#include <vitaGL.h>
#include <imgui_vita.h>
#include <ctime>
#include "Keyboard.h"
#include "Generique.h"

int _newlib_heap_size_user = 192 * 1024 * 1024;

uint8_t language = 1;
uint64_t startTick;

bool bilinear = true;
bool vflux_window = false;
bool credits_window = false;
bool vflux_enabled = false;
float vcolors[3];
uint16_t *vindices;
float *colors;
float *vertices;
uint64_t tmr1;

SDL_Shader shader = SDL_SHADER_NONE;

int SDL_GetKeyStateVita(void* p)
{
	SceCtrlData pad;
	sceCtrlPeekBufferPositive(0, &pad, 1);
    
    // Left analog support
    if ((!(pad.buttons & SCE_CTRL_LEFT)) & (pad.lx < 80)) pad.buttons += SCE_CTRL_LEFT;
    if ((!(pad.buttons & SCE_CTRL_RIGHT)) & (pad.lx > 160)) pad.buttons += SCE_CTRL_RIGHT;
    if ((!(pad.buttons & SCE_CTRL_UP)) & (pad.ly < 80)) pad.buttons += SCE_CTRL_UP;
    if ((!(pad.buttons & SCE_CTRL_DOWN)) & (pad.ly > 160)) pad.buttons += SCE_CTRL_DOWN;
    
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

SDL_Surface* init() {             // initialise SDL
	vglUseVram(true);
    if(SDL_Init(SDL_INIT_VIDEO) == -1) {
        exit(-1);
    }
    atexit(SDL_Quit);
	 
	SDL_ShowCursor(SDL_DISABLE);

    scePowerSetArmClockFrequency(444);
	scePowerSetBusClockFrequency(222);
	scePowerSetGpuClockFrequency(222);
	scePowerSetGpuXbarClockFrequency(166);
    sceCtrlSetSamplingMode(SCE_CTRL_MODE_ANALOG_WIDE);
	sceTouchSetSamplingState(SCE_TOUCH_PORT_FRONT, SCE_TOUCH_SAMPLING_STATE_START);
    
	// Getting system language
	sceAppUtilInit(&(SceAppUtilInitParam){}, &(SceAppUtilBootParam){});
	int lang;
	sceAppUtilSystemParamGetInt(SCE_SYSTEM_PARAM_ID_LANG, &lang);
	switch (lang){
		case SCE_SYSTEM_PARAM_LANG_FRENCH:
			language = 2;
			break;
		case SCE_SYSTEM_PARAM_LANG_SPANISH:
			language = 5;
			break;
		case SCE_SYSTEM_PARAM_LANG_ITALIAN:
			language = 4;
			break;
		default:
			language = 1;
			break;
	}
	
	// Starting audio
	soundInit();
	
	// Setting start tick
	startTick = sceKernelGetProcessTimeWide();
	
	// In case game dir doesn't exist
    sceIoMkdir("ux0:data/ZeldaOLB", 0777);

    
	return SDL_SetVideoMode(320, 240, 16, SDL_DOUBLEBUF);
}

bool credits = false;

void ImGui_callback() {
	
	ImGui_ImplVitaGL_NewFrame();
	
	if (ImGui::BeginMainMenuBar()){
		
		if (ImGui::BeginMenu("Graphics")){
			if (ImGui::MenuItem("Bilinear Filter", nullptr, bilinear)){
				bilinear = !bilinear;
				SDL_SetVideoModeBilinear(bilinear);
			}
			if (ImGui::MenuItem("vFlux Config", nullptr, vflux_window)){
				vflux_window = !vflux_window;
			}
			if (ImGui::BeginMenu("Shaders")){
				if (ImGui::MenuItem("None", nullptr, shader == SDL_SHADER_NONE)){
					shader = SDL_SHADER_NONE;
					SDL_SetVideoShader(SDL_SHADER_NONE);
				}
				if (ImGui::MenuItem("Sharp Bilinear", nullptr, shader == SDL_SHADER_SHARP_BILINEAR)){
					shader = SDL_SHADER_SHARP_BILINEAR;
					SDL_SetVideoShader(SDL_SHADER_SHARP_BILINEAR);
				}
				if (ImGui::MenuItem("Sharp Bilinear (Scancode)", nullptr, shader == SDL_SHADER_SHARP_BILINEAR_SIMPLE)){
					shader = SDL_SHADER_SHARP_BILINEAR_SIMPLE;
					SDL_SetVideoShader(SDL_SHADER_SHARP_BILINEAR_SIMPLE);
				}
				if (ImGui::MenuItem("LCD 3x", nullptr, shader == SDL_SHADER_LCD3X)){
					shader = SDL_SHADER_LCD3X;
					SDL_SetVideoShader(SDL_SHADER_LCD3X);
				}
				if (ImGui::MenuItem("xBR x2", nullptr, shader == SDL_SHADER_XBR_2X_FAST)){
					shader = SDL_SHADER_XBR_2X_FAST;
					SDL_SetVideoShader(SDL_SHADER_XBR_2X_FAST);
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenu();
		}
		
		if (ImGui::BeginMenu("Info")){
			if (ImGui::MenuItem("Credits", nullptr, credits_window)){
				credits_window = !credits_window;
			}
			ImGui::EndMenu();
		}
		
		
		if (vflux_window){
			ImGui::Begin("vFlux Configuration", &vflux_window);
			ImGui::ColorPicker3("Filter Color", vcolors);
			ImGui::Checkbox("Enable vFlux", &vflux_enabled);
			ImGui::End();
		}
		
		if (credits_window){
			ImGui::Begin("Credits", &credits_window);
			ImGui::TextColored(ImVec4(255, 255, 0, 255), "Zelda: Oni Link Begins Vita v.1.5");
			ImGui::Text("Game Creator: Vincent Jouillat");
			ImGui::Text("Port Author: Rinnegatamante");
			ImGui::Text("Tester: ChrisHighwind");
			ImGui::Separator();
			ImGui::TextColored(ImVec4(255, 255, 0, 255), "Patreon Supporters:");
			ImGui::Text("XandridFire");
			ImGui::Text("Styde Pregny");
			ImGui::Text("Billy McLaughlin II");
			ImGui::Separator();
			ImGui::TextColored(ImVec4(255, 255, 0, 255), "Special thanks to:");
			ImGui::Text("rsn8887 for fixing shaders for vitaGL usage");
			ImGui::Text("ocornut for dear ImGui");
			ImGui::End();
		}
		
		ImGui::SameLine();
		ImGui::SetCursorPosX(870);
		
		ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate); 
		ImGui::EndMainMenuBar();
	}
	
	if (vflux_enabled) {
		memcpy(&colors[0], vcolors, sizeof(float) * 3);
		memcpy(&colors[4], vcolors, sizeof(float) * 3);
		memcpy(&colors[8], vcolors, sizeof(float) * 3);
		memcpy(&colors[12], vcolors, sizeof(float) * 3);
		
		float a;
		SceDateTime time;
		sceRtcGetCurrentClockLocalTime(&time);
		if (time.hour < 6)		// Night/Early Morning
			a = 0.25f;
		else if (time.hour < 10) // Morning/Early Day
			a = 0.1f;
		else if (time.hour < 15) // Mid day
			a = 0.05f;
		else if (time.hour < 19) // Late day
			a = 0.15f;
		else // Evening/Night
			a = 0.2f;
		colors[3] = colors[7] = colors[11] = colors[15] = a;
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glEnableClientState(GL_COLOR_ARRAY);
		glEnableClientState(GL_VERTEX_ARRAY);
		vglIndexPointerMapped(vindices);
		vglVertexPointerMapped(vertices);
		vglColorPointerMapped(GL_FLOAT, colors);
		vglDrawObjects(GL_TRIANGLE_FAN, 4, true);
		glDisableClientState(GL_COLOR_ARRAY);
		glDisableClientState(GL_VERTEX_ARRAY);
	}
	
	ImGui::Render();
	ImGui_ImplVitaGL_RenderDrawData(ImGui::GetDrawData());
	
	SceTouchData touch;
	sceTouchPeek(SCE_TOUCH_PORT_FRONT, &touch, 1);
	uint64_t delta_touch = sceKernelGetProcessTimeWide() - tmr1;
	if (touch.reportNum > 0){
		ImGui::GetIO().MouseDrawCursor = true;
		tmr1 = sceKernelGetProcessTimeWide();
	}else if (delta_touch > 3000000) ImGui::GetIO().MouseDrawCursor = false;
	
}

int main(int argc, char** argv) {
    if (argc && argv); //pour éviter un warning.....
    
    std::srand(std::time(NULL));
    
    SDL_Surface* gpScreen = NULL;
    
    int mode = 2; //mode=0;
    
    gpScreen = init();
	SDL_SetVideoModeScaling(0, 0, 960, 544);
	
	vindices = (uint16_t*)malloc(sizeof(uint16_t)*4);
	colors = (float*)malloc(sizeof(float)*4*4);
	vertices = (float*)malloc(sizeof(float)*3*4);
	vertices[0] =   0.0f;
	vertices[1] =   0.0f;
	vertices[2] =   0.0f;
	vertices[3] = 960.0f;
	vertices[4] =   0.0f;
	vertices[5] =   0.0f;
	vertices[6] = 960.0f;
	vertices[7] = 544.0f;
	vertices[8] =   0.0f;
	vertices[9] =   0.0f;
	vertices[10]= 544.0f;
	vertices[11]=   0.0f;
	vindices[0] = 0;
	vindices[1] = 1;
	vindices[2] = 2;
	vindices[3] = 3;
	
	ImGui::CreateContext();
	ImGui_ImplVitaGL_Init();
	ImGui_ImplVitaGL_TouchUsage(true);
	ImGui_ImplVitaGL_MouseStickUsage(false);
	ImGui_ImplVitaGL_UseIndirectFrontTouch(true);
	ImGui::StyleColorsDark();
	ImGui::GetIO().MouseDrawCursor = false;
	
	SDL_SetVideoCallback(ImGui_callback);
	
    Audio* gpAudio = new Audio();
    Jeu* gpJeu = new Jeu(gpAudio);
    Carte* gpCarte = new Carte(gpJeu);
    Encyclopedie* gpEncyclopedie = new Encyclopedie(gpJeu);
    Keyboard* gpKeyboard = new Keyboard(gpJeu, gpCarte, gpEncyclopedie, gpScreen, mode);
    gpJeu->setKeyboard(gpKeyboard);
    Generique* gpGenerique = new Generique(gpJeu);
    gpJeu->setGenerique(gpGenerique);
    gpGenerique->initLogo();
    
    //gpJeu->init(0);
    
    bool gLoop = true;
    
    Uint32 lastAnimTime = SDL_GetTicks();
    
    while (gLoop) {
        
		credits = false;
		
        if (gpKeyboard->gererClavier() == -1) {gLoop = false;}
        
        switch (gpKeyboard->getMode()) {
            case 0 : //jeu normal
				gpJeu->draw(gpScreen); break;
            case 1 : //disclamer
            case 2 : //logo
				credits = true;
            case 3 : //titre
            case 14 : //générique score
            case 17 : //menu d'aide 1
            case 18 : //menu d'aide 2
				gpGenerique->draw(gpScreen); break;
            case 4 : //selection
                gpGenerique->drawSelection(gpScreen, gpKeyboard->getLigne(), 
                    gpKeyboard->getColonne()); break;
            case 6 : //options
                gpGenerique->drawOption(gpScreen, gpKeyboard->getLigneOption(),
                gpKeyboard->getVolume()/4, gpKeyboard->getVolson()/4); break;
            case 7 : //charger partie
                gpGenerique->drawCharger(gpScreen, gpKeyboard->getLigne(), 
                    gpKeyboard->getLigneVal()); break;
            case 8 : //générique intro
                gpGenerique->drawIntro(gpScreen, gpKeyboard->getIntro()); break;
            case 9 : //effacer partie
                gpGenerique->drawEffacerSave(gpScreen, gpKeyboard->getLigne(), 
                    gpKeyboard->getLigneVal()); break;
            case 10 : //générique début chez link
                gpGenerique->drawDebut(gpScreen); break;
            case 11 : //générique fin
                gpGenerique->drawFin(gpScreen); break;
            case 12 : //carte
            case 22 : //carte téléportation
                gpCarte->draw(gpScreen); break;
            case 13 : //encyclopédie des monstres
                gpEncyclopedie->draw(gpScreen); break;
            case 15 : //records
            case 19 : //rang 100%
            case 20 : //rang ultime
            case 21 : //rang de rapidité
                gpGenerique->drawRecord(gpScreen, gpKeyboard->getLigneRecord(),
                    gpKeyboard->getColonneRecord()); break;
            case 16 : //effacer record
                gpGenerique->drawEffacer(gpScreen, gpKeyboard->getLigneVal()); break;
            default : break;
        }

        SDL_Flip(gpScreen);
        
        if (SDL_GetTicks() < lastAnimTime + 20) SDL_Delay(lastAnimTime+20-SDL_GetTicks());
        lastAnimTime = SDL_GetTicks();
    
    }

    delete gpEncyclopedie;
    delete gpGenerique;
    delete gpKeyboard;
    delete gpCarte;
    delete gpJeu;
    delete gpAudio;
    
    SDL_Quit();
    soundClose();
    exit(0);
    
    return 0;
}
