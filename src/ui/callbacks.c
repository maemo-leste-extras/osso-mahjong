/**
    @file callbacks.c

    Provides callbacks for the user interface.

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

/* Function prototypes */
#include "callbacks.h"
#include <gdk/gdk.h>
#include <gdk/gdkkeysyms.h>
#include <X11/Xlib.h>
#include <gdk/gdkx.h>
#include <string.h>

/* Definitions of UI identificators and AppData */
#include "interface.h"

#define KEY_ESCAPE_TIMEOUT 1500
guint escape_timeout = 0;

gboolean is_key_press = FALSE;
gboolean is_overlay_press = FALSE;

gboolean
focus_is_own()
{
    Atom wm_class = XInternAtom(GDK_DISPLAY(), "WM_CLASS", True);

    Window focus_window;
    long BUFSIZE = 2048;
    int ret;
    Atom type_ret;
    int format_ret;
    unsigned long nitems_ret;
    unsigned long unused;
    unsigned char* data_ret = NULL;

    XGetInputFocus(GDK_DISPLAY(), &focus_window, &ret);
    if(PointerRoot == focus_window)
    {
        /* This value is returned, when clicking on titlebar of window, and
         * dialog becomes transparent, focus is in game main view */
        return TRUE;
    }

    if(None == focus_window)
    {
        return FALSE;
    }

    /* Getting WM_CLASS property of the window, who stole the focus */
    if (XGetWindowProperty(GDK_DISPLAY(), focus_window, wm_class, 
                           0L, BUFSIZE, False, AnyPropertyType,
                           &type_ret, &format_ret, &nitems_ret,
                           &unused, &data_ret ) == Success)
    {
	    if(!data_ret) {
		    AppData *app_data = get_app_data();
		    if(app_data->app_ui_data->dialog) {
			    return(TRUE);
		    }
		    return FALSE;
	    }
        return ( (0 == strcmp((char *)data_ret, "Mahjong")) );
    }

    return FALSE;
}


gboolean
main_win_focus_out(GtkWidget * widget,
                   GdkEventFocus * event, gpointer user_data)
{
    (void) widget;
    (void) event;

    if (!focus_is_own())
    {
        mahjong_pause(NULL, user_data);
        return TRUE;
    }

    return FALSE;
}

gboolean
end_dialog_focus_out(GtkWidget * widget,
                     GdkEventFocus * event, gpointer user_data)
{
    (void) widget;
    (void) event;

    //AppData *app_data = get_app_data();

    if (!focus_is_own())
    {
	    /* Rama - NB# 98882. Do not destroy widget, instead emit OK
	     * signal from End Dialog */
            //gtk_widget_destroy(widget);
            //app_data->app_ui_data->dialog = NULL;
            mahjong_pause(NULL, user_data);
            return TRUE;
    }

    return FALSE;
}


gboolean
visibility_notify_event(GtkWidget * widget, GdkEventVisibility * event,
                        AppUIData * app_ui_data)
{
    switch (event->state)
    {
        case GDK_VISIBILITY_FULLY_OBSCURED:
            mahjong_pause(widget, app_ui_data);
            break;
        case GDK_VISIBILITY_UNOBSCURED:
        case GDK_VISIBILITY_PARTIAL:
            break;
    }

    return FALSE;
}

gboolean
key_press(GtkWidget * widget, GdkEventKey * event, AppUIData * app_ui_data)
{
    (void) widget;

    if (event->state & (GDK_CONTROL_MASK |
                        GDK_SHIFT_MASK |
                        GDK_MOD1_MASK |
                        GDK_MOD3_MASK | GDK_MOD4_MASK | GDK_MOD5_MASK))
    {
        return TRUE;
    }

    is_key_press = TRUE;

    if (event->keyval == GDK_Escape && escape_timeout == 0)
    {
        escape_timeout = g_timeout_add(KEY_ESCAPE_TIMEOUT,
                                       app_escape_timeout,
                                       (gpointer) app_ui_data);
    }

    if ((event->keyval == GDK_F4) || (event->keyval == GDK_F6)
          ||(event->keyval == GDK_F5))
    {
        mahjong_pause(widget, app_ui_data);
        is_key_press = FALSE;

        return TRUE;

    }
    return FALSE;
}

gboolean
key_release(GtkWidget * widget, GdkEventKey * event, AppUIData * app_ui_data)
{
    if (event->state & (GDK_CONTROL_MASK |
                        GDK_SHIFT_MASK |
                        GDK_MOD1_MASK |
                        GDK_MOD3_MASK | GDK_MOD4_MASK | GDK_MOD5_MASK))
    {
        return TRUE;
    }

    switch (event->keyval)
    {
        case GDK_Escape:
            app_remove_timeout();
            if (is_key_press)
                mahjong_pause(widget, app_ui_data);
            is_key_press = FALSE;

            return TRUE;
        case GDK_KP_Enter:
        case GDK_Return:
        case GDK_minus:
        case GDK_plus:
        case GDK_KP_Add:
        case GDK_KP_Subtract:
        default:
            break;
    }

    is_key_press = FALSE;


    return FALSE;
}

gboolean
app_escape_timeout(gpointer data)
{
    AppUIData *app_ui_data = (AppUIData *) data;

    if (!app_ui_data)
        return TRUE;

    settings_set_bool(SETTINGS_ESCAPE_TIMEOUT, TRUE);

    escape_timeout = 0;
    mahjong_pause(NULL, app_ui_data);

    return FALSE;
}

void
app_remove_timeout(void)
{
    if (escape_timeout > 0)
    {
        g_source_remove(escape_timeout);
        escape_timeout = 0;
    }
}
gboolean on_overlay_press(GtkWidget *widget, GdkEventButton *event, gpointer *data)
{
	GtkRequisition req;
	AppUIData *app_ui_data = (AppUIData *)data;

	(void)widget;
	gtk_widget_size_request(app_ui_data->control_back, &req);
	if(((gint)event->x > (BOARD_WIDTH + req.width - OVERLAY_BUTTON_WIDTH)) &&
	  		((gint)event->y < OVERLAY_BUTTON_HEIGHT)) {
		is_overlay_press = TRUE;
	}
	return TRUE;
}

gboolean on_overlay_release(GtkWidget *widget, GdkEventButton *event, gpointer *data)
{
	GtkRequisition req;
	AppUIData *app_ui_data = (AppUIData *)data;

	gtk_widget_size_request(app_ui_data->control_back, &req);

	if(((gint)event->x > (BOARD_WIDTH + req.width - OVERLAY_BUTTON_WIDTH)) &&
	  		((gint)event->y < OVERLAY_BUTTON_HEIGHT) && is_overlay_press) 
	{
		mahjong_pause(widget, (AppUIData *)data);
		is_overlay_press = FALSE;
	}
	return TRUE;
}

