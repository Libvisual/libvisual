## Process this file with automake to generate a Makefile.in

actor_plugin_LTLIBRARIES = actor_nastyfft.la

AM_CFLAGS = $(LIBVISUAL_CFLAGS)

INCLUDES = $(all_includes) -I$(top_srcdir) 
LIBS += -lGL -lGLU $(LIBVISUAL_LIBS)

actor_nastyfft_la_LDFLAGS = -module -avoid-version 
actor_nastyfft_la_SOURCES = actor_nastyfft.c
