## Process this file with automake to generate a Makefile.in

lib_LTLIBRARIES = $(ESD)

EXTRA_LTLIBRARIES = input_esd.la

libdir = @LIBVISUAL_PLUGINS_BASE_DIR@/input

LIBS += -L. -L$(prefix)/lib @LIBESD_LIBS@ @LIBVISUAL_LIBS@

AM_CFLAGS = @LIBESD_CFLAGS@ @LIBVISUAL_CFLAGS@

INCLUDES = $(all_includes) -I$(top_srcdir) 

input_esd_la_LDFLAGS = -rpath $(libdir) -module -avoid-version 

input_esd_la_SOURCES = input_esd.c

