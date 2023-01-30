## Process this file with automake to generate a Makefile.in

actor_plugin_LTLIBRARIES = actor_gstreamer.la

AM_CFLAGS = $(LIBVISUAL_CFLAGS) $(GSTREAMER_CFLAGS)

AM_CPPFLAGS = $(all_includes) \
	-I$(top_srcdir) 

actor_gstreamer_la_LDFLAGS = -module -avoid-version 
actor_gstreamer_la_LIBADD = $(LIBVISUAL_LIBS) $(GSTREAMER_LIBS)
actor_gstreamer_la_SOURCES = actor_gstreamer.c

