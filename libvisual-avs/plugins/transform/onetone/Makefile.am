## Process this file with automake to generate a Makefile.in

lib_LTLIBRARIES = $(AVS_ONETONE)

EXTRA_LTLIBRARIES = transform_avs_onetone.la

libdir = @LIBVISUAL_PLUGINS_BASE_DIR@/transform

LIBS += -L. -L$(prefix)/lib @LIBVISUAL_LIBS@

AM_CFLAGS = @LIBVISUAL_CFLAGS@

INCLUDES = $(all_includes) \
	-I$(top_srcdir) \
	-I$(top_srcdir)/common

transform_avs_onetone_la_LDFLAGS = -rpath $(libdir) -module -avoid-version 

transform_avs_onetone_la_SOURCES = transform_avs_onetone.c

transform_avs_onetone_la_LIBADD = ../../../common/libavs.la

