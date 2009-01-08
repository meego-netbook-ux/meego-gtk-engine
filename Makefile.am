INCLUDES = $(GTK_CFLAGS) $(DEVELOPMENT_CFLAGS)

enginedir = $(libdir)/gtk-2.0/$(GTK_VERSION)/engines

engine_LTLIBRARIES = libsato-engine.la

libsato_engine_la_SOURCES = \
	./src/sato-draw-cairo.c       \
	./src/sato-draw.h             \
	./src/sato-main.c             \
	./src/sato-style.c            \
	./src/sato-style.h \
	./src/sato-utils.c \
	./src/sato-utils.h

libsato_engine_la_LDFLAGS = -module -avoid-version -no-undefined -Werror
libsato_engine_la_LIBADD = $(GTK_LIBS)

themedir = $(datadir)/themes/Moblin-Netbook

gtkthemedir = $(themedir)/gtk-2.0
dist_gtktheme_DATA = data/gtkrc

metacitythemedir = $(themedir)/metacity-1
dist_metacitytheme_DATA = data/metacity-theme-1.xml

indexthemedir = $(themedir)/
dist_indextheme_DATA = data/index.theme
