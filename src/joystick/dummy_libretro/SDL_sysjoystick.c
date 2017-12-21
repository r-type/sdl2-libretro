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

#if defined(SDL_JOYSTICK_DUMMY) || defined(SDL_JOYSTICK_DISABLED)

/* This is the dummy implementation of the SDL joystick API */

#include "SDL_joystick.h"
#include "../SDL_sysjoystick.h"
#include "../SDL_joystick_c.h"
#include "SDL_events.h"

#include "../../video/dummy_libretro/libretro.h"
//extern retro_input_state_t input_state_cb;
extern short int libretro_input_state_cb(unsigned port,unsigned device,unsigned index,unsigned id);

#define NAMESIZE 20

typedef struct SDL_LIBRETRO_JoyData
{
	char name[NAMESIZE];
} SDL_LIBRETRO_JoyData;

struct joystick_hwdata
{
    char but[16];
    int an[4];// 0/1 left analog 2/3 right analog
};

static SDL_LIBRETRO_JoyData joy_data[4];


/* Function to scan the system for joysticks.
 * It should return 0, or -1 on an unrecoverable fatal error.
 */
int
SDL_SYS_JoystickInit(void)
{
	int i = 0;
	//SDL_numjoysticks = 4;

	SDL_zero( joy_data);


	for(i = 0; i < 4; i++)
	{

		{
			sprintf( joy_data[i].name, "LIBRETRO_JOYPAD%02X", i);			
		} 
	}

    return 4;
}

int SDL_SYS_NumJoysticks()
{
    return 4;
//    return 0;
}

void SDL_SYS_JoystickDetect()
{
}

/* Function to get the device-dependent name of a joystick */
const char *
SDL_SYS_JoystickNameForDeviceIndex(int device_index)
{

    return joy_data[device_index].name;

  //  SDL_SetError("Logic error: No joysticks available");
  //  return (NULL);
}

/* Function to perform the mapping from device index to the instance id for this index */
SDL_JoystickID SDL_SYS_GetInstanceIdOfDeviceIndex(int device_index)
{
    return device_index;
}

/* Function to open a joystick for use.
   The joystick to open is specified by the device index.
   This should fill the nbuttons and naxes fields of the joystick structure.
   It returns 0, or -1 if there is an error.
 */
int
SDL_SYS_JoystickOpen(SDL_Joystick * joystick, int device_index)
{

    if (!(joystick->hwdata = SDL_malloc(sizeof(struct joystick_hwdata))))
        return -1;
    int i;
    for(i=0;i<16;i++)joystick->hwdata->but[i]=0;

    joystick->hwdata->an[0]=0;
    joystick->hwdata->an[1]=0;
    joystick->hwdata->an[2]=0;
    joystick->hwdata->an[3]=0;

	joystick->naxes = 4;
	joystick->nhats = 0;
	joystick->nballs = 0;
	joystick->nbuttons = 16;

    return 0;

    //return SDL_SetError("Logic error: No joysticks available");
}

/* Function to determine if this joystick is attached to the system right now */
SDL_bool SDL_SYS_JoystickAttached(SDL_Joystick *joystick)
{
    return SDL_TRUE;
}

/* Function to update the state of a joystick - called as a device poll.
 * This function shouldn't update the joystick structure directly,
 * but instead should call SDL_PrivateJoystick*() to deliver events
 * and update joystick device state.
 */
void
SDL_SYS_JoystickUpdate(SDL_Joystick * joystick)
{
      int new_but[16];
      int new_an[4];
      int i;
      //FIXME only joy 0 for now
      for(i=0;i<16;i++){
            new_but[i]=0;
           if( libretro_input_state_cb(0/*joystick->index*/, RETRO_DEVICE_JOYPAD, 0, i))
                new_but[i]=1;
      }            

      for(i=0;i<16;i++){

            if( new_but[i] != joystick->hwdata->but[i]) {
                if( new_but[i]== 0){ 
		            SDL_PrivateJoystickButton( joystick, (i), SDL_RELEASED); 
                }
		        else {
		            SDL_PrivateJoystickButton( joystick, (i), SDL_PRESSED); 
                }
            }
            joystick->hwdata->but[i]=new_but[i];
      }

      new_an[0] = (libretro_input_state_cb(0/*joystick->index*/, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_LEFT,  RETRO_DEVICE_ID_ANALOG_X));
      new_an[1] = (libretro_input_state_cb(0/*joystick->index*/, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_LEFT,  RETRO_DEVICE_ID_ANALOG_Y));
      new_an[2] = (libretro_input_state_cb(0/*joystick->index*/, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_RIGHT, RETRO_DEVICE_ID_ANALOG_X));
      new_an[3] = (libretro_input_state_cb(0/*joystick->index*/, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_RIGHT, RETRO_DEVICE_ID_ANALOG_Y));

      for(i=0;i<4;i++){
	        if( new_an[i] != joystick->hwdata->an[i]) {
		        SDL_PrivateJoystickAxis( joystick, i, new_an[i]); 
	        } 
	        joystick->hwdata->an[i] = new_an[i];
      }


}

/* Function to close a joystick after use */
void
SDL_SYS_JoystickClose(SDL_Joystick * joystick)
{
    if (joystick->hwdata)
        SDL_free(joystick->hwdata);
}

/* Function to perform any system-specific joystick related cleanup */
void
SDL_SYS_JoystickQuit(void)
{
}

SDL_JoystickGUID SDL_SYS_JoystickGetDeviceGUID( int device_index )
{
    SDL_JoystickGUID guid;
    /* the GUID is just the first 16 chars of the name for now */
    const char *name = SDL_SYS_JoystickNameForDeviceIndex( device_index );
    SDL_zero( guid );
    SDL_memcpy( &guid, name, SDL_min( sizeof(guid), SDL_strlen( name ) ) );
    return guid;
}


SDL_JoystickGUID SDL_SYS_JoystickGetGUID(SDL_Joystick * joystick)
{
    SDL_JoystickGUID guid;
    /* the GUID is just the first 16 chars of the name for now */
    const char *name = joystick->name;
    SDL_zero( guid );
    SDL_memcpy( &guid, name, SDL_min( sizeof(guid), SDL_strlen( name ) ) );
    return guid;
}

#endif /* SDL_JOYSTICK_DUMMY || SDL_JOYSTICK_DISABLED */

/* vi: set ts=4 sw=4 expandtab: */
