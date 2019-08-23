/**
   @file plugin.c

   Osso Mahjong plugin functions.

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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libintl.h>
#include <gtk/gtk.h>
#include <gconf/gconf.h>
#include <gconf/gconf-client.h>
#include <gdk/gdkkeysyms.h>
#include <hildon/hildon-caption.h>
#include <hildon/hildon-banner.h>
#include <gio/gio.h>
#include <startup_plugin.h>

#define OSSO_MAHJONG_HELP_PATH "Applications_mahjong_startupview"
#define OSSO_MAHJONG_HELP_BESTTIMES "Applications_mahjong_highscores"

#define SETTINGS_DIFFICULTY     "/apps/osso/mahjong/board_layout"
#define SETTINGS_SOUND          "/apps/osso/mahjong/enable_sound"

#define MAHJONG_BOARD_1         0
#define MAHJONG_BOARD_2         1
#define MAHJONG_BOARD_3         2
#define MAHJONG_BOARD_4         3
#define MAHJONG_BOARD_5         4
#define MAHJONG_BOARD_6         5
#define MAHJONG_BOARD_7         6
#define MAHJONG_BOARD_8         7
#define MAHJONG_SOUND           8
#define MAHJONG_HIGH_SCORES     9
#define STARTUP_UNTOP 50
#define COMBOBOX_WIDTH 250
#define COMBOBOX_HEIGHT -1
#define MA_GAME_PLAYING_START 30
#define MA_GAME_PLAYING 31
#define MA_GAME_RESET_OLD_SETTINGS 35
#define MA_GAME_ENDED 37
#define MA_GAME_RESTORE_LAST_SETTINGS 38

#define HIGHSCORE_SIZE 10
#define HIGHSCORE_COLS 3
#define SETTINGS_MAHJONG_HIGH_SCORES "/apps/osso/mahjongg/scores"

#define BEST_TIME_HIGH 8
#define BEST_TIME_LOW 0


enum {
    HIGHSCORE_LEVEL_COLUMN,
    HIGHSCORE_TIME_COLUMN,
    HIGHSCORE_DATE_COLUMN
};

struct HighScore {
    gchar *level;
    gchar *time;
    gchar *date;
};

gint board_for_next_game = -1;
gchar *file = NULL;
GConfClient *gcc = NULL;
GtkWidget *sound_check = NULL;
GtkWidget *board_box = NULL;
GtkWidget *mahjong_scores_button = NULL;
GtkDialog *dialog = NULL;

#define _(String) dgettext("osso-games", String)
#define __(String) dgettext("osso-games", String)


static GtkWidget *load_plugin(void);
static void unload_plugin(void);
static void write_config(void);
static GtkWidget **load_menu(guint *);
static void update_menu(void);

static void mahjong_sound_cb(GtkWidget * widget, gpointer data);
static void mahjong_board_cb(GtkWidget * widget, gpointer data);
static void plugin_callback(GtkWidget *, gpointer data);
static void ui_show_highscore(GtkWidget * mahjong_tree_model);
static GtkTreeModel *create_highscore_list(struct HighScore *scores);
static void mahjongg_scores_cb(GtkWidget * widget, gpointer data);
static GConfValue *plugin_settings_get(const gchar * key);
static gboolean plugin_settings_get_bool(const gchar * key);
static gboolean plugin_settings_get_bool_fallback(gchar * key, gboolean fall);
/*static void plugin_game_help_cb(GtkMenuItem * menuitem, gpointer user_data);*/
static GString *plugin_highscore_read_file(void);
static void plugin_highscore_load(struct HighScore *score);
static void set_all_insensitive(void);
static void set_all_sensitive(void);
static void plugin_show_infoprint(gchar * msg);
static void plugin_restore_original_settings(void);
static void plugin_destroy_highscore_dialog(void);
static void plugin_highscore_response(GtkDialog * dialog, gint arg1,
                                      gpointer user_data);
static gboolean plugin_highscore_key_release(GtkWidget * widget,
                                             GdkEventKey * event,
                                             gpointer user_data);
static void plugin_restore_requested_settings(void);


static int changed = FALSE;
GameStartupInfo gs;
static GtkWidget *menu_items[3];
GtkWidget *help_item;

