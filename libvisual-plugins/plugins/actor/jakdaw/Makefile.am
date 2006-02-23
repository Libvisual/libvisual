## Process this file with automake to generate a Makefile.in

actor_plugin_LTLIBRARIES = actor_jakdaw.la

LIBS += $(LIBVISUAL_LIBS)

AM_CFLAGS = $(LIBVISUAL_CFLAGS)

INCLUDES = $(all_includes) -I$(top_srcdir) 

actor_jakdaw_la_LDFLAGS = -module -avoid-version 

actor_jakdaw_la_SOURCES = actor_jakdaw.c \
			  actor_jakdaw.h \
			  feedback.c \
			  feedback.h \
			  plotter.c \
			  plotter.h
