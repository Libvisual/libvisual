## Process this file with automake to generate a Makefile.in

imagesdir = $(datadir)/libvisual/actor/actor_madspin
images_DATA = star1.bmp \
	      star2.bmp

EXTRA_DIST = $(images_DATA)

lib_LTLIBRARIES = $(MADSPIN)

EXTRA_LTLIBRARIES = actor_madspin.la

libdir = @LIBVISUAL_PLUGINS_BASE_DIR@/actor

LIBS += -L. -L$(prefix)/lib -lGL @LIBVISUAL_LIBS@

AM_CFLAGS = @LIBVISUAL_CFLAGS@ -DSTAR_DIR=\""$(imagesdir)/"\"

INCLUDES = $(all_includes) -I$(top_srcdir) 

actor_madspin_la_LDFLAGS = -rpath $(libdir) -module -avoid-version 

actor_madspin_la_SOURCES = madspin.c