gboolean score_dialog_displayed = FALSE;

static StartupPluginInfo plugin_info = {
    load_plugin,
    unload_plugin,
    write_config,
    load_menu,
    update_menu,
    plugin_callback,
    NULL
};

STARTUP_INIT_PLUGIN(plugin_info, gs, FALSE, FALSE)
  static void plugin_show_infoprint(gchar * msg)
{

    hildon_banner_show_information(GTK_WIDGET(gs.ui->hildon_appview), NULL,
                                   msg);
}

/* Get key value */
static GConfValue *
plugin_settings_get(const gchar * key)
{
    return gconf_client_get(gcc, key, NULL);
}

/* Get boolean type key value */
static gboolean
plugin_settings_get_bool(const gchar * key)
{
    return gconf_client_get_bool(gcc, key, NULL);
}


/**
 Helper function to get an bool entry, returning defined value, if not found.
 @param key GConf key to be get.
 @param fall Fallback to this, if not found, or invalid.
 @return Value got from GConf or the value specified in fall.
*/
static gboolean
plugin_settings_get_bool_fallback(gchar * key, gboolean fall)
{
    GConfValue *gc_val = plugin_settings_get(key);

    if (gc_val)
    {
        if (gc_val->type == GCONF_VALUE_BOOL)
        {
            gconf_value_free(gc_val);
            return plugin_settings_get_bool(key);
        }
        gconf_value_free(gc_val);
    }
    return fall;
}



static GtkWidget *
load_plugin(void)
{
    guint xoptions = GTK_EXPAND | GTK_FILL;
    guint yoptions = GTK_EXPAND;    
    const gchar *tmp;
    gchar *path;
    gint sound, board;
    GtkWidget *game_table;
    GtkWidget *board_label;
    GtkWidget *sound_label;
    GtkWidget *alignment;
    
    gcc = gconf_client_get_default();
    sound = plugin_settings_get_bool_fallback(SETTINGS_SOUND, TRUE);
    board = gconf_client_get_int(gcc, SETTINGS_DIFFICULTY, NULL);
    tmp = gconf_client_get_string(gcc, SETTINGS_MAHJONG_HIGH_SCORES, NULL);
    if (!tmp)
    {
        tmp = (gchar *) g_get_home_dir();
        path = g_build_filename(tmp, ".mahjong_scores", NULL);
        gconf_client_set_string(gcc, SETTINGS_MAHJONG_HIGH_SCORES, path,
                                NULL);
        file = g_strdup(path);
        g_free(path);
    }
    else
    {
        file = g_strdup(tmp);
    }
    g_free((gpointer *) tmp);
    
    game_table = gtk_table_new (2, 2, FALSE);
    g_assert(game_table);

    /* Board combo box */
    board_box = gtk_combo_box_new_text();
    g_assert(board_box);

    gtk_combo_box_append_text(GTK_COMBO_BOX(board_box),
                              __("game_va_mahjong_settings_board_1"));
    gtk_combo_box_append_text(GTK_COMBO_BOX(board_box),
                              __("game_va_mahjong_settings_board_2"));
    gtk_combo_box_append_text(GTK_COMBO_BOX(board_box),
                              __("game_va_mahjong_settings_board_3"));
    gtk_combo_box_append_text(GTK_COMBO_BOX(board_box),
                              __("game_va_mahjong_settings_board_4"));
    gtk_combo_box_append_text(GTK_COMBO_BOX(board_box),
                              __("game_va_mahjong_settings_board_5"));
    gtk_combo_box_append_text(GTK_COMBO_BOX(board_box),
                              __("game_va_mahjong_settings_board_6"));
    gtk_combo_box_append_text(GTK_COMBO_BOX(board_box),
                              __("game_va_mahjong_settings_board_7"));
    gtk_combo_box_append_text(GTK_COMBO_BOX(board_box),
                              __("game_va_mahjong_settings_board_8"));

    gtk_combo_box_set_active(GTK_COMBO_BOX(board_box), board);

    gtk_widget_set_size_request(board_box, COMBOBOX_WIDTH, -1);

    /* Sound check button */
    sound_check = gtk_check_button_new();
    g_assert(sound_check);

    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(sound_check), sound);

    board_label =
        hildon_caption_new(NULL, __("game_fi_mahjong_settings_board"),
                           board_box, NULL, HILDON_CAPTION_OPTIONAL);
    sound_label =
        hildon_caption_new(NULL, __("game_fi_mahjong_settings_sound"),
                           sound_check, NULL, HILDON_CAPTION_OPTIONAL);
    
    /* High scores button */
    mahjong_scores_button =
        gtk_button_new_with_label(__("game_bu_mahjong_settings_high_scores"));
    
    hildon_gtk_widget_set_theme_size (mahjong_scores_button, HILDON_SIZE_FINGER_HEIGHT);

    g_assert(mahjong_scores_button);

    alignment = gtk_alignment_new (1.0f, 1.0f, 0.0f, 0.0f);

    gtk_container_add(GTK_CONTAINER(alignment), mahjong_scores_button);

    gtk_table_attach (GTK_TABLE(game_table), sound_label,
		      0, 1, 0, 1,
		      xoptions, yoptions,
		      0, 0);

    gtk_table_attach (GTK_TABLE(game_table), board_label,
		      0, 1, 1, 2,
		      xoptions, yoptions,
		      0, 0);

    gtk_table_attach (GTK_TABLE(game_table), alignment,
		      1, 2, 0, 2,
		      xoptions, GTK_EXPAND | GTK_FILL,
		      0, 0);
    
    g_signal_connect(G_OBJECT(board_box), "changed",
                     G_CALLBACK(mahjong_board_cb), NULL);

    g_signal_connect(G_OBJECT(sound_check), "clicked",
                     G_CALLBACK(mahjong_sound_cb), NULL);

    g_signal_connect(G_OBJECT(mahjong_scores_button), "clicked",
                     G_CALLBACK(mahjongg_scores_cb), NULL);

    return game_table;
}

