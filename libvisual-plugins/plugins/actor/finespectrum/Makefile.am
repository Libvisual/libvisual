## Process this file with automake to generate a Makefile.in

lib_LTLIBRARIES = $(FINESPECTRUM)

EXTRA_LTLIBRARIES = actor_finespectrum.la

libdir = @LIBVISUAL_PLUGINS_BASE_DIR@/actor

LIBS += -L. -L$(prefix)/lib @LIBVISUAL_LIBS@

AM_CFLAGS = @LIBVISUAL_CFLAGS@

INCLUDES = $(all_includes) \
	-I$(top_srcdir) 

actor_finespectrum_la_LDFLAGS = -rpath $(libdir) -module -avoid-version 

actor_finespectrum_la_SOURCES = actor_finespectrum.c

