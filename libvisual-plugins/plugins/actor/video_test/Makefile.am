lib_LTLIBRARIES = video_test.la

libdir = $(prefix)/lib/libvisual/actor

LIBS = -L. -L$(prefix)/lib @LV_LIBS@

CFLAGS =

INCLUDES = $(all_includes) \
	-I$(top_srcdir) 

video_test_la_LDFLAGS = -module -avoid-version 

video_test_la_SOURCES = video_test.c

