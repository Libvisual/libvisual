## Process this file with automake to generate a Makefile.in

lib_LTLIBRARIES = $(FLASH)

EXTRA_LTLIBRARIES = morph_flash.la

libdir = @LIBVISUAL_PLUGINS_BASE_DIR@/morph

LIBS += -L. -L$(prefix)/lib @LIBVISUAL_LIBS@

AM_CFLAGS = @LIBVISUAL_CFLAGS@

INCLUDES = $(all_includes) \
	-I$(top_srcdir) 

morph_flash_la_LDFLAGS = -rpath $(libdir) -module -avoid-version 

morph_flash_la_SOURCES = morph_flash.c

