## Process this file with automake to generate a Makefile.in

lib_LTLIBRARIES = actor_gdkpixbuf.la

libdir = $(prefix)/lib/libvisual/actor

LIBS = -L. -L$(prefix)/lib @LIBVISUAL_LIBS@ @GDKPIXBUF_LIBS@

AM_CFLAGS = -Wall -rdynamic @DEBUG_CFLAGS@ @LIBVISUAL_CFLAGS@ @GDKPIXBUF_CFLAGS@ -O3

INCLUDES = $(all_includes) -I$(top_srcdir) 

actor_gdkpixbuf_la_LDFLAGS = -module -avoid-version 

actor_gdkpixbuf_la_SOURCES = actor_gdkpixbuf.c \
			     actor_gdkpixbuf.h

