## Process this file with automake to generate a Makefile.in

lib_LTLIBRARIES = $(BUMPSCOPE)

EXTRA_LTLIBRARIES = actor_bumpscope.la

libdir = @LIBVISUAL_PLUGINS_BASE_DIR@/actor

LIBS += -L. -L$(prefix)/lib @LIBVISUAL_LIBS@

AM_CFLAGS = @LIBVISUAL_CFLAGS@

INCLUDES = $(all_includes) -I$(top_srcdir) 

actor_bumpscope_la_LDFLAGS = -rpath $(libdir) -module -avoid-version 

actor_bumpscope_la_SOURCES = actor_bumpscope.c \
			  actor_bumpscope.h \
			  bump_scope.c \
			  bump_scope.h

