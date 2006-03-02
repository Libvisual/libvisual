## Process this file with automake to generate a Makefile.in

actor_plugin_LTLIBRARIES = actor_gforce.la

GFORCE_TOP_SRCDIR = $(top_srcdir)/plugins/actor/G-Force

LIBS += $(LIBVISUAL_LIBS)

AM_CXXFLAGS = $(LIBVISUAL_CFLAGS)

INCLUDES = \
	-I$(GFORCE_TOP_SRCDIR)/GForceCommon/Headers \
	-I$(GFORCE_TOP_SRCDIR)/Common/UI/Headers \
	-I$(GFORCE_TOP_SRCDIR)/Common \
	-I$(GFORCE_TOP_SRCDIR)/Common/GeneralTools/Headers \
	-I$(GFORCE_TOP_SRCDIR)/Common/math/Headers \
	-I$(GFORCE_TOP_SRCDIR)/Common/io/Headers \
	-I$(GFORCE_TOP_SRCDIR)/unix/Headers \
	-I$(GFORCE_TOP_SRCDIR)/unix/libmfl \
	-I$(top_srcdir)

actor_gforce_la_LDFLAGS = -module -avoid-version

actor_gforce_la_SOURCES = actor_gforce.cpp
