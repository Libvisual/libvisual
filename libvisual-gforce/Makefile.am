SUBDIRS = Common GForceCommon unix GForceColorMaps \
	  GForceDeltaFields GForceParticles GForceWaveShapes

lib_LTLIBRARIES = actor_gforce.la

libdir = $(prefix)/lib/libvisual/actor

LIBS = -L. -L$(prefix)/lib @LIBVISUAL_LIBS@

AM_CFLAGS = -Wall -rdynamic @LIBVISUAL_CFLAGS@ -DJAHETWORDTGEDAANHOOOOOR -O3

INCLUDES = $(all_includes) -I$(top_srcdir)

actor_gforce_la_LDFLAGS = -module -avoid-version

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
	cp -r $(top_srcdir)/deffont "GForceColorMaps" "GForceDeltaFields" "GForceParticles" "GForceWaveShapes" "$(DESTDIR)$(pkgdatadir)"

uninstall-local:
	rm -rf "$(DESTDIR)$(pkgdatadir)"
