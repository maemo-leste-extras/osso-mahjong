/* -*- Mode: C; indent-tabs-mode: t; tab-width: 8; c-basic-offset: 8 -*- */

/* 
 * GNOME-Mahjongg
 * (C) 1998-1999 the Free Software Foundation
 *
 *
 * Author: Francisco Bustamante
 *
 *
 * http://www.nuclecu.unam.mx/~pancho/
 * pancho@nuclecu.unam.mx
 */

#include <sys/types.h>
#include <sys/time.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <dirent.h>
#include <config.h>

#include <gtk/gtk.h>
#include <gconf/gconf-client.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <games-clock.h>

#include "mahjongg.h"
#include "drawing.h"
#include "solubility.h"
#include "mahjong_i18n.h"

#include "sound.h"
#include "hiscore.h"

#define APPNAME "mahjongg"
#define APPNAME_LONG "Mahjongg"
#define MAHJONG_ENDGAME_DIALOG_MIN_SIZE 250

/* #defines for the tile selection code. */
#define SELECTED_FLAG   1
#define HINT_FLAG       16

/* The number of half-cycles to blink during a hint (less 1) */
#define HINT_BLINK_NUM 5

#define PAUSE_WITH_HW_KEY "/apps/osso/games_startup/pause_with_hw_key"

static void set_hint_sensitive(void);

/* Sorted such that the bottom leftest are first, and layers decrease Bottom
 * left = high y, low x ! */

tilepos easy_map[MAX_TILES] = {
    {13, 7, 4}, {12, 8, 3}, {14, 8, 3}, {12, 6, 3},
    {14, 6, 3}, {10, 10, 2}, {12, 10, 2}, {14, 10, 2},
    {16, 10, 2}, {10, 8, 2}, {12, 8, 2}, {14, 8, 2},
    {16, 8, 2}, {10, 6, 2}, {12, 6, 2}, {14, 6, 2},
    {16, 6, 2}, {10, 4, 2}, {12, 4, 2}, {14, 4, 2},
    {16, 4, 2}, {8, 12, 1}, {10, 12, 1}, {12, 12, 1},
    {14, 12, 1}, {16, 12, 1}, {18, 12, 1}, {8, 10, 1},
    {10, 10, 1}, {12, 10, 1}, {14, 10, 1}, {16, 10, 1},
    {18, 10, 1}, {8, 8, 1}, {10, 8, 1}, {12, 8, 1},
    {14, 8, 1}, {16, 8, 1}, {18, 8, 1}, {8, 6, 1},
    {10, 6, 1}, {12, 6, 1}, {14, 6, 1}, {16, 6, 1},
    {18, 6, 1}, {8, 4, 1}, {10, 4, 1}, {12, 4, 1},
    {14, 4, 1}, {16, 4, 1}, {18, 4, 1}, {8, 2, 1},
    {10, 2, 1}, {12, 2, 1}, {14, 2, 1}, {16, 2, 1},
    {18, 2, 1}, {2, 14, 0}, {4, 14, 0}, {6, 14, 0},
    {8, 14, 0}, {10, 14, 0}, {12, 14, 0}, {14, 14, 0},
    {16, 14, 0}, {18, 14, 0}, {20, 14, 0}, {22, 14, 0},
    {24, 14, 0}, {6, 12, 0}, {8, 12, 0}, {10, 12, 0},
    {12, 12, 0}, {14, 12, 0}, {16, 12, 0}, {18, 12, 0},
    {20, 12, 0}, {4, 10, 0}, {6, 10, 0}, {8, 10, 0},
    {10, 10, 0}, {12, 10, 0}, {14, 10, 0}, {16, 10, 0},
    {18, 10, 0}, {20, 10, 0}, {22, 10, 0}, {0, 7, 0},
    {2, 8, 0}, {4, 8, 0}, {6, 8, 0}, {8, 8, 0},
    {10, 8, 0}, {12, 8, 0}, {14, 8, 0}, {16, 8, 0},
    {18, 8, 0}, {20, 8, 0}, {22, 8, 0}, {24, 8, 0},
    {2, 6, 0}, {4, 6, 0}, {6, 6, 0}, {8, 6, 0},
    {10, 6, 0}, {12, 6, 0}, {14, 6, 0}, {16, 6, 0},
    {18, 6, 0}, {20, 6, 0}, {22, 6, 0}, {24, 6, 0},
    {4, 4, 0}, {6, 4, 0}, {8, 4, 0}, {10, 4, 0},
    {12, 4, 0}, {14, 4, 0}, {16, 4, 0}, {18, 4, 0},
    {20, 4, 0}, {22, 4, 0}, {6, 2, 0}, {8, 2, 0},
    {10, 2, 0}, {12, 2, 0}, {14, 2, 0}, {16, 2, 0},
    {18, 2, 0}, {20, 2, 0}, {2, 0, 0}, {4, 0, 0},
    {6, 0, 0}, {8, 0, 0}, {10, 0, 0}, {12, 0, 0},
    {14, 0, 0}, {16, 0, 0}, {18, 0, 0}, {20, 0, 0},
    {22, 0, 0}, {24, 0, 0}, {26, 7, 0}, {28, 7, 0}
};

tilepos hard_map[MAX_TILES] = {
    {10, 6, 6},
    {9, 6, 5},
    {11, 6, 5},
    {8, 6, 4},
    {10, 6, 4},
    {12, 6, 4},
    {5, 6, 3},
    {7, 7, 3},
    {7, 5, 3},
    {9, 7, 3},
    {9, 5, 3},
    {11, 7, 3},
    {11, 5, 3},
    {13, 7, 3},
    {13, 5, 3},
    {15, 6, 3},
    {5, 8, 2},
    {7, 8, 2},
    {9, 8, 2},
    {11, 8, 2},
    {13, 8, 2},
    {15, 8, 2},
    {4, 6, 2},
    {6, 6, 2},
    {8, 6, 2},
    {10, 6, 2},
    {12, 6, 2},
    {14, 6, 2},
    {16, 6, 2},
    {5, 4, 2},
    {7, 4, 2},
    {9, 4, 2},
    {11, 4, 2},
    {13, 4, 2},
    {15, 4, 2},
    {7, 12, 1},
    {9, 11, 1},
    {11, 11, 1},
    {13, 12, 1},
    {2, 10, 1},
    {4, 10, 1},
    {6, 10, 1},
    {8, 9, 1},
    {10, 9, 1},
    {12, 9, 1},
    {14, 10, 1},
    {16, 10, 1},
    {18, 10, 1},
    {3, 8, 1},
    {3, 6, 1},
    {5, 8, 1},
    {5, 6, 1},
    {7, 7, 1},
    {9, 7, 1},
    {11, 7, 1},
    {13, 7, 1},
    {15, 8, 1},
    {17, 8, 1},
    {3, 4, 1},
    {5, 4, 1},
    {7, 5, 1},
    {9, 5, 1},
    {11, 5, 1},
    {13, 5, 1},
    {15, 6, 1},
    {17, 6, 1},
    {2, 2, 1},
    {4, 2, 1},
    {6, 2, 1},
    {8, 3, 1},
    {10, 3, 1},
    {12, 3, 1},
    {15, 4, 1},
    {17, 4, 1},
    {7, 0, 1},
    {9, 1, 1},
    {11, 1, 1},
    {14, 2, 1},
    {16, 2, 1},
    {18, 2, 1},
    {13, 0, 1},
    {6, 12, 0},
    {8, 12, 0},
    {10, 12, 0},
    {12, 12, 0},
    {14, 12, 0},
    {1, 11, 0},
    {3, 11, 0},
    {1, 9, 0},
    {0, 6, 0},
    {3, 9, 0},
    {5, 10, 0},
    {7, 10, 0},
    {9, 10, 0},
    {11, 10, 0},
    {13, 10, 0},
    {15, 10, 0},
    {17, 11, 0},
    {19, 11, 0},
    {2, 7, 0},
    {4, 7, 0},
    {6, 8, 0},
    {8, 8, 0},
    {2, 5, 0},
    {4, 5, 0},
    {6, 6, 0},
    {8, 6, 0},
    {10, 8, 0},
    {10, 6, 0},
    {12, 8, 0},
    {12, 6, 0},
    {14, 8, 0},
    {14, 6, 0},
    {17, 9, 0},
    {16, 7, 0},
    {19, 9, 0},
    {18, 7, 0},
    {1, 3, 0},
    {3, 3, 0},
    {6, 4, 0},
    {8, 4, 0},
    {10, 4, 0},
    {12, 4, 0},
    {14, 4, 0},
    {16, 5, 0},
    {18, 5, 0},
    {20, 6, 0},
    {1, 1, 0},
    {3, 1, 0},
    {5, 2, 0},
    {7, 2, 0},
    {9, 2, 0},
    {11, 2, 0},
    {13, 2, 0},
    {15, 2, 0},
    {17, 3, 0},
    {19, 3, 0},
    {17, 1, 0},
    {19, 1, 0},
    {6, 0, 0},
    {8, 0, 0},
    {10, 0, 0},
    {12, 0, 0},
    {14, 0, 0}
};

