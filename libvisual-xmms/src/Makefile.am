## Process this file with automake to produce Makefile.in

libdir = @XMMS_VISUALIZATION_PLUGIN_DIR@

lib_LTLIBRARIES = libvisual_xmms.la

LIBS = -L$(top_srcdir)/src -L$(prefix)/lib @XMMS_LIBS@ @SDL_LIBS@ @GTK_LIBS@ @LIBVISUAL_LIBS@

AM_CFLAGS = @XMMS_CFLAGS@ @SDL_CFLAGS@ @GTK_CFLAGS@ @LIBVISUAL_CFLAGS@ @DEBUG_CFLAGS@\
	    -rdynamic -O2

INCLUDES = $(all_includes) \
	-I$(top_srcdir)/src

DEFS = @DEFS@ -DVERSION=\"$(VERSION)\"

libvisual_xmms_la_LDFLAGS = -module -avoid-version

libvisual_xmms_la_SOURCES = lv_xmms.c lv_xmms.h

