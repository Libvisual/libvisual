## Process this file with automake to generate a Makefile.in

input_plugin_LTLIBRARIES = input_jack.la

LIBS += $(LIBJACK_LIBS) $(LIBVISUAL_LIBS)

AM_CFLAGS = $(LIBJACK_CFLAGS) $(LIBVISUAL_CFLAGS)

INCLUDES = $(all_includes) -I$(top_srcdir) 

input_jack_la_LDFLAGS = -module -avoid-version 

input_jack_la_SOURCES = input_jack.c