tilepos pyramid_map[MAX_TILES] = {
    {13, 14, 6}, {13, 8, 6}, {13, 2, 6}, {12, 14, 5},
    {14, 14, 5}, {12, 8, 5}, {14, 8, 5}, {12, 2, 5},
    {14, 2, 5}, {10, 14, 4}, {12, 14, 4}, {14, 14, 4},
    {16, 14, 4}, {2, 8, 4}, {11, 8, 4}, {13, 8, 4},
    {15, 8, 4}, {24, 8, 4}, {10, 2, 4}, {12, 2, 4},
    {14, 2, 4}, {16, 2, 4}, {8, 14, 3}, {10, 14, 3},
    {12, 14, 3}, {14, 14, 3}, {16, 14, 3}, {18, 14, 3},
    {2, 9, 3}, {24, 9, 3}, {10, 8, 3}, {12, 8, 3},
    {14, 8, 3}, {16, 8, 3}, {2, 7, 3}, {24, 7, 3},
    {8, 2, 3}, {10, 2, 3}, {12, 2, 3}, {14, 2, 3},
    {16, 2, 3}, {18, 2, 3}, {6, 14, 2}, {8, 14, 2},
    {10, 14, 2}, {12, 14, 2}, {14, 14, 2}, {16, 14, 2},
    {18, 14, 2}, {20, 14, 2}, {2, 10, 2}, {24, 10, 2},
    {2, 8, 2}, {9, 8, 2}, {11, 8, 2}, {13, 8, 2},
    {15, 8, 2}, {17, 8, 2}, {24, 8, 2}, {2, 6, 2},
    {24, 6, 2}, {6, 2, 2}, {8, 2, 2}, {10, 2, 2},
    {12, 2, 2}, {14, 2, 2}, {16, 2, 2}, {18, 2, 2},
    {20, 2, 2}, {4, 14, 1}, {6, 14, 1}, {8, 14, 1},
    {10, 14, 1}, {12, 14, 1}, {14, 14, 1}, {16, 14, 1},
    {18, 14, 1}, {20, 14, 1}, {22, 14, 1}, {2, 11, 1},
    {24, 11, 1}, {2, 9, 1}, {24, 9, 1}, {8, 8, 1},
    {10, 8, 1}, {12, 8, 1}, {14, 8, 1}, {16, 8, 1},
    {18, 8, 1}, {2, 7, 1}, {24, 7, 1}, {2, 5, 1},
    {24, 5, 1}, {4, 2, 1}, {6, 2, 1}, {8, 2, 1},
    {10, 2, 1}, {12, 2, 1}, {14, 2, 1}, {16, 2, 1},
    {18, 2, 1}, {20, 2, 1}, {22, 2, 1}, {2, 14, 0},
    {4, 14, 0}, {6, 14, 0}, {8, 14, 0}, {10, 14, 0},
    {12, 14, 0}, {14, 14, 0}, {16, 14, 0}, {18, 14, 0},
    {20, 14, 0}, {22, 14, 0}, {24, 14, 0}, {2, 12, 0},
    {24, 12, 0}, {2, 10, 0}, {24, 10, 0}, {2, 8, 0},
    {7, 8, 0}, {9, 8, 0}, {11, 8, 0}, {13, 8, 0},
    {15, 8, 0}, {17, 8, 0}, {19, 8, 0}, {24, 8, 0},
    {2, 6, 0}, {24, 6, 0}, {2, 4, 0}, {24, 4, 0},
    {2, 2, 0}, {4, 2, 0}, {6, 2, 0}, {8, 2, 0},
    {10, 2, 0}, {12, 2, 0}, {14, 2, 0}, {16, 2, 0},
    {18, 2, 0}, {20, 2, 0}, {22, 2, 0}, {24, 2, 0}
};

