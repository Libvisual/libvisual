## Process this file with automake to generate a Makefile.in

lib_LTLIBRARIES = $(AVS_DMOVEMENT)

EXTRA_LTLIBRARIES = transform_avs_dmovement.la

libdir = @LIBVISUAL_PLUGINS_BASE_DIR@/transform

LIBS += -L. -L$(prefix)/lib @LIBVISUAL_LIBS@

AM_CFLAGS = @LIBVISUAL_CFLAGS@

INCLUDES = $(all_includes) \
	-I$(top_srcdir) \
	-I$(top_srcdir)/common \
	-I$(top_srcdir)/visscript \
	-I$(top_srcdir)/src

transform_avs_dmovement_la_LDFLAGS = -rpath $(libdir) -module -avoid-version 

transform_avs_dmovement_la_SOURCES = transform_avs_dmovement.c

transform_avs_dmovement_la_LIBADD = ../../../common/libavs.la ../../../visscript/libvisscript.la

