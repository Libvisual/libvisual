## Process this file with automake to generate a Makefile.in

lib_LTLIBRARIES = glx.la glx_new.la

LIBS = -L. -L$(prefix)/lib @LIBVISUAL_LIBS@

AM_CFLAGS = -Wall -rdynamic -Wall -Werror -Wno-unused-variable  @LIBVISUAL_CFLAGS@ -O0 -g

INCLUDES = $(all_includes) -I$(top_srcdir) -I$(top_srcdir)/lvdisplay

glx_la_LDFLAGS = -module -avoid-version
glx_new_la_LDFLAGS = -module -avoid-version

glx_la_SOURCES = glx.c
glx_new_la_SOURCES = glx_new.c
