## Process this file with automake to generate a Makefile.in

pkglib_LTLIBRARIES = $(GOOM2)

EXTRA_LTLIBRARIES = actor_goom2.la

pkglibdir = $(prefix)/lib/libvisual/actor

LIBS += -L. -L$(prefix)/lib @LIBGOOM2_LIBS@ @LIBVISUAL_LIBS@

AM_CFLAGS = -Wall -rdynamic @DEBUG_CFLAGS@ @LIBGOOM2_CFLAGS@ @LIBVISUAL_CFLAGS@ -O3

INCLUDES = $(all_includes) -I$(top_srcdir)

actor_goom2_la_LDFLAGS = -rpath '$(pkglibdir)' -module -avoid-version

actor_goom2_la_SOURCES = actor_goom2.c

