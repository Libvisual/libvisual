## Process this file with automake to generate a Makefile.in

lib_LTLIBRARIES = liblvdisplay.la

LIBS = -L. -L$(prefix)/lib @LIBVISUAL_LIBS@

AM_CFLAGS = -Wall -rdynamic -Wall -Werror -Wno-unused-variable  @LIBVISUAL_CFLAGS@ -O0 -g

INCLUDES = $(all_includes) -I$(top_srcdir) 

liblvdisplay_la_LDFLAGS =
liblvdisplay_la_LIBADD = -lvisual

liblvdisplay_la_SOURCES = lv_display.c \
			  lv_display.h

