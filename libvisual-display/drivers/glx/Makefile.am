## Process this file with automake to generate a Makefile.in
libdir=@LIBVISUAL_PLUGINS_BASE_DIR@/display

lib_LTLIBRARIES = glx.la

LIBS = -L. -L$(prefix)/lib @LIBVISUAL_LIBS@

AM_CFLAGS = @LIBVISUAL_CFLAGS@

INCLUDES = $(all_includes) -I$(top_srcdir) -I$(top_srcdir)/lvdisplay

glx_la_LDFLAGS = -module -avoid-version -lGL

glx_la_SOURCES = glx.c
