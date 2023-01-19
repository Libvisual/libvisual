## Process this file with automake to generate a Makefile.in

actor_plugin_LTLIBRARIES = actor_flower.la

actor_flower_la_LDFLAGS = -module -avoid-version 
actor_flower_la_CFLAGS = -I$(top_srcdir) $(LIBVISUAL_CFLAGS) $(GL_CFLAGS) $(GLU_CFLAGS)
actor_flower_la_LIBADD = $(LIBVISUAL_LIBS) $(GL_LIBS) $(GLU_LIBS)
actor_flower_la_SOURCES = \
	actor_flower.c \
	main.c \
	main.h \
	notch.c \
	notch.h
