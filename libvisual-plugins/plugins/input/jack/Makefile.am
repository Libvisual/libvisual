## Process this file with automake to generate a Makefile.in

lib_LTLIBRARIES = $(JACK)

EXTRA_LTLIBRARIES = input_jack.la

libdir = @LIBVISUAL_PLUGINS_BASE_DIR@/input

LIBS += -L. -L$(prefix)/lib @LIBJACK_LIBS@ @LIBVISUAL_LIBS@

AM_CFLAGS = @LIBJACK_CFLAGS@ @LIBVISUAL_CFLAGS@

INCLUDES = $(all_includes) -I$(top_srcdir) 

input_jack_la_LDFLAGS = -rpath $(libdir) -module -avoid-version 

input_jack_la_SOURCES = input_jack.c

