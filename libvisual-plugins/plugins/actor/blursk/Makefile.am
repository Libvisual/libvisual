## Process this file with automake to generate a Makefile.in

actor_plugin_LTLIBRARIES = actor_blursk.la

actor_blursk_la_LIBADD = $(LIBVISUAL_LIBS)

AM_CFLAGS = $(LIBVISUAL_CFLAGS)

INCLUDES = $(all_includes) -I$(top_srcdir) 

actor_blursk_la_LDFLAGS = -module -avoid-version 

actor_blursk_la_SOURCES = blursk.c \
			  actor_blursk.h \
			  text.c \
			  bitmap.c \
			  paste.c 

