/**
    @file interface.c

    Implementation of the user interface functions.

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

/* Include function prototypes */
#include "interface.h"
#include "../mahjong/games-clock.h"

/* Show or hide main window */
void
ui_view_main_window(AppUIData * app_ui_data, gboolean show)
{
    g_assert(app_ui_data);

    if (show == TRUE)
    {
        osso_log(LOG_DEBUG, "Showing window and setting full screen");

        gtk_window_fullscreen(app_ui_data->window);
        gtk_widget_show_all(GTK_WIDGET(app_ui_data->window));
    }
    else
    {
        osso_log(LOG_DEBUG, "Hiding window");
        gtk_widget_hide_all(GTK_WIDGET(app_ui_data->window));
        gtk_window_unfullscreen(app_ui_data->window);
        // gtk_widget_hide_all( GTK_WIDGET (app_ui_data->window) );
    }
}

/* Create the main window */
void
ui_create_main_window(AppData * app_data)
{
    AppUIData *app_ui_data;

    /* Paranoia */
    g_assert(app_data);

    app_ui_data = app_data->app_ui_data;

    app_ui_data->window = ui_create_main_app();

    app_data->app_ui_data->dialog = NULL;

    gtk_container_set_border_width(GTK_CONTAINER(app_ui_data->window), 0);

    /* Create a hbox for all our stuff inside main view */
    app_ui_data->main_hbox = gtk_hbox_new(FALSE, 0);

    gtk_container_set_border_width(GTK_CONTAINER(app_ui_data->main_hbox), 0);

    gtk_container_add(GTK_CONTAINER(app_ui_data->window),
                      app_ui_data->main_hbox);

    /* Singal connect */
    g_signal_connect(G_OBJECT(app_ui_data->window), "key-press-event",
                     G_CALLBACK(key_press), app_ui_data);

    g_signal_connect(G_OBJECT(app_ui_data->window), "key-release-event",
                     G_CALLBACK(key_release), app_ui_data);
    gtk_widget_add_events(GTK_WIDGET(app_ui_data->window),
                          GDK_VISIBILITY_NOTIFY_MASK);
    g_signal_connect(G_OBJECT(app_ui_data->window), "visibility-notify-event",
                     G_CALLBACK(visibility_notify_event), app_ui_data);

    g_signal_connect(app_ui_data->window, "focus-out-event",
                     G_CALLBACK(main_win_focus_out), app_ui_data);

    /* Add event handling for overlay area */
    gtk_widget_add_events(GTK_WIDGET(app_ui_data->window),
		    GDK_BUTTON_PRESS_MASK |
		    GDK_BUTTON_RELEASE_MASK);
    g_signal_connect(G_OBJECT(app_ui_data->window),
		    "button_press_event",
		    G_CALLBACK(on_overlay_press), app_ui_data);
    g_signal_connect(GTK_WIDGET(app_ui_data->window),
		    "button_release_event",
		    G_CALLBACK(on_overlay_release), app_ui_data);


    ui_create_board(app_ui_data);
    ui_create_control(app_ui_data);

    return;
}

void
ui_create_board(AppUIData * app_ui_data)
{
    /* Load and create mahjong board */
    load_images("tiles.png");
    // set_map_selection(0);
    app_ui_data->board = create_mahjongg_board();
    gtk_container_add(GTK_CONTAINER(app_ui_data->main_hbox),
                      app_ui_data->board);

    gtk_widget_set_size_request(app_ui_data->board,
                                BOARD_WIDTH, BOARD_HEIGHT);
}

static void
size_chd(GtkWidget * widget, GtkRequisition * requisition, gpointer user_data)
{
    AppUIData *app_ui_data = (AppUIData *) user_data;
    requisition = NULL;
    widget = NULL;
    time_t seconds =
        games_clock_get_seconds(GAMES_CLOCK(app_ui_data->chrono));
    if (seconds >= 600000)
    {
        gtk_fixed_move(GTK_FIXED(app_ui_data->control),
                       app_ui_data->chrono, TIME_LEFT - 23, TIME_TOP);
    }
    else if (seconds >= 60000)
    {
        gtk_fixed_move(GTK_FIXED(app_ui_data->control),
                       app_ui_data->chrono, TIME_LEFT - 15, TIME_TOP);
    }
}

