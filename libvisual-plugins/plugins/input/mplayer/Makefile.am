## Process this file with automake to generate a Makefile.in

lib_LTLIBRARIES = $(MPLAYER)

EXTRA_LTLIBRARIES = input_mplayer.la

libdir = @LIBVISUAL_PLUGINS_BASE_DIR@/input

LIBS += -L. -L$(prefix)/lib @LIBVISUAL_LIBS@

AM_CFLAGS = @LIBVISUAL_CFLAGS@

INCLUDES = $(all_includes) -I$(top_srcdir) 

input_mplayer_la_LDFLAGS = -rpath $(libdir) -module -avoid-version 

input_mplayer_la_SOURCES = input_mplayer.c

