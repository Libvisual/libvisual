## Process this file with automake to generate a Makefile.in

lib_LTLIBRARIES = $(SLIDE)

EXTRA_LTLIBRARIES = morph_slide.la

libdir = @LIBVISUAL_PLUGINS_BASE_DIR@/morph

LIBS += -L. -L$(prefix)/lib @LIBVISUAL_LIBS@

AM_CFLAGS = @LIBVISUAL_CFLAGS@

INCLUDES = $(all_includes) -I$(top_srcdir) 

morph_slide_la_LDFLAGS = -rpath $(libdir) -module -avoid-version 

morph_slide_la_SOURCES = morph_slide.c