static void
unload_plugin(void)
{
    g_object_unref(gcc);
    g_free(file);
}

static void
write_config(void)
{
    gconf_client_set_bool(gcc, SETTINGS_SOUND,
                          gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON
                                                       (sound_check)), NULL);
    gconf_client_set_int(gcc, SETTINGS_DIFFICULTY,
                         gtk_combo_box_get_active(GTK_COMBO_BOX(board_box)),
                         NULL);
}

GtkWidget *board_rd[8], *sound_ck;

/* Rama - Bug#100799 */
#if 0
static void
plugin_game_help_cb(GtkMenuItem * menuitem, gpointer user_data)
{
    StartupApp *app = NULL;
    user_data = NULL;
    menuitem = NULL;
    app = gs.ui->app;
    hildon_help_show(app->osso, OSSO_MAHJONG_HELP_PATH, 0);
}
#endif

static GtkWidget **
load_menu(guint * nitems)
{
    GSList *group = NULL;
    GtkWidget *sub_menu, *high_scores;
    *nitems = 3;

    menu_items[0] =
        gtk_menu_item_new_with_label(__("game_me_mahjong_main_menu_board"));
    sub_menu = gtk_menu_new();
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(menu_items[0]), sub_menu);

    board_rd[0] =
        gtk_radio_menu_item_new_with_label(group,
                                           __
                                           ("game_me_mahjong_menu_board_1"));
    group = gtk_radio_menu_item_group(GTK_RADIO_MENU_ITEM(board_rd[0]));
    gtk_menu_append(GTK_MENU(sub_menu), board_rd[0]);
    g_signal_connect(G_OBJECT(board_rd[0]), "toggled",
                     G_CALLBACK(plugin_callback), (gpointer) MAHJONG_BOARD_1);

    board_rd[1] =
        gtk_radio_menu_item_new_with_label(group,
                                           __
                                           ("game_me_mahjong_menu_board_2"));
    group = gtk_radio_menu_item_group(GTK_RADIO_MENU_ITEM(board_rd[1]));
    gtk_menu_append(GTK_MENU(sub_menu), board_rd[1]);
    g_signal_connect(G_OBJECT(board_rd[1]), "toggled",
                     G_CALLBACK(plugin_callback), (gpointer) MAHJONG_BOARD_2);

    board_rd[2] =
        gtk_radio_menu_item_new_with_label(group,
                                           __
                                           ("game_me_mahjong_menu_board_3"));
    group = gtk_radio_menu_item_group(GTK_RADIO_MENU_ITEM(board_rd[2]));
    gtk_menu_append(GTK_MENU(sub_menu), board_rd[2]);
    g_signal_connect(G_OBJECT(board_rd[2]), "toggled",
                     G_CALLBACK(plugin_callback), (gpointer) MAHJONG_BOARD_3);

    board_rd[3] =
        gtk_radio_menu_item_new_with_label(group,
                                           __
                                           ("game_me_mahjong_menu_board_4"));
    group = gtk_radio_menu_item_group(GTK_RADIO_MENU_ITEM(board_rd[3]));
    gtk_menu_append(GTK_MENU(sub_menu), board_rd[3]);
    g_signal_connect(G_OBJECT(board_rd[3]), "toggled",
                     G_CALLBACK(plugin_callback), (gpointer) MAHJONG_BOARD_4);

    board_rd[4] =
        gtk_radio_menu_item_new_with_label(group,
                                           __
                                           ("game_me_mahjong_menu_board_5"));
    group = gtk_radio_menu_item_group(GTK_RADIO_MENU_ITEM(board_rd[4]));
    gtk_menu_append(GTK_MENU(sub_menu), board_rd[4]);
    g_signal_connect(G_OBJECT(board_rd[4]), "toggled",
                     G_CALLBACK(plugin_callback), (gpointer) MAHJONG_BOARD_5);

    board_rd[5] =
        gtk_radio_menu_item_new_with_label(group,
                                           __
                                           ("game_me_mahjong_menu_board_6"));
    group = gtk_radio_menu_item_group(GTK_RADIO_MENU_ITEM(board_rd[5]));
    gtk_menu_append(GTK_MENU(sub_menu), board_rd[5]);
    g_signal_connect(G_OBJECT(board_rd[5]), "toggled",
                     G_CALLBACK(plugin_callback), (gpointer) MAHJONG_BOARD_6);

    board_rd[6] =
        gtk_radio_menu_item_new_with_label(group,
                                           __
                                           ("game_me_mahjong_menu_board_7"));
    group = gtk_radio_menu_item_group(GTK_RADIO_MENU_ITEM(board_rd[6]));
    gtk_menu_append(GTK_MENU(sub_menu), board_rd[6]);
    g_signal_connect(G_OBJECT(board_rd[6]), "toggled",
                     G_CALLBACK(plugin_callback), (gpointer) MAHJONG_BOARD_7);

    board_rd[7] =
        gtk_radio_menu_item_new_with_label(group,
                                           __
                                           ("game_me_mahjong_menu_board_8"));
    //group = gtk_radio_menu_item_group(GTK_RADIO_MENU_ITEM(board_rd[7]));
    //CID 8844
    gtk_radio_menu_item_group(GTK_RADIO_MENU_ITEM(board_rd[7]));
    gtk_menu_append(GTK_MENU(sub_menu), board_rd[7]);
    g_signal_connect(G_OBJECT(board_rd[7]), "toggled",
                     G_CALLBACK(plugin_callback), (gpointer) MAHJONG_BOARD_8);

    menu_items[1] =
        gtk_menu_item_new_with_label(__
                                     ("game_me_mahjong_main_menu_settings"));
    sub_menu = gtk_menu_new();
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(menu_items[1]), sub_menu);

    sound_ck =
        gtk_check_menu_item_new_with_label(__
                                           ("game_me_mahjong_menu_settings_sound"));
    gtk_menu_append(GTK_MENU(sub_menu), sound_ck);
    g_signal_connect(G_OBJECT(sound_ck), "toggled",
                     G_CALLBACK(plugin_callback), (gpointer) MAHJONG_SOUND);

    menu_items[2] =
        gtk_menu_item_new_with_label(__("game_me_mahjong_main_menu_tools"));
    sub_menu = gtk_menu_new();
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(menu_items[2]), sub_menu);

    high_scores =
        gtk_menu_item_new_with_label(__
                                     ("game_me_mahjong_menu_tools_high_scores"));
    gtk_menu_append(GTK_MENU(sub_menu), high_scores);
    g_signal_connect(G_OBJECT(high_scores), "activate",
                     G_CALLBACK(plugin_callback),
                     (gpointer) MAHJONG_HIGH_SCORES);

    /* Rama - Bug#100799 - Removed hepl from main menu */
    return menu_items;
}

