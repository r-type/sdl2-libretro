libretro SDL2 software render test 

need a test.png in RA system folder.

the concept is to create a window
then a surface in this window
point videoBuffer to the surface pixels
create a software render with this surface.


    window = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );

    if( window == NULL ) { printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );exit(1); }

    screenSurface = SDL_GetWindowSurface( window );

    videoBuffer=(unsigned int *)screenSurface->pixels;

    renderer = SDL_CreateSoftwareRenderer(screenSurface);
