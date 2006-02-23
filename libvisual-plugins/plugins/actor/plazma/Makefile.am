## Process this file with automake to generate a Makefile.in

actor_plugin_LTLIBRARIES = actor_plazma.la

LIBS += $(LIBVISUAL_LIBS)

AM_CFLAGS = $(LIBVISUAL_CFLAGS)

INCLUDES = $(all_includes) -I$(top_srcdir) 

actor_plazma_la_LDFLAGS = -module -avoid-version 

actor_plazma_la_SOURCES = actor_plazma.c \
			  actor_plazma.h \
			  plazma.c \
			  plazma.h

