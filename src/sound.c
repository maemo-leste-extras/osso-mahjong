/**
    @file sound.c

    libcanbeera Sound functionality.

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

#include "sound.h"
#if 0
#include <unistd.h>
#include <stdlib.h>
#include <locale.h>
#include <stdio.h>
#endif
#include <canberra.h>




#define COMMON SOUNDSDIR "/game-mahjong_"

gboolean
sound_init(AppSoundData * app_sound_data)
{
    if (!app_sound_data)
    {
	return FALSE;
    }
    int ret = -1;
    if (!app_sound_data->ca)
    {
         if ((ret = ca_context_create (&(app_sound_data->ca))) != CA_SUCCESS)
         {
                g_warning("ca_context_create: %s", ca_strerror (ret));
                return FALSE;
         }
   	 else if ((ret = ca_context_open (app_sound_data->ca)) != CA_SUCCESS)
   	 {
        	g_warning("ca_context_open: %s", ca_strerror (ret));
         	ca_context_destroy(app_sound_data->ca);
         	app_sound_data->ca = NULL;
         	return FALSE;
   	 }
    }

    ca_proplist_create (&(app_sound_data->pl));
    return TRUE;
}

void
sound_deinit(AppSoundData * app_sound_data)
{
	if (!app_sound_data || !app_sound_data->ca || !app_sound_data->pl)
    {
        return;
    }

    ca_context_destroy(app_sound_data->ca);
    ca_proplist_destroy(app_sound_data->pl);
    return;
}


gboolean
sound_play(AppSoundData * app_sound_data, enum SoundType sound)
{

	if (!app_sound_data || !app_sound_data->ca || !app_sound_data->pl)
    {
    	return FALSE;
    }
    
    const gchar * fname = NULL;
    int ret = -1;

    static const gchar *sounds[SOUND_COUNT] = {
        COMMON "successful_selection.wav",
        COMMON "incorrect_selection.wav",
        COMMON "correct_combination.wav",
        COMMON "incorrect_combination.wav",
        COMMON "hint_usage.wav",
        COMMON "shuffle_usage.wav",
        COMMON "undo_usage.wav",
    };

    fname = sounds[sound];
    ca_proplist_sets (app_sound_data->pl, CA_PROP_MEDIA_FILENAME, fname);
    ca_proplist_sets (app_sound_data->pl, CA_PROP_MEDIA_ROLE , "x-maemo");
    

    if ((ret = ca_context_play_full (app_sound_data->ca, 0, app_sound_data->pl, NULL, NULL)) != CA_SUCCESS)
    {
	g_warning("ca_context_play_full: %s", ca_strerror (ret));
        return FALSE;
    }
    return TRUE;
}
