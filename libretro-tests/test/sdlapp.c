#include <stdint.h>

/********************************************************************************/
/*                   SDL PART	:BEGIN						*/
/********************************************************************************/
extern uint32_t *videoBuffer;
#include <stdio.h>
#include "SDL.h"
#include "SDL_image.h"

extern char retro_system_conf[512];

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

SDL_Window* window = NULL;
SDL_Surface* screenSurface = NULL;

SDL_Renderer *renderer=NULL;

SDL_Texture* gTexture = NULL;

SDL_Texture* loadTexture( const char *path )
{
	//The final texture
	SDL_Texture* newTexture = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load( path );
	if( loadedSurface == NULL )
	{
		printf( "Unable to load image %s! SDL_image Error: %s\n", path, IMG_GetError() );
	}
	else
	{
		//Create texture from surface pixels
        newTexture = SDL_CreateTextureFromSurface( renderer, loadedSurface );
		if( newTexture == NULL )
		{
			printf( "Unable to create texture from %s! SDL Error: %s\n", path, SDL_GetError() );
		}

		//Get rid of old loaded surface
		SDL_FreeSurface( loadedSurface );
	}

	return newTexture;
}

void quit_app(void)
{	
	SDL_DestroyTexture( gTexture );
	gTexture = NULL;

	SDL_DestroyRenderer( renderer );
	renderer = NULL;

	SDL_DestroyWindow( window );
	window = NULL;

	IMG_Quit();
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
    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);

	//Initialize PNG loading
	int imgFlags = IMG_INIT_PNG;
	if( !( IMG_Init( imgFlags ) & imgFlags ) )
	{
		printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
        	exit(1);
	}

	gTexture = loadTexture( retro_system_conf );
}



void exec_app(){


				//Clear screen
				SDL_SetRenderDrawColor( renderer, 0xFF, 0xFF, 0xFF, 0xFF );
				SDL_RenderClear( renderer );


				//Top left corner viewport
				SDL_Rect topLeftViewport;
				topLeftViewport.x = 0;
				topLeftViewport.y = 0;
				topLeftViewport.w = SCREEN_WIDTH / 2;
				topLeftViewport.h = SCREEN_HEIGHT / 2;
				SDL_RenderSetViewport( renderer, &topLeftViewport );
				
				//Render texture to screen
				SDL_RenderCopy( renderer, gTexture, NULL, NULL );


				//Top right viewport
				SDL_Rect topRightViewport;
				topRightViewport.x = SCREEN_WIDTH / 2;
				topRightViewport.y = 0;
				topRightViewport.w = SCREEN_WIDTH / 2;
				topRightViewport.h = SCREEN_HEIGHT / 2;
				SDL_RenderSetViewport( renderer, &topRightViewport );
				
				//Render texture to screen
				SDL_RenderCopy( renderer, gTexture, NULL, NULL );


				//Bottom viewport
				SDL_Rect bottomViewport;
				bottomViewport.x = 0;
				bottomViewport.y = SCREEN_HEIGHT / 2;
				bottomViewport.w = SCREEN_WIDTH;
				bottomViewport.h = SCREEN_HEIGHT / 2;
				SDL_RenderSetViewport( renderer, &bottomViewport );

				
				//Render texture to screen
				SDL_RenderCopy( renderer, gTexture, NULL, NULL );


		//Update the surface
		SDL_UpdateWindowSurface( window );

}

/********************************************************************************/
/*                   SDL PART	:END						*/
/********************************************************************************/


