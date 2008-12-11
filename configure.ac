AC_INIT([sato-engine], [0.3.1])
AC_CONFIG_SRCDIR([README])
AM_CONFIG_HEADER(config.h)

AM_INIT_AUTOMAKE

AC_PROG_CC
AM_PROG_CC_STDC
AC_PROG_INSTALL
AC_PROG_MAKE_SET

dnl Initialize libtool
AM_DISABLE_STATIC
AC_LIBTOOL_WIN32_DLL
AM_PROG_LIBTOOL

PKG_PROG_PKG_CONFIG()

AC_ARG_ENABLE(cairo,AC_HELP_STRING([--enable-cairo],[Enable rendering through
      cairo [[default=no]]]), want_cairo="yes")

if test "x$want_cairo" = "xyes"; then
  PKG_CHECK_MODULES(GTK, gtk+-2.0 >= 2.8.0,, AC_MSG_ERROR([GTK+-2.8 is required to compile sato-engine]))
  AC_DEFINE(ENABLE_CAIRO, 1, [Define if you want to enable rendering through cairo])
  DRAW_C="sato-draw-cairo.c"
  AC_SUBST(DRAW_C)
else
  PKG_CHECK_MODULES(GTK, gtk+-2.0 >= 2.6.0,, AC_MSG_ERROR([GTK+-2.6 is required to compile sato-engine]))
  DRAW_C="sato-draw.c"
  AC_SUBST(DRAW_C)
fi

flavour=vga
AC_ARG_WITH([mode],
            AC_HELP_STRING([--with-mode=@<:@vga/qvga@:>@], 
            [Use vga or qvga images sizes [default vga]]),
            flavour=$with_mode)

AM_CONDITIONAL(WITH_QVGA, test "x$flavour" = "xqvga")

if test "x$flavour" = "xqvga" ; then
  cp data/gtkrc-qvga data/gtkrc
else
  cp data/gtkrc-vga data/gtkrc
fi


AC_SUBST(GTK_CFLAGS)
AC_SUBST(GTK_LIBS)

GTK_VERSION=`$PKG_CONFIG --variable=gtk_binary_version gtk+-2.0`
AC_SUBST(GTK_VERSION)

AC_CONFIG_FILES([
Makefile
]) 


AC_OUTPUT

echo "
        Using images for screen size : ${flavour}
"
