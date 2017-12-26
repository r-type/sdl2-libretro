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
#include "softmouse.h"

#include <GL/gl.h>
#include "zbuffer.h"

ZBuffer *frameBuffer;

extern char retro_system_conf[512];

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
extern int retrow,retroh;

SDL_Window* window = NULL;
SDL_Surface* screenSurface = NULL;

SDL_Renderer *renderer=NULL;

int quit=0;

#define TRUE  1
#define FALSE 0

#define NUM_TEXTURES 2
GLuint  base;                  /* Base Display List For The Font           */
GLuint  texture[NUM_TEXTURES]; /* Storage For Our Font Texture             */
GLuint  loop;                  /* Generic Loop Variable                    */

GLfloat cnt1;                  /* Counter Used To Move Text & For Coloring */
GLfloat cnt2;                  /* Counter Used To Move Text & For Coloring */

GLvoid KillFont( GLvoid )
{
   // glDeleteLists( base, 256 ); /* Delete All 256 Display Lists */

    return;
}

int LoadGLTextures( )
{
    /* Status indicator */
    int Status = FALSE;

    /* Create storage space for the texture */
    SDL_Surface *TextureImage[2];

    /* Load The Bitmap, Check For Errors, If Bitmap's Not Found Quit */
    if ( ( TextureImage[0] = SDL_LoadBMP( "data/font.bmp" ) ) && 
	 ( TextureImage[1] = SDL_LoadBMP( "data/bumps.bmp" ) ) )
        {

	    /* Set the status to true */
	    Status = TRUE;

	    /* Create The Texture */
	    glGenTextures( NUM_TEXTURES, &texture[0] );

	    /* Load in texture 1 */
	    /* Typical Texture Generation Using Data From The Bitmap */
	    glBindTexture( GL_TEXTURE_2D, texture[0] );

	    /* Generate The Texture */
	    glTexImage2D( GL_TEXTURE_2D, 0, 3, TextureImage[0]->w,
			  TextureImage[0]->h, 0, GL_BGR,
			  GL_UNSIGNED_BYTE, TextureImage[0]->pixels );
	    
	    /* Nearest Filtering */
	    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

	    /* Load in texture 2 */
	    /* Typical Texture Generation Using Data From The Bitmap */
	    glBindTexture( GL_TEXTURE_2D, texture[1] );

	    /* Linear Filtering */
	    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

	    /* Generate The Texture */
	    glTexImage2D( GL_TEXTURE_2D, 0, 3, TextureImage[1]->w,
			  TextureImage[1]->h, 0, GL_BGR,
			  GL_UNSIGNED_BYTE, TextureImage[1]->pixels );
        }

    /* Free up any memory we may have used */
    if ( TextureImage[0] )
	SDL_FreeSurface( TextureImage[0] );
    if ( TextureImage[1] )
	SDL_FreeSurface( TextureImage[1] );

    return Status;
}

