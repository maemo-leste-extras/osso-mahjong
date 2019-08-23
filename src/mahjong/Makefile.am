INCLUDES = $(GTK_CFLAGS) $(GCONF_CFLAGS) $(OSSO_CFLAGS) $(PANGO_CFLAGS) $(MAEMO_LAUNCHER_CFLAGS)

lib_LIBRARIES = libmahjong.a
libmahjong_a_CFLAGS = $(INCLUDES) \
		-DLOCALEDIR=\"$(chessuilocaledir)\" \
		-DPIXMAPSDIR=\"$(pixmapsdir)\" \
                -DPREFIX=\"$(prefix)\" -I$(top_srcdir) -I..
libmahjong_a_CXXFLAGS = $(libmahjong_a_CFLAGS)

libmahjong_a_SOURCES = \
	drawing.c drawing.h \
	mahjongg.c mahjongg.h \
	solubility.c solubility.h \
	games-clock.c games-clock.h
