/* 
 * Gnome-Mahjonggg main header
 * (C) 1998-1999 the Free Software Foundation
 *
 *
 * Author: Francisco Bustamante et al.
 *
 *
 * http://www.nuclecu.unam.mx/~pancho/
 * pancho@nuclecu.unam.mx
 *
 */

#ifndef MAHJONGG_H
#define MAHJONGG_H

/* #include <gnome.h> */
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <gdk/gdkkeysyms.h>

#include "mahjong_core.h"

#define MAX_TILES 144
#define MAX_TILES_STR "144"

#define PENALTY_HINT 30
#define PENALTY_SHUFFLE 60

typedef enum {
    NEW_GAME,
    NEW_GAME_WITH_SEED,
    RESTART_GAME,
    QUIT_GAME
} game_state;

struct _tilepos {
    int x;
    int y;
    int layer;
};
typedef struct _tilepos tilepos;

extern tilepos *pos;


typedef struct _tile tile;

struct _tile {
    int type;
    int image;
    int visible;
    int selected;
    int sequence;
    int number;
};


gboolean dialog_key_press(GtkWidget * widget, GdkEventKey * event,
                          gpointer data);

/**
 Handler for the pressing of undo button.
 Undoes the last action, if available.
 @param widget The button for which the callback was called.
 @param data Data assosiated with the callback.
*/
void undo_tile_callback(GtkWidget * widget, gpointer data);
/**
 Handler for the pressing of redo button.
 Redoes the last action undone, if available.
 @param widget The button for which the callback was called.
 @param data Data assosiated with the callback.
*/
void redo_tile_callback(GtkWidget * widget, gpointer data);
/**
 Handler for the pressing of hide button
 Hints the player if available tile pairs are available and adds 30s penalty.
 @param widget The button for which the callback was called.
 @param data Data assosiated with the callback.
*/
void hint_callback(GtkWidget * widget, gpointer data);
/**
 Handler for the pressing of shuffle button.
 Shuffles the tiles to a new order in which the game can be completed.
 Adds 60 seconds to the game clock.
 @param widget The button for which the callback was called.
 @param data Data assosiated with the callback.
*/
void shuffle_tiles_callback(GtkWidget * widget, gpointer data);

/**
 Starts a new game.
 @param with_seed Should a new seed be used.
*/
void new_game(gboolean with_seed);

/**
 Handles an event on given tile.
 Selects/unselects tiles or removes them, if applicable.
 @param tileno Tile for which the event occurs.
 @param button Which button was pressed.
*/
void tile_event(gint tileno, gint button);

extern tile tiles[MAX_TILES];
extern gint paused;
extern gchar *tileset;
extern gchar *mapset;
extern gint moves_left;

/**
 Sets the name of selected map.
 @param mapno The order number of the selected map.
*/
void set_map_selection(gint mapno);
/**
 Initializes a game of mahjongg.
 @return The clock widget for the game.
*/
GtkWidget *init_mahjong(void);

/**
 Deinitializes a game of mahjongg.
*/
void deinit_mahjong(void);

/**
 Initializes the map as specified by the mapset variable.

 @return none
*/
void load_map(void);

/**
 Counts doable moves.
 @return none
*/
void check_free(void);

gboolean delete_event_callback(GtkWidget * widget, GdkEvent * any,
                               gpointer data);
#endif
