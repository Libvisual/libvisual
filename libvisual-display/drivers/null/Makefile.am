## Process this file with automake to generate a Makefile.in

lib_LTLIBRARIES = null.la

LIBS = -L. -L$(prefix)/lib @LIBVISUAL_LIBS@

AM_CFLAGS = -Wall -rdynamic -Wall -Werror -Wno-unused-variable  @LIBVISUAL_CFLAGS@ -O0 -g

INCLUDES = $(all_includes) -I$(top_srcdir) -I$(top_srcdir)/lvdisplay

null_la_LDFLAGS = -module -avoid-version
null_la_SOURCES = null.c

