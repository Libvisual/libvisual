## Process this file with automake to generate a Makefile.in

input_plugin_LTLIBRARIES = input_alsa.la

LIBS += $(ALSA_LIBS) $(LIBVISUAL_LIBS)

AM_CFLAGS = $(ALSA_CFLAGS) $(LIBVISUAL_CFLAGS)

INCLUDES = $(all_includes) -I$(top_srcdir) -I$(top_builddir)

input_alsa_la_LDFLAGS = -module -avoid-version 

input_alsa_la_SOURCES = input_alsa.c
