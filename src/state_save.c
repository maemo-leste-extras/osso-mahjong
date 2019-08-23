/**
    @file state_save.c

    Application state saving.

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

#include <glib.h>
#include <libosso.h>
#include <string.h>
#include "state_save.h"
#include "mahjong/games-clock.h"
#include "mahjong/mahjongg.h"

extern tile tiles[MAX_TILES];
extern gint selected_tile;
extern gint visible_tiles;
extern gint sequence_number;

typedef struct {
    gint playing_level;
    time_t time;
    tile tiles[MAX_TILES];
    gint sequence_number;
    gint selected_tile;
    gint visible_tiles;
    gchar level;
} StateData;

void
save_state(AppData * app_data)
{
    if (!app_data || !app_data->app_ui_data || !app_data->app_osso_data)
        return;

    StateData statedata;
    osso_state_t state;

    memcpy(statedata.tiles, tiles, sizeof(tile) * MAX_TILES);
    statedata.sequence_number = sequence_number;
    statedata.selected_tile = selected_tile;
    statedata.visible_tiles = visible_tiles;
    statedata.time =
        games_clock_get_seconds(GAMES_CLOCK(app_data->app_ui_data->chrono));
    statedata.level = mapset[strlen(mapset) - 1];

    state.state_data = (void *) &statedata;
    state.state_size = sizeof(statedata);

    osso_state_write(app_data->app_osso_data->osso, &state);
}

void
read_state(AppData * app_data)
{
    if (!app_data || !app_data->app_ui_data || !app_data->app_osso_data)
        return;

    StateData statedata;
    osso_state_t state;
    int i;

    state.state_data = (void *) &statedata;
    state.state_size = sizeof(statedata);

    if (osso_state_read(app_data->app_osso_data->osso, &state) != OSSO_OK)
        return;

    memcpy(tiles, statedata.tiles, sizeof(tile) * MAX_TILES);
    sequence_number = statedata.sequence_number;
    selected_tile = statedata.selected_tile;
    visible_tiles = statedata.visible_tiles;
    if (mapset)
        g_free(mapset);
    mapset =
        g_strdup_printf("game_fi_mahjong_game_board_%c", statedata.level);
    games_clock_set_seconds(GAMES_CLOCK(app_data->app_ui_data->chrono),
                            statedata.time);
    load_map();
    update_undo_redo(sequence_number > 1, FALSE);
    for (i = 0; i < MAX_TILES; i++)
        if (tiles[i].sequence == sequence_number)
        {
            update_undo_redo(sequence_number > 1, TRUE);
            break;
        }
    check_free();

    update_hint(moves_left > 0);
}
