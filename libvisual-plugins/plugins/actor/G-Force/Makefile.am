SUBDIRS = Common GForceCommon unix GForceColorMaps \
	  GForceDeltaFields GForceParticles GForceWaveShapes \
	  NotWorkingWaveShapes docs

lib_LTLIBRARIES = $(GFORCE)

EXTRA_LTLIBRARIES = actor_gforce.la

libdir = @LIBVISUAL_PLUGINS_BASE_DIR@/actor

LIBS = -L. -L$(prefix)/lib @LIBVISUAL_LIBS@

AM_CFLAGS = @LIBVISUAL_CFLAGS@ -DUNIX_X -D_REENTRANT

INCLUDES = $(all_includes) -I$(top_srcdir)

actor_gforce_la_LDFLAGS = -rpath $(libdir) -module -avoid-version

actor_gforce_la_SOURCES = null.cpp

actor_gforce_la_LIBADD = \
	GForceCommon/libgforce.la \
	Common/io/libio.la \
	Common/math/libmath.la \
	Common/UI/libUI.la \
	Common/GeneralTools/libgt.la \
	unix/libmfl/libmfl.la \
	unix/libvisual/libactor_gforce.la

install-data-local:
	test -d "$(DESTDIR)$(pkgdatadir)" || mkdirhier "$(DESTDIR)$(pkgdatadir)"
	cp -r deffont "GForceColorMaps" "GForceDeltaFields" "GForceParticles" "GForceWaveShapes" "$(DESTDIR)$(pkgdatadir)"

uninstall-local:
	rm -rf "$(DESTDIR)$(pkgdatadir)"

EXTRA_DIST = deffont

