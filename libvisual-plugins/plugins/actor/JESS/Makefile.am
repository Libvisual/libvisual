## Process this file with automake to generate a Makefile.in

lib_LTLIBRARIES = @JESSLA@

EXTRA_LTLIBRARIES = actor_JESS.la

libdir = $(prefix)/lib/libvisual/actor

LIBS = -L. -L$(prefix)/lib @LIBVISUAL_LIBS@

AM_CFLAGS = -Wall -rdynamic @DEBUG_CFLAGS@ @LIBVISUAL_CFLAGS@ -O3

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

