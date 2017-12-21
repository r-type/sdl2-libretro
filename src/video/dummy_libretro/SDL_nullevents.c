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

/* Being a null driver, there's no event stream. We just define stubs for
   most of the API. */

#include "../../events/SDL_events_c.h"

#include "SDL_nullvideo.h"
#include "SDL_nullevents_c.h"

#include "libretro.h"
extern short int libretro_input_state_cb(unsigned port,unsigned device,unsigned index,unsigned id);

#define RETROMOD(a,i) {\
	    SDL_SendKeyboardKey(a ? SDL_PRESSED : SDL_RELEASED, SDL_GetScancodeFromKey(i));\
}  

#include "../../events/SDL_mouse_c.h"
#include "SDL_assert.h"
#include "../../events/default_cursor.h"

typedef struct _dummy_CursorData
{
    const char *system_cursor;
} dummy_CursorData;

static SDL_Cursor*
dummy_CreateDefaultCursor()
{   
 return SDL_CreateCursor(default_cdata, default_cmask, DEFAULT_CWIDTH, DEFAULT_CHEIGHT, DEFAULT_CHOTX, DEFAULT_CHOTY);

    SDL_Cursor* cursor;
    dummy_CursorData *curdata;

    cursor = SDL_calloc(1, sizeof(SDL_Cursor));
    if (cursor) {
        curdata = (dummy_CursorData *) SDL_calloc(1, sizeof(*curdata));
        if (!curdata) {
            SDL_OutOfMemory();
            SDL_free(cursor);
            return NULL;
        }

        curdata->system_cursor = "default";
        cursor->driverdata = curdata;
    }
    else {
        SDL_OutOfMemory();
    }

    return cursor;
}


/*
static SDL_Cursor*
dummy_CreateCursor(SDL_Surface* sruface, int hot_x, int hot_y)
{
    return dummy_CreateDefaultCursor();
}
*/

static SDL_Cursor*
dummy_CreateSystemCursor(SDL_SystemCursor id)
{
    SDL_Cursor *cursor;
    dummy_CursorData *curdata;
    const char *cursor_name = NULL;

    switch(id) {
        case SDL_SYSTEM_CURSOR_ARROW:
            cursor_name = "default";
            break;
        case SDL_SYSTEM_CURSOR_IBEAM:
            cursor_name = "text";
            break;
        case SDL_SYSTEM_CURSOR_WAIT:
            cursor_name = "wait";
            break;
        case SDL_SYSTEM_CURSOR_CROSSHAIR:
            cursor_name = "crosshair";
            break;
        case SDL_SYSTEM_CURSOR_WAITARROW:
            cursor_name = "progress";
            break;
        case SDL_SYSTEM_CURSOR_SIZENWSE:
            cursor_name = "nwse-resize";
            break;
        case SDL_SYSTEM_CURSOR_SIZENESW:
            cursor_name = "nesw-resize";
            break;
        case SDL_SYSTEM_CURSOR_SIZEWE:
            cursor_name = "ew-resize";
            break;
        case SDL_SYSTEM_CURSOR_SIZENS:
            cursor_name = "ns-resize";
            break;
        case SDL_SYSTEM_CURSOR_SIZEALL:
            break;
        case SDL_SYSTEM_CURSOR_NO:
            cursor_name = "not-allowed";
            break;
        case SDL_SYSTEM_CURSOR_HAND:
            cursor_name = "pointer";
            break;
        default:
            SDL_assert(0);
            return NULL;
    }

    cursor = (SDL_Cursor *) SDL_calloc(1, sizeof(*cursor));
    if (!cursor) {
        SDL_OutOfMemory();
        return NULL;
    }
    curdata = (dummy_CursorData *) SDL_calloc(1, sizeof(*curdata));
    if (!curdata) {
        SDL_OutOfMemory();
        SDL_free(cursor);
        return NULL;
    }

    curdata->system_cursor = cursor_name;
    cursor->driverdata = curdata;

    return cursor;
}

