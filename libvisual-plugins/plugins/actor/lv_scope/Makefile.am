## Process this file with automake to generate a Makefile.in

lib_LTLIBRARIES = $(SCOPE)

EXTRA_LTLIBRARIES = actor_lv_scope.la

libdir = @LIBVISUAL_PLUGINS_BASE_DIR@/actor

LIBS += -L. -L$(prefix)/lib @LIBVISUAL_LIBS@

AM_CFLAGS = @LIBVISUAL_CFLAGS@

INCLUDES = $(all_includes) -I$(top_srcdir) 

actor_lv_scope_la_LDFLAGS = -rpath $(libdir) -module -avoid-version 

actor_lv_scope_la_SOURCES = actor_lv_scope.c

