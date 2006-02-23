## Process this file with automake to generate a Makefile.in

actor_plugin_LTLIBRARIES = actor_flower.la

LIBS += -lGL -lGLU $(LIBVISUAL_LIBS)

AM_CFLAGS = $(LIBVISUAL_CFLAGS)

INCLUDES = $(all_includes) -I$(top_srcdir) 

actor_flower_la_LDFLAGS = -module -avoid-version 

actor_flower_la_SOURCES = actor_flower.c \
			  main.c \
			  main.h \
			  notch.c \
			  notch.h