static void
mahjongg_scores_cb(GtkWidget * widget, gpointer data)
{
    widget = NULL;
    data = NULL;
    struct HighScore scores[HIGHSCORE_SIZE];

    if(!score_dialog_displayed) {
	    score_dialog_displayed = TRUE;
	    plugin_highscore_load((struct HighScore *) &scores);
	    ui_show_highscore((GtkWidget *) create_highscore_list(scores));
    }
}

static void
set_all_insensitive(void)
{
    gtk_widget_set_sensitive(sound_check, FALSE);
    gtk_widget_set_sensitive(board_box, FALSE);
    gtk_widget_set_sensitive(mahjong_scores_button, FALSE);
    if (GTK_WIDGET_VISIBLE(gs.ui->play_button))
    {
        gtk_widget_set_sensitive(gs.ui->play_button, FALSE);
    }
    if (GTK_WIDGET_VISIBLE(gs.ui->restart_button))
    {
        gtk_widget_set_sensitive(gs.ui->restart_button, FALSE);
    }

}

static void
set_all_sensitive(void)
{
    gtk_widget_set_sensitive(sound_check, TRUE);
    gtk_widget_set_sensitive(board_box, TRUE);
    gtk_widget_set_sensitive(mahjong_scores_button, TRUE);
}


static void
plugin_restore_requested_settings(void)
{
    gint original_settings =
        gconf_client_get_int(gcc, SETTINGS_DIFFICULTY, NULL);
    gint current_value = gtk_combo_box_get_active(GTK_COMBO_BOX(board_box));
    if (original_settings != current_value)
        board_for_next_game = current_value;
    
    if (board_for_next_game != -1)
    {
        gtk_combo_box_set_active(GTK_COMBO_BOX(board_box),
                                 board_for_next_game);
        board_for_next_game = -1;
    }
    return;
}

