## Process this file with automake to generate a Makefile.in

lib_LTLIBRARIES = actor_xpyre.la

libdir = @LIBVISUAL_PLUGINS_BASE_DIR@/actor

LIBS = -L. -L$(prefix)/lib @LIBVISUAL_LIBS@

AM_CFLAGS = @LIBVISUAL_CFLAGS@

INCLUDES = $(all_includes) -I$(top_srcdir) 

actor_xpyre_la_LDFLAGS = -rpath $(libdir) -module -avoid-version 

actor_xpyre_la_SOURCES = actor_xpyre.c \
			 actor_xpyre.h \
			 draw.c \
			 draw.h

