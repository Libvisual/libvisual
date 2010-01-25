## Process this file with automake to generate a Makefile.in

lib_LTLIBRARIES = $(AVS)

EXTRA_LTLIBRARIES = actor_AVS.la

libdir = @LIBVISUAL_PLUGINS_BASE_DIR@/actor

LIBS += -L. -L$(prefix)/lib @LIBVISUAL_LIBS@

AM_CFLAGS = @LIBVISUAL_CFLAGS@

INCLUDES = $(all_includes) -I$(top_srcdir) -I$(top_srcdir)/common

actor_AVS_la_LDFLAGS = -rpath $(libdir) -module -avoid-version 

actor_AVS_la_SOURCES = actor_AVS.c \
		       avs_parse.c \
		       avs_parse.h \
		       avs_serialize.c \
		       avs_serialize.h \
		       lvavs_preset.c \
		       lvavs_preset.h \
		       lvavs_pipeline.c \
		       lvavs_pipeline.h

actor_AVS_la_LIBADD = ../common/libavs.la