static void
plugin_restore_original_settings(void)
{
    gint original_settings =
        gconf_client_get_int(gcc, SETTINGS_DIFFICULTY, NULL);
    gint current_value = gtk_combo_box_get_active(GTK_COMBO_BOX(board_box));
    if (original_settings != current_value)
    {
        board_for_next_game = current_value;
        plugin_show_infoprint(_("game_ib_changes_effect_next_game"));
        gtk_combo_box_set_active(GTK_COMBO_BOX(board_box), original_settings);
    }

}

static void
plugin_callback(GtkWidget * widget, gpointer data)
{

    switch ((int) data)
    {
        case MAHJONG_BOARD_1:
        case MAHJONG_BOARD_2:
        case MAHJONG_BOARD_3:
        case MAHJONG_BOARD_4:
        case MAHJONG_BOARD_5:
        case MAHJONG_BOARD_6:
        case MAHJONG_BOARD_7:
        case MAHJONG_BOARD_8:
            if (!changed)
            {
                changed = TRUE;
                gtk_combo_box_set_active(GTK_COMBO_BOX(board_box),
                                         (int) data);
                changed = FALSE;
            }
            break;

        case MAHJONG_SOUND:
            if (!changed)
            {
                changed = TRUE;
                gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(sound_check),
                                             gtk_check_menu_item_get_active
                                             (GTK_CHECK_MENU_ITEM(sound_ck)));
                changed = FALSE;
            }
            break;

        case MAHJONG_HIGH_SCORES:
            mahjongg_scores_cb(widget, NULL);
            break;
        case STARTUP_UNTOP:
        {
            break;
        }
        case MA_GAME_PLAYING_START:
        {
            set_all_insensitive();
            break;
        }
        case MA_GAME_PLAYING:
        {
            set_all_sensitive();
            break;
        }
        case MA_GAME_RESET_OLD_SETTINGS:
        {
            plugin_restore_original_settings();
            break;
        }
        case MA_GAME_ENDED:
        case MA_GAME_RESTORE_LAST_SETTINGS:
        {
            plugin_restore_requested_settings();
            break;
        }
    }
}

