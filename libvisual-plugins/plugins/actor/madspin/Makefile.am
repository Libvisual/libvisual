## Process this file with automake to generate a Makefile.in

imagesdir = $(datadir)/libvisual/actor/actor_madspin
images_DATA = star1.bmp \
	      star2.bmp

EXTRA_DIST = $(images_DATA)

pkglib_LTLIBRARIES = $(MADSPIN)

EXTRA_LTLIBRARIES = actor_madspin.la

pkglibdir = $(prefix)/lib/libvisual/actor

LIBS += -L. -L$(prefix)/lib -lGL @LIBVISUAL_LIBS@

AM_CFLAGS = -Wall -rdynamic @DEBUG_CFLAGS@ @LIBVISUAL_CFLAGS@ -O3 -DSTAR_DIR=\""$(imagesdir)/"\"

INCLUDES = $(all_includes) -I$(top_srcdir) 

actor_madspin_la_LDFLAGS = -rpath '$(pkglibdir)' -module -avoid-version 

actor_madspin_la_SOURCES = madspin.c

