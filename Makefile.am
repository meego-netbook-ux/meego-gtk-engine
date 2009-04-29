INCLUDES = $(GTK_CFLAGS) $(DEVELOPMENT_CFLAGS)

enginedir = $(libdir)/gtk-2.0/$(GTK_VERSION)/engines

engine_LTLIBRARIES = libmoblin-netbook-engine.la

libmoblin_netbook_engine_la_SOURCES =   \
	./src/moblin-netbook-style.c    \
	./src/moblin-netbook-style.h    \
	./src/moblin-netbook-rc-style.c \
	./src/moblin-netbook-rc-style.h \
	./src/moblin-netbook-main.c     \
	./src/moblin-netbook-utils.c    \
	./src/moblin-netbook-utils.h

libmoblin_netbook_engine_la_LDFLAGS = -module -avoid-version -no-undefined -Werror
libmoblin_netbook_engine_la_LIBADD = $(GTK_LIBS)

themedir = $(datadir)/themes/Moblin-Netbook

gtkthemedir = $(themedir)/gtk-2.0
dist_gtktheme_DATA = data/gtkrc

metacitythemedir = $(themedir)/metacity-1
dist_metacitytheme_DATA = data/metacity-theme-1.xml

indexthemedir = $(themedir)/
dist_indextheme_DATA = data/index.theme
