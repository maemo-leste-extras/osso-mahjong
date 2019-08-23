/**
    @file hiscore.c
 
    Hiscore list header.

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

#ifndef HISCORE_H
#define HISCORE_H

#include <time.h>
#include "appdata.h"

typedef struct {
    char level;
    int score;
    time_t date;
} HiScore;

/**
 Reads highscores from file specified in gconf SETTINGS_MAHJONG_HIGH_SCORES
 @param scores Pointer to a pointer that stores the read HiScore array.
 @return Amount of high scores read.
*/
int hiscore_get(HiScore ** scores);

/**
 Get's certain level's score.
 @param scores Pointer to the HiScore array.
 @param count Size of HiScore array.
 @param level The level's identifier char.
 @return The score of the level..
*/
time_t hiscore_get_score(HiScore * scores, int count, char level);

/**
 Sets the score for certain level.
 @param scores Pointer to a pointer that stores the HiScore array.
 @param count Size of the HiScore array.
 @param level The level's identifier char.
 @param score The score for the level.
 @return New size of HiScore array.
*/
int hiscore_set(HiScore ** scores, int count, char level, int score);

/**
 Save highscores to file specified in gconf SETTINGS_MAHJONG_HIGH_SCORES
 @param scores Pointer to the HiScore array.
 @param count Size of HiScore array.
*/
void hiscore_put(HiScore * scores, int count, AppData * app_data);

#endif
