## Process this file with automake to generate a Makefile.in

morph_plugin_LTLIBRARIES = morph_alphablend.la

AM_CFLAGS = $(LIBVISUAL_CFLAGS)

INCLUDES = $(all_includes) -I$(top_srcdir) 

morph_alphablend_la_LDFLAGS = -module -avoid-version 
morph_alphablend_la_LIBADD = $(LIBVISUAL_LIBS)
morph_alphablend_la_SOURCES = morph_alphablend.c
