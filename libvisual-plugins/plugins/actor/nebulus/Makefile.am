## Process this file with automake to generate a Makefile.in

images_DATA =                  \
	background_texture.bmp \
	child_texture.bmp      \
	energy_texture.bmp     \
	tentacle_texture.bmp   \
	tunnel_texture.bmp     \
	twist_texture.bmp

imagesdir = $(LIBVISUAL_PLUGINS_DATA_DIR)/actor/actor_nebulus

actor_plugin_LTLIBRARIES = actor_nebulus.la

actor_nebulus_la_LDFLAGS = -module -avoid-version

actor_nebulus_la_CFLAGS = \
	-I$(top_srcdir) $(LIBVISUAL_CFLAGS) -DBITMAP_DIR=\"$(imagesdir)/\"

actor_nebulus_la_LIBS = $(LIBVISUAL_LIBS) $(X_LIBS) -lGL -lGLU 

actor_nebulus_la_SOURCES =                   \
	nebulus.c nebulus.h                  \
	draw_gl.c spectrum.c face.c          \
	face_data.c knot.c glthreads.c       \
	tunnel.c tentacles.c twist.c child.c \
	child_data.c energy.c

EXTRA_DIST = $(images_DATA)
