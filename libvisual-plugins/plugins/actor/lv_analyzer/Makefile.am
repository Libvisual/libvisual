lib_LTLIBRARIES = actor_lv_analyzer.la

libdir = $(prefix)/lib/libvisual/actor

LIBS = -L. -L$(prefix)/lib @LV_LIBS@

CFLAGS =

INCLUDES = $(all_includes) \
	-I$(top_srcdir) 

actor_lv_analyzer_la_LDFLAGS = -module -avoid-version 

actor_lv_analyzer_la_SOURCES = actor_lv_analyzer.c

