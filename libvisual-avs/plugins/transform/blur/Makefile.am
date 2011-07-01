## Process this file with automake to generate a Makefile.in

lib_LTLIBRARIES = $(AVS_BLUR)

EXTRA_LTLIBRARIES = transform_avs_blur.la

libdir = @LIBVISUAL_PLUGINS_BASE_DIR@/transform

LIBS += -L. $(GLIB_LIBS) -L$(prefix)/lib @LIBVISUAL_LIBS@

AM_CFLAGS = $(GLIB_CFLAGS) @LIBVISUAL_CFLAGS@ -fopenmp

INCLUDES = $(all_includes) \
	-I$(top_srcdir) \
	-I$(top_srcdir)/common \
	-I$(top_srcdir)/src

transform_avs_blur_la_LDFLAGS = -rpath $(libdir) -module -avoid-version 

transform_avs_blur_la_SOURCES = transform_avs_blur.c

transform_avs_blur_la_LIBADD = ../../../common/libavs.la

