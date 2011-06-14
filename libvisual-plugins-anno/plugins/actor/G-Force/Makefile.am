SUBDIRS = Common GForceCommon unix GForceColorMaps \
	  GForceDeltaFields GForceParticles GForceWaveShapes \
	  NotWorkingWaveShapes docs

actor_plugin_LTLIBRARIES = actor_gforce.la

INCLUDES = $(all_includes) -I$(top_srcdir)

actor_gforce_la_LDFLAGS = -module -avoid-version

actor_gforce_la_SOURCES = null.cpp

actor_gforce_la_LIBADD = $(LIBVISUAL_LIBS) \
	GForceCommon/libgforce.la \
	Common/io/libio.la \
	Common/math/libmath.la \
	Common/UI/libUI.la \
	Common/GeneralTools/libgt.la \
	unix/libmfl/libmfl.la \
	unix/libvisual/libactor_gforce.la

EXTRA_DIST = deffont

