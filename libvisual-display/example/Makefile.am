
SUBDIRS = plugins

noinst_PROGRAMS = test

AM_CFLAGS = -I$(top_srcdir) -DPLUGINSDIR=\"@LIBVISUAL_PLUGINS_BASE_DIR@\" \
	@LIBVISUAL_CFLAGS@

test_SOURCES = main.c
test_LDADD = -L/usr/X11R6/lib \
    ../lvdisplay/liblvdisplay.la \
    -lvisual -lpthread
