lib_LTLIBRARIES = actor_infinite.la

libdir = $(prefix)/lib/libvisual/actor

LIBS = -L. -L$(prefix)/lib @LV_LIBS@

CFLAGS =

INCLUDES = $(all_includes) \
	-I$(top_srcdir) 

actor_infinite_la_LDFLAGS = -module -avoid-version 

actor_infinite_la_SOURCES = compute.c display.c file.c lv_infinite.c renderer.c

