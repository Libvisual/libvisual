
SUBDIRS = Headers

GFORCE_TOP_SRCDIR = $(top_srcdir)/plugins/actor/G-Force

INCLUDES = \
	-I$(GFORCE_TOP_SRCDIR)/unix/Headers \
	-I$(GFORCE_TOP_SRCDIR)/GForceCommon/Headers \
	-I$(GFORCE_TOP_SRCDIR)/Common/math/Headers \
	-I$(GFORCE_TOP_SRCDIR)/Common/GeneralTools/Headers \
	-I$(GFORCE_TOP_SRCDIR)/Common/UI/Headers \
	-I$(GFORCE_TOP_SRCDIR)/Common/io/Headers \
	-I$(GFORCE_TOP_SRCDIR)/Common \
	-I$(GFORCE_TOP_SRCDIR)/unix/libmfl \
	-I$(GFORCE_TOP_SRCDIR)/unix/libxpce \
	-I$(top_srcdir)

DATADIR = $(LIBVISUAL_PLUGINS_DATA_DIR)/actor/actor_gforce

LIBS = $(LIBVISUAL_LIBS)
AM_CXXFLAGS = -DDATADIR=\"$(DATADIR)\" $(LIBVISUAL_CFLAGS)


noinst_LTLIBRARIES = libgforce.la

libgforce_la_SOURCES = DeltaField.cpp G-Force.cpp GF_Palette.cpp\
		       GForcePixPort.cpp ParticleGroup.cpp WaveShape.cpp 

