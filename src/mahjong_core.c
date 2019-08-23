/**
    @file mahjong_core.c

    Implements MahjongCore

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

#include "mahjong_core.h"
#include "mahjong/games-clock.h"
#include "mahjong/mahjongg.h"
#include "state_save.h"

#define PAUSE_WITH_HW_KEY "/apps/osso/games_startup/pause_with_hw_key"


void
mahjong_home(GtkWidget * widget, gpointer data)
{
    (void) widget;


    // settings_set_int(PAUSE_WITH_HW_KEY,286);


    /* CID 7265 */
    if (data != NULL)
    {
    	if (((AppUIData *) data)->dialog)
    	{
        	if (!data)
            		return;
        	gtk_dialog_response(GTK_DIALOG(((AppUIData *) data)->dialog),
                	            /*GTK_RESPONSE_ACCEPT*/GTK_RESPONSE_OK);
        	return;
    	}


    	if (!data)
        	return;
    }

    /* Pause the clock */
    games_clock_stop(GAMES_CLOCK(((AppUIData *) data)->chrono));

    /* Hide main window */
    ui_view_main_window((AppUIData *) data, FALSE);
    gtk_main_iteration();

    /* Set game paused, send message to start up screen */

    save_state(get_app_data());
    /* Hide window */
    // gtk_widget_hide( GTK_WIDGET( ((AppUIData*)data)->window ) );

    deinit_mahjong();
    app_remove_timeout();

    gtk_main_quit();
}


void
mahjong_pause(GtkWidget * widget, gpointer data)
{
    (void) widget;

    if (!data)
        return;
    if (((AppUIData *) data)->dialog)
    {
        gtk_dialog_response(GTK_DIALOG(((AppUIData *) data)->dialog),
                            /*GTK_RESPONSE_ACCEPT*/GTK_RESPONSE_OK);
        return;
    }
    /* Pause the clock */
    games_clock_stop(GAMES_CLOCK(((AppUIData *) data)->chrono));

    /* Hide main window */
    ui_view_main_window((AppUIData *) data, FALSE);

    /* Set game paused, send message to start up screen */
    set_game_state(GAME_PAUSE_METHOD);

    /* Hide window */
    gtk_widget_hide(GTK_WIDGET(((AppUIData *) data)->window));

    save_state(get_app_data());
    deinit_mahjong();
    app_remove_timeout();
    gtk_main_quit();
}

void
mahjong_close(void)
{
    deinit_mahjong();
    app_remove_timeout();
    gtk_main_quit();
}

void
mahjong_end(void)
{
    if (!get_app_data())
        return;


    AppUIData *data = get_app_data()->app_ui_data;
    /* CID 7264 */
    if (data != NULL)
    {
	    if (data->dialog)
	    {
		if (!data)
		    return;
		gtk_dialog_response(GTK_DIALOG(data->dialog), /*GTK_RESPONSE_ACCEPT*/GTK_RESPONSE_OK);
		return;
    	    }

    }
    /* Hide main window */
    ui_view_main_window((AppUIData *) data, FALSE);

    set_game_state(GAME_END_METHOD);

    /* Hide window */
    gtk_widget_hide(GTK_WIDGET(((AppUIData *) data)->window));

    deinit_mahjong();
    app_remove_timeout();
    gtk_main_quit();
}
