## Process this file with automake to produce Makefile.in

lib_LTLIBRARIES = libvisual_bmp.la

libdir = @BMP_VISUALIZATION_PLUGIN_DIR@

LIBS = -L$(top_srcdir)/src -L$(prefix)/lib @BMP_LIBS@ @SDL_LIBS@ @LIBVISUAL_LIBS@

AM_CFLAGS = -Wall -Wmissing-prototypes\
	    @BMP_CFLAGS@ @SDL_CFLAGS@ @LIBVISUAL_CFLAGS@ @DEBUG_CFLAGS@

INCLUDES = $(all_includes) \
	-I$(top_srcdir)/src -I$(top_builddir)

DEFS = -DLOCALEDIR=\"$(datadir)/locale\" @DEFS@

libvisual_bmp_la_LDFLAGS = -module -avoid-version

libvisual_bmp_la_SOURCES = main.c \
			    lv_bmp_config.c lv_bmp_config.h \
			    config_gui.c config_gui.h \
			    about.c about.h \
			    gettext.h