tilepos cross_map[MAX_TILES] = {
    {13, 8, 5},
    {13, 12, 4}, {13, 10, 4}, {9, 8, 4}, {11, 8, 4},
    {13, 8, 4}, {15, 8, 4}, {17, 8, 4}, {13, 6, 4},
    {13, 4, 4}, {5, 14, 3}, {21, 14, 3}, {13, 13, 3},
    {13, 11, 3}, {6, 8, 3},
    {8, 8, 3}, {10, 8, 3}, {12, 9, 3}, {12, 7, 3}, {14, 9, 3}, {14, 7, 3},
        {16, 8, 3}, {18, 8, 3}, {20, 8, 3},
    {13, 5, 3},
    {13, 3, 3}, {5, 2, 3}, {21, 2, 3}, {5, 14, 2},
    {7, 14, 2}, {13, 14, 2}, {19, 14, 2}, {21, 14, 2},
    {5, 12, 2}, {13, 12, 2}, {21, 12, 2}, {13, 10, 2},
    {5, 8, 2}, {7, 8, 2}, {9, 8, 2}, {11, 8, 2},
    {13, 8, 2}, {15, 8, 2}, {17, 8, 2}, {19, 8, 2},
    {21, 8, 2}, {13, 6, 2}, {5, 4, 2}, {13, 4, 2},
    {21, 4, 2}, {5, 2, 2}, {7, 2, 2}, {13, 2, 2},
    {19, 2, 2}, {21, 2, 2}, {13, 15, 1}, {5, 14, 1},
    {7, 14, 1}, {9, 14, 1}, {17, 14, 1}, {19, 14, 1},
    {21, 14, 1}, {13, 13, 1}, {5, 12, 1}, {7, 12, 1},
    {19, 12, 1}, {21, 12, 1}, {13, 11, 1}, {5, 10, 1},
    {21, 10, 1}, {4, 8, 1},
    {6, 8, 1}, {8, 8, 1}, {10, 8, 1}, {12, 9, 1}, {12, 7, 1},
    {14, 9, 1}, {14, 7, 1}, {16, 8, 1}, {18, 8, 1}, {20, 8, 1},
    {22, 8, 1}, {5, 6, 1}, {21, 6, 1}, {13, 5, 1},
    {5, 4, 1}, {7, 4, 1}, {19, 4, 1}, {21, 4, 1},
    {13, 3, 1}, {5, 2, 1}, {7, 2, 1}, {9, 2, 1},
    {17, 2, 1}, {19, 2, 1}, {21, 2, 1}, {13, 1, 1},
    {13, 16, 0}, {5, 14, 0}, {7, 14, 0}, {9, 14, 0},
    {11, 14, 0}, {13, 14, 0}, {15, 14, 0}, {17, 14, 0},
    {19, 14, 0}, {21, 14, 0}, {5, 12, 0}, {7, 12, 0},
    {13, 12, 0}, {19, 12, 0}, {21, 12, 0}, {5, 10, 0},
    {13, 10, 0}, {21, 10, 0}, {3, 8, 0}, {5, 8, 0},
    {7, 8, 0}, {9, 8, 0}, {11, 8, 0}, {13, 8, 0},
    {15, 8, 0}, {17, 8, 0}, {19, 8, 0}, {21, 8, 0},
    {23, 8, 0}, {5, 6, 0}, {13, 6, 0}, {21, 6, 0},
    {5, 4, 0}, {7, 4, 0}, {13, 4, 0}, {19, 4, 0},
    {21, 4, 0}, {5, 2, 0}, {7, 2, 0}, {9, 2, 0},
    {11, 2, 0}, {13, 2, 0}, {15, 2, 0}, {17, 2, 0},
    {19, 2, 0}, {21, 2, 0}, {13, 0, 0}
};


tilepos tictactoe_map[MAX_TILES] = {
    {6, 12, 4}, {8, 12, 4}, {10, 12, 4}, {12, 12, 4}, {14, 12, 4}, {16, 12,
                                                                    4}, {18,
                                                                         12,
                                                                         4},
    {6, 10, 4}, {18, 10, 4},
    {6, 8, 4}, {18, 8, 4},
    {6, 6, 4}, {18, 6, 4},
    {6, 4, 4}, {8, 4, 4}, {10, 4, 4}, {12, 4, 4}, {14, 4, 4}, {16, 4, 4}, {18,
                                                                           4,
                                                                           4},

    {6, 12, 3}, {8, 12, 3}, {10, 12, 3}, {12, 12, 3}, {14, 12, 3}, {16, 12,
                                                                    3}, {18,
                                                                         12,
                                                                         3},
    {6, 10, 3}, {18, 10, 3},
    {6, 8, 3}, {18, 8, 3},
    {6, 6, 3}, {18, 6, 3},
    {6, 4, 3}, {8, 4, 3}, {10, 4, 3}, {12, 4, 3}, {14, 4, 3}, {16, 4, 3}, {18,
                                                                           4,
                                                                           3},

    {6, 14, 2}, {18, 14, 2},
    {4, 12, 2}, {6, 12, 2}, {8, 12, 2}, {10, 12, 2}, {12, 12, 2}, {14, 12, 2},
        {16, 12, 2}, {18, 12, 2}, {20, 12, 2},
    {6, 10, 2}, {18, 10, 2},
    {6, 8, 2}, {18, 8, 2},
    {6, 6, 2}, {18, 6, 2},
    {4, 4, 2}, {6, 4, 2}, {8, 4, 2}, {10, 4, 2}, {12, 4, 2}, {14, 4, 2}, {16,
                                                                          4,
                                                                          2},
        {18, 4, 2}, {20, 4, 2},
    {6, 2, 2}, {18, 2, 2},

    {6, 16, 1}, {18, 16, 1},
    {6, 14, 1}, {18, 14, 1},
    {2, 12, 1}, {4, 12, 1}, {6, 12, 1}, {8, 12, 1}, {10, 12, 1}, {12, 12, 1},
        {14, 12, 1}, {16, 12, 1}, {18, 12, 1}, {20, 12, 1}, {22, 12, 1},
    {6, 10, 1}, {18, 10, 1},
    {6, 8, 1}, {18, 8, 1},
    {6, 6, 1}, {18, 6, 1},
    {2, 4, 1}, {4, 4, 1}, {6, 4, 1}, {8, 4, 1}, {10, 4, 1}, {12, 4, 1}, {14,
                                                                         4,
                                                                         1},
        {16, 4, 1}, {18, 4, 1}, {20, 4, 1}, {22, 4, 1},
    {6, 2, 1}, {18, 2, 1},
    {6, 0, 1}, {18, 0, 1},

    {6, 16, 0}, {18, 16, 0},
    {6, 14, 0}, {18, 14, 0},
    {0, 12, 0}, {2, 12, 0}, {4, 12, 0}, {6, 12, 0}, {8, 12, 0}, {10, 12, 0},
        {12, 12, 0}, {14, 12, 0}, {16, 12, 0}, {18, 12, 0}, {20, 12, 0}, {22,
                                                                          12,
                                                                          0},
        {24, 12, 0},
    {6, 10, 0}, {18, 10, 0},
    {6, 8, 0}, {18, 8, 0},
    {6, 6, 0}, {18, 6, 0},
    {0, 4, 0}, {2, 4, 0}, {4, 4, 0}, {6, 4, 0}, {8, 4, 0}, {10, 4, 0}, {12, 4,
                                                                        0},
        {14, 4, 0}, {16, 4, 0}, {18, 4, 0}, {20, 4, 0}, {22, 4, 0}, {24, 4,
                                                                     0},
    {6, 2, 0}, {18, 2, 0},
    {6, 0, 0}, {18, 0, 0},
};

