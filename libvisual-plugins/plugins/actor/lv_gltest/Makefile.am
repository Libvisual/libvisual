## Process this file with automake to generate a Makefile.in

lib_LTLIBRARIES = $(GLTEST)

EXTRA_LTLIBRARIES = actor_lv_gltest.la

libdir = @LIBVISUAL_PLUGINS_BASE_DIR@/actor

LIBS += -L. -L$(prefix)/lib -lGL -lGLU @LIBVISUAL_LIBS@

AM_CFLAGS = @LIBVISUAL_CFLAGS@

INCLUDES = $(all_includes) -I$(top_srcdir) 

actor_lv_gltest_la_LDFLAGS = -rpath $(libdir) -module -avoid-version 

actor_lv_gltest_la_SOURCES = actor_lv_gltest.c

