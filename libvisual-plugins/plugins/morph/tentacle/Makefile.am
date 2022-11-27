## Process this file with automake to generate a Makefile.in

morph_plugin_LTLIBRARIES = morph_tentacle.la

AM_CFLAGS = $(LIBVISUAL_CFLAGS)

INCLUDES = $(all_includes) -I$(top_srcdir) 

morph_tentacle_la_LDFLAGS = -module -avoid-version 
morph_tentacle_la_LIBADD = $(LIBVISUAL_LIBS)
morph_tentacle_la_SOURCES = morph_tentacle.c