tilepos cloud_map[MAX_TILES] = {
    {12, 11, 2},

    {0, 6, 2}, {4, 6, 2}, {8, 6, 2}, {12, 6, 2}, {16, 6, 2}, {20, 6, 2}, {24,
                                                                          6,
                                                                          2},
    {0, 4, 2}, {4, 4, 2}, {8, 4, 2}, {12, 4, 2}, {16, 4, 2}, {20, 4, 2}, {24,
                                                                          4,
                                                                          2},
    {0, 2, 2}, {4, 2, 2}, {8, 2, 2}, {12, 2, 2}, {16, 2, 2}, {20, 2, 2}, {24,
                                                                          2,
                                                                          2},
    {0, 0, 2}, {4, 0, 2}, {8, 0, 2}, {12, 0, 2}, {16, 0, 2}, {20, 0, 2}, {24,
                                                                          0,
                                                                          2},

    {6, 11, 1}, {8, 11, 1}, {10, 11, 1}, {12, 11, 1}, {14, 11, 1},
    {16, 11, 1}, {18, 11, 1}, {20, 11, 1},

    {0, 6, 1}, {4, 6, 1}, {8, 6, 1}, {12, 6, 1}, {16, 6, 1}, {20, 6, 1}, {24,
                                                                          6,
                                                                          1},
    {0, 4, 1}, {4, 4, 1}, {8, 4, 1}, {12, 4, 1}, {16, 4, 1}, {20, 4, 1}, {24,
                                                                          4,
                                                                          1},
    {0, 2, 1}, {4, 2, 1}, {8, 2, 1}, {12, 2, 1}, {16, 2, 1}, {20, 2, 1}, {24,
                                                                          2,
                                                                          1},
    {0, 0, 1}, {4, 0, 1}, {8, 0, 1}, {12, 0, 1}, {16, 0, 1}, {20, 0, 1}, {24,
                                                                          0,
                                                                          1},

    {5, 11, 0}, {7, 11, 0}, {9, 11, 0}, {11, 11, 0}, {13, 11, 0},
    {15, 11, 0}, {17, 11, 0}, {19, 11, 0}, {21, 11, 0},

    {0, 8, 0}, {2, 8, 0}, {4, 8, 0}, {6, 8, 0}, {8, 8, 0}, {10, 8, 0}, {12, 8,
                                                                        0},
    {14, 8, 0}, {16, 8, 0}, {18, 8, 0}, {20, 8, 0}, {22, 8, 0}, {24, 8, 0},
        {26, 8, 0},

    {0, 6, 0}, {2, 6, 0}, {4, 6, 0}, {6, 6, 0}, {8, 6, 0}, {10, 6, 0}, {12, 6,
                                                                        0},
    {14, 6, 0}, {16, 6, 0}, {18, 6, 0}, {20, 6, 0}, {22, 6, 0}, {24, 6, 0},
        {26, 6, 0},

    {0, 4, 0}, {2, 4, 0}, {4, 4, 0}, {6, 4, 0}, {8, 4, 0}, {10, 4, 0}, {12, 4,
                                                                        0},
    {14, 4, 0}, {16, 4, 0}, {18, 4, 0}, {20, 4, 0}, {22, 4, 0}, {24, 4, 0},
        {26, 4, 0},

    {0, 2, 0}, {2, 2, 0}, {4, 2, 0}, {6, 2, 0}, {8, 2, 0}, {10, 2, 0}, {12, 2,
                                                                        0},
    {14, 2, 0}, {16, 2, 0}, {18, 2, 0}, {20, 2, 0}, {22, 2, 0}, {24, 2, 0},
        {26, 2, 0},

    {0, 0, 0}, {2, 0, 0}, {4, 0, 0}, {6, 0, 0}, {8, 0, 0}, {10, 0, 0}, {12, 0,
                                                                        0},
    {14, 0, 0}, {16, 0, 0}, {18, 0, 0}, {20, 0, 0}, {22, 0, 0}, {24, 0, 0},
        {26, 0, 0},

};

tilepos reddragon_map[MAX_TILES] = {
    {10, 8, 2},
    {12, 8, 2},
    {14, 8, 2},
    {16, 8, 2},
    {22, 8, 2},
    {10, 6, 2},
    {12, 6, 2},
    {14, 6, 2},
    {16, 6, 2},
    {10, 4, 2},
    {12, 4, 2},
    {14, 4, 2},
    {16, 4, 2},
    {10, 2, 2},
    {12, 2, 2},
    {14, 2, 2},
    {16, 2, 2},
    {5, 9, 1},
    {7, 9, 1},
    {9, 9, 1},
    {11, 9, 1},
    {13, 9, 1},
    {15, 9, 1},
    {17, 9, 1},
    {19, 9, 1},
    {21, 9, 1},
    {7, 7, 1},
    {9, 7, 1},
    {11, 7, 1},
    {13, 7, 1},
    {15, 7, 1},
    {17, 7, 1},
    {19, 7, 1},
    {21, 7, 1},
    {23, 7, 1},
    {7, 5, 1},
    {9, 5, 1},
    {11, 5, 1},
    {13, 5, 1},
    {15, 5, 1},
    {17, 5, 1},
    {19, 5, 1},
    {21, 5, 1},
    {23, 5, 1},
    {5, 3, 1},
    {7, 3, 1},
    {9, 3, 1},
    {11, 3, 1},
    {13, 3, 1},
    {15, 3, 1},
    {17, 3, 1},
    {19, 3, 1},
    {21, 3, 1},
    {23, 3, 1},
    {7, 1, 1},
    {9, 1, 1},
    {11, 1, 1},
    {13, 1, 1},
    {15, 1, 1},
    {17, 1, 1},
    {19, 1, 1},
    {21, 1, 1},
    {4, 13, 0},
    {8, 13, 0},
    {12, 13, 0},
    {16, 13, 0},
    {20, 13, 0},
    {24, 13, 0},
    {0, 12, 0},
    {28, 12, 0},
    {4, 10, 0},
    {6, 10, 0},
    {8, 10, 0},
    {10, 10, 0},
    {12, 10, 0},
    {14, 10, 0},
    {16, 10, 0},
    {18, 10, 0},
    {20, 10, 0},
    {22, 10, 0},
    {24, 10, 0},
    {0, 9, 0},
    {28, 9, 0},
    {4, 8, 0},
    {6, 8, 0},
    {8, 8, 0},
    {10, 8, 0},
    {12, 8, 0},
    {14, 8, 0},
    {16, 8, 0},
    {18, 8, 0},
    {20, 8, 0},
    {22, 8, 0},
    {24, 8, 0},
    {0, 6, 0},
    {4, 6, 0},
    {6, 6, 0},
    {8, 6, 0},
    {10, 6, 0},
    {12, 6, 0},
    {14, 6, 0},
    {16, 6, 0},
    {18, 6, 0},
    {20, 6, 0},
    {22, 6, 0},
    {24, 6, 0},
    {28, 6, 0},
    {4, 4, 0},
    {6, 4, 0},
    {8, 4, 0},
    {10, 4, 0},
    {12, 4, 0},
    {14, 4, 0},
    {16, 4, 0},
    {18, 4, 0},
    {20, 4, 0},
    {22, 4, 0},
    {24, 4, 0},
    {0, 3, 0},
    {28, 3, 0},
    {4, 2, 0},
    {6, 2, 0},
    {8, 2, 0},
    {10, 2, 0},
    {12, 2, 0},
    {14, 2, 0},
    {16, 2, 0},
    {18, 2, 0},
    {20, 2, 0},
    {22, 2, 0},
    {24, 2, 0},
    {0, 0, 0},
    {4, 0, 0},
    {6, 0, 0},
    {8, 0, 0},
    {10, 0, 0},
    {12, 0, 0},
    {14, 0, 0},
    {16, 0, 0},
    {18, 0, 0},
    {20, 0, 0},
    {22, 0, 0},
    {24, 0, 0},
    {28, 0, 0}
};
tilepos fourbridges_map[MAX_TILES] = {
    {7, 13, 3},
    {17, 13, 3},
    {7, 3, 3},
    {17, 3, 3},
    {6, 14, 2},
    {8, 14, 2},
    {16, 14, 2},
    {18, 14, 2},
    {6, 12, 2},
    {8, 12, 2},
    {16, 12, 2},
    {18, 12, 2},
    {6, 4, 2},
    {8, 4, 2},
    {16, 4, 2},
    {18, 4, 2},
    {6, 2, 2},
    {8, 2, 2},
    {16, 2, 2},
    {18, 2, 2},
    {5, 15, 1},
    {7, 15, 1},
    {9, 15, 1},
    {15, 15, 1},
    {17, 15, 1},
    {19, 15, 1},
    {5, 13, 1},
    {7, 13, 1},
    {9, 13, 1},
    {15, 13, 1},
    {17, 13, 1},
    {19, 13, 1},
    {5, 11, 1},
    {7, 11, 1},
    {9, 11, 1},
    {15, 11, 1},
    {17, 11, 1},
    {19, 11, 1},
    {5, 5, 1},
    {7, 5, 1},
    {9, 5, 1},
    {15, 5, 1},
    {17, 5, 1},
    {19, 5, 1},
    {5, 3, 1},
    {7, 3, 1},
    {9, 3, 1},
    {15, 3, 1},
    {17, 3, 1},
    {19, 3, 1},
    {5, 1, 1},
    {7, 1, 1},
    {9, 1, 1},
    {15, 1, 1},
    {17, 1, 1},
    {19, 1, 1},
    {2, 16, 0},
    {4, 16, 0},
    {6, 16, 0},
    {8, 16, 0},
    {10, 16, 0},
    {12, 16, 0},
    {14, 16, 0},
    {16, 16, 0},
    {18, 16, 0},
    {20, 16, 0},
    {22, 16, 0},
    {4, 14, 0},
    {6, 14, 0},
    {8, 14, 0},
    {10, 14, 0},
    {14, 14, 0},
    {16, 14, 0},
    {18, 14, 0},
    {20, 14, 0},
    {4, 12, 0},
    {6, 12, 0},
    {8, 12, 0},
    {10, 12, 0},
    {12, 12, 0},
    {14, 12, 0},
    {16, 12, 0},
    {18, 12, 0},
    {20, 12, 0},
    {0, 10, 0},
    {2, 10, 0},
    {4, 10, 0},
    {6, 10, 0},
    {8, 10, 0},
    {10, 10, 0},
    {12, 10, 0},
    {14, 10, 0},
    {16, 10, 0},
    {18, 10, 0},
    {20, 10, 0},
    {22, 10, 0},
    {24, 10, 0},
    {3, 8, 0},
    {5, 8, 0},
    {7, 8, 0},
    {17, 8, 0},
    {19, 8, 0},
    {21, 8, 0},
    {0, 6, 0},
    {2, 6, 0},
    {4, 6, 0},
    {6, 6, 0},
    {8, 6, 0},
    {10, 6, 0},
    {12, 6, 0},
    {14, 6, 0},
    {16, 6, 0},
    {18, 6, 0},
    {20, 6, 0},
    {22, 6, 0},
    {24, 6, 0},
    {4, 4, 0},
    {6, 4, 0},
    {8, 4, 0},
    {10, 4, 0},
    {12, 4, 0},
    {14, 4, 0},
    {16, 4, 0},
    {18, 4, 0},
    {20, 4, 0},
    {4, 2, 0},
    {6, 2, 0},
    {8, 2, 0},
    {10, 2, 0},
    {14, 2, 0},
    {16, 2, 0},
    {18, 2, 0},
    {20, 2, 0},
    {2, 0, 0},
    {4, 0, 0},
    {6, 0, 0},
    {8, 0, 0},
    {10, 0, 0},
    {12, 0, 0},
    {14, 0, 0},
    {16, 0, 0},
    {18, 0, 0},
    {20, 0, 0},
    {22, 0, 0}
};



