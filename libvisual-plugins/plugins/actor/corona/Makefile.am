## Process this file with automake to generate a Makefile.in

lib_LTLIBRARIES = $(corona)

EXTRA_LTLIBRARIES = actor_corona.la

libdir = @LIBVISUAL_PLUGINS_BASE_DIR@/actor

LIBS += -L. -L$(prefix)/lib @LIBVISUAL_LIBS@

AM_CXXFLAGS = @LIBVISUAL_CFLAGS@

INCLUDES = $(all_includes) -I$(top_srcdir)

actor_corona_la_LDFLAGS = -rpath $(libdir) -module -avoid-version

actor_corona_la_SOURCES = actor_corona.cpp \
			  autopal.h \
			  corona.cpp \
			  corona.h \
			  corona_types.h \
			  palette.cpp \
			  palette.h

