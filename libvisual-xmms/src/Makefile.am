## Process this file with automake to produce Makefile.in

lib_LTLIBRARIES = libvisual_xmms.la

libdir = @XMMS_VISUALIZATION_PLUGIN_DIR@

LIBS = -L$(top_srcdir)/src -L$(prefix)/lib @XMMS_LIBS@ \
	@SDL_LIBS@ @GTK_LIBS@ @LTLIBTINTL@ \
	@LIBVISUAL_LIBS@ @LIBVISUAL_WIDGETS_LIBS@

AM_CFLAGS = @XMMS_CFLAGS@ @SDL_CFLAGS@ @GTK_CFLAGS@ \
	@LIBVISUAL_CFLAGS@ @LIBVISUAL_WIDGETS_CFLAGS@

INCLUDES = $(all_includes) \
	-I$(top_srcdir)/src -I$(top_builddir)

DEFS = -DLOCALEDIR=\"$(datadir)/locale\" @DEFS@

libvisual_xmms_la_LDFLAGS = -module -avoid-version

libvisual_xmms_la_SOURCES = main.c main.h \
			    lv_xmms_config.c lv_xmms_config.h \
			    config_gui.c config_gui.h \
			    about.c about.h \
			    gettext.h