static void
dummy_FreeCursor(SDL_Cursor* cursor)
{
    dummy_CursorData *curdata;
    if (cursor) {
        curdata = (dummy_CursorData *) cursor->driverdata;

        if (curdata != NULL) {
            SDL_free(cursor->driverdata);
        }

        SDL_free(cursor);
    }
}
static int
dummy_ShowCursor(SDL_Cursor* cursor)
{

}
static int
dummy_SetRelativeMouseMode(SDL_bool enabled)
{

}
static void
dummy_WarpMouse(SDL_Window* window, int x, int y)
{
    SDL_Unsupported();
}

void InitMouse(_THIS){

    SDL_Mouse* mouse = SDL_GetMouse();
//FIXME HANDLE CURSOR
/*
    mouse->CreateCursor         = dummy_CreateCursor;
*/ 
//    mouse->ShowCursor           = dummy_ShowCursor;
    mouse->FreeCursor           = dummy_FreeCursor;
//    mouse->WarpMouse            = dummy_WarpMouse;
    mouse->CreateSystemCursor   = dummy_CreateSystemCursor;
//    mouse->SetRelativeMouseMode = dummy_SetRelativeMouseMode;

    SDL_SetDefaultCursor(dummy_CreateDefaultCursor());

}

void FiniMouse(_THIS){

    SDL_Mouse* mouse = SDL_GetMouse();

   // dummy_FreeCursor(mouse->def_cursor);
   // mouse->def_cursor = NULL;

    mouse->CreateCursor         = NULL;
  //  mouse->ShowCursor           = NULL;
    mouse->FreeCursor           = NULL;
  //  mouse->WarpMouse            = NULL;
    mouse->CreateSystemCursor   = NULL;
  //  mouse->SetRelativeMouseMode = NULL;
}

