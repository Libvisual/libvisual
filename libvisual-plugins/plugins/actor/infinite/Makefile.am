## Process this file with automake to generate a Makefile.in

lib_LTLIBRARIES = actor_infinite.la

libdir = $(prefix)/lib/libvisual/actor

LIBS = -L. -L$(prefix)/lib @LIBVISUAL_LIBS@

AM_CFLAGS = -Wall -rdynamic @DEBUG_CFLAGS@ @LIBVISUAL_CFLAGS@ -O3

INCLUDES = $(all_includes) -I$(top_srcdir) 

actor_infinite_la_LDFLAGS = -module -avoid-version 

actor_infinite_la_SOURCES = compute.c display.c file.c lv_infinite.c renderer.c

