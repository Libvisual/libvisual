INCLUDES=-I../unix/Headers -IHeaders -I../Common/math/Headers -I../Common/GeneralTools/Headers -I../Common/UI/Headers -I../Common/io/Headers -I../Common -I../unix/libmfl -I../unix/libxpce

AM_CXXFLAGS = -DDATADIR="\"$(pkgdatadir)\""

ALLSOURCES = DeltaField.cpp G-Force.cpp GF_Palette.cpp GForcePixPort.cpp ParticleGroup.cpp WaveShape.cpp 

noinst_LTLIBRARIES = libgforce.la

libgforce_la_SOURCES = $(ALLSOURCES)

