
SUBDIRS = Headers

INCLUDES = -I$(top_srcdir)/unix/Headers\
	-I$(top_srcdir)/GForceCommon/Headers\
	-I$(top_srcdir)/Common/math/Headers\
	-I$(top_srcdir)/Common/GeneralTools/Headers\
	-I$(top_srcdir)/Common/UI/Headers\
	-I$(top_srcdir)/Common/io/Headers\
	-I$(top_srcdir)/Common\
	-I$(top_srcdir)/unix/libmfl\
	-I$(top_srcdir)/unix/libxpce

AM_CXXFLAGS = -DDATADIR="\"$(pkgdatadir)\""

noinst_LTLIBRARIES = libgforce.la
#lib_LTLIBRARIES = libgforce.la

#libdir = $(prefix)/lib/libvisual/actor

libgforce_la_SOURCES = DeltaField.cpp G-Force.cpp GF_Palette.cpp\
		       GForcePixPort.cpp ParticleGroup.cpp WaveShape.cpp 

