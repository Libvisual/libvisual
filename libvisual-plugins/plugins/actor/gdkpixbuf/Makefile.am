## Process this file with automake to generate a Makefile.in

lib_LTLIBRARIES = $(GDKPIXBUF)

EXTRA_LTLIBRARIES = actor_gdkpixbuf.la

libdir = @LIBVISUAL_PLUGINS_BASE_DIR@/actor

LIBS += -L. -L$(prefix)/lib @LIBVISUAL_LIBS@ @GDKPIXBUF_LIBS@

AM_CFLAGS = @LIBVISUAL_CFLAGS@ @GDKPIXBUF_CFLAGS@

INCLUDES = $(all_includes) -I$(top_srcdir) 

actor_gdkpixbuf_la_LDFLAGS = -rpath $(libdir) -module -avoid-version 

actor_gdkpixbuf_la_SOURCES = actor_gdkpixbuf.c \
			     actor_gdkpixbuf.h

