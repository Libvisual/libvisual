## Process this file with automake to generate a Makefile.in

lib_LTLIBRARIES = $(ALPHABLEND)

EXTRA_LTLIBRARIES = morph_alphablend.la

libdir = @LIBVISUAL_PLUGINS_BASE_DIR@/morph

LIBS += -L. -L$(prefix)/lib @LIBVISUAL_LIBS@

AM_CFLAGS = @LIBVISUAL_CFLAGS@

INCLUDES = $(all_includes) -I$(top_srcdir) 

morph_alphablend_la_LDFLAGS = -rpath $(libdir) -module -avoid-version 

morph_alphablend_la_SOURCES = morph_alphablend.c

