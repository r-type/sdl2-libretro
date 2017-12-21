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

/* Output audio to nowhere... */

#include "SDL_timer.h"
#include "SDL_audio.h"
#include "../SDL_audio_c.h"
#include "SDL_dummyaudio.h"

static int
DUMMYAUDIO_OpenDevice(_THIS, void *handle, const char *devname, int iscapture)
{
    /* handle != NULL means "user specified the placeholder name on the fake detected device list" */
 
    this->hidden = (struct SDL_PrivateAudioData *)
        SDL_malloc(sizeof(*this->hidden));
    if (this->hidden == NULL) {
        return SDL_OutOfMemory();
    }

    SDL_zerop(this->hidden);

    //this->spec.samples = 1024;
    //this->spec.freq = 44100;

    this->spec.format = AUDIO_S16LSB;
    
    SDL_CalculateAudioSpec(&this->spec);

    this->hidden->write_delay = ((this->spec.samples * 1000) / this->spec.freq);
 	this->hidden->initial_calls = 2;


    /* Allocate mixing buffer */
    if (!iscapture) {
        this->hidden->mixlen = this->spec.size;
        this->hidden->mixbuf = (Uint8 *) SDL_malloc(this->spec.size);
        if (this->hidden->mixbuf == NULL) {
            return SDL_OutOfMemory();
        }
        SDL_memset(this->hidden->mixbuf, this->spec.silence, this->spec.size);
    }
    //printf("open audio dev\n");
    return 0;                   /* always succeeds. */
}


static void
DUMMYAUDIO_PlayDevice(_THIS)
{
	SDL_AudioSpec *spec = &this->spec;
	int i;

	//printf("sz:%d c:%d freq:%d fmt:%d\n",spec->size,spec->channels,spec->freq,spec->format);
	Sint16 *ptr=(Sint16*)this->hidden->mixbuf;

        if (this->spec.channels == 1){
    		for(i=0;i<spec->size/2/2;i++)libretro_audio_cb(*ptr,*ptr++);
        }
        else {
            for(i=0;i< spec->size/spec->channels/2;i++)libretro_audio_cb(*ptr++,*ptr++);
        }

}

static void
DUMMYAUDIO_WaitDevice(_THIS)
{
/*
	if (this->hidden->initial_calls)
		this->hidden->initial_calls--;
	else
		SDL_Delay(this->hidden->write_delay);
*/
}


static void
DUMMYAUDIO_CloseDevice(_THIS)
{
	if ( this->hidden->mixbuf != NULL ) {
        SDL_free(this->hidden->mixbuf);
		this->hidden->mixbuf = NULL;
	}

    SDL_free(this->hidden);
}

static Uint8 *
DUMMYAUDIO_GetDeviceBuf(_THIS)
{
    return (this->hidden->mixbuf);
}

static int
DUMMYAUDIO_Init(SDL_AudioDriverImpl * impl)
{
    /* Set the function pointers */
    impl->OpenDevice = DUMMYAUDIO_OpenDevice;
    impl->WaitDevice = DUMMYAUDIO_WaitDevice;
    impl->PlayDevice = DUMMYAUDIO_PlayDevice;
    impl->GetDeviceBuf = DUMMYAUDIO_GetDeviceBuf;
  //  impl->CaptureFromDevice = DUMMYAUDIO_CaptureFromDevice;
    impl->HasCaptureSupport = SDL_FALSE;

    impl->CloseDevice = DUMMYAUDIO_CloseDevice;
    impl->OnlyHasDefaultOutputDevice = 1;
   // impl->OnlyHasDefaultCaptureDevice = 1;
   // impl->HasCaptureSupport = SDL_TRUE;

    return 1;   /* this audio target is available. */
}

AudioBootStrap DUMMYAUDIO_bootstrap = {
    "dummy", "SDL dummy audio driver", DUMMYAUDIO_Init, 0
};

/* vi: set ts=4 sw=4 expandtab: */
