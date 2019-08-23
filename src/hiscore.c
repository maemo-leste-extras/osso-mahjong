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
#include <gio/gio.h>
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

    GFile *handle;
    GFileInputStream *istream;
    GError *err = NULL;
    GString *scorestr = g_string_new("");

    handle = g_file_new_for_path (filename);
    istream = g_file_read (handle, NULL, &err);

    if (err != NULL)
    {
        g_error ("Could not open %s for reading: %s\n", filename, err->message);
        g_error_free (err);
    }
    else
    {
        static const guint64 DATA_BUFFER_SIZE = 1024;
        gboolean  success;
        gchar buffer[DATA_BUFFER_SIZE + 1];
        gsize bytes_read;
        
        success = g_input_stream_read_all (G_INPUT_STREAM(istream), buffer, sizeof (buffer), &bytes_read, NULL, &err);

        //Might need \0 added first
        if (bytes_read > 0)
          g_string_append_len (scorestr, buffer, bytes_read);

        if (!success)
	{
	    if (scorestr->len > 0)
            {
	        g_printerr ("%s: read error in '%s', trying to scan "
	                         "partial content: %s",
	                          G_STRFUNC, filename, err->message);
	        g_clear_error (&err);
            }
        }
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
    g_input_stream_close(G_INPUT_STREAM(istream),NULL,NULL);
    g_object_unref(istream);
    g_object_unref (G_OBJECT (handle));

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

    GFile *handle;
    GFileOutputStream *ostream;
    GError *err = NULL;
    GString *scorestr = g_string_new("");

    handle = g_file_new_for_path (filename_temp);
    ostream = g_file_replace (handle, NULL, FALSE, G_FILE_CREATE_NONE, NULL, &err);

    if (err != NULL)
    {
        //TODO: Add Maemo storage full notification if needed.
        //if (file_method_result == GNOME_VFS_ERROR_NO_SPACE)
        //{
        //    hildon_banner_show_information(GTK_WIDGET
        //        (app_data->app_ui_data->window),
        //        NULL, dgettext("ke-recv", "cerm_device_memory_full"));
        //}
        g_error ("Could not open %s for writing: %s\n", filename_temp, err->message);
        g_error_free (err);
    }
    else
    {
        gsize cnt;
        gboolean  success;
       /* Write file contents */
        for (i = 0; i < count; i++)
        {
            g_string_printf(scorestr, "%d %ld %c\n", scores[i].score,
                            (unsigned long) scores[i].date, scores[i].level);
            success = g_output_stream_write_all (G_OUTPUT_STREAM(ostream), (gpointer) scorestr->str, scorestr->len, &cnt, NULL, NULL);
            if (!success)
            {
                break;
            }
        }
        if (!success)
        {
            hildon_banner_show_information(
                GTK_WIDGET(app_data->app_ui_data->window),
                NULL, dgettext("ke-recv", "cerm_device_memory_full"));
        }
	g_output_stream_close(G_OUTPUT_STREAM(ostream),NULL,NULL);
	g_object_unref(ostream);
        g_object_unref (G_OBJECT (handle));
        if (success) {
            g_unlink(filename);
            g_rename(filename_temp,filename);
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
