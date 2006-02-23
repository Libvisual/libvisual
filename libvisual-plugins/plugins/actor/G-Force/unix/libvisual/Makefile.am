GFORCE_TOP_SRCDIR = $(top_srcdir)/plugins/actor/G-Force

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

ALLSOURCES = actor_gforce.cpp

LIBS += $(LIBVISUAL_LIBS)
AM_CXXFLAGS = $(LIBVISUAL_CFLAGS)

noinst_LTLIBRARIES = libactor_gforce.la

libactor_gforce_la_SOURCES = $(ALLSOURCES)
