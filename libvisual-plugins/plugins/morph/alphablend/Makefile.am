## Process this file with automake to generate a Makefile.in

pkglib_LTLIBRARIES = $(ALPHABLEND)

EXTRA_LTLIBRARIES = morph_alphablend.la

pkglibdir = $(prefix)/lib/libvisual/morph

LIBS += -L. -L$(prefix)/lib @LIBVISUAL_LIBS@

AM_CFLAGS = -Wall @DEBUG_CFLAGS@ @LIBVISUAL_CFLAGS@ -O3

INCLUDES = $(all_includes) -I$(top_srcdir) 

morph_alphablend_la_LDFLAGS = -rpath '$(pkglibdir)' -module -avoid-version 

morph_alphablend_la_SOURCES = morph_alphablend.c

