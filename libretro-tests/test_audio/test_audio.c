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

// Audio
SDL_AudioSpec wavSpec;
Uint32 wavLength;
Uint8 *wavBuffer;
SDL_AudioDeviceID deviceId;

int quit=0;

int quit_app(void)
{
    // Audio
    SDL_PauseAudioDevice(deviceId, 1);
    SDL_FreeWAV(wavBuffer);
    SDL_CloseAudioDevice(deviceId);

	SDL_DestroyRenderer( renderer );
	renderer = NULL;

	SDL_DestroyWindow( window );
	window = NULL;

	SDL_Quit();

    return 0;
}

void init_app(){

    printf("Press the Z key\n");
    if ( SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0 )
    {
        fprintf(stderr, "Could not initialize SDL: %s\n", SDL_GetError());
        exit(1);
    }

    window = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );

    if( window == NULL ) { printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );exit(1); }

    if (SDL_ShowCursor(SDL_DISABLE) < 0) {
        printf( "Could not hide the mouse cursor. SDL_Error: %s\n", SDL_GetError() );
    }

    screenSurface = SDL_GetWindowSurface( window );

    videoBuffer=(unsigned int *)screenSurface->pixels;

    renderer = SDL_CreateSoftwareRenderer(screenSurface);
    if (!renderer) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Render creation for surface fail : %s\n",SDL_GetError());
        exit(1);
    }

    /* Clear the rendering surface with the specified color */
    SDL_SetRenderDrawColor(renderer, 76, 175, 80, 0xFF);

    /* Open the Audio Device. */
    deviceId = SDL_OpenAudioDevice(NULL, 0, &wavSpec, NULL, 0);

    /* Audio */
    if (SDL_LoadWAV("sample.wav", &wavSpec, &wavBuffer, &wavLength) == NULL) {
        fprintf(stderr, "Could not open sample.wav: %s\n", SDL_GetError());
    }
                    SDL_QueueAudio(deviceId, wavBuffer, wavLength);
                    SDL_PauseAudioDevice(deviceId, 0);
}



void exec_app(){

	//Clear screen
	SDL_RenderClear( renderer );

	SDL_RenderPresent(renderer);

	SDL_Event event;

	while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                quit = 1;
            break;
            case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
                case SDLK_ESCAPE:
                    quit = 1;
                break;
                case SDLK_z:
                    printf("Playing Audio\n");
                    SDL_QueueAudio(deviceId, wavBuffer, wavLength);
                    SDL_PauseAudioDevice(deviceId, 0);
                break;
            }
            break;
        }
    }
}

/********************************************************************************/
/*                   SDL PART	:END						*/
/********************************************************************************/

#ifndef __LIBRETRO__
int main( int argc, char* args[] ) {

    init_app();

    while(!quit){
    	exec_app();
    	//?needed on native with software render
    	SDL_UpdateWindowSurface( window );
    }

    return quit_app();
}
#endif
