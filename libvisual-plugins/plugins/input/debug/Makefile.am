## Process this file with automake to generate a Makefile.in

lib_LTLIBRARIES = $(INPUTDEBUG)

EXTRA_LTLIBRARIES = input_debug.la

libdir = @LIBVISUAL_PLUGINS_BASE_DIR@/input

LIBS += -L. -L$(prefix)/lib @LIBVISUAL_LIBS@

AM_CFLAGS = @LIBVISUAL_CFLAGS@

INCLUDES = $(all_includes) -I$(top_srcdir) -I$(top_builddir)

input_debug_la_LDFLAGS = -rpath $(libdir) -module -avoid-version 

input_debug_la_SOURCES = input_debug.c