GLvoid BuildFont( GLvoid )
{
    GLuint loop; /* Loop variable               */
    float cx;    /* Holds Our X Character Coord */
    float cy;    /* Holds Our Y Character Coord */

    /* Creating 256 Display List */
    base  = glGenLists( 256 );
    /* Select Our Font Texture */
    glBindTexture( GL_TEXTURE_2D, texture[0] );

    /* Loop Through All 256 Lists */
    for ( loop = 0; loop < 256; loop++ )
        {
	    /* NOTE:
	     *  BMPs are stored with the top-leftmost pixel being the
	     * last byte and the bottom-rightmost pixel being the first
	     * byte. So an image that is displayed as
	     *    1 0
	     *    0 0
	     * is represented data-wise like
	     *    0 0
	     *    0 1
	     * And because SDL_LoadBMP loads the raw data without
	     * translating to how it is thought of when viewed we need
	     * to start at the bottom-right corner of the data and work
	     * backwards to get everything properly. So the below code has
	     * been modified to reflect this. Examine how this is done and
	     * how the original tutorial is done to grasp the differences.
	     *
	     * As a side note BMPs are also stored as BGR instead of RGB
	     * and that is why we load the texture using GL_BGR. It's
	     * bass-ackwards I know but whattaya gonna do?
	     */

	    /* X Position Of Current Character */
	    cx = 1 - ( float )( loop % 16 ) / 16.0f;
	    /* Y Position Of Current Character */
	    cy = 1 - ( float )( loop / 16 ) / 16.0f;

            /* Start Building A List */
	    glNewList( base + ( 255 - loop ), GL_COMPILE );
	      /* Use A Quad For Each Character */
	      glBegin( GL_QUADS );
	        /* Texture Coord (Bottom Left) */
	        glTexCoord2f( cx - 0.0625, cy );
		/* Vertex Coord (Bottom Left) */
		glVertex2f( 0.0, 0.0 );

		/* Texture Coord (Bottom Right) */
		glTexCoord2f( cx, cy );
		/* Vertex Coord (Bottom Right) */
		glVertex2f( 16.0, 0.0 );

		/* Texture Coord (Top Right) */
		glTexCoord2f( cx, cy - 0.0625f );
		/* Vertex Coord (Top Right) */
		glVertex2f( 16.0, 16.0 );

		/* Texture Coord (Top Left) */
		glTexCoord2f( cx - 0.0625f, cy - 0.0625f);
		/* Vertex Coord (Top Left) */
		glVertex2f( 0.0, 16.0 );
	      glEnd( );

	      /* Move To The Left Of The Character */
	      glTranslatef( 16.0, 0.0, 0.0 );
	    glEndList( );
        }
}

void gluPerspective( GLdouble fovY, GLdouble aspect, GLdouble zNear, GLdouble zFar ) {
		
	const GLdouble pi = 3.1415926535897932384626433832795;
	GLdouble fW, fH;
	fH = tan( fovY / 360 * pi ) * zNear;
	fW = fH * aspect;
	glFrustum( -fW, fW, -fH, fH, zNear, zFar );
}

int resizeWindow( int width, int height )
{
    /* Height / width ration */
    GLfloat ratio;

    /* Protect against a divide by zero */
    if ( height == 0 )
	height = 1;

    ratio = ( GLfloat )width / ( GLfloat )height;

    /* Setup our viewport. */
    glViewport( 0, 0, ( GLint )width, ( GLint )height );

    /* change to the projection matrix and set our viewing volume. */
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity( );

    /* Set our perspective */
    gluPerspective( 45.0f, ratio, 0.1f, 100.0f );

    /* Make sure we're chaning the model view and not the projection */
    glMatrixMode( GL_MODELVIEW );

    /* Reset The View */
    glLoadIdentity( );

    return( TRUE );
}

void InitGL(int Width, int Height)
//int initGL( GLvoid )
{

    /* Load in the textures */
    if ( !LoadGLTextures( ) )
	return FALSE;

    /* Build our font list */
    BuildFont( );

    /* Enable smooth shading */
    glShadeModel( GL_SMOOTH );

    /* Set the background black */
    glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );

    /* Depth buffer setup */
    glClearDepth( 1.0f );

//FIXME: add depth/blend to tinygl from residualvm
    /* The Type Of Depth Test To Do */
//    glDepthFunc( GL_LEQUAL );

    /* Select The Type Of Blending */
//    glBlendFunc( GL_SRC_ALPHA, GL_ONE );

    /* Enable 2D Texture Mapping */
    glEnable( GL_TEXTURE_2D );
    
   // return( TRUE );
}

