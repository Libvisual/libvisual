## Process this file with automake to generate a Makefile.in

actor_plugin_LTLIBRARIES = actor_lv_gltest.la

LIBS += -lGL -lGLU $(LIBVISUAL_LIBS)

AM_CFLAGS = $(LIBVISUAL_CFLAGS)

INCLUDES = $(all_includes) -I$(top_srcdir) 

actor_lv_gltest_la_LDFLAGS = -module -avoid-version 

actor_lv_gltest_la_SOURCES = actor_lv_gltest.c

