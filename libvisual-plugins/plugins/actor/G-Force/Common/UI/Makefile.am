
SUBDIRS = Headers

GFORCE_TOP_SRCDIR = $(top_srcdir)/plugins/actor/G-Force

INCLUDES = \
	-I$(GFORCE_TOP_SRCDIR)/unix/Headers \
	-I$(GFORCE_TOP_SRCDIR)/Common \
	-I$(GFORCE_TOP_SRCDIR)/Common/GeneralTools/Headers \
	-I$(GFORCE_TOP_SRCDIR)/Common/UI/Headers \
	-I$(GFORCE_TOP_SRCDIR)/Common/math/Headers \
	-I$(GFORCE_TOP_SRCDIR)/Common/io/Headers \
	-I$(GFORCE_TOP_SRCDIR)/unix/libxpce \
	-I$(GFORCE_TOP_SRCDIR)/unix/libmfl \
	-I$(GFORCE_TOP_SRCDIR)/unix/libvisual

DATADIR = $(LIBVISUAL_PLUGINS_DATA_DIR)
AM_CXXFLAGS = -DDATADIR="\"$(DATADIR)\""

# Missing DrawXX.cpp and LineXX.cpp
ALLSOURCES = EgOSUtils.cpp PixPort.cpp ScreenDevice.cpp

noinst_LTLIBRARIES = libUI.la

libUI_la_SOURCES = $(ALLSOURCES)

EXTRA_DIST = DrawXX.cpp LineXX.cpp

