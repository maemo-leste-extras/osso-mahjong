/**
    @file main.c

    Application main.

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

#include "main.h"
#include "sound.h"
#define PAUSE_WITH_HW_KEY "/apps/osso/games_startup/pause_with_hw_key"

void
sig_handler(int sig)
{
    osso_log(LOG_DEBUG, "Mahjong caught signal %d", sig);

    if (sig == SIGINT)
    {
        mahjong_end();
        mahjong_close();
    }
}

int
main(int argc, char **argv)
{
    AppData *app_data;

    ULOG_OPEN(PACKAGE_NAME " " PACKAGE_VERSION);

    setlocale(LC_ALL, "");
    bindtextdomain(GETTEXT_PACKAGE, LOCALEDIR);
    bind_textdomain_codeset(GETTEXT_PACKAGE, "UTF-8");
    textdomain(GETTEXT_PACKAGE);

    gtk_init(&argc, &argv);

    osso_log(LOG_DEBUG, "Mahjong start");

    app_data = g_new0(AppData, 1);
    app_data->app_ui_data = g_new0(AppUIData, 1);
    app_data->app_osso_data = g_new0(AppOSSOData, 1);
    app_data->app_gconf_data = g_new0(AppGConfData, 1);
    app_data->app_sound_data = g_new0(AppSoundData, 1);

    ui_create_main_window(app_data);

    signal(SIGINT, sig_handler);

    if (!init_osso(app_data))
    {
        osso_log(LOG_ERR, "Osso initialization failed");
        return 1;
    }

    /* Init GConf */
    if (!init_settings(app_data))
    {
        osso_log(LOG_ERR, "GConf initialization failed");
        return 1;
    }

    if (settings_get_bool(SETTINGS_ENABLE_SOUND))
        sound_init(app_data->app_sound_data);
    settings_set_int(PAUSE_WITH_HW_KEY, 0);
    ui_view_main_window(app_data->app_ui_data, FALSE);
    gtk_main();

    deinit_osso(app_data);

    sound_deinit(app_data->app_sound_data);

    g_free(app_data->app_ui_data);
    g_free(app_data->app_osso_data);
    g_free(app_data->app_gconf_data);
    g_free(app_data->app_sound_data);
    g_free(app_data);

    LOG_CLOSE();

    return 0;
}
