## Process this file with automake to generate a Makefile.in

actor_plugin_LTLIBRARIES = actor_xpyre.la

LIBS = $(LIBVISUAL_LIBS)

AM_CFLAGS = $(LIBVISUAL_CFLAGS)

INCLUDES = $(all_includes) -I$(top_srcdir) 

actor_xpyre_la_LDFLAGS = -module -avoid-version 

actor_xpyre_la_SOURCES = actor_xpyre.c \
			 actor_xpyre.h \
			 draw.c \
			 draw.h

