## Process this file with automake to generate a Makefile.in

lib_LTLIBRARIES = $(DANCINGPARTICLES)

EXTRA_LTLIBRARIES = actor_dancingparticles.la

libdir = @LIBVISUAL_PLUGINS_BASE_DIR@/actor

LIBS += -L. -L$(prefix)/lib -lGL -lGLU @LIBVISUAL_LIBS@

AM_CXXFLAGS = @LIBVISUAL_CFLAGS@

INCLUDES = $(all_includes) -I$(top_srcdir) 

actor_dancingparticles_la_LDFLAGS = -rpath $(libdir) -module -avoid-version 

actor_dancingparticles_la_SOURCES = actor_dancingparticles.cpp \
				    actor_dancingparticles.h \
				    epic.cpp \
				    etoile.cpp \
				    gl.cpp \
				    parameters.cpp \
				    signal.cpp \
				    t1font.cpp \
				    etoile.h \
				    fastmath.h \
				    floatpoint.h \
				    gl.h \
				    matrix.h \
				    t1font.h



