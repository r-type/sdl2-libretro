#include <stdint.h>

#define LIBCO_C 
#include "libco/libco.h"
extern cothread_t mainThread;
extern cothread_t emuThread;

/********************************************************************************/
/*                   SDL PART	:BEGIN						*/
/********************************************************************************/
extern uint32_t *videoBuffer;

#include <stdio.h>
#include "SDL.h"
#include "softmouse.h"

#include "common.h"

#include "apu.h"
#include "cpu.h"
#include "input.h"
#include "mapper.h"
#include "rom.h"
#include "sdl_backend.h"

extern char RPATH[512];
extern char retro_system_conf[512];
extern int retrow;
extern int retroh;
#define rwidth retrow
#define rheight retroh
#define SCREEN_WIDTH retrow
#define SCREEN_HEIGHT retroh

//SDL_Window* window = NULL;
//SDL_Surface* screenSurface = NULL;
extern SDL_Renderer *renderer;//=NULL;

bool show_test_window = true;
bool show_another_window = false;
int done=0;

static int emulation_thread(void*) {
    run();
    return 0;
}

int nmain(char argv[]) {

    install_fatal_signal_handlers();

    // One-time initialization of various components
    init_apu();
    init_input();
    init_mappers();

    load_rom(RPATH, true);

    // Create a separate emulation thread and use this thread as the rendering
    // thread

    init_sdl();

    SDL_Thread *emu_thread;
    fail_if(!(emu_thread = SDL_CreateThread(emulation_thread, "emulation", 0)),
            "failed to create emulation thread: %s", SDL_GetError());
    sdl_thread();
    SDL_WaitThread(emu_thread, 0);

    deinit_sdl();

    unload_rom();

    puts("Shut down cleanly");
    co_switch(mainThread);
}

void quit_app(void)
{   
//	DBGfreemousecursor();

}

void init_app(){

//    DBGcreatmousecursor();

}


void exec_app(){

       // DBGshowmouse(1);

}

/********************************************************************************/
/*                   SDL PART	:END						*/
/********************************************************************************/

