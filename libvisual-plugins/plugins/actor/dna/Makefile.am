## Process this file with automake to generate a Makefile.in

pkglib_LTLIBRARIES = $(DNA)

EXTRA_LTLIBRARIES = actor_lv_dna.la

pkglibdir = $(prefix)/lib/libvisual/actor

LIBS += -L. -L$(prefix)/lib -lGL -lGLU @LIBVISUAL_LIBS@

AM_CFLAGS = -Wall -rdynamic @LIBVISUAL_CFLAGS@ @DEBUG_CFLAGS@ -O3

INCLUDES = $(all_includes) -I$(top_srcdir) 

actor_lv_dna_la_LDFLAGS = -rpath '$(pkglibdir)' -module -avoid-version

actor_lv_dna_la_SOURCES = actor_lv_dna.c

