## Process this file with automake to generate a Makefile.in

imagesdir = $(datadir)/libvisual/actor/actor_nebulus
images_DATA = background_texture.bmp \
	      child_texture.bmp \
	      energy_texture.bmp \
	      tentacle_texture.bmp \
	      tunnel_texture.bmp \
	      twist_texture.bmp

EXTRA_DIST = $(images_DATA)

lib_LTLIBRARIES = actor_nebulus.la

libdir = @LIBVISUAL_PLUGINS_BASE_DIR@/actor

LIBS = -L. -L$(prefix)/lib -lGL -lGLU @LIBVISUAL_LIBS@

AM_CFLAGS = @LIBVISUAL_CFLAGS@ -DBITMAP_DIR=\"$(imagesdir)/\"

INCLUDES = -I$(top_srcdir)/src -I$(top_builddir)

actor_nebulus_la_LDFLAGS = -module -avoid-version 

actor_nebulus_la_SOURCES = nebulus.c nebulus.h \
		    draw_gl.c spectrum.c face.c \
		    face_data.c knot.c glthreads.c \
		    tunnel.c tentacles.c twist.c child.c \
		    child_data.c energy.c

