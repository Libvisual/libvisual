## Process this file with automake to generate a Makefile.in
libdir=@LIBVISUAL_PLUGINS_BASE_DIR@/display

lib_LTLIBRARIES = testactor.la testactor2.la

LIBS = -L. -L$(prefix)/lib @LIBVISUAL_LIBS@

AM_CFLAGS = @LIBVISUAL_CFLAGS@

INCLUDES = $(all_includes) -I$(top_srcdir)

testactor_la_LDFLAGS = -module -avoid-version -lGLU
testactor_la_SOURCES = testactor.c

testactor2_la_LDFLAGS = -module -avoid-version -lGLU
testactor2_la_SOURCES = testactor2.c
