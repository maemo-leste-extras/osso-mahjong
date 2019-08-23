/**
    @file interface.h

    Function prototypes and variable definitions for general user
    interface functionality.

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

#ifndef INTERFACE_H
#define INTERFACE_H

/* GTK */
#include <gtk/gtk.h>

/* Log */
#include <log-functions.h>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

/* Application data structures */
#include "appdata.h"

/* Localization data */
#include "mahjong_i18n.h"

#include "mahjong_core.h"

/* Include callback prototypes */
#include "callbacks.h"

#include "mahjong_button.h"

#include "mahjong/mahjongg.h"
#include "mahjong/drawing.h"

/* Definations */

#define BOARD_LEFT 10
#define BOARD_TOP 10
#define BOARD_WIDTH 610
#define BOARD_HEIGHT 470

#define BACKGROUND_IMAGE "Background_board.jpg"
#define CONTROL_IMAGE "Background_controls.jpg"
#define BACKGROUND_LEFT 0
#define BACKGROUND_TOP 0
#define CONTROL_LEFT 0
#define CONTROL_TOP 0

#define SHUFFLEHINT_BUTTON_WIDTH 75
#define SHUFFLEHINT_BUTTON_HEIGHT 70

#define UNDOREDO_BUTTON_WIDTH 70
#define UNDOREDO_BUTTON_HEIGHT 70

#define OVERLAY_BUTTON_WIDTH 120
#define OVERLAY_BUTTON_HEIGHT 60

#define OVERLAY_LEFT 80
#define OVERLAY_TOP  0

#define SHUFFLE_LEFT 59
#define SHUFFLE_TOP 137

#define HINT_LEFT 59
#define HINT_TOP 225

#define UNDO_LEFT 18
#define UNDO_TOP 377

#define REDO_LEFT 106
#define REDO_TOP 377

#define TIME_LEFT 58
#define TIME_TOP 42

/**
   Show or hide the main window.

   @param app_ui_data Pointer to application UI data structure
   @param show TRUE to show, FALSE to hide
*/
void ui_view_main_window(AppUIData * app_ui_data, gboolean show);

/**
   create the main "window". initializes the application ui.

   @param app_data pointer to application data structure
*/
void ui_create_main_window(AppData * app_data);

/**
   Create board area.

   @param app_data pointer to application data structure
*/
void ui_create_board(AppUIData * app_ui_data);

/**
   Create control area and controls.

   @param app_data pointer to application data structure
*/
void ui_create_control(AppUIData * app_ui_data);

/**
   Create the main Window.

   @return Pointer to GtkWindow
*/
GtkWindow *ui_create_main_app(void);

/**
   Clean up UI, used before exiting program.

   @param app_ui_data AppData pointer to application data structure
*/
void ui_clean_up(AppUIData * app_ui_data);
#endif
