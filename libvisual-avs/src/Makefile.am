## Process this file with automake to generate a Makefile.in

lib_LTLIBRARIES = $(AVS)

EXTRA_LTLIBRARIES = actor_AVS.la

libdir = @LIBVISUAL_PLUGINS_BASE_DIR@/actor

LIBS += -L. -L$(prefix)/lib $(XML_LIBS) $(GLIB_LIBS) @LIBVISUAL_LIBS@ -lxml2

AM_CFLAGS = $(XML_CFLAGS) $(GLIB_CFLAGS) @LIBVISUAL_CFLAGS@ -I/usr/include/libxml2/

INCLUDES = $(all_includes) -I$(top_srcdir) -I$(top_srcdir)/common

actor_AVS_la_LDFLAGS = -rpath $(libdir) -module -avoid-version

actor_AVS_la_SOURCES = actor_AVS.c

actor_AVS_la_LIBADD = ../common/libavs.la
