/**
    @file mahjong_button.c

    Implementation of the MahjongButton component.

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
#include "mahjong_button.h"
#include "mahjong/mahjongg.h"

/* Cairo */
#include <cairo/cairo.h>

#define BUTTONS_UNDO_REDO "undo_redo_buttons.jpg"
#define BUTTONS_UNDO_REDO_DIMMED "undo_redo_buttons_dimmed.jpg"
#define BUTTONS_UNDO_REDO_PRESSED "undo_redo_buttons_pressed.jpg"

#define BUTTONS_SHUFFLE_HINT "shuffle_hint_buttons.jpg"
#define BUTTONS_SHUFFLE_HINT_DIMMED "shuffle_hint_buttons_dimmed.jpg"
#define BUTTONS_SHUFFLE_HINT_PRESSED "shuffle_hint_buttons_pressed.jpg"

#define BUTTON_OVERLAY	"general_overlay_back"

struct MahjongButton *
mahjong_button_new(enum Mahjong_Button_Style style, gint width, gint height)
{
    struct MahjongButton *tmp = g_new(struct MahjongButton, 1);
    GError *error = NULL;
    if (tmp != NULL)
    {
        /* Loading images */
        switch (style)
        {
            case mb_undo:
            case mb_redo:
                tmp->normal =
                    GTK_IMAGE(gtk_image_new_from_file
                              (PIXMAPSDIR "/" BUTTONS_UNDO_REDO));
                tmp->dimmed =
                    GTK_IMAGE(gtk_image_new_from_file
                              (PIXMAPSDIR "/" BUTTONS_UNDO_REDO_DIMMED));
                tmp->pressed =
                    GTK_IMAGE(gtk_image_new_from_file
                              (PIXMAPSDIR "/" BUTTONS_UNDO_REDO_PRESSED));
                break;

	    case mb_return:
		tmp->overlay = gtk_icon_theme_load_icon(
					gtk_icon_theme_get_default(), 
					BUTTON_OVERLAY, 64, 
					GTK_ICON_LOOKUP_NO_SVG, &error);
		break;

            case mb_shuffle:
            case mb_hint:
            default:
                tmp->normal =
                    GTK_IMAGE(gtk_image_new_from_file
                              (PIXMAPSDIR "/" BUTTONS_SHUFFLE_HINT));
                tmp->dimmed =
                    GTK_IMAGE(gtk_image_new_from_file
                              (PIXMAPSDIR "/" BUTTONS_SHUFFLE_HINT_DIMMED));
                tmp->pressed =
                    GTK_IMAGE(gtk_image_new_from_file
                              (PIXMAPSDIR "/" BUTTONS_SHUFFLE_HINT_PRESSED));
                break;
        }
        tmp->style = style;

        tmp->width = width;
        tmp->height = height;

        /* Setting mode */
        mahjong_button_set_mode(tmp, mbm_normal);
        tmp->image = tmp->normal;

        tmp->widget = GTK_DRAWING_AREA(gtk_drawing_area_new());
        gtk_widget_set_size_request(GTK_WIDGET(tmp->widget),
                                    tmp->width, tmp->height);

        /* Connect signal to draw the widget */
        g_signal_connect(GTK_WIDGET(tmp->widget),
                         "expose-event",
                         GTK_SIGNAL_FUNC(mahjong_button_draw), tmp);

        /* Set signal mask */
        gtk_widget_add_events(GTK_WIDGET(tmp->widget),
                              GDK_POINTER_MOTION_MASK |
                              GDK_BUTTON_MOTION_MASK |
                              GDK_BUTTON_PRESS_MASK |
                              GDK_BUTTON_RELEASE_MASK);


        /* Connect mouse handling signals */
        g_signal_connect(GTK_WIDGET(tmp->widget),
                         "button-press-event",
                         GTK_SIGNAL_FUNC(mahjong_button_mouse_press), tmp);
        g_signal_connect(GTK_WIDGET(tmp->widget),
                         "button-release-event",
                         GTK_SIGNAL_FUNC(mahjong_button_mouse_press), tmp);
        g_signal_connect(GTK_WIDGET(tmp->widget),
                         "motion-notify-event",
                         GTK_SIGNAL_FUNC(mahjong_button_mouse_press), tmp);
    }

    return tmp;
}

void
mahjong_button_set_mode(struct MahjongButton *button,
                        enum Mahjong_Button_Mode mode)
{
    if (button == NULL)
        return;
    g_assert(button);

    button->mode = mode;

    /* Select mode and set proper image */
    switch (mode)
    {
        case mbm_normal:
            button->image = button->normal;
            break;
        case mbm_pressed:
            button->image = button->pressed;
            break;
        case mbm_dimmed:
            button->image = button->dimmed;
            break;
    }
}