static void
update_menu(void)
{
    mahjong_sound_cb(sound_check, NULL);
    mahjong_board_cb(board_box, NULL);
}

/* Set menu item activity */
static void
mahjong_board_cb(GtkWidget * widget, gpointer data)
{
    data = NULL;
    gint active;

    if (!changed)
    {
        changed = TRUE;
        active = gtk_combo_box_get_active(GTK_COMBO_BOX(widget));
        if (active >= 0 && active < 8)
            gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM
                                           (board_rd[active]), TRUE);
        changed = FALSE;
    }
}

static void
mahjong_sound_cb(GtkWidget * widget, gpointer data)
{
    data = NULL;
    if (!changed)
    {
        changed = TRUE;
        gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(sound_ck),
                                       gtk_toggle_button_get_active
                                       (GTK_TOGGLE_BUTTON(widget)));
        changed = FALSE;
    }
}

static void
plugin_destroy_highscore_dialog(void)
{
    gtk_widget_destroy(GTK_WIDGET(dialog));
    dialog = NULL;
    return;
}

static void
plugin_highscore_response(GtkDialog * dial, gint arg1, gpointer user_data)
{
    dial = NULL;
    user_data = NULL;
    if (arg1 == /*GTK_RESPONSE_ACCEPT*/GTK_RESPONSE_OK)
    {
        plugin_destroy_highscore_dialog();
    }
    score_dialog_displayed = FALSE;
    return;
}

static gboolean
plugin_highscore_key_release(GtkWidget * widget,
                             GdkEventKey * event, gpointer user_data)
{
    widget = NULL;
    user_data = NULL;
    if (event == NULL)
        return FALSE;

    switch (event->keyval)
    {
        case GDK_Escape:
            plugin_destroy_highscore_dialog();
	    score_dialog_displayed = FALSE;
            return TRUE;
            break;
    }
    return FALSE;
}

static void
ui_show_highscore(GtkWidget * mahjong_tree_model)
{

    GtkTreeView *tree = NULL;
    GtkWidget *scrolledwindow = NULL;
    GtkCellRenderer *renderer = NULL;
    GtkTreeViewColumn *level_col = NULL, *time_col = NULL, *date_col = NULL;
    StartupApp *app = NULL;
    app = gs.ui->app;

    /* Creating the dialog */
    dialog = GTK_DIALOG(gtk_dialog_new());
    gtk_window_set_title(GTK_WINDOW(&dialog->window),(const gchar *)__("game_ti_mahjong_high_scores_title"));
        /*
	 * GTK_DIALOG(gtk_dialog_new_with_buttons
                   (__("game_ti_mahjong_high_scores_title"),
                    GTK_WINDOW(gs.ui->hildon_appview),
                    GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                    __("game_bd_mahjong_high_scores_close"),
                    GTK_RESPONSE_OK, NULL));
	    */

    /*hildon_help_dialog_help_enable(GTK_DIALOG(dialog),
                                   OSSO_MAHJONG_HELP_BESTTIMES, app->osso);*/
    gtk_signal_connect(GTK_OBJECT(dialog),
                       "response",
                       G_CALLBACK(plugin_highscore_response),
                       GTK_WIDGET(dialog));

    gtk_signal_connect(GTK_OBJECT(dialog),
                       "key-press-event",
                       G_CALLBACK(plugin_highscore_key_release),
                       GTK_WIDGET(dialog));

    tree =
        (GtkTreeView *)
        gtk_tree_view_new_with_model(GTK_TREE_MODEL(mahjong_tree_model));

    g_assert(tree);
    scrolledwindow = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledwindow),
                                   GTK_POLICY_AUTOMATIC,
                                   GTK_POLICY_AUTOMATIC);
    /* Rendering level columnn */
    renderer = gtk_cell_renderer_text_new();
    g_assert(renderer);

    level_col =
        gtk_tree_view_column_new_with_attributes(__
                                                 ("game_ti_mahjong_high_scores_level"),
                                                 renderer, "text",
                                                 HIGHSCORE_LEVEL_COLUMN,
                                                 NULL);

    /* Rendering time columnn */
    renderer = gtk_cell_renderer_text_new();
    g_assert(renderer);

    time_col =
        gtk_tree_view_column_new_with_attributes(__
                                                 ("game_ti_mahjong_high_scores_time"),
                                                 renderer, "text",
                                                 HIGHSCORE_TIME_COLUMN, NULL);
    g_object_set(renderer, "xalign", 1.0, NULL);
    gtk_tree_view_column_set_alignment(time_col, 1.0);
    /* Rendering date columnn */
    renderer = gtk_cell_renderer_text_new();
    g_assert(renderer);

    date_col =
        gtk_tree_view_column_new_with_attributes(__
                                                 ("game_ti_mahjong_high_scores_date"),
                                                 renderer, "text",
                                                 HIGHSCORE_DATE_COLUMN, NULL);
    gtk_tree_view_column_set_alignment(date_col, 1.0);
    g_object_set(renderer, "xalign", 1.0, NULL);
    gtk_tree_view_insert_column(tree, level_col, -1);
    gtk_tree_view_insert_column(tree, time_col, -1);
    gtk_tree_view_insert_column(tree, date_col, -1);

    gtk_tree_view_set_headers_visible(tree, TRUE);
    gtk_container_add(GTK_CONTAINER(scrolledwindow), GTK_WIDGET(tree));
    gtk_container_add(GTK_CONTAINER(dialog->vbox),
                      GTK_WIDGET(scrolledwindow));
   gtk_widget_set_size_request (GTK_WIDGET (dialog), 800, 340);

   gtk_widget_grab_focus(dialog->action_area);
   gtk_widget_show_all(GTK_WIDGET(dialog));

}

