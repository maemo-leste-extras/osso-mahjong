plugin_LTLIBRARIES = osso_mahjong_plugin.la
plugindir = @prefix@/lib/osso-mahjong

osso_mahjong_plugin_la_LDFLAGS = -export-dynamic -avoid-version -module
osso_mahjong_plugin_la_CFLAGS = $(GTK_CFLAGS) $(GCONF_CFLAGS) $(GNOMEVFS_CFLAGS) $(OSSO_CFLAGS)
osso_mahjong_plugin_la_SOURCES = plugin.c
