## Process this file with automake to generate a Makefile.in
libdir=@LIBVISUAL_PLUGINS_BASE_DIR@/display

lib_LTLIBRARIES = x11_new.la x11_fullscreen.la

LIBS = -L. -L$(prefix)/lib @LIBVISUAL_LIBS@

AM_CFLAGS = @LIBVISUAL_CFLAGS@

INCLUDES = $(all_includes) -I$(top_srcdir)

x11_new_la_LDFLAGS = -module -avoid-version
x11_fullscreen_la_LDFLAGS = -module -avoid-version -lXxf86vm

x11_new_la_SOURCES = x11_new.c
x11_fullscreenw_la_SOURCES = x11_new.c

