## Process this file with automake to generate a Makefile.in

lib_LTLIBRARIES = morph_tentacle.la

libdir = $(prefix)/lib/libvisual/morph

LIBS = -L. -L$(prefix)/lib @LIBVISUAL_LIBS@

AM_CFLAGS = -Wall -rdynamic @DEBUG_CFLAGS@ @LIBVISUAL_CFLAGS@ -O3

INCLUDES = $(all_includes) -I$(top_srcdir) 

morph_tentacle_la_LDFLAGS = -module -avoid-version 

morph_tentacle_la_SOURCES = morph_tentacle.c

