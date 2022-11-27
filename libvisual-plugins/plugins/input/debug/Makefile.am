## Process this file with automake to generate a Makefile.in

input_plugin_LTLIBRARIES = input_debug.la

AM_CFLAGS = $(LIBVISUAL_CFLAGS)

INCLUDES = $(all_includes) -I$(top_srcdir) -I$(top_builddir)

input_debug_la_LDFLAGS = -module -avoid-version 
input_debug_la_LIBADD = $(LIBVISUAL_LIBS)
input_debug_la_SOURCES = input_debug.c
