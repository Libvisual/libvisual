lib_LTLIBRARIES = video_test.la

libdir = @LIBVISUAL_PLUGINS_BASE_DIR@/actor

LIBS = -L. -L$(prefix)/lib @LIBVISUAL_LIBS@

AM_CFLAGS = @LIBVISUAL_CFLAGS@

INCLUDES = $(all_includes) \
	-I$(top_srcdir) 

video_test_la_LDFLAGS = -rpath $(libdir) -module -avoid-version 

video_test_la_SOURCES = video_test.c

