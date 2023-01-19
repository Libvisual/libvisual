## Process this file with automake to generate a Makefile.in

imagesdir = $(LIBVISUAL_PLUGINS_DATA_DIR)/actor/actor_madspin
images_DATA = star1.bmp \
	      star2.bmp

EXTRA_DIST = $(images_DATA)

actor_plugin_LTLIBRARIES = actor_madspin.la

actor_madspin_la_CFLAGS = -I$(top_srcdir) $(LIBVISUAL_CFLAGS) -DSTAR_DIR=\""$(imagesdir)/"\" $(GL_CFLAGS) $(GLU_CFLAGS)
actor_madspin_la_LDFLAGS = -module -avoid-version 
actor_madspin_la_LIBADD = $(LIBVISUAL_LIBS) $(GL_LIBS)
actor_madspin_la_SOURCES = madspin.c
