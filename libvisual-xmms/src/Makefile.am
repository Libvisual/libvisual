## Process this file with automake to produce Makefile.in

lib_LTLIBRARIES = libvisual_xmms.la

libdir = @XMMS_VISUALIZATION_PLUGIN_DIR@

localedir = $(datadir)/locale

LIBS = -L$(top_srcdir)/src -L$(prefix)/lib @XMMS_LIBS@ @SDL_LIBS@ @GTK_LIBS@ @LIBVISUAL_LIBS@ @LTLIBTINTL@

AM_CFLAGS = -Wall -Wmissing-prototypes\
	    @XMMS_CFLAGS@ @SDL_CFLAGS@ @GTK_CFLAGS@ @LIBVISUAL_CFLAGS@ @DEBUG_CFLAGS@

INCLUDES = $(all_includes) \
	-I$(top_srcdir)/src

DEFS = -DLOCALEDIR=\"$(localedir)\" -DVERSION=\"$(VERSION)\" -DPACKAGE_DATADIR=\"$(pkgdatadir)\" @DEFS@

libvisual_xmms_la_LDFLAGS = -module -avoid-version

libvisual_xmms_la_SOURCES = main.c\
			    config.c config.h\
			    config_gui.c config_gui.h\
			    about.c about.h\
			    gettext.h

