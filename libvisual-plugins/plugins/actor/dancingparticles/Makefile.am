## Process this file with automake to generate a Makefile.in

actor_plugin_LTLIBRARIES = actor_dancingparticles.la

actor_dancingparticles_la_LDFLAGS = -module -avoid-version 

actor_dancingparticles_la_CXXFLAGS = \
	-I$(top_srcdir) $(LIBVISUAL_CFLAGS) $(GL_CFLAGS) $(GLU_CFLAGS)

actor_dancingparticles_la_LIBADD = \
	$(LIBVISUAL_LIBS) $(GL_LIBS) $(GLU_LIBS)

actor_dancingparticles_la_SOURCES = \
	actor_dancingparticles.cpp \
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



