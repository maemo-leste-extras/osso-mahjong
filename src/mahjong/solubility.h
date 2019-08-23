/* 
 * Gnome-Mahjonggg-solubility header file
 * (C) 1998-2002 the Free Software Foundation
 *
 *
 * Author: Michael Meeks.
 *
 * http://www.gnome.org/~michael
 * michael@ximian.com
 */

/**
 Checks whether the tile is available.
 @param index Index of the tile to be checked.
 @return 1 if the tile is available, 0 if not.
*/
extern int tile_free(int index);

/**
 Generates a new game by shuffling the tiles to place.
 @param seed Seed used for generating the game.
*/
extern void generate_game(guint seed);

/**
 Free things reserved by generate_game
*/
void degenerate_game(void);

/**
 Generates dependency structure for the newly generated game.
*/
extern void generate_dependencies(void);

/**
 Shuffles the tiles left.
 @return True if the generated game is playable.
*/
int shuffle(void);
