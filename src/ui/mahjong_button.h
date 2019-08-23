/**
    @file mahjong_button.h

    Function prototypes and variable definitions MahjongButton component.

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

#ifndef MAHJONG_BUTTON_H
#define MAHJONG_BUTTON_H

/* GTK */
#include <gtk/gtk.h>
#include <gdk/gdk.h>

enum Mahjong_Button_Style { mb_undo = 0, mb_redo, mb_hint, mb_shuffle, mb_return };
enum Mahjong_Button_Mode { mbm_normal = 0, mbm_pressed, mbm_dimmed };

struct MahjongButton {
    GtkDrawingArea *widget;
    GtkImage *image;
    GtkImage *normal;
    GtkImage *pressed;
    GtkImage *dimmed;
    GdkPixbuf *overlay;
    enum Mahjong_Button_Style style;
    enum Mahjong_Button_Mode mode;
    gint width, height;
};

/**
 Create new button, load images.

 @param style Button style
 @param width Button width
 @param height Button height
 @returns Pointer to MahjongButton structure
 */
struct MahjongButton *mahjong_button_new(enum Mahjong_Button_Style style,
                                         gint width, gint height);

/**
 Set button mode.

 @param button Pointer to MahjongButton structure
 @param mode Button mode (normal, pressed, dimmed)
 */
void mahjong_button_set_mode(struct MahjongButton *button,
                             enum Mahjong_Button_Mode mode);

/**
 Draw button
 @param widget Widget that should be drawn.
 @param event Event causing the draw request.
 @param data Data assosiated with the callback.
 @return Always TRUE
 */
gboolean mahjong_button_draw(GtkWidget * widget,
                             GdkEventExpose * event, gpointer data);

/**
 Handle mouse press event
 @param widget Widget that was pressed.
 @param event Event causing the callback.
 @param data Data assosiated with the callback.
 @return FALSE if event is NULL, TRUE otherwise.
 */
gboolean mahjong_button_mouse_press(GtkWidget * widget,
                                    GdkEventButton * event, gpointer data);

/**
 Handle calling proper method after button press.

 @param button Pointer to MahjongButton structure
 */
void mahjong_button_act(struct MahjongButton *button);

/**
 Handle mouse actions
 @param button Button that's being handled.
 @param pressed Is the event press or release event.
 */
void mahjong_button_mouse(struct MahjongButton *button, gboolean pressed);

/**
 Update undo and redo states
 @param undo Undo available
 @param redo Redo available
 */
void update_undo_redo(gboolean undo, gboolean redo);

/**
 Update hint button state
 @param state Is hint available
 */
void update_hint(gboolean state);

#endif
