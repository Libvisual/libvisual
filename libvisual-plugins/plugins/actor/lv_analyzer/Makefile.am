## Process this file with automake to generate a Makefile.in

lib_LTLIBRARIES = $(ANALYZER)

EXTRA_LTLIBRARIES = actor_lv_analyzer.la

libdir = @LIBVISUAL_PLUGINS_BASE_DIR@/actor

LIBS += -L. -L$(prefix)/lib @LIBVISUAL_LIBS@

AM_CFLAGS = @LIBVISUAL_CFLAGS@

INCLUDES = $(all_includes) \
	-I$(top_srcdir) 

actor_lv_analyzer_la_LDFLAGS = -rpath $(libdir) -module -avoid-version 

actor_lv_analyzer_la_SOURCES = actor_lv_analyzer.c