tilepos ziggurat_map[MAX_TILES] = {
    {10, 8, 5}, {12, 8, 5}, {14, 8, 5}, {16, 8, 5}, {18, 8, 5},
    {10, 6, 5}, {12, 6, 5}, {14, 6, 5}, {16, 6, 5}, {18, 6, 5},
    {8, 8, 4}, {10, 8, 4}, {12, 8, 4}, {14, 8, 4}, {16, 8, 4}, {18, 8, 4},
        {20, 8, 4},
    {8, 6, 4}, {10, 6, 4}, {12, 6, 4}, {14, 6, 4}, {16, 6, 4}, {18, 6, 4},
        {20, 6, 4},
    {6, 8, 3}, {8, 8, 3}, {10, 8, 3}, {12, 8, 3}, {14, 8, 3}, {16, 8, 3}, {18,
                                                                           8,
                                                                           3},
        {20, 8, 3}, {22, 8, 3},
    {6, 6, 3}, {8, 6, 3}, {10, 6, 3}, {12, 6, 3}, {14, 6, 3}, {16, 6, 3}, {18,
                                                                           6,
                                                                           3},
        {20, 6, 3}, {22, 6, 3},
    {6, 13, 2}, {22, 13, 2}, {14, 12, 2},
    {4, 8, 2}, {6, 8, 2}, {8, 8, 2}, {20, 8, 2}, {22, 8, 2}, {24, 8, 2},
    {4, 6, 2}, {6, 6, 2}, {8, 6, 2}, {20, 6, 2}, {22, 6, 2}, {24, 6, 2},
    {14, 2, 2}, {6, 1, 2}, {22, 1, 2},
    {6, 13, 1}, {22, 13, 1}, {13, 12, 1}, {15, 12, 1},
    {2, 8, 1}, {4, 8, 1}, {6, 8, 1}, {22, 8, 1}, {24, 8, 1}, {26, 8, 1},
    {2, 6, 1}, {4, 6, 1}, {6, 6, 1}, {22, 6, 1}, {24, 6, 1}, {26, 6, 1},
    {13, 2, 1}, {15, 2, 1}, {6, 1, 1}, {22, 1, 1},
    {0, 14, 0}, {5, 14, 0}, {7, 14, 0}, {9, 14, 0}, {11, 14, 0}, {13, 14, 0},
        {15, 14, 0}, {17, 14, 0}, {19, 14, 0}, {21, 14, 0}, {23, 14, 0}, {28,
                                                                          14,
                                                                          0},
    {0, 12, 0}, {5, 12, 0}, {13, 12, 0}, {15, 12, 0}, {23, 12, 0}, {28, 12,
                                                                    0},
    {0, 10, 0}, {5, 10, 0}, {13, 10, 0}, {15, 10, 0}, {23, 10, 0}, {28, 10,
                                                                    0},
    {0, 8, 0}, {2, 8, 0}, {4, 8, 0}, {6, 8, 0}, {22, 8, 0}, {24, 8, 0}, {26,
                                                                         8,
                                                                         0},
        {28, 8, 0},
    {0, 6, 0}, {2, 6, 0}, {4, 6, 0}, {6, 6, 0}, {22, 6, 0}, {24, 6, 0}, {26,
                                                                         6,
                                                                         0},
        {28, 6, 0},
    {0, 4, 0}, {5, 4, 0}, {13, 4, 0}, {15, 4, 0}, {23, 4, 0}, {28, 4, 0},
    {0, 2, 0}, {5, 2, 0}, {13, 2, 0}, {15, 2, 0}, {23, 2, 0}, {28, 2, 0},
    {0, 0, 0}, {5, 0, 0}, {7, 0, 0}, {9, 0, 0}, {11, 0, 0}, {13, 0, 0}, {15,
                                                                         0,
                                                                         0},
        {17, 0, 0}, {19, 0, 0}, {21, 0, 0}, {23, 0, 0}, {28, 0, 0},
};

tilepos *pos = 0;

tile tiles[MAX_TILES];

gint selected_tile, visible_tiles;
gint sequence_number;

guint32 current_seed, next_seed;

// static gint windowwidth, windowheight;

GList *tileset_list = NULL;

