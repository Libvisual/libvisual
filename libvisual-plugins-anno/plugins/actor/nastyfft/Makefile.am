## Process this file with automake to generate a Makefile.in

actor_plugin_LTLIBRARIES = actor_nastyfft.la

actor_nastyfft_la_CFLAGS = -I$(top_srcdir) $(LIBVISUAL_CFLAGS) $(X_CFLAGS)
actor_nastyfft_la_LDFLAGS = -module -avoid-version 
actor_nastyfft_la_LIBADD = $(LIBVISUAL_LIBS) $(X_LIBS) -lGL -lGLU
actor_nastyfft_la_SOURCES = actor_nastyfft.c
