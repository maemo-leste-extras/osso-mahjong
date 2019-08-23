/**
    @file hiscore.c

    Hiscore list functionality.

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

#include "hiscore.h"
#include "settings.h"
#include <glib.h>
#include <glib/gstdio.h>

#include <string.h>
#include <libgnomevfs/gnome-vfs.h>
#include <libintl.h>

#include <hildon/hildon-banner.h>

int
hiscore_get(HiScore ** scores)
{
    if (!scores)
        return 0;

    gchar *filename = settings_get_string(SETTINGS_HIGH_SCORES);
    if (!filename)
        return 0;

    if (!gnome_vfs_initialized())
        gnome_vfs_init();

    GnomeVFSHandle *handle;
    GString *scorestr = g_string_new("");

    /* Try to open the file */
    if (gnome_vfs_open(&handle, filename, GNOME_VFS_OPEN_READ) ==
        GNOME_VFS_OK)
    {

        static const GnomeVFSFileSize DATA_BUFFER_SIZE = 1024;
        GnomeVFSFileSize cnt;
        GnomeVFSResult res = 0;
        gchar data[DATA_BUFFER_SIZE + 1];

        /* Read file contents */
        do
        {
            res = gnome_vfs_read(handle, (gpointer) data,
                                 DATA_BUFFER_SIZE, &cnt);

            if (cnt > 0)
            {
                g_assert(cnt <= DATA_BUFFER_SIZE);

                data[cnt] = 0;
                g_string_append(scorestr, data);
            }
        } while (res == GNOME_VFS_OK && cnt > 0);

        gnome_vfs_close(handle);
    }

    char this_level;
    gchar *chardata = scorestr->str;
    int this_score;
    int this_date;
    int count = 0;
    while (chardata && *chardata)
    {
        if (sscanf(chardata, "%d %d %c", &this_score, &this_date, &this_level)
            < 3)
            continue;
        count++;
        chardata = strchr(chardata, '\n');
        if (chardata)
            chardata++;
        else
            break;
    }

    chardata = scorestr->str;

    if (count)
        *scores = g_new0(HiScore, count);
    else
    {
        *scores = NULL;
        g_free(filename);

        return count;
    }

    count = 0;

    while (chardata && *chardata)
    {
        if (sscanf(chardata, "%d %d %c", &this_score, &this_date, &this_level)
            < 3)
            continue;
        (*scores)[count].level = this_level;
        (*scores)[count].score = this_score;
        (*scores)[count].date = this_date;
        count++;
        chardata = strchr(chardata, '\n');
        if (chardata)
            chardata++;
        else
            break;
    }

    g_free(filename);
    g_string_free(scorestr, 1);

    return count;
}

time_t
hiscore_get_score(HiScore * scores, int count, char level)
{
    int i;

    if (!scores)
        return 0;

    for (i = 0; i < count; i++)
        if (scores[i].level == level)
            return scores[i].score;
    return 0;
}


int
hiscore_set(HiScore ** scores, int count, char level, int score)
{
    int i, j;
    HiScore *newscores = NULL;
    if (!scores)
        return 0;
    if (count == 0)
    {
        count++;
        *scores = g_new0(HiScore, count);
        (*scores)->level = level;
        (*scores)->date = time(NULL);
        (*scores)->score = score;
        return count;
    }
    for (i = 0; i < count; i++)
        if ((*scores)[i].level == level)
        {
            (*scores)[i].score = score;
            (*scores)[i].date = time(NULL);
            break;
        }
    if (i == count)
    {
        count++;
        newscores = g_new0(HiScore, count);
        for (i = 0, j = 0; j < count; i++, j++)
        {
            if (i == j && (i == count - 1 || (*scores)[i].level > level))
            {
                newscores[j].level = level;
                newscores[j].score = score;
                newscores[j].date = time(NULL);
                j++;
                if (j == count)
                    break;
            }
            newscores[j].level = (*scores)[i].level;
            newscores[j].score = (*scores)[i].score;
            newscores[j].date = (*scores)[i].date;
        }
        g_free(*scores);
        *scores = newscores;
    }
    return count;
}

void
hiscore_put(HiScore * scores, int count, AppData * app_data)
{
    int i;

    if (count && !scores)
        return;

    gchar *filename = settings_get_string(SETTINGS_HIGH_SCORES);
    gchar *filename_temp = g_strconcat(filename,"~",(char *)NULL);
    if (!filename)
        return;

    if (!gnome_vfs_initialized())
        gnome_vfs_init();

    GnomeVFSHandle *handle;
    GnomeVFSResult file_method_result;
    GString *scorestr = g_string_new("");

    file_method_result =
        gnome_vfs_create(&handle, filename_temp, GNOME_VFS_OPEN_WRITE, FALSE,
                         0644);

    /* Try to open the file */
    if (file_method_result == GNOME_VFS_OK)
    {

        GnomeVFSFileSize cnt;
        GnomeVFSResult res = GNOME_VFS_OK;

        /* Write file contents */
        for (i = 0; i < count; i++)
        {
            g_string_printf(scorestr, "%d %ld %c\n", scores[i].score,
                            (unsigned long) scores[i].date, scores[i].level);
            res =
                gnome_vfs_write(handle, (gpointer) scorestr->str,
                                scorestr->len, &cnt);
            if (res != GNOME_VFS_OK)
            {
                break;
            }
        }
        if (res != GNOME_VFS_OK)
        {
            hildon_banner_show_information(
                GTK_WIDGET(app_data->app_ui_data->window),
                NULL, dgettext("ke-recv", "cerm_device_memory_full"));
        }

        gnome_vfs_close(handle);
        if (res == GNOME_VFS_OK) {
            g_unlink(filename);
            g_rename(filename_temp,filename);
        }
    }
    else
    {
        if (file_method_result == GNOME_VFS_ERROR_NO_SPACE)
        {
            hildon_banner_show_information(GTK_WIDGET
                (app_data->app_ui_data->window),
                NULL, dgettext("ke-recv", "cerm_device_memory_full"));
        }
    }

    if (filename != NULL) {
        g_free(filename);
        filename = NULL;
    }
    
    if (filename_temp!=NULL) {
        g_free(filename_temp);
        filename_temp = NULL;
    }
    g_string_free(scorestr, TRUE);
}
