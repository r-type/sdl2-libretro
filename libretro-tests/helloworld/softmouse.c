#include "SDL.h"

extern SDL_Surface* screenSurface ;

#define DEFAULT_CWIDTH  16
#define DEFAULT_CHEIGHT 16
#define DEFAULT_CHOTX   0
#define DEFAULT_CHOTY   0
#define USE_MACOS_CURSOR

#ifdef USE_MACOS_CURSOR

static SDL_Surface *mousecursor;
static const unsigned char default_cdata[] = {
    0x00, 0x00,
    0x40, 0x00,
    0x60, 0x00,
    0x70, 0x00,
    0x78, 0x00,
    0x7C, 0x00,
    0x7E, 0x00,
    0x7F, 0x00,
    0x7F, 0x80,
    0x7C, 0x00,
    0x6C, 0x00,
    0x46, 0x00,
    0x06, 0x00,
    0x03, 0x00,
    0x03, 0x00,
    0x00, 0x00
};

static const unsigned char default_cmask[] = {
    0xC0, 0x00,
    0xE0, 0x00,
    0xF0, 0x00,
    0xF8, 0x00,
    0xFC, 0x00,
    0xFE, 0x00,
    0xFF, 0x00,
    0xFF, 0x80,
    0xFF, 0xC0,
    0xFF, 0xE0,
    0xFE, 0x00,
    0xEF, 0x00,
    0xCF, 0x00,
    0x87, 0x80,
    0x07, 0x80,
    0x03, 0x00
};

#else

static const unsigned char default_cdata[] = {
    0x00, 0x00,
    0x40, 0x00,
    0x60, 0x00,
    0x70, 0x00,
    0x78, 0x00,
    0x7C, 0x00,
    0x7E, 0x00,
    0x7F, 0x00,
    0x7F, 0x80,
    0x7C, 0x00,
    0x6C, 0x00,
    0x46, 0x00,
    0x06, 0x00,
    0x03, 0x00,
    0x03, 0x00,
    0x00, 0x00
};

static const unsigned char default_cmask[] = {
    0x40, 0x00,
    0xE0, 0x00,
    0xF0, 0x00,
    0xF8, 0x00,
    0xFC, 0x00,
    0xFE, 0x00,
    0xFF, 0x00,
    0xFF, 0x80,
    0xFF, 0xC0,
    0xFF, 0x80,
    0xFE, 0x00,
    0xEF, 0x00,
    0x4F, 0x00,
    0x07, 0x80,
    0x07, 0x80,
    0x03, 0x00
};
#endif

void creatmousecursor(const Uint8 * data, const Uint8 * mask,
                 int w, int h, int hot_x, int hot_y){

  int x, y;
    Uint32 *pixel;
    Uint8 datab = 0, maskb = 0;
    const Uint32 black = 0xFF000000;
    const Uint32 white = 0xFFFFFFFF;
    const Uint32 transparent = 0x00000000;
    w = ((w + 7) & ~7);

    mousecursor = SDL_CreateRGBSurface(0, w, h, 32,
                                   0x00FF0000,
                                   0x0000FF00,
                                   0x000000FF,
                                   0xFF000000);
    if (!mousecursor) {
        return ;
    }

    for (y = 0; y < h; ++y) {
        pixel = (Uint32 *) ((Uint8 *) mousecursor->pixels + y * mousecursor->pitch);
        for (x = 0; x < w; ++x) {
            if ((x % 8) == 0) {
                datab = *data++;
                maskb = *mask++;
            }
            if (maskb & 0x80) {
                *pixel++ = (datab & 0x80) ? black : white;
            } else {
                *pixel++ = (datab & 0x80) ? black : transparent;
            }
            datab <<= 1;
            maskb <<= 1;
        }
    }

}

void DBGcreatmousecursor(){
 creatmousecursor(default_cdata, default_cmask, DEFAULT_CWIDTH, DEFAULT_CHEIGHT, DEFAULT_CHOTX, DEFAULT_CHOTY);
}

void DBGshowmouse(int active){
	
	int x, y;

	if(!active)return;

	SDL_GetMouseState( &x, &y );
	//printf("(%d,%d)\n",x,y);
	SDL_Rect rect = { x, y, mousecursor->w,mousecursor->h };
	SDL_BlitSurface(mousecursor,NULL, screenSurface, &rect);
}

void DBGfreemousecursor(){


	if (mousecursor)
	{
		SDL_FreeSurface(mousecursor);
		mousecursor = NULL;
	}

}
