## Process this file with automake to generate a Makefile.in

pkglib_LTLIBRARIES = $(PLAZMA)

EXTRA_LTLIBRARIES = actor_plazma.la

pkglibdir = $(prefix)/lib/libvisual/actor

LIBS += -L. -L$(prefix)/lib @LIBVISUAL_LIBS@

AM_CFLAGS = -Wall -rdynamic @DEBUG_CFLAGS@ @LIBVISUAL_CFLAGS@ -O3

INCLUDES = $(all_includes) -I$(top_srcdir) 

actor_plazma_la_LDFLAGS = -rpath '$(pkglibdir)' -module -avoid-version 

actor_plazma_la_SOURCES = actor_plazma.c \
			  actor_plazma.h \
			  plazma.c \
			  plazma.h

