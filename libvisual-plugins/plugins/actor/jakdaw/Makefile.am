## Process this file with automake to generate a Makefile.in

pkglib_LTLIBRARIES = $(JAKDAW)

EXTRA_LTLIBRARIES = actor_jakdaw.la

pkglibdir = $(prefix)/lib/libvisual/actor

LIBS += -L. -L$(prefix)/lib @LIBVISUAL_LIBS@

AM_CFLAGS = -Wall -rdynamic @DEBUG_CFLAGS@ @LIBVISUAL_CFLAGS@ -O3

INCLUDES = $(all_includes) -I$(top_srcdir) 

actor_jakdaw_la_LDFLAGS = -rpath '$(pkglibdir)' -module -avoid-version 

actor_jakdaw_la_SOURCES = actor_jakdaw.c \
			  actor_jakdaw.h \
			  feedback.c \
			  feedback.h \
			  plotter.c \
			  plotter.h
			  

