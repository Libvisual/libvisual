## Process this file with automake to generate a Makefile.in

pkglib_LTLIBRARIES = $(GLTEST)

EXTRA_LTLIBRARIES = actor_lv_gltest.la

pkglibdir = $(prefix)/lib/libvisual/actor

LIBS += -L. -L$(prefix)/lib -lGL -lGLU @LIBVISUAL_LIBS@

AM_CFLAGS = -Wall -rdynamic @DEBUG_CFLAGS@ @LIBVISUAL_CFLAGS@ -O3

INCLUDES = $(all_includes) -I$(top_srcdir) 

actor_lv_gltest_la_LDFLAGS = -rpath '$(pkglibdir)' -module -avoid-version 

actor_lv_gltest_la_SOURCES = actor_lv_gltest.c