GLvoid glPrint( GLint x, GLint y, char *string, int set )
{
    if ( set > 1 )
	set = 1;

    /* Select our texture */
    glBindTexture( GL_TEXTURE_2D, texture[0] );

    /* Disable depth testing */
    glDisable( GL_DEPTH_TEST );

    /* Select The Projection Matrix */
    glMatrixMode( GL_PROJECTION );
    /* Store The Projection Matrix */
    glPushMatrix( );

    /* Reset The Projection Matrix */
    glLoadIdentity( );
    /* Set Up An Ortho Screen */
    glOrtho( 0, 640, 0, 480, -1, 1 );

    /* Select The Modelview Matrix */
    glMatrixMode( GL_MODELVIEW );
    /* Stor the Modelview Matrix */
    glPushMatrix( );
    /* Reset The Modelview Matrix */
    glLoadIdentity( );

    /* Position The Text (0,0 - Bottom Left) */
    glTranslatef( ( GLfloat )x,( GLfloat ) y, 0.0 );

    /* Choose The Font Set (0 or 1) */
//    glListBase( base - 32 + ( 128 * set ) );

	int i;
	for(i=0;i<strlen( string );i++)
	glCallList(string[i]-32 + ( 128 * set ));

    /* Write The Text To The Screen */
//    glCallLists( strlen( string ), GL_BYTE, string );

    /* Select The Projection Matrix */
    glMatrixMode( GL_PROJECTION );
    /* Restore The Old Projection Matrix */
    glPopMatrix( );

    /* Select the Modelview Matrix */
    glMatrixMode( GL_MODELVIEW );
    /* Restore the Old Projection Matrix */
    glPopMatrix( );

    /* Re-enable Depth Testing */
    glEnable( GL_DEPTH_TEST );
}

