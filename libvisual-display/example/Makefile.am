
SUBDIRS = 

noinst_PROGRAMS = test

AM_CFLAGS = -I$(top_srcdir)/lvdisplay -O0 -g

test_SOURCES = main.c
test_LDADD = -L/usr/X11R6/lib -L$(top_srcdir)/lvdisplay -llvdisplay -lGL -lvisual -lpthread
