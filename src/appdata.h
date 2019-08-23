/**
    @file appdata.h

    Data structures for the whole application.

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

#ifndef APPDATA_H
#define APPDATA_H

/* GTK */
#include <gtk/gtk.h>

/* GConf */
#include <gconf/gconf.h>
#include <gconf/gconf-client.h>

/* OSSO */
#include <libosso.h>

/* Sounds */
#include "sounds.h"
#include <canberra.h>

#include "ui/mahjong_button.h"

/* Help context */
//#define MAHJONG_HELP_ENDGAME "//mahjong/a/2"

/* 
 * Application UI data
 * 
 * This structure should contain ALL application UI related data, which
 * otherwise would be impossible to pass with events to callbacks. It makes
 * complicated intercommunication between widgets possible.
 * 
 * So when you add a widget to ui, put a pointer to it inside this struct. */
typedef struct _AppUIData AppUIData;
struct _AppUIData {
    GtkWindow *window;
    GtkWidget *main_hbox;
    GtkWidget *board;
    GtkWidget *control;
    GtkWidget *control_back;
    GtkWidget *chrono;
    GtkDialog *dialog;
    struct MahjongButton *undo;
    struct MahjongButton *redo;
    struct MahjongButton *shuffle;
    struct MahjongButton *hint;
   struct MahjongButton *overlay; 
    
};

typedef struct _AppOSSOData AppOSSOData;
struct _AppOSSOData {
    osso_context_t *osso;
};

typedef struct _AppGConfData AppGConfData;
struct _AppGConfData {
    GConfClient *gc_client;
};

typedef struct _AppSoundData AppSoundData;
struct _AppSoundData {
    int sound_ids[SOUND_COUNT];
    
    ca_context *ca;
    ca_proplist *pl;
};

/* 
 * Application data structure. Pointer to this is passed eg. with UI event
 * callbacks. */
typedef struct _AppData AppData;
struct _AppData {
    AppUIData *app_ui_data;
    AppOSSOData *app_osso_data;
    AppGConfData *app_gconf_data;
    AppSoundData *app_sound_data;
};

#endif /* APPDATA_H */
