## Process this file with automake to generate a Makefile.in

lib_LTLIBRARIES = $(LVW_GTK2)

EXTRA_LTLIBRARIES = liblvwidget_gtk2.la

libdir = $(prefix)/lib

library_includedir = $(includedir)/lvwidget/gtk2
library_include_HEADERS = lv_widget_gtk2_visui.h

DEFS = @DEFS@

LIBS += -L. -L$(libdir) @LIBVISUAL_LIBS@ @LIBGTK2_LIBS@

AM_CFLAGS = -Wall -Wno-unused-variable -Wmissing-prototypes \
	    @DEBUG_CFLAGS@ @LIBVISUAL_CFLAGS@ @LIBGTK2_CFLAGS@ -O3

INCLUDES = -I$(top_srcdir) -I$(top_srcdir)/gtk2 \
	   -I$(top_builddir) -I$(top_builddir)/gtk2

liblvwidget_gtk2_la_LDFLAGS = -export-dynamic -no-undefined

liblvwidget_gtk2_la_SOURCES = lv_widget_gtk2_visui.c

