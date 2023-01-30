## Process this file with automake to generate a Makefile.in

input_plugin_LTLIBRARIES = input_alsa.la

AM_CFLAGS = $(ALSA_CFLAGS) $(LIBVISUAL_CFLAGS)

AM_CPPFLAGS = $(all_includes) -I$(top_srcdir) -I$(top_builddir)

input_alsa_la_LDFLAGS = -module -avoid-version 
input_alsa_la_LIBADD = $(ALSA_LIBS) $(LIBVISUAL_LIBS)
input_alsa_la_SOURCES = input_alsa.c
