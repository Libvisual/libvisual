## Process this file with automake to generate a Makefile.in

pkglib_LTLIBRARIES = $(JACK)

EXTRA_LTLIBRARIES = input_jack.la

pkglibdir = $(prefix)/lib/libvisual/input

LIBS += -L. -L$(prefix)/lib @LIBJACK_LIBS@ @LIBVISUAL_LIBS@

AM_CFLAGS = -Wall -rdynamic @LIBJACK_CFLAGS@ @LIBVISUAL_CFLAGS@ -O3

INCLUDES = $(all_includes) -I$(top_srcdir) 

input_jack_la_LDFLAGS = -rpath '$(pkglibdir)' -module -avoid-version 

input_jack_la_SOURCES = input_jack.c

