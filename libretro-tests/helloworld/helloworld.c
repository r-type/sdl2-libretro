#include <stdint.h>

/* Create the videoBuffer if we're not in libretro.*/
#ifndef __LIBRETRO__
uint32_t *videoBuffer;
#endif

/********************************************************************************/
/*                   SDL PART	:BEGIN						*/
/********************************************************************************/
extern uint32_t *videoBuffer;
#include <stdio.h>
#include "SDL.h"

extern char retro_system_conf[512];

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

SDL_Window* window = NULL;
SDL_Surface* screenSurface = NULL;

SDL_Renderer *renderer=NULL;

void quit_app(void)
{
	SDL_DestroyRenderer( renderer );
	renderer = NULL;

	SDL_DestroyWindow( window );
	window = NULL;

	SDL_Quit();
}

void init_app(){

    if ( SDL_Init(SDL_INIT_VIDEO) < 0 )
    {
        fprintf(stderr, "Could not initialize SDL: %s\n", SDL_GetError());
        exit(1);
    } 

    window = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );

    if( window == NULL ) { printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );exit(1); }

    screenSurface = SDL_GetWindowSurface( window );

    videoBuffer=(unsigned int *)screenSurface->pixels;

    renderer = SDL_CreateSoftwareRenderer(screenSurface);
    if (!renderer) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Render creation for surface fail : %s\n",SDL_GetError());
        exit(1);
    }

    /* Clear the rendering surface with the specified color */
    SDL_SetRenderDrawColor(renderer, 76, 175, 80, 0xFF);
}



void exec_app(){
	//Clear screen
	SDL_RenderClear( renderer );


	//Update the surface
	SDL_UpdateWindowSurface( window );

}

/********************************************************************************/
/*                   SDL PART	:END						*/
/********************************************************************************/

#ifndef __LIBRETRO__
int main( int argc, char* args[] ) {
    init_app();
    exec_app();

    //Wait two seconds
    SDL_Delay( 2000 );
    quit_app();
}
#endif