gchar *tileset = NULL;
gchar *mapset = NULL;
static gchar *score_current_mapset = NULL;

// static gchar *selected_tileset = NULL;

#if 0
// They don't like N_, so use #if 0 -hack
_("game_fi_mahjong_game_board_1");
_("game_fi_mahjong_game_board_2");
_("game_fi_mahjong_game_board_3");
_("game_fi_mahjong_game_board_4");
_("game_fi_mahjong_game_board_5");
_("game_fi_mahjong_game_board_6");
_("game_fi_mahjong_game_board_7");
_("game_fi_mahjong_game_board_8");
#endif

struct _maps {
    gchar *name;
    gchar *score_name;
    tilepos *map;
} maps[] =
{
    {
    "game_fi_mahjong_game_board_1", "easy", easy_map},
    {
    "game_fi_mahjong_game_board_2", "difficult", hard_map},
    {
    "game_fi_mahjong_game_board_3", "confounding", cross_map},
    {
    "game_fi_mahjong_game_board_4", "pyramid", pyramid_map},
    {
    "game_fi_mahjong_game_board_5", "tictactoe", tictactoe_map},
    {
    "game_fi_mahjong_game_board_6", "cloud", cloud_map},
    {
    "game_fi_mahjong_game_board_7", "dragon", reddragon_map},
    {
"game_fi_mahjong_game_board_8", "bridges", fourbridges_map},};


gint hint_tiles[2];
guint timer;
guint timeout_counter = HINT_BLINK_NUM + 1;

GtkWidget *moves_label;
gint moves_left = 0;
GtkWidget *chrono;
gint paused = 0;

/* for the preferences */
GConfClient *conf_client;
gboolean popup_warn = FALSE;
GtkWidget *warn_cb = NULL, *confirm_cb = NULL;
GtkWidget *colour_well = NULL;
GtkWidget *pref_dialog = NULL;
GtkWidget *about = NULL;

/* Has the map been changed ? */
gboolean new_map = TRUE;

enum {
    GAME_RUNNING = 0,
    GAME_WON,
    GAME_LOST,
    GAME_DEAD
} game_over;

static void clear_undo_queue(void);
void you_won(void);
void no_match(void);
void properties_callback(GtkWidget * widget, gpointer data);
void about_callback(GtkWidget * widget, gpointer data);
void show_tb_callback(GtkWidget * widget, gpointer data);
void sound_on_callback(GtkWidget * widget, gpointer data);
void scores_callback(GtkWidget * widget, gpointer data);
void confirm_action(GtkWidget * widget, gpointer data);
void ensure_pause_off(void);
void pause_callback(void);
void restart_game(void);
void select_game(GtkWidget * widget, gpointer data);

/* Undo and redo sensitivity functionality. */
static void
set_undoredo_sensitive(gboolean undo, gboolean redo)
{
    update_undo_redo(undo, redo);
    set_hint_sensitive();
}

static void
message(gchar * themessage)
{
    themessage = themessage;
}

static void
chrono_start(void)
{
    games_clock_stop(GAMES_CLOCK(chrono));
    games_clock_set_seconds(GAMES_CLOCK(chrono), 0);
    games_clock_start(GAMES_CLOCK(chrono));
}

static gint
update_moves_left(void)
{
    check_free();

    return moves_left;
}

static void
select_tile(gint tileno)
{
    tiles[tileno].selected |= SELECTED_FLAG;
    draw_tile(tileno);
    selected_tile = tileno;

    sound_play(get_app_data()->app_sound_data, SOUND_SELECT);
}

static void
unselect_tile(gint tileno)
{
    selected_tile = MAX_TILES + 1;
    tiles[tileno].selected &= ~SELECTED_FLAG;
    draw_tile(tileno);

    sound_play(get_app_data()->app_sound_data, SOUND_SELECT);
}

static void
remove_pair(gint tile1, gint tile2)
{
    tiles[tile1].visible = tiles[tile2].visible = 0;
    tiles[tile1].selected &= ~SELECTED_FLAG;
    tiles[tile2].selected &= ~SELECTED_FLAG;
    draw_tile(tile1);
    draw_tile(tile2);
    clear_undo_queue();
    tiles[tile1].sequence = tiles[tile2].sequence = sequence_number;
    sequence_number++;
    selected_tile = MAX_TILES + 1;
    visible_tiles -= 2;

    update_moves_left();
    set_undoredo_sensitive(TRUE, FALSE);

    if (visible_tiles <= 0)
    {
        sound_play(get_app_data()->app_sound_data, SOUND_MOVE);
        games_clock_stop(GAMES_CLOCK(chrono));
        you_won();
    }
    else
    {
        sound_play(get_app_data()->app_sound_data, SOUND_MOVE);
    }
}

void
tile_event(gint tileno, gint button)
{
    if (paused)
        return;

    if (!tile_free(tileno))
    {
        sound_play(get_app_data()->app_sound_data, SOUND_SELECT_FAIL);
        return;
    }

    switch (button)
    {
        case 1:
            if (tiles[tileno].selected & SELECTED_FLAG)
            {
                unselect_tile(tileno);
                return;
            }
            if (selected_tile >= MAX_TILES)
            {
                select_tile(tileno);
                return;
            }
            if ((tiles[selected_tile].type == tiles[tileno].type))
            {
                remove_pair(selected_tile, tileno);
                return;
            }
            no_match();
            break;

        case 3:
            if (selected_tile < MAX_TILES)
                unselect_tile(selected_tile);
            select_tile(tileno);

        default:
            break;
    }
}

void
set_map_selection(gint mapno)
{
    struct _maps *map;

    map = &(maps[mapno]);

    new_map = TRUE;

    g_free(mapset);
    mapset = g_strdup(map->name);
}

void
no_match(void)
{
    sound_play(get_app_data()->app_sound_data, SOUND_MOVE_FAIL);
}

void
check_free(void)
{
    gint i;
    gint tile_count[MAX_TILES];

    moves_left = 0;

    for (i = 0; i < MAX_TILES; i++)
        tile_count[i] = 0;

    for (i = 0; i < MAX_TILES; i++)
    {
        if (tile_free(i))
            tile_count[tiles[i].type]++;
    }

    for (i = 0; i < MAX_TILES; i++)
        moves_left += tile_count[i] >> 1;
}

gboolean
dialog_key_press(GtkWidget * widget, GdkEventKey * event, gpointer data)
{
    (void) data;

    if (event->state & (GDK_CONTROL_MASK |
                        GDK_SHIFT_MASK |
                        GDK_MOD1_MASK |
                        GDK_MOD3_MASK | GDK_MOD4_MASK | GDK_MOD5_MASK))
    {
        return FALSE;
    }

    switch (event->keyval)
    {
        case GDK_Escape:
            gtk_dialog_response(GTK_DIALOG(widget), GTK_RESPONSE_CANCEL);
            return TRUE;
            break;
        case GDK_F6:
        case GDK_F4:
        case GDK_F5:
            settings_set_int(PAUSE_WITH_HW_KEY, event->keyval);
            gtk_dialog_response(GTK_DIALOG(widget), GTK_RESPONSE_CANCEL);
            break;
        default:
            break;
    }

    return FALSE;
}


