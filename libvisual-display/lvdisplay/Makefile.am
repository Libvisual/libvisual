## Process this file with automake to generate a Makefile.in

library_includedir = $(includedir)/lvdisplay
library_include_HEADERS = lv_display.h

lib_LTLIBRARIES = liblvdisplay.la

LIBS = -L. -L$(prefix)/lib @LIBVISUAL_LIBS@

AM_CFLAGS = @LIBVISUAL_CFLAGS@

INCLUDES = $(all_includes) -I$(top_srcdir) 

liblvdisplay_la_LDFLAGS =
liblvdisplay_la_LIBADD = -lvisual

liblvdisplay_la_SOURCES = lv_display.c lv_fpsctls.c

