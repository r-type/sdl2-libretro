/*
  Simple DirectMedia Layer
  Copyright (C) 1997-2016 Sam Lantinga <slouken@libsdl.org>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/
#include "../../SDL_internal.h"

#if SDL_VIDEO_DRIVER_DUMMY
#include "SDL_nullvideo.h"
#include "../SDL_sysvideo.h"
#include "SDL_nullframebuffer_c.h"


#define DUMMY_SURFACE   "_SDL_DummySurface"

int SDL_DUMMY_CreateWindowFramebuffer(_THIS, SDL_Window * window, Uint32 * format, void ** pixels, int *pitch)
{
    SDL_Surface *surface;
    const Uint32 surface_format = SDL_PIXELFORMAT_RGB888;
    int w, h;
    int bpp;
    Uint32 Rmask, Gmask, Bmask, Amask;

    /* Free the old framebuffer surface */
    surface = (SDL_Surface *) SDL_GetWindowData(window, DUMMY_SURFACE);
    SDL_FreeSurface(surface);

    /* Create a new one */
    SDL_PixelFormatEnumToMasks(surface_format, &bpp, &Rmask, &Gmask, &Bmask, &Amask);
    SDL_GetWindowSize(window, &w, &h);
    surface = SDL_CreateRGBSurface(0, w, h, bpp, Rmask, Gmask, Bmask, Amask);
    if (!surface) {
        return -1;
    }

    printf("CreateWindowFramebuffer %dx%d%xd\n",w,h,bpp);

    SDL_VideoData *phdata = (SDL_VideoData *) _this->driverdata;

    phdata->w = w;
    phdata->h = h;
    phdata->_omx += w/2;
    phdata->_omy += h/2;
    phdata->_mouseConnected = SDL_TRUE;
    phdata->_mouseButtons = 0;

    /* Save the info and return! */
    SDL_SetWindowData(window, DUMMY_SURFACE, surface);
    *format = surface_format;
    *pixels = surface->pixels;
    *pitch = surface->pitch;

    SDL_SendMouseMotion(window, 0, 0, phdata->_omx,phdata->_omy);

    return 0;
}

int SDL_DUMMY_UpdateWindowFramebuffer(_THIS, SDL_Window * window, const SDL_Rect * rects, int numrects)
{
    static int frame_number;
    SDL_Surface *surface;

    surface = (SDL_Surface *) SDL_GetWindowData(window, DUMMY_SURFACE);
    if (!surface) {
        return SDL_SetError("Couldn't find dummy surface for window");
    }

    /* Send the data to the display */
    if (SDL_getenv("SDL_VIDEO_DUMMY_SAVE_FRAMES")) {
        char file[128];
        SDL_snprintf(file, sizeof(file), "SDL_window%d-%8.8d.bmp",
                     SDL_GetWindowID(window), ++frame_number);
        SDL_SaveBMP(surface, file);
    }
    return 0;
}

void SDL_DUMMY_DestroyWindowFramebuffer(_THIS, SDL_Window * window)
{
    SDL_Surface *surface;

    surface = (SDL_Surface *) SDL_SetWindowData(window, DUMMY_SURFACE, NULL);
    SDL_FreeSurface(surface);
}

#endif /* SDL_VIDEO_DRIVER_DUMMY */

/* vi: set ts=4 sw=4 expandtab: */