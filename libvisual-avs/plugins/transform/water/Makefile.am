## Process this file with automake to generate a Makefile.in

lib_LTLIBRARIES = $(AVS_WATER)

EXTRA_LTLIBRARIES = transform_avs_water.la

libdir = @LIBVISUAL_PLUGINS_BASE_DIR@/transform

LIBS += -L. $(GLIB_LIBS) -L$(prefix)/lib @LIBVISUAL_LIBS@

AM_CFLAGS = $(GLIB_CFLAGS) @LIBVISUAL_CFLAGS@

INCLUDES = $(all_includes) \
	-I$(top_srcdir) \
	-I$(top_srcdir)/common \
    -I$(top_srcdir)/visscript

transform_avs_water_la_LDFLAGS = -rpath $(libdir) -module -avoid-version 

transform_avs_water_la_SOURCES = transform_avs_water.c

transform_avs_water_la_LIBADD = ../../../common/libavs.la

