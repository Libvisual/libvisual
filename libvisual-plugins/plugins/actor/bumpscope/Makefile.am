## Process this file with automake to generate a Makefile.in

lib_LTLIBRARIES = actor_bumpscope.la

libdir = $(prefix)/lib/libvisual/actor

LIBS = -L. -L$(prefix)/lib @LIBVISUAL_LIBS@

AM_CFLAGS = -Wall -rdynamic @DEBUG_CFLAGS@ @LIBVISUAL_CFLAGS@ -O3

INCLUDES = $(all_includes) -I$(top_srcdir) 

actor_bumpscope_la_LDFLAGS = -module -avoid-version 

actor_bumpscope_la_SOURCES = actor_bumpscope.c \
			  actor_bumpscope.h

