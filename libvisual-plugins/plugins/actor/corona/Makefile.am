## Process this file with automake to generate a Makefile.in

actor_plugin_LTLIBRARIES = actor_corona.la

LIBS += $(LIBVISUAL_LIBS)

AM_CXXFLAGS = $(LIBVISUAL_CFLAGS)

INCLUDES = $(all_includes) -I$(top_srcdir)

actor_corona_la_LDFLAGS = -module -avoid-version

actor_corona_la_SOURCES = actor_corona.cpp \
			  autopal.h \
			  corona.cpp \
			  corona.h \
			  corona_types.h \
			  palette.cpp \
			  palette.h

