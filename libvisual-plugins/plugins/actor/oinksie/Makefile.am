## Process this file with automake to generate a Makefile.in

actor_plugin_LTLIBRARIES = actor_oinksie.la

LIBS += $(LIBVISUAL_LIBS)

AM_CFLAGS = $(LIBVISUAL_CFLAGS)

INCLUDES = $(all_includes) -I$(top_srcdir) 

actor_oinksie_la_LDFLAGS = -module -avoid-version 

actor_oinksie_la_SOURCES = actor_oinksie.c\
			   oinksie.c oinksie.h\
			   config.c config.h\
			   gfx-background.c gfx-background.h\
			   gfx-misc.c gfx-misc.h\
			   gfx-scope.c gfx-scope.h\
			   screen.c screen.h\
			   audio.c audio.h\
			   gfx-analyzer.c gfx-analyzer.h\
			   gfx-blur.c gfx-blur.h\
			   gfx-palette.c gfx-palette.h\
			   misc.c misc.h\
			   scene.c scene.h\
			   table.c table.h\
			   config-defines.h common.h

