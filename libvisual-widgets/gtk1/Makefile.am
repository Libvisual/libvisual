## Process this file with automake to generate a Makefile.in

lib_LTLIBRARIES = $(LVW_GTK1)

EXTRA_LTLIBRARIES = liblvwidget_gtk1.la

libdir = $(prefix)/lib

library_includedir = $(includedir)/lvwidget/gtk1
library_include_HEADERS = lv_widget_gtk1_visui.h

DEFS = @DEFS@

LIBS += -L. -L$(libdir) @LIBVISUAL_LIBS@ @LIBGTK1_LIBS@

AM_CFLAGS = -Wall -Wno-unused-variable -Wmissing-prototypes \
	    @DEBUG_CFLAGS@ @LIBVISUAL_CFLAGS@ @LIBGTK1_CFLAGS@ -O3

INCLUDES = -I$(top_srcdir) -I$(top_srcdir)/gtk1 -I$(top_builddir)

liblvwidget_gtk1_la_LDFLAGS = -rpath '$(libdir)' -export-dynamic -no-undefined

liblvwidget_gtk1_la_SOURCES = lv_widget_gtk1_visui.c
