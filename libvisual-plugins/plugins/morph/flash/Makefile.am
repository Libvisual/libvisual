## Process this file with automake to generate a Makefile.in

pkglib_LTLIBRARIES = $(FLASH)

EXTRA_LTLIBRARIES = morph_flash.la

pkglibdir = $(prefix)/lib/libvisual/morph

LIBS += -L. -L$(prefix)/lib @LIBVISUAL_LIBS@

AM_CFLAGS = -Wall -rdynamic @DEBUG_CFLAGS@ @LIBVISUAL_CFLAGS@ -O3

INCLUDES = $(all_includes) \
	-I$(top_srcdir) 

morph_flash_la_LDFLAGS = -rpath '$(pkglibdir)' -module -avoid-version 

morph_flash_la_SOURCES = morph_flash.c

