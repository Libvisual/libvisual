lib_LTLIBRARIES = input_esd.la

libdir = $(prefix)/lib/libvisual/input

LIBS = -L. -L$(prefix)/lib @LIBESD_LIBS@ @LV_LIBS@

CFLAGS = @LIBESD_CFLAGS@

INCLUDES = $(all_includes) \
	-I$(top_srcdir) 

LDFLAGS =

input_esd_la_LDFLAGS = -module -avoid-version 

input_esd_la_SOURCES = input_esd.c

