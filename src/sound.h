/**
	 @file sound.c

    ESD Sound headers.

    Copyright (c) 2004, 2005 Nokia Corporation.
	
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
    02111-1307 USA
*/

#ifndef SOUND_H
#define SOUND_H

#include <glib.h>
#include "sounds.h"
#include "appdata.h"

/**
 Initialize sounds.

 @param app_sound_data Pointer to the sound data structure.
 @return TRUE on success, FAIL on failure.
*/
gboolean sound_init(AppSoundData * app_sound_data);

/**
 Deinitialize sounds.

 @param app_sound_data Pointer to the sound data structure.
*/
void sound_deinit(AppSoundData * app_sound_data);

/**
 Play sound.

 @param app_sound_data Pointer to the sound data structure.
 @param sound ID of the sound to be played.
 @return TRUE on success, FALSE otherwise.
*/
gboolean sound_play(AppSoundData * app_sound_data, enum SoundType sound);

#endif
