
SUBDIRS = Headers

INCLUDES = -I$(top_srcdir)/unix/Headers\
	-I$(top_srcdir)/Common\
	-I$(top_srcdir)/Common/GeneralTools/Headers\
	-I$(top_srcdir)/Common/UI/Headers\
	-I$(top_srcdir)/Common/math/Headers\
	-I$(top_srcdir)/Common/io/Headers\
	-I$(top_srcdir)/unix/libxpce\
	-I$(top_srcdir)/unix/libmfl\
	-I$(top_srcdir)/unix/libvisual

AM_CXXFLAGS = -DDATADIR="\"$(pkgdatadir)\""

# Are missing DrawXX.cpp and LineXX.cpp
ALLSOURCES = EgOSUtils.cpp PixPort.cpp ScreenDevice.cpp

noinst_LTLIBRARIES = libUI.la

libUI_la_SOURCES = $(ALLSOURCES)

EXTRA_DIST = DrawXX.cpp LineXX.cpp

