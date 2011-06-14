## Process this file with automake to generate a Makefile.in

noinst_LTLIBRARIES = libactor_gforce.la

GFORCE_TOP_SRCDIR = $(top_srcdir)/plugins/actor/G-Force

libactor_gforce_la_LIBADD = $(LIBVISUAL_LIBS)

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

libactor_gforce_la_LDFLAGS = -module -avoid-version

libactor_gforce_la_SOURCES = actor_gforce.cpp
