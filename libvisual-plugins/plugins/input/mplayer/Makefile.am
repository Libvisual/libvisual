## Process this file with automake to generate a Makefile.in

lib_LTLIBRARIES = input_mplayer.la

libdir = $(prefix)/lib/libvisual/input

LIBS = -L. -L$(prefix)/lib @LIBVISUAL_LIBS@

AM_CFLAGS = -Wall -rdynamic @LIBVISUAL_CFLAGS@

INCLUDES = $(all_includes) -I$(top_srcdir) 

input_mplayer_la_LDFLAGS = -module -avoid-version 

input_mplayer_la_SOURCES = input_mplayer.c

