## Process this file with automake to generate a Makefile.in

lib_LTLIBRARIES = actor_lv_scope.la

libdir = $(prefix)/lib/libvisual/actor

LIBS = -L. -L$(prefix)/lib @LV_LIBS@

AM_CFLAGS =

INCLUDES = $(all_includes) \
	-I$(top_srcdir) 

actor_lv_scope_la_LDFLAGS = -module -avoid-version 

actor_lv_scope_la_SOURCES = actor_lv_scope.c

