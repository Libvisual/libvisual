## Process this file with automake to generate a Makefile.in

lib_LTLIBRARIES = $(AVS_STARS)

EXTRA_LTLIBRARIES = actor_avs_stars.la

libdir = @LIBVISUAL_PLUGINS_BASE_DIR@/actor

LIBS += -L. -L$(prefix)/lib @LIBVISUAL_LIBS@

AM_CFLAGS = @LIBVISUAL_CFLAGS@ -fopenmp

INCLUDES = $(all_includes) \
	-I$(top_srcdir) \
	-I$(top_srcdir)/common \
	-I$(top_srcdir)/src

actor_avs_stars_la_LDFLAGS = -rpath $(libdir) -module -avoid-version

actor_avs_stars_la_SOURCES = actor_avs_stars.c

actor_avs_stars_la_LIBADD = ../../../common/libavs.la

