## Process this file with automake to generate a Makefile.in

actor_plugin_LTLIBRARIES = actor_JESS.la

LIBS += $(LIBVISUAL_LIBS)

AM_CFLAGS = $(LIBVISUAL_CFLAGS)

INCLUDES = $(all_includes) -I$(top_srcdir) 

actor_JESS_la_LDFLAGS = -module -avoid-version

actor_JESS_la_SOURCES = analyser.c analyser.h analyser_struct.h\
			distorsion.c distorsion.h\
			draw.c draw.h\
			draw_low_level.c draw_low_level.h\
			jess.c jess.h\
			pal.c pal.h\
			projection.c projection.h\
			renderer.c renderer.h\
			def.h struct.h

