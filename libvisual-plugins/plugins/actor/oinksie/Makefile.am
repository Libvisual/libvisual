## Process this file with automake to generate a Makefile.in

lib_LTLIBRARIES = actor_oinksie.la

libdir = $(prefix)/lib/libvisual/actor

LIBS = -L. -L$(prefix)/lib @LIBVISUAL_LIBS@

AM_CFLAGS = -Wall -rdynamic @DEBUG_CFLAGS@ @LIBVISUAL_CFLAGS@ -O3

INCLUDES = $(all_includes) -I$(top_srcdir) 

actor_oinksie_la_LDFLAGS = -module -avoid-version 

actor_oinksie_la_SOURCES = actor_oinksie.c config.c gfx-background.c gfx-misc.c \
			   gfx-scope.c oinksie.c screen.c audio.c gfx-analyzer.c \
			   gfx-blur.c gfx-palette.c misc.c scene.c table.c

