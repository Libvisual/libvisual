## Process this file with automake to generate a Makefile.in

lib_LTLIBRARIES = $(JAKDAW)

EXTRA_LTLIBRARIES = actor_jakdaw.la

libdir = @LIBVISUAL_PLUGINS_BASE_DIR@/actor

LIBS += -L. -L$(prefix)/lib @LIBVISUAL_LIBS@

AM_CFLAGS = @LIBVISUAL_CFLAGS@

INCLUDES = $(all_includes) -I$(top_srcdir) 

actor_jakdaw_la_LDFLAGS = -rpath $(libdir) -module -avoid-version 

actor_jakdaw_la_SOURCES = actor_jakdaw.c \
			  actor_jakdaw.h \
			  feedback.c \
			  feedback.h \
			  plotter.c \
			  plotter.h
			  

