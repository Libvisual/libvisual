## Process this file with automake to generate a Makefile.in

lib_LTLIBRARIES = input_alsa.la

libdir = $(prefix)/lib/libvisual/input

LIBS = -L. -L$(prefix)/lib @ALSA_LIBS@ @LIBVISUAL_LIBS@

AM_CFLAGS = -Wall -rdynamic @ALSA_CFLAGS@ @LIBVISUAL_CFLAGS@ -O3

INCLUDES = $(all_includes) -I$(top_srcdir) -I$(top_builddir)

input_alsa_la_LDFLAGS = -module -avoid-version 

input_alsa_la_SOURCES = input_alsa.c

