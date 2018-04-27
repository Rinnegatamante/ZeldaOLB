
#include "os_vita.h"
#include "gfx_vita.h"
#include "sound_vita.h"

bool drawing,drawing2;
int offset;
float scalepos;
float scalewidth;

vita2d_texture* fbo;

void vita2d_texture_set_pixel(vita2d_texture* texture, int x, int y, unsigned int color){
	uint32_t* data = vita2d_texture_get_datap(texture);
	uint32_t pitch = vita2d_texture_get_stride(texture)>>2;
	data[x+y*pitch] = color;
}

uint32_t vita2d_texture_get_pixel(const vita2d_texture* texture, int x, int y ){
	uint32_t* data = vita2d_texture_get_datap(texture);
	uint32_t pitch = vita2d_texture_get_stride(texture)>>2;
	return data[x+y*pitch];
}

void endTopFrame(void)
{
	if(drawing)
	{ 
		if (offset)
		{
			vita2d_draw_rectangle(0, 0, 40, 240, RGBA8(0, 0, 0, 0xff)); 
			vita2d_draw_rectangle(360, 0, 40, 240, RGBA8(0, 0, 0, 0xff));
		}
		vita2d_end_drawing();
		vita2d_start_drawing_advanced(NULL, SCE_GXM_SCENE_VERTEX_WAIT_FOR_DEPENDENCY);
		vita2d_clear_screen();
		vita2d_draw_texture_scale(fbo,27,0,2.2666,2.2666);
		vita2d_draw_rectangle(0, 0, 27, 544, RGBA8(0, 0, 0, 0xff));
		vita2d_draw_rectangle(933, 0, 27, 544, RGBA8(0, 0, 0, 0xff));
		vita2d_end_drawing();
	}
	drawing = false;
}

void SDL_Flip(void * p)
{
	endTopFrame();
	vita2d_wait_rendering_done();
	vita2d_swap_buffers();
}

SDL_Surface* SDL_SetVideoMode(int width, int height, int bpp, int flag)
{
	drawing=false;
	drawing2=false;
	if(flag & SDL_FULLSCREEN)
	{
		offset = 0;
		scalepos = 1.25; 
		scalewidth = 1.28; //should be 1.25 but setting a slight bigger value avoid tiles positioning defects caused by scaled width approximation
	} else {
		offset = 1;
		scalepos = 1; 
		scalewidth = 1; 
	}
	/*if (width == 320) return (SDL_Surface*) 2;
	else*/ return (SDL_Surface*) 1;
}

SDL_Surface* IMG_Load(const char* f)
{
    SDL_Surface* s;
	s = (SDL_Surface*) malloc(sizeof(SDL_Surface));
	if(!s) return NULL;
	s->texture = vita2d_load_PNG_file(f);

	if (s->texture) {
		s->w = vita2d_texture_get_width(s->texture);
		s->h = vita2d_texture_get_height(s->texture);
		
		vita2d_texture_set_filters(s->texture, SCE_GXM_TEXTURE_FILTER_LINEAR, SCE_GXM_TEXTURE_FILTER_LINEAR); 
		s->flags = 0;
		s->format = 0;
		return s;
	} else {
		free(s);
		return NULL;
	} 
}

SDL_Surface* SDL_CreateRGBSurface(int type, int width, int height, int bpp, int a, int b, int c, int d)
{
    SDL_Surface* s;
	s = (SDL_Surface*) malloc(sizeof(SDL_Surface));
	if(!s) return NULL;
	s->texture = vita2d_create_empty_texture_format(width, height, SCE_GXM_TEXTURE_FORMAT_U8U8U8U8_ABGR);
	if (s->texture) {
		s->w = vita2d_texture_get_width(s->texture);
		s->h = vita2d_texture_get_height(s->texture);
		s->flags = 0;
		vita2d_texture_set_filters(s->texture, SCE_GXM_TEXTURE_FILTER_LINEAR, SCE_GXM_TEXTURE_FILTER_LINEAR);
		s->format = 0;
		return s;
	} else {
		free(s);
		return NULL;
	} 
}


void SDL_FreeSurface(SDL_Surface* s)
{
    if(s)
	{
		if((s==(SDL_Surface*) 1) || (s==(SDL_Surface*) 2)) s=NULL;
		else
		{
			if(s->texture) vita2d_free_texture(s->texture);
			free(s);
		}
	}
}


