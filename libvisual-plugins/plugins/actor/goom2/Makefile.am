lib_LTLIBRARIES = actor_goom2.la

libdir = $(prefix)/lib/libvisual/actor

LIBS = -L. -L$(prefix)/lib @LIBGOOM2_LIBS@ @LV_LIBS@

CFLAGS = @LIBGOOM2_CFLAGS@

INCLUDES = $(all_includes) \
	-I$(top_srcdir)

LDFLAGS =

actor_goom2_la_LDFLAGS = -module -avoid-version

actor_goom2_la_SOURCES = actor_goom2.c

