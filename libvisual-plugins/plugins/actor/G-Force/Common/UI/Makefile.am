
SUBDIRS = Headers

INCLUDES = -I../../unix/Headers\
	-I../../Common\
	-I../../Common/GeneralTools/Headers\
	-I../../Common/UI/Headers\
	-I../../Common/math/Headers\
	-I../../Common/io/Headers\
	-I../../unix/libxpce\
	-I../../unix/libmfl\
	-I../../unix/libvisual

DATADIR = $(datadir)/libvisual/actor/actor_gforce
AM_CXXFLAGS = -DDATADIR="\"$(pkgdatadir)\""

# Are missing DrawXX.cpp and LineXX.cpp
ALLSOURCES = EgOSUtils.cpp PixPort.cpp ScreenDevice.cpp

noinst_LTLIBRARIES = libUI.la

libUI_la_SOURCES = $(ALLSOURCES)

EXTRA_DIST = DrawXX.cpp LineXX.cpp

