/**
    @file main.h

    Prototypes for main

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

#ifndef MAIN_HH
#define MAIN_HH

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <locale.h>
#include <gtk/gtk.h>
#include <signal.h>
#include <osso-log.h>
#include <log-functions.h>

#include "mahjong_i18n.h"
#include "ui/interface.h"
#include "dbus.h"
#include "settings.h"
#include "appdata.h"
#include "mahjong_core.h"

/**
 Handles signals, ensure everything gets deinitted
 @param sig Signal
 */
void sig_handler(int sig);

#endif
