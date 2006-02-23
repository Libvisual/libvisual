## Process this file with automake to generate a Makefile.in

actor_plugin_LTLIBRARIES = actor_gdkpixbuf.la

LIBS += $(LIBVISUAL_LIBS) $(GDKPIXBUF_LIBS)

AM_CFLAGS = $(LIBVISUAL_CFLAGS) $(GDKPIXBUF_CFLAGS)

INCLUDES = $(all_includes) -I$(top_srcdir) 

actor_gdkpixbuf_la_LDFLAGS = -module -avoid-version 

actor_gdkpixbuf_la_SOURCES = actor_gdkpixbuf.c