/* Here goes our drawing code */
int drawGLScene( GLvoid )
{
    /* These are to calculate our fps */
    static GLint T0     = 0;
    static GLint Frames = 0;

    /* Clear The Screen And The Depth Buffer */
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glLoadIdentity( );

    /* Select Our Second Texture */
    glBindTexture( GL_TEXTURE_2D, texture[1] );
    /* Move Into The Screen 5 Units */
    glTranslatef( 0.0f, 0.0f, -5.0f );
    /* Rotate On The Z Axis 45 Degrees (Clockwise) */
    glRotatef( 45.0f, 0.0f, 0.0f, 1.0f );
    /* Rotate On The X & Y Axis By cnt1 (Left To Right) */
    glRotatef( cnt1 * 30.0f, 1.0f, 1.0f, 0.0f );

    /* Disable Blending Before We Draw In 3D */
    glDisable( GL_BLEND );
    glColor3f( 1.0f, 1.0f, 1.0f ); /* Bright White                       */
    glBegin( GL_QUADS );           /* Draw Our First Texture Mapped Quad */
      glTexCoord2f( 0.0f,  0.0f ); /* First Texture Coord                */
      glVertex2f(  -1.0f,  1.0f ); /* First Vertex                       */
      glTexCoord2f( 1.0f,  0.0f ); /* Second Texture Coord               */
      glVertex2f(   1.0f,  1.0f ); /* Second Vertex                      */
      glTexCoord2f( 1.0f,  1.0f ); /* Third Texture Coord                */
      glVertex2f(   1.0f, -1.0f ); /* Third Vertex                       */
      glTexCoord2f( 0.0f,  1.0f ); /* Fourth Texture Coord               */
      glVertex2f(  -1.0f, -1.0f ); /* Fourth Vertex                      */
    glEnd( );

    /* Rotate On The X & Y Axis By 90 Degrees (Left To Right) */
    glRotatef( 90.0f, 1.0f, 1.0f, 0.0f );
    glBegin( GL_QUADS );           /* Draw Our Second Texture Mapped Quad */
      glTexCoord2f( 0.0f,  0.0f ); /* First Texture Coord                 */
      glVertex2f(  -1.0f,  1.0f ); /* First Vertex                        */
      glTexCoord2f( 1.0f,  0.0f ); /* Second Texture Coord                */
      glVertex2f(   1.0f,  1.0f ); /* Second Vertex                       */
      glTexCoord2f( 1.0f,  1.0f ); /* Third Texture Coord                 */
      glVertex2f(   1.0f, -1.0f ); /* Third Vertex                        */
      glTexCoord2f( 0.0f,  1.0f ); /* Fourth Texture Coord                */
      glVertex2f(  -1.0f, -1.0f ); /* Fourth Vertex                       */
    glEnd( );

    /* Re-enable Blending */
    glEnable( GL_BLEND );
    /* Reset the view */
    glLoadIdentity( );


    /* Pulsing Colors Based On Text Position */
    /* Print GL Text To The Screen */
    glColor3f( 1.0f * ( float )cos( cnt1 ),
	       1.0f * ( float )sin( cnt2 ),
	       1.0f - 0.5f * ( float )cos( cnt1 + cnt2 ) );
    glPrint( ( int )( 280 + 250 * cos( cnt1 ) ),
	     ( int )( 235 + 200 * sin( cnt2 ) ),
	     "NeHe", 0 );
    glColor3f( 1.0f * ( float )sin( cnt2 ),
	       1.0f - 0.5f * ( float )cos( cnt1 + cnt2 ),
	       1.0f * ( float )cos( cnt1 ) );
    glPrint( ( int )( 280 + 230 * cos( cnt2 ) ),
	     ( int )( 235 + 200 * sin( cnt1 ) ),
	     "OpenGL", 1 );

    /* Set Color to Red */
    glColor3f( 0.0f, 0.0f, 1.0f );
    /* Draw Text To The Screen */
    glPrint( ( int )( 240 + 200 * cos( ( cnt2 + cnt1 ) / 5 ) ), 2,
	     "Giuseppe D'Agata", 0 );

    /* Set Color To White */
    glColor3f( 1.0f, 1.0f, 1.0f );
    /* Draw Offset Text To The Screen */
    glPrint( ( int )( 242 + 200 * cos( ( cnt2 + cnt1 ) / 5 ) ), 2,
	     "Giuseppe D'Agata", 0 );

    cnt1 += 0.01f;   /* Increase The First Counter  */
    cnt2 += 0.0081f; /* Increase The Second Counter */

    /* Draw it to the screen */

	glSwapLR();

    /* Gather our frames per second */
    Frames++;
    {
	GLint t = SDL_GetTicks();
	if (t - T0 >= 5000) {
	    GLfloat seconds = (t - T0) / 1000.0;
	    GLfloat fps = Frames / seconds;
	    printf("%d frames in %g seconds = %g FPS\n", Frames, seconds, fps);
	    T0 = t;
	    Frames = 0;
	}
    }

    return( TRUE );
}

void quit_app(void)
{

    	KillFont( );

	/* Clean up our textures */
	glDeleteTextures( NUM_TEXTURES, &texture[0] );

	ZB_close(frameBuffer);

	DBGfreemousecursor();

	SDL_DestroyRenderer( renderer );
	renderer = NULL;

	SDL_DestroyWindow( window );
	window = NULL;

	SDL_Quit();
}

void glInitLR() {

	frameBuffer = ZB_open( retrow, retroh,ZB_MODE_RGBA, 0, 0, 0, 0);
	glInit( frameBuffer );
}

void glSwapLR() {
	ZB_copyFrameBuffer(frameBuffer,videoBuffer, retrow*4);
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
    //SDL_SetRenderDrawColor(renderer, 76, 175, 80, 0xFF);

    DBGcreatmousecursor();

    glInitLR();
    InitGL(retrow,retroh);
    resizeWindow(retrow,retroh);
}

void exec_app(){

	//Clear screen
	SDL_RenderClear( renderer );

	SDL_RenderPresent(renderer);

	SDL_Event event;

	while (SDL_PollEvent(&event)) {
		if( event.type == SDL_QUIT )
        	{
        	    quit = 1;
        	}
    	}

	drawGLScene();

        DBGshowmouse(1);
}

/********************************************************************************/
/*                   SDL PART	:END						*/
/********************************************************************************/

