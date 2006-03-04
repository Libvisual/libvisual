## Process this file with automake to generate a Makefile.in

actor_plugin_LTLIBRARIES = actor_gstreamer.la

LIBS += $(LIBVISUAL_LIBS) $(GSTREAMER_LIBS)

AM_CFLAGS = $(LIBVISUAL_CFLAGS) $(GSTREAMER_CFLAGS)

INCLUDES = $(all_includes) \
	-I$(top_srcdir) 

actor_gstreamer_la_LDFLAGS = -module -avoid-version 

actor_gstreamer_la_SOURCES = actor_gstreamer.c

