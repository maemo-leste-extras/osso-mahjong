/**
    @file dbus.c

    DBUS functionality.

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

#include <gtk/gtk.h>
#include <libosso.h>

#include "appdata.h"
#include "dbus.h"
#include "mahjong_communication.h"

AppData *_app_data = NULL;

/* Define topping */

osso_application_top_cb_f *
osso_top_callback(const gchar * arguments, AppUIData * app_ui_data)
{
    g_assert(app_ui_data);
    (void) arguments;
    if (app_ui_data)
        gtk_window_present(GTK_WINDOW(app_ui_data->window));
    return NULL;
}

gint
dbus_req_handler(const gchar * interface,
                 const gchar * method,
                 GArray * arguments, gpointer data, osso_rpc_t * retval)
{
    (void) interface;
    return dbus_message_handler(method, arguments, data, retval);
}

gint
dbus_message_handler(const gchar * method,
                     GArray * arguments, gpointer data, osso_rpc_t * retval)
{
    return handle_dbus_message(method, arguments, data, retval);
}

/* Send d-bus message */
osso_return_t
send_dbus_message(const gchar * service,
                  const gchar * object_path,
                  const gchar * iface,
                  const gchar * method,
                  GArray * args, osso_rpc_t * retval, AppData * app_data)
{
    (void) args;
    return osso_rpc_run(app_data->app_osso_data->osso,
                        service,
                        object_path,
                        iface,
                        method,
                        retval,
                        DBUS_TYPE_INT32, gt_mahjong, DBUS_TYPE_INVALID);
}

/* Depending on the state of hw, do something */
void
hw_event_handler(osso_hw_state_t * state, gpointer data)
{
    AppData *app_data = (AppData *) data;
    if (!state)
        return;


    if (state->shutdown_ind)
    {
        /* Rebooting */
        mahjong_close();
    }
    if (state->memory_low_ind)
    {
        /* Memory low */
        mahjong_close();
    }
    if (state->save_unsaved_data_ind)
    {
        /* Unsaved data should be saved */
        if (app_data)
        {
            mahjong_pause(NULL, app_data->app_ui_data);
        }
    }
    if (state->system_inactivity_ind)
    {
		/* Rama - NB#96276. call puase handler when 
		 * backscreen goes to sleep */
        /* Minimum activity */
        if (app_data)
        {
            mahjong_pause(NULL, app_data->app_ui_data);
        }
    }
}

void display_event_handler(osso_display_state_t state, gpointer data)
{
	AppData *app_data = NULL;
	if(!data)
		return;

	app_data = (AppData *)data;

	if(OSSO_DISPLAY_ON == state) {
		/* Do Nothing */
	}
	else if( (OSSO_DISPLAY_OFF == state) || (OSSO_DISPLAY_DIMMED == state) ) {
		if (data) {
			mahjong_pause(NULL, app_data->app_ui_data);
		}
	}

	return;
}

/* Do initialization for OSSO, create osso context, set topping callback,
 * dbus-message handling callbaks, and hw-event callbacks. TODO: System bus
 * still not seem working well, so HW-event callbacks no tested */

gboolean
init_osso(AppData * app_data)
{

    osso_return_t ret;

    if (!app_data || !app_data->app_osso_data)
        return FALSE;

    _app_data = app_data;

    /* Init osso */
    osso_log(LOG_INFO, "Initializing osso");
    app_data->app_osso_data->osso = osso_initialize(PACKAGE_NAME,
                                                    PACKAGE_VERSION,
                                                    TRUE, NULL);

    if (app_data->app_osso_data->osso == NULL)
    {
        osso_log(LOG_ERR, "Osso initialization failed");
        return FALSE;
    }

    /* Set topping callback */
    osso_application_set_top_cb(app_data->app_osso_data->osso,
                                (osso_application_top_cb_f *)
                                osso_top_callback, (gpointer) (app_data));
    /* Set handling d-bus messages from session bus */
    ret = osso_rpc_set_cb_f(app_data->app_osso_data->osso,
                            OSSO_MAHJONG_SERVICE,
                            OSSO_MAHJONG_OBJECT_PATH,
                            OSSO_MAHJONG_INTERFACE,
                            dbus_req_handler, app_data);

    if (ret != OSSO_OK)
    {
        osso_log(LOG_ERR, "Could not set callback for receiving messages");
    }

    /* Set handling changes in HW states. Note: not tested */
    ret = osso_hw_set_event_cb(app_data->app_osso_data->osso,
                               NULL, hw_event_handler, app_data);

    if (ret != OSSO_OK)
    {
        osso_log(LOG_ERR, "Could not set callback for HW monitoring");
    }

    /* Rama - Bug#104224 Set Callback for handling Display Chnage Events */
    ret = osso_hw_set_display_event_cb(app_data->app_osso_data->osso,
		    display_event_handler,
		    app_data);

    if (ret != OSSO_OK)
    {
	    osso_log(LOG_ERR, "Could not set callback for Display monitoring");
    }


    return TRUE;
}

/* Deinitialize osso specific data TODO: Check of return values from osso */
gboolean
deinit_osso(AppData * app_data)
{
    if (!app_data || !app_data->app_osso_data)
        return FALSE;
    _app_data = NULL;

    /* Unset callbacks */
    osso_application_unset_top_cb(app_data->app_osso_data->osso,
                                  (osso_application_top_cb_f *)
                                  osso_top_callback, (gpointer) (NULL));

    osso_rpc_unset_cb_f(app_data->app_osso_data->osso,
                        OSSO_MAHJONG_SERVICE,
                        OSSO_MAHJONG_OBJECT_PATH,
                        OSSO_MAHJONG_INTERFACE, dbus_req_handler, app_data);


    osso_hw_unset_event_cb(app_data->app_osso_data->osso, NULL);

    /* Deinit osso */
    osso_deinitialize(app_data->app_osso_data->osso);

    return TRUE;
}

AppData *
get_app_data(void)
{
    return _app_data;
}
