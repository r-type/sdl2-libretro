#include <stdint.h>

/* Create the videoBuffer if we're not in libretro.*/
#ifndef __LIBRETRO__
uint32_t *videoBuffer;
int retrow=800;
int retroh=600;
#endif

/********************************************************************************/
/*                   SDL PART	:BEGIN						*/
/********************************************************************************/
extern uint32_t *videoBuffer;
#include <stdio.h>
#include "SDL.h"
#include "softmouse.h"

extern char retro_system_conf[512];
extern int retrow;
extern int retroh;

#define SCREEN_WIDTH retrow
#define SCREEN_HEIGHT retroh

SDL_Window* window = NULL;
SDL_Surface* screenSurface = NULL;

SDL_Renderer *renderer=NULL;

#include "testgfx.c"

int i, done=0, drawn, test = 0;
SDL_Event event;
Uint32 then, now, frames;
int numTests;

void quit_app(void)
{

    now = SDL_GetTicks();
    if (now > then) {
        double fps = ((double) frames * 1000) / (now - then);
        printf("%2.2f frames per second\n", fps);
    }

	DBGfreemousecursor();

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
    SDL_SetRenderDrawColor(renderer, 0xA0, 0xA0, 0xA0, 0xFF);
    DBGcreatmousecursor();

    frames = 0;
    then = SDL_GetTicks();
    done = 0;
    drawn = 0;

}


void exec_app(){

       /* Check for events */
        ++frames;
        while (SDL_PollEvent(&event)) {
            //SDLTest_CommonEvent(state, &event, &done);
			switch (event.type) {
				case SDL_QUIT: {        	
        	    			done = 1;
					break;
        			}
				case SDL_KEYDOWN: {
					switch (event.key.keysym.sym) {
						case SDLK_SPACE: {
							/* Switch to next test */
							test++;
							drawn = 0;
							break;
						}
					}
					break;
				}
				case SDL_MOUSEBUTTONDOWN: {
					switch (event.button.button) {
						case SDL_BUTTON_LEFT: {
							/* Switch to next test */
							test++;
							drawn = 0;
							break;
						}
						case SDL_BUTTON_RIGHT: {
							/* Switch to prev test */
							test--;
							drawn = 0;
							break;
						}
					}
					break;
				}
			}
        }

		if (!drawn) {
			/* Set test range */
			numTests = 24;
			if (test < 0) { 
				test = (numTests - 1); 
			} else {
				test = test % (numTests + 1);
			}

			/* Create random points */
			InitRandomPoints(test);

			/* Draw */
               
				switch (test) {
					case 0: {
						ExecuteTest(renderer, TestPixel, test, "Pixel");
						break;
					}
					case 1: {
						ExecuteTest(renderer, TestHline, test, "Hline");
						break;
					}
					case 2: {
						ExecuteTest(renderer, TestVline, test, "Vline");
						break;
					}
					case 3: {
						ExecuteTest(renderer, TestRectangle, test, "Rectangle");
						break;
					}
					case 4: {
						ExecuteTest(renderer, TestRoundedRectangle, test, "RoundedRectangle");
						break;
					}
					case 5: {
						ExecuteTest(renderer, TestBox, test, "Box");
						break;
					}
					case 6: {
						ExecuteTest(renderer, TestLine, test, "Line");
						break;
					}
					case 7: {
						ExecuteTest(renderer, TestCircle, test, "Circle");
						break;
					}
					case 8: {
						ExecuteTest(renderer, TestAACircle, test, "AACircle");
						break;
					}
					case 9: {
						ExecuteTest(renderer, TestFilledCircle, test, "FilledCircle");
						break;
					}
					case 10: {
						ExecuteTest(renderer, TestEllipse, test, "Ellipse");
						break;
					}
					case 11: {
						ExecuteTest(renderer, TestAAEllipse, test, "AAEllipse");
						break;
					}
					case 12: {
						ExecuteTest(renderer, TestFilledEllipse, test, "FilledEllipse");
						break;
					}
					case 13: {
						ExecuteTest(renderer, TestBezier, test, "Bezier");
						break;
					}
					case 14: {
						ExecuteTest(renderer, TestPolygon, test, "Polygon");
						break;
					}
					case 15: {
						ExecuteTest(renderer, TestAAPolygon, test, "AAPolygon");
						break;
					}
					case 16: {
						ExecuteTest(renderer, TestFilledPolygon, test, "FilledPolygon");
						break;
					}
					case 17: {
						ExecuteTest(renderer, TestTrigon, test, "Trigon");
						break;
					}
					case 18: {
						ExecuteTest(renderer, TestArc, test, "Arc");
						break;
					}
					case 19: {
						ExecuteTest(renderer, TestPie, test, "Pie");
						break;
					}
					case 20: {
						ExecuteTest(renderer, TestFilledPie, test, "FilledPie");
						break;
					}
					case 21: {
						ExecuteTest(renderer, TestThickLine, test, "ThickLine");
						break;
					}
					case 22: {
						ExecuteTest(renderer, TestTexturedPolygon, test, "TexturedPolygon");
						break;
					}
					case 23: {
						ExecuteTest(renderer, TestRoundedBox, test, "RoundedBox");
						break;
					}					
					case 24: {
						ExecuteTest(renderer, TestThickLineAccuracy, test, "ThickLine (Accuracy)");
						break;
					}					
					default: {
						ClearScreen(renderer, "Unknown Test");
						break;
					}
				}

				SDL_RenderPresent(renderer);
			
			drawn = 1;
		}

		/* Adjust framerate */
		//SDL_Delay(25);

        //DBGshowmouse(1);

}

/********************************************************************************/
/*                   SDL PART	:END						*/
/********************************************************************************/

#ifndef __LIBRETRO__
int main( int argc, char* args[] ) {

    init_app();

    while(!done){

	exec_app();
	//?needed on native with software render
	SDL_UpdateWindowSurface( window );
    }

    quit_app();
}
#endif
