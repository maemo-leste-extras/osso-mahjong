/**
    @file dbus.h

    DBUS functionality, definitions.

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

#ifndef DBUS1_H
#define DBUS1_H

#ifdef HAVE_CONFIG_H
#include "../config.h"
#endif /* HAVE_CONFIG_H */

#include "appdata.h"

#include <libosso.h>
#include <glib.h>
#include <gtk/gtk.h>
#include <osso-log.h>
#include <log-functions.h>

/* Sketch definitions */
#define OSSO_MAHJONG_SERVICE "com.nokia.osso_mahjong"
#define OSSO_MAHJONG_INTERFACE "com.nokia.osso_mahjong"
#define OSSO_MAHJONG_OBJECT_PATH "/com/nokia/osso_mahjong"

typedef enum {
    gt_none = 0,
    gt_chess,
    gt_mahjong,
    gt_lmarbles
} GameType;


/* Define d-bus messages, to which application have to react */
/* For instance game application needs methods to start, continue, restart,
 * close and pause the game */

/* Testing d-bus messaging with displaying infoprint */
// #define OSSO_SKETCH_DISPLAY_INFOPRINT "osso_sketch_display_infoprint"

gboolean set_dbus_handler(osso_rpc_cb_f * func);

/**
 Send DBUS message
 
 @param service Service to be called.
 @param object_path Path to object being called.
 @param iface Interface to be called.
 @param method Method to be called.
 @param args A GArray of osso_rpc_t structures.
 @param retval The return value of the method.
 @param app_data Application specific data
 @return #OSSO_OK if the message was sent. #OSSO_INVALID if a parameter
 is invalid. If the remote method returns an error, or does not return
 anything, then #OSSO_ERROR is returned, and retval is set to an error
 message. #OSSO_ERROR is also returned if any other kind of error
 occures, like IO error.
 */

osso_return_t send_dbus_message(const gchar * service,
                                const gchar * object_path,
                                const gchar * iface,
                                const gchar * method,
                                GArray * args,
                                osso_rpc_t * retval, AppData * app_data);


/** 
 Receive D-BUS messages and handles them

 @param interface The interface of the called method.
 @param method The method that was called.
 @param arguments A GArray of osso_rpc_t_structures.
 @param app_ui_data An application specific pointer.
 @param retval The return value of the method.
 @return gint value
 */

gint dbus_req_handler(const gchar * interface,
                      const gchar * method,
                      GArray * arguments,
                      gpointer app_ui_data, osso_rpc_t * retval);


/** Handles incoming D-BUS message

 @param method The method that was called.
 @param arguments A GArray of osso_rpc_t structures.
 @param app_ui_data An application specific pointer.
 @param retval The return value of the method.
 @return gint value
 */

gint dbus_message_handler(const gchar * method,
                          GArray * arguments,
                          gpointer app_ui_data, osso_rpc_t * retval);


/**
 Set hardware event handler

 @param state Signals that are of interest
 @param cb Callback function.
 @param data Application specific data.
 */

gboolean set_hw_event_handler(osso_hw_state_t * state,
                              osso_hw_cb_f * cb, gpointer data);


/** 
 Handles hardware events.

 @todo Just a skeleton
 @param state State occured.
 @param data Application specific data.
 */

void hw_event_handler(osso_hw_state_t * state, gpointer data);

/* Rama - Added to handle Display status */
void display_event_handler(osso_display_state_t state, gpointer data);

/**
 Osso topping callback
 
 @param arguments Extra parameters
 @param app_ui_data Application specific data
 @return Always NULL 
 */

osso_application_top_cb_f *osso_top_callback(const gchar * arguments,
                                             AppUIData * app_ui_data);

/**
 Initialize osso

 @param app_data Application specific data
 @return TRUE on success, FALSE otherwise
 */

gboolean init_osso(AppData * app_data);

/**
 Deinitialize osso

 @param app_data Application specific data
 @return TRUE on success, FALSE otherwise
 */
gboolean deinit_osso(AppData * app_data);

/**
 Get pointer to AppData structure

 @return pointer to AppData structure
 */
AppData *get_app_data(void);

#endif