GtkTreeModel *
create_highscore_list(struct HighScore *scores)
{
    int i = 0;
    GtkTreeStore *tree_store;
    GtkTreeIter iter;
    int j = 0;

    typedef struct
    {
	gchar *board_name;
	gchar *best_time;
	gchar *date;
    }best_time_dialog;
	
    best_time_dialog scores_list[] =
    {
	{__("game_me_mahjong_menu_board_1"),"-","-"},
	{__("game_me_mahjong_menu_board_2"),"-","-"},
	{__("game_va_mahjong_settings_board_3"),"-","-"},
	{__("game_me_mahjong_menu_board_4"),"-","-"},
	{__("game_me_mahjong_menu_board_5"),"-","-"},
	{__("game_me_mahjong_menu_board_6"),"-","-"},
	{__("game_me_mahjong_menu_board_7"),"-","-"},
	{__("game_me_mahjong_menu_board_8"),"-","-"},
	{NULL,NULL,NULL}
    };

    
    if (scores == NULL)
        return NULL;

    /* New tree store with HIGHSCORE_COLS columns, specifying three first of
     * them to be string */
    tree_store = gtk_tree_store_new(HIGHSCORE_COLS,
                                    G_TYPE_STRING,
                                    G_TYPE_STRING, G_TYPE_STRING);

    /* Building contents for the tree store */
    for (i = 0; i < HIGHSCORE_SIZE; i++)
    {
	/* to fix 85263 */
	if(scores_list[j].board_name != NULL)
	{
		for(i=BEST_TIME_LOW;(i<BEST_TIME_HIGH) && (scores[i].level != NULL);i++)
		{
			/*Loop 8 times to */
			for(j=BEST_TIME_LOW;j<BEST_TIME_HIGH;j++)
			{
				/*compare the name of the game to the array list*/
				if( 0 == strcmp(scores_list[j].board_name,scores[i].level))
				{
					/*copy the matching names to the local struct array*/
					scores_list[j].board_name= g_strdup(scores[i].level);
					scores_list[j].best_time = g_strdup(scores[i].time);
					scores_list[j].date = g_strdup(scores[i].date);

					g_free(scores[i].level);
		                        g_free(scores[i].time);
					g_free(scores[i].date);
	                        }
			}
		}	
                /* Final display of all the 8 names*/
                for(j=BEST_TIME_LOW;j<BEST_TIME_HIGH;j++)
                {
	                /* New row append */
	                gtk_tree_store_append(tree_store, &iter, NULL);

                        /* Setting string values for the specified columns */
                        gtk_tree_store_set(tree_store, &iter,
	                        HIGHSCORE_LEVEL_COLUMN, scores_list[j].board_name,
	                        HIGHSCORE_TIME_COLUMN, scores_list[j].best_time,
	                        HIGHSCORE_DATE_COLUMN, scores_list[j].date, -1);
	        }
	}
    }

    return GTK_TREE_MODEL(tree_store);
}

