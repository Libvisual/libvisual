## Process this file with automake to generate a Makefile.in

library_includedir = $(includedir)/lvw/gtk2
library_include_HEADERS = lvw_gtk2_visui.h

lib_LTLIBRARIES = liblvw_gtk2.la

DEFS = @DEFS@

LIBS += -L. -L$(libdir) @LIBVISUAL_LIBS@ @LIBGTK2_LIBS@

AM_CFLAGS = -Wall -Wno-unused-variable -Wmissing-prototypes -Wstrict-prototypes \
	    @DEBUG_CFLAGS@ @LIBVISUAL_CFLAGS@ @LIBGTK2_CFLAGS@ -O3

INCLUDES = -I$(top_srcdir) -I$(top_srcdir)/gtk2 \
	   -I$(top_builddir) -I$(top_builddir)/gtk2

liblvw_gtk2_la_LDFLAGS = -export-dynamic -no-undefined

liblvw_gtk2_la_SOURCES = lvw_gtk2_visui.c

