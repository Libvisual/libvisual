## Process this file with automake to generate a Makefile.in

input_plugin_LTLIBRARIES = input_esd.la

AM_CFLAGS = $(LIBESD_CFLAGS) $(LIBVISUAL_CFLAGS)

INCLUDES = $(all_includes) -I$(top_srcdir)

input_esd_la_LDFLAGS = -module -avoid-version 
input_esd_la_LIBADD = $(LIBESD_LIBS) $(LIBVISUAL_LIBS)
input_esd_la_SOURCES = input_esd.c
