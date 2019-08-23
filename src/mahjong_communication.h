/**
    @file mahjong_communication.h

    Prototypes for Communication

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

#ifndef MAHJONG_COMMUNICATION_H
#define MAHJONG_COMMUNICATION_H

#include <glib.h>
#include <libosso.h>
#include <osso-log.h>
#include <log-functions.h>

#include "dbus.h"
#include "settings.h"
#include "mahjong_core.h"
#include "mahjong/mahjongg.h"

#define STARTUP_SERVICE "com.nokia.osso_mahjong.startup"
#define STARTUP_IFACE   "com.nokia.osso_mahjong.startup"
#define STARTUP_OBJECT_PATH "/com/nokia/osso_mahjong/startup"

#define GAME_RUN_METHOD "game_run"
#define GAME_CONTINUE_METHOD "game_continue"
#define GAME_RESTART_METHOD "game_restart"
#define GAME_CLOSE_METHOD "game_close"
#define GAME_PAUSE_METHOD "game_pause"
#define GAME_HOME_METHOD "game_home"
#define GAME_END_METHOD "game_end"
#define GAME_SAVE_METHOD "game_save"
#define GAME_LOAD_METHOD "game_load"

/**
 Handles incoming D-BUS message

 @param method The method that was called.
 @param arguments A GArray of osso_rpc_t structures. 
 @param data An application specific pointer.
 @param retval The return value of the method.
 @returns osso_rpc_type_t value
 */
gint handle_dbus_message(const gchar * method, GArray * arguments,
                         gpointer data, osso_rpc_t * retval);

/**
 Send game state message to Start-up scren via D-BUS
 @param state State message to be sent.
 @return TRUE on success, FALSE on failure.
 */
gboolean set_game_state(gchar * state);

#endif
