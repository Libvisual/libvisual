## Process this file with automake to generate a Makefile.in

lib_LTLIBRARIES = actor_xpyre.la

libdir = $(prefix)/lib/libvisual/actor

LIBS = -L. -L$(prefix)/lib @LIBVISUAL_LIBS@

AM_CFLAGS = -Wall -rdynamic @DEBUG_CFLAGS@ @LIBVISUAL_CFLAGS@ -O3

INCLUDES = $(all_includes) -I$(top_srcdir) 

actor_xpyre_la_LDFLAGS = -module -avoid-version 

actor_xpyre_la_SOURCES = actor_xpyre.c \
			 actor_xpyre.h \
			 draw.c \
			 draw.h

