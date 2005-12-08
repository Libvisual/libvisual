## Process this file with automake to generate a Makefile.in

lib_LTLIBRARIES = $(NASTYFFT)

EXTRA_LTLIBRARIES = actor_nastyfft.la

libdir = @LIBVISUAL_PLUGINS_BASE_DIR@/actor

LIBS += -L. -L$(prefix)/lib -lGL -lGLU @LIBVISUAL_LIBS@

AM_CFLAGS = @LIBVISUAL_CFLAGS@

INCLUDES = $(all_includes) -I$(top_srcdir) 

actor_nastyfft_la_LDFLAGS = -rpath $(libdir) -module -avoid-version 

actor_nastyfft_la_SOURCES = actor_nastyfft.c

