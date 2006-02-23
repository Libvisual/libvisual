## Process this file with automake to generate a Makefile.in

actor_plugin_LTLIBRARIES = actor_goom2.la

LIBS += $(LIBGOOM2_LIBS) $(LIBVISUAL_LIBS)

AM_CFLAGS = $(LIBGOOM2_CFLAGS) $(LIBVISUAL_CFLAGS)

INCLUDES = $(all_includes) -I$(top_srcdir)

actor_goom2_la_LDFLAGS = -module -avoid-version

actor_goom2_la_SOURCES = actor_goom2.c
