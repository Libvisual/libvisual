## Process this file with automake to generate a Makefile.in

pkglib_LTLIBRARIES = $(SLIDE)

EXTRA_LTLIBRARIES = morph_slide.la

pkglibdir = $(prefix)/lib/libvisual/morph

LIBS += -L. -L$(prefix)/lib @LIBVISUAL_LIBS@

AM_CFLAGS = -Wall -rdynamic @DEBUG_CFLAGS@ @LIBVISUAL_CFLAGS@ -O3

INCLUDES = $(all_includes) -I$(top_srcdir) 

morph_slide_la_LDFLAGS = -rpath '$(pkglibdir)' -module -avoid-version 

morph_slide_la_SOURCES = morph_slide.c

