INCLUDES = $(GTK_CFLAGS) $(DEVELOPMENT_CFLAGS)

enginedir = $(libdir)/gtk-2.0/$(GTK_VERSION)/engines

engine_LTLIBRARIES = libmeego-netbook.la

libmeego_netbook_la_SOURCES =   \
	./src/moblin-netbook-style.c    \
	./src/moblin-netbook-style.h    \
	./src/moblin-netbook-rc-style.c \
	./src/moblin-netbook-rc-style.h \
	./src/moblin-netbook-main.c     \
	./src/moblin-netbook-utils.c    \
	./src/moblin-netbook-utils.h

libmeego_netbook_la_LDFLAGS = -module -avoid-version -no-undefined -Werror
libmeego_netbook_la_LIBADD = $(GTK_LIBS)

themedir = $(datadir)/themes/Netbook

install-data-local:
	@$(NORMAL_INSTALL)
	$(MKDIR_P) $(DESTDIR)$(themedir)/gtk-2.0/Assets
	$(MKDIR_P) $(DESTDIR)$(themedir)/metacity-1
	$(INSTALL_DATA) $(top_srcdir)/data/Netbook/gtk-2.0/Assets/*.png $(DESTDIR)$(themedir)/gtk-2.0/Assets
	$(INSTALL_DATA) $(top_srcdir)/data/Netbook/gtk-2.0/gtkrc $(DESTDIR)$(themedir)/gtk-2.0
	$(INSTALL_DATA) $(top_srcdir)/data/Netbook/metacity-1/* $(DESTDIR)$(themedir)/metacity-1
	$(INSTALL_DATA) $(top_srcdir)/data/Netbook/index.theme $(DESTDIR)$(themedir)/index.theme

uninstall-local:
	@$(NORMAL_UNINSTALL)
	-rm -rf $(DESTDIR)$(themedir)

EXTRA_DIST=data/Netbook
