lib_LTLIBRARIES = actor_oinksie.la

libdir = $(prefix)/lib/libvisual/actor

LIBS = -L. -L$(prefix)/lib @LV_LIBS@

CFLAGS =

INCLUDES = $(all_includes) \
	-I$(top_srcdir) 

actor_oinksie_la_LDFLAGS = -module -avoid-version 

actor_oinksie_la_SOURCES = actor_oinksie.c config.c gfx-background.c gfx-misc.c \
			   gfx-scope.c oinksie.c screen.c audio.c gfx-analyzer.c \
			   gfx-blur.c gfx-palette.c misc.c scene.c table.c

