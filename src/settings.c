/**
    @file settings.c

    Implementation of Settings component. Access to GConf and
    possibility to read and change key values.

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

#include "settings.h"

/* Global variable for the client, however visibility only inside this
 * module. I prefer this than passing the AppData structure to every
 * function. */
GConfClient *gc_client = NULL;

/* Set key value */
gboolean
settings_set(const gchar * key, const GConfValue * value)
{
    GError *err = NULL;

    g_assert(gc_client);
    g_assert(key);

    if (!gc_client || !key || !value)
        return FALSE;

    /* Set key value */
    osso_log(LOG_DEBUG, "Setting key %s value\n", key);
    gconf_client_set(gc_client, key, value, &err);

    /* Check errors */
    if (err != NULL)
    {
        osso_log(LOG_ERR, "Settings set failed: %s", err->message);
        g_error_free(err);
        return FALSE;
    }

    return TRUE;
}

/* Get key value */
GConfValue *
settings_get(const gchar * key)
{
    if (!key)
        return NULL;

    osso_log(LOG_DEBUG, "Getting key %s value\n", key);
    return gconf_client_get(gc_client, key, NULL);
}

/* Set int type value */
gboolean
settings_set_int(const gchar * key, const gint val)
{
    if (!gc_client || !key)
        return FALSE;
    osso_log(LOG_DEBUG, "Setting key %s value to %i\n", key, val);
    return gconf_client_set_int(gc_client, key, val, NULL);
}

/* Set string type value */
gboolean
settings_set_string(const gchar * key, const gchar * val)
{
    if (!gc_client || !key || !val)
        return FALSE;
    osso_log(LOG_DEBUG, "Setting key %s value to %s\n", key, val);
    return gconf_client_set_string(gc_client, key, val, NULL);
}

/* Set float type value */
gboolean
settings_set_float(const gchar * key, const gdouble val)
{
    if (!gc_client || !key)
        return FALSE;
    osso_log(LOG_DEBUG, "Setting key %s value to %f\n", key, val);
    return gconf_client_set_float(gc_client, key, val, NULL);
}

/* Set boolean type value */
gboolean
settings_set_bool(const gchar * key, const gboolean val)
{
    if (!gc_client || !key)
        return FALSE;
    osso_log(LOG_DEBUG, "Setting key %s value to %i\n", key, val);
    return gconf_client_set_bool(gc_client, key, val, NULL);
}

/* Get int type key value */
gint
settings_get_int(const gchar * key)
{
    if (!gc_client || !key)
        return 0;
    osso_log(LOG_DEBUG, "Getting key %s value as int\n", key);
    return gconf_client_get_int(gc_client, key, NULL);
}

/* Get float type key value */
gdouble
settings_get_float(const gchar * key)
{
    if (!gc_client || !key)
        return 0.;
    osso_log(LOG_DEBUG, "Getting key %s value as float\n", key);
    return gconf_client_get_float(gc_client, key, NULL);
}

/* Get string type key value */
gchar *
settings_get_string(const gchar * key)
{
    if (!gc_client || !key)
        return NULL;
    osso_log(LOG_DEBUG, "Getting key %s value as string\n", key);
    return gconf_client_get_string(gc_client, key, NULL);
}

/* Get boolean type key value */
gboolean
settings_get_bool(const gchar * key)
{
    if (!gc_client || !key)
        return FALSE;
    osso_log(LOG_DEBUG, "Getting key %s value as boolean\n", key);
    return gconf_client_get_bool(gc_client, key, NULL);
}

/* Notify for changes in key/folder */
guint
settings_notify_add(const gchar * key, GConfClientNotifyFunc func)
{
    GError *err;

    if (!gc_client || !key || !func)
        return 0;

    /* Add a notify */
    osso_log(LOG_DEBUG, "Adding notify to key %s\n", key);
    guint id = gconf_client_notify_add(gc_client, key, func,
                                       NULL, NULL, &err);

    /* Check error */
    if (err != NULL)
    {
        osso_log(LOG_ERR, "Settings notify addition failed: %s",
                 err->message);
        g_error_free(err);
        return 0;
    }

    return id;
}

/* Remove notify about changes */
void
settings_notify_remove(guint id)
{
    osso_log(LOG_DEBUG, "Removing notify %i\n", id);
    gconf_client_notify_remove(gc_client, id);
}

/* Init the gconf settings object */
gboolean
init_settings(AppData * app_data)
{
    if (app_data == NULL || app_data->app_gconf_data == NULL)
    {
        return FALSE;
    }

    g_assert(app_data);

    /* Init type system */
    g_type_init();

    /* Init the client */
    osso_log(LOG_DEBUG, "Initializing GConf\n");
    app_data->app_gconf_data->gc_client = gconf_client_get_default();
    gc_client = app_data->app_gconf_data->gc_client;

    if (gc_client == NULL)
    {
        return FALSE;
    }

    g_assert(gc_client);

    return TRUE;
}

/* Deinit gconf settings object */
gboolean
deinit_settings(AppData * app_data)
{
    (void) app_data;
    if (gc_client == NULL)
    {
        return FALSE;
    }

    g_assert(gc_client);

    osso_log(LOG_DEBUG, "Deinitializing GConf\n");
    g_object_unref(gc_client);
    gc_client = NULL;

    return TRUE;
}
