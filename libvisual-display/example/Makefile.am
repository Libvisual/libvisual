
SUBDIRS = 

noinst_PROGRAMS = test

AM_CFLAGS = -I$(top_srcdir)/lvdisplay -DPLUGINSDIR=\"@LIBVISUAL_PLUGINS_BASE_DIR@\"

test_SOURCES = main.c
test_LDADD = -L/usr/X11R6/lib \
    -L$(top_srcdir)/lvdisplay \
    ../lvdisplay/liblvdisplay.la \
    -lGL -lvisual -lpthread
