SUBDIRS = ui mahjong plugin
INCLUDES = $(GTK_CFLAGS) $(OSSO_CFLAGS) $(GCONF_CFLAGS) $(PNG_CFLAGS) \
	   $(PANGO_CFLAGS) $(ESOUND_CFLAGS) $(GNOMEVFS_CFLAGS)

servicefiledir=$(prefix)/share/dbus-1/services
servicefile_DATA = com.nokia.osso_mahjong.service \
		   com.nokia.osso_mahjong.startup.service

cuddir=/etc/osso-cud-scripts/
cud_SCRIPTS = osso-mahjong-cud.sh

configdir = $(datadir)/osso-mahjong
config_DATA = osso_mahjong.conf

hildondesktopentry_DATA = mahjong_startup.desktop

bin_PROGRAMS = Mahjong

Mahjong_CFLAGS = $(INCLUDES) \
		-DLOCALEDIR=\"$(appuilocaledir)\" \
		-DPIXMAPSDIR=\"$(pixmapsdir)\" \
		-DSOUNDSDIR=\"$(soundsdir)\" \
                -DPREFIX=\"$(prefix)\" -I$(top_srcdir) \
				$(MAEMO_LAUNCHER_CFLAGS)

Mahjong_CXXFLAGS = $(Mahjong_CFLAGS)


Mahjong_SOURCES = \
	main.c main.h \
	dbus.c dbus.h \
	settings.c settings.h \
	state_save.c state_save.h \
	mahjong_communication.c mahjong_communication.h \
	mahjong_core.c mahjong_core.h \
	appdata.h mahjong_i18n.h \
	sound.c sound.h sounds.h \
	hiscore.c hiscore.h

edit = sed \
	-e 's,@datadir\@,$(datadir),g' \
	-e 's,@prefix\@,$(prefix),g' \
	-e 's,@VERSION\@,$(VERSION),g'


osso_mahjong.conf: osso_mahjong.conf.in
	@$(edit) $< > $@


backupconfigurationdir=/etc/osso-backup/applications
backupconfiguration_DATA=osso-mahjong.conf


Mahjong_LDFLAGS = $(MAEMO_LAUNCHER_LDFLAGS)
Mahjong_LDADD = $(GTK_LIBS) $(OSSO_LIBS) $(GCONF_LIBS) $(PNG_LIBS) \
	$(PANGO_LIBS) $(ESOUND_LIBS) $(GNOMEVFS_LIBS) ui/libmahjongui.a \
       	mahjong/libmahjong.a
