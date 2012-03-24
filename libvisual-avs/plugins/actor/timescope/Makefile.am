## Process this file with automake to generate a Makefile.in

lib_LTLIBRARIES = $(AVS_TIMESCOPE)

EXTRA_LTLIBRARIES = actor_avs_timescope.la

libdir = @LIBVISUAL_PLUGINS_BASE_DIR@/actor

LIBS += -L. $(GLIB_LIBS) -L$(prefix)/lib @LIBVISUAL_LIBS@

AM_CFLAGS = $(GLIB_CFLAGS) @LIBVISUAL_CFLAGS@

INCLUDES = $(all_includes) \
	-I$(top_srcdir) \
	-I$(top_srcdir)/common \
	-I$(top_srcdir)/visscript

actor_avs_timescope_la_LDFLAGS = -rpath $(libdir) -module -avoid-version 

actor_avs_timescope_la_SOURCES = actor_avs_timescope.c


actor_avs_timescope_la_LIBADD = ../../../common/libavs.la ../../../visscript/libvisscript.la
