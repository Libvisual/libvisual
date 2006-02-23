## Process this file with automake to generate a Makefile.in

input_plugin_LTLIBRARIES = input_mplayer.la

LIBS += $(LIBVISUAL_LIBS)

AM_CFLAGS = $(LIBVISUAL_CFLAGS)

INCLUDES = $(all_includes) -I$(top_srcdir) 

input_mplayer_la_LDFLAGS = -module -avoid-version 
input_mplayer_la_SOURCES = input_mplayer.c

