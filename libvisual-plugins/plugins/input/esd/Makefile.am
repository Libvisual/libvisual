## Process this file with automake to generate a Makefile.in

pkglib_LTLIBRARIES = $(ESD)

EXTRA_LTLIBRARIES = input_esd.la

pkglibdir = $(prefix)/lib/libvisual/input

LIBS += -L. -L$(prefix)/lib @LIBESD_LIBS@ @LIBVISUAL_LIBS@

AM_CFLAGS = -Wall -rdynamic @LIBESD_CFLAGS@ @LIBVISUAL_CFLAGS@ -O3

INCLUDES = $(all_includes) -I$(top_srcdir) 

input_esd_la_LDFLAGS = -rpath '$(pkglibdir)' -module -avoid-version 

input_esd_la_SOURCES = input_esd.c

