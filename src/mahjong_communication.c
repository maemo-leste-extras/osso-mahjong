/**
    @file mahjong_communication.c

    Implements Communication module

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

#include "mahjong_communication.h"
#include "mahjong/games-clock.h"
#include "mahjong/mahjongg.h"
#include "state_save.h"
#include "sound.h"

gint
handle_dbus_message(const gchar * method, GArray * arguments,
                    gpointer data, osso_rpc_t * retval)
{
    (void) arguments;
    (void) data;
    g_assert(method);

    if (!method || !retval)
        return OSSO_INVALID;

    osso_log(LOG_DEBUG, "DBUS method: %s", method);

    /* Handle start game and restart */
    if (g_ascii_strcasecmp(method, GAME_RUN_METHOD) == 0 ||
        g_ascii_strcasecmp(method, GAME_RESTART_METHOD) == 0)
    {

        AppData *app_data = get_app_data();
        gint map = 0;

        osso_log(LOG_DEBUG, "Starting new game");

        /* Reading map setting */
        map = settings_get_int(SETTINGS_DIFFICULTY);

        /* Starting the game */
        set_map_selection(map);
        new_game(TRUE);

        if (app_data)
        {
            if (settings_get_bool(SETTINGS_ENABLE_SOUND))
                sound_init(app_data->app_sound_data);
            else
                sound_deinit(app_data->app_sound_data);

            /* Show main window */
            ui_view_main_window(app_data->app_ui_data, TRUE);

            /* Set return value */
            g_assert(retval);
            retval->type = DBUS_TYPE_BOOLEAN;
            retval->value.b = TRUE;

            return OSSO_OK;
        }
        else
        {
            retval->type = DBUS_TYPE_BOOLEAN;
            retval->value.b = FALSE;
            return OSSO_ERROR;
        }
    }
    /* Handle continue game */
    else if (g_ascii_strcasecmp(method, GAME_CONTINUE_METHOD) == 0)
    {

        AppData *app_data = get_app_data();

        read_state(app_data);

        osso_log(LOG_DEBUG, "Continuing game");

        /* Continue clock */
        if (app_data)
        {
            if (app_data->app_ui_data)
                games_clock_start(GAMES_CLOCK(app_data->app_ui_data->chrono));

            if (settings_get_bool(SETTINGS_ENABLE_SOUND))
                sound_init(app_data->app_sound_data);
            else
                sound_deinit(app_data->app_sound_data);

            /* Show main window */
            ui_view_main_window(app_data->app_ui_data, TRUE);

            /* Set return value */
            g_assert(retval);
            retval->type = DBUS_TYPE_BOOLEAN;
            retval->value.b = TRUE;
        }
        else
        {
            g_assert(retval);
            retval->type = DBUS_TYPE_BOOLEAN;
            retval->value.b = FALSE;
            return OSSO_ERROR;
        }

        return OSSO_OK;
    }
    /* Handle close game */
    else if (g_ascii_strcasecmp(method, GAME_CLOSE_METHOD) == 0)
    {

        osso_log(LOG_DEBUG, "Closing game");

        /* Set return value */
        g_assert(retval);
        retval->type = DBUS_TYPE_BOOLEAN;
        retval->value.b = TRUE;

        /* Close the game */
        mahjong_close();

        return OSSO_OK;
    }

    return OSSO_ERROR;
}

gboolean
set_game_state(gchar * method)
{
    osso_return_t ret;
    osso_rpc_t retval;

    if (!method)
        return FALSE;

    osso_log(LOG_DEBUG, "Writing to DBUS: %s\n", method);

    /* Send message */
    ret = send_dbus_message(STARTUP_SERVICE,
                            STARTUP_OBJECT_PATH,
                            STARTUP_IFACE,
                            method, NULL, &retval, get_app_data());

    osso_log(LOG_DEBUG, "Return value type: %d\n", retval.type);
    osso_rpc_free_val(&retval);

    if (ret == OSSO_OK)
        return TRUE;
    return FALSE;
}
