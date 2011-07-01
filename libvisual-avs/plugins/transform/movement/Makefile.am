## Process this file with automake to generate a Makefile.in

lib_LTLIBRARIES = $(AVS_MOVEMENT)

EXTRA_LTLIBRARIES = transform_avs_movement.la

libdir = @LIBVISUAL_PLUGINS_BASE_DIR@/transform

LIBS += -L. $(GLIB_LIBS) -L$(prefix)/lib @LIBVISUAL_LIBS@

AM_CFLAGS = $(GLIB_CFLAGS) @LIBVISUAL_CFLAGS@ -fopenmp

INCLUDES = $(all_includes) \
	-I$(top_srcdir) \
	-I$(top_srcdir)/common \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/visscript

transform_avs_movement_la_LDFLAGS = -rpath $(libdir) -module -avoid-version 

transform_avs_movement_la_SOURCES = transform_avs_movement.c

transform_avs_movement_la_LIBADD = ../../../common/libavs.la ../../../visscript/libvisscript.la