void SDL_SetColorKey(SDL_Surface* s, int flag, uint32_t color)
{
 //   s->colorKey= color;
}

uint32_t SDL_MapRGB(int flag , uint8_t r, uint8_t g, uint8_t b)
{
    return RGBA8(r, g, b, 0xff);
}

void SDL_FillRect(SDL_Surface* s, SDL_Rect* rect, uint32_t color)
{
    if (s==(SDL_Surface*) 1) 
	{

		if(!drawing)
		{
            vita2d_pool_reset();
			vita2d_start_drawing_advanced(fbo, SCE_GXM_SCENE_FRAGMENT_SET_DEPENDENCY);
			drawing=true;
		} 
		if(rect) vita2d_draw_rectangle(rect->x+40*offset, rect->y, rect->w, rect->h, color);
		else  vita2d_draw_rectangle(40*offset, 0, 320 *scalepos, 240, color);
	} else if (s!=(SDL_Surface*) 2) {
		if(s) 
			if(s->texture){ 
				int i,j;
				int dx = (rect)?rect->x:0;
				int dy = (rect)?rect->y:0;
				int dw = (rect)?rect->w:vita2d_texture_get_width(s->texture);
				int dh = (rect)?rect->h:vita2d_texture_get_height(s->texture);
				if (dw > vita2d_texture_get_width(s->texture)) dw = vita2d_texture_get_width(s->texture);
				if (dh > vita2d_texture_get_height(s->texture)) dh = vita2d_texture_get_height(s->texture);
				for (i=dx;i< dw; i++)
					for (j=dy;j< dh; j++) 
						vita2d_texture_set_pixel(s->texture, i, j, color);
			}
	}
}

void filledEllipseRGBA(SDL_Surface* s, int x, int y, int rx, int ry, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    if (s==(SDL_Surface*) 1) 
	{
		if(!drawing)
		{
			vita2d_pool_reset();
			vita2d_start_drawing_advanced(fbo, SCE_GXM_SCENE_FRAGMENT_SET_DEPENDENCY);
			drawing=true;
		} 
		vita2d_draw_fill_circle(x*scalepos+40*offset, y, rx*scalewidth, RGBA8(r, g, b, a));
	}
}

void SDL_BlitSurface(SDL_Surface* s, SDL_Rect * src, SDL_Surface* d, SDL_Rect * dst)
{
	uint32_t pixel;
    if(d==(SDL_Surface*) 1)
	{
		if(!drawing)
		{
			vita2d_pool_reset();
			vita2d_start_drawing_advanced(fbo, SCE_GXM_SCENE_FRAGMENT_SET_DEPENDENCY);
			drawing=true;
		} 

		if(src) {
			if(dst) 
				vita2d_draw_texture_part_scale(s->texture, (int)(dst->x*scalepos+40*offset),dst->y, src->x, src->y, src->w, src->h, scalewidth,1);
			else
				vita2d_draw_texture_part_scale(s->texture, 40*offset,0, src->x, src->y, src->w, src->h, scalewidth,1);
		} else {
			if(dst) 
				vita2d_draw_texture_scale(s->texture, (int)(dst->x*scalepos+40*offset),dst->y, scalewidth,1);
			else
				vita2d_draw_texture_scale(s->texture, 40*offset,0, scalewidth,1);
		}
	} else if (d!=(SDL_Surface*)2) {
		if(s && d) 
			if(s->texture && d->texture){ 
				int i,j;

				int sx = (src)?src->x:0;
				int sy = (src)?src->y:0;
				int dx = (dst)?dst->x:0;
				int dy = (dst)?dst->y:0;
				int sw = (src)?src->w:vita2d_texture_get_width(s->texture);
				int sh = (src)?src->h:vita2d_texture_get_height(s->texture);
				if (sx+sw > vita2d_texture_get_width(s->texture)) sw = vita2d_texture_get_width(s->texture) - sx;
				if (sy+sh > vita2d_texture_get_height(s->texture)) sh = vita2d_texture_get_height(s->texture) - sy;
				for (i=0;i< sw; i++)
					for (j=0;j< sh; j++) {
						pixel = vita2d_texture_get_pixel ( s->texture,i+sx,j+sy);
						if ((pixel >> 24) & 0xff) vita2d_texture_set_pixel(d->texture,i+dx,j+dy,pixel); 		
					}
			}
	}
}