void
you_won(void)
{
#define TEXT_SIZE 512
    time_t seconds;
    time_t best;
    gint min, sec;
    gint bmin, bsec;
    gfloat score;
    GtkWidget *label = NULL;
    AppData *app_data = NULL;
    gchar text[TEXT_SIZE];
    gboolean best_time = FALSE;

    HiScore *scores;
    int count;

    count = hiscore_get(&scores);

    best = hiscore_get_score(scores, count, mapset[strlen(mapset) - 1]);

    bmin = best / 60;
    bsec = best % 60;

    games_clock_stop(GAMES_CLOCK(chrono));
    seconds = games_clock_get_seconds(GAMES_CLOCK(chrono));

    score = (seconds / 60) * 1.0 + (seconds % 60) / 100.0;

    if ((best == 0) || (seconds < best))
        best_time = TRUE;
    else
        best_time = FALSE;

    set_hint_sensitive();

    app_data = get_app_data();

    min = seconds / 60;
    sec = seconds % 60;

    app_data->app_ui_data->dialog =
        GTK_DIALOG(gtk_dialog_new_with_buttons
                   (_("game_ti_mahjong_end_game_title"),
                    app_data->app_ui_data->window,
                    GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                    dgettext ("hildon-libs", "wdgt_bd_done" ), /*GTK_RESPONSE_ACCEPT*/GTK_RESPONSE_OK,
                    NULL));
    g_signal_connect(app_data->app_ui_data->dialog, "focus-out-event",
                     G_CALLBACK(end_dialog_focus_out),
                     (gpointer) app_data->app_ui_data);
    /*hildon_help_dialog_help_enable(GTK_DIALOG(app_data->app_ui_data->dialog),
                                   MAHJONG_HELP_ENDGAME,
                                   get_app_data()->app_osso_data->osso);*/

    if (!best_time)
    {
        g_snprintf(text, TEXT_SIZE,
                   _
                   ("game_fi_mahjong_end_game_description%s%.2d%.2d%.2d%.2d"),
                   _(mapset), min, sec, bmin, bsec);
    }
    else
    {
        if (best != 0)
        {
            g_snprintf(text, TEXT_SIZE,
                       _
                       ("game_fi_mahjong_end_game_description2%s%.2d%.2d%.2d%.2d"),
                       _(mapset), min, sec, bmin, bsec);
        }
        else
        {
            g_snprintf(text, TEXT_SIZE,
                       _("game_fi_mahjong_end_game_description3%s%.2d%.2d"),
                       _(mapset), min, sec);
        }
        count = hiscore_set(&scores,
                            count, mapset[strlen(mapset) - 1], seconds);
        hiscore_put(scores, count, app_data);
    }
    g_free(scores);

    label = gtk_label_new(text);
    /* Bugfix for bug 35795 */
    gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_CENTER);
    gtk_label_set_ellipsize(GTK_LABEL(label), PANGO_ELLIPSIZE_NONE);
    GtkRequisition size;
    gtk_container_add(GTK_CONTAINER(app_data->app_ui_data->dialog->vbox),
                      label);
    g_signal_connect(G_OBJECT(app_data->app_ui_data->dialog),
                     "key_press_event", G_CALLBACK(dialog_key_press), NULL);
    gtk_widget_size_request(GTK_WIDGET(label), &size);
    if ((size.width + 5) <= MAHJONG_ENDGAME_DIALOG_MIN_SIZE)
    {
        gtk_widget_set_size_request(GTK_WIDGET(app_data->app_ui_data->dialog),
                                    MAHJONG_ENDGAME_DIALOG_MIN_SIZE, -1);
    }
    else
    {
        gtk_widget_set_size_request(GTK_WIDGET(app_data->app_ui_data->dialog),
                                    size.width + 5, -1);
    }
    gtk_widget_show_all(GTK_WIDGET(app_data->app_ui_data->dialog));

    gtk_dialog_run(app_data->app_ui_data->dialog);
    if (app_data->app_ui_data->dialog)
    {
        gtk_widget_destroy(GTK_WIDGET(app_data->app_ui_data->dialog));
    }
    app_data->app_ui_data->dialog = NULL;
    mahjong_end();
}

static gint
hint_timeout(gpointer data)
{
    data = data;
    timeout_counter++;

    if (timeout_counter > HINT_BLINK_NUM)
    {
        if (selected_tile < MAX_TILES)
            tiles[selected_tile].selected = 1;
        return 0;
    }

    tiles[hint_tiles[0]].selected ^= HINT_FLAG;
    tiles[hint_tiles[1]].selected ^= HINT_FLAG;
    draw_tile(hint_tiles[0]);
    draw_tile(hint_tiles[1]);

    return 1;
}

void
hint_callback(GtkWidget * widget, gpointer data)
{
    widget = widget;
    data = data;

    gint i, j, free = 0, type;

    if (paused || game_over)
        return;

    /* This prevents the flashing speeding up if the hint button is pressed
     * multiple times. */
    if (timeout_counter <= HINT_BLINK_NUM)
        return;

    /* Snarfed from check free Tile Free is now _so_ much quicker, it is more 
     * elegant to do a British Library search, and safer. */

    /* Clear any selection */
    if (selected_tile < MAX_TILES)
    {
        tiles[selected_tile].selected = 0;
        draw_tile(selected_tile);
        selected_tile = MAX_TILES + 1;
    }

    for (i = 0; i < MAX_TILES && !free; i++)
        if (tile_free(i))
        {
            type = tiles[i].type;
            for (j = 0; j < MAX_TILES && !free; j++)
            {
                free = (tiles[j].type == type && i != j && tile_free(j));
                if (free)
                {
                    tiles[i].selected ^= HINT_FLAG;
                    tiles[j].selected ^= HINT_FLAG;
                    draw_tile(i);
                    draw_tile(j);
                    hint_tiles[0] = i;
                    hint_tiles[1] = j;
                }
            }
        }

    /* This is a good way to test check_free for (i=0;i<MAX_TILES;i++) if
     * (tiles[i].selected == 17) tiles[i].visible = 0 ; */

    timeout_counter = 0;
    timer = g_timeout_add(250, (GSourceFunc) hint_timeout, NULL);

    /* 30s penalty */
    games_clock_add_seconds(GAMES_CLOCK(chrono), PENALTY_HINT);

    sound_play(get_app_data()->app_sound_data, SOUND_HINT);
}

void
pause_callback(void)
{
    static gboolean noloops = FALSE;

    /* The calls to set the menu bar toggle-button will trigger another
     * callback, which will trigger another callback ... this must be
     * stopped. */
    if (noloops)
        return;

    noloops = TRUE;
    paused = !paused;
    draw_all_tiles();
    if (paused)
    {
        games_clock_stop(GAMES_CLOCK(chrono));
    }
    else
    {
        games_clock_start(GAMES_CLOCK(chrono));
    }
    noloops = FALSE;
}

void
ensure_pause_off(void)
{
    if (paused)
    {
        draw_all_tiles();
        message("");
    }
    paused = FALSE;
}

static void
init_game(void)
{
    update_moves_left();
    game_over = GAME_RUNNING;
    sequence_number = 1;
    visible_tiles = MAX_TILES;
    selected_tile = MAX_TILES + 1;
    set_undoredo_sensitive(FALSE, FALSE);

    chrono_start();
}

