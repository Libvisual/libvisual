## Process this file with automake to generate a Makefile.in

input_plugin_LTLIBRARIES = input_xmms2.la

LIBS += $(XMMS2_LIBS) $(LIBVISUAL_LIBS)

AM_CFLAGS = $(XMMS2_CFLAGS) $(LIBVISUAL_CFLAGS)

INCLUDES = $(all_includes) -I$(top_srcdir) -I$(top_builddir)

input_xmms2_la_LDFLAGS = -module -avoid-version 

input_xmms2_la_SOURCES = input_xmms2.c
