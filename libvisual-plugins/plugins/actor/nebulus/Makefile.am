## Process this file with automake to generate a Makefile.in

imagesdir = $(LIBVISUAL_PLUGINS_DATA_DIR)/actor/actor_nebulus
images_DATA = background_texture.bmp \
	      child_texture.bmp \
	      energy_texture.bmp \
	      tentacle_texture.bmp \
	      tunnel_texture.bmp \
	      twist_texture.bmp

EXTRA_DIST = $(images_DATA)

actor_plugin_LTLIBRARIES = actor_nebulus.la

LIBS += -lGL -lGLU $(LIBVISUAL_LIBS)

AM_CFLAGS = $(LIBVISUAL_CFLAGS) -DBITMAP_DIR=\"$(imagesdir)/\"

INCLUDES = $(all_includes) -I$(top_srcdir)

actor_nebulus_la_LDFLAGS = -module -avoid-version

actor_nebulus_la_SOURCES = nebulus.c nebulus.h \
		    draw_gl.c spectrum.c face.c \
		    face_data.c knot.c glthreads.c \
		    tunnel.c tentacles.c twist.c child.c \
		    child_data.c energy.c