void initkmap(_THIS){

   SDL_VideoData *phdata = (SDL_VideoData *) _this->driverdata;

phdata->kmap[RETROK_BACKSPACE      ]=SDLK_BACKSPACE;//8,
phdata->kmap[RETROK_TAB            ]=SDLK_TAB;//9,
phdata->kmap[RETROK_CLEAR          ]=SDLK_CLEAR;//12,
phdata->kmap[RETROK_RETURN         ]=SDLK_RETURN;//13,
phdata->kmap[RETROK_PAUSE          ]=SDLK_PAUSE;//19,
phdata->kmap[RETROK_ESCAPE         ]=SDLK_ESCAPE;//27,
phdata->kmap[RETROK_SPACE          ]=SDLK_SPACE;//32,
phdata->kmap[RETROK_EXCLAIM        ]=SDLK_EXCLAIM;//33,
phdata->kmap[RETROK_QUOTEDBL       ]=SDLK_QUOTEDBL;//34,
phdata->kmap[RETROK_HASH           ]=SDLK_HASH;//35,
phdata->kmap[RETROK_DOLLAR         ]=SDLK_DOLLAR;//36,
phdata->kmap[RETROK_AMPERSAND      ]=SDLK_AMPERSAND;//38,
phdata->kmap[RETROK_QUOTE          ]=SDLK_QUOTE;//39,
phdata->kmap[RETROK_LEFTPAREN      ]=SDLK_LEFTPAREN;//40,
phdata->kmap[RETROK_RIGHTPAREN     ]=SDLK_RIGHTPAREN;//41,
phdata->kmap[RETROK_ASTERISK       ]=SDLK_ASTERISK;//42,
phdata->kmap[RETROK_PLUS           ]=SDLK_PLUS;//43,
phdata->kmap[RETROK_COMMA          ]=SDLK_COMMA;//44,
phdata->kmap[RETROK_MINUS          ]=SDLK_MINUS;//45,
phdata->kmap[RETROK_PERIOD         ]=SDLK_PERIOD;//46,
phdata->kmap[RETROK_SLASH          ]=SDLK_SLASH;//47,
phdata->kmap[RETROK_0              ]=SDLK_0;//48,
phdata->kmap[RETROK_1              ]=SDLK_1;//49,
phdata->kmap[RETROK_2              ]=SDLK_2;//50,
phdata->kmap[RETROK_3              ]=SDLK_3;//51,
phdata->kmap[RETROK_4              ]=SDLK_4;//52,
phdata->kmap[RETROK_5              ]=SDLK_5;//53,
phdata->kmap[RETROK_6              ]=SDLK_6;//54,
phdata->kmap[RETROK_7              ]=SDLK_7;//55,
phdata->kmap[RETROK_8              ]=SDLK_8;//56,
phdata->kmap[RETROK_9              ]=SDLK_9;//57,
phdata->kmap[RETROK_COLON          ]=SDLK_COLON;//58,
phdata->kmap[RETROK_SEMICOLON      ]=SDLK_SEMICOLON;//59,
phdata->kmap[RETROK_LESS           ]=SDLK_LESS;//60,
phdata->kmap[RETROK_EQUALS         ]=SDLK_EQUALS;//61,
phdata->kmap[RETROK_GREATER        ]=SDLK_GREATER;//62,
phdata->kmap[RETROK_QUESTION       ]=SDLK_QUESTION;//63,
phdata->kmap[RETROK_AT             ]=SDLK_AT;//64,
phdata->kmap[RETROK_LEFTBRACKET    ]=SDLK_LEFTBRACKET;//91,
phdata->kmap[RETROK_BACKSLASH      ]=SDLK_BACKSLASH;//92,
phdata->kmap[RETROK_RIGHTBRACKET   ]=SDLK_RIGHTBRACKET;//93,
phdata->kmap[RETROK_CARET          ]=SDLK_CARET;//94,
phdata->kmap[RETROK_UNDERSCORE     ]=SDLK_UNDERSCORE;//95,
phdata->kmap[RETROK_BACKQUOTE      ]=SDLK_BACKQUOTE;//96,
phdata->kmap[RETROK_a              ]=SDLK_a;//97,
phdata->kmap[RETROK_b              ]=SDLK_b;//98,
phdata->kmap[RETROK_c              ]=SDLK_c;//99,
phdata->kmap[RETROK_d              ]=SDLK_d;//100,
phdata->kmap[RETROK_e              ]=SDLK_e;//101,
phdata->kmap[RETROK_f              ]=SDLK_f;//102,
phdata->kmap[RETROK_g              ]=SDLK_g;//103,
phdata->kmap[RETROK_h              ]=SDLK_h;//104,
phdata->kmap[RETROK_i              ]=SDLK_i;//105,
phdata->kmap[RETROK_j              ]=SDLK_j;//106,
phdata->kmap[RETROK_k              ]=SDLK_k;//107,
phdata->kmap[RETROK_l              ]=SDLK_l;//108,
phdata->kmap[RETROK_m              ]=SDLK_m;//109,
phdata->kmap[RETROK_n              ]=SDLK_n;//110,
phdata->kmap[RETROK_o              ]=SDLK_o;//111,
phdata->kmap[RETROK_p              ]=SDLK_p;//112,
phdata->kmap[RETROK_q              ]=SDLK_q;//113,
phdata->kmap[RETROK_r              ]=SDLK_r;//114,
phdata->kmap[RETROK_s              ]=SDLK_s;//115,
phdata->kmap[RETROK_t              ]=SDLK_t;//116,
phdata->kmap[RETROK_u              ]=SDLK_u;//117,
phdata->kmap[RETROK_v              ]=SDLK_v;//118,
phdata->kmap[RETROK_w              ]=SDLK_w;//119,
phdata->kmap[RETROK_x              ]=SDLK_x;//120,
phdata->kmap[RETROK_y              ]=SDLK_y;//121,
phdata->kmap[RETROK_z              ]=SDLK_z;//122,
phdata->kmap[RETROK_DELETE         ]=SDLK_DELETE;//127,

phdata->kmap[RETROK_KP0            ]=SDLK_KP_0;//256,
phdata->kmap[RETROK_KP1            ]=SDLK_KP_1;//257,
phdata->kmap[RETROK_KP2            ]=SDLK_KP_2;//258,
phdata->kmap[RETROK_KP3            ]=SDLK_KP_3;//259,
phdata->kmap[RETROK_KP4            ]=SDLK_KP_4;//260,
phdata->kmap[RETROK_KP5            ]=SDLK_KP_5;//261,
phdata->kmap[RETROK_KP6            ]=SDLK_KP_6;//262,
phdata->kmap[RETROK_KP7            ]=SDLK_KP_7;//263,
phdata->kmap[RETROK_KP8            ]=SDLK_KP_8;//264,
phdata->kmap[RETROK_KP9            ]=SDLK_KP_9;//265,
phdata->kmap[RETROK_KP_PERIOD      ]=SDLK_KP_PERIOD;//266,
phdata->kmap[RETROK_KP_DIVIDE      ]=SDLK_KP_DIVIDE;//267,
phdata->kmap[RETROK_KP_MULTIPLY    ]=SDLK_KP_MULTIPLY;//268,
phdata->kmap[RETROK_KP_MINUS       ]=SDLK_KP_MINUS;//269,
phdata->kmap[RETROK_KP_PLUS        ]=SDLK_KP_PLUS;//270,
phdata->kmap[RETROK_KP_ENTER       ]=SDLK_KP_ENTER;//271,
phdata->kmap[RETROK_KP_EQUALS      ]=SDLK_KP_EQUALS;//272,

phdata->kmap[RETROK_UP             ]=SDLK_UP;//273,
phdata->kmap[RETROK_DOWN           ]=SDLK_DOWN;//274,
phdata->kmap[RETROK_RIGHT          ]=SDLK_RIGHT;//275,
phdata->kmap[RETROK_LEFT           ]=SDLK_LEFT;//276,
phdata->kmap[RETROK_INSERT         ]=SDLK_INSERT;//277,
phdata->kmap[RETROK_HOME           ]=SDLK_HOME;//278,
phdata->kmap[RETROK_END            ]=SDLK_END;//279,
phdata->kmap[RETROK_PAGEUP         ]=SDLK_PAGEUP;//280,
phdata->kmap[RETROK_PAGEDOWN       ]=SDLK_PAGEDOWN;//281,

phdata->kmap[RETROK_F1             ]=SDLK_F1;//282,
phdata->kmap[RETROK_F2             ]=SDLK_F2;//283,
phdata->kmap[RETROK_F3             ]=SDLK_F3;//284,
phdata->kmap[RETROK_F4             ]=SDLK_F4;//285,
phdata->kmap[RETROK_F5             ]=SDLK_F5;//286,
phdata->kmap[RETROK_F6             ]=SDLK_F6;//287,
phdata->kmap[RETROK_F7             ]=SDLK_F7;//288,
phdata->kmap[RETROK_F8             ]=SDLK_F8;//289,
phdata->kmap[RETROK_F9             ]=SDLK_F9;//290,
phdata->kmap[RETROK_F10            ]=SDLK_F10;//291,
phdata->kmap[RETROK_F11            ]=SDLK_F11;//292,
phdata->kmap[RETROK_F12            ]=SDLK_F12;//293,
phdata->kmap[RETROK_F13            ]=SDLK_F13;//294,
phdata->kmap[RETROK_F14            ]=SDLK_F14;//295,
phdata->kmap[RETROK_F15            ]=SDLK_F15;//296,

phdata->kmap[RETROK_NUMLOCK        ]=SDLK_NUMLOCKCLEAR;//300,
phdata->kmap[RETROK_CAPSLOCK       ]=SDLK_CAPSLOCK;//301,
phdata->kmap[RETROK_SCROLLOCK      ]=SDLK_SCROLLLOCK;//302,
phdata->kmap[RETROK_RSHIFT         ]=SDLK_RSHIFT;//303,
phdata->kmap[RETROK_LSHIFT         ]=SDLK_LSHIFT;//304,
phdata->kmap[RETROK_RCTRL          ]=SDLK_RCTRL;//305,
phdata->kmap[RETROK_LCTRL          ]=SDLK_LCTRL;//306,
phdata->kmap[RETROK_RALT           ]=SDLK_RALT;//307,
phdata->kmap[RETROK_LALT           ]=SDLK_LALT;//308,
phdata->kmap[RETROK_RMETA          ]=SDLK_RGUI;//309,
phdata->kmap[RETROK_LMETA          ]=SDLK_LGUI;//310,
phdata->kmap[RETROK_LSUPER         ]=SDLK_LGUI;//311,
phdata->kmap[RETROK_RSUPER         ]=SDLK_RGUI;//312,
phdata->kmap[RETROK_MODE           ]=SDLK_MODE;//313,
phdata->kmap[RETROK_COMPOSE        ]=SDLK_APPLICATION;//314,

phdata->kmap[RETROK_HELP           ]=SDLK_HELP;//315,
phdata->kmap[RETROK_PRINT          ]=SDLK_PRINTSCREEN;//316,
phdata->kmap[RETROK_SYSREQ         ]=SDLK_SYSREQ;//317,
phdata->kmap[RETROK_BREAK          ]=SDLK_STOP;//318,
phdata->kmap[RETROK_MENU           ]=SDLK_MENU;//319,
phdata->kmap[RETROK_POWER          ]=SDLK_POWER;//320,
phdata->kmap[RETROK_EURO           ]=SDLK_UNKNOWN;//321,
phdata->kmap[RETROK_UNDO           ]=SDLK_UNDO;//322,

}