void
ui_create_control(AppUIData * app_ui_data)
{
    /* Control area */
    app_ui_data->control = gtk_fixed_new();
    gtk_container_add(GTK_CONTAINER(app_ui_data->main_hbox),
                      app_ui_data->control);

    app_ui_data->control_back =
        gtk_image_new_from_file(PIXMAPSDIR "/" CONTROL_IMAGE);
    gtk_fixed_put(GTK_FIXED(app_ui_data->control),
                  GTK_WIDGET(app_ui_data->control_back), CONTROL_LEFT,
                  CONTROL_TOP);

    /* Undo button */
    app_ui_data->undo = mahjong_button_new(mb_undo,
                                           UNDOREDO_BUTTON_WIDTH,
                                           UNDOREDO_BUTTON_HEIGHT);
    gtk_fixed_put(GTK_FIXED(app_ui_data->control),
                  GTK_WIDGET(app_ui_data->undo->widget), UNDO_LEFT, UNDO_TOP);
    mahjong_button_set_mode(app_ui_data->undo, mbm_dimmed);

    /* Redo button */
    app_ui_data->redo = mahjong_button_new(mb_redo,
                                           UNDOREDO_BUTTON_WIDTH,
                                           UNDOREDO_BUTTON_HEIGHT);
    gtk_fixed_put(GTK_FIXED(app_ui_data->control),
                  GTK_WIDGET(app_ui_data->redo->widget), REDO_LEFT, REDO_TOP);
    mahjong_button_set_mode(app_ui_data->redo, mbm_dimmed);

    /* Shuffle button */
    app_ui_data->shuffle = mahjong_button_new(mb_shuffle,
                                              SHUFFLEHINT_BUTTON_WIDTH,
                                              SHUFFLEHINT_BUTTON_HEIGHT);
    gtk_fixed_put(GTK_FIXED(app_ui_data->control),
                  GTK_WIDGET(app_ui_data->shuffle->widget), SHUFFLE_LEFT,
                  SHUFFLE_TOP);

    /* Overlay button */
    app_ui_data->overlay = mahjong_button_new(mb_return, 
		    			OVERLAY_BUTTON_WIDTH,
					OVERLAY_BUTTON_HEIGHT);
    gtk_fixed_put(GTK_FIXED(app_ui_data->control),
		    GTK_WIDGET(app_ui_data->overlay->widget), OVERLAY_LEFT,
		    OVERLAY_TOP); 
    mahjong_button_set_mode(app_ui_data->overlay, mbm_normal);

    /* Hint button */
    app_ui_data->hint = mahjong_button_new(mb_hint,
                                           SHUFFLEHINT_BUTTON_WIDTH,
                                           SHUFFLEHINT_BUTTON_HEIGHT);
    gtk_fixed_put(GTK_FIXED(app_ui_data->control),
                  GTK_WIDGET(app_ui_data->hint->widget), HINT_LEFT, HINT_TOP);

    app_ui_data->chrono = init_mahjong();
    gtk_fixed_put(GTK_FIXED(app_ui_data->control),
                  app_ui_data->chrono, TIME_LEFT, TIME_TOP);
    g_signal_connect(G_OBJECT(app_ui_data->chrono), "size-request",
                     G_CALLBACK(size_chd), (gpointer) app_ui_data);
}

GtkWindow *
ui_create_main_app(void)
{
    GtkWindow *result;

    /* Create HildonApp */
    result = GTK_WINDOW(gtk_window_new(GTK_WINDOW_TOPLEVEL));

    g_signal_connect(G_OBJECT(result), "delete_event",
                     G_CALLBACK(delete_event_callback), GINT_TO_POINTER(QUIT_GAME));
    return result;
}

void
ui_clean_up(AppUIData * app_ui_data)
{
    app_ui_data = app_ui_data;
}
