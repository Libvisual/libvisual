lib_LTLIBRARIES = video_test.la

libdir = $(prefix)/lib/libvisual/actor

LIBS = -L. -L$(prefix)/lib @LIBVISUAL_LIBS@

AM_CFLAGS = -Wall -rdynamic @DEBUG_CFLAGS@ @LIBVISUAL_CFLAGS@ -O3

INCLUDES = $(all_includes) \
	-I$(top_srcdir) 

video_test_la_LDFLAGS = -module -avoid-version 

video_test_la_SOURCES = video_test.c

