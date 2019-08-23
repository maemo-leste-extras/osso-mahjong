INCLUDES = $(GTK_CFLAGS) $(GCONF_CFLAGS) $(OSSO_CFLAGS) $(PANGO_CFLAGS) $(MAEMO_LAUNCHER_CFLAGS)

lib_LIBRARIES = libmahjongui.a
libmahjongui_a_CFLAGS = $(INCLUDES) \
		-DLOCALEDIR=\"$(chessuilocaledir)\" \
		-DPIXMAPSDIR=\"$(pixmapsdir)\" \
                -DPREFIX=\"$(prefix)\" -I$(top_srcdir) -I.. -I../mahjong

libmahjongui_a_SOURCES = \
	interface.c interface.h \
	callbacks.c callbacks.h \
	mahjong_button.c mahjong_button.h
