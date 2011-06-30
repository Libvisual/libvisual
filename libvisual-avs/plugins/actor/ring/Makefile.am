## Process this file with automake to generate a Makefile.in

lib_LTLIBRARIES = $(AVS_RING)

EXTRA_LTLIBRARIES = actor_avs_ring.la

libdir = @LIBVISUAL_PLUGINS_BASE_DIR@/actor

LIBS += -L. -L$(prefix)/lib @LIBVISUAL_LIBS@

AM_CFLAGS = @LIBVISUAL_CFLAGS@

INCLUDES = $(all_includes) \
	-I$(top_srcdir) \
	-I$(top_srcdir)/common \
	-I$(top_srcdir)/src

actor_avs_ring_la_LDFLAGS = -rpath $(libdir) -module -avoid-version 

actor_avs_ring_la_SOURCES = actor_avs_ring.c

actor_avs_ring_la_LIBADD = ../../../common/libavs.la

