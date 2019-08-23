/* 
 * clock.c: 
 *
 * Copyright (C) 2001, 2003 Iain Holmes
 *           (C) 2001 Mark McLoughlin
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 * Authors: Iain Holmes <iain@ximian.com>
 *          Mark McLoughlin <mark@skynet.ie>
 */

#include <glib.h>

#include "games-clock.h"

static GtkLabelClass *parent_class = NULL;

static void
games_clock_finalize(GObject * object)
{
    GamesClock *clock;

    g_return_if_fail(object && GAMES_IS_CLOCK(object));

    clock = GAMES_CLOCK(object);

    if (clock->timer_id != -1)
    {
        g_source_remove(clock->timer_id);
        clock->timer_id = -1;
    }

    G_OBJECT_CLASS(parent_class)->finalize(object);
}

static void
games_clock_class_init(GamesClockClass * klass)
{
    GObjectClass *object_class = (GObjectClass *) klass;

    object_class->finalize = games_clock_finalize;

    parent_class = g_type_class_peek_parent(klass);
}

static void
games_clock_instance_init(GamesClock * clock)
{
    clock->timer_id = -1;
    clock->seconds = 0;

    gtk_label_set_text(GTK_LABEL(clock), "00:00");
}

GType
games_clock_get_type(void)
{
    static GType type = 0;

    if (!type)
    {
        GTypeInfo info = {
            sizeof(GamesClockClass),
            NULL,
            NULL,
            (GClassInitFunc) games_clock_class_init,
            NULL,
            NULL,
            sizeof(GamesClock),
            0,
            (GInstanceInitFunc) games_clock_instance_init,
            NULL
        };

        type = g_type_register_static(GTK_TYPE_LABEL, "GamesClock", &info, 0);
    }

    return type;
}

GtkWidget *
games_clock_new(void)
{
    GamesClock *clock;

    clock = g_object_new(games_clock_get_type(), NULL);

    return GTK_WIDGET(clock);
}

static void
clock_paint(GamesClock * clock)
{
    char *string;
    int secs;
    int mins;

    PangoAttribute *pa;
    PangoAttrList *pal;

    g_return_if_fail(clock && GAMES_IS_CLOCK(clock));

    /* hours = clock->seconds / 3600; secs = clock->seconds - hours*3600; */
    secs = clock->seconds;
    mins = secs / 60;
    secs = secs - mins * 60;

    // string = g_strdup_printf ( "%.2d:%.2d:%.2d", hours, mins, secs);
    string = g_strdup_printf("%.2d:%.2d", mins, secs);

    pa = pango_attr_size_new(CLOCK_TEXT_SIZE);
    pa->start_index = 0;
    pa->end_index = G_MAXINT;
    pal = pango_attr_list_new();
    pango_attr_list_insert(pal, pa);

    gtk_label_set_attributes(GTK_LABEL(clock), pal);
    gtk_label_set_text(GTK_LABEL(clock), string);

    g_free(string);
    pango_attr_list_unref(pal);
}


static gboolean
games_clock_update(GamesClock * clock)
{
    g_return_val_if_fail(clock && GAMES_IS_CLOCK(clock), FALSE);

    clock->seconds++;

    clock_paint(clock);

    return TRUE;
}

void
games_clock_start(GamesClock * clock)
{
    g_return_if_fail(clock && GAMES_IS_CLOCK(clock));

    if (clock->timer_id != -1)
        return;

    clock->timer_id = g_timeout_add(1000,
                                    (GSourceFunc) games_clock_update, clock);
}

void
games_clock_stop(GamesClock * clock)
{
    g_return_if_fail(clock && GAMES_IS_CLOCK(clock));

    if (clock->timer_id == -1)
        return;

    g_source_remove(clock->timer_id);
    clock->timer_id = -1;
    clock->stopped = clock->seconds;
}

void
games_clock_set_seconds(GamesClock * clock, time_t seconds)
{
    g_return_if_fail(clock && GAMES_IS_CLOCK(clock));

    clock->seconds = seconds;
    clock_paint(clock);
}

time_t
games_clock_get_seconds(GamesClock * clock)
{
    g_return_val_if_fail(clock && GAMES_IS_CLOCK(clock), 0);

    return clock->seconds;
}

void
games_clock_add_seconds(GamesClock * clock, time_t seconds)
{
    g_return_if_fail(clock && GAMES_IS_CLOCK(clock));

    clock->seconds += seconds;
    clock_paint(clock);
}
