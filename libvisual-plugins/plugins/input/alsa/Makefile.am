## Process this file with automake to generate a Makefile.in

lib_LTLIBRARIES = $(ALSA)

EXTRA_LTLIBRARIES = input_alsa.la

libdir = @LIBVISUAL_PLUGINS_BASE_DIR@/input

LIBS += -L. -L$(prefix)/lib @ALSA_LIBS@ @LIBVISUAL_LIBS@

AM_CFLAGS = @ALSA_CFLAGS@ @LIBVISUAL_CFLAGS@

INCLUDES = $(all_includes) -I$(top_srcdir) -I$(top_builddir)

input_alsa_la_LDFLAGS = -rpath $(libdir) -module -avoid-version 

input_alsa_la_SOURCES = input_alsa.c

