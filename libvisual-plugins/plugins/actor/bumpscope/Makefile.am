## Process this file with automake to generate a Makefile.in

actor_plugin_LTLIBRARIES = actor_bumpscope.la

LIBS += $(LIBVISUAL_LIBS)

AM_CFLAGS = $(LIBVISUAL_CFLAGS)

INCLUDES = $(all_includes) -I$(top_srcdir) 

actor_bumpscope_la_LDFLAGS = -module -avoid-version 

actor_bumpscope_la_SOURCES = actor_bumpscope.c \
			  actor_bumpscope.h \
			  bump_scope.c \
			  bump_scope.h

