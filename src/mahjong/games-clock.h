/* 
 * clock.h: Clock widget.
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

#ifndef __GAMES_CLOCK_H__
#define __GAMES_CLOCK_H__

#include <glib/gmacros.h>
#include <gtk/gtklabel.h>
#include <time.h>

G_BEGIN_DECLS
#define GAMES_TYPE_CLOCK            (games_clock_get_type ())
#define GAMES_CLOCK(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GAMES_TYPE_CLOCK, GamesClock))
#define GAMES_CLOCK_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), GAMES_TYPE_CLOCK, GamesClockClass))
#define GAMES_IS_CLOCK(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GAMES_TYPE_CLOCK))
#define GAMES_IS_CLOCK_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GAMES_TYPE_CLOCK))
#define CLOCK_TEXT_SIZE 20000
    typedef struct _GamesClock {
    GtkLabel label;

    gint timer_id;

    time_t seconds;
    time_t stopped;
} GamesClock;

typedef struct _GamesClockClass {
    GtkLabelClass parent_class;
} GamesClockClass;

/**
 Gets the type of the clock.
 @return The type.
*/
GType games_clock_get_type(void);
/**
 Creates new clock widget.
 @return The widget created.
*/
GtkWidget *games_clock_new(void);
/**
 Starts the clock.
 @param clock The clock to be started.
*/
void games_clock_start(GamesClock * clock);
/**
 Stops the clock.
 @param clock The clock to be stopped.
*/
void games_clock_stop(GamesClock * clock);

/**
 Sets the time on a clock.
 @param clock The clock who's time is to be set.
*/
void games_clock_set_seconds(GamesClock * clock, time_t seconds);

/**
 Gets the time from a clock.
 @param clock Clock to get the time from.
 @return The time of the clock.
*/
time_t games_clock_get_seconds(GamesClock * clock);

/**
 Adds clock's time.
 @param clock Clock to be added.
 @param seconds Time to be added to the clock.
*/
void games_clock_add_seconds(GamesClock * clock, time_t seconds);

G_END_DECLS
#endif /* __GAMES_CLOCK_H__ */
