## Process this file with automake to generate a Makefile.in

lib_LTLIBRARIES = actor_nebulus.la

libdir = $(prefix)/lib/libvisual/actor

LIBS = -L. -L$(prefix)/lib -lGL -lGLU @LIBVISUAL_LIBS@

AM_CFLAGS = -Wall @DEBUG_CFLAGS@ @LIBVISUAL_CFLAGS@ -O3

INCLUDES = $(all_includes) -I$(top_srcdir) 

actor_nebulus_la_LDFLAGS = -module -avoid-version 

actor_nebulus_la_SOURCES = nebulus.c nebulus.h \
		    draw_gl.c spectrum.c face.c \
		    face_data.c knot.c glthreads.c \
		    tunnel.c tentacles.c twist.c child.c \
		    child_data.c energy.c background.c \
		    tunnel_texture.c tentacle_texture.c \
		    twist_texture.c child_texture.c energy_texture.c

