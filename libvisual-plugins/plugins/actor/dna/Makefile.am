## Process this file with automake to generate a Makefile.in

lib_LTLIBRARIES = actor_lv_dna.la

libdir = $(prefix)/lib/libvisual/actor

LIBS = -L. -L$(prefix)/lib -lGL -lGLU @LV_LIBS@

INCLUDES = $(all_includes) \
	-I$(top_srcdir) 

actor_lv_dna_la_LDFLAGS = -module -avoid-version 

actor_lv_dna_la_SOURCES = actor_lv_dna.c