static GString *
plugin_highscore_read_file(void)
{
    /* Test if file exists */
    if (g_file_test(file, G_FILE_TEST_EXISTS | G_FILE_TEST_IS_REGULAR))
    {
        GFile *handle;
        GFileInputStream *istream;
        GError *err = NULL;

        handle = g_file_new_for_path (file);
        istream = g_file_read (handle, NULL, &err);

        if (err != NULL)
        {
            g_error ("Could not open %s for reading: %s\n", file, err->message);
            g_error_free (err);
            return NULL;
        }

        static const guint64 DATA_BUFFER_SIZE = 1024;
        gboolean  success;
        gchar buffer[DATA_BUFFER_SIZE + 1];
        gsize bytes_read;
        GString *scores = g_string_new("");

        success = g_input_stream_read_all (G_INPUT_STREAM(istream), buffer, sizeof (buffer), &bytes_read, NULL, &err);

        //Might need \0 added first
        if (bytes_read > 0)
          g_string_append_len (scores, buffer, bytes_read);

        if (!success)
	{
	    if (scores->len > 0)
            {
	        g_printerr ("%s: read error in '%s', trying to scan "
	                         "partial content: %s",
	                          G_STRFUNC, file, err->message);
	        g_clear_error (&err);
            }
        }

        g_object_unref(handle);
        g_object_unref(istream);

        return scores;
    }
    return NULL;
}

static void
plugin_highscore_load(struct HighScore *score)
{
    guint i = 0;
    GString *data = plugin_highscore_read_file();
    gchar *tmp = NULL;
    gchar buf[50];

    for (i = 0; i < HIGHSCORE_SIZE; i++)
    {
        score[i].level = NULL;
        score[i].time = NULL;
        score[i].date = NULL;
    }

    if (data == NULL)
        return;
    g_assert(data);
    tmp = data->str;

    for (i = 0; i < HIGHSCORE_SIZE; i++)
    {
        gchar *tmp2 = NULL;
        int intmp;
        time_t time;

        /* Get first space separated entry */
        if (strlen(tmp) == 0)
            break;
        tmp2 = g_strstr_len(tmp, strlen(tmp), " ");
        if (tmp2 == NULL)
            break;
        *tmp2 = 0;

        /* Set it as time */
        intmp = atoi(tmp);
        g_snprintf(buf, 49, "%02d:%02d", intmp / 60, intmp % 60);
        score[i].time = g_strdup(buf);

        /* Get second space separated entry */
        if (tmp2 >= data->str + data->len - 1)
            break;
        tmp = tmp2 + 1;
        if (strlen(tmp) == 0)
            break;
        tmp2 = g_strstr_len(tmp, strlen(tmp), " ");
        if (tmp2 == NULL)
            break;
        *tmp2 = 0;

        /* Set it as date */
        time = atoi(tmp);
       // static const char *gcc_bug = "%x";
        strftime(buf, 49, dgettext("hildon-libs","wdgt_va_date"), localtime(&time));
        score[i].date = g_strdup(buf);

        /* Get everything till the end of line */
        if (tmp2 >= data->str + data->len - 1)
            break;
        tmp = tmp2 + 1;
        if (strlen(tmp) == 0)
            break;
        tmp2 = g_strstr_len(tmp, strlen(tmp), "\n");
        if (tmp2 != NULL)
        {
            *tmp2 = 0;
        }

        /* Set it as level name */
        gchar *locale_id = g_strdup("game_va_mahjong_settings_board_x");
        locale_id[strlen(locale_id) - 1] = *tmp;
        score[i].level = g_strdup(gettext(locale_id));
        g_free(locale_id);

        /* Checks */
        if (tmp2 == NULL)
            break;
        if (strlen(tmp) == 0)
            break;
        if (tmp2 >= data->str + data->len - 1)
            break;
        tmp = tmp2 + 1;
    }

    g_string_free(data, TRUE);
}
