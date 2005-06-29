## Process this file with automake to generate a Makefile.in

lib_LTLIBRARIES = $(GSTREAMER)

EXTRA_LTLIBRARIES = actor_gstreamer.la

libdir = @LIBVISUAL_PLUGINS_BASE_DIR@/actor

LIBS += -L. -L$(prefix)/lib @LIBVISUAL_LIBS@ @GSTREAMER_LIBS@

AM_CFLAGS = @LIBVISUAL_CFLAGS@ @GSTREAMER_CFLAGS@

INCLUDES = $(all_includes) \
	-I$(top_srcdir) 

actor_gstreamer_la_LDFLAGS = -rpath $(libdir) -module -avoid-version 

actor_gstreamer_la_SOURCES = actor_gstreamer.c

