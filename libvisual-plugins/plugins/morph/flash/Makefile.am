lib_LTLIBRARIES = morph_flash.la

libdir = $(prefix)/lib/libvisual/morph

LIBS = -L. -L$(prefix)/lib @LV_LIBS@

CFLAGS =

INCLUDES = $(all_includes) \
	-I$(top_srcdir) 

morph_flash_la_LDFLAGS = -module -avoid-version 

morph_flash_la_SOURCES = morph_flash.c