gboolean
mahjong_button_draw(GtkWidget * widget, GdkEventExpose * event, gpointer data)
{
    struct MahjongButton *tmp = NULL;
    GdkPixbuf *buf = NULL;
    gint pos = 0;
    gint ovr = 0;
	GdkColormap *colormap;
	GdkPixmap *pixmap = NULL;
	GdkBitmap *mask = NULL;

    event = event;
    widget = widget;
    tmp = (struct MahjongButton *) data;
   
    /* Get pixbuf only for other images */
    if(tmp->style != mb_return)
    	buf = gtk_image_get_pixbuf(tmp->image);

    switch (tmp->style)
    {
        default:
        case mb_undo:
        case mb_shuffle:
            pos = 0;
            break;
        case mb_redo:
        case mb_hint:
            pos = 1;
            break;
	case mb_return:
		ovr = 1;
		break;
    }

    if(ovr == 1)
    {
		colormap = gdk_screen_get_rgba_colormap(gtk_widget_get_screen(GTK_WIDGET(tmp->widget)));
//		gtk_widget_set_colormap(GTK_WIDGET(tmp->widget), colormap);
		g_assert(colormap);
		gdk_drawable_set_colormap(GTK_WIDGET(tmp->widget)->window, colormap);
		gdk_pixbuf_render_pixmap_and_mask_for_colormap(
				tmp->overlay, colormap, &pixmap,
				&mask, 127);
		g_assert(pixmap);		
		if(pixmap)
		{
			gdk_window_set_back_pixmap(GTK_WIDGET(tmp->widget)->window,
						pixmap, FALSE);
			g_object_unref(pixmap);
		}
				
		if(mask)
		{
			gdk_window_shape_combine_mask(GTK_WIDGET(tmp->widget)->window,
					mask, 0, 0);
			g_object_unref(mask);
		}

		gdk_draw_pixbuf(GTK_WIDGET(tmp->widget)->window,
				GTK_WIDGET(tmp->widget)->style->fg_gc[GTK_STATE_NORMAL], 
				GDK_PIXBUF(tmp->overlay),
				pos * tmp->width, 0,
				0, 0, tmp->width, tmp->height, GDK_RGB_DITHER_NONE, 0, 0);
		ovr = 0;
		return TRUE;
    }
    
    gdk_draw_pixbuf(GTK_WIDGET(tmp->widget)->window,
                    GTK_WIDGET(tmp->widget)->style->fg_gc[GTK_STATE_NORMAL],
                    buf,
                    pos * tmp->width, 0,
                    0, 0, tmp->width, tmp->height, GDK_RGB_DITHER_NONE, 0, 0);
    return TRUE;
}

gboolean
mahjong_button_mouse_press(GtkWidget * widget,
                           GdkEventButton * event, gpointer data)
{
    gboolean pressed = FALSE;
    widget = widget;

    if (event == NULL)
        return FALSE;
    g_assert(event);

    if (event->type == GDK_BUTTON_PRESS)
        pressed = TRUE;
    else if (event->type == GDK_BUTTON_RELEASE)
        pressed = FALSE;

    mahjong_button_mouse((struct MahjongButton *) data, pressed);

    return TRUE;
}

void
mahjong_button_act(struct MahjongButton *button)
{
    switch (button->style)
    {
        case mb_undo:
            undo_tile_callback(GTK_WIDGET(button->widget), NULL);
            break;
        case mb_redo:
            redo_tile_callback(GTK_WIDGET(button->widget), NULL);
            break;
        case mb_hint:
            hint_callback(GTK_WIDGET(button->widget), NULL);
            break;
        case mb_shuffle:
            shuffle_tiles_callback(GTK_WIDGET(button->widget), NULL);
            break;
	case mb_return:
	    mahjong_pause(GTK_WIDGET(button->widget), get_app_data()->app_ui_data);
	    break;
    }
}

void
update_undo_redo(gboolean undo, gboolean redo)
{
    AppData *app_data = get_app_data();
    if (app_data == NULL)
        return;
    g_assert(app_data);

    if (redo)
    {
        mahjong_button_set_mode(app_data->app_ui_data->redo, mbm_normal);
    }
    else
    {
        mahjong_button_set_mode(app_data->app_ui_data->redo, mbm_dimmed);
    }

    if (undo)
    {
        mahjong_button_set_mode(app_data->app_ui_data->undo, mbm_normal);
    }
    else
    {
        mahjong_button_set_mode(app_data->app_ui_data->undo, mbm_dimmed);
    }

    gtk_widget_queue_draw(GTK_WIDGET(app_data->app_ui_data->undo->widget));
    gtk_widget_queue_draw(GTK_WIDGET(app_data->app_ui_data->redo->widget));
}

void
update_hint(gboolean state)
{
    AppData *app_data = get_app_data();
    if (app_data == NULL)
        return;
    g_assert(app_data);

    if (state)
    {
        mahjong_button_set_mode(app_data->app_ui_data->hint, mbm_normal);
    }
    else
    {
        mahjong_button_set_mode(app_data->app_ui_data->hint, mbm_dimmed);
    }

    gtk_widget_queue_draw(GTK_WIDGET(app_data->app_ui_data->hint->widget));
}

void
mahjong_button_mouse(struct MahjongButton *button, gboolean pressed)
{
    gboolean update = FALSE;
    if (button->mode == mbm_dimmed)
        return;

    if (pressed && button->mode != mbm_pressed)
    {
        mahjong_button_set_mode(button, mbm_pressed);
        update = TRUE;
    }
    else if (button->mode != mbm_normal)
    {
        mahjong_button_set_mode(button, mbm_normal);
        mahjong_button_act(button);
        update = TRUE;
    }

    if (update)
    {
        // gtk_widget_queue_draw_area(, 0, 0, size.x, size.y);
	if(button->style != mb_return)
        	gtk_widget_queue_draw(GTK_WIDGET(button->widget));
    }
}
