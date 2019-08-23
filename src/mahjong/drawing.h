/* drawing.h : Drawing routines (what else would it be) Copyright (C) 2003
 * by Callum McKenzie Created: <2003-09-07 10:40:24 callum> Time-stamp:
 * <2003-10-03 08:47:27 callum> */

#ifndef DRAWING_H
#define DRAWING_H

#include <gtk/gtk.h>
#include <gdk/gdk.h>

/**
 Creates a widget for the board.
 @return The widget created.
*/
GtkWidget *create_mahjongg_board(void);

/**
 Loads new tileset.
 @param File name of the tileset.
 @return TRUE always.
*/
gboolean load_images(gchar * file);

/**
 Sets the background of the game board.
 @param colour Colour specification.
*/
void set_background(gchar * colour);

/**
 Draws tile.
 @param Number of tile to be drawn.
*/
void draw_tile(gint tileno);

/**
 Draws all tiles.
*/
void draw_all_tiles(void);

/**
 Calculates geometry for the view.
*/
void calculate_view_geometry(void);

/**
 Configures pixmaps.
*/
void configure_pixmaps(void);

extern GdkColor bgcolour;

#endif

/* EOF */
