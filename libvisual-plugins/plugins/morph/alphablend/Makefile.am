## Process this file with automake to generate a Makefile.in

lib_LTLIBRARIES = morph_alphablend.la

libdir = $(prefix)/lib/libvisual/morph

LIBS = -L. -L$(prefix)/lib @LV_LIBS@

AM_CFLAGS =

INCLUDES = $(all_includes) \
	-I$(top_srcdir) 

morph_alphablend_la_LDFLAGS = -module -avoid-version 

morph_alphablend_la_SOURCES = morph_alphablend.c

