lib_LTLIBRARIES = morph_tentacle.la

libdir = $(prefix)/lib/libvisual/morph

LIBS = -L. -L$(prefix)/lib @LV_LIBS@

CFLAGS =

INCLUDES = $(all_includes) \
	-I$(top_srcdir) 

morph_tentacle_la_LDFLAGS = -module -avoid-version 

morph_tentacle_la_SOURCES = morph_tentacle.c