void
confirm_action(GtkWidget * widget, gpointer data)
{
    widget = widget;
    data = data;

    switch (GPOINTER_TO_INT(data))
    {
        case NEW_GAME:
        case NEW_GAME_WITH_SEED:
            ensure_pause_off();
            new_game(GPOINTER_TO_INT(data) == NEW_GAME);
            break;
        case RESTART_GAME:
            restart_game();
            break;
        case QUIT_GAME:
            gtk_main_quit();
            break;
        default:
            break;
    }
}

gboolean
delete_event_callback(GtkWidget * widget, GdkEvent * any, gpointer data)
{
    any = any;

    mahjong_pause(NULL, get_app_data()->app_ui_data);
    confirm_action(widget, data);
    return TRUE;
}

void
restart_game(void)
{
    gint i;

    ensure_pause_off();
    for (i = 0; i < MAX_TILES; i++)
    {
        tiles[i].visible = 1;
        tiles[i].selected = 0;
        tiles[i].sequence = 0;
    }
    draw_all_tiles();
    init_game();
}

void
redo_tile_callback(GtkWidget * widget, gpointer data)
{
    gint i, change;

    widget = widget;
    data = data;

    if (paused)
        return;
    if (sequence_number > (MAX_TILES / 2))
        return;

    if (selected_tile < MAX_TILES)
    {
        tiles[selected_tile].selected = 0;
        draw_tile(selected_tile);
        selected_tile = MAX_TILES + 1;
    }
    change = 0;
    for (i = 0; i < MAX_TILES; i++)
        if (tiles[i].sequence == sequence_number)
        {
            tiles[i].selected = 0;
            tiles[i].visible = 0;
            draw_tile(i);
            visible_tiles--;
            change = 1;
        }
    if (change)
    {
        if (sequence_number < MAX_TILES)
            sequence_number++;
    }

    set_undoredo_sensitive(TRUE, FALSE);
    for (i = 0; i < MAX_TILES; i++)
    {
        if (tiles[i].sequence == sequence_number)
            set_undoredo_sensitive(TRUE, TRUE);
    }

    update_moves_left();
    set_hint_sensitive();       /* 20/05/2005 */

    sound_play(get_app_data()->app_sound_data, SOUND_UNDO);
}

void
undo_tile_callback(GtkWidget * widget, gpointer data)
{
    gint i;

    data = data;
    widget = widget;

    if (paused || game_over == GAME_WON)
        return;
    if (game_over == GAME_LOST)
        game_over = GAME_RUNNING;
    if (sequence_number > 1)
        sequence_number--;
    else
        return;

    games_clock_start(GAMES_CLOCK(chrono));

    if (selected_tile < MAX_TILES)
    {
        tiles[selected_tile].selected = 0;
        draw_tile(selected_tile);
        selected_tile = MAX_TILES + 1;
    }

    for (i = 0; i < MAX_TILES; i++)
        if (tiles[i].sequence == sequence_number)
        {
            tiles[i].selected = 0;
            tiles[i].visible = 1;
            visible_tiles++;
            draw_tile(i);
        }


    /* set_menus_sensitive (); */
    // set_hint_sensitive ();
    set_undoredo_sensitive(sequence_number > 1, TRUE);
    update_moves_left();
    set_hint_sensitive();       /* 20/05/2004 */

    sound_play(get_app_data()->app_sound_data, SOUND_UNDO);
}

void
sound_on_callback(GtkWidget * widget, gpointer data)
{
    widget = widget;
    data = data;
    // g_print ("mer\n");
}

/* You loose your re-do queue when you make a move */
static void
clear_undo_queue(void)
{
    gint lp;

    for (lp = 0; lp < MAX_TILES; lp++)
        if (tiles[lp].sequence >= sequence_number)
            tiles[lp].sequence = 0;
}

void
load_map(void)
{
    gchar *name = mapset;
    guint lp = 0;
    gboolean found;

    new_map = FALSE;

    found = FALSE;
    if (name)
    {
        for (lp = 0; lp < G_N_ELEMENTS(maps); lp++)
            if (g_ascii_strcasecmp(maps[lp].name, name) == 0)
            {
                found = TRUE;
                break;
            }
    }

    if (!found)
    {
        lp = 0;
        g_free(mapset);
        mapset = g_strdup(maps[0].name);
        /* We don't set the gconf key to avoid warning messages appearing
         * multiple times. Yes, I know this is a bad excuse. */
    }

    pos = maps[lp].map;

    generate_dependencies();
    calculate_view_geometry();
    configure_pixmaps();
}

static void
do_game(void)
{
    current_seed = next_seed;

    if (new_map)
        load_map();
    generate_game(current_seed);    /* puts in the positions of the tiles */
}

#if 0
/* Record any changes to our window size. */
static gboolean
window_configure_cb(GtkWidget * w, GdkEventConfigure * e, gpointer data)
{
    w = w;
    e = e;
    data = data;
    return FALSE;
}
#endif

static void
new_seed()
{
    struct timeval t;
    gettimeofday(&t, NULL);

    next_seed = (guint32) (t.tv_sec ^ t.tv_usec);
}

static void
set_score_file(gchar * mapset)
{
    guint i;

    /* FIXME: This is a bit ugly, but we only save the name of the map (and
     * it isn't suitable for generating the scorefile name. It's also a bit
     * close to code freeze to introduce gratuitous changes so this is it. */
    for (i = 0; i < G_N_ELEMENTS(maps); i++)
    {
        if (g_utf8_collate(mapset, maps[i].name) == 0)
            score_current_mapset = maps[i].score_name;
    }
}

void
new_game(gboolean re_seed)
{
    if (re_seed)
        new_seed();

    do_game();

    /* draw_all_tiles (); */

    init_game();

    set_score_file(mapset);

}

void
shuffle_tiles_callback(GtkWidget * widget, gpointer data)
{
    gboolean ok;

    widget = widget;
    data = data;

    if (timeout_counter <= HINT_BLINK_NUM)
    {
        tiles[hint_tiles[0]].selected &= ~HINT_FLAG;
        tiles[hint_tiles[1]].selected &= ~HINT_FLAG;
        timeout_counter = HINT_BLINK_NUM + 1;
    }


    if (paused || game_over == GAME_DEAD || game_over == GAME_WON)
        return;

    /* Make sure no tiles are selected. */
    if (selected_tile < MAX_TILES)
    {
        unselect_tile(selected_tile);
    }

    ok = shuffle();

    if (!ok)
    {
        // GtkWidget *mb;
        game_over = GAME_DEAD;
        games_clock_stop(GAMES_CLOCK(chrono));
    }
    else
    {

        draw_all_tiles();

        game_over = GAME_RUNNING;

        /* 60s penalty */
        games_clock_add_seconds(GAMES_CLOCK(chrono), PENALTY_SHUFFLE);

        update_moves_left();
        /* Disable undo/redo after a shuffle. */
        sequence_number = 1;
        clear_undo_queue();
        set_undoredo_sensitive(FALSE, FALSE);

        sound_play(get_app_data()->app_sound_data, SOUND_SHUFFLE);
    }
}

static void
set_hint_sensitive(void)
{
    /* Hint */
    if (moves_left > 0)
        update_hint(TRUE);
    else
        update_hint(FALSE);
}


GtkWidget *
init_mahjong(void)
{
    new_seed();
    chrono = games_clock_new();
    do_game();
    init_game();
    return chrono;
}

void
deinit_mahjong(void)
{
    if (tileset)
        g_free(tileset);
    if (mapset)
        g_free(mapset);

    degenerate_game();
}
