lib_LTLIBRARIES = actor_JESS.la

libdir = $(prefix)/lib/libvisual/actor

LIBS = -L. -L$(prefix)/lib @LV_LIBS@

CFLAGS =

INCLUDES = $(all_includes) \
	-I$(top_srcdir) 

actor_JESS_la_LDFLAGS = -module -avoid-version 

actor_JESS_la_SOURCES = analyser.c distorsion.c draw.c draw_low_level.c \
			jess.c pal.c projection.c renderer.c