void Process_key(_THIS)
{
	int i;

    SDL_VideoData *phdata = (SDL_VideoData *) _this->driverdata;

	for(i=0;i<322;i++)
        	phdata->Key_Sate[i]=libretro_input_state_cb(0, RETRO_DEVICE_KEYBOARD, 0,i) ? 0x80: 0;
   
	if(memcmp( phdata->Key_Sate,phdata->old_Key_Sate , sizeof(phdata->Key_Sate) ) )
	 	for(i=0;i<322;i++)
			if(phdata->Key_Sate[i] && phdata->Key_Sate[i]!=phdata->old_Key_Sate[i]  )
        	{
				//printf("press: %d \n",i);
                    RETROMOD(1,phdata->kmap[i]);
	
        	}	
        	else if ( !phdata->Key_Sate[i] && phdata->Key_Sate[i]!=phdata->old_Key_Sate[i]  )
        	{
				//printf("release: %d \n",i);
                    RETROMOD(0,phdata->kmap[i]);
	
        	}	

	memcpy(phdata->old_Key_Sate,phdata->Key_Sate , sizeof(phdata->Key_Sate) );

}

void PumpMouse(_THIS){

    SDL_Window *win = (SDL_Window *)_this->windows;
    SDL_VideoData *phdata = (SDL_VideoData *) _this->driverdata;

    //update position
    int tmx,tmy;
    static int oldx=160,oldy=120;

    tmx=libretro_input_state_cb(0, RETRO_DEVICE_MOUSE, 0, RETRO_DEVICE_ID_MOUSE_X);
    tmy=libretro_input_state_cb(0, RETRO_DEVICE_MOUSE, 0, RETRO_DEVICE_ID_MOUSE_Y);

    phdata->_omx += tmx;
    phdata->_omy += tmy;

#ifdef WIIU
	int p_x = libretro_input_state_cb(0, RETRO_DEVICE_POINTER, 0, RETRO_DEVICE_ID_POINTER_X);
	int p_y = libretro_input_state_cb(0, RETRO_DEVICE_POINTER, 0, RETRO_DEVICE_ID_POINTER_Y);
	int p_press  = libretro_input_state_cb(0, RETRO_DEVICE_POINTER, 0,RETRO_DEVICE_ID_POINTER_PRESSED);
	int px=(int)((p_x+0x7fff)*phdata->w /0xffff);
	int py=(int)((p_y+0x7fff)*phdata->h/0xffff);
    phdata->_omx=px;
    phdata->_omy=py;
#endif

    if(phdata->_omx<0)phdata->_omx=0;
    if(phdata->_omy<0)phdata->_omy=0;
    if(phdata->_omx>phdata->w-1)phdata->_omx=phdata->w-1;
    if(phdata->_omy>phdata->h-1)phdata->_omy=phdata->h-1;

    if(oldx!=phdata->_omx || oldy!=phdata->_omy)
            SDL_SendMouseMotion(win, 0, 0, phdata->_omx,phdata->_omy);

    oldx=phdata->_omx;
    oldy=phdata->_omy;

    //update button
    Uint8 butt=0;

    if(libretro_input_state_cb(0, RETRO_DEVICE_MOUSE, 0, RETRO_DEVICE_ID_MOUSE_LEFT))butt |= 0x1;
    if(libretro_input_state_cb(0, RETRO_DEVICE_MOUSE, 0, RETRO_DEVICE_ID_MOUSE_RIGHT))butt |= 0x2;
    if(libretro_input_state_cb(0, RETRO_DEVICE_MOUSE, 0, RETRO_DEVICE_ID_MOUSE_MIDDLE))butt |= 0x4;
#ifdef WIIU
    if(libretro_input_state_cb(0,RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_L3))butt |= 0x1;
    if(libretro_input_state_cb(0,RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_R3))butt |= 0x2;
#endif
    // Check left mouse button changes
    bool oldLMB = phdata->_mouseButtons & 1;
    bool newLMB = butt & 1;
    if (newLMB != oldLMB) {
       SDL_SendMouseMotion(win, 0, 0, phdata->_omx,phdata->_omy);
       SDL_SendMouseButton(win, 0,newLMB ? SDL_PRESSED : SDL_RELEASED, SDL_BUTTON_LEFT);
       // SDL_PrivateMouseButton(newLMB ? SDL_PRESSED : SDL_RELEASED,SDL_BUTTON_LEFT, 0, 0);

    }

    // Check rigth mouse button changes
    bool oldRMB = phdata->_mouseButtons & 2;
    bool newRMB = butt & 2;
    if (newRMB != oldRMB) {
       SDL_SendMouseMotion(win, 0, 0, phdata->_omx,phdata->_omy);
       SDL_SendMouseButton(win,0, newRMB ? SDL_PRESSED : SDL_RELEASED, SDL_BUTTON_RIGHT);
      //   SDL_PrivateMouseButton(newRMB ? SDL_PRESSED : SDL_RELEASED,SDL_BUTTON_RIGHT,0,0);
    }

    // Check middle mouse button changes
    bool oldMMB = phdata->_mouseButtons & 4;
    bool newMMB = butt & 4;
    if (newMMB != oldMMB) {
       SDL_SendMouseMotion(win, 0, 0, phdata->_omx,phdata->_omy);
       SDL_SendMouseButton(win,0, newMMB ? SDL_PRESSED : SDL_RELEASED, SDL_BUTTON_MIDDLE);
       //SDL_PrivateMouseButton(newMMB ? SDL_PRESSED : SDL_RELEASED, SDL_BUTTON_MIDDLE, 0,0);
    }

    phdata->_mouseButtons = butt;

}

void
DUMMY_PumpEvents(_THIS)
{
    /* do nothing. */

	static Uint32 last=0;
	Uint32 now;

    //if(last==0)SDL_SetDefaultCursor(null_CreateDefaultCursor());

	now  = SDL_GetTicks()-last;
    //printf("now:%d\n",now);
	if(now<10)return;


	PumpMouse(_this);
    Process_key(_this);

	last  = SDL_GetTicks();
}

#endif /* SDL_VIDEO_DRIVER_DUMMY */

/* vi: set ts=4 sw=4 expandtab: */
