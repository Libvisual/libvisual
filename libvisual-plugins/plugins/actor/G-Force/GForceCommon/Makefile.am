
SUBDIRS = Headers

INCLUDES = -I../unix/Headers\
	-I../GForceCommon/Headers\
	-I../Common/math/Headers\
	-I../Common/GeneralTools/Headers\
	-I../Common/UI/Headers\
	-I../Common/io/Headers\
	-I../Common\
	-I../unix/libmfl\
	-I../unix/libxpce \
	-I$(top_srcdir)

DATADIR = $(LIBVISUAL_PLUGINS_DATA_DIR)/actor/actor_gforce

LIBS += $(LIBVISUAL_LIBS)
AM_CXXFLAGS = -DDATADIR=\"$(DATADIR)\" $(LIBVISUAL_CFLAGS)


noinst_LTLIBRARIES = libgforce.la
#lib_LTLIBRARIES = libgforce.la

#libdir = $(prefix)/lib/libvisual/actor

libgforce_la_SOURCES = DeltaField.cpp G-Force.cpp GF_Palette.cpp\
		       GForcePixPort.cpp ParticleGroup.cpp WaveShape.cpp 